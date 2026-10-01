# Plugin CMake configuration
# This file configures the plugin library and its dependencies

cmake_minimum_required(VERSION 3.16)

# Plugin library
add_library(nlm_plugin STATIC
    PluginInterface.hpp
    PluginRegistry.cpp
    CustomSTDPPlugin.cpp
    CustomSTDPPlugin.hpp
)

target_include_directories(nlm_plugin PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(nlm_plugin PUBLIC cxx_std_20)

# Link dependencies
if(UNIX AND NOT APPLE)
    target_link_libraries(nlm_plugin PUBLIC dl)
endif()

# Installation
install(TARGETS nlm_plugin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES PluginInterface.hpp CustomSTDPPlugin.hpp CustomSTDPPlugin.cpp
    DESTINATION include/nlm/plugin
)