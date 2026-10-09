cmake_minimum_required(VERSION 3.16)

# Test configuration
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Create test executable
add_executable(nlm_test
    test_main.cpp
    test_types.cpp
    test_config.cpp
    test_random.cpp
    test_clock.cpp
    test_neuron.cpp
    test_synapse.cpp
    test_stdp.cpp
    test_brain.cpp
    test_neuromodulation.cpp
    test_error_handling.cpp
    test_neuromodulation_enhanced.cpp
    test_vision_audio_internal.cpp
    test_prediction_system.cpp
    test_motor_system_enhanced.cpp
    test_development_system.cpp
    test_cognitive_system.cpp
    test_memory_system.cpp
    test_integration_systems.cpp
    test_phase4_comprehensive.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Python bindings test (if built)
if(TARGET pynlm)
    add_test(NAME python_bindings_test COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/../test_enhanced_bindings_comprehensive.py)
endif()
