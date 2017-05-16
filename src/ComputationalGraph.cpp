#include "ComputationalGraph.hpp"

namespace Hardware {
namespace Computational {

// Interface

const std::string Interface::classLabel = "HardwareInterface";
unsigned Interface::lastSuperclassId = 0;

Set* Interface::Superclass()
{
    Hyperedge *edge;
    if (!Interface::lastSuperclassId || !(edge = Hyperedge::find(Interface::lastSuperclassId)))
    {
        // First call or previous superclass has been destroyed
        edge = Set::create(Interface::classLabel);
        lastSuperclassId = edge->id();
    }
    return static_cast<Set*>(edge);
}

// Bus

const std::string Bus::classLabel = "Bus";
unsigned Bus::lastSuperclassId = 0;

Set* Bus::Superclass()
{
    Hyperedge *edge;
    if (!Bus::lastSuperclassId || !(edge = Hyperedge::find(Bus::lastSuperclassId)))
    {
        // First call or previous superclass has been destroyed
        edge = Set::create(Bus::classLabel);
        Bus::lastSuperclassId = edge->id();
    }
    return static_cast<Set*>(edge);
}

// Interfaces & Busses
bool Bus::connects(Set* interface)
{
    bool result = true;

    // Make sure, that we are a Bus
    result &= this->isA(Bus::Superclass());
    // Make sure, that the set to be used as an interface is derived from interface superclass
    result &= interface->isA(Interface::Superclass());

    // Find a connects relation in device
    // TODO: These relations should also be registered by static relation class labels!!
    auto edges = pointingTo("connects");
    Relation *connects = NULL;
    if (edges.size())
    {
        connects = Relation::promote(Hyperedge::find(*edges.begin()));
        result &= connects->to(interface);
    } else {
        // Finally we create a new relation (1-to-1)
        connects = Relation::create("connects");
        result &= connects->from(this);
        result &= connects->to(interface);
    }

    return result;
}

bool Bus::connects(Set::Sets interfaces)
{
    bool result = true;

    // 1-N relation based on 2-hyperedges (1-1 relations)
    for (auto interfaceId : interfaces)
    {
        auto interface = Set::promote(Hyperedge::find(interfaceId));
        result &= connects(interface);
    }

    return result;
}

// Device
const std::string Device::classLabel = "Device";
unsigned Device::lastSuperclassId = 0;

Set* Device::Superclass()
{
    Hyperedge *edge;
    if (!Device::lastSuperclassId || !(edge = Hyperedge::find(Device::lastSuperclassId)))
    {
        // First call or previous superclass has been destroyed
        edge = Set::create(Device::classLabel);
        Device::lastSuperclassId = edge->id();
    }
    return static_cast<Set*>(edge);
}

// Devices & Interfaces
bool Device::has(Set* interface)
{
    bool result = true;

    // At first, we have to add device & interface to the corresponding sets
    result &= this->isA(Device::Superclass());
    result &= interface->isA(Interface::Superclass());

    // Find a has relation in device
    auto edges = pointingTo("has");
    Relation *has = NULL;
    if (edges.size())
    {
        has = Relation::promote(Hyperedge::find(*edges.begin()));
        result &= has->to(interface);
    } else {
        // Finally we create a new relation (1-to-1)
        has = Relation::create("has");
        result &= has->from(this);
        result &= has->to(interface);
    }

    return result;
}

bool Device::has(Set::Sets interfaces)
{
    bool result = true;
    // 1-N relation based on 2-hyperedges (1-1 relations)
    for (auto interfaceId : interfaces)
    {
        auto interface = Set::promote(Hyperedge::find(interfaceId));
        result &= has(interface);
    }
    return result;
}

Set* Device::aggregates()
{
    Set::Sets result;
    Hyperedge::Hyperedges hasRels = pointingTo("has");
    for (auto relId : hasRels)
    {
        auto others = Set::promote(Hyperedge::find(relId)->pointingTo());
        result.insert(others.begin(), others.end());
    }
    return Set::create(result, "aggregates");
}   

// Graph

Graph* Graph::create(const std::string& label)
{
    Graph *neu = static_cast<Graph*>(Set::create(label));
    Device::Superclass()->memberOf(neu);
    Interface::Superclass()->memberOf(neu);
    Bus::Superclass()->memberOf(neu);
    return neu;
}

Set* Graph::devices()
{
    Relation *superOf = Device::Superclass()->superclassOf();
    Set *result = Set::create(Set::promote(superOf->pointingTo()), "Devices");
    delete superOf;
    return result;
}

Set* Graph::interfaces()
{
    Relation *superOf = Interface::Superclass()->superclassOf();
    Set *result = Set::create(Set::promote(superOf->pointingTo()), "Interfaces");
    delete superOf;
    return result;
}

Set* Graph::busses()
{
    Relation *superOf = Bus::Superclass()->superclassOf();
    Set *result = Set::create(Set::promote(superOf->pointingTo()), "Busses");
    delete superOf;
    return result;
}

Device* Graph::createDevice(const std::string& name)
{
    return Set::create<Device>(name);
}

Interface* Graph::createInterface(const std::string& name)
{
    return Set::create<Interface>(name);
}

Bus* Graph::createBus(const std::string& name)
{
    return Set::create<Bus>(name);
}

bool Graph::has(Set* device, Set* interface)
{
    return device->promote<Device>()->has(interface);
}

bool Graph::has(Set* device, Set::Sets interfaces)
{
    return device->promote<Device>()->has(interfaces);
}

bool Graph::has(Set::Sets devices, Set* interface)
{
    bool result = true;

    // N-1 relation based on 2-hyperedges
    for (auto deviceId : devices)
    {
        auto device = Set::promote(Hyperedge::find(deviceId))->promote<Device>();
        result &= device->has(interface);
    }
    return result;
}

bool Graph::has(Set::Sets devices, Set::Sets interfaces)
{
    bool result = true;
    // N-M relation based on N * (1-M) relations
    for (auto deviceId : devices)
    {
        auto device = Set::promote(Hyperedge::find(deviceId))->promote<Device>();
        result &= device->has(interfaces);
    }
    return result;
}

bool Graph::connects(Set* bus, Set* interface)
{
    return bus->promote<Bus>()->connects(interface);
}

bool Graph::connects(Set* bus, Set::Sets interfaces)
{
    return bus->promote<Bus>()->connects(interfaces);
}

bool Graph::connects(Set::Sets busses, Set* interface)
{
    bool result = true;
    // N-1 relation based on 2-hyperedges (1-1 relations)
    for (auto busId : busses)
    {
        auto bus = Set::promote(Hyperedge::find(busId))->promote<Bus>();
        result &= bus->connects(interface);
    }
    return result;
}

bool Graph::connects(Set::Sets busses, Set::Sets interfaces)
{
    bool result = true;
    // N-M relation based on N * (1-M) relations
    for (auto busId : busses)
    {
        auto bus = Set::promote(Hyperedge::find(busId))->promote<Bus>();
        result &= bus->connects(interfaces);
    }
    return result;
}

}
}
