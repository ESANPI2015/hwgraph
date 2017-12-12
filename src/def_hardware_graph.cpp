#include "ComputationalGraph.hpp"
#include "HyperedgeYAML.hpp"

#include <fstream>
#include <sstream>

int main(void)
{
    // NOTE: Everything in here could be a constructor of a derived class!
    Hardware::Computational::Graph hwgraph;

    // Upper classes
    // TODO: We need a class which holds all UNMODELED stuff e.g. configurations/annotations etc.
    // In system_modelling this is modeled as a TREE OF PROPERTY NODES with DATATYPE and DATAVALUE nodes as leafs!!!
    // In our model such CONFIGURATIONs should be INTERFACES which already have DEFAULT VALUES (also a TODO!)
    // We could have an ALGORITHM which assigns CONFIGURATIONS for other ALGORITHMs :D
    auto devClassId = hwgraph.createDevice("Device");
    auto networkClassId = hwgraph.createDevice("flatten_computation"); // better: Network
    hwgraph.createDevice("Memory");
    auto peripheralClassId = hwgraph.createDevice("Peripheral");
    auto outputDevClassId = hwgraph.createDevice("OutputDevice");
    auto inputDevClassId = hwgraph.createDevice("InputDevice");

    auto procClassId = hwgraph.createProcessor("Processor");
    auto convClassId = hwgraph.createProcessor("Conventional");
    auto fpgaClassId = hwgraph.createProcessor("FPGA");

    auto ifClassId = hwgraph.createInterface("Interface");
    auto busClassId = hwgraph.createBus("Bus");

    // Upper relations
    auto devHasInterfaceId = hwgraph.relate(devClassId, ifClassId, "system_modelling::graph_basics::Has");
    auto busConnectsInterfaceId = hwgraph.relate(busClassId, ifClassId, "system_modelling::graph_basics::ConnectsTo");
    auto devIsPartOfId = hwgraph.relate(devClassId, networkClassId, "system_modelling::graph_basics::PartOf");
    auto devContainsId = hwgraph.relate(networkClassId, devClassId, "system_modelling::graph_basics::Contains");
    auto instanceOfTemplateId = hwgraph.relate(Hyperedges{Conceptgraph::IsConceptId}, Hyperedges{Conceptgraph::IsConceptId}, "system_modelling::graph_basics::InstanceOf");

    // Refer new relations to our common and/or special relations!
    hwgraph.subrelationOf(devHasInterfaceId, Hyperedges{Hardware::Computational::Graph::HasAId});
    hwgraph.subrelationOf(busConnectsInterfaceId, Hyperedges{Hardware::Computational::Graph::ConnectsId});
    // Special case: instanceOf in system_modelling is a mix of both, CommonConceptGraph::InstanceOf and CommonConceptGraph::IsA
    hwgraph.subrelationOf(instanceOfTemplateId, Hyperedges{CommonConceptGraph::InstanceOfId});
    hwgraph.subrelationOf(instanceOfTemplateId, Hyperedges{CommonConceptGraph::IsAId});
    hwgraph.subrelationOf(devIsPartOfId, Hyperedges{Hardware::Computational::Graph::PartOfId});
    // TODO: What do we do about Contains?

    // Now the language def by relations
    hwgraph.isA(inputDevClassId, peripheralClassId);
    hwgraph.isA(outputDevClassId, peripheralClassId);
    hwgraph.isA(convClassId, procClassId);
    hwgraph.isA(fpgaClassId, procClassId);

    // Is that all?
    // Now we can load stuff, right?

    // Store the setup in a YAML for inspection
    YAML::Node test;
    test = static_cast<Hypergraph*>(&hwgraph);

    std::ofstream fout;
    fout.open("def_hardware_graph.yml");
    if(fout.good()) {
        fout << test;
    } else {
        std::cout << "FAILED\n";
    }
    fout.close();
}
