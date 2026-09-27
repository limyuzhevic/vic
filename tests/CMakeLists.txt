cmake_minimum_required(VERSION 3.16)
project(NLM_TESTS VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Create test executables for each test module
add_executable(test_neuron
    test_neuron.cpp
    test_config.cpp
    test_main.cpp
)

target_include_directories(test_neuron PRIVATE ../src)
target_link_libraries(test_neuron nlm_core)

add_executable(test_json_config
    test_json_config.cpp
    test_config.cpp
)

target_include_directories(test_json_config PRIVATE ../src)
target_link_libraries(test_json_config nlm_core)

add_executable(test_cmd
    test_cmd.cpp
)

target_include_directories(test_cmd PRIVATE ../src)
target_link_libraries(test_cmd nlm_core)

# Add tests to CTest
enable_testing()

add_test(NAME neuron_tests
    COMMAND test_neuron
)

add_test(NAME json_config_tests
    COMMAND test_json_config
)

add_test(NAME cmd_tests
    COMMAND test_cmd
)

# Set working directory for tests
set_tests_properties(neuron_tests cmd_tests json_config_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)