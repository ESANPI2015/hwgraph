#include "ComputationalGraph.hpp"
#include "HyperedgeYAML.hpp"

#include <fstream>
#include <iostream>
#include <cassert>

int main(void)
{
    std::cout << "*** HW GRAPH TEST ***\n";

    Hardware::Computational::Graph hwgraph;

    std::cout << "> Create device\n";
    auto firstId = hwgraph.createDevice("First device");
    std::cout << "First id: " << firstId << "\n";

    std::cout << "> Create interface\n";
    auto secondId = hwgraph.createInterface("First interface");
    std::cout << "Second id: " << secondId << "\n";

    std::cout << "> Assign interface to device\n";
    hwgraph.has(firstId, secondId);

    std::cout << "> All sets" << std::endl;
    auto sets = hwgraph.find();
    for (auto setId : sets)
    {
        std::cout << setId << " " << hwgraph.get(setId)->label() << std::endl;
    }

    std::cout << "> Create 2nd device\n";
    auto thirdId = hwgraph.createDevice("Second device");
    std::cout << "Third id: " << thirdId << "\n";

    std::cout << "> Create 2nd interface\n";
    auto fourthId = hwgraph.createInterface("Second interface");
    std::cout << "Fourth id: " << fourthId << "\n";

    std::cout << "> Assign interface to device\n";
    hwgraph.has(thirdId, fourthId);

    std::cout << "> Connect the two interfaces\n";
    auto busId = hwgraph.createBus("MyBus");
    hwgraph.connects(busId, secondId);
    hwgraph.connects(busId, fourthId);

    std::cout << "> Query deviceClasses\n";
    auto devsId = hwgraph.deviceClasses();
    for (auto setId : devsId)
    {
        std::cout << setId << " " << hwgraph.get(setId)->label() << std::endl;
    }

    std::cout << "> Query interfaceClasses\n";
    auto ifsId = hwgraph.interfaceClasses();
    for (auto setId : ifsId)
    {
        std::cout << setId << " " << hwgraph.get(setId)->label() << std::endl;
    }

    std::cout << "> Query busClasses\n";
    auto bussesId = hwgraph.busClasses();
    for (auto setId : bussesId)
    {
        std::cout << setId << " " << hwgraph.get(setId)->label() << std::endl;
    }

    std::cout << "> Store hwgraph using YAML" << std::endl;

    YAML::Node test;
    test = static_cast<Hypergraph*>(&hwgraph);

    std::ofstream fout;
    fout.open("hwgraph.yml");
    if(fout.good()) {
        fout << test;
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();

    std::cout << "> Cleanup the hwgraph\n";
    auto allIds = hwgraph.find();
    for (auto id : allIds)
        hwgraph.destroy(id);
    hwgraph.createMainConcepts();

    std::cout << "> Create real world example\n";
    // Create classes
    // NOTE: The following reads as "Every mdaq2 is a DEVICE"
    unsigned mdaqSC = hwgraph.createDevice("mdaq2");
    unsigned pcSC = hwgraph.createDevice("PC");
    unsigned spineSC = hwgraph.createDevice("spine_board");
    unsigned convSC = hwgraph.createDevice("LVDS2USB");
    unsigned lvdsSC = hwgraph.createInterface("LVDS");
    unsigned usbSC = hwgraph.createInterface("USB");
    unsigned ndlcomSC = hwgraph.createBus("NDLCom");
    unsigned usbBusSC = hwgraph.createBus("USB");
    // Define bus domains
    hwgraph.connects(usbBusSC, usbSC);
    hwgraph.connects(ndlcomSC, lvdsSC);

    // Create interfaces
    // NOTE: The following reads as "Every mdaq2 HAS a LVDS1 and a LVDS2 interface of class LVDS"
    unsigned id1 = hwgraph.instantiateFrom(lvdsSC, "LVDS1");
    unsigned id2 = hwgraph.instantiateFrom(lvdsSC, "LVDS2");
    hwgraph.has(Hypergraph::Hyperedges{mdaqSC}, Hypergraph::Hyperedges{id1,id2});
    id1 = hwgraph.instantiateFrom(lvdsSC, "LVDS1");
    id2 = hwgraph.instantiateFrom(lvdsSC, "LVDS2");
    hwgraph.has(Hypergraph::Hyperedges{spineSC}, Hypergraph::Hyperedges{id1,id2});
    id1 = hwgraph.instantiateFrom(lvdsSC, "LVDS1");
    id2 = hwgraph.instantiateFrom(usbSC, "USB1");
    hwgraph.has(Hypergraph::Hyperedges{convSC}, Hypergraph::Hyperedges{id1,id2});
    id2 = hwgraph.instantiateFrom(usbSC, "/dev/ttyUSB0");
    hwgraph.has(pcSC, id2);

    // The hardware graph now contains the models of the devices we want to instantiate and connect in the following
    // We start with instantiating one device, the interfaces we want to connect
    // NOTE: The following reads as "There is a TEST BOARD of type mdaq2"
    unsigned mdaqId = hwgraph.instantiateDevice(mdaqSC, "TEST BOARD");
    unsigned spineId = hwgraph.instantiateDevice(spineSC);
    unsigned convId = hwgraph.instantiateDevice(convSC);
    unsigned laptopId = hwgraph.instantiateDevice(pcSC, "My Laptop");

    // We should now have the devices and the needed interfaces. It is time to connect them
    // NOTE: The following reads as "There is a bus of type NDLCom which connects a LVDS1 of TEST BOARD and LVDS1 of spine_board"
    hwgraph.instantiateBus(ndlcomSC, hwgraph.unite(hwgraph.interfaces(mdaqId, "LVDS1"),hwgraph.interfaces(spineId,"LVDS1")));
    hwgraph.instantiateBus(ndlcomSC, hwgraph.unite(hwgraph.interfaces(convId, "LVDS1"),hwgraph.interfaces(spineId,"LVDS2")));
    hwgraph.instantiateBus(usbBusSC, hwgraph.unite(hwgraph.interfaces(convId, "USB1"),hwgraph.interfaces(laptopId,"/dev/ttyUSB0")));

    // We could now make the specific instances and the network they form PART-OF some X. This X would then represent all occurences of this setting/network.

    std::cout << "> Store hwgraph using YAML" << std::endl;

    test = static_cast<Hypergraph*>(&hwgraph);
    fout.open("demo.yml");
    if(fout.good()) {
        fout << test;
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();


    std::cout << "*** TEST DONE ***\n";
}
