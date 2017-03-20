#include "ComputationalGraph.hpp"

namespace Hardware {
namespace Computational {

Graph::Graph(const std::string& label)
: Hyperedge(label)
{
    Hyperedge *devices = Hyperedge::create("Devices");
    Hyperedge *interfaces = Hyperedge::create("Interfaces");
    Hyperedge *busses = Hyperedge::create("Busses");
    contains(devices);
    contains(interfaces);
    contains(busses);
    // Register Ids for easier access
    _devId = devices->id();
    _ifId = interfaces->id();
    _busId = busses->id();
}

Hyperedge* Graph::devices()
{
    // TODO: When we have a find by id function, we should use that one (because labels are not guaranteed to be unique)
    Hyperedge *devs = members("Devices").begin()->second;
    return devs->id() == _devId ? devs : NULL;
}

Hyperedge* Graph::interfaces()
{
    Hyperedge *ifs = members("Interfaces").begin()->second;
    return ifs->id() == _ifId ? ifs : NULL;
}

Hyperedge* Graph::busses()
{
    Hyperedge *buss = members("Busses").begin()->second;
    return buss->id() == _busId ? buss : NULL;
}

Hyperedge* Graph::createDevice(const std::string& name)
{
    Hyperedge *newbie = (Hyperedge::create(name));
    devices()->contains(newbie);
    return newbie;
}

Hyperedge* Graph::createInterface(const std::string& name)
{
    Hyperedge *newbie = (Hyperedge::create(name));
    interfaces()->contains(newbie);
    return newbie;
}

Hyperedge* Graph::createBus(const std::string& name)
{
    Hyperedge *newbie = (Hyperedge::create(name));
    busses()->contains(newbie);
    return newbie;
}

bool Graph::has(Hyperedge* device, Hyperedge* interface)
{
    // At first, we have to add device & interface to the corresponding sets
    devices()->contains(device);
    interfaces()->contains(interface);

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
    busses()->contains(bus);
    interfaces()->contains(interface);

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
