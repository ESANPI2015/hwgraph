#include "ComputationalGraph.hpp"
#include <iostream>

namespace Hardware {
namespace Computational {

// DICTIONARY
// Concept Ids
const unsigned Graph::InterfaceId = 1001;
const unsigned Graph::BusId       = 1002;
const unsigned Graph::DeviceId    = 1000;
// Relation Concept Ids
// TODO: These are general enough to be noted in a common base class/library
const unsigned Graph::IsAId      = 11;
const unsigned Graph::HasAId     = 22;
const unsigned Graph::ConnectsId = 33;

// Graph
void Graph::createMainConcepts()
{
    Conceptgraph::create(Graph::DeviceId, "DEVICE");
    Conceptgraph::create(Graph::InterfaceId, "INTERFACE");
    Conceptgraph::create(Graph::BusId, "BUS");
    // FIXME: The following concepts should be relations not concepts ... or?
    Conceptgraph::create(Graph::IsAId, "IS-A");
    Conceptgraph::create(Graph::HasAId, "HAS-A");
    Conceptgraph::create(Graph::ConnectsId, "CONNECTS");
}

Graph::Graph()
: Conceptgraph()
{
    createMainConcepts();
}

Graph::Graph(Conceptgraph& A)
: Conceptgraph(A)
{
    createMainConcepts();
}

Graph::~Graph()
{
}

Hypergraph::Hyperedges Graph::devices(const std::string& name)
{
    Hyperedges result;
    result = Conceptgraph::traverse(Graph::DeviceId, name, get(Graph::IsAId)->label(), UP);
    result.erase(Graph::DeviceId);
    return result;
}

Hypergraph::Hyperedges Graph::interfaces(const std::string& name)
{
    Hyperedges result;
    result = Conceptgraph::traverse(Graph::InterfaceId, name, get(Graph::IsAId)->label(), UP);
    result.erase(Graph::InterfaceId);
    return result;
}

Hypergraph::Hyperedges Graph::busses(const std::string& name)
{
    Hyperedges result;
    result = Conceptgraph::traverse(Graph::BusId, name, get(Graph::IsAId)->label(), UP);
    result.erase(Graph::BusId);
    return result;
}

unsigned Graph::createDevice(const std::string& name)
{
    createMainConcepts();
    unsigned a = create(name);
    Conceptgraph::relate(a, Graph::DeviceId, get(Graph::IsAId)->label()); // TODO: This should be in a base class
    return a;
}

unsigned Graph::createInterface(const std::string& name)
{
    unsigned a = create(name);
    createMainConcepts();
    Conceptgraph::relate(a, Graph::InterfaceId, get(Graph::IsAId)->label()); // TODO: This should be in a base class
    return a;
}

unsigned Graph::createBus(const std::string& name)
{
    unsigned a = create(name);
    createMainConcepts();
    Conceptgraph::relate(a, Graph::BusId, get(Graph::IsAId)->label()); // TODO: This should be in a base class
    return a;
}

unsigned Graph::has(unsigned deviceId, unsigned interfaceId)
{
    // Either we
    // * check if deviceId -- isA --> device
    // * or, we imply deviceId -- isA --> device
    if (devices().count(deviceId) && interfaces().count(interfaceId))
    {
        return relate(deviceId, interfaceId, get(Graph::HasAId)->label());
    }
    return 0;
}

unsigned Graph::has(const Hyperedges& devices, const Hyperedges& interfaces)
{
    // Is this elegant or not :)
    return relate(intersect(this->devices(), devices), intersect(this->interfaces(), interfaces), get(Graph::HasAId)->label());
}

unsigned Graph::connects(unsigned busId, unsigned interfaceId)
{
    // Either we
    // * check if deviceId -- isA --> device
    // * or, we imply deviceId -- isA --> device
    // This is the first approach?
    if (busses().count(busId) && interfaces().count(interfaceId))
    {
        return relate(busId, interfaceId, get(Graph::ConnectsId)->label()); // TODO: This relation label should be in a base class/dictionary
    }
    return 0;
}

unsigned Graph::connects(const Hyperedges& busses, const Hyperedges& interfaces)
{
    // Is this elegant or not :)
    return relate(intersect(this->busses(), busses), intersect(this->interfaces(), interfaces), get(Graph::ConnectsId)->label());
}

}
}
