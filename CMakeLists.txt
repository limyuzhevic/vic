# NLM (Neural Learning Machine) - CMake Configuration
# Improved build system with better configuration management, installation, and cross-platform support

cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "Neural Learning Machine - Experimental Artificial Developmental Brain")

# Set project properties
set(PROJECT_NAME "NLM")
set(PROJECT_DESCRIPTION "Neural Learning Machine - Experimental Artificial Developmental Brain")
set(PROJECT_URL "https://github.com/example/nlm")
set(PROJECT_LICENSE "MIT")

# C++20 standard with modern C++ features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configurations with optimized settings
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel" CACHE STRING "Available build types" FORCE)

# Compiler flags and warnings
if(MSVC)
    # MSVC compiler flags
    add_compile_options(/W4 /permissive- /Zc:__cplusplus)
    add_compile_options(
        /bigobj                    # Large object files for many templates
        / EHsc                     # Exceptions only in C++
        / FP: fast               # Fast floating-point math
        / GS-                     # Buffer security checks
        / we4242                   # 'variable': conversion from 'type1' to 'type1', possible loss of data
        / we4254                   # 'operator bool': conversion from 'type1' to 'type1' possible loss of data
        / we4263                   # 'function': member function does not override any base class virtual member function
        / we4265                   # 'classname': constructor, which was expected to throw an exception, does not throw any (or with catch which prevents exceptions)
        / we4266                   # 'function': static member function does not override any base class virtual member function
        / we4267                   # 'variable': member initialization is not covered by a base-class virtual constructor
        / we4270                   # 'enum': inconsistent initializer for default member initializer
        / we4289                   # nonstandard extension used: 'variable': loop control variable declared in the for-loop is used outside the for-loop scope
        / we4296                   # 'function': expression is always 'value' when true and always 'other' when false
        / we4311                   # 'variable': pointer truncation from 'type1' to 'type2'
        / we4312                   # 'variable': pointer truncation from 'type1' to 'type2'
        / we4315                   # 'variable': pointer truncation from 'type1' to 'type2'
        / we4407                   # 'variable': conversion from 'HMODULE *' to 'HMODULE *' of greater alignment
        / we4458                   # 'function': member function 'function' is not virtual; inheritance of member function 'function' from class 'class' may not capture objects of derived classes
        / we4529                   # 'function': implicit copy constructor required
        / we4541                     # 'function': overriding operator delete function after virtual destructor
        / we4547                     # 'function': 'function': warning calling 'function' on 'argument' after NULL check
        / we4549                     # 'function': 'function': 'function' called after NULL check
        / we4555                     # 'expression': _asm statement contains 'expression'
        / we4619                     # 'function': conflict with a previous declaration of 'function' in the class
        / we4640                     # 'function': reference member initializer is not a constant expression
        / we4626                     # 'function': recursive call with the same arguments
        / we4668                     # 'macro': not a macro; use #pragma once or #pragma comment
        / we4738                     # 'function': inconsistent Microsoft C++ language standard
        / we4826                     # 'variable': potential loss of data if you convert between different precisions
    )
else()
    # GCC/Clang compiler flags
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_compile_options(
        -fPIC                       # Position-independent code for shared libraries
        -fvisibility=hidden        # Hide symbols by default
        -fvisibility-inlines-hidden # Hide inline function symbols
        -fsigned-char              # Signed char by default
        -fexceptions                # Exception support
        -fpermissive                # Relaxed C++ conformance
        -march=native              # Optimize for current CPU
        -mtune=native              # Tune for current CPU
        -flto                       # Link-time optimization
        -ffast-math                # Fast math optimizations
        -frtti                     # Run-time type information
        -fstrict-aliasing          # Strict aliasing rules
        -fwrapv                    # Signed overflow wraps
        -fstack-protector-strong   # Strong stack protection
        -D_FORTIFY_SOURCE=3        # Buffer security checks
    )
endif()

# Source directory definition
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(NLM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(NLM_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/configs")
set(NLM_DOC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/docs")
set(NLM_SCRIPTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/scripts")

# Platform-specific settings
if(WIN32)
    # Windows-specific settings
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)  # Export all symbols for Windows DLL
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/win32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/win32)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/win32)
elseif(APPLE)
    # macOS-specific settings
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/macos)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/macos)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/macos)
else()
    # Linux/Unix-like systems
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/linux)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/linux)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/linux)
endif()

# Performance optimization flags based on build type
string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER)
if(BUILD_TYPE_LOWER STREQUAL "debug")
    add_compile_options(-g -O0 -DDEBUG -DNDEBUG)
    add_definitions(-DNLM_DEBUG)
elseif(BUILD_TYPE_LOWER STREQUAL "release")
    add_compile_options(-O3 -DNDEBUG)
    add_definitions(-DNLM_RELEASE)
elseif(BUILD_TYPE_LOWER STREQUAL "reldebug")
    add_compile_options(-O2 -g -DNDEBUG)
    add_definitions(-DNLM_RELEASE_DEBUG)
elseif(BUILD_TYPE_LOWER STREQUAL "minsize")
    add_compile_options(-Os -DNDEBUG)
    add_definitions(-DNLM_MIN_SIZE)
endif()

# Core library with comprehensive source organization
add_library(nlm_core STATIC
    # Core infrastructure
    src/core/Types/Types.cpp
    src/core/Config/Config.cpp
    src/core/Random/Random.cpp
    src/core/Logger/Logger.cpp
    src/core/SimulationClock/SimulationClock.cpp
    
    # Brain computation
    src/brain/Neuron.cpp
    src/brain/Synapse.cpp
    src/brain/NeuralPopulation.cpp
    src/brain/NeuralRegion.cpp
    src/brain/Brain.cpp
    
    # Neural dynamics
    src/dynamics/NeuralDynamics.cpp
    src/dynamics/SpikeSystem.cpp
    
    # Plasticity mechanisms
    src/plasticity/PlasticityRule.cpp
    src/plasticity/Hebbian.cpp
    src/plasticity/STDP.cpp
    src/plasticity/StructuralPlasticity.cpp
    
    # Development systems
    src/development/DevelopmentSystem.cpp
    src/development/Synaptogenesis.cpp
    src/development/Pruning.cpp
    src/development/Maturation.cpp
    
    # Neuromodulation
    src/neuromodulation/Neuromodulator.cpp
    src/neuromodulation/Reward.cpp
    src/neuromodulation/PredictionError.cpp
    src/neuromodulation/Novelty.cpp
    src/neuromodulation/Curiosity.cpp
    
    # Memory systems
    src/memory/WorkingMemory.cpp
    src/memory/EpisodicMemory.cpp
    src/memory/SemanticMemory.cpp
    src/memory/ProceduralMemory.cpp
    src/memory/AssociativeMemory.cpp
    src/memory/NeuralWorkingMemory.cpp
    src/memory/NeuralEpisodicMemory.cpp
    
    # Cognitive functions
    src/cognition/ConceptFormation.cpp
    src/cognition/NeuralPlanner.cpp
    
    # Sensory systems
    src/sensory/SensoryInput.cpp
    src/sensory/Vision.cpp
    src/sensory/Audio.cpp
    src/sensory/InternalSignals.cpp
    
    # Prediction systems
    src/prediction/PredictionSystem.cpp
    src/prediction/NeuralPrediction.cpp
    
    # Motor systems
    src/motor/MotorSystem.cpp
    
    # Environment and interaction
    src/environment/Environment.cpp
    src/environment/Observation.cpp
    src/environment/Action.cpp
    
    # Experiment framework
    src/experiments/Experiment.cpp
    src/experiments/ExperimentRunner.cpp
    src/experiments/Metrics.cpp
    
    # Visualization interfaces
    src/visualization/VisualizationInterface.cpp
)

# Core library include directories
target_include_directories(nlm_core PUBLIC
    $<BUILD_INTERFACE:${NLM_SOURCE_DIR}>
    $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Core library compile definitions
target_compile_definitions(nlm_core PUBLIC
    -DNLM_CORE
    -DNLM_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    -DNLM_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    -DNLM_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

# Agent system (body, sensory percept, agent-brain interface)
add_library(nlm_agent STATIC
    src/agent/SensoryPercept.cpp
    src/agent/AgentBrain.cpp
)
target_include_directories(nlm_agent PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_agent PUBLIC nlm_core)

# World system (environment, objects, simple world)
add_library(nlm_world STATIC
    src/world/SimpleWorld.cpp
)
target_include_directories(nlm_world PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_world PUBLIC nlm_core)

# Phase 3 experiments
add_library(nlm_phase3 STATIC
    src/experiments/Phase3Experiment.cpp
)
target_include_directories(nlm_phase3 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)

# Phase 4 experiments
add_library(nlm_phase4 STATIC
    src/experiments/Phase4Experiment.cpp
)
target_include_directories(nlm_phase4 PUBLIC ${NLM_SOURCE_DIR})
target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)

# Main executable
add_executable(nlm
    src/main.cpp
)
target_link_libraries(nlm PUBLIC nlm_core)

# Phase 3 demo executable
add_executable(nlm_phase3_demo
    src/experiments/Phase3Demo.cpp
)
target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)

# Phase 4 demo executable
add_executable(nlm_phase4_demo
    src/experiments/Phase4Demo.cpp
)
target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)

# Testing framework
target_include_directories(nlm_core PUBLIC ${NLM_SOURCE_DIR})
enable_testing()

# Add tests directory as subdirectory
add_subdirectory(tests)

# Installation configuration
install(DIRECTORY ${NLM_INCLUDE_DIR}/ DESTINATION include/nlm FILES_MATCHING PATTERN "*.hpp")

# Install configuration files
install(DIRECTORY ${NLM_CONFIG_DIR}/ DESTINATION share/nlm/configs
    FILES_MATCHING PATTERN "*.cfg"
)

# Install documentation
install(DIRECTORY ${NLM_DOC_DIR}/ DESTINATION share/nlm/docs
    FILES_MATCHING PATTERN "*.md"
    FILES_MATCHING PATTERN "*.pdf"
)

# Install scripts
install(DIRECTORY ${NLM_SCRIPTS_DIR}/ DESTINATION bin
    FILES_MATCHING PATTERN "*.sh"
    FILES_MATCHING PATTERN "*.bat"
    FILES_MATCHING PATTERN "*.ps1"
)

# Installation targets
install(TARGETS nlm nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4 nlm_phase3_demo nlm_phase4_demo
    EXPORT NLMTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Package configuration for find_package support
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/NLMConfigVersion.cmake
    DESTINATION lib/cmake/NLM
)

# Package export for CMake package management
install(EXPORT NLMTargets
    FILE NLMTargets.cmake
    NAMESPACE nlm::
    DESTINATION lib/cmake/NLM
)

# CPack configuration for packaging
if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "NLM_${PROJECT_VERSION}_Registry")
    set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_NAME} ${PROJECT_VERSION}")
    set(CPACK_NSIS_CONTACT "noreply@example.com")
elseif(APPLE)
    set(CPACK_GENERATOR "ZIP;DragNDrop")
    set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-macos")
elseif(UNIX)
    set(CPACK_GENERATOR "TGZ;DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "NLM Team")
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Neural Learning Machine - Experimental Brain Simulator")
endif()

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "NLM Research")
set(CPACK_PACKAGE_DESCRIPTION "Neural Learning Machine - Phase 2 Real Neural Computation")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "noreply@example.com")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "NLM ${PROJECT_VERSION}")

set(CPACK_RESOURCE_FILE_LICENSE "${NLM_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${NLM_SOURCE_DIR}/README.md")

set(CPACK_PACKAGING_INSTALL_SCRIPT "${NLM_SCRIPTS_DIR}/install.cmake")

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6")

set(CPACK_MACOSX_PACKAGE_VERSION "${PROJECT_VERSION}")

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

set(CPACK_COMPONENTS ALL)

# Component definitions
install_component("nlm_core" DESCRIPTION "Core neural computation library")
install_component("nlm_agent" DESCRIPTION "Agent system (sensory, motor, interaction)")
install_component("nlm_world" DESCRIPTION "World simulation environment")
install_component("nlm_phase3" DESCRIPTION "Phase 3 experience-action-consequence learning")
install_component("nlm_phase4" DESCRIPTION "Phase 4 cognitive capabilities")
install_component("nlm" DESCRIPTION "Main NLM executable")
install_component("nlm_phase3_demo" DESCRIPTION "Phase 3 demonstration executable")
install_component("nlm_phase4_demo" DESCRIPTION "Phase 4 demonstration executable")

# Create documentation target
set(DOCS_SOURCE_DIR ${NLM_DOC_DIR})
if(EXISTS ${DOCS_SOURCE_DIR}/index.md)
    add_custom_target(docs
        COMMAND ${CMAKE_COMMAND} -E echo "Documentation build not configured yet. Please use Doxygen or other documentation tools."
        COMMENT "Building documentation..."
    )
endif()

# Create run target for executables
add_custom_target(run-nlm COMMAND nlm DEPENDS nlm COMMENT "Running NLM main executable..." VERBATIM)
add_custom_target(run-nlm-phase3-demo COMMAND nlm_phase3_demo ${nlm_phase3_demo_ARGS} DEPENDS nlm_phase3_demo COMMENT "Running NLM Phase 3 demo..." VERBATIM)
add_custom_target(run-nlm-phase4-demo COMMAND nlm_phase4_demo ${nlm_phase4_demo_ARGS} DEPENDS nlm_phase4_demo COMMENT "Running NLM Phase 4 demo..." VERBATIM)

# Add testing targets for common operations
add_custom_target(test-all
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ctest
    COMMENT "Running all tests..."
    VERBATIM
)

add_custom_target(quick-test
    COMMAND ${CMAKE_CTEST_COMMAND} -R "test_config|test_neuron|test_synapse|test_stdp|test_brain"
    DEPENDS ctest
    COMMENT "Running quick test suite..."
    VERBATIM
)

# Print build summary
message(STATUS "NLM Build Configuration Summary:")
message(STATUS "  Project: ${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Core library target: nlm_core")
message(STATUS "  Executable targets: nlm, nlm_phase3_demo, nlm_phase4_demo")
message(STATUS "  Installation prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Configuration directory: ${NLM_CONFIG_DIR}")
message(STATUS "  Documentation directory: ${NLM_DOC_DIR}")

# Print component availability
message(STATUS "")
message(STATUS "Available components:")
foreach(component nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
    if(TARGET ${component})
        message(STATUS "  ${component}")
    endif()
endforeach()
