#ifndef _HW_COMPUTATIONAL_GRAPH_HPP
#define _HW_COMPUTATIONAL_GRAPH_HPP

#include "Hyperedge.hpp"

namespace Hardware {
namespace Computational {

/*
    Some notes:
    We could have had some subclasses here, like Device, Interface & Bus
    but we are not going to do this, because we dont want to break DYNAMIC CLASS creation.
    That means that the classes Device, Interface & Bus are defined solely by there membership
    in the CREATED (by constructor) super-classes Devices, Interfaces & Busses.
    
    Open questions:
    * Does it makes sense to have 1-1, 1-N, N-1 and N-M versions of the relations?
    * Does it makes sense to use (N*1-1) and (N*1-M) versions of the N-1,N-M relations?
    * Would it be better to just use SIMPLE 2-hyperedges for the relations (everything based on 1-1 relations)?
      NOTE: This would better fit to databases with fixed tables like ID -> (ID, ID)
    * Can interfaces of type 0-hyperedge be owned by more than one device?
    * Subclassing versus Subgraphs: Specify this. How do we handle devices containing other devices?
*/

class Graph : public Hyperedge
{
    public:
        // Constructor which creates a Hyperedge and all standard subsets
        Graph(const std::string& label="Computational Hardware");

        // Factory functions
        Hyperedge* createDevice(const std::string& name="Device");
        Hyperedge* createInterface(const std::string& name="Interface");
        Hyperedge* createBus(const std::string& name="Bus");

        // Read access
        Hyperedge* devices();
        Hyperedge* interfaces();
        Hyperedge* busses();
    
        // Devices & Interfaces
        bool has(Hyperedge* device, Hyperedge* interface);
        bool has(Hyperedge* device, Hyperedge::Hyperedges interfaces);
        bool has(Hyperedge::Hyperedges devices, Hyperedge* interface);
        bool has(Hyperedge::Hyperedges devices, Hyperedge::Hyperedges interfaces);

        // Interfaces & Busses
        bool connects(Hyperedge* bus, Hyperedge* interface);
        bool connects(Hyperedge* bus, Hyperedge::Hyperedges interfaces);
        bool connects(Hyperedge::Hyperedges busses, Hyperedge* interface);
        bool connects(Hyperedge::Hyperedges busses, Hyperedge::Hyperedges interfaces);

    private:
        // Ids of superclasses created by this modelling domain
        unsigned _devId, _ifId, _busId;
};

}
}

#endif
