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
    protected:
        void setupMetaModel();
};

}
#endif
