#ifndef _HW_COMPUTATIONAL_GRAPH_HPP
#define _HW_COMPUTATIONAL_GRAPH_HPP

#include "Set.hpp"
#include "Relation.hpp"

namespace Hardware {
namespace Computational {

/*
    Some notes:
    We introduce some SUPERCLASSES as C++ classes AS WELL.
    But under the hood a IS-A relationship ensures that e.g. every subdevice is a device.
    
    Open questions:
    * Does it makes sense to have 1-1, 1-N, N-1 and N-M versions of the relations?
    * Does it makes sense to use (N*1-1) and (N*1-M) versions of the N-1,N-M relations?
    * Would it be better to just use SIMPLE 2-hyperedges for the relations (everything based on 1-1 relations)?
      NOTE: This would better fit to databases with fixed tables like ID -> (ID, ID)
    * How are the constraints on AGGREGATION (has)? Or, can an interface be owned by only one device?
    * Subclassing versus Subgraphs: Specify this. How do we handle devices containing other devices?
      -> SEE SUBSUMPTION AND COMPOSITION RELATIONS
    * Should we introduce intermediate concepts like Ownership(has) or others which are shared amongst other domains?
*/

class Interface : public Set
{
    public:
        // Gives a ptr to the interface superclass
        static Set* Superclass();
        
    private:
        // The classLabel is a string representing the class of interfaces
        static const std::string classLabel;
        static unsigned lastSuperclassId;
};

class Bus : public Set
{
    public:
        // Interfaces & Busses
        bool connects(Set* interface);
        bool connects(Set::Sets interfaces);

        // Gives a ptr to the interface superclass
        static Set* Superclass();

    private:
        // The classLabel is a string representing the class of interfaces
        static const std::string classLabel;
        static unsigned lastSuperclassId;
};

class Device : public Set
{
    public:
        // Gives a ptr to the interface superclass
        static Set* Superclass();

        // Devices & Interfaces
        bool has(Set* interface);
        bool has(Set::Sets interfaces);

        // Queries:
        // Returns a set containing all things which are related to us by a "has" relation
        Set* aggregates();

    private:
        // The classLabel is a string representing the class of interfaces
        static const std::string classLabel;
        static unsigned lastSuperclassId;
};

class Graph : public Set
{
    public:
        // Factory functions
        static Graph* create(const std::string& label="Computational Hardware");
        Device* createDevice(const std::string& name="Device");
        Interface* createInterface(const std::string& name="Interface");
        Bus* createBus(const std::string& name="Bus");

        // Queries
        // NOTE: Return true sets whose members are kinds of the superclasses
        Set* devices();
        Set* interfaces();
        Set* busses();

        // Global functions for devices & interfaces
        bool has(Set *device, Set *interface);
        bool has(Set *device, Set::Sets interfaces);
        bool has(Set::Sets devices, Set *interface);
        bool has(Set::Sets devices, Set::Sets interfaces);

        // Global functions for busses & interfaces
        bool connects(Set *bus, Set *interface);
        bool connects(Set *bus, Set::Sets interfaces);
        bool connects(Set::Sets busses, Set *interface);
        bool connects(Set::Sets busses, Set::Sets interfaces);

};

}
}

#endif
