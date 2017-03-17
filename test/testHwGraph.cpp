#include "HwGraph.hpp"

#include <iostream>
#include <cassert>

int main(void)
{
    Hardware::Computational::Graph hwgraph;
    std::cout << "*** HW Graph instantaition ***" << std::endl;
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    assert(hwgraph.members().size() == 3);
    std::cout << "*** Adding dummy device ***" << std::endl;
    hwgraph.createDevice();
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    std::cout << "*** Adding dummy interface ***" << std::endl;
    hwgraph.createInterface();
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    std::cout << "*** Adding dummy bus ***" << std::endl;
    hwgraph.createBus();
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    std::cout << "*** Assigning dummy interface to device ***" << std::endl;
    Hyperedge *devQuery = (hwgraph.labelPartOf("Device"));
    Hyperedge *ifQuery = (hwgraph.labelPartOf("Interface"));
    std::cout << Hyperedge::serialize(devQuery) << std::endl;
    assert(devQuery->members().size() == 1);
    std::cout << Hyperedge::serialize(ifQuery) << std::endl;
    assert(ifQuery->members().size() == 1);
    hwgraph.has(devQuery->members("Device"), ifQuery->members("Interface"));
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    // So far so good. Now we want a second interface
    std::cout << "*** Adding another dummy interface ***" << std::endl;
    hwgraph.has(devQuery->members("Device"), hwgraph.createInterface());
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    std::cout << "*** Deleting queries ***" << std::endl;
    delete devQuery;
    delete ifQuery;
    std::cout << "*** Connect interfaces ***" << std::endl;
    Hyperedge *busQuery = (hwgraph.labelPartOf("Bus"));
    assert(busQuery->members().size() == 1);
    ifQuery = (hwgraph.labelPartOf("Interface")); // Gives us two interfaces :)
    assert(ifQuery->members().size() == 2);
    std::cout << Hyperedge::serialize(busQuery) << std::endl;
    std::cout << Hyperedge::serialize(ifQuery) << std::endl;
    hwgraph.connects(busQuery->members(), ifQuery->members());
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    // Ok, we created a device, two interfaces and a bus to connect them
    // Voila, we have our first network

    // Create a device EXTERNALLY and let it then HAVE an interface
    std::cout << "*** Pull externally defined device into domain when getting interfaces ***" << std::endl;
    Hyperedge *decoupled = Hyperedge::create("oO");
    hwgraph.has(decoupled, ifQuery->members());
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    
    // Create a sub-device ... that means that e.g. a x86 is a subdevice of processor
    std::cout << "*** Subclassing ***" << std::endl;
    Hyperedge* proc = hwgraph.createDevice("Processor");
    Hyperedge* x86 = hwgraph.createDevice("x86");
    proc->contains(x86);
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;

    std::cout << "*** Get all 0-Hyperedges or INSTANCES ***" << std::endl;
    std::cout << Hyperedge::serialize(hwgraph.cardinalityLessThanOrEqual()) << std::endl;
    
    std::cout << "*** Get all DIRECT supersets of x86 ***" << std::endl;
    for (auto super : x86->supers())
    {
        std::cout << super.second->label() << std::endl;
    }

    std::cout << "*** Get all supersets of x86 ***" << std::endl;
    Hyperedge *superQuery = x86->supersOf();
    std::cout << Hyperedge::serialize(superQuery) << std::endl;

    std::cout << "** Cycle detection in Hierarchy ***" << std::endl;
    Hyperedge *memberQuery = x86->membersOf();
    std::cout << Hyperedge::serialize(memberQuery) << std::endl;
    Hyperedge *intersection = superQuery->intersect(memberQuery);
    std::cout << Hyperedge::serialize(intersection) << std::endl;

    assert(intersection->cardinality() == 1);

    // TODO NEXT:
    // import/export
    // constraints/rules

    std::cout << "*** End of test ***" << std::endl;
    Hyperedge::cleanup();
}
