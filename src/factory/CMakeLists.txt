cmake_minimum_required(VERSION 3.16)

project(nlm_factory LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Factory library
add_library(nlm_factory STATIC
    BrainFactory.cpp
    BrainFactory.hpp
)

target_include_directories(nlm_factory PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_factory PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic -Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

target_link_libraries(nlm_factory PUBLIC nlm_core nlm_agent nlm_world)

install(TARGETS nlm_factory
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES BrainFactory.hpp
    DESTINATION include/nlm/factory
)