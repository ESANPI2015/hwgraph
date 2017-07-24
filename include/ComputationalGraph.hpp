#ifndef _HW_COMPUTATIONAL_GRAPH_HPP
#define _HW_COMPUTATIONAL_GRAPH_HPP

#include "Conceptgraph.hpp"

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

class Graph : public Conceptgraph
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
        Graph(Conceptgraph& A);
        ~Graph();

        // Creates the main concepts
        void createMainConcepts();

        // Factory functions
        unsigned createDevice(const std::string& name="Device");
        unsigned createInterface(const std::string& name="Interface");
        unsigned createBus(const std::string& name="Bus");

        // Queries
        Hyperedges devices(const std::string& name="");
        Hyperedges interfaces(const std::string& name="");
        Hyperedges busses(const std::string& name="");

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
