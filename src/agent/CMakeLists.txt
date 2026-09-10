// Fixed CMake configuration for AgentBrain components
// Added missing AgentBrain.cpp and AgentBody.cpp files

add_library(nlm_agent STATIC
    src/agent/AgentBrain.cpp
    src/agent/AgentBody.cpp
    src/agent/SensoryPercept.cpp
)

// Add dependencies
target_link_libraries(nlm_agent PUBLIC nlm_core)

// Add include directories
target_include_directories(nlm_agent PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/agent
)

// Set target properties
target_compile_features(nlm_agent PUBLIC cxx_std_20)
set_target_properties(nlm_agent PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)