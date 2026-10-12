# CMake configuration for NLM (Neural Learning Machine)
# Project version and settings

cmake_minimum_required(VERSION 3.10)
project(NLM VERSION 2.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project options
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_BENCHMARKS "Build benchmarks" ON)
option(ENABLE_TESTS "Enable tests" ON)
option(ENABLE_PROFILING "Enable profiling" OFF)
option(ENABLE_OPTIMIZATIONS "Enable performance optimizations" ON)
option(ENABLE_MEMORY_TRACKING "Enable memory tracking" OFF)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Add source files
file(GLOB_RECURSE SRC_FILES
    "src/*.cpp"
    "src/**/*.cpp"
)

# Add include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

# Python bindings
if(BUILD_PYTHON_BINDINGS)
    find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
    
    # Create Python bindings module
    pybind11_add_module(pybindings python/bindings.cpp)
    target_link_libraries(pybindings PRIVATE ${PYTHON_LIBRARIES})
endif()

# Main NLM executable
add_executable(nlm src/main.cpp ${SRC_FILES})

# Target properties
target_compile_options(nlm PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

if(ENABLE_PROFILING)
    target_compile_options(nlm PRIVATE -pg)
    target_link_options(nlm PRIVATE -pg)
endif()

if(ENABLE_MEMORY_TRACKING)
    target_compile_options(nlm PRIVATE -fsanitize=address,undefined)
    target_link_options(nlm PRIVATE -fsanitize=address,undefined)
endif()

# Link libraries
target_link_libraries(nlm
    # Add NLM internal libraries here
)

# Install rules
install(TARGETS nlm
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Tests
if(BUILD_TESTS AND ENABLE_TESTS)
    enable_testing()
    
    # Add test executables
    add_test(NAME unit_tests
        COMMAND nlm --test unit
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/bin
    )
    
    add_test(NAME integration_tests
        COMMAND nlm --test integration
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/bin
    )
    
    # Add benchmark targets
    if(BUILD_BENCHMARKS)
        add_custom_target(benchmarks
            COMMAND $<TARGET_FILE:nlm> --benchmark
            DEPENDS nlm
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/bin
        )
    endif()
endif()

# Packaging
include(GNUInstallDirs)

install(FILES
    ${PROJECT_SOURCE_DIR}/README.md
    DESTINATION ${CMAKE_INSTALL_DATAROOT}/doc/nlm
)

install(FILES
    ${PROJECT_SOURCE_DIR}/configs/nlm.cfg
    DESTINATION ${CMAKE_INSTALL_DATAROOT}/config
)

# Development targets
add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test
    COMMENT "Run all tests"
)

add_custom_target(docs
    COMMAND doxygen Doxyfile
    COMMENT "Generate documentation"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

# Add subdirectories for components
add_subdirectory(src/core)
add_subdirectory(src/brain)
add_subdirectory(src/neuromodulation)
add_subdirectory(src/memory)
add_subdirectory(src/plasticity)
add_subdirectory(src/performance)
add_subdirectory(src/cognition)
add_subdirectory(src/prediction)
add_subdirectory(src/sensory)
add_subdirectory(src/motor)
add_subdirectory(src/environment)
add_subdirectory(src/experiments)
add_subdirectory(src/world)
add_subdirectory(src/dynamics)
add_subdirectory(src/visualization)
add_subdirectory(src/agent)
