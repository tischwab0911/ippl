set (_SRCS
        ParticleInteractionBase.h
        TruncatedGreenParticleInteraction.h
        TruncatedGreenParticleInteraction.hpp
)

set (_HDRS
)

include_directories (
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_ippl_sources (${_SRCS})
add_ippl_headers (${_HDRS})

install (FILES ${_HDRS} DESTINATION include/ShortRange)

# vi: set et ts=4 sw=4 sts=4:

# Local Variables:
# mode: cmake
# cmake-tab-width: 4
# indent-tabs-mode: nil
# require-final-newline: nil
# End:

