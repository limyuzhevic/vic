cmake_minimum_required(VERSION 3.16)

# NLM (熙然) - Neural Learning Machine
# Advanced AI/ML project with modular architecture

project(NLM VERSION 0.1.0 LANGUAGES CXX)

# ==============================================================================
# BUILD CONFIGURATION
# ==============================================================================

# C++20 standard with modern C++ features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration with all common build types
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Comprehensive build type options
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# ==============================================================================
# COMPREHENSIVE WARNING SYSTEM
# ==============================================================================

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_compile_options(-Wshadow -Wformat=2 -Wconversion -Wnull-dereference)
    add_compile_options(-Wlogical-op -Wduplicated-cond -Wduplicated-branches)
    add_compile_options(-Wcovered-switch-default -Wswitch-enum)
    add_compile_options(-Wdocumentation -Wdocumentation-header)
endif()

# ==============================================================================
# PROJECT STRUCTURE AND MODULAR COMPONENTS
# ==============================================================================

# Set source directory
set(NLM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(NLM_INCLUDE_DIR "${NLM_SOURCE_DIR}")
set(NLM_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/configs")
set(NLM_DOC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/docs")

# ==============================================================================
# COMPONENT OPTIONS
# ==============================================================================

option(BUILD_CORE "Build core neural simulation components" ON)
option(BUILD_BRAIN "Build brain components (neurons, synapses, regions)" ON)
option(BUILD_DYNAMICS "Build neural dynamics systems" ON)
option(BUILD_PLASTICITY "Build plasticity rules (STDP, Hebbian, structural)" ON)
option(BUILD_DEVELOPMENT "Build development systems" ON)
option(BUILD_NEUROMODULATION "Build neuromodulation systems" ON)
option(BUILD_MEMORY "Build memory systems (working, episodic, etc.)" ON)
option(BUILD_COGNITION "Build cognitive systems (planning, concept formation)" ON)
option(BUILD_SENSORY "Build sensory systems" ON)
option(BUILD_MOTOR "Build motor systems" ON)
option(BUILD_ENVIRONMENT "Build environment simulation" ON)
option(BUILD_PREDICTION "Build prediction systems" ON)
option(BUILD_EXPERIMENTS "Build experiment frameworks" ON)
option(BUILD_AGENT "Build agent systems" ON)
option(BUILD_WORLD "Build world simulation systems" ON)
option(BUILD_VISUALIZATION "Build visualization interfaces" ON)
option(BUILD_PYTHON_BINDINGS "Build Python bindings via PyBind11" OFF)

# Demo and example options
option(BUILD_MAIN_DEMO "Build main demonstration executable" ON)
option(BUILD_PHASE3_DEMO "Build Phase 3 demo executable" ON)
option(BUILD_PHASE4_DEMO "Build Phase 4 demo executable" ON)
option(BUILD_PHASE6_DEMO "Build Phase 6 integration demo" ON)
option(BUILD_TESTS "Build test executables" ON)

# ==============================================================================
# AUTOMATIC SOURCE FILE DISCOVERY
# ==============================================================================

macro(nlm_collect_sources)
    file(GLOB_RECURSE source_files "src/*.cpp")
    
    # Filter out unwanted files (like test files, benchmark files)
    list(FILTER source_files EXCLUDE REGEX ".*test.*\.cpp$|.*benchmark.*\.cpp$|^.*\.cpp$" )
    
    # Separate files by component
    foreach(src_file ${source_files})
        if(src_file MATCHES "src/core/|src/logger/|src/random/|src/simulationclock")
            list(APPEND CORE_SOURCES ${src_file})
        elseif(src_file MATCHES "src/brain/|src/neuron/|src/synapse/|src/neuralpopulation/|src/neuralregion")
            list(APPEND BRAIN_SOURCES ${src_file})
        elseif(src_file MATCHES "src/dynamics/|src/spikesystem")
            list(APPEND DYNAMICS_SOURCES ${src_file})
        elseif(src_file MATCHES "src/plasticity/|src/hebbian/|src/stdp/|src/structuralplasticity")
            list(APPEND PLASTICITY_SOURCES ${src_file})
        elseif(src_file MATCHES "src/development/|src/synaptogenesis/|src/pruning/|src/maturation")
            list(APPEND DEVELOPMENT_SOURCES ${src_file})
        elseif(src_file MATCHES "src/neuromodulation/|src/dopamine/|src/reward/|src/predictionerror/|src/novelty/|src/curiosity")
            list(APPEND NEUROMODULATION_SOURCES ${src_file})
        elseif(src_file MATCHES "src/memory/|src/workingmemory/|src/episodicmemory/|src/semanticmemory/|src/proceduralmemory/|src/associativememory/|src/neuralworkingmemory/|src/neuralepisodicmemory")
            list(APPEND MEMORY_SOURCES ${src_file})
        elseif(src_file MATCHES "src/cognition/|src/conceptformation/|src/neuralplanner")
            list(APPEND COGNITION_SOURCES ${src_file})
        elseif(src_file MATCHES "src/sensory/|src/vision/|src/audio/|src/internalsignals")
            list(APPEND SENSORY_SOURCES ${src_file})
        elseif(src_file MATCHES "src/motor/|src/motorsystem")
            list(APPEND MOTOR_SOURCES ${src_file})
        elseif(src_file MATCHES "src/environment/|src/observation/|src/action")
            list(APPEND ENVIRONMENT_SOURCES ${src_file})
        elseif(src_file MATCHES "src/prediction/|src/neuralprediction")
            list(APPEND PREDICTION_SOURCES ${src_file})
        elseif(src_file MATCHES "src/experiments/|src/experimentrunner/|src/experiment")
            list(APPEND EXPERIMENTS_SOURCES ${src_file})
        elseif(src_file MATCHES "src/agent/|src/sensorypercept/|src/agentbrain")
            list(APPEND AGENT_SOURCES ${src_file})
        elseif(src_file MATCHES "src/world/|src/simpleworld")
            list(APPEND WORLD_SOURCES ${src_file})
        elseif(src_file MATCHES "src/visualization/|src/visualizationinterface")
            list(APPEND VISUALIZATION_SOURCES ${src_file})
        else()
            list(APPEND OTHER_SOURCES ${src_file})
        endif()
    endforeach()
    
    # Apply component filters
    if(NOT BUILD_CORE)
        list(FILTER CORE_SOURCES EXCLUDE)
    endif()
    if(NOT BUILD_BRAIN)
        list(FILTER BRAIN_SOURCES EXCLUDE)
    endif()
    if(NOT BUILD_DYNAMICS)
        list(FILTER DYNAMICS_SOURCES EXCLUDE)
    endif()
    if(NOT BUILD_PLASTICITY)
        list(FILTER PLASTICITY_SOURCES EXCLUDE)
    endif()
    if(NOT BUILD_DEVELOPMENT)
        list(FILTER DEVELOPMENT_SOURCES EXCLUDE)
    endif()
    if(NOT BUILD_NEUROMODULATION)
        list(FILTER NEUROMODULATION_SOURCES EXCLUDE)
    endif()
    if(NOT BUILD_MEMORY)
        list(FILTER MEMORY_SOURCES EXCLUDE)
    endif()
    if(NOT BUILD_COGNITION)
        list(FILTER COGNITION_SOURCES EXCLUDE)
    endif()
    if(NOT BUILD_SENSORY)
        list(FILTER SENSORY_SOURCES EXCLUDE)
    endif()
    if(NOT BUILD_MOTOR)
        list(FILTER MOTOR_SOURCES EXCLUDE)
    endif()
    if(NOT BUILD_ENVIRONMENT)
        list(FILTER ENVIRONMENT_SOURCES EXCLUDE)
    endif()
    if(NOT BUILD_PREDICTION)
        list(FILTER PREDICTION_SOURCES EXCLUDE)
    endif()
    if(NOT BUILD_EXPERIMENTS)
        list(FILTER EXPERIMENTS_SOURCES EXCLUDE)
    endif()
    if(NOT BUILD_AGENT)
        list(FILTER AGENT_SOURCES EXCLUDE)
    endif()
    if(NOT BUILD_WORLD)
        list(FILTER WORLD_SOURCES EXCLUDE)
    endif()
    if(NOT BUILD_VISUALIZATION)
        list(FILTER VISUALIZATION_SOURCES EXCLUDE)
    endif()
    
    # Summary
    message(STATUS "NLM Source Configuration:")
    message(STATUS "  Core sources: ${CORE_SOURCES}")
    message(STATUS "  Brain sources: ${BRAIN_SOURCES}")
    message(STATUS "  Dynamics sources: ${DYNAMICS_SOURCES}")
    message(STATUS "  Plasticity sources: ${PLASTICITY_SOURCES}")
    message(STATUS "  Development sources: ${DEVELOPMENT_SOURCES}")
    message(STATUS "  Neuromodulation sources: ${NEUROMODULATION_SOURCES}")
    message(STATUS "  Memory sources: ${MEMORY_SOURCES}")
    message(STATUS "  Cognition sources: ${COGNITION_SOURCES}")
    message(STATUS "  Sensory sources: ${SENSORY_SOURCES}")
    message(STATUS "  Motor sources: ${MOTOR_SOURCES}")
    message(STATUS "  Environment sources: ${ENVIRONMENT_SOURCES}")
    message(STATUS "  Prediction sources: ${PREDICTION_SOURCES}")
    message(STATUS "  Experiment sources: ${EXPERIMENTS_SOURCES}")
    message(STATUS "  Agent sources: ${AGENT_SOURCES}")
    message(STATUS "  World sources: ${WORLD_SOURCES}")
    message(STATUS "  Visualization sources: ${VISUALIZATION_SOURCES}")
    
    # Combine all sources
    set(ALL_SOURCES
        ${CORE_SOURCES} ${BRAIN_SOURCES} ${DYNAMICS_SOURCES} ${PLASTICITY_SOURCES}
        ${DEVELOPMENT_SOURCES} ${NEUROMODULATION_SOURCES} ${MEMORY_SOURCES}
        ${COGNITION_SOURCES} ${SENSORY_SOURCES} ${MOTOR_SOURCES}
        ${ENVIRONMENT_SOURCES} ${PREDICTION_SOURCES} ${EXPERIMENTS_SOURCES}
        ${AGENT_SOURCES} ${WORLD_SOURCES} ${VISUALIZATION_SOURCES}
    )
    
    # Set final source lists
    set(NLM_CORE_SOURCES ${CORE_SOURCES})
    set(NLM_BRAIN_SOURCES ${BRAIN_SOURCES})
    set(NLM_DYNAMICS_SOURCES ${DYNAMICS_SOURCES})
    set(NLM_PLASTICITY_SOURCES ${PLASTICITY_SOURCES})
    set(NLM_DEVELOPMENT_SOURCES ${DEVELOPMENT_SOURCES})
    set(NLM_NEUROMODULATION_SOURCES ${NEUROMODULATION_SOURCES})
    set(NLM_MEMORY_SOURCES ${MEMORY_SOURCES})
    set(NLM_COGNITION_SOURCES ${COGNITION_SOURCES})
    set(NLM_SENSORY_SOURCES ${SENSORY_SOURCES})
    set(NLM_MOTOR_SOURCES ${MOTOR_SOURCES})
    set(NLM_ENVIRONMENT_SOURCES ${ENVIRONMENT_SOURCES})
    set(NLM_PREDICTION_SOURCES ${PREDICTION_SOURCES})
    set(NLM_EXPERIMENTS_SOURCES ${EXPERIMENTS_SOURCES})
    set(NLM_AGENT_SOURCES ${AGENT_SOURCES})
    set(NLM_WORLD_SOURCES ${WORLD_SOURCES})
    set(NLM_VISUALIZATION_SOURCES ${VISUALIZATION_SOURCES})
endmacro()

# Execute automatic source discovery
nlm_collect_sources()

# ==============================================================================
# CORE LIBRARY BUILDING
# ==============================================================================

if(BUILD_CORE OR BUILD_BRAIN OR BUILD_DYNAMICS OR BUILD_PLASTICITY OR BUILD_DEVELOPMENT OR BUILD_NEUROMODULATION OR BUILD_MEMORY OR BUILD_COGNITION OR BUILD_SENSORY OR BUILD_MOTOR OR BUILD_ENVIRONMENT OR BUILD_PREDICTION OR BUILD_EXPERIMENTS OR BUILD_AGENT OR BUILD_WORLD OR BUILD_VISUALIZATION)
    add_library(nlm_core STATIC ${NLM_CORE_SOURCES} ${NLM_BRAIN_SOURCES} ${NLM_DYNAMICS_SOURCES} ${NLM_PLASTICITY_SOURCES}
                                  ${NLM_DEVELOPMENT_SOURCES} ${NLM_NEUROMODULATION_SOURCES} ${NLM_MEMORY_SOURCES}
                                  ${NLM_COGNITION_SOURCES} ${NLM_SENSORY_SOURCES} ${NLM_MOTOR_SOURCES}
                                  ${NLM_ENVIRONMENT_SOURCES} ${NLM_PREDICTION_SOURCES} ${NLM_EXPERIMENTS_SOURCES}
                                  ${NLM_AGENT_SOURCES} ${NLM_WORLD_SOURCES} ${NLM_VISUALIZATION_SOURCES})
    
    target_include_directories(nlm_core PUBLIC
        $<BUILD_INTERFACE:${NLM_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
    )
    
    # Compiler options specific to core library
    target_compile_options(nlm_core PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/bigobj>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-fvisibility=hidden>
    )
    
    # Position independent code for shared library potential
    set_target_properties(nlm_core PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
endif()

# ==============================================================================
# COMPONENT-LIBRARY BUILDING
# ==============================================================================

# Separate component libraries for better modularity

# Agent system
if(BUILD_AGENT AND BUILD_CORE)
    add_library(nlm_agent STATIC src/agent/SensoryPercept.cpp src/agent/AgentBrain.cpp)
    target_include_directories(nlm_agent PUBLIC src)
    target_link_libraries(nlm_agent PUBLIC nlm_core)
endif()

# World system  
if(BUILD_WORLD AND BUILD_CORE)
    add_library(nlm_world STATIC src/world/SimpleWorld.cpp)
    target_include_directories(nlm_world PUBLIC src)
    target_link_libraries(nlm_world PUBLIC nlm_core)
endif()

# Phase 3 experiments
if(BUILD_EXPERIMENTS AND BUILD_CORE AND BUILD_AGENT AND BUILD_WORLD)
    add_library(nlm_phase3 STATIC src/experiments/Phase3Experiment.cpp)
    target_include_directories(nlm_phase3 PUBLIC src)
    target_link_libraries(nlm_phase3 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# Phase 4 experiments
if(BUILD_EXPERIMENTS AND BUILD_CORE AND BUILD_AGENT AND BUILD_WORLD)
    add_library(nlm_phase4 STATIC src/experiments/Phase4Experiment.cpp)
    target_include_directories(nlm_phase4 PUBLIC src)
    target_link_libraries(nlm_phase4 PUBLIC nlm_core nlm_agent nlm_world)
endif()

# ==============================================================================
# EXECUTABLES
# ==============================================================================

# Main demo executable
if(BUILD_MAIN_DEMO AND BUILD_CORE)
    add_executable(nlm src/main.cpp)
    target_link_libraries(nlm PUBLIC nlm_core)
endif()

# Phase 3 demo executable
if(BUILD_PHASE3_DEMO AND BUILD_CORE AND BUILD_AGENT AND BUILD_WORLD AND BUILD_PHASE3)
    add_executable(nlm_phase3_demo src/experiments/Phase3Demo.cpp)
    target_link_libraries(nlm_phase3_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3)
endif()

# Phase 4 demo executable
if(BUILD_PHASE4_DEMO AND BUILD_CORE AND BUILD_AGENT AND BUILD_WORLD AND BUILD_PHASE3 AND BUILD_PHASE4)
    add_executable(nlm_phase4_demo src/experiments/Phase4Demo.cpp)
    target_link_libraries(nlm_phase4_demo PUBLIC nlm_core nlm_agent nlm_world nlm_phase3 nlm_phase4)
endif()

# Phase 6 demo executable
if(BUILD_PHASE6_DEMO AND BUILD_CORE)
    add_executable(nlm_phase6_demo src/experiments/Phase6Demo.cpp)
    target_link_libraries(nlm_phase6_demo PUBLIC nlm_core)
endif()

# Test configuration include
if(BUILD_TESTS)
    include(cmake/NLMTestConfig.cmake)
endif()

# ==============================================================================
# PYTHON BINDINGS
# ==============================================================================

if(BUILD_PYTHON_BINDINGS)
    # Find Python and PyBind11
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
    find_package(pybind11 REQUIRED)
    
    # Create Python module
    pybind11_add_module(pynlm
        src/python/PyBind11Module.cpp
        src/python/PyBind11Brain.cpp
        src/python/PyBind11Neuron.cpp
        src/python/PyBind11Synapse.cpp
    )
    
    target_link_libraries(pynlm PRIVATE nlm_core)
    target_include_directories(pynlm PRIVATE src)
    
    # Set Python module properties
    set_target_properties(pynlm PROPERTIES
        PREFIX ""
        SUFFIX ".so"
        OUTPUT_NAME "pynlm"
    )
    
    # Install Python module
    install(TARGETS pynlm
        LIBRARY DESTINATION ${Python3_SITEARCH}
        RUNTIME DESTINATION bin
    )
endif()

# ==============================================================================
# TESTING FRAMEWORK
# ==============================================================================

if(BUILD_TESTS)
    enable_testing()
    
    # Add tests directory
    add_subdirectory(tests)
    
    # Configure tests based on enabled components
    if(BUILD_CORE)
        add_test(NAME test_core
            COMMAND test_core
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()
    
    if(BUILD_BRAIN)
        add_test(NAME test_brain
            COMMAND test_brain
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()
    
    if(BUILD_PLASTICITY)
        add_test(NAME test_plasticity
            COMMAND test_plasticity
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    endif()
endif()

# ==============================================================================
# INSTALLATION
# ==============================================================================

# Install targets
install(TARGETS nlm nlm_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY src/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h"
)

# Install documentation
install(DIRECTORY ${NLM_DOC_DIR}/
    DESTINATION doc/nlm
    FILES_MATCHING PATTERN "*.md" PATTERN "*.pdf" PATTERN "*.html"
)

# Install configuration files
install(DIRECTORY ${NLM_CONFIG_DIR}/
    DESTINATION etc/nlm
)

# ==============================================================================
# UTILITY TARGETS
# ==============================================================================

# Build helper targets
add_custom_target(generate_docs
    COMMAND doxygen Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generate project documentation"
    VERBATIM
)

add_custom_target(performance_benchmark
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/nlm --benchmark
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Run performance benchmark"
    VERBATIM
)

add_custom_target(memory_analysis
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/valgrind --tool=massif ./nlm
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Run memory analysis"
    VERBATIM
)

# ==============================================================================
# PROJECT SUMMARY
# ==============================================================================

message(STATUS "NLM CMake Configuration Summary:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Core library: ${BUILD_CORE}")
message(STATUS "  Python bindings: ${BUILD_PYTHON_BINDINGS}")
message(STATUS "  Tests enabled: ${BUILD_TESTS}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Output directories:")
message(STATUS "    Executables: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "    Libraries: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "  Installation prefix: ${CMAKE_INSTALL_PREFIX}")

# Add convenience targets for development
add_custom_target(help
    COMMAND ${CMAKE_COMMAND} -E echo "Available targets:"
    COMMAND ${CMAKE_COMMAND} -E echo "  all - Build all targets"
    COMMAND ${CMAKE_COMMAND} -E echo "  demos - Build demo executables"
    COMMAND ${CMAKE_COMMAND} -E echo "  tests - Build and run tests"
    COMMAND ${CMAKE_COMMAND} -E echo "  docs - Generate documentation"
    COMMAND ${CMAKE_COMMAND} -E echo "  benchmark - Run performance benchmarks"
    COMMENT "Show available build targets"
)