set (_SRCS
    )

set (_HDRS
    ParticleAttribBase.h
    ParticleAttrib.h
    ParticleAttrib.hpp
    ParticleBase.h
    ParticleBase.hpp
    ParticleBC.h
    ParticleLayout.h
    ParticleLayout.hpp
    ParticleSpatialLayout.h
    ParticleSpatialLayout.hpp
    ParticleSpatialOverlapLayout.h
    ParticleSpatialOverlapLayout.hpp
    )

include_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

add_ippl_sources (${_SRCS})
add_ippl_headers (${_HDRS})

install (FILES ${_HDRS} DESTINATION include/Particle)

# vi: set et ts=4 sw=4 sts=4:

# Local Variables:
# mode: cmake
# cmake-tab-width: 4
# indent-tabs-mode: nil
# require-final-newline: nil
# End:

