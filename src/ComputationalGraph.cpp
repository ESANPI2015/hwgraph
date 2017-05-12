#include "ComputationalGraph.hpp"

namespace Hardware {
namespace Computational {

// Interface

Set* Interface::superclass = NULL;

Interface::Interface(const std::string& label)
: Set(label)
{
    // Create isA relation to the superclass
    this->isA(Interface::Superclass());
}

Set* Interface::Superclass()
{
    if (!Interface::superclass)
    {
        Interface::superclass = Set::create("Interface");
    }
    return Interface::superclass;
}

// Bus

Set* Bus::superclass = NULL;

Bus::Bus(const std::string& label)
: Set(label)
{
    // Create isA relation to the superclass
    this->isA(superclass);
}

Set* Bus::Superclass()
{
    if (!Bus::superclass)
    {
        Bus::superclass = Set::create("Bus");
    }
    return Bus::superclass;
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
Set* Device::superclass = NULL;

Device::Device(const std::string& label)
: Set(label)
{
    // Create isA relation to the superclass
    this->isA(superclass);
}

Set* Device::Superclass()
{
    if (!Device::superclass)
    {
        Device::superclass = Set::create("Device");
    }
    return Device::superclass;
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

Graph::Graph(const std::string& label)
: Set(label)
{
    Device::Superclass()->memberOf(this);
    Interface::Superclass()->memberOf(this);
    Bus::Superclass()->memberOf(this);
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
    Set *newbie = Set::create(name);
    newbie->isA(Device::Superclass());
    return static_cast<Device*>(newbie);
}

Interface* Graph::createInterface(const std::string& name)
{
    Set *newbie = Set::create(name);
    newbie->isA(Interface::Superclass());
    return static_cast<Interface*>(newbie);
}

Bus* Graph::createBus(const std::string& name)
{
    Set *newbie = Set::create(name);
    newbie->isA(Bus::Superclass());
    return static_cast<Bus*>(newbie);
}

bool Graph::has(Set* device, Set* interface)
{
    return static_cast<Device*>(device)->has(interface);
}

bool Graph::has(Set* device, Set::Sets interfaces)
{
    return static_cast<Device*>(device)->has(interfaces);
}

bool Graph::has(Set::Sets devices, Set* interface)
{
    bool result = true;

    // N-1 relation based on 2-hyperedges
    for (auto deviceId : devices)
    {
        auto device = static_cast<Device*>(Hyperedge::find(deviceId));
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
        auto device = static_cast<Device*>(Hyperedge::find(deviceId));
        result &= device->has(interfaces);
    }
    return result;
}

bool Graph::connects(Set* bus, Set* interface)
{
    return static_cast<Bus*>(bus)->connects(interface);
}

bool Graph::connects(Set* bus, Set::Sets interfaces)
{
    return static_cast<Bus*>(bus)->connects(interfaces);
}

bool Graph::connects(Set::Sets busses, Set* interface)
{
    bool result = true;
    // N-1 relation based on 2-hyperedges (1-1 relations)
    for (auto busId : busses)
    {
        auto bus = static_cast<Bus*>(Hyperedge::find(busId));
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
        auto bus = static_cast<Bus*>(Hyperedge::find(busId));
        result &= bus->connects(interfaces);
    }
    return result;
}

}
}
