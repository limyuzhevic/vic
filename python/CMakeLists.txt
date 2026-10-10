# Modern pybind11 integration for NLM Python bindings

# Package configuration
set(PYTHON_PACKAGE_NAME "pynlm" CACHE STRING "Python package name")
set(PYTHON_MODULE_NAME "pynlm" CACHE STRING "Main Python module name")
set(PYTHON_VERSION "3.8" CACHE STRING "Minimum Python version")

# Find Python and pybind11 dependencies
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG REQUIRED)

# C++ standard requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Python binding configuration options
option(NLM_ENABLE_PYTHON_BINDINGS "Enable Python bindings" ON)
option(NLM_ENABLE_PYTHON_TESTS "Enable Python tests" ON)
option(NLM_ENABLE_PYTHON_DOCS "Enable Python documentation generation" OFF)

# Compiler optimization flags for Python bindings
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include core NLM library
add_subdirectory(../src src)

# Core bindings - essential components
pybind11_add_module(pynlm_core
    bindings/bindings_core.cpp
)
target_link_libraries(pynlm_core PRIVATE
    nlm_core_fundamental
    nlm_brain
    nlm_sensory
    nlm_environment
    nlm_agent
)
target_include_directories(pynlm_core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Neuromodulation bindings (conditional)
if(NLM_ENABLE_NEUROMODULATION)
    pybind11_add_module(pynlm_neuromodulation
        bindings/bindings_neuromodulation.cpp
    )
    target_link_libraries(pynlm_neuromodulation PRIVATE
        nlm_core_fundamental
        nlm_neuromodulation
    )
    target_include_directories(pynlm_neuromodulation PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
endif()

# Memory bindings (conditional)
if(NLM_ENABLE_MEMORY_SYSTEMS)
    pybind11_add_module(pynlm_memory
        bindings/bindings_memory.cpp
    )
    target_link_libraries(pynlm_memory PRIVATE
        nlm_core_fundamental
        nlm_memory
    )
    target_include_directories(pynlm_memory PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
endif()

# Cognition bindings (conditional)
if(NLM_ENABLE_COGNITION)
    pybind11_add_module(pynlm_cognition
        bindings/bindings_cognition.cpp
    )
    target_link_libraries(pynlm_cognition PRIVATE
        nlm_core_fundamental
        nlm_cognition
    )
    target_include_directories(pynlm_cognition PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
endif()

# Prediction bindings (conditional)
if(NLM_ENABLE_PREDICTION)
    pybind11_add_module(pynlm_prediction
        bindings/bindings_prediction.cpp
    )
    target_link_libraries(pynlm_prediction PRIVATE
        nlm_core_fundamental
        nlm_prediction
    )
    target_include_directories(pynlm_prediction PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
endif()

# Development bindings (conditional)
if(NLM_ENABLE_DEVELOPMENT)
    pybind11_add_module(pynlm_development
        bindings/bindings_development.cpp
    )
    target_link_libraries(pynlm_development PRIVATE
        nlm_core_fundamental
        nlm_development
    )
    target_include_directories(pynlm_development PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
endif()

# Visualization bindings (conditional)
if(NLM_ENABLE_VISUALIZATION)
    pybind11_add_module(pynlm_visualization
        bindings/bindings_visualization.cpp
    )
    target_link_libraries(pynlm_visualization PRIVATE
        nlm_core_fundamental
        nlm_visualization
    )
    target_include_directories(pynlm_visualization PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
endif()

# Performance bindings (conditional)
if(NLM_ENABLE_PERFORMANCE_OPTIMIZATIONS)
    pybind11_add_module(pynlm_performance
        bindings/bindings_performance.cpp
    )
    target_link_libraries(pynlm_performance PRIVATE
        nlm_core_fundamental
        nlm_performance
    )
    target_include_directories(pynlm_performance PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
endif()

# Create Python package initialization file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/bindings/__init__.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/__init__.py
    @ONLY
)

# Install Python bindings
install(TARGETS pynlm_core
    LIBRARY DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
    ARCHIVE DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
    RUNTIME DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
)

# Install optional bindings conditionally
if(NLM_ENABLE_NEUROMODULATION)
    install(TARGETS pynlm_neuromodulation
        LIBRARY DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
        ARCHIVE DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
        RUNTIME DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
    )
endif()

if(NLM_ENABLE_MEMORY_SYSTEMS)
    install(TARGETS pynlm_memory
        LIBRARY DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
        ARCHIVE DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
        RUNTIME DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
    )
endif()

if(NLM_ENABLE_COGNITION)
    install(TARGETS pynlm_cognition
        LIBRARY DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
        ARCHIVE DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
        RUNTIME DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
    )
endif()

if(NLM_ENABLE_PREDICTION)
    install(TARGETS pynlm_prediction
        LIBRARY DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
        ARCHIVE DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
        RUNTIME DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
    )
endif()

if(NLM_ENABLE_DEVELOPMENT)
    install(TARGETS pynlm_development
        LIBRARY DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
        ARCHIVE DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
        RUNTIME DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
    )
endif()

if(NLM_ENABLE_VISUALIZATION)
    install(TARGETS pynlm_visualization
        LIBRARY DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
        ARCHIVE DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
        RUNTIME DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
    )
endif()

if(NLM_ENABLE_PERFORMANCE_OPTIMIZATIONS)
    install(TARGETS pynlm_performance
        LIBRARY DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
        ARCHIVE DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
        RUNTIME DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
    )
endif()

# Install Python package files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/__init__.py
    DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}
)

# Create and install CMake configuration for find_package
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pynlmConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/pynlmConfig.cmake
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/pynlmConfig.cmake
    DESTINATION ${Python_SITEARCH}/${PYTHON_PACKAGE_NAME}/cmake
)

# Create Python setup configuration for development
file(WRITE
    ${CMAKE_CURRENT_BINARY_DIR}/setup.py
    "import sys
import os
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

# Pybind11 extension configuration
ext_modules = []

# Add core extension
if os.path.exists('${CMAKE_CURRENT_BINARY_DIR}/pynlm_core.so'):
    ext_modules.append(Extension(
        'pynlm.pynlm_core',
        sources=[],
        libraries=[],
        library_dirs=[],
        extra_compile_args=[],
        extra_link_args=[]
    ))

# Add optional extensions
${OPTIONAL_EXTENSIONS}

setup(
    name='${PYTHON_PACKAGE_NAME}',
    version='${PROJECT_VERSION}',
    description='Python bindings for NLM - Neural Learning Machine',
    author='NLM Team',
    packages=['pynlm'],
    ext_modules=ext_modules,
    install_requires=[],
    python_requires='>=3.8',
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: C++',
    ],
    zip_safe=False,
)
"
)

# Create Python virtual environment setup script
file(WRITE
    ${CMAKE_CURRENT_BINARY_DIR}/setup_venv.py
    "import subprocess
import sys
import os

# Create virtual environment
venv_path = 'venv'
if not os.path.exists(venv_path):
    print(f'Creating virtual environment at {venv_path}...')
    subprocess.run([sys.executable, '-m', 'venv', venv_path], check=True)

# Activate virtual environment
if sys.platform == 'win32':
    python_exe = os.path.join(venv_path, 'Scripts', 'python.exe')
else:
    python_exe = os.path.join(venv_path, 'bin', 'python')

# Install package in development mode
print('Installing package in development mode...')
subprocess.run([python_exe, '-m', 'pip', 'install', '-e', '.'], cwd=os.getcwd(), check=True)

print('Python development environment setup complete!')
print(f'Activate with: source {venv_path}/bin/activate  # On Unix')
print(f'Activate with: {venv_path}/Scripts/activate  # On Windows')
"
)

# Create test script for Python bindings
add_custom_target(pynlm_python_tests
    COMMAND ${Python_EXECUTABLE} -c \"import sys; sys.path.insert(0, '${CMAKE_CURRENT_BINARY_DIR}'); import pynlm_core; print('Core bindings test: PASSED')\"
    COMMAND ${Python_EXECUTABLE} -c \"import sys; sys.path.insert(0, '${CMAKE_CURRENT_BINARY_DIR}'); import pynlm_neuromodulation; print('Neuromodulation bindings test: PASSED')\"
    COMMAND ${Python_EXECUTABLE} -c \"import sys; sys.path.insert(0, '${CMAKE_CURRENT_BINARY_DIR}'); import pynlm_memory; print('Memory bindings test: PASSED')\"
    COMMAND ${Python_EXECUTABLE} -c \"import sys; sys.path.insert(0, '${CMAKE_CURRENT_BINARY_DIR}'); import pynlm_cognition; print('Cognition bindings test: PASSED')\"
    COMMAND ${Python_EXECUTABLE} -c \"import sys; sys.path.insert(0, '${CMAKE_CURRENT_BINARY_DIR}'); import pynlm_prediction; print('Prediction bindings test: PASSED')\"
    COMMAND ${Python_EXECUTABLE} -c \"import sys; sys.path.insert(0, '${CMAKE_CURRENT_BINARY_DIR}'); import pynlm_development; print('Development bindings test: PASSED')\"
    COMMAND ${Python_EXECUTABLE} -c \"import sys; sys.path.insert(0, '${CMAKE_CURRENT_BINARY_DIR}'); import pynlm_visualization; print('Visualization bindings test: PASSED')\"
    COMMAND ${Python_EXECUTABLE} -c \"import sys; sys.path.insert(0, '${CMAKE_CURRENT_BINARY_DIR}'); import pynlm_performance; print('Performance bindings test: PASSED')\"
    COMMENT "Testing Python bindings"
    VERBATIM
)

# Add Python tests to CTest if enabled
if(NLM_ENABLE_PYTHON_TESTS)
    enable_testing()
    add_test(NAME pynlm_python_bindings_test COMMAND pynlm_python_tests)
endif()

# Create documentation target for Python bindings
if(NLM_ENABLE_PYTHON_DOCS)
    find_program(SPHINX_BUILD sphinx-build)
    if(SPHINX_BUILD)
        add_custom_target(pynlm_python_docs
            COMMAND ${SPHINX_BUILD} -b html ${CMAKE_CURRENT_SOURCE_DIR}/python/docs ${CMAKE_CURRENT_BINARY_DIR}/docs/html/pynlm
            COMMENT "Building Python bindings documentation"
            VERBATIM
        )
    endif()
endif()
