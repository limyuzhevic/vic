#!/usr/bin/env cmake -P
# CMake script to build NLM advanced command system

cmake_minimum_required(VERSION 3.16)
project(NLM_Advanced_Command_System VERSION 0.1.0 LANGUAGES CXX)

# Source files for command system
set(COMMAND_SOURCE_FILES
    src/commands/CommandBase.hpp
    src/commands/CommandParsing.hpp
    src/commands/CommandIntegration.hpp
    src/commands/CommandSystem.hpp
    src/commands/MonitoringCommands.hpp
    src/commands/NetworkExplorationCommands.hpp
    src/commands/LearningControlCommands.hpp
    src/commands/SimulationControlCommands.hpp
    src/commands/ExperimentControlCommands.hpp
    src/commands/VisualizationControlCommands.hpp
)

# Source files for UI components
set(UI_SOURCE_FILES
    src/ui/NLMUI.hpp
)

# Source files for console interface
set(CONSOLE_SOURCE_FILES
    src/console/InteractiveConsole.hpp
)

# Create command system library
add_library(nlm_commands INTERFACE)
add_library(nlm_ui INTERFACE)
add_library(nlm_console INTERFACE)

# Configure include directories
target_include_directories(nlm_commands INTERFACE src/commands)
target_include_directories(nlm_ui INTERFACE src/ui)
target_include_directories(nlm_console INTERFACE src/console)

# Add dependencies
target_link_libraries(nlm_commands INTERFACE nlm_core)
target_link_libraries(nlm_ui INTERFACE nlm_core nlm_commands)
target_link_libraries(nlm_console INTERFACE nlm_core nlm_commands nlm_ui)

# Install targets
install(TARGETS nlm_commands nlm_ui nlm_console
    EXPORT NLMCommandTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Create example applications
set(EXAMPLE_APPS
    examples/command_demo
    examples/ui_demo
    examples/console_demo
)

# Function to create example
set(EXAMPLE_COMMAND_DEMO_SOURCE
    examples/command_demo.cpp
)

# Create demo executables
foreach(example ${EXAMPLE_APPS})
    add_executable(${example}
        ${${example}_SOURCE}
        ${${example}_SOURCE_FILES}
    )
    target_link_libraries(${example} PUBLIC nlm_core nlm_commands nlm_ui nlm_console)
    install(TARGETS ${example} DESTINATION bin)
endforeach()

# Testing
enable_testing()
add_subdirectory(tests)

message(STATUS "NLM Advanced Command System configured successfully")
