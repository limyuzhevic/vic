cmake_minimum_required(VERSION 3.16)

project(nlm_core VERSION 1.0.0 LANGUAGES CXX)

# Core library sources

set(CORE_SOURCES
    Config/Config.cpp
    Logger/Logger.cpp
    Random/Random.cpp
    SimulationClock/SimulationClock.cpp
    Types/Types.cpp
)

# Add core library
add_library(nlm_core STATIC ${CORE_SOURCES})

target_include_directories(nlm_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
)

target_link_libraries(nlm_core PUBLIC nlohmann_json::nlohmann_json yaml-cpp)

# Install the core library
install(TARGETS nlm_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY ../core/Config/ DESTINATION include/nlm/core/Config
    FILES_MATCHING PATTERN "*.hpp"
)
install(DIRECTORY ../core/Logger/ DESTINATION include/nlm/core/Logger
    FILES_MATCHING PATTERN "*.hpp"
)
install(DIRECTORY ../core/Random/ DESTINATION include/nlm/core/Random
    FILES_MATCHING PATTERN "*.hpp"
)
install(DIRECTORY ../core/SimulationClock/ DESTINATION include/nlm/core/SimulationClock
    FILES_MATCHING PATTERN "*.hpp"
)
install(DIRECTORY ../core/Types/ DESTINATION include/nlm/core/Types
    FILES_MATCHING PATTERN "*.hpp"
)
