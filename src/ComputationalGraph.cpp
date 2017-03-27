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
    Hyperedge *query;
    // This query gives all subtypes of this
    query = device()->traversal(
        [&](Hyperedge *x){return ((x->id() != device()->id()) && (x->label() != "isA")) ? true : false;},
        [](Hyperedge *x, Hyperedge *y){return ((x->label() == "isA") || (y->label() == "isA")) ? true : false;},
        "Devices",
        UP
    );
    return promote(query);
}

Set* Graph::interfaces()
{
    Hyperedge *query;
    // This query gives all subtypes of this
    query = interface()->traversal(
        [&](Hyperedge *x){return ((x->id() != interface()->id()) && (x->label() != "isA")) ? true : false;},
        [](Hyperedge *x, Hyperedge *y){return ((x->label() == "isA") || (y->label() == "isA")) ? true : false;},
        "Interfaces",
        UP
    );
    return promote(query);
}

Set* Graph::busses()
{
    Hyperedge *query;
    // This query gives all subtypes of this
    query = bus()->traversal(
        [&](Hyperedge *x){return ((x->id() != bus()->id()) && (x->label() != "isA")) ? true : false;},
        [](Hyperedge *x, Hyperedge *y){return ((x->label() == "isA") || (y->label() == "isA")) ? true : false;},
        "Busses",
        UP
    );
    return promote(query);
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
    // At first, we have to add device & interface to the corresponding sets
    device->isA(this->device());
    interface->isA(this->interface());

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
    bus->isA(this->bus());
    interface->isA(this->interface());

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
