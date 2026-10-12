# Minimal CMakeLists.txt for Python bindings - references root build
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# Use libraries from parent build
add_subdirectory(..)

# Create Python bindings module
pybind11_add_module(pynlm bindings.cpp)
target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)

target_include_directories(pynlm PRIVATE ${NLM_SOURCE_DIR})

# Platform-specific compilation flags
if(MSVC)
    target_compile_options(pynlm PRIVATE /permissive-)
endif()

# Install Python bindings
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)