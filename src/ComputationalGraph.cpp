#include "ComputationalGraph.hpp"

namespace Hardware {
namespace Computational {

Graph::Graph(const std::string& label)
: Set(label)
{
    // NOTE: These are TYPES/SUPERCLASSES
    Set *device = Set::create("Device");
    Set *interface = Set::create("Interface");
    Set *bus = Set::create("Bus");
    device->memberOf(this);
    interface->memberOf(this);
    bus->memberOf(this);
    // Register Ids for easier access
    _devId = device->id();
    _ifId = interface->id();
    _busId = bus->id();
}

Set* Graph::devices()
{
    return device()->subclasses();
}

Set* Graph::interfaces()
{
    return interface()->subclasses();
}

Set* Graph::busses()
{
    return bus()->subclasses();
}

Set* Graph::device()
{
    return static_cast<Set*>(_created[_devId]);
}

Set* Graph::interface()
{
    return static_cast<Set*>(_created[_ifId]);
}

Set* Graph::bus()
{
    return static_cast<Set*>(_created[_busId]);
}

Set* Graph::createDevice(const std::string& name)
{
    Set *newbie = (Set::create(name));
    newbie->isA(device());
    return newbie;
}

Set* Graph::createInterface(const std::string& name)
{
    Set *newbie = (Set::create(name));
    newbie->isA(interface());
    return newbie;
}

Set* Graph::createBus(const std::string& name)
{
    Set *newbie = (Set::create(name));
    newbie->isA(bus());
    return newbie;
}

bool Graph::has(Set* device, Set* interface)
{
    bool result = true;

    // At first, we have to add device & interface to the corresponding sets
    result &= device->isA(this->device());
    result &= interface->isA(this->interface());

    // Find a has relation in device
    auto edges = device->pointingTo("has");
    Relation *has = NULL;
    if (edges.size())
    {
        has = static_cast< Relation *>(edges.begin()->second);
        result &= has->to(interface);
    } else {
        // Finally we create a new relation (1-to-1)
        has = (Relation::create("has"));
        result &= has->from(device);
        result &= has->to(interface);
    }

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
    bool result = true;

    // At first, we have to add device & interface to the corresponding sets
    result &= bus->isA(this->bus());
    result &= interface->isA(this->interface());

    // Find a connects relation in device
    auto edges = bus->pointingTo("connects");
    Relation *connects = NULL;
    if (edges.size())
    {
        connects = static_cast< Relation *>(edges.begin()->second);
        result &= connects->to(interface);
    } else {
        // Finally we create a new relation (1-to-1)
        connects = (Relation::create("connects"));
        result &= connects->from(bus);
        result &= connects->to(interface);
    }

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
