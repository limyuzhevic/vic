cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create Phase 2 test executable (backward compatibility)
add_executable(nlm_test_phase2
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
)

target_link_libraries(nlm_test_phase2 PRIVATE nlm_core)
target_compile_features(nlm_test_phase2 PRIVATE cxx_std_20)

# Create Phase 6 integration test executable
add_executable(nlm_test_phase6
    test_main.cpp
    test_phase6_integration.cpp
    test_system_connectivity.cpp
    test_memory_integration.cpp
    test_neuromodulation_integration.cpp
    test_checkpointing.cpp
    test_replay.cpp
)

target_link_libraries(nlm_test_phase6 PRIVATE nlm_core)
target_compile_features(nlm_test_phase6 PRIVATE cxx_std_20)

# Enable testing
enable_testing()

# Add tests to CTest
add_test(NAME nlm_test_phase2 COMMAND nlm_test_phase2)
add_test(NAME nlm_test_phase6 COMMAND nlm_test_phase6 "--phase6" "--memory" "--neuromodulation" "--checkpointing" "--replay")

# Installation targets
install(TARGETS nlm_test_phase2 nlm_test_phase6
    RUNTIME DESTINATION bin
)
