cmake_minimum_required(VERSION 3.16)

# Motor module CMakeLists.txt
# Contains: MotorSystem

add_library(nlm_motor STATIC
    motor/MotorSystem.cpp
)

target_include_directories(nlm_motor PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_motor PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

find_package(nlm_core REQUIRED)
target_link_libraries(nlm_motor PUBLIC nlm_core)

install(TARGETS nlm_motor
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY motor/
    DESTINATION include/motor
    FILES_MATCHING PATTERN "*.hpp"
)
