cmake_minimum_required(VERSION 3.16)
project(NLMDemo VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find NLM libraries
find_package(nlm REQUIRED)

add_executable(performance_demo
    performance_demo.cpp
)

target_link_libraries(performance_demo
    nlm_core
)