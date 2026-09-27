# NLM Environment Layer - World simulation and interaction

# Environment
add_library(nlm_environment STATIC
    src/environment/Environment.cpp
)

target_include_directories(nlm_environment PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/environment>
    $<INSTALL_INTERFACE:include/environment>
)

# Observation
add_library(nlm_observation STATIC
    src/environment/Observation.cpp
)

target_include_directories(nlm_observation PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/environment>
    $<INSTALL_INTERFACE:include/environment>
)

# Action
add_library(nlm_action_static STATIC
    src/environment/Action.cpp
)

target_include_directories(nlm_action_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/environment>
    $<INSTALL_INTERFACE:include/environment>
)

# Environment layer interface library
add_library(nlm_environment INTERFACE)
target_link_libraries(nlm_environment INTERFACE
    nlm_environment
    nlm_observation
    nlm_action_static
)

# Environment layer installation
install(TARGETS nlm_environment nlm_observation nlm_action_static
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/environment
    DESTINATION include/environment
    FILES_MATCHING PATTERN "*.hpp"
)

# Environment interface header
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/environment/include/nlm_environment.hpp
    DESTINATION include/environment
)