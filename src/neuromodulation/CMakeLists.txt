cmake_minimum_required(VERSION 3.16)

# Neuromodulation module CMakeLists.txt
# Contains: Neuromodulator, Reward, PredictionError, Novelty, Curiosity

add_library(nlm_neuromodulation STATIC
    neuromodulation/Neuromodulator.cpp
    neuromodulation/Reward.cpp
    neuromodulation/PredictionError.cpp
    neuromodulation/Novelty.cpp
    neuromodulation/Curiosity.cpp
)

target_include_directories(nlm_neuromodulation PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_neuromodulation PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

find_package(nlm_core REQUIRED)
target_link_libraries(nlm_neuromodulation PUBLIC nlm_core)

install(TARGETS nlm_neuromodulation
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY neuromodulation/
    DESTINATION include/neuromodulation
    FILES_MATCHING PATTERN "*.hpp"
)
