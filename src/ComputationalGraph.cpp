#include "ComputationalGraph.hpp"

namespace Hardware {
namespace Computational {

// Interface
const std::string Graph::InterfaceLabel = "HardwareInterface";
// Bus
const std::string Graph::BusLabel = "Bus";
// Device
const std::string Graph::DeviceLabel = "Device";

// Graph
Graph::Graph()
: Conceptgraph()
{
}

Graph::Graph(Conceptgraph& A)
: Conceptgraph(A)
{
    // We have to search the underlying concept graph and sort everything into 3 sets
    // A <-- ISA --> DeviceLabel means that A is to be inserted into _devices set
    Hyperedges candidates = Conceptgraph::find(Graph::DeviceLabel);
    for (unsigned candidateId : candidates)
    {
        // We now have access to all ROOT NODES: traverse the subgraph spanned by IS-A relations and add every hedge to the corresponding set
        Hyperedges devices = traverse(candidateId, "", "IS-A");
        _devices.insert(devices.begin(), devices.end());
    }
    // Handle interface concept
    candidates = Conceptgraph::find(Graph::InterfaceLabel);
    for (unsigned candidateId : candidates)
    {
        // We now have access to all ROOT NODES: traverse the subgraph spanned by IS-A relations and add every hedge to the corresponding set
        Hyperedges interfaces = traverse(candidateId, "", "IS-A");
        _interfaces.insert(interfaces.begin(), interfaces.end());
    }
    // Handle bus concept
    candidates = Conceptgraph::find(Graph::BusLabel);
    for (unsigned candidateId : candidates)
    {
        // We now have access to all ROOT NODES: traverse the subgraph spanned by IS-A relations and add every hedge to the corresponding set
        Hyperedges busses = traverse(candidateId, "", "IS-A");
        _busses.insert(busses.begin(), busses.end());
    }
    // Now we have sorted everything ... maybe we should have this also as a reparse function?
}

Graph::~Graph()
{
}

unsigned Graph::deviceConcept()
{
    // TODO: This could be a nice method at concept graph level (named findOrCreate(label))
    unsigned id;
    Hyperedges candidates = Conceptgraph::find(Graph::DeviceLabel);
    if (candidates.size())
    {
        id = *candidates.begin();
    } else {
        id = Conceptgraph::create(Graph::DeviceLabel);
    }
    return id;
}

unsigned Graph::interfaceConcept()
{
    // TODO: This could be a nice method at concept graph level (named findOrCreate(label))
    unsigned id;
    Hyperedges candidates = Conceptgraph::find(Graph::InterfaceLabel);
    if (candidates.size())
    {
        id = *candidates.begin();
    } else {
        id = Conceptgraph::create(Graph::InterfaceLabel);
    }
    return id;
}

unsigned Graph::busConcept()
{
    // TODO: This could be a nice method at concept graph level (named findOrCreate(label))
    unsigned id;
    Hyperedges candidates = Conceptgraph::find(Graph::BusLabel);
    if (candidates.size())
    {
        id = *candidates.begin();
    } else {
        id = Conceptgraph::create(Graph::BusLabel);
    }
    return id;
}

Hypergraph::Hyperedges Graph::devices(const std::string& name)
{
    // Use the _devices cache for now.
    // TODO: However, we need some update or constructor (see ConceptGraph) which takes care of constructing/parsing a hwgraph from a conceptgraph
    Hyperedges result;
    for (auto id : _devices)
    {
        if (name.empty() || (name == get(id)->label()))
        {
            result.insert(id);
        }
    }
    return result;
}

Hypergraph::Hyperedges Graph::interfaces(const std::string& name)
{
    Hyperedges result;
    for (auto id : _interfaces)
    {
        if (name.empty() || (name == get(id)->label()))
        {
            result.insert(id);
        }
    }
    return result;
}

Hypergraph::Hyperedges Graph::busses(const std::string& name)
{
    Hyperedges result;
    for (auto id : _busses)
    {
        if (name.empty() || (name == get(id)->label()))
        {
            result.insert(id);
        }
    }
    return result;
}

unsigned Graph::createDevice(const std::string& name)
{
    // Find the deviceConcept
    unsigned A = deviceConcept();
    unsigned a = create(name);
    relate(a, A, "IS-A"); // TODO: This should be in a base class
    _devices.insert(a);
    return a;
}

unsigned Graph::createInterface(const std::string& name)
{
    // Find the interfaceConcept
    unsigned A = interfaceConcept();
    unsigned a = create(name);
    relate(a, A, "IS-A"); // TODO: This should be in a base class
    _interfaces.insert(a);
    return a;
}

unsigned Graph::createBus(const std::string& name)
{
    // Find the busConcept
    unsigned A = busConcept();
    unsigned a = create(name);
    relate(a, A, "IS-A"); // TODO: This should be in a base class
    _busses.insert(a);
    return a;
}

unsigned Graph::has(unsigned deviceId, unsigned interfaceId)
{
    // Either we
    // * check if deviceId -- isA --> device
    // * or, we imply deviceId -- isA --> device
    // This is the first approach?
    if (_devices.count(deviceId) && _interfaces.count(interfaceId))
    {
        return relate(deviceId, interfaceId, "HAS"); // TODO: This relation label should be in a base class/dictionary
    }
    return 0;
}

unsigned Graph::has(const Hyperedges& devices, const Hyperedges& interfaces)
{
    // TODO: Implement. Try to use one N:M relation
    return 0;
}

unsigned Graph::connects(unsigned busId, unsigned interfaceId)
{
    // Either we
    // * check if deviceId -- isA --> device
    // * or, we imply deviceId -- isA --> device
    // This is the first approach?
    if (_busses.count(busId) && _interfaces.count(interfaceId))
    {
        return relate(busId, interfaceId, "CONNECTS"); // TODO: This relation label should be in a base class/dictionary
    }
    return 0;
}

unsigned Graph::connects(const Hyperedges& busses, const Hyperedges& interfaces)
{
    // TODO: Implement. Try to use one N:M relation
    return 0;
}

}
}
