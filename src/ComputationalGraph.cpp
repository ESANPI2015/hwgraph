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
const unsigned Graph::IsAId      = 1003;
const unsigned Graph::HasAId     = 1004;
const unsigned Graph::ConnectsId = 1005;

// Graph
void Graph::createMainConcepts()
{
    // Create concepts
    Conceptgraph::create(Graph::DeviceId, "DEVICE");
    Conceptgraph::create(Graph::InterfaceId, "INTERFACE");
    Conceptgraph::create(Graph::BusId, "BUS");
    // Define relations
    Conceptgraph::relate(Graph::IsAId, 1, 1, "IS-A");
    Conceptgraph::relate(Graph::HasAId, Graph::DeviceId, Graph::InterfaceId, "HAS-A");
    Conceptgraph::relate(Graph::ConnectsId, Graph::BusId, Graph::InterfaceId, "CONNECTS");
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
    Conceptgraph::relate(a, Graph::DeviceId, Graph::IsAId); // TODO: This should be in a base class
    return a;
}

unsigned Graph::createInterface(const std::string& name)
{
    unsigned a = create(name);
    createMainConcepts();
    Conceptgraph::relate(a, Graph::InterfaceId, Graph::IsAId); // TODO: This should be in a base class
    return a;
}

unsigned Graph::createBus(const std::string& name)
{
    unsigned a = create(name);
    createMainConcepts();
    Conceptgraph::relate(a, Graph::BusId, Graph::IsAId); // TODO: This should be in a base class
    return a;
}

unsigned Graph::has(unsigned deviceId, unsigned interfaceId)
{
    // Either we
    // * check if deviceId -- isA --> device
    // * or, we imply deviceId -- isA --> device
    if (devices().count(deviceId) && interfaces().count(interfaceId))
    {
        return relate(deviceId, interfaceId, Graph::HasAId);
    }
    return 0;
}

unsigned Graph::has(const Hyperedges& devices, const Hyperedges& interfaces)
{
    // Is this elegant or not :)
    return relate(intersect(this->devices(), devices), intersect(this->interfaces(), interfaces), Graph::HasAId);
}

unsigned Graph::connects(unsigned busId, unsigned interfaceId)
{
    // Either we
    // * check if deviceId -- isA --> device
    // * or, we imply deviceId -- isA --> device
    // This is the first approach?
    if (busses().count(busId) && interfaces().count(interfaceId))
    {
        return relate(busId, interfaceId, Graph::ConnectsId);
    }
    return 0;
}

unsigned Graph::connects(const Hyperedges& busses, const Hyperedges& interfaces)
{
    // Is this elegant or not :)
    return relate(intersect(this->busses(), busses), intersect(this->interfaces(), interfaces), Graph::ConnectsId);
}

}
}
