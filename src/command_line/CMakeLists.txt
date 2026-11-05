cmake_minimum_required(VERSION 3.16)

project(nlm_cli LANGUAGES CXX)

# Command line interface for NLM

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

# Command line tool executable
add_executable(nlm-cli
    NLMCommandLine.cpp
    NLMCommandLine.hpp
)

target_link_libraries(nlm-cli PUBLIC nlm_core nlm_agent nlm_world)

target_include_directories(nlm-cli PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Install the command line tool
install(TARGETS nlm-cli
    RUNTIME DESTINATION bin
)

# Add to parent project if this is a subdirectory
if(NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # This is a subdirectory, add the target to the parent
    add_executable(nlm CLI/nlm-cli)
    target_link_libraries(nlm PUBLIC nlm_core nlm_agent nlm_world)
endif()