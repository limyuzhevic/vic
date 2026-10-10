cmake_minimum_required(VERSION 3.16)

# Agent module library
add_library(nlm_agent
    SensoryPercept.cpp
    AgentBrain.cpp
)

target_include_directories(nlm_agent PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/agent>
    $<INSTALL_INTERFACE:include/agent>
)

target_link_libraries(nlm_agent PUBLIC nlm_core nlm_neural)

# Agent test target
if(NLM_BUILD_TESTS)
    add_executable(nlm_agent_test
        ../tests/systems/test_agent.cpp
    )
    
    target_link_libraries(nlm_agent_test PRIVATE nlm_agent)
    target_compile_features(nlm_agent_test PRIVATE cxx_std_20)
    
    add_test(NAME nlm_agent_test COMMAND nlm_agent_test)
endif()