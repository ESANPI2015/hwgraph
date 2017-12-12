#include "ComputationDomain.hpp"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <sstream>

namespace Simple {

void Computation::setupMetaModel()
{
    createProcessor("Simple::Computation::Processor::Conventional", "CONVENTIONAL");
    createProcessor("Simple::Computation::Processor::Fpga", "FPGA");
    createInterface("Simple::Computation::Interface::NDLComLVDS", "NDLCOM");
    createInterface("Simple::Computation::Interface::Input", "SOURCE");
    createInterface("Simple::Computation::Interface::Output", "SINK");
    Hyperedges busIfs;
    busIfs = unite(busIfs, instatiateFrom(Hyperedges{"Simple::Computation::Interface::NDLCom"}, "A"));
    busIfs = unite(busIfs, instatiateFrom(Hyperedges{"Simple::Computation::Interface::NDLCom"}, "B"));
    hasInterface(createBus("Simple::Computation::Bus::NDLComOverLVDS", "NDLCOM@LVDS"), busIfs);
    createNetwork("Simple::Computation::Subgraph", "SUBGRAPH");
}

Computation::Computation()
{
    setupMetaModel();
}

Computation::Computation(const Hypergraph& base)
: Hardware::Computational::Network(base)
{
    setupMetaModel();
}

Computation::~Computation()
{
}

std::string Computation::domainSpecificExport(const UniqueId& uid)
{
    return std::string();
}

bool Computation::domainSpecificImport(const std::string& serialized)
{
    std::map< std::string,Hyperedges > old2new;
    std::map< std::string, std::map< std::string, Hyperedges> > old2newPort;
    YAML::Node spec = YAML::Load(serialized);
    if (!spec["name"].IsDefined())
        return false;
    const std::string& name(spec["name"].as<std::string>());
    if (!spec["nodes"].IsDefined())
        return false;
    const YAML::Node& nodesYAML(spec["nodes"]); // FIXME: Better naming: processors, devices
    for (YAML::Node::const_iterator nit = nodesYAML.begin(); nit != nodesYAML.end(); ++nit)
    {
        const YAML::Node& nodeYAML(*nit);
        const std::string& nodeId(nodeYAML["id"].as<std::string>()); // FIXME: We might have to PRESERVE this id ... it could be an NDLCom DeviceId!!!
        const std::string& nodeName(nodeYAML["name"].as<std::string>());
        const std::string& nodeType(nodeYAML["type"].as<std::string>());
        // Instantiate processor
        Hyperedges uid(instantiateComponent(processorClasses(type), name));
        old2new[id] = uid;

        // Handle interfaces
        const YAML::Node& portsYAML(nodeYAML["ports"]); // FIXME: better naming: interfaces
        if (portsYAML.IsDefined())
        {
            Hyperedges interfaceUids;
            for (YAML::Node::const_iterator pit = portsYAML.begin(); pit != portsYAML.end(); ++pit)
            {
                const YAML::Node& portYAML(*pit);
                const std::string& portId(portYAML["id"].as<std::string>());
                const std::string& portName(portYAML["name"].as<std::string>());
                const std::string& portType(portYAML["type"].as<std::string>());
                // Instantiate interface
                Hyperedges interfaceUid(instatiateFrom(interfaceClasses(portType), portName));
                old2newPort[id][portId] = interfaceUid;
            }
            hasInterface(uid, interfaceUids);
        }
    }
    const YAML::Node& edgesYAML(spec["edges"]);
    if (edgesYAML.IsDefined())
    {
        for (YAML::Node::const_iterator eit = edgesYAML.begin(); eit != edgesYAML.end(); ++eit)
        {
            const YAML::Node& edgeYAML(*eit);
            const std::string& edgeId(edgeYAML["id"].as<std::string>());
            const std::string& edgeName(edgeYAML["name"].as<std::string>());
            const YAML::Node& edgeNodesYAML(edgeYAML["nodes"]); // FIXME: better naming: endpoints
            if (!edgeNodesYAML.IsDefined())
                return false;
            for (YAML::Node::const_iterator enit = edgeNodesYAML.begin(); enit != edgeNodesYAML.end(); ++enit)
            {
                const YAML::Node& edgeNodeYAML(*enit);
                const std::string& nodeId(edgeNodeYAML["id"].as<std::string>());
                const std::string& portId(edgeNodeYAML["port"].as<std::string>()); // FIXME: better naming: nodeId, portId
            }
        }
    }
}

}
