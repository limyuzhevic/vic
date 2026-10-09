# Experiments CMakeLists.txt
# Contains experimental features and research code

# Experiment: Neural plasticity
add_executable(experiment_plasticity
    plasticity_experiment.cpp
)

target_link_libraries(experiment_plasticity PRIVATE nlm_core nlm_agent)

# Experiment: Memory consolidation
add_executable(experiment_memory
    memory_consolidation.cpp
)

target_link_libraries(experiment_memory PRIVATE nlm_core)

# Experiment: Neuromodulation
add_executable(experiment_neuromodulation
    neuromodulation_experiment.cpp
)

target_link_libraries(experiment_neuromodulation PRIVATE nlm_core nlm_agent)

# Install experiments
install(TARGETS experiment_plasticity experiment_memory experiment_neuromodulation
    RUNTIME DESTINATION bin/experiments
)
