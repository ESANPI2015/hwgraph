#ifndef _HW_COMPUTATIONAL_GRAPH_HPP
#define _HW_COMPUTATIONAL_GRAPH_HPP

#include "SetSystem.hpp"
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
    * How to we encode and enforce constraints on relations?
    * 

    The underlying concept is a system of sets linked by relations
*/

class Graph;

class Interface : public Set
{
    public:
        /* Constructor */
        Interface(const unsigned id, const std::string& label="")
        : Set(id,label)
        {}

        // This is holding a label to identify instances of this class via (* -- isA --> superclassLabel) patterns
        static const std::string superclassLabel;
};

class Bus : public Set
{
    public:
        /* Constructor */
        Bus(const unsigned id, const std::string& label="")
        : Set(id,label)
        {}

        // This is holding a label to identify instances of this class via (* -- isA --> superclassLabel) patterns
        static const std::string superclassLabel;

        // Interfaces & Busses
        bool connects(Graph* graph, const unsigned id);
};

class Device : public Set
{
    public:
        /* Constructor */
        Device(const unsigned id, const std::string& label="")
        : Set(id,label)
        {}

        // This is holding a label to identify instances of this class via (* -- isA --> superclassLabel) patterns
        static const std::string superclassLabel;

        // Devices & Interfaces
        bool has(Graph* graph, const unsigned id);
};

class Graph : public SetSystem
{
    public:
        // Constructor/Destructor
        Graph();
        ~Graph();

        // Returns the id of an representative of the classes
        unsigned deviceClass();
        unsigned interfaceClass();
        unsigned busClass();

        // Factory functions
        unsigned createDevice(const std::string& name="Device");
        unsigned createInterface(const std::string& name="Interface");
        unsigned createBus(const std::string& name="Bus");

        // Queries
        // NOTE: Return true sets whose members are kinds of the superclasses
        unsigned devices();
        unsigned interfaces();
        unsigned busses();

        // Global functions for devices & interfaces
        unsigned has(const unsigned deviceId, const unsigned interfaceId);

        // Global functions for busses & interfaces
        unsigned connects(const unsigned busId, const unsigned interfaceId);
};

}
}

#endif
