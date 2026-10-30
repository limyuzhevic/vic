cmake_minimum_required(VERSION 3.16)

# Agent module CMakeLists.txt
# Contains: SensoryPercept, AgentBrain, AgentBody

add_library(nlm_agent STATIC
    agent/SensoryPercept.cpp
    agent/AgentBrain.cpp
    agent/AgentBody.cpp
)

target_include_directories(nlm_agent PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_agent PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

find_package(nlm_core REQUIRED)
target_link_libraries(nlm_agent PUBLIC nlm_core)

install(TARGETS nlm_agent
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY agent/
    DESTINATION include/agent
    FILES_MATCHING PATTERN "*.hpp"
)
