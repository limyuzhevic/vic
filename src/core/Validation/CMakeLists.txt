cmake_minimum_required(VERSION 3.16)
project(nlm_validation)

add_library(nlm_validation STATIC Validation.cpp)

target_include_directories(nlm_validation PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/core/Validation>
    $<INSTALL_INTERFACE:include>
)

# Make this library available for linking
add_library(nlm::validation ALIAS nlm_validation)
