# NLM Plasticity Layer - Synaptic modification rules

# Plasticity rule base
add_library(nlm_plasticity_rule STATIC
    src/plasticity/PlasticityRule.cpp
)

target_include_directories(nlm_plasticity_rule PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/plasticity>
    $<INSTALL_INTERFACE:include/plasticity>
)

# STDP (Spike-Timing Dependent Plasticity)
add_library(nlm_stdp STATIC
    src/plasticity/STDP.cpp
)

target_include_directories(nlm_stdp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/plasticity>
    $<INSTALL_INTERFACE:include/plasticity>
)

# Hebbian plasticity
add_library(nlm_hebbian STATIC
    src/plasticity/Hebbian.cpp
)

target_include_directories(nlm_hebbian PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/plasticity>
    $<INSTALL_INTERFACE:include/plasticity>
)

# Structural plasticity
add_library(nlm_structural_plasticity STATIC
    src/plasticity/StructuralPlasticity.cpp
)

target_include_directories(nlm_structural_plasticity PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/plasticity>
    $<INSTALL_INTERFACE:include/plasticity>
)

# Plasticity layer interface library
add_library(nlm_plasticity INTERFACE)
target_link_libraries(nlm_plasticity INTERFACE
    nlm_plasticity_rule
    nlm_stdp
    nlm_hebbian
    nlm_structural_plasticity
)

# Plasticity layer installation
install(TARGETS nlm_plasticity_rule nlm_stdp nlm_hebbian nlm_structural_plasticity
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/plasticity
    DESTINATION include/plasticity
    FILES_MATCHING PATTERN "*.hpp"
)

# Plasticity interface header
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/plasticity/include/nlm_plasticity.hpp
    DESTINATION include/plasticity
)