#include "ComputationalGraph.hpp"

namespace Hardware {
namespace Computational {

// Interface

const std::string Interface::superclassLabel = "HardwareInterface";

// Bus

const std::string Bus::superclassLabel = "Bus";

// Interfaces & Busses
bool Bus::connects(Graph* graph, const unsigned id)
{
    return graph->connects(this->id(), id);
}

// Device
const std::string Device::superclassLabel = "Device";

// Devices & Interfaces
bool Device::has(Graph* graph, const unsigned id)
{
    return graph->has(this->id(), id);
}

// Graph
Graph::Graph()
{
}

Graph::~Graph()
{
}

unsigned Graph::deviceClass()
{
    return getClass(Device::superclassLabel);
}

unsigned Graph::interfaceClass()
{
    return getClass(Interface::superclassLabel);
}

unsigned Graph::busClass()
{
    return getClass(Bus::superclassLabel);
}

unsigned Graph::devices()
{
    // FIXME: This is at least problematic. If we do not make sure, that we only have one representative, then we might produce inconsistent queries here
    auto superSets = find(Device::superclassLabel);
    unsigned relId = Hypergraph::create("DUMMY RELATION");
    for (auto superId : superSets)
    {
        // For each of the superSets we have to get the instances
        // and merge the results
        auto otherId = superclassOf(superId); // Reads: superId -- superclassOf --> ?
        auto nextId  = Hypergraph::unite(relId, otherId);
        Hypergraph::destroy(relId);
        relId = nextId;
    }
    return create(Hypergraph::get(relId)->pointingTo(), "Devices");
}

unsigned Graph::interfaces()
{
    // FIXME: This is at least problematic. If we do not make sure, that we only have one representative, then we might produce inconsistent queries here
    auto superSets = find(Interface::superclassLabel);
    unsigned relId = Hypergraph::create("DUMMY RELATION");
    for (auto superId : superSets)
    {
        // For each of the superSets we have to get the instances
        // and merge the results
        auto otherId = superclassOf(superId); // Reads: superId -- superclassOf --> ?
        auto nextId  = Hypergraph::unite(relId, otherId);
        Hypergraph::destroy(relId);
        relId = nextId;
    }
    return create(Hypergraph::get(relId)->pointingTo(), "Interfaces");
}

unsigned Graph::busses()
{
    // FIXME: This is at least problematic. If we do not make sure, that we only have one representative, then we might produce inconsistent queries here
    auto superSets = find(Bus::superclassLabel);
    unsigned relId = Hypergraph::create("DUMMY RELATION");
    for (auto superId : superSets)
    {
        // For each of the superSets we have to get the instances
        // and merge the results
        auto otherId = superclassOf(superId); // Reads: superId -- superclassOf --> ?
        auto nextId  = Hypergraph::unite(relId, otherId);
        Hypergraph::destroy(relId);
        relId = nextId;
    }
    return create(Hypergraph::get(relId)->pointingTo(), "Busses");
}

unsigned Graph::createDevice(const std::string& name)
{
    // TODO: Make templated function create<"Device">(name) ? Or at least instantiateFrom("Device", name);
    auto id = create(name);
    relateTo(id, deviceClass(), Relation::isALabel);
    return id;
}

unsigned Graph::createInterface(const std::string& name)
{
    // TODO: Make templated function create<"Device">(name) ? Or at least instantiateFrom("Device", name);
    auto id = create(name);
    relateTo(id, interfaceClass(), Relation::isALabel);
    return id;
}

unsigned Graph::createBus(const std::string& name)
{
    // TODO: Make templated function create<"Device">(name) ? Or at least instantiateFrom("Device", name);
    auto id = create(name);
    relateTo(id, busClass(), Relation::isALabel);
    return id;
}

unsigned Graph::has(unsigned deviceId, unsigned interfaceId)
{
    // Either we
    // * check if deviceId -- isA --> device
    // * or, we imply deviceId -- isA --> device
    // For now, we follow the second approach (although this ALWAYS creates a new relation!!!)
    relateTo(deviceId, deviceClass(), Relation::isALabel);
    relateTo(interfaceId, interfaceClass(), Relation::isALabel);
    return relateTo(deviceId, interfaceId, Relation::hasLabel);
}

unsigned Graph::connects(unsigned busId, unsigned interfaceId)
{
    // Either we
    // * check if deviceId -- isA --> device
    // * or, we imply deviceId -- isA --> device
    // For now, we follow the second approach (although this ALWAYS creates a new relation!!!)
    relateTo(busId, busClass(), Relation::isALabel);
    relateTo(interfaceId, interfaceClass(), Relation::isALabel);
    return relateTo(busId, interfaceId, Relation::connectedToLabel);
}

}
}
