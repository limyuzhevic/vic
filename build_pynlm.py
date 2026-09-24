#!/usr/bin/env python3
"""
Build script for NLM Python bindings.
This script compiles the existing NLM C++ library and adds Python bindings.
"""

import os
import sys
import subprocess
import tempfile
import shutil
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def run_command(cmd, cwd=None):
    """Run a command and check for success"""
    print(f"Running: {cmd}")
    result = subprocess.run(cmd, shell=True, cwd=cwd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error: {result.stderr}")
        return False
    return True

def check_build_tools():
    """Check if build tools are available"""
    tools = ['cmake', 'g++', 'python3']
    for tool in tools:
        if not run_command(f"which {tool}"):
            print(f"Error: {tool} is not available")
            return False
    return True

def build_cpp_library():
    """Build the NLM C++ core library"""
    print("Building NLM C++ core library...")
    
    build_dir = Path("build")
    if build_dir.exists():
        shutil.rmtree(build_dir)
    
    build_dir.mkdir()
    
    # Configure CMake
    if not run_command("cmake ..", cwd=build_dir):
        return False
    
    # Build
    if not run_command("make -j4", cwd=build_dir):
        return False
    
    # Install the library to a local directory for Python bindings
    lib_dir = Path("lib")
    lib_dir.mkdir(exist_ok=True)
    
    if not run_command(f"make install DESTDIR={lib_dir}", cwd=build_dir):
        return False
    
    return True

def build_python_bindings():
    """Build the Python bindings extension"""
    print("Building Python bindings...")
    
    # Try to import pybind11
    try:
        import pybind11
        pybind11_include = pybind11.get_include()
    except ImportError:
        print("Error: pybind11 is required")
        return False
    
    # Create a minimal setup.py for pybind11 extension
    setup_content = '''from setuptools import setup, Extension
import sys
import os

# Get pybind11 include path
try:
    import pybind11
    pybind11_include = pybind11.get_include()
except ImportError:
    print("Error: pybind11 is required")
    sys.exit(1)

# Define the extension module
pynlm_module = Extension(
    name='pynlm._pynlm',
    sources=[
        'python/bindings.cpp',
    ],
    include_dirs=[
        pybind11_include,
        '../src',
        'lib/usr/local/include',
    ],
    define_macros=[
        ('VERSION_INFO', '1.0.0'),
    ],
    cxx_std=20,
    extra_compile_args=[
        '-std=c++20',
        '-Wall',
        '-Wextra',
        '-Wpedantic',
        '-O2',
    ],
    extra_link_args=[],
    language='c++',
)

setup(
    name='pynlm',
    version='0.1.0',
    description='Python bindings for NLM (Neural Learning Machine)',
    ext_modules=[pynlm_module],
    packages=['pynlm'],
    package_dir={'': 'python'},
    install_requires=[
        'numpy>=1.20',
    ],
)
'''
    
    # Write setup.py
    setup_py_path = Path("setup.py")
    with open(setup_py_path, 'w') as f:
        f.write(setup_content)
    
    # Install the Python package
    if not run_command("pip install -e ."):
        return False
    
    return True

def run_tests():
    """Run the test suite"""
    print("Running tests...")
    
    if not run_command("python test_bindings.py"):
        return False
    
    return True

def main():
    """Main build function"""
    print("NLM Python Bindings Build Script")
    print("=" * 40)
    
    if not check_build_tools():
        print("Build tools check failed")
        return 1
    
    if not build_cpp_library():
        print("C++ library build failed")
        return 1
    
    if not build_python_bindings():
        print("Python bindings build failed")
        return 1
    
    if not run_tests():
        print("Tests failed")
        return 1
    
    print("\nBuild successful!")
    print("\nTo use pynlm:")
    print("  import pynlm")
    print("  # Explore the API with help(pynlm)")
    print("  # Run tests with python test_bindings.py")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())