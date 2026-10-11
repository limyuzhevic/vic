cmake_minimum_required(VERSION 3.16)
project(NLM Enhanced Python Bindings VERSION 2.0.0 LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find required packages
find_package(nlohmann_json REQUIRED)
find_package(yaml-cpp REQUIRED)

# Option to enable enhanced Python bindings
option(ENHANCED_BINDINGS "Enable enhanced Python bindings with modern features" ON)
option(PYTHON_EMBEDDED "Build Python bindings" ON)

# Enhanced Python bindings include directory
if(ENHANCED_BINDINGS)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/core)
endif()

# Core Config library
add_library(nlm_config STATIC
    src/core/Config/Config.cpp
)

target_include_directories(nlm_config PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
    ${nlohmann_json_INCLUDE_DIRS}
    ${YAML_CPP_INCLUDE_DIRS}
)

target_link_libraries(nlm_config PUBLIC
    nlohmann_json::nlohmann_json
    yaml-cpp
)

# Install target
install(TARGETS nlm_config
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# Enhanced Python bindings library
if(ENHANCED_BINDINGS AND PYTHON_EMBEDDED)
    add_library(nlm_enhanced_python MODULE
        python/bindings.cpp
        src/core/EnhancedMain.cpp
    )
    
    set_target_properties(nlm_enhanced_python PROPERTIES
        OUTPUT_NAME "nlm_enhanced"
        PREFIX ""
        SUFFIX ".so"
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
    
    target_include_directories(nlm_enhanced_python PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${nlohmann_json_INCLUDE_DIRS}
        ${YAML_CPP_INCLUDE_DIRS}
        pybind11/include
    )
    
    target_link_libraries(nlm_enhanced_python PRIVATE
        nlm_config
        nlohmann_json::nlohmann_json
        yaml-cpp
    )
    
    # Python module compilation
    target_compile_definitions(nlm_enhanced_python PRIVATE
        PYBIND11_MODULE_NAME="pynlm_enhanced"
    )
    
    # Install Python module
    install(TARGETS nlm_enhanced_python
        LIBRARY DESTINATION lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages
    )
endif()

# Original Python bindings
if(PYTHON_EMBEDDED)
    add_library(pynlm MODULE
        python/bindings.cpp
    )
    
    set_target_properties(pynlm PROPERTIES
        OUTPUT_NAME "pynlm"
        PREFIX ""
        SUFFIX ".so"
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
    
    target_include_directories(pynlm PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${nlohmann_json_INCLUDE_DIRS}
        ${YAML_CPP_INCLUDE_DIRS}
        pybind11/include
    )
    
    target_link_libraries(pynlm PRIVATE
        nlm_config
        nlohmann_json::nlohmann_json
        yaml-cpp
    )
    
    # Install original Python module
    install(TARGETS pynlm
        LIBRARY DESTINATION lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages
    )
endif()

# Enhanced Python examples
if(ENHANCED_BINDINGS AND PYTHON_EMBEDDED)
    add_custom_target(run_examples COMMENT "Running enhanced Python examples")
    
    add_custom_command(
        TARGET run_examples POST_BUILD
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/examples/example1_basic.py
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/examples/example2_events.py
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/examples/example3_training.py
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/examples/example4_simulation.py
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/examples/example5_visualization.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running enhanced Python examples"
        VERBATIM
    )
endif()

# Testing
enable_testing()
add_subdirectory(tests)

# Python module installation
if(PYTHON_EMBEDDED)
    # Create Python package structure
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/python)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/python/bindings.cpp
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/python)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/python/CMakeLists.txt
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/python)
    
    # Generate Python package files
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/pynlm_enhanced/__init__.py
        ${CMAKE_CURRENT_BINARY_DIR}/python/__init__.py
        @ONLY
    )
endif()
