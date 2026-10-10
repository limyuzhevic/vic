cmake_minimum_required(VERSION 3.16)

# Plugin examples
add_subdirectory(plugin)

# Basic demonstration program
add_executable(nlm_demo_basic
    demo/basic_example.cpp
)

target_link_libraries(nlm_demo_basic
    nlm_core
    nlm_neural
    nlm_memory
    nlm_cognition
    nlm_plasticity
    nlm_systems
)

target_include_directories(nlm_demo_basic PUBLIC src)

# Integration demonstration program
add_executable(nlm_demo_integration
    demo/integration_example.cpp
)

target_link_libraries(nlm_demo_integration
    nlm_core
    nlm_neural
    nlm_memory
    nlm_cognition
    nlm_plasticity
    nlm_systems
)

target_include_directories(nlm_demo_integration PUBLIC src)

# Advanced usage demonstration program
add_executable(nlm_demo_advanced
    demo/advanced_example.cpp
)

target_link_libraries(nlm_demo_advanced
    nlm_core
    nlm_neural
    nlm_memory
    nlm_cognition
    nlm_plasticity
    nlm_systems
)

target_include_directories(nlm_demo_advanced PUBLIC src)

# Documentation target
add_custom_target(docs
    COMMAND ${CMAKE_COMMAND} -E echo "Documentation build not implemented yet"
    COMMENT "Building documentation"
)

# Installation targets for examples
install(TARGETS nlm_demo_basic nlm_demo_integration nlm_demo_advanced
    RUNTIME DESTINATION bin
)