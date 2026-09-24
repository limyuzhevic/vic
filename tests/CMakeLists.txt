cmake_minimum_required(VERSION 3.20)

# Test configuration for NLM

# Find NLM core library
find_package(NLM REQUIRED)

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
)

target_link_libraries(nlm_test PRIVATE NLM::nlm_core)
target_compile_features(nlm_test PRIVATE cxx_std_20)

# Enable testing
enable_testing()
add_test(NAME nlm_test COMMAND nlm_test)

# Add test properties
target_include_directories(nlm_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Custom test properties
if(NLM_ENABLE_SANITIZERS AND NOT MSVC)
    target_compile_options(nlm_test PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(nlm_test PRIVATE -fsanitize=address)
endif()
