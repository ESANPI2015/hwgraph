#include "ComputationalGraph.hpp"

#include <iostream>
#include <cassert>

int main(void)
{
    Hardware::Computational::Graph hwgraph;
    std::cout << "*** HW Graph instantaition ***" << std::endl;
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    assert(hwgraph.pointedBy().size() == 3);
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
    Set *devQuery = hwgraph.devices();
    Set *ifQuery =  hwgraph.interfaces();
    std::cout << Hyperedge::serialize(devQuery) << std::endl;
    assert(devQuery->pointingTo().size() == 1);
    std::cout << Hyperedge::serialize(ifQuery) << std::endl;
    assert(ifQuery->pointingTo().size() == 1);
    hwgraph.has(Set::promote(devQuery->pointingTo()), Set::promote(ifQuery->pointingTo()));
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    // So far so good. Now we want a second interface
    std::cout << "*** Adding another dummy interface ***" << std::endl;
    hwgraph.has(Set::promote(devQuery->pointingTo()), hwgraph.createInterface());
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    std::cout << "*** Deleting queries ***" << std::endl;
    delete devQuery;
    delete ifQuery;
    std::cout << "*** Connect interfaces ***" << std::endl;
    Hyperedge *busQuery = hwgraph.busses();
    assert(busQuery->pointingTo().size() == 1);
    ifQuery = hwgraph.interfaces(); // Gives us two interfaces :)
    assert(ifQuery->pointingTo().size() == 2);
    std::cout << Hyperedge::serialize(busQuery) << std::endl;
    std::cout << Hyperedge::serialize(ifQuery) << std::endl;
    hwgraph.connects(Set::promote(busQuery->pointingTo()), Set::promote(ifQuery->pointingTo()));
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    // Ok, we created a device, two interfaces and a bus to connect them
    // Voila, we have our first network

    // Create a device EXTERNALLY and let it then HAVE an interface
    std::cout << "*** Pull externally defined device into domain when getting interfaces ***" << std::endl;
    Set *decoupled = Set::create("oO");
    hwgraph.has(decoupled, Set::promote(ifQuery->pointingTo()));
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    
    // Create a sub-device ... that means that e.g. a x86 is a subdevice of processor
    std::cout << "*** Subclassing ***" << std::endl;
    Set* proc = hwgraph.createDevice("Processor");
    Set* x86 = hwgraph.createDevice("x86");
    x86->isA(proc);
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;

    std::cout << "*** Get all supertypes of x86 ***" << std::endl;
    Set *superQuery = x86->kindOf();
    std::cout << Hyperedge::serialize(superQuery) << std::endl;

    //std::cout << "** Cycle detection in Hierarchy ***" << std::endl;
    Set *subQuery = x86->subclasses();
    std::cout << Hyperedge::serialize(subQuery) << std::endl;
    Set *intersection = superQuery->intersect(subQuery);
    std::cout << Hyperedge::serialize(intersection) << std::endl;

    assert(intersection->cardinality() == 0);

    std::cout << "*** Make a device out of other devices ***\n";
    /*
        This looks like this:
        
        ---------------------
        |               PCB |
       USB----------|       |
        |      ----USB---   |
        |      |   x86  |   |
        |      ----------   |
        |                   |
        ---------------------
    */
    Set *pcb = hwgraph.createDevice("PCB");
    x86->partOf(pcb);
    Set *usb = hwgraph.createInterface("USB");
    Set *usb1 = hwgraph.createInterface("USB");
    Set *usb2 = hwgraph.createInterface("USB");
    usb1->isA(usb);
    usb2->isA(usb);
    hwgraph.has(pcb, usb1);
    hwgraph.has(x86, usb2);
    Set *bus = hwgraph.createBus("USB");
    hwgraph.connects(bus, usb1);
    hwgraph.connects(bus, usb2);

    for (auto succIt : pcb->predecessors<Set>()->pointingTo())
    {
        std::cout << succIt.second << std::endl;
    }

    // TODO NEXT:
    // import/export
    // constraints/rules

    std::cout << "*** End of test ***" << std::endl;
    Hyperedge::cleanup();
}
