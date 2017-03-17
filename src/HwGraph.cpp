#include "HwGraph.hpp"

namespace Hardware {
namespace Computational {

Graph::Graph(const std::string& label)
: Hyperedge(label)
{
    Hyperedge *devices = Hyperedge::create("Devices");
    Hyperedge *interfaces = Hyperedge::create("Interfaces");
    Hyperedge *busses = Hyperedge::create("Busses");
    // TODO: Register the new hyperedges for faster access?
    // Or provide convenient private member function
    contains(devices);
    contains(interfaces);
    contains(busses);
}

Hyperedge* Graph::createDevice(const std::string& name)
{
    Hyperedge *newbie = (Hyperedge::create(name));
    Hyperedge *devices = (members("Devices").begin()->second);
    devices->contains(newbie);
    return newbie;
}

Hyperedge* Graph::createInterface(const std::string& name)
{
    Hyperedge *newbie = (Hyperedge::create(name));
    Hyperedge *interfaces = (members("Interfaces").begin()->second);
    interfaces->contains(newbie);
    return newbie;
}

Hyperedge* Graph::createBus(const std::string& name)
{
    Hyperedge *newbie = (Hyperedge::create(name));
    Hyperedge *busses = (members("Busses").begin()->second);
    busses->contains(newbie);
    return newbie;
}

bool Graph::has(Hyperedge* device, Hyperedge* interface)
{
    // At first, we have to add device & interface to the corresponding sets
    Hyperedge *devices = (members("Devices").begin()->second);
    Hyperedge *interfaces = (members("Interfaces").begin()->second);
    devices->contains(device);
    interfaces->contains(interface);

    // TODO: Then we have to check if the device already HAS this interface, right? Or is it ok to have redundant info?
    
    // Finally we create a new relation (1-to-1)
    Hyperedge *has = (Hyperedge::create("has"));
    has->contains(device);
    has->contains(interface);
    contains(has);
    return true;
}

bool Graph::has(Hyperedge* device, Hyperedge::Hyperedges interfaces)
{
    // 1-N relation based on 2-hyperedges (1-1 relations)
    for (auto interfaceIt : interfaces)
    {
        auto interface = interfaceIt.second;
        has(device, interface);
    }
    return true;
}

bool Graph::has(Hyperedge::Hyperedges devices, Hyperedge* interface)
{
    // N-1 relation based on 2-hyperedges
    for (auto deviceIt : devices)
    {
        auto device = deviceIt.second;
        has(device, interface);
    }
    return true;
}

bool Graph::has(Hyperedge::Hyperedges devices, Hyperedge::Hyperedges interfaces)
{
    // N-M relation based on N * (1-M) relations
    for (auto deviceIt : devices)
    {
        auto device = deviceIt.second;
        has(device, interfaces);
    }
    return true;
}

bool Graph::connects(Hyperedge* bus, Hyperedge* interface)
{
    // At first, we have to add device & interface to the corresponding sets
    Hyperedge *busses = (members("Busses").begin()->second);
    Hyperedge *interfaces = (members("Interfaces").begin()->second);
    busses->contains(bus);
    interfaces->contains(interface);

    // TODO: Then we have to check if a bus already CONNECTS this interface, right? Or is it ok to have redundant info?
    
    // Finally we create a new relation (1-to-1)
    Hyperedge *connects = (Hyperedge::create("connects"));
    connects->contains(bus);
    connects->contains(interface);
    contains(connects);
    return true;
}

bool Graph::connects(Hyperedge* bus, Hyperedge::Hyperedges interfaces)
{
    // 1-N relation based on 2-hyperedges (1-1 relations)
    for (auto interfaceIt : interfaces)
    {
        auto interface = interfaceIt.second;
        connects(bus, interface);
    }
    return true;
}

bool Graph::connects(Hyperedge::Hyperedges busses, Hyperedge* interface)
{
    // N-1 relation based on 2-hyperedges (1-1 relations)
    for (auto busIt : busses)
    {
        auto bus = busIt.second;
        connects(bus, interface);
    }
    return true;
}

bool Graph::connects(Hyperedge::Hyperedges busses, Hyperedge::Hyperedges interfaces)
{
    // N-M relation based on N * (1-M) relations
    for (auto busIt : busses)
    {
        auto bus = busIt.second;
        connects(bus, interfaces);
    }
    return true;
}

}
}
