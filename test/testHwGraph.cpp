#include "ComputationalGraph.hpp"
#include "HyperedgeYAML.hpp"

#include <fstream>
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
    assert(devQuery->members().size() == 1);
    std::cout << Hyperedge::serialize(ifQuery) << std::endl;
    assert(ifQuery->members().size() == 1);
    hwgraph.has(devQuery->members(), ifQuery->members());
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    // So far so good. Now we want a second interface
    std::cout << "*** Adding another dummy interface ***" << std::endl;
    hwgraph.has(devQuery->members(), hwgraph.createInterface());
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    std::cout << "*** Deleting queries ***" << std::endl;
    delete devQuery;
    delete ifQuery;
    std::cout << "*** Connect interfaces ***" << std::endl;
    Set *busQuery = hwgraph.busses();
    assert(busQuery->members().size() == 1);
    ifQuery = hwgraph.interfaces(); // Gives us two interfaces :)
    assert(ifQuery->members().size() == 2);
    std::cout << Hyperedge::serialize(busQuery) << std::endl;
    std::cout << Hyperedge::serialize(ifQuery) << std::endl;
    hwgraph.connects(busQuery->members(), ifQuery->members());
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    // Ok, we created a device, two interfaces and a bus to connect them
    // Voila, we have our first network

    // Create a device EXTERNALLY and let it then HAVE an interface
    std::cout << "*** Pull externally defined device into domain when getting interfaces ***" << std::endl;
    Set *decoupled = Set::create("oO");
    hwgraph.has(decoupled, ifQuery->members());
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    
    // Create a sub-device ... that means that e.g. a x86 is a subdevice of processor
    std::cout << "*** Subclassing ***" << std::endl;
    Set* proc = hwgraph.createDevice("Processor");
    Set* x86 = hwgraph.createDevice("x86");
    x86->isA(proc);
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;

    std::cout << "*** Get all supertypes of x86 ***" << std::endl;
    Relation *superQuery = x86->kindOf();
    std::cout << Hyperedge::serialize(superQuery) << std::endl;

    //std::cout << "** Cycle detection in Hierarchy ***" << std::endl;
    Set *superQuerySet = Set::create(Set::promote(superQuery->pointingTo()), "superclasses");
    Relation *subQuery = x86->superclassOf();
    Set *subQuerySet = Set::create(Set::promote(subQuery->pointingTo()), "subclasses");
    std::cout << Hyperedge::serialize(subQuery) << std::endl;
    Set *intersection = superQuerySet->intersect(subQuerySet);
    std::cout << intersection << " = { ";
    for (auto memberId : intersection->members())
    {
        std::cout << memberId << ", ";
    }
    std::cout << "}\n";

    assert(intersection->members().size() == 0);

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

    auto pred = pcb->predecessors();
    for (auto predId : pred->pointingTo())
    {
        std::cout << Hyperedge::find(predId) << std::endl;
    }
    delete pred;

    // import/export
    std::cout << "> Store everything to YAML file\n";
    std::ofstream fout;
    fout.open("test.yml");
    if(fout.good()) {
        fout << YAML::store(&hwgraph);
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();

    // TODO NEXT:
    // constraints/rules

    std::cout << "*** Final state ***" << std::endl;
    std::cout << Hyperedge::serialize(&hwgraph) << std::endl;
    std::cout << "*** End of test ***" << std::endl;
    Hyperedge::cleanup();
}
