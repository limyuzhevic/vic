cmake_minimum_required(VERSION 3.16)

# NLM Enhanced Test Infrastructure
# Phase 7: Comprehensive Test Suite

project(NLM_Enhanced_Test_Infra VERSION 0.1.0 LANGUAGES CXX)

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

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Enhanced test infrastructure libraries
add_library(nlm_test_infrastructure STATIC
    base/test_base.cpp
    base/test_isolation.cpp
    utils/test_config.cpp
    utils/test_reporting.cpp
    fixtures/test_fixture.cpp
    macros/test_macros.hpp
    memory_leak_tests/test_memory_leak_detection.cpp
    enhanced_test_suite/enhanced_test_suite.cpp
)

target_include_directories(nlm_test_infrastructure PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/base>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/utils>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/fixtures>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/macros>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/memory_leak_tests>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/enhanced_test_suite>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_test_infrastructure PUBLIC nlm_core)

# Enhanced test executables
add_executable(nlm_enhanced_test_suite
    main_enhanced_test.cpp
)

target_link_libraries(nlm_enhanced_test_suite PRIVATE nlm_test_infrastructure)
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "// Enhanced Test Suite Main Implementation" > ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "#include \"test_infrastructure/enhanced_test_suite/enhanced_test_suite.hpp\"" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "#include <iostream>" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "int main() {" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    std::cout << \"=== NLM Enhanced Test Suite ===\" << std::endl;" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    
" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    // Create enhanced test suite configuration" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    EnhancedTestSuite suite(" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "        \"NLM Enhanced Test Suite Phase 7\"," >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "        \"Comprehensive test suite for NLM Phase 7: Enhanced Test Infrastructure\"," >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "        \"7\"" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    );" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    
" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    // Configure test options" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    suite.enable_memory_leak_tests = true;" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    suite.enable_stress_tests = true;" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    suite.enable_performance_tests = true;" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    suite.enable_integration_tests = true;" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    suite.enable_cross_platform_tests = true;" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    suite.enable_regression_tests = true;" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    suite.enable_edge_case_tests = true;" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    
" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    // Create enhanced test suite runner" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    EnhancedTestSuiteRunner runner(suite);" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    
" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    // Run enhanced test suite" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    runner.run_enhanced_test_suite();" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    
" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "    return 0;" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    COMMAND ${CMAKE_COMMAND} -E echo "}" >> ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating enhanced test suite main implementation"
    VERBATIM
)

add_custom_target(generate_enhanced_test_main DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp)

add_executable(nlm_test_enhanced
    ${CMAKE_CURRENT_BINARY_DIR}/main_enhanced_test.cpp
)

target_link_libraries(nlm_test_enhanced PRIVATE nlm_test_infrastructure)
add_test(NAME nlm_enhanced_test_suite COMMAND nlm_test_enhanced)

# Installation
install(TARGETS nlm_test_infrastructure
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES
    base/test_base.hpp
    base/test_isolation.hpp
    utils/test_config.hpp
    utils/test_reporting.hpp
    fixtures/test_fixture.hpp
    macros/test_macros.hpp
    memory_leak_tests/test_memory_leak_detection.hpp
    enhanced_test_suite/enhanced_test_suite.hpp
    enhanced_test_suite/enhanced_test_suite.cpp
    enhanced_test_suite/Enhanced_Test_Suite_Documentation.md
    DESTINATION include/nlm_test_infrastructure
)

# Summary
message(STATUS "NLM Enhanced Test Infrastructure configured successfully")
message(STATUS "  - Base test framework with utilities and patterns")
message(STATUS "  - Advanced test configuration system")
message(STATUS "  - Comprehensive test reporting system")
message(STATUS "  - Advanced test isolation system")
message(STATUS "  - Powerful test fixture system")
message(STATUS "  - Enhanced test macro system")
message(STATUS "  - Memory leak detection tests")
message(STATUS "  - Enhanced test suite runner")
message(STATUS "  - Documentation and examples")
message(STATUS "")
message(STATUS "Test targets available:")
message(STATUS "  nlm_test_infrastructure - Enhanced test infrastructure library")
message(STATUS "  nlm_enhanced_test_suite - Enhanced test suite executable")
message(STATUS "  nlm_test_enhanced - Alias for nlm_enhanced_test_suite")
message(STATUS "")
message(STATUS "For usage information, see Enhanced_Test_Suite_Documentation.md")