# Build enhanced Python bindings
pybind11_add_module(pynlm_enhanced enhanced_bindings.cpp)
target_link_libraries(pynlm_enhanced PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm_enhanced PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Build Python utilities module
pybind11_add_module(pynlm_utils python_utils.cpp)
target_link_libraries(pynlm_utils PRIVATE nlm_core nlm_agent nlm_world)
target_include_directories(pynlm_utils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)