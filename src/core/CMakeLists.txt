cmake_minimum_required(VERSION 3.16)

# Error handling module
add_library(nlm_error_handling STATIC
    ErrorHandling/ErrorHandling.cpp
)
target_include_directories(nlm_error_handling PUBLIC src)

# Configuration builder module
add_library(nlm_config_builder STATIC
    Config/ConfigBuilder.cpp
)
target_include_directories(nlm_config_builder PUBLIC src)
target_link_libraries(nlm_config_builder PUBLIC nlm_core nlm_error_handling)

# Link new modules to core library
# This will be updated in the main CMakeLists.txt