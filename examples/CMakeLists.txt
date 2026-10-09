# Examples CMakeLists.txt
# Contains example applications and demonstrations

# Example: Basic brain usage
add_executable(example_basic
    basic_brain.cpp
)

target_link_libraries(example_basic PRIVATE nlm_core)

# Example: Agent-world interaction
add_executable(example_agent
    agent_world.cpp
)

target_link_libraries(example_agent PRIVATE nlm_core nlm_agent nlm_world)

# Example: Memory system demonstration
add_executable(example_memory
    memory_demo.cpp
)

target_link_libraries(example_memory PRIVATE nlm_core)

# Install examples
install(TARGETS example_basic example_agent example_memory
    RUNTIME DESTINATION bin/examples
)
