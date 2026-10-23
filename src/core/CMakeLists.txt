cmake_minimum_required(VERSION 3.16)

# Create utilities library
add_library(nlm_utilities STATIC
    Utilities/ErrorHandling.cpp
    Utilities/ErrorHandler.cpp
    Utilities/Validator.cpp
)

target_include_directories(nlm_utilities PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(nlm_utilities PUBLIC nlm_core)

# Make error handling available for core library
target_link_libraries(nlm_core PUBLIC nlm_utilities)