cmake_minimum_required(VERSION 3.16)

# Development module CMakeLists.txt
# Contains: DevelopmentSystem, Synaptogenesis, Pruning, Maturation

add_library(nlm_development STATIC
    development/DevelopmentSystem.cpp
    development/Synaptogenesis.cpp
    development/Pruning.cpp
    development/Maturation.cpp
)

target_include_directories(nlm_development PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_development PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

find_package(nlm_core REQUIRED)
target_link_libraries(nlm_development PUBLIC nlm_core)

install(TARGETS nlm_development
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY development/
    DESTINATION include/development
    FILES_MATCHING PATTERN "*.hpp"
)
