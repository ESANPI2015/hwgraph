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

    std::cout << "> Create 2nd device\n";
    auto thirdId = hwgraph.createDevice("Second device");
    std::cout << "Third id: " << thirdId << "\n";

    std::cout << "> Create 2nd interface\n";
    auto fourthId = hwgraph.createInterface("Second interface");
    std::cout << "Fourth id: " << fourthId << "\n";

    std::cout << "> Assign interface to device\n";
    hwgraph.has(thirdId, fourthId);

    std::cout << "> Connect the two interfaces\n";
    auto busId = hwgraph.createBus("MyBus");
    hwgraph.connects(busId, secondId);
    hwgraph.connects(busId, fourthId);

    std::cout << "> Query devices\n";
    auto devsId = hwgraph.devices();
    for (auto setId : devsId)
    {
        std::cout << setId << " " << hwgraph.get(setId)->label() << std::endl;
    }

    std::cout << "> Query interfaces\n";
    auto ifsId = hwgraph.interfaces();
    for (auto setId : ifsId)
    {
        std::cout << setId << " " << hwgraph.get(setId)->label() << std::endl;
    }

    std::cout << "> Query busses\n";
    auto bussesId = hwgraph.busses();
    for (auto setId : bussesId)
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
