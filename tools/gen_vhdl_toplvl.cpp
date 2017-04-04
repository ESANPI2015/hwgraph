#include "ComputationalGraph.hpp"

#include <iostream>
#include <cassert>

int main(void)
{
    Hardware::Computational::Graph hwgraph;

    // TODO:
    // * Import hardware spec
    // * Search for all devices
    // * Create a toplvl vhdl skeleton for each device

    auto dev = hwgraph.createDevice("TestDevice");
    hwgraph.has(dev, hwgraph.createInterface("Ethernet"));
    hwgraph.has(dev, hwgraph.createInterface("RS232"));
    hwgraph.has(dev, hwgraph.createInterface("LVDS"));
    hwgraph.has(hwgraph.createDevice("dummy"),hwgraph.createInterface("dummy"));

    //std::cout << Hyperedge::serialize(&hwgraph);

    // Find specific device(s) in the set of all devices
    auto devices = hwgraph.devices()->members("TestDevice");
    // For each of these devices
    for (auto deviceIt : devices)
    {
        auto device = static_cast<Hardware::Computational::Device*>(deviceIt.second);
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
        auto interfaces = hwgraph.interfaces();
        // My interfaces: Intersection of ALL interfaces with the aggregates of the device
        auto myinterfaces = aggregates->intersect(interfaces);
        for (auto interfaceIt : myinterfaces->members())
        {
            auto interface = interfaceIt.second;
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
