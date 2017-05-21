#include "ComputationalGraph.hpp"
#include "HyperedgeYAML.hpp"

#include <fstream>
#include <iostream>
#include <cassert>

int main(void)
{
    std::cout << "*** HW GRAPH TEST ***\n";

    Hardware::Computational::Graph hwgraph;

    std::cout << "> Create device\n";
    auto firstId = hwgraph.createDevice("First device");
    std::cout << "First id: " << firstId << "\n";

    std::cout << "> Create interface\n";
    auto secondId = hwgraph.createInterface("First interface");
    std::cout << "Second id: " << secondId << "\n";

    std::cout << "> Assign interface to device\n";
    hwgraph.has(firstId, secondId);

    std::cout << "> All sets" << std::endl;
    auto sets = hwgraph.find();
    for (auto setId : sets)
    {
        std::cout << setId << " " << hwgraph.get(setId)->label() << std::endl;
    }

    std::cout << "> Store hwgraph using YAML" << std::endl;

    YAML::Node test;
    test = static_cast<Hypergraph*>(&hwgraph);

    std::ofstream fout;
    fout.open("hwgraph.yml");
    if(fout.good()) {
        fout << test;
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();


    std::cout << "*** TEST DONE ***\n";
}
