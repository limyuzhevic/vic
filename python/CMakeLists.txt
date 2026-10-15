# CMakeLists.txt for NLM Python bindings

# Find pybind11
find_package(pybind11 REQUIRED)

# Define the Python module
pybind11_add_module(pynlm
    bindings.cpp
)

# Target properties
target_include_directories(pynlm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PYBIND11_INCLUDE_DIR}
)

target_link_libraries(pynlm PRIVATE
    nlm_core
)

# Set target properties
set_target_properties(pynlm PROPERTIES
    PREFIX ""
    SUFFIX ".so"
    OUTPUT_NAME "pynlm"
    VERSION "0.1.0"
    SOVERSION "0.1"
)

# Install target
install(TARGETS pynlm
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Generate Python package files
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pynlm.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/pynlm.py
    @ONLY
)

# Install Python package files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/pynlm.py
    DESTINATION ${CMAKE_INSTALL_DATAROOT}/python/pynlm
)