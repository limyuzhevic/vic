# Visualization Component CMake Configuration

# Add visualization module to the main build

# Visualization library
add_library(nlm_visualization STATIC
    src/visualization/Visualization.cpp
)

target_include_directories(nlm_visualization PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(nlm_visualization PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Link with core libraries and performance monitoring
target_link_libraries(nlm_visualization PUBLIC nlm_core nlm_agent nlm_world nlm_performance_monitoring)

# Add dependencies
add_dependencies(nlm_visualization nlm_core nlm_agent nlm_world nlm_performance_monitoring)

# Install targets
install(TARGETS nlm_visualization
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Python bindings for visualization
pybind11_add_module(pynlm_visualization visualization_python_bindings.cpp)
target_link_libraries(pynlm_visualization PRIVATE nlm_visualization nlm_core nlm_agent nlm_world nlm_performance_monitoring)
target_include_directories(pynlm_visualization PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(MSVC)
    target_compile_options(pynlm_visualization PRIVATE /permissive-)
endif()

install(TARGETS pynlm_visualization
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)