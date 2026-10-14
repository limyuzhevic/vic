cmake_minimum_required(VERSION 3.16)

project(nlm_tests LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Test framework source files
set(TEST_SOURCES
    test_main.cpp
    TestFramework.hpp
)

# Test framework executable
add_executable(nlm_tests ${TEST_SOURCES})

# Link with NLM core libraries
target_link_libraries(nlm_tests PUBLIC nlm_core)

# Add tests directory to include path
target_include_directories(nlm_tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# Add test configuration files
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_config.cfg
     ${CMAKE_CURRENT_SOURCE_DIR}/config_test.cfg
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
     FILES_MATCHING PATTERN "*.cfg"
)

# Add test data files
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_cases/
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/test_cases
     FILES_MATCHING PATTERN "*.txt"
     PATTERN "*.json"
     PATTERN "*.yaml"
)

# Install test framework
install(TARGETS nlm_tests
    RUNTIME DESTINATION bin
)

# Add test cases
file(GLOB TEST_CASE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_cases/*.cpp
)

foreach(test_file ${TEST_CASE_FILES})
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(nlm_test_${test_name} ${test_file})
    target_link_libraries(nlm_test_${test_name} PUBLIC nlm_core)
    target_include_directories(nlm_test_${test_name} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
    )
    add_test(NAME nlm_test_${test_name} COMMAND nlm_test_${test_name})
endforeach()
