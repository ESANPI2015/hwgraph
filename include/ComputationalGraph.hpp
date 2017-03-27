#ifndef _HW_COMPUTATIONAL_GRAPH_HPP
#define _HW_COMPUTATIONAL_GRAPH_HPP

#include "Set.hpp"
#include "Relation.hpp"

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
      -> SEE SUBSUMPTION AND COMPOSITION RELATIONS
    * Should we introduce intermediate concepts like Ownership(has) or others which are shared amongst other domains?
*/

class Graph : public Set
{
    public:
        // Constructor which creates a Set and all standard subsets
        Graph(const std::string& label="Computational Hardware");

        // Factory functions
        Set* createDevice(const std::string& name="Device");
        Set* createInterface(const std::string& name="Interface");
        Set* createBus(const std::string& name="Bus");

        // Read access
        // NOTE: Return the Superclasses
        Set* device();
        Set* interface();
        Set* bus();

        // Queries
        // NOTE: Return true sets whose members are all of the same type/superclass
        Set* devices();
        Set* interfaces();
        Set* busses();
    
        // Devices & Interfaces
        bool has(Set* device, Set* interface);
        bool has(Set* device, Set::Sets interfaces);
        // The following is allowed because DEVICE CLASSES should share INTERFACE CLASSES
        bool has(Set::Sets devices, Set* interface);
        bool has(Set::Sets devices, Set::Sets interfaces);

        // Interfaces & Busses
        bool connects(Set* bus, Set* interface);
        bool connects(Set* bus, Set::Sets interfaces);
        bool connects(Set::Sets busses, Set* interface);
        bool connects(Set::Sets busses, Set::Sets interfaces);

    private:
        // Ids of superclasses created by this modelling domain
        unsigned _devId, _ifId, _busId;
};

}
}

#endif
