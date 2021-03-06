#include "ComputationalGraph.hpp"
#include "HypergraphYAML.hpp"

#include <iostream>
#include <cassert>

int main(void)
{
    // * Import hardware spec
    // * Search for all devices
    // TODO:
    // * Create a toplvl vhdl skeleton for each device

    // Test case:
    Hypergraph hypergraph(YAML::LoadFile("demo.yml").as<Hypergraph>());
    Conceptgraph cgraph(hypergraph);
    CommonConceptGraph ccgraph(cgraph);
    Hardware::Computational::Graph hwgraph(ccgraph);
    // Find specific device(s) in the set of all devices
    auto deviceIds = hwgraph.deviceClasses();

    // For each of these devices
    for (auto deviceId : deviceIds)
    {
        auto device = hwgraph.get(deviceId);

        // Produce VHDL skeleton
        std::cout << "-- CHWG to VHDL TOPLVL Generator --\n";
        std::cout << "-- libraries here --\n";
        std::cout << "-- packages here --\n";
        std::cout << "entity " << device->label() << " is\n";
        std::cout << "port(\n";
        std::cout << "-- interfaces here --\n";
        // Handle interfaces
        auto myInterfaceIds = hwgraph.interfaces(deviceId);
        for (auto interfaceId : myInterfaceIds)
        {
            auto interface = hwgraph.get(interfaceId);
            std::cout << "-- Interface: " << interface->label() << std::endl;
            std::cout << "-- TODO: How do we know what pins/groups and directions we have to assign here?\n";
        }
        std::cout << ");\n";
        std::cout << "end " << device->label() << ";\n";
        std::cout << "architecture BEHAVIOURAL of " << device->label() << " is\n";
        std::cout << "-- signals here --\n";
        std::cout << "\nbegin\n";
        std::cout << "-- processes here --\n";
        std::cout << "end BEHAVIORAL;\n\n";
    }
}
