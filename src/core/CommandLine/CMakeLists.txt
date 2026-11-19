cmake_minimum_required(VERSION 3.16)

# Command-line parser library
add_library(nlm_command_line
    CommandLineParser.cpp
)

target_include_directories(nlm_command_line PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(nlm_command_line PRIVATE cxx_std_20)

# Installation
install(TARGETS nlm_command_line
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES CommandLineParser.hpp
    DESTINATION include/nlm/core
)