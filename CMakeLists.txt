# Python bindings
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

pybind11_add_module(pynlm python/bindings_enhanced.cpp)
target_link_libraries(pynlm PRIVATE nlm_core)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(pynlm PRIVATE pthread)
endif()
