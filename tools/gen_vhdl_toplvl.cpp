#include "ComputationalGraph.hpp"
#include "HyperedgeYAML.hpp"

#include <iostream>
#include <cassert>

int main(void)
{
    // TODO:
    // * Import hardware spec
    // * Search for all devices
    // * Create a toplvl vhdl skeleton for each device

    // Test case:
    auto hypergraph = YAML::LoadFile("hwgraph.yml").as<Hypergraph*>();
    auto hwgraph = static_cast<Hardware::Computational::Graph*>(hypergraph);

    // Find specific device(s) in the set of all devices
    auto deviceIds = hwgraph->members(hwgraph->devices());

    // For each of these devices
    for (auto deviceId : deviceIds)
    {
        auto device = hwgraph->get(deviceId);

        // Produce VHDL skeleton
        std::cout << "-- CHWG to VHDL TOPLVL Generator --\n";
        std::cout << "-- libraries here --\n";
        std::cout << "-- packages here --\n";
        std::cout << "entity " << device->label() << " is\n";
        std::cout << "port(\n";
        std::cout << "-- interfaces here --\n";
        // TODO: Instead of set intersection we could just create TWO RELATIONS and use hyperedge intersection?
        // Get all things related to the device by a "has" relation
        auto aggregatesId = hwgraph->create(hwgraph->directlyRelatedTo(deviceId, Relation::hasLabel), "Aggregates");
        // Get all interfaces
        auto interfacesId = hwgraph->interfaces();
        // The intersection between the Set of all things the device owns && all interfaces of 
        // TODO: Set intersection not yet implemented. Use aggregates directly for now!
        //auto myinterfacesId = hwgraph->intersect(aggregatesId, interfacesId);
        auto myinterfacesId = aggregatesId;
        for (auto interfaceId : hwgraph->members(myinterfacesId))
        {
            auto interface = hwgraph->get(interfaceId);
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
    delete hwgraph;
}
