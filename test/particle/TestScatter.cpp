#include "Ippl.h"
#include <random>

template<class PLayout>
struct Bunch : public ippl::ParticleBase<PLayout>
{

    Bunch(PLayout& playout)
    : ippl::ParticleBase<PLayout>(playout)
    {
        this->addAttribute(Q);
    }

    ~Bunch(){ }
    
    typedef ippl::ParticleAttrib<double> charge_container_type;
    charge_container_type Q;
};

int main(int argc, char *argv[]) {
    Ippl ippl(argc, argv);

    typedef ippl::ParticleSpatialLayout<double, 3> playout_type;
    typedef Bunch<playout_type> bunch_type;

    int pt = 64;
    ippl::Index I(pt);
    ippl::NDIndex<3> owned(I, I, I);

    ippl::e_dim_tag allParallel[3];    // Specifies SERIAL, PARALLEL dims
    for (unsigned int d=0; d<3; d++)
        allParallel[d] = ippl::PARALLEL;

    // all parallel layout, standard domain, normal axis order
    ippl::FieldLayout<3> layout(owned, allParallel);

    double dx = 1.0 / double(pt);
    ippl::Vector<double, 3> hx = {dx, dx, dx};
    ippl::Vector<double, 3> origin = {0, 0, 0};
    ippl::UniformCartesian<double, 3> mesh(owned, hx, origin);

    playout_type pl(layout, mesh);

    bunch_type bunch(pl);
    typedef ippl::Field<double, 3> field_type;

    field_type field;

    field.initialize(mesh, layout);

    int nRanks = Ippl::Comm->size();
    unsigned int nParticles = 4;//std::pow(256,3);
    
    if (nParticles % nRanks > 0) {
        if (Ippl::Comm->rank() == 0) {
            std::cerr << nParticles << " not a multiple of " << nRanks << std::endl;
        }
        return 0;
    }

    unsigned int nLoc = nParticles/nRanks;

    bunch.create(nLoc);

    std::mt19937_64 eng;
    eng.seed(42);
    eng.discard( nLoc * Ippl::Comm->rank());
    //std::uniform_real_distribution<double> unif(hx[0]/1.5, 1-(hx[0]/1.5));
    std::uniform_real_distribution<double> unif(0.3, 0.6);

    typename bunch_type::particle_position_type::HostMirror R_host = bunch.R.getHostMirror();
    for(unsigned int i = 0; i < nLoc; ++i) {
        ippl::Vector<double, 3> r = {unif(eng), unif(eng), unif(eng)};
        R_host(i) = r;
    }
    Kokkos::deep_copy(bunch.R.getView(), R_host);

    bunch.Q = 1.0;

    field = 0.0;

    scatter(bunch.Q, field, bunch.R);

    //field.write();

    // Check charge conservation
    try {
        double Total_charge_field = field.sum(0);

        std::cout << "Total charge in the field:" << Total_charge_field << std::endl;
        std::cout << "Total charge of the particles:" << bunch.Q.sum() << std::endl;
    } catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
