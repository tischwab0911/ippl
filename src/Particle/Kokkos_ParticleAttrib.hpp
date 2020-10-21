
namespace ippl {

    template<typename T, class... Properties>
    void ParticleAttrib<T, Properties...>::create(size_t n) {
        size_t current = this->size();
        this->resize(current + n);
    }


    template<typename T, class... Properties>
    void ParticleAttrib<T, Properties...>::destroy(boolean_view_type invalidIndex,
                                                   Kokkos::View<int*> newIndex, size_t n) {
        Kokkos::View<T*> temp("temp", n);
        Kokkos::parallel_for("ParticleAttrib::destroy()",
                             size(),
                             KOKKOS_CLASS_LAMBDA(const size_t i)
                             {
                                 if ( !invalidIndex(i) ) {
                                    temp(newIndex(i)) = this->operator()(i);
                                 }
                             });
        this->resize(n);
        Kokkos::deep_copy(*this, temp);
    }


    template<typename T, class... Properties>
    template <unsigned Dim, class M, class C, class PT>
    void ParticleAttrib<T, Properties...>::scatter(Field<T,Dim,M,C>& f,
                                                   const ParticleAttrib< Vector<PT,Dim>, Properties... >& pp)
    const
    {
        // single LField only
        typename Field<T, Dim, M, C>::LField_t::view_type view = f(0).getView();

        const M& mesh = f.get_mesh();

        using vector_type = typename M::vector_type;
        using value_type  = typename ParticleAttrib<T, Properties...>::value_type;

        const vector_type& dx = mesh.getMeshSpacing();
        const vector_type& origin = mesh.getOrigin();
        const vector_type invdx = 1.0 / dx;


        Kokkos::parallel_for("ParticleAttrib::scatter",
                             size(),
                             KOKKOS_CLASS_LAMBDA(const size_t idx)
                             {
                                 // find nearest grid point
                                 vector_type l = (pp(idx) - origin) * invdx + 0.5;
                                 Vector<int, Dim> index = l;
                                 Vector<double, Dim> whi = l - index;
                                 Vector<double, Dim> wlo = 1.0 - whi;

                                 const int i = index[0] + 1;
                                 const int j = index[1] + 1;
                                 const int k = index[2] + 1;

                                 // scatter
                                 const value_type& val = this->operator()(idx);
                                 Kokkos::atomic_add(&view(i-1, j-1, k-1), wlo[0] * wlo[1] * wlo[2] * val);
                                 Kokkos::atomic_add(&view(i-1, j-1, k  ), wlo[0] * wlo[1] * whi[2] * val);
                                 Kokkos::atomic_add(&view(i-1, j,   k-1), wlo[0] * whi[1] * wlo[2] * val);
                                 Kokkos::atomic_add(&view(i-1, j,   k  ), wlo[0] * whi[1] * whi[2] * val);
                                 Kokkos::atomic_add(&view(i,   j-1, k-1), whi[0] * wlo[1] * wlo[2] * val);
                                 Kokkos::atomic_add(&view(i,   j-1, k  ), whi[0] * wlo[1] * whi[2] * val);
                                 Kokkos::atomic_add(&view(i,   j,   k-1), whi[0] * whi[1] * wlo[2] * val);
                                 Kokkos::atomic_add(&view(i,   j,   k  ), whi[0] * whi[1] * whi[2] * val);
                             });
    }


    template<typename T, class... Properties>
    template <unsigned Dim, class M, class C, typename P2>
    void ParticleAttrib<T, Properties...>::gather(const Field<T, Dim, M, C>& f,
                                                  const ParticleAttrib<Vector<P2, Dim>, Properties...>& pp)
    {
        // single LField only
        const typename Field<T, Dim, M, C>::LField_t::view_type view = f(0).getView();

        const M& mesh = f.get_mesh();

        using vector_type = typename M::vector_type;
        using value_type  = typename ParticleAttrib<T, Properties...>::value_type;

        const vector_type& dx = mesh.getMeshSpacing();
        const vector_type& origin = mesh.getOrigin();
        const vector_type invdx = 1.0 / dx;


        Kokkos::parallel_for("ParticleAttrib::gather",
                             size(),
                             KOKKOS_CLASS_LAMBDA(const size_t idx)
                             {
                                 // find nearest grid point
                                 vector_type l = (pp(idx) - origin) * invdx + 0.5;
                                 Vector<int, Dim> index = l;
                                 Vector<double, Dim> whi = l - index;
                                 Vector<double, Dim> wlo = 1.0 - whi;

                                 const int i = index[0] + 1;
                                 const int j = index[1] + 1;
                                 const int k = index[2] + 1;

                                 // scatter
                                 value_type& val = this->operator()(idx);
                                 val = wlo[0] * wlo[1] * wlo[2] * view(i-1, j-1, k-1)
                                     + wlo[0] * wlo[1] * whi[2] * view(i-1, j-1, k  )
                                     + wlo[0] * whi[1] * wlo[2] * view(i-1, j,   k-1)
                                     + wlo[0] * whi[1] * whi[2] * view(i-1, j,   k  )
                                     + whi[0] * wlo[1] * wlo[2] * view(i,   j-1, k-1)
                                     + whi[0] * wlo[1] * whi[2] * view(i,   j-1, k  )
                                     + whi[0] * whi[1] * wlo[2] * view(i,   j,   k-1)
                                     + whi[0] * whi[1] * whi[2] * view(i,   j,   k  );
                             });
    }



    /*
     * Non-class function
     *
     */


    template<typename P1, unsigned Dim, class M, class C, typename P2, class... Properties>
    inline
    void scatter(const ParticleAttrib<P1, Properties...>& attrib, Field<P1, Dim, M, C>& f,
                 const ParticleAttrib<Vector<P2, Dim>, Properties...>& pp)
    {
        attrib.scatter(f, pp);
    }


    template<typename P1, unsigned Dim, class M, class C, typename P2, class... Properties>
    inline
    void gather(ParticleAttrib<P1, Properties...>& attrib, const Field<P1, Dim, M, C>& f,
                const ParticleAttrib<Vector<P2, Dim>, Properties...>& pp)
    {
        attrib.gather(f, pp);
    }
}