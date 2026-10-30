cmake_minimum_required(VERSION 3.16)

# Plasticity module CMakeLists.txt
# Contains: PlasticityRule, Hebbian, STDP, StructuralPlasticity

add_library(nlm_plasticity STATIC
    plasticity/PlasticityRule.cpp
    plasticity/Hebbian.cpp
    plasticity/STDP.cpp
    plasticity/StructuralPlasticity.cpp
)

target_include_directories(nlm_plasticity PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_plasticity PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

find_package(nlm_core REQUIRED)
target_link_libraries(nlm_plasticity PUBLIC nlm_core)

install(TARGETS nlm_plasticity
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY plasticity/
    DESTINATION include/plasticity
    FILES_MATCHING PATTERN "*.hpp"
)
