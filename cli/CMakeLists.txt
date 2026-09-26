cmake_minimum_required(VERSION 3.16)

project(nlm_cli VERSION 0.1.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find NLM libraries
find_package(nlm REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

# Create CLI executable
add_executable(nlm_cli
    main.cpp
)

target_link_libraries(nlm_cli PRIVATE nlm_core nlm_agent nlm_world)
target_compile_features(nlm_cli PRIVATE cxx_std_20)

# Install target
install(TARGETS nlm_cli
    RUNTIME DESTINATION bin
)

# Add custom target for generating help
add_custom_target(help
    COMMAND nlm_cli --help
    COMMENT "Generate help information"
    VERBATIM
)