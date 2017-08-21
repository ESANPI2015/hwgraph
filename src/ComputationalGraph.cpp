#include "ComputationalGraph.hpp"
#include <iostream>

namespace Hardware {
namespace Computational {

// DICTIONARY
// Concept Ids
const unsigned Graph::InterfaceId = 1001;
const unsigned Graph::BusId       = 1002;
const unsigned Graph::DeviceId    = 1000;
const unsigned Graph::ProcessorId = 1003;
// Relation Concept Ids
const unsigned Graph::IsAId      = CommonConceptGraph::IsAId;
const unsigned Graph::HasAId     = 1004;
const unsigned Graph::ConnectsId = 1005;
const unsigned Graph::PartOfId   = 1006;

// Graph
void Graph::createMainConcepts()
{
    // Create concepts
    Conceptgraph::create(Graph::DeviceId, "DEVICE");
    Conceptgraph::create(Graph::ProcessorId, "PROCESSOR");
    CommonConceptGraph::isA(Graph::ProcessorId, Graph::DeviceId);
    Conceptgraph::create(Graph::InterfaceId, "INTERFACE");
    Conceptgraph::create(Graph::BusId, "BUS");
    // Define relations
    // These are subrelations of the general HAS-A and CONNECTS relations
    Conceptgraph::relate(Graph::HasAId, Graph::DeviceId, Graph::InterfaceId, "HAS-A");
    CommonConceptGraph::subrelationOf(Graph::HasAId, CommonConceptGraph::HasAId);
    Conceptgraph::relate(Graph::ConnectsId, Graph::BusId, Graph::InterfaceId, "CONNECTS");
    CommonConceptGraph::subrelationOf(Graph::ConnectsId, CommonConceptGraph::ConnectsId);
    Conceptgraph::relate(Graph::PartOfId, Graph::ProcessorId, Graph::DeviceId, "PART-OF");
    CommonConceptGraph::subrelationOf(Graph::PartOfId, CommonConceptGraph::PartOfId);
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

Hypergraph::Hyperedges Graph::processorClasses(const std::string& name)
{
    Hyperedges result = CommonConceptGraph::subclassesOf(Graph::ProcessorId, name);
    return result;
}

Hypergraph::Hyperedges Graph::deviceClasses(const std::string& name)
{
    Hyperedges result = CommonConceptGraph::subclassesOf(Graph::DeviceId, name);
    return result;
}

Hypergraph::Hyperedges Graph::interfaceClasses(const std::string& name)
{
    Hyperedges result = CommonConceptGraph::subclassesOf(Graph::InterfaceId, name);
    return result;
}

Hypergraph::Hyperedges Graph::busClasses(const std::string& name)
{
    Hyperedges result = CommonConceptGraph::subclassesOf(Graph::BusId, name);
    return result;
}

unsigned Graph::createProcessor(const std::string& name)
{
    unsigned a = create(name);
    CommonConceptGraph::isA(a, Graph::ProcessorId);
    return a;
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

Hypergraph::Hyperedges Graph::instantiateInterface(const unsigned deviceId, const std::string name)
{
    Hyperedges result;
    Hyperedges interfacesOfDev = interfaces(deviceId, name);
    // Get all subrelationsOf INSTANCE-OF
    Hyperedges subRels = CommonConceptGraph::subrelationsOf(CommonConceptGraph::InstanceOfId);
    // Get all factsOf these subrelations
    Hyperedges facts = CommonConceptGraph::factsOf(subRels);
    for (unsigned interfaceId : interfacesOfDev)
    {
        // Get superclass of interface
        Hyperedges relsFromSub = Conceptgraph::relationsFrom(interfaceId);
        Hyperedges relevantRels = Hypergraph::intersect(relsFromSub, facts);
        Hyperedges superIds = Hypergraph::to(relevantRels);
        // TODO: What if superIds > 1?
        result.insert(CommonConceptGraph::instantiateFrom(*(superIds.begin()), get(interfaceId)->label()));
    }
    return result;
}

unsigned Graph::instantiateDevice(const unsigned superId, const std::string& name)
{
    unsigned id = CommonConceptGraph::instantiateFrom(superId, name);
    has(Hyperedges{id}, instantiateInterface(superId)); // TODO: Really?
    return id;
}

unsigned Graph::instantiateBus(const unsigned superId, const Hyperedges& interfaceIds, const std::string& name)
{
    unsigned id = CommonConceptGraph::instantiateFrom(superId, name);
    connects(Hyperedges{id}, interfaceIds);
    return id;
}

Hypergraph::Hyperedges Graph::devices(const std::string& name, const std::string& className)
{
    // Get all device classes
    Hyperedges classIds = deviceClasses(className);
    // ... and return all instances of them
    return CommonConceptGraph::instancesOf(classIds, name);
}

Hypergraph::Hyperedges Graph::processors(const std::string& name, const std::string& className)
{
    // Get all processor classes
    Hyperedges classIds = processorClasses(className);
    // ... and return all instances of them
    return CommonConceptGraph::instancesOf(classIds, name);
}

Hypergraph::Hyperedges Graph::interfaces(const unsigned deviceId, const std::string& name, const std::string& className)
{
    // Get all interfaceClasses
    Hyperedges classIds = interfaceClasses(className);
    // ... get the instances with the given name
    Hyperedges result = CommonConceptGraph::instancesOf(classIds, name);
    // then intersect it with the children of device (if given)
    if (deviceId)
    {
        result = intersect(result, CommonConceptGraph::childrenOf(Hyperedges{deviceId}));
    }
    return result;
}

Hypergraph::Hyperedges Graph::busses(const std::string& name, const std::string& className)
{
    // Get all busClasses
    Hyperedges classIds = busClasses(className);
    // ... and return all instances of them
    return CommonConceptGraph::instancesOf(classIds, name);
}

unsigned Graph::has(unsigned deviceId, unsigned interfaceId)
{
    return has(Hyperedges{deviceId}, Hyperedges{interfaceId});
}

unsigned Graph::has(const Hyperedges& devices, const Hyperedges& interfaces)
{
    // A device instance or class can only have a interface instance
    Hyperedges fromIds = unite(intersect(this->devices(), devices), intersect(deviceClasses(),devices));
    Hyperedges toIds = intersect(this->interfaces(), interfaces);
    if (fromIds.size() && toIds.size())
        return CommonConceptGraph::relateFrom(fromIds, toIds, Graph::HasAId);
    return 0;
}

unsigned Graph::connects(unsigned busId, unsigned interfaceId)
{
    return connects(Hyperedges{busId}, Hyperedges{interfaceId});
}

unsigned Graph::connects(const Hyperedges& busses, const Hyperedges& interfaces)
{
    // A bus instance can connect interface instances (network), whereas a bus class can connect interface classes (compatibility)
    Hyperedges fromIds = intersect(this->busses(), busses);
    Hyperedges toIds = intersect(this->interfaces(), interfaces);
    if (fromIds.size() && toIds.size())
        return CommonConceptGraph::relateFrom(fromIds, toIds, Graph::ConnectsId);
    fromIds = intersect(busClasses(), busses);
    toIds = intersect(interfaceClasses(), interfaces);
    if (fromIds.size() && toIds.size())
        return CommonConceptGraph::relateFrom(fromIds, toIds, Graph::ConnectsId);
    return 0;
}

unsigned Graph::partOf(const Hyperedges& processorIds, const Hyperedges& deviceIds)
{
    // A device class or instance can contain processor instances, but not processor classes
    Hyperedges toIds = unite(intersect(this->devices(), deviceIds), intersect(deviceClasses(), deviceIds));
    Hyperedges fromIds = intersect(this->processors(), processorIds);
    if (fromIds.size() && toIds.size())
        return CommonConceptGraph::relateFrom(fromIds, toIds, Graph::PartOfId);
    return 0;
}

}
}
