# Plasticity components

file(GLOB PLASTICITY_SOURCES
    "plasticity/PlasticityRule.cpp"
    "plasticity/Hebbian.cpp"
    "plasticity/STDP.cpp"
    "plasticity/StructuralPlasticity.cpp"
)

add_library(nlm_plasticity STATIC ${PLASTICITY_SOURCES})

target_include_directories(nlm_plasticity PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nlm_plasticity PUBLIC nlm_core nlm_dynamics)