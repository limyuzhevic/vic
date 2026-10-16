cmake_minimum_required(VERSION 3.16)

project(nlm_world VERSION 1.0.0 LANGUAGES CXX)

# World library sources

set(WORLD_SOURCES
    ../environment/Environment.cpp
    Observation.cpp
)

# Add world library
add_library(nlm_world STATIC ${WORLD_SOURCES})

target_include_directories(nlm_world PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../environment
)

target_link_libraries(nlm_world PUBLIC nlm_core nlm_brain nlm_agent)

# Install the world library
install(TARGETS nlm_world
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ../world/ DESTINATION include/nlm/world
    FILES_MATCHING PATTERN "*.hpp"
)
install(DIRECTORY ../environment/ DESTINATION include/nlm/environment
    FILES_MATCHING PATTERN "*.hpp"
)
