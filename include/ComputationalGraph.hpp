#ifndef _HW_COMPUTATIONAL_GRAPH_HPP
#define _HW_COMPUTATIONAL_GRAPH_HPP

#include "CommonConceptGraph.hpp"

namespace Hardware {
namespace Computational {

/*
    Some notes:
    The underlying fundament is the concept graph.
    In this environment there are two main entities, concepts & relations between them.
    This class is derived from the concept graph class.

    It introduces three different concepts: DEVICE, INTERFACE & BUS
    The domain is encoded as follows:
    
    DEVICE -- has --> INTERFACE <-- connects(>=2) -- BUS

    If some X is a DEVICE then there exists a path of IS-A relations from X to DEVICE

    NOTE: The concepts are encoded by UNIQUE IDS. This makes them natural language independent.

    The devices are subclassed by the concept of PROCESSOR.
    This class shall group all devices which are able to process data/execute algorithms.
    Normally, this processor devices are PART-OF bigger devices which also host interface(s), power electronics etc.

    PROCESSOR -- partOf --> DEVICE    

    However, this encoding is optional and a network of connected processors is valid in itself.

*/

class Graph;

class Graph : public CommonConceptGraph
{
    public:
        // Ids for identifiing devices, interfaces and busses
        static const UniqueId DeviceId;
        static const UniqueId ProcessorId;
        static const UniqueId InterfaceId;
        static const UniqueId BusId;
        // Ids for identifiing IsA, HasA and Connects
        static const UniqueId IsAId;
        static const UniqueId HasAId;
        static const UniqueId ConnectsId;
        static const UniqueId PartOfId;

        // Constructor/Destructor
        Graph();
        Graph(CommonConceptGraph& A);
        ~Graph();

        // Creates the main concepts
        void createMainConcepts();

        // Factory functions
        // NOTE: These create classes, not individuals
        Hyperedges createDevice(const std::string& name="Device");
        Hyperedges createProcessor(const std::string& name="Processor");
        Hyperedges createInterface(const std::string& name="Interface");
        Hyperedges createBus(const std::string& name="Bus");
        // NOTE: These create individuals from classes
        Hyperedges instantiateDevice(const UniqueId superId, const std::string& name="");
        Hyperedges instantiateInterface(const UniqueId deviceId, const std::string name="");
        Hyperedges instantiateBus(const UniqueId superId, const Hyperedges& interfaceIds, const std::string& name="");
        Hyperedges instantiateDevice(const Hyperedges& superId, const std::string& name="");
        Hyperedges instantiateInterface(const Hyperedges& deviceId, const std::string name="");
        Hyperedges instantiateBus(const Hyperedges& superId, const Hyperedges& interfaceIds, const std::string& name="");

        // Queries
        // NOTE: These return the subclasses of the corresponding main concepts
        Hyperedges deviceClasses(const std::string& name="");
        Hyperedges processorClasses(const std::string& name="");
        Hyperedges interfaceClasses(const std::string& name="");
        Hyperedges busClasses(const std::string& name="");
        // NOTE: These return the individuals of all the corresponding classes
        Hyperedges devices(const std::string& name="", const std::string& className="");
        Hyperedges processors(const std::string& name="", const std::string& className="");
        Hyperedges interfaces(const UniqueId deviceId="", const std::string& name="", const std::string& className=""); //< If a deviceId is given, only its interfaces are returned
        Hyperedges interfaces(const Hyperedges deviceIds, const std::string& name="", const std::string& className=""); //< If a deviceId is given, only its interfaces are returned
        Hyperedges busses(const std::string& name="", const std::string& className="");

        // Facts
        // NOTE: These will work for both, CLASSES and INSTANCES
        // Global functions for devices & interfaces
        Hyperedges has(const UniqueId deviceId, const UniqueId interfaceId);
        Hyperedges has(const Hyperedges& devices, const Hyperedges& interfaces);
        // Global functions for busses & interfaces
        Hyperedges connects(const UniqueId busId, const UniqueId interfaceId);
        Hyperedges connects(const Hyperedges& busses, const Hyperedges& interfaces);
        // Global function to encode processor/device relationship
        Hyperedges partOf(const Hyperedges& processorIds, const Hyperedges& deviceIds);
};

}
}

#endif
