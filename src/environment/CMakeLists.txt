cmake_minimum_required(VERSION 3.16)

# Environment module CMakeLists.txt
# Contains: Environment, Observation, Action

add_library(nlm_environment STATIC
    environment/Environment.cpp
    environment/Observation.cpp
    environment/Action.cpp
)

target_include_directories(nlm_environment PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_environment PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

find_package(nlm_core REQUIRED)
target_link_libraries(nlm_environment PUBLIC nlm_core)

install(TARGETS nlm_environment
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY environment/
    DESTINATION include/environment
    FILES_MATCHING PATTERN "*.hpp"
)
