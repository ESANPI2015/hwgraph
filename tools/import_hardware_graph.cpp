#include "ComputationalGraph.hpp"
#include "HyperedgeYAML.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <getopt.h>

static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {0,0,0,0}
};

void usage (const char *myName)
{
    std::cout << "Usage:\n";
    std::cout << myName << " <yaml-file-in> <lang-spec-file> <yaml-file-out>\n\n";
    std::cout << "Options:\n";
    std::cout << "--help\t" << "Show usage\n";
    std::cout << "\nExample:\n";
    std::cout << myName << " hardware_graph.yml def_hardware_graph.yml imported.yml\n";
}

// This tool takes a language definition and tries to interpret a given domain specific format given that definition
int main (int argc, char **argv)
{

    // Parse command line
    int c;
    while (1)
    {
        int option_index = 0;
        c = getopt_long(argc, argv, "h", long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
            case 'h':
            case '?':
                break;
            default:
                std::cout << "W00t?!\n";
                return 1;
        }
    }

    if ((argc - optind) < 3)
    {
        usage(argv[0]);
        return 1;
    }

    // Set vars
    std::string fileNameIn(argv[optind]);
    std::string langSpecIn(argv[optind+1]);
    std::string fileNameOut(argv[optind+2]);
    Hypergraph* hypergraph = YAML::LoadFile(langSpecIn).as<Hypergraph*>();
    Conceptgraph cgraph(*hypergraph);
    CommonConceptGraph ccgraph(cgraph);
    Hardware::Computational::Graph hwgraph(ccgraph);

    YAML::Node import = YAML::LoadFile(fileNameIn);
    const YAML::Node& nodes(import["nodes"]);
    if (nodes.IsDefined())
    {
        std::map<UniqueId, Hyperedges> old2new;
        for (auto it = nodes.begin(); it != nodes.end(); it++)
        {
            const YAML::Node node(*it);
            UniqueId id = node["id"].as<UniqueId>();
            std::string label = node["label"].as<std::string>();
            std::string type = node["type"].as<std::string>();
            // First: Find the superclass
            Hyperedges super = hwgraph.find(type);
            if (!super.size())
            {
                std::cout << "Missing super class " << type << " for concept " << label << "\n";
                continue;
            }
            // Second:
            // system_modelling only knows instances but no subclasses
            // Unfortunately, we now have to fix here some inconsistencies:
            // PortConnections have to be INSTANTIATED
            Hyperedges sub;
            if (type == "system_modelling::task_graph::PortConnection")
            {
                sub = hwgraph.instantiateFrom(super, label);
            } else {
                sub = hwgraph.create(label);
            }
            hwgraph.isA(sub, super);
            old2new[id] = sub;
        }
        const YAML::Node& edges(import["edges"]);
        for (auto it = edges.begin(); it != edges.end(); it++)
        {
            const YAML::Node edge(*it);
            UniqueId id = edge["id"].as<UniqueId>();
            std::string label = edge["label"].as<std::string>();
            std::string type = edge["type"].as<std::string>();
            UniqueId fromId = edge["fromId"].as<UniqueId>();
            UniqueId toId = edge["toId"].as<UniqueId>();
            // First: Find the superclass
            Hyperedges super = hwgraph.relations(type);
            if (!super.size())
            {
                std::cout << "Missing super class " << type << " for relation " << label << "\n";
                continue;
            }
            // Second: Instantiate from that superclass
            Hyperedges sub = hwgraph.relateFrom(old2new[fromId], old2new[toId], super);
            if (!sub.size())
            {
                std::cout << "Could not relate " << old2new[fromId] << " to " << old2new[toId] << "\n";
                continue;
            }
            Hyperedges edgeId = subtract(subtract(sub, old2new[fromId]), old2new[toId]);
            hwgraph.get(*edgeId.begin())->updateLabel(label);
        }
    }

    // Store modified graph
    std::ofstream fout;
    fout.open(fileNameOut);
    if(!fout.good()) {
        std::cout << "FAILED\n";
    }
    YAML::Node test;
    test = static_cast<Hypergraph*>(&hwgraph);
    fout << test;
    fout.close();

    return 0;
}
