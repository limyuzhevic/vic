cmake_minimum_required(VERSION 3.16)
project(pynlm_bindings LANGUAGES CXX)

# Python bindings improvements
pybind11_add_module(pynlm_enhanced python/bindings_improvements.cpp)
target_link_libraries(pynlm_enhanced PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm_enhanced PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if(MSVC)
    target_compile_options(pynlm_enhanced PRIVATE /permissive-)
endif()

# Install enhanced bindings
install(TARGETS pynlm_enhanced
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

# Original bindings (for backward compatibility)
pybind11_add_module(pynlm python/bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

# Install original bindings
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)
