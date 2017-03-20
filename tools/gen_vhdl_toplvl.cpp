#include "HwGraph.hpp"

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
    hwgraph.createInterface("dummy");

    std::cout << Hyperedge::serialize(&hwgraph);

    // Get the superclass of all devices
    auto superDevice = hwgraph.devices();
    // Find specific device(s)
    auto devices = superDevice->membersOf()->members("TestDevice");
    // For each of these devices
    for (auto deviceIt : devices)
    {
        auto device = deviceIt.second;
        // Produce VHDL skeleton
        std::cout << "-- CHWG to VHDL TOPLVL Generator --\n";
        std::cout << "-- libraries here --\n";
        std::cout << "-- packages here --\n";
        std::cout << "entity " << device->label() << " is\n";
        std::cout << "port(\n";
        std::cout << "-- interfaces here --\n";
        // Get all interfaces of this device
        // That means: get all "has" relations (has(x,a), has(y,b) ...)
        // For each: filter according to the given device (has(x,a).membersOf() && device)
        // Find the "has" relations which contain the device we are currently interested in.
        // This means we
        // I. get all supersets of device (which contains all has(device, *) relations and other supersets)
        // II. get all has relations (only contains has(*,*) relations)
        // III. intersect: Contains only has(device,*) relations
        // IV. get the membersOf(): Contains (has(device, *), device, *)
        // V. Intersect with all interfaces: Contains all interfaces of device
        auto query = (device->supersOf()->intersect(hwgraph.labelPartOf("has")))->membersOf()->intersect(hwgraph.interfaces()->membersOf());
        for (auto memberIt : query->members())
        {
            auto interface = memberIt.second;
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
