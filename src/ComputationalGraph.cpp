#include "ComputationalGraph.hpp"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <sstream>

namespace Simple {

void Computation::setupMetaModel()
{
    createProcessor("Simple::Computation::Processor::Conventional", "CONVENTIONAL");
    createProcessor("Simple::Computation::Processor::Fpga", "FPGA");
    createInterface("Simple::Computation::Interface::NDLComLVDS", "NDLCOM");
    createInterface("Simple::Computation::Interface::Input", "SOURCE");
    createInterface("Simple::Computation::Interface::Output", "SINK");
    createBus("Simple::Computation::Bus::NDLComOverLVDS", "NDLCOM@LVDS");
    createNetwork("Simple::Computation::Subgraph", "SUBGRAPH");
}

Computation::Computation()
{
    setupMetaModel();
}

Computation::Computation(const Hypergraph& base)
: Hardware::Computational::Network(base)
{
    setupMetaModel();
}

Computation::~Computation()
{
}

std::string Computation::labelFromIdAndName(const unsigned id, const std::string& name)
{
    return labelFromIdAndName(std::to_string(id), name);
}

std::string Computation::labelFromIdAndName(const std::string& id, const std::string& name)
{
    return name + "_" + id;
}

unsigned Computation::idFromLabel(const std::string& label)
{
    std::string id;
    std::size_t pos = label.rfind("_");
    if (pos != std::string::npos)
    {
        id = label.substr(pos+1);
    }
    return id.empty() ? 0 : std::stoul(id);
}

std::string Computation::nameFromLabel(const std::string& label)
{
    std::size_t pos = label.rfind("_");
    return label.substr(0,pos);
}

std::string Computation::domainSpecificExport(const UniqueId& uid)
{
    std::map< UniqueId,unsigned > unique2nodeId;
    std::map< UniqueId,unsigned > unique2portId;
    std::map< std::string, std::map< std::string, Hyperedges> > old2newPort;
    std::stringstream ss;
    YAML::Node spec;
    spec["name"] = get(uid)->label();

    // Store parts
    Hyperedges partsOfNet(componentsOf(Hyperedges{uid}));
    Hyperedges busses;
    YAML::Node nodesYAML(spec["nodes"]);
    for (const UniqueId& partUid : partsOfNet)
    {
        YAML::Node nodeYAML;
        Hyperedges superClasses(instancesOf(Hyperedges{partUid}, "", TraversalDirection::FORWARD));
        if (superClasses.empty())
        {
            std::cout << "No superclass for " << partUid << "\n";
            continue;
        }
        // Register but skip busses
        if (superClasses.count("Simple::Computation::Bus::NDLComOverLVDS"))
        {
            busses.insert(partUid);
            continue;
        }

        // Handle interfaces
        YAML::Node portsYAML(nodeYAML["ports"]);
        Hyperedges interfaceUids(interfacesOf(Hyperedges{partUid}));
        for (const UniqueId& interfaceUid : interfaceUids)
        {
            Hyperedges superClasses2(instancesOf(Hyperedges{interfaceUid}, "", TraversalDirection::FORWARD));
            if (superClasses2.empty())
            {
                std::cout << "No superclass for " << interfaceUid << "\n";
                continue;
            }
            YAML::Node portYAML;
            const unsigned portId(idFromLabel(get(interfaceUid)->label()));
            portYAML["id"] = portId;
            portYAML["name"] = nameFromLabel(get(interfaceUid)->label());
            portYAML["type"] = get(*superClasses2.begin())->label();
            portsYAML.push_back(portYAML);
            unique2portId[interfaceUid] = portId;
        }

        // Store node
        const unsigned nodeId(idFromLabel(get(partUid)->label()));
        nodeYAML["id"] = nodeId;
        nodeYAML["name"] = nameFromLabel(get(partUid)->label());
        nodeYAML["type"] = get(*superClasses.begin())->label();
        nodesYAML.push_back(nodeYAML);
        unique2nodeId[partUid] = nodeId;
    }

    // Store connections
    YAML::Node edgesYAML(spec["edges"]);
    for (const UniqueId& busUid : busses)
    {
        YAML::Node edgeYAML;
        // Handle interfaces
        YAML::Node edgeNodesYAML(edgeYAML["nodes"]);
        Hyperedges interfaceUids(interfacesOf(Hyperedges{busUid}));
        for (const UniqueId& interfaceUid : interfaceUids)
        {
            YAML::Node edgeNodeYAML;
            Hyperedges otherInterfaceUids(endpointsOf(Hyperedges{interfaceUid}));
            Hyperedges otherNodeUids(childrenOf(otherInterfaceUids, "", TraversalDirection::INVERSE));
            edgeNodeYAML["id"] = unique2nodeId[*otherNodeUids.begin()];
            edgeNodeYAML["port"] = unique2portId[*otherInterfaceUids.begin()];
            edgeNodesYAML.push_back(edgeNodeYAML);
        }
        edgeYAML["id"] = idFromLabel(get(busUid)->label());
        edgeYAML["name"] = nameFromLabel(get(busUid)->label());
        edgesYAML.push_back(edgeYAML);
    }

    ss << spec;
    return ss.str();
}

bool Computation::domainSpecificImport(const std::string& serialized)
{
    std::map< std::string,Hyperedges > old2new;
    std::map< std::string, std::map< std::string, Hyperedges> > old2newPort;
    YAML::Node spec = YAML::Load(serialized);
    if (!spec["name"].IsDefined())
        return false;
    const std::string& name(spec["name"].as<std::string>());
    // Create a network class
    Hyperedges networkUid(createNetwork("Simple::Computation::Subgraph::"+name, name, Hyperedges{"Simple::Computation::Subgraph"}));
    if (!spec["nodes"].IsDefined())
        return false;
    Hyperedges partsOfNet;
    const YAML::Node& nodesYAML(spec["nodes"]); // FIXME: Better naming: processors, devices
    for (YAML::Node::const_iterator nit = nodesYAML.begin(); nit != nodesYAML.end(); ++nit)
    {
        const YAML::Node& nodeYAML(*nit);
        const std::string& nodeId(nodeYAML["id"].as<std::string>()); // FIXME: We might have to PRESERVE this id ... it could be an NDLCom DeviceId!!!
        const std::string& nodeName(nodeYAML["name"].as<std::string>());
        const std::string& nodeType(nodeYAML["type"].as<std::string>());
        // Instantiate processor
        Hyperedges uid(instantiateComponent(processorClasses(nodeType), labelFromIdAndName(nodeId,nodeName)));
        old2new[nodeId] = uid;
        std::cout << "Instantiated " << labelFromIdAndName(nodeId,nodeName) << " of type " << nodeType << "\n";
        partsOfNet = unite(partsOfNet, uid);

        // Handle interfaces
        const YAML::Node& portsYAML(nodeYAML["ports"]); // FIXME: better naming: interfaces
        if (portsYAML.IsDefined())
        {
            Hyperedges interfaceUids;
            for (YAML::Node::const_iterator pit = portsYAML.begin(); pit != portsYAML.end(); ++pit)
            {
                const YAML::Node& portYAML(*pit);
                const std::string& portId(portYAML["id"].as<std::string>());
                const std::string& portName(portYAML["name"].as<std::string>());
                const std::string& portType(portYAML["type"].as<std::string>());
                // Instantiate interface
                Hyperedges interfaceUid(instantiateFrom(interfaceClasses(portType), labelFromIdAndName(portId, portName)));
                std::cout << "Instantiated interface " << labelFromIdAndName(portId, portName) << " of type " << portType << "\n";
                old2newPort[nodeId][portId] = interfaceUid;
                interfaceUids = unite(interfaceUids, interfaceUid);
            }
            hasInterface(uid, interfaceUids);
            partsOfNet = unite(partsOfNet, interfaceUids);
        }
    }
    const YAML::Node& edgesYAML(spec["edges"]);
    if (edgesYAML.IsDefined())
    {
        for (YAML::Node::const_iterator eit = edgesYAML.begin(); eit != edgesYAML.end(); ++eit)
        {
            const YAML::Node& edgeYAML(*eit);
            const std::string& edgeId(edgeYAML["id"].as<std::string>());
            const std::string& edgeName(edgeYAML["name"].as<std::string>());
            const YAML::Node& edgeNodesYAML(edgeYAML["nodes"]); // FIXME: better naming: endpoints
            if (!edgeNodesYAML.IsDefined())
                return false;
            // Instantiate bus
            Hyperedges busUid(instantiateFrom(Hyperedges{"Simple::Computation::Bus::NDLComOverLVDS"}, labelFromIdAndName(edgeId, edgeName)));
            std::cout << "Instantiated bus " << labelFromIdAndName(edgeId, edgeName) << "\n";
            partsOfNet = unite(partsOfNet, busUid);

            // Create interfaces
            Hyperedges interfaceUids;
            for (YAML::Node::const_iterator enit = edgeNodesYAML.begin(); enit != edgeNodesYAML.end(); ++enit)
            {
                const YAML::Node& edgeNodeYAML(*enit);
                const std::string& nodeId(edgeNodeYAML["id"].as<std::string>());
                const std::string& portId(edgeNodeYAML["port"].as<std::string>()); // FIXME: better naming: nodeId, portId
                // Instantiate bus interface
                Hyperedges interfaceUid(instantiateFrom(Hyperedges{"Simple::Computation::Interface::NDLComLVDS"}, nodeId+portId));
                std::cout << "Instantiated bus interface " << nodeId+portId << "\n";
                // Connect it to the other interface
                std::cout << connectInterface(interfaceUid, old2newPort[nodeId][portId]) << std::endl;
                interfaceUids = unite(interfaceUids, interfaceUid);
            }
            hasInterface(busUid, interfaceUids);
            partsOfNet = unite(partsOfNet, interfaceUids);
        }
    }
    // Make all stuff part of the network
    partOfNetwork(partsOfNet, networkUid);
    return true;
}

}
