file (RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
message (STATUS "Adding unit tests found in ${_relPath}")

include_directories (
    ${CMAKE_SOURCE_DIR}/src
    ${GTEST_INCLUDE_DIRS}
)

link_directories (
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GTEST_LIBRARY_DIRS}
    ${Boost_LIBRARY_DIRS}
    ${Kokkos_DIR}/..
)

add_executable (PIC PIC.cpp)
target_link_libraries (
    PIC
    ippl
    ${MPI_CXX_LIBRARIES}
    ${GTEST_BOTH_LIBRARIES}
    boost_timer
)

# vi: set et ts=4 sw=4 sts=4:

# Local Variables:
# mode: cmake
# cmake-tab-width: 4
# indent-tabs-mode: nil
# require-final-newline: nil
# End: