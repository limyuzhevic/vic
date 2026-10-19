# Advanced NLM Components CMake Configuration

# Create advanced module
add_library(nlm_advanced STATIC
    src/advanced/AttentionMechanism.cpp
)

target_include_directories(nlm_advanced PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_advanced PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Link with core libraries
target_link_libraries(nlm_advanced PUBLIC nlm_core nlm_agent nlm_world)

# Add dependencies for enhanced neural networks
add_dependencies(nlm_advanced nlm_core nlm_agent nlm_world)

# Install targets
install(TARGETS nlm_advanced
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Python bindings for advanced components
pybind11_add_module(pynlm_advanced advanced_python_bindings.cpp)
target_link_libraries(pynlm_advanced PRIVATE nlm_advanced nlm_core nlm_agent nlm_world)
target_include_directories(pynlm_advanced PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(MSVC)
    target_compile_options(pynlm_advanced PRIVATE /permissive-)
endif()

install(TARGETS pynlm_advanced
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)