# Experiments components

file(GLOB EXPERIMENTS_SOURCES
    "experiments/Experiment.cpp"
    "experiments/ExperimentRunner.cpp"
    "experiments/Metrics.cpp"
)

add_library(nlm_experiments STATIC ${EXPERIMENTS_SOURCES})

target_include_directories(nlm_experiments PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nlm_experiments PUBLIC nlm_core nlm_brain nlm_dynamics nlm_plasticity nlm_neuromodulation nlm_memory nlm_cognition nlm_sensory nlm_prediction nlm_motor nlm_environment)