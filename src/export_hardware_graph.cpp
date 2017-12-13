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
    std::cout << myName << " <yaml-file-in> <yaml-file-out>\n\n";
    std::cout << "Options:\n";
    std::cout << "--help\t" << "Show usage\n";
    std::cout << "\nExample:\n";
    std::cout << myName << " native.yml name-of-model-to-export.yml\n";
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

    if ((argc - optind) < 2)
    {
        usage(argv[0]);
        return 1;
    }

    // Set vars
    std::string fileNameIn(argv[optind]);
    std::string fileNameOut(argv[optind+1]);

    // Load file
    Hypergraph* hg = YAML::LoadFile(fileNameIn).as<Hypergraph*>();
    Simple::Computation sc(*hg);

    // Call domain specific export
    std::size_t pos(fileNameOut.rfind("."));
    std::string name(fileNameOut.substr(0,pos));
    Hyperedges candidateIds(sc.networkClasses(name));
    std::string result(sc.domainSpecificExport(*candidateIds.begin()));

    // Store export
    std::ofstream fout;
    fout.open(fileNameOut);
    if(!fout.good()) {
        std::cout << "WRITE FAILED\n";
    }
    fout << result;
    fout.close();

    return 0;
}
