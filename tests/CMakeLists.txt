# Basic tests (Phase 2)
add_executable(test_brain test_brain.cpp)
target_link_libraries(test_brain nlm_core)

add_executable(test_synapse test_synapse.cpp)
target_link_libraries(test_synapse nlm_core)

add_executable(test_neuron test_neuron.cpp)
target_link_libraries(test_neuron nlm_core)

add_executable(test_stdp test_stdp.cpp)
target_link_libraries(test_stdp nlm_core)

add_executable(test_types test_types.cpp)
target_link_libraries(test_types nlm_core)

add_executable(test_clock test_clock.cpp)
target_link_libraries(test_clock nlm_core)

add_executable(test_config test_config.cpp)
target_link_libraries(test_config nlm_core)

add_executable(test_random test_random.cpp)
target_link_libraries(test_random nlm_core)

# Phase 6 integration tests

# Add Phase 6 test main executable
add_executable(test_phase6_integration tests/test_phase6_integration.cpp)
target_link_libraries(test_phase6_integration nlm_core)

add_test(NAME test_phase6_integration
    COMMAND test_phase6_integration)

# Existing Phase 6 tests in phase6/ directory

# Create library for Phase 6 tests
add_library(nlm_phase6_tests STATIC
    phase6/brain_test_common.hpp
    phase6/test_config.hpp
    phase6/test_brain_integration.cpp
    phase6/test_memory_integration.cpp
    phase6/test_prediction_integration.cpp
    phase6/test_neuromodulation_integration.cpp
    phase6/test_cognition_integration.cpp
    phase6/test_development_integration.cpp
    phase6/test_persistence_integration.cpp
    phase6/test_stress_integration.cpp
)

# Add test executables for each integration area
add_executable(test_brain_integration phase6/test_brain_integration.cpp)
target_link_libraries(test_brain_integration nlm_core nlm_phase6_tests)

add_executable(test_memory_integration phase6/test_memory_integration.cpp)
target_link_libraries(test_memory_integration nlm_core nlm_phase6_tests)

add_executable(test_prediction_integration phase6/test_prediction_integration.cpp)
target_link_libraries(test_prediction_integration nlm_core nlm_phase6_tests)

add_executable(test_neuromodulation_integration phase6/test_neuromodulation_integration.cpp)
target_link_libraries(test_neuromodulation_integration nlm_core nlm_phase6_tests)

add_executable(test_cognition_integration phase6/test_cognition_integration.cpp)
target_link_libraries(test_cognition_integration nlm_core nlm_phase6_tests)

add_executable(test_development_integration phase6/test_development_integration.cpp)
target_link_libraries(test_development_integration nlm_core nlm_phase6_tests)

add_executable(test_persistence_integration phase6/test_persistence_integration.cpp)
target_link_libraries(test_persistence_integration nlm_core nlm_phase6_tests)

add_executable(test_stress_integration phase6/test_stress_integration.cpp)
target_link_libraries(test_stress_integration nlm_core nlm_phase6_tests)

add_executable(test_main phase6/test_main.cpp)
target_link_libraries(test_main nlm_core nlm_phase6_tests)

# Add all Phase 6 tests to test suite
add_test(NAME test_brain_integration
    COMMAND test_brain_integration)

add_test(NAME test_memory_integration
    COMMAND test_memory_integration)

add_test(NAME test_prediction_integration
    COMMAND test_prediction_integration)

add_test(NAME test_neuromodulation_integration
    COMMAND test_neuromodulation_integration)

add_test(NAME test_cognition_integration
    COMMAND test_cognition_integration)

add_test(NAME test_development_integration
    COMMAND test_development_integration)

add_test(NAME test_persistence_integration
    COMMAND test_persistence_integration)

add_test(NAME test_stress_integration
    COMMAND test_stress_integration)

add_test(NAME test_main
    COMMAND test_main)

add_test(NAME test_phase6_integration
    COMMAND test_phase6_integration)