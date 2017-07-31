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
const unsigned Graph::IsAId      = CommonConceptGraph::IsAId;
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
    // These are subrelations of the general HAS-A and CONNECTS relations
    Conceptgraph::relate(Graph::HasAId, Graph::DeviceId, Graph::InterfaceId, "HAS-A");
    CommonConceptGraph::subrelationOf(Graph::HasAId, CommonConceptGraph::HasAId);
    Conceptgraph::relate(Graph::ConnectsId, Graph::BusId, Graph::InterfaceId, "CONNECTS");
    CommonConceptGraph::subrelationOf(Graph::ConnectsId, CommonConceptGraph::ConnectsId);
}

Graph::Graph()
: CommonConceptGraph()
{
    createMainConcepts();
}

Graph::Graph(CommonConceptGraph& A)
: CommonConceptGraph(A)
{
    createMainConcepts();
}

Graph::~Graph()
{
}

Hypergraph::Hyperedges Graph::devices(const std::string& name)
{
    Hyperedges result = CommonConceptGraph::subclassesOf(Graph::DeviceId, name);
    result.erase(Graph::DeviceId);
    return result;
}

Hypergraph::Hyperedges Graph::interfaces(const std::string& name)
{
    Hyperedges result = CommonConceptGraph::subclassesOf(Graph::InterfaceId, name);
    result.erase(Graph::InterfaceId);
    return result;
}

Hypergraph::Hyperedges Graph::busses(const std::string& name)
{
    Hyperedges result = CommonConceptGraph::subclassesOf(Graph::BusId, name);
    result.erase(Graph::BusId);
    return result;
}

unsigned Graph::createDevice(const std::string& name)
{
    unsigned a = create(name);
    CommonConceptGraph::isA(a, Graph::DeviceId);
    return a;
}

unsigned Graph::createInterface(const std::string& name)
{
    unsigned a = create(name);
    CommonConceptGraph::isA(a, Graph::InterfaceId);
    return a;
}

unsigned Graph::createBus(const std::string& name)
{
    unsigned a = create(name);
    CommonConceptGraph::isA(a, Graph::BusId);
    return a;
}

unsigned Graph::has(unsigned deviceId, unsigned interfaceId)
{
    // TODO: domain checking should incorporate classes AND instances thereof
    return CommonConceptGraph::relateFrom(deviceId, interfaceId, Graph::HasAId);
}

unsigned Graph::has(const Hyperedges& devices, const Hyperedges& interfaces)
{
    // TODO: domain checking should incorporate classes AND instances thereof
    return CommonConceptGraph::relateFrom(devices, interfaces, Graph::HasAId);
}

unsigned Graph::connects(unsigned busId, unsigned interfaceId)
{
    // TODO: domain checking should incorporate classes AND instances thereof
    return CommonConceptGraph::relateFrom(busId, interfaceId, Graph::ConnectsId);
}

unsigned Graph::connects(const Hyperedges& busses, const Hyperedges& interfaces)
{
    // TODO: domain checking should incorporate classes AND instances thereof
    return CommonConceptGraph::relateFrom(busses, interfaces, Graph::ConnectsId);
}

}
}
