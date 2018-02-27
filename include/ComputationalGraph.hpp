#ifndef _SIMPLE_COMPUTATIONAL_HW_HPP
#define _SIMPLE_COMPUTATIONAL_HW_HPP

#include <HardwareComputationalNetwork.hpp>

namespace Simple {

/*
  This domain is a very simple domain consisting of
  * Processors
    - CONVENTIONAL
    - FPGA
  * Interfaces
    - NDLCOM
    - SOURCE
    - SINK
  * Links
*/

class Computation : public Hardware::Computational::Network
{
    public:
        Computation();
        Computation(const Hypergraph& base);
        ~Computation();

        std::string domainSpecificExport(const UniqueId& uid);
        bool domainSpecificImport(const std::string& serialized);

        std::string labelFromIdAndName(const unsigned id, const std::string& name);
        std::string labelFromIdAndName(const std::string& id, const std::string& name);
        unsigned idFromLabel(const std::string& label);
        std::string nameFromLabel(const std::string& label);

    protected:
        void setupMetaModel();
};

}
#endif
