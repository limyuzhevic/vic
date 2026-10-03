cmake_minimum_required(VERSION 3.16)
project(single_neuron_learning VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project configuration
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_EXAMPLES "Build example executables" ON)
option(ENABLE_VISUALIZATION "Enable visualization output" OFF)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include NLM libraries
find_package(nlm REQUIRED)

# Main example executable
if(BUILD_EXAMPLES)
    add_executable(single_neuron_learning
        src/single_neuron_learning.cpp
        src/learning_utils.cpp
        src/learning_analysis.cpp
        src/neural_dynamics.cpp
        src/plasticity_models.cpp
    )
    
    target_link_libraries(single_neuron_learning PRIVATE nlm::nlm_core)
    target_include_directories(single_neuron_learning PRIVATE
        ${NLM_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    
    # Add compiler definitions
    target_compile_definitions(single_neuron_learning PRIVATE
        "NLM_EXPERIMENT_NAME=SingleNeuronLearning"
        "NLM_DEFAULT_NEURON_COUNT=50"
        "NLM_DEFAULT_STIMULI_COUNT=200"
        "NLM_DEFAULT_LEARNING_RATE=0.001"
    )
    
    # Set target properties
    set_target_properties(single_neuron_learning PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        DEBUG_POSTFIX "_d"
        RELEASE_POSTFIX ""
    )
    
    # Add executable to tests if tests are enabled
    if(BUILD_TESTS)
        add_test(NAME single_neuron_learning_basic
            COMMAND single_neuron_learning
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        
        # Add visualization test if visualization is enabled
        if(ENABLE_VISUALIZATION)
            add_test(NAME single_neuron_learning_visualization
                COMMAND ${CMAKE_BINARY_DIR}/bin/single_neuron_learning --visualize
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )
        endif()
    endif()
endif()

# Unit tests
if(BUILD_TESTS)
    add_executable(learning_test
        test/test_neuron_dynamics.cpp
        test/test_plasticity.cpp
        test/test_learning_curves.cpp
        test/test_configuration.cpp
    )
    
    target_link_libraries(learning_test PRIVATE nlm::nlm_core GTest::gtest GTest::gtest_main)
    target_include_directories(learning_test PRIVATE
        ${NLM_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${GTEST_INCLUDE_DIR}
    )
    
    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(learning_test)
endif()

# Installation
install(TARGETS single_neuron_learning
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Installation for development mode
install(DIRECTORY include/ DESTINATION include)

# Package generation
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/single_neuron_learning-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config/single_neuron_learning-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/single_neuron_learning-config.cmake
    INSTALL_DESTINATION lib/cmake/single_neuron_learning
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/single_neuron_learning-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/single_neuron_learning-config-version.cmake
    DESTINATION lib/cmake/single_neuron_learning
)