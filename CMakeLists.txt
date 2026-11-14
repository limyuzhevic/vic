# Python bindings (Phase 5: this is what `pip install .` builds).
# Previously pyproject.toml claimed scikit-build-core would build pynlm,
# but the root CMakeLists never built the module — pip install
# succeeded yet `import pynlm` failed. The option is OFF by default so
# a plain C++ build needs no Python/pybind11; pyproject.toml turns it
# ON. The standalone python/CMakeLists.txt build still works too, but
# it must not be added here as a subdirectory: it defines its own
# nlm_core/nlm_agent/nlm_world targets, which would collide with the
# ones above. This target reuses the root libraries instead.
option(NLM_BUILD_PYTHON_BINDINGS "Build the pynlm Python module" OFF)
if(NLM_BUILD_PYTHON_BINDINGS)
    find_package(Python REQUIRED COMPONENTS Interpreter Development)
    find_package(pybind11 CONFIG REQUIRED)
    # The module is shared, so the static libs it links need PIC.
    set_target_properties(nlm_core nlm_agent nlm_world PROPERTIES
        POSITION_INDEPENDENT_CODE ON)
    pybind11_add_module(pynlm python/bindings.cpp)
    target_link_libraries(pynlm PRIVATE nlm_core nlm_agent nlm_world)
    target_include_directories(pynlm PRIVATE src)
    install(TARGETS pynlm LIBRARY DESTINATION .)
endif()