# Python bindings for NLM
# Modular bindings using pybind11

pybind11_add_module(pynlm
    pynlm_bindings.cpp
)

target_include_directories(pynlm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(pynlm
    PRIVATE
        nlm_core
        nlm_agent
        nlm_world
)

# Set Python module properties
set_target_properties(pynlm PROPERTIES
    PREFIX ""
    SUFFIX ".so"
    OUTPUT_NAME "pynlm"
)

# Installation
install(TARGETS pynlm
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)
