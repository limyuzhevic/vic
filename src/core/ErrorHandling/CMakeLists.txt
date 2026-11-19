cmake_minimum_required(VERSION 3.16)

# Error handling and recovery library
add_library(nlm_error_handling
    ErrorLogger.cpp
)

target_include_directories(nlm_error_handling PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(nlm_error_handling PRIVATE cxx_std_20)

# Installation
install(TARGETS nlm_error_handling
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ErrorLogger.hpp ErrorHandling/ErrorLogger.cpp
    DESTINATION include/nlm/core
)