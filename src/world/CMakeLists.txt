cmake_minimum_required(VERSION 3.16)

# World module CMakeLists.txt
# Contains: SimpleWorld, WorldObject, WorldConfig

add_library(nlm_world STATIC
    world/SimpleWorld.cpp
    world/WorldObject.cpp
    world/WorldConfig.cpp
)

target_include_directories(nlm_world PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_world PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

find_package(nlm_core REQUIRED)
target_link_libraries(nlm_world PUBLIC nlm_core)

install(TARGETS nlm_world
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY world/
    DESTINATION include/world
    FILES_MATCHING PATTERN "*.hpp"
)
