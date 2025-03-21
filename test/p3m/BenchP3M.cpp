constexpr unsigned Dim = 3;
using T = double;

#include "Ippl.h"
#include "../alpine/datatypes.h"
#include "../alpine/P3MBenchManager.hpp"

#include "Utility/IpplTimings.h"

int main(int argc, char* argv[]){
    ippl::initialize(argc, argv);
    {
        Inform msg(argv[0]);
        Inform msg2all(argv[0], INFORM_ALL_NODES);
        
        const double boxlen         = 0.01;
        const double beam_rad       = 0.005;
        const unsigned int np       = 1e7;             // 10 million particles             
        // const double rcut           = 0.0003125;    // 8 * PM grid spacing
        // const double alpha          = 2./rcut;      // choice motivated by B. Ulmer
        const double dt             = 2.15623e-13;
        const double eps            = 0;
        const unsigned int nt       = 1000;
        const double m_e            = 1;
        const double q_e            = 1;
        const double focus_strength = 1.5;

        int arg = 1;
        Vector_t<int, Dim> nr;
        for (unsigned d = 0; d < Dim; d++) {
            nr[d] = std::atoi(argv[arg++]);
        }

        const double rcut = 3.0 * (boxlen/(double)nr[0]);
        const double alpha = 2./rcut;
        
        P3M3DBenchManager<T, Dim> manager(np, nt, dt, nr, rcut, alpha, beam_rad, focus_strength, boxlen);
        manager.pre_run();
    }
    ippl::finalize();
    
    return 0;
}