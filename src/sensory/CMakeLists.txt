cmake_minimum_required(VERSION 3.16)

# Sensory module CMakeLists.txt
# Contains: SensoryInput, Vision, Audio, InternalSignals

add_library(nlm_sensory STATIC
    sensory/SensoryInput.cpp
    sensory/Vision.cpp
    sensory/Audio.cpp
    sensory/InternalSignals.cpp
)

target_include_directories(nlm_sensory PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_sensory PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

find_package(nlm_core REQUIRED)
target_link_libraries(nlm_sensory PUBLIC nlm_core)

install(TARGETS nlm_sensory
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY sensory/
    DESTINATION include/sensory
    FILES_MATCHING PATTERN "*.hpp"
)
