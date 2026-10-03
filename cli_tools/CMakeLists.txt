cmake_minimum_required(VERSION 3.16)
project(NLM_CLI_TOOLS VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/brain)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/dynamics)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments)

# Find required libraries
find_package(Threads REQUIRED)

# NLM CLI Tools executable
add_executable(nlm_cli_tools
    cli_tools.cpp
    ../core/Config/Config.cpp
    ../core/Logger/Logger.cpp
    ../brain/Brain.cpp
    ../brain/Neuron.cpp
    ../brain/Synapse.cpp
    ../brain/NeuralPopulation.cpp
    ../brain/NeuralRegion.cpp
    ../dynamics/SpikeSystem.cpp
    ../experiments/Phase6IntegratedExperiment.cpp
)

# Target link libraries
target_link_libraries(nlm_cli_tools
    PRIVATE
        Threads::Threads
)

# Include directories for target
target_include_directories(nlm_cli_tools
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/core
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/brain
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/dynamics
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/experiments
)

# Installation
install(TARGETS nlm_cli_tools
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# Create symlinks to individual CLI tools
add_custom_target(create_cli_symlinks
    COMMAND ln -sf nlm_cli_tools nlm-config-validate
    COMMAND ln -sf nlm_cli_tools nlm-network-visualize
    COMMAND ln -sf nlm_cli_tools nlm-profile
    COMMAND ln -sf nlm_cli_tools nlm-tune
    COMMAND ln -sf nlm_cli_tools nlm-sim
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin
    COMMENT "Creating CLI tool symlinks"
)

add_dependencies(nlm_cli_tools create_cli_symlinks)

# Output files
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/README.txt
    "NLM Command-Line Tools\n"
    "=====================\n\n"
    "This directory contains the NLM command-line tools.\n\n"
    "Available tools:\n"
    "  nlm-config-validate  Validate configuration files\n"
    "  nlm-network-visualize Visualize neural network\n"
    "  nlm-profile            Profile simulation performance\n"
    "  nlm-tune              Tune neural network parameters\n"
    "  nlm-sim               Run simulation\n\n"
    "For more information, use: <tool> --help\n"
    "\n"
    "The tools use the NLM (Neural Learning Machine) framework\n"
    "for neural network simulations and analysis.\n"
)

# Test target
enable_testing()

add_test(NAME config_validate_test
    COMMAND nlm_cli_tools config-validate --help
)

add_test(NAME network_visualize_test
    COMMAND nlm_cli_tools network-visualize --help
)

add_test(NAME profile_test
    COMMAND nlm_cli_tools profile --help
)

add_test(NAME tune_test
    COMMAND nlm_cli_tools tune --help
)

add_test(NAME sim_test
    COMMAND nlm_cli_tools sim --help
)