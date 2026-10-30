cmake_minimum_required(VERSION 3.16)

# Cognition module CMakeLists.txt
# Contains: ConceptFormation, NeuralPlanner

add_library(nlm_cognition STATIC
    cognition/ConceptFormation.cpp
    cognition/NeuralPlanner.cpp
)

target_include_directories(nlm_cognition PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_cognition PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

find_package(nlm_core REQUIRED)
target_link_libraries(nlm_cognition PUBLIC nlm_core)

install(TARGETS nlm_cognition
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY cognition/
    DESTINATION include/cognition
    FILES_MATCHING PATTERN "*.hpp"
)
