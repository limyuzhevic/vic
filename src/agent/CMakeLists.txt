# NLM Agent Layer - Agent systems and interfaces

# Sensory percept
add_library(nlm_sensory_percept STATIC
    src/agent/SensoryPercept.cpp
)

target_include_directories(nlm_sensory_percept PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/agent>
    $<INSTALL_INTERFACE:include/agent>
)

# Agent brain
add_library(nlm_agent_brain STATIC
    src/agent/AgentBrain.cpp
)

target_include_directories(nlm_agent_brain PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/agent>
    $<INSTALL_INTERFACE:include/agent>
)

# Agent body
add_library(nlm_agent_body STATIC
    src/agent/AgentBody.cpp
)

target_include_directories(nlm_agent_body PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/agent>
    $<INSTALL_INTERFACE:include/agent>
)

# Agent layer interface library
add_library(nlm_agent INTERFACE)
target_link_libraries(nlm_agent INTERFACE
    nlm_sensory_percept
    nlm_agent_brain
    nlm_agent_body
)

# Agent layer installation
install(TARGETS nlm_sensory_percept nlm_agent_brain nlm_agent_body
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/agent
    DESTINATION include/agent
    FILES_MATCHING PATTERN "*.hpp"
)

# Agent interface header
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/agent/include/nlm_agent.hpp
    DESTINATION include/agent
)