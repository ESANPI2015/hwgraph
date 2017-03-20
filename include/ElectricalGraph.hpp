#ifndef _HW_ELECTRICAL_GRAPH_HPP
#define _HW_ELECTRICAL_GRAPH_HPP

#include "Hyperedge.hpp"

namespace Hardware {
namespace Electrical {

class Graph : public Hyperedge
{
    public:
        // Constructor which creates a Hyperedge and all standard subsets
        Graph(const std::string& label="Electrical Hardware");

        // Factory functions
        Hyperedge* createDevice(const std::string& name="Device");
        Hyperedge* createPin(const std::string& name="Pin");
        Hyperedge* createWire(const std::string& name="Wire");

        // Read access
        Hyperedge* devices();
        Hyperedge* pins();
        Hyperedge* wires();
    
        // Devices & Pins
        bool has(Hyperedge* device, Hyperedge* pin);
        bool has(Hyperedge* device, Hyperedge::Hyperedges pins);
        bool has(Hyperedge::Hyperedges devices, Hyperedge* pin);
        bool has(Hyperedge::Hyperedges devices, Hyperedge::Hyperedges pins);

        // Pins & Wires
        bool connects(Hyperedge* wire, Hyperedge* pin);
        bool connects(Hyperedge* wire, Hyperedge::Hyperedges pins);
        bool connects(Hyperedge::Hyperedges wires, Hyperedge* pin);
        bool connects(Hyperedge::Hyperedges wires, Hyperedge::Hyperedges pins);

    private:
        // Ids of superclasses created by this modelling domain
        unsigned _devId, _pinId, _wireId;
};

}
}
