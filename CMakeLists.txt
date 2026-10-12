cmake_minimum_required(VERSION 3.16)
project(NLM_Tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find necessary packages
find_package(GTest QUIET)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/core)
include_directories(${CMAKE_SOURCE_DIR}/src/agent)
include_directories(${CMAKE_SOURCE_DIR}/src/brain)
include_directories(${CMAKE_SOURCE_DIR}/src/memory)
include_directories(${CMAKE_SOURCE_DIR}/src/neuromodulation)
include_directories(${CMAKE_SOURCE_DIR}/src/cognition)
include_directories(${CMAKE_SOURCE_DIR}/src/sensory)
include_directories(${CMAKE_SOURCE_DIR}/src/motor)
include_directories(${CMAKE_SOURCE_DIR}/src/prediction)
include_directories(${CMAKE_SOURCE_DIR}/src/development)
include_directories(${CMAKE_SOURCE_DIR}/src/plasticity)
include_directories(${CMAKE_SOURCE_DIR}/src/dynamics)
include_directories(${CMAKE_SOURCE_DIR}/src/experiments)
include_directories(${CMAKE_SOURCE_DIR}/src/world)
include_directories(${CMAKE_SOURCE_DIR}/src/core/Types)

# Test files
file(GLOB TEST_SOURCES
    "tests/*.cpp"
)

# Create test executable
add_executable(nlm_tests ${TEST_SOURCES})

# Link libraries (assuming NLM is already built as a static library)
# You may need to adjust library names and paths
# target_link_libraries(nlm_tests nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Add test to ctest
enable_testing()
add_test(NAME nlm_tests COMMAND nlm_tests)

# Custom target for building and running tests
add_custom_target(run_all_tests
    COMMAND ${CMAKE_PROJECT_NAME}_tests
    DEPENDS nlm_tests
    COMMENT "Running all NLM tests"
)

# Summary
message(STATUS "NLM Tests configured")
message(STATUS "Test files: ${TEST_SOURCES}")