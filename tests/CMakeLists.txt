cmake_minimum_required(VERSION 3.16)
project(NLM_Tests LANGUAGES CXX)

# Tests require the core NLM library
add_executable(NLM_tests
    tests/TestMain.cpp
)

target_link_libraries(NLM_tests PUBLIC nlm_core)

# Add test target
enable_testing()
add_test(NAME NLM_Tests COMMAND NLM_tests)

# Copy configuration files
file(COPY configs/default.cfg DESTINATION ${CMAKE_CURRENT_BINARY_DIR} OPTIONAL)

# Add test dependencies if needed
# target_include_directories(NLM_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)