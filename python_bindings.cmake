# Add Python bindings to the main build
# This file should be included in the main CMakeLists.txt

# Find Python and pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Python module target
pybind11_add_module(pynlm python/bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

# Install Python module
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)

# Enhanced Python module
pybind11_add_module(pynlm_enhanced python/bindings_improvements.cpp)
target_link_libraries(pynlm_enhanced PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm_enhanced PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(MSVC)
    target_compile_options(pynlm_enhanced PRIVATE /permissive-)
endif()

# Install enhanced module
install(TARGETS pynlm_enhanced
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)