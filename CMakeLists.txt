# NLM - Comprehensive Build Configuration
# This CMake file configures the compilation of the complete NLM (Neural Learning Machine) system
# Including both error handling and the enhanced logging and monitoring system

cmake_minimum_required(VERSION 3.10)
project(NLM VERSION 6.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific configurations
if(WIN32)
    add_definitions(-DNLM_PLATFORM_WINDOWS)
elseif(APPLE)
    add_definitions(-DNLM_PLATFORM_MACOS)
elseif(UNIX)
    add_definitions(-DNLM_PLATFORM_LINUX)
endif()

# Find required packages
find_package(Threads REQUIRED)

# Configuration options
option(NLM_ENABLE_TESTS "Enable unit tests" ON)
option(NLM_ENABLE_DEMO "Enable demo programs" ON)
option(NLM_ENABLE_LOGGING "Enable enhanced logging and monitoring system" ON)
option(NLM_ENABLE_PERFORMANCE "Enable performance monitoring features" ON)
option(NLM_ENABLE_WEB_DASHBOARD "Enable web dashboard features" OFF)

# Core system library
add_library(nlm_core
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
)

# Error handling library
add_library(nlm_errors
    src/core/Error/ErrorHandler.cpp
    src/core/Error/ErrorCategory.cpp
    src/core/Error/ErrorIntegration.cpp
)

# Enhanced logging and monitoring library
if(NLM_ENABLE_LOGGING)
    add_library(nlm_logging
        src/core/Logger/Logger.cpp
        src/core/Logger/Logger.cpp
        src/core/Logger/AdvancedLogger.cpp
        src/core/Logger/PerformanceMonitor.cpp
        src/core/Logger/RuntimeMetricsCollector.cpp
        src/core/Logger/LogFormatter.cpp
        src/core/Logger/MonitorDashboard.cpp
        src/core/Logger/MetricCalculator.cpp
        src/core/Logger/LoggerIntegration.cpp
    )
    
    target_include_directories(nlm_logging PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core/Logger
    )
    
    target_link_libraries(nlm_logging nlm_core nlm_errors)
    
    # Performance monitor specific
    if(NLM_ENABLE_PERFORMANCE)
        target_compile_definitions(nlm_logging PRIVATE NLM_PERFORMANCE_MONITORING)
    endif()
    
    # Web dashboard specific
    if(NLM_ENABLE_WEB_DASHBOARD)
        target_compile_definitions(nlm_logging PUBLIC NLM_WEB_DASHBOARD)
        target_link_libraries(nlm_logging PUBLIC websocketpp)
    endif()
endif()

# Main NLM brain library
add_library(nlm_brain
    src/brain/Brain.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralRegion.cpp
    src/brain/AfferentConnection.cpp
    src/brain/EfferentConnection.cpp
)

# Link core dependencies
foreach(lib nlm_core nlm_errors)
    target_link_libraries(nlm_brain PUBLIC ${lib})
endforeach()

# Other NLM systems libraries
add_library(nlm_plasticity
    src/plasticity/STDP.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/StructuralPlasticity.cpp
)

add_library(nlm_neuromodulation
    src/neuromodulation/Dopamine.cpp
    src/neuromodulation/Curiosity.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/Neuromodulator.cpp
)

add_library(nlm_memory
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    src/memory/Memory.cpp
)

add_library(nlm_prediction
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
)

add_library(nlm_cognition
    src/cognition/NeuralPlanner.cpp
    src/cognition/ConceptFormation.cpp
)

add_library(nlm_development
    src/development/DevelopmentSystem.cpp
    src/development/Maturation.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
)

add_library(nlm_sensory
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/SensoryInput.cpp
    src/sensory/InternalSignals.cpp
)

add_library(nlm_motor
    src/motor/Action.cpp
    src/motor/MotorSystem.cpp
)

add_library(nlm_environment
    src/environment/Environment.cpp
    src/environment/Observation.cpp
)

# Add all subsystems to brain
foreach(lib nlm_plasticity nlm_neuromodulation nlm_memory nlm_prediction 
        nlm_cognition nlm_development nlm_sensory nlm_motor nlm_environment)
    target_link_libraries(nlm_brain PUBLIC ${lib})
endforeach()

# Integration layer
add_library(nlm_integration
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
)

# Main executable
add_executable(nlm_main
    src/main.cpp
)

target_link_libraries(nlm_main nlm_brain nlm_integration)
if(NLM_ENABLE_LOGGING)
    target_link_libraries(nlm_main nlm_logging)
endif()

# Demo program
if(NLM_ENABLE_DEMO)
    add_executable(nlm_phase6_demo
        src/experiments/Phase6IntegratedExperiment.cpp
    )
    
    target_link_libraries(nlm_phase6_demo nlm_brain nlm_integration)
    if(NLM_ENABLE_LOGGING)
        target_link_libraries(nlm_phase6_demo nlm_logging)
    endif()
endif()

# Enhanced logging demo
if(NLM_ENABLE_DEMO)
    add_executable(nlm_logging_demo
        demo_logging.cpp
    )
    
    target_link_libraries(nlm_logging_demo nlm_logging)
endif()

# Error handling test
add_executable(test_error_handling
    tests/test_error_handling_integration.cpp
)

target_link_libraries(test_error_handling nlm_errors)

# Logging system test
if(NLM_ENABLE_TESTS)
    add_executable(test_logging_integration
        tests/test_logging_integration.cpp
    )
    
    target_link_libraries(test_logging_integration nlm_logging)
endif()

# Set up testing
if(NLM_ENABLE_TESTS)
    enable_testing()
    add_test(NAME test_error_handling
        COMMAND test_error_handling
    )
    
    add_test(NAME test_logging_integration
        COMMAND test_logging_integration
    )
endif()

# Installation
install(TARGETS nlm_core nlm_errors nlm_brain nlm_plasticity
        nlm_neuromodulation nlm_memory nlm_prediction nlm_cognition
        nlm_development nlm_sensory nlm_motor nlm_environment nlm_integration
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY src/core/ src/brain/ src/plasticity/ src/neuromodulation/
              src/memory/ src/prediction/ src/cognition/ src/development/
              src/sensory/ src/motor/ src/environment/ src/experiments/
              DESTINATION include/nlm
)

if(NLM_ENABLE_LOGGING)
    install(TARGETS nlm_logging
        LIBRARY DESTINATION lib/nlm
        ARCHIVE DESTINATION lib/nlm
        RUNTIME DESTINATION bin
    )
    
    install(DIRECTORY src/core/Logger/
            DESTINATION include/nlm/Logger
    )
endif()

# Installation of headers
install(FILES
    include/nlm.hpp
    DESTINATION include/nlm
)

# Package configuration
include(GNUInstallDirs)

# Custom commands for documentation
set(DOC_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/docs)

# Create build information file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/NLMConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/nlm_config.cmake"
    @ONLY
)

# Add custom targets
add_custom_target(create_docs
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DOC_OUTPUT_DIR}
    COMMENT "Creating documentation directory"
)

add_custom_target(build_all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR} --target all
    COMMENT "Building all targets"
)

add_custom_target(run_tests
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR} --target test
    COMMENT "Running all tests"
)

# Summary message
message(STATUS "=== NLM Build Configuration ===")
message(STATUS "Project: NLM v${PROJECT_VERSION}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Error handling: enabled")
message(STATUS "Enhanced logging and monitoring: ${NLM_ENABLE_LOGGING}")
message(STATUS "Performance monitoring: ${NLM_ENABLE_PERFORMANCE}")
message(STATUS "Web dashboard: ${NLM_ENABLE_WEB_DASHBOARD}")
message(STATUS "Tests: ${NLM_ENABLE_TESTS}")
message(STATUS "Demo programs: ${NLM_ENABLE_DEMO}")
message(STATUS "===================================")