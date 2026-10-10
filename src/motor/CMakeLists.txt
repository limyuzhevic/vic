cmake_minimum_required(VERSION 3.16)

# Motor module library
add_library(nlm_motor
    MotorSystem.cpp
    Action.cpp
)

target_include_directories(nlm_motor PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/motor>
    $<INSTALL_INTERFACE:include/motor>
)
