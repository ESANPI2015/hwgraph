#include "ComputationalGraph.hpp"

namespace Hardware {
namespace Computational {

Graph::Graph(const std::string& label)
: Set(label)
{
    Set *devices = Set::create("Devices");
    Set *interfaces = Set::create("Interfaces");
    Set *busses = Set::create("Busses");
    contains(devices);
    contains(interfaces);
    contains(busses);
    // Register Ids for easier access
    _devId = devices->id();
    _ifId = interfaces->id();
    _busId = busses->id();
}

Set* Graph::devices()
{
    return static_cast<Set*>(_created[_devId]);
}

Set* Graph::interfaces()
{
    return static_cast<Set*>(_created[_ifId]);
}

Set* Graph::busses()
{
    return static_cast<Set*>(_created[_busId]);
}

Set* Graph::createDevice(const std::string& name)
{
    Set *newbie = (Set::create(name));
    devices()->contains(newbie);
    return newbie;
}

Set* Graph::createInterface(const std::string& name)
{
    Set *newbie = (Set::create(name));
    interfaces()->contains(newbie);
    return newbie;
}

Set* Graph::createBus(const std::string& name)
{
    Set *newbie = (Set::create(name));
    busses()->contains(newbie);
    return newbie;
}

bool Graph::has(Set* device, Set* interface)
{
    // At first, we have to add device & interface to the corresponding sets
    devices()->contains(device);
    interfaces()->contains(interface);

    // TODO: Then we have to check if the device already HAS this interface, right? Or is it ok to have redundant info?
    
    // Finally we create a new relation (1-to-1)
    Relation *has = (Relation::create("has"));
    has->from(device);
    has->to(interface);
    return true;
}

bool Graph::has(Set* device, Set::Sets interfaces)
{
    // 1-N relation based on 2-hyperedges (1-1 relations)
    for (auto interfaceIt : interfaces)
    {
        auto interface = interfaceIt.second;
        has(device, interface);
    }
    return true;
}

bool Graph::has(Set::Sets devices, Set* interface)
{
    // N-1 relation based on 2-hyperedges
    for (auto deviceIt : devices)
    {
        auto device = deviceIt.second;
        has(device, interface);
    }
    return true;
}

bool Graph::has(Set::Sets devices, Set::Sets interfaces)
{
    // N-M relation based on N * (1-M) relations
    for (auto deviceIt : devices)
    {
        auto device = deviceIt.second;
        has(device, interfaces);
    }
    return true;
}

bool Graph::connects(Set* bus, Set* interface)
{
    // At first, we have to add device & interface to the corresponding sets
    busses()->contains(bus);
    interfaces()->contains(interface);

    // TODO: Then we have to check if a bus already CONNECTS this interface, right? Or is it ok to have redundant info?
    
    // Finally we create a new relation (1-to-1)
    Relation *connects = (Relation::create("connects"));
    connects->from(bus);
    connects->to(interface);
    return true;
}

bool Graph::connects(Set* bus, Set::Sets interfaces)
{
    // 1-N relation based on 2-hyperedges (1-1 relations)
    for (auto interfaceIt : interfaces)
    {
        auto interface = interfaceIt.second;
        connects(bus, interface);
    }
    return true;
}

bool Graph::connects(Set::Sets busses, Set* interface)
{
    // N-1 relation based on 2-hyperedges (1-1 relations)
    for (auto busIt : busses)
    {
        auto bus = busIt.second;
        connects(bus, interface);
    }
    return true;
}

bool Graph::connects(Set::Sets busses, Set::Sets interfaces)
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
