# Dynamics components

file(GLOB DYNAMICS_SOURCES
    "dynamics/NeuralDynamics.cpp"
    "dynamics/SpikeSystem.cpp"
)

add_library(nlm_dynamics STATIC ${DYNAMICS_SOURCES})

target_include_directories(nlm_dynamics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nlm_dynamics PUBLIC nlm_core nlm_brain)