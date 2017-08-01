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
    
*/

class Graph;

class Graph : public CommonConceptGraph
{
    public:
        // Ids for identifiing devices, interfaces and busses
        static const unsigned DeviceId;
        static const unsigned InterfaceId;
        static const unsigned BusId;
        // Ids for identifiing IsA, HasA and Connects
        static const unsigned IsAId;
        static const unsigned HasAId;
        static const unsigned ConnectsId;

        // Constructor/Destructor
        Graph();
        Graph(CommonConceptGraph& A);
        ~Graph();

        // Creates the main concepts
        void createMainConcepts();

        // Factory functions
        // NOTE: These create classes, not individuals
        unsigned createDevice(const std::string& name="Device");
        unsigned createInterface(const std::string& name="Interface");
        unsigned createBus(const std::string& name="Bus");
        // NOTE: These create individuals from classes
        unsigned instantiateDevice(const unsigned superId, const std::string& name="");
        Hyperedges instantiateInterface(const unsigned deviceId, const std::string name="");
        unsigned instantiateBus(const unsigned superId, const Hyperedges& interfaceIds, const std::string& name="");

        // Queries
        // NOTE: These return the subclasses of the corresponding main concepts
        Hyperedges deviceClasses(const std::string& name="");
        Hyperedges interfaceClasses(const std::string& name="");
        Hyperedges busClasses(const std::string& name="");
        // NOTE: These return the individuals of all the corresponding classes
        Hyperedges devices(const std::string& name="", const std::string& className="");
        Hyperedges interfaces(const unsigned deviceId=0, const std::string& name="", const std::string& className=""); //< If a deviceId is given, only its interfaces are returned
        Hyperedges busses(const std::string& name="", const std::string& className="");

        // Facts
        // NOTE: These will work for both, CLASSES and INSTANCES
        // Global functions for devices & interfaces
        unsigned has(const unsigned deviceId, const unsigned interfaceId);
        unsigned has(const Hyperedges& devices, const Hyperedges& interfaces);
        // Global functions for busses & interfaces
        unsigned connects(const unsigned busId, const unsigned interfaceId);
        unsigned connects(const Hyperedges& busses, const Hyperedges& interfaces);
};

}
}

#endif
