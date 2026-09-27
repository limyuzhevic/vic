cmake_minimum_required(VERSION 3.16)
project(AgentBrainTest)
set(CMAKE_CXX_STANDARD 20)

# Test AgentBrain compilation
add_executable(test_agent_brain src/agent/AgentBrain.cpp)
target_include_directories(test_agent_brain PRIVATE src src/agent)

# Test basic includes
include_directories(
    src/core
    src/brain
    src/neuromodulation
    src/prediction
)
