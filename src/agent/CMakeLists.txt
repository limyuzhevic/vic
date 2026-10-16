cmake_minimum_required(VERSION 3.16)

project(nlm_agent VERSION 1.0.0 LANGUAGES CXX)

# Agent library sources

set(AGENT_SOURCES
    AgentBrain.cpp
    SensoryPercept.cpp
)

# Add agent library
add_library(nlm_agent STATIC ${AGENT_SOURCES})

target_include_directories(nlm_agent PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../agent
)

target_link_libraries(nlm_agent PUBLIC nlm_core nlm_brain nlm_neuromodulation nlm_cognition nlm_memory)

# Install the agent library
install(TARGETS nlm_agent
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ../agent/ DESTINATION include/nlm/agent
    FILES_MATCHING PATTERN "*.hpp"
)
