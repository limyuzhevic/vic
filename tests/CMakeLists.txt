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
    ../src/core/ImprovementTests.cpp
)

target_link_libraries(nlm_test PRIVATE nlm_core nlm_agent)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Additional test executable for demonstrating improvements
add_executable(nlm_improvements_test
    ../src/core/ImprovementTests.cpp
)

target_link_libraries(nlm_improvements_test PRIVATE nlm_core nlm_agent)
target_compile_features(nlm_improvements_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_improvements_test COMMAND nlm_improvements_test)

# Python bindings build configuration
if(NLM_BUILD_PYTHON_BINDINGS)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    find_package(pybind11 CONFIG REQUIRED)
    set_target_properties(nlm_core nlm_agent nlm_world PROPERTIES
        POSITION_INDEPENDENT_CODE ON)
    pybind11_add_module(pynlm ../python/bindings.cpp)
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
    target_include_directories(pynlm PRIVATE ../src)
    install(TARGETS pynlm LIBRARY DESTINATION .)
endif()

# Benchmark executable
add_executable(nlm_benchmark
    ../src/benchmark.cpp
)

target_link_libraries(nlm_benchmark PUBLIC nlm_core)

# Install targets (future)
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase5 nlm_phase6 nlm_phase3_demo nlm_phase4_demo nlm_phase5_demo nlm_phase6_demo nlm_benchmark nlm_test nlm_improvements_test
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
