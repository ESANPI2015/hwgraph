#include "ComputationalGraph.hpp"
#include "HyperedgeYAML.hpp"

#include <iostream>
#include <cassert>

int main(void)
{
    Hardware::Computational::Graph* hwgraph = Hardware::Computational::Graph::create();

    // TODO:
    // * Import hardware spec
    // * Search for all devices
    // * Create a toplvl vhdl skeleton for each device

    // Test case:
    auto newEdges = YAML::load(YAML::LoadFile("test.yml"));
    // Find specific device(s) in the set of all devices
    auto devices = hwgraph->devices()->members();

    // For each of these devices
    for (auto deviceId : devices)
    {
        auto device = static_cast<Hardware::Computational::Device*>(Hyperedge::find(deviceId));
        // Produce VHDL skeleton
        std::cout << "-- CHWG to VHDL TOPLVL Generator --\n";
        std::cout << "-- libraries here --\n";
        std::cout << "-- packages here --\n";
        std::cout << "entity " << device->label() << " is\n";
        std::cout << "port(\n";
        std::cout << "-- interfaces here --\n";
        // Get all things related to the device by a "has" relation
        auto aggregates = device->aggregates();
        // Get all interfaces
        auto interfaces = hwgraph->interfaces();
        // My interfaces: Intersection of ALL interfaces with the aggregates of the device
        auto myinterfaces = aggregates->intersect(interfaces);
        for (auto interfaceId : myinterfaces->members())
        {
            auto interface = Hyperedge::find(interfaceId);
            std::cout << "-- Interface: " << interface->label() << std::endl;
            std::cout << "-- TODO: How do we know what pins/groups and directions we have to assign here?\n";
        }
        std::cout << ");\n";
        std::cout << "end " << device->label() << ";\n";
        std::cout << "architecture BEHAVIOURAL of " << device->label() << " is\n";
        std::cout << "-- signals here --\n";
        std::cout << "\nbegin\n";
        std::cout << "-- processes here --\n";
        std::cout << "end BEHAVIORAL;\n";
    }

    Hyperedge::cleanup();
}
