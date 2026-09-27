# ==============================================================================
# Advanced Features Build System
# ==============================================================================
cmake_minimum_required(VERSION 3.10)
project(NLM_Advanced LANGUAGES CXX)

# Find required packages
find_package(Threads REQUIRED)
find_package(CryptoPP REQUIRED)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Advanced features source files
set(ADVANCED_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/AutomatedDebugger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PerformanceOptimizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NeuralAnalyzer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ExperimentComparator.cpp
)

# Advanced features header files
set(ADVANCED_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/AutomatedDebugger.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PerformanceOptimizer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/NeuralAnalyzer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ExperimentComparator.hpp
)

# Create advanced library
add_library(nlm_advanced ${ADVANCED_SOURCES} ${ADVANCED_HEADERS})

# Target properties
target_include_directories(nlm_advanced PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(nlm_advanced PUBLIC cxx_std_17)

# Link libraries
target_link_libraries(nlm_advanced PUBLIC
    Threads::Threads
    CryptoPP::cryptopp
)

# Install target
install(TARGETS nlm_advanced
    EXPORT NLM_Advanced_Targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(FILES ${ADVANCED_HEADERS}
    DESTINATION include/nlm/debug
)

# Export targets
install(EXPORT NLM_Advanced_Targets
    FILE NLM_Advanced_Targets.cmake
    NAMESPACE nlm::
    DESTINATION lib/cmake/nlm_advanced
)

# Create automated debugger target
add_executable(automated_debugger ${CMAKE_CURRENT_SOURCE_DIR}/AutomatedDebuggerCLI.cpp)
target_link_libraries(automated_debugger PRIVATE nlm_advanced nlm_debugging)
install(TARGETS automated_debugger DESTINATION bin)

# Create performance optimizer target
add_executable(performance_optimizer ${CMAKE_CURRENT_SOURCE_DIR}/PerformanceOptimizerCLI.cpp)
target_link_libraries(performance_optimizer PRIVATE nlm_advanced nlm_performance)
install(TARGETS performance_optimizer DESTINATION bin)

# Create neural analyzer target
add_executable(neural_analyzer ${CMAKE_CURRENT_SOURCE_DIR}/NeuralAnalyzerCLI.cpp)
target_link_libraries(neural_analyzer PRIVATE nlm_advanced nlm_visualization)
install(TARGETS neural_analyzer DESTINATION bin)

# Create experiment comparator target
add_executable(experiment_comparator ${CMAKE_CURRENT_SOURCE_DIR}/ExperimentComparatorCLI.cpp)
target_link_libraries(experiment_comparator PRIVATE nlm_advanced nlm_experiments)
install(TARGETS experiment_comparator DESTINATION bin)

# Package generation
include(GNUInstallDirs)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/NLM_AdvancedConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_AdvancedConfig.cmake
    @ONLY
)

# Generate pkg-config file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/NLM_AdvancedPkgConfig.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_AdvancedPkgConfig.pc
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_AdvancedConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/NLM_AdvancedPkgConfig.pc
    DESTINATION lib/cmake/nlm_advanced
)

# Add dependencies
add_dependencies(nlm_advanced nlm_debugging nlm_performance nlm_visualization nlm_experiments)

# Build options
option(BUILD_ADVANCED "Build NLM Advanced Features" ON)
option(INSTALL_ADVANCED "Install NLM Advanced Features" ON)
option(INSTALL_ADVANCED_COMPONENTS "Install Advanced features components" ON)

if(NOT BUILD_ADVANCED)
    add_custom_target(advanced_off)
    add_dependencies(nlm_advanced_off advanced_off)
endif()
