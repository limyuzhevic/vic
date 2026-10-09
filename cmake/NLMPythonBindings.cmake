# NLM Python Bindings Module
# Provides Python bindings integration using pybind11

# Check if pybind11 is available
if(TARGET pybind11::module)
    # Create pybind11 module using the same approach as pybind11_add_module
    add_library(pynlm MODULE bindings.cpp)
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
    target_include_directories(pynlm PRIVATE ${NLM_SOURCE_DIR}/src)
    
    # Set module properties similar to pybind11_add_module
    set_target_properties(pynlm PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
        PREFIX ""
        SUFFIX ".so"
    )
    
    # Set output directory
    set_target_properties(pynlm PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/python_bindings
    )
    
    # Platform-specific compilation flags
    if(MSVC)
        target_compile_options(pynlm PRIVATE /permissive-)
    endif()
    
    # Python-specific compiler options
    if(PYTHON_VERSION_MAJOR EQUAL 3)
        target_compile_options(pynlm PRIVATE -Wno-unused-variable -Wno-unused-function)
    endif()
    
    # Package configuration for Python bindings
    install(TARGETS pynlm
        LIBRARY DESTINATION pynlm
        ARCHIVE DESTINATION pynlm
        RUNTIME DESTINATION pynlm
    )
    
    # Create a custom target for generating Python bindings documentation
    add_custom_target(pynlm_docs
        COMMAND ${PYTHON_EXECUTABLE} -c "print('Python bindings module available: pynlm')"
        COMMENT "Generating Python bindings documentation"
        VERBATIM
    )
else()
    message(WARNING "pybind11 not found. Python bindings will not be enabled.")
endif()
