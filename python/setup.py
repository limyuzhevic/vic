from setuptools import setup, find_packages
import os
import sys
import subprocess
from pathlib import Path

# Try to build the C++ extension
def build_extension():
    """Build the C++ extension if needed."""
    build_dir = Path(__file__).parent.parent / "build"
    
    # Check if C++ extension already exists
    ext_pattern = Path(__file__).parent / "pynlm*.so"
    if list(ext_pattern.parent.glob(ext_pattern.name)):
        print("✓ C++ extension found, skipping build")
        return
    
    print("Building C++ extension...")
    
    # Create build directory
    build_dir.mkdir(exist_ok=True)
    
    # Configure with CMake
    cmake_cmd = [
        "cmake",
        str(Path(__file__).parent.parent),
        "-DCMAKE_BUILD_TYPE=Release",
        f"-DPYTHON_EXECUTABLE={sys.executable}"
    ]
    
    try:
        result = subprocess.run(cmake_cmd, cwd=build_dir, capture_output=True, text=True, timeout=120)
        if result.returncode != 0:
            print(f"CMake configuration failed: {result.stderr}")
            print("You can manually build with:")
            print("  cd build && cmake .. -DCMAKE_BUILD_TYPE=Release")
            print("  cd build && make pynlm")
            raise RuntimeError("CMake configuration failed")
        
        # Build extension
        build_cmd = ["cmake", "--build", ".", "--target", "pynlm", "--config", "Release"]
        result = subprocess.run(build_cmd, cwd=build_dir, capture_output=True, text=True, timeout=300)
        if result.returncode != 0:
            print(f"Build failed: {result.stderr}")
            raise RuntimeError("Build failed")
        
        print("✓ C++ extension built successfully")
    except subprocess.TimeoutExpired:
        print("Build timed out. You can manually build:")
        print("  cd build && cmake .. -DCMAKE_BUILD_TYPE=Release")
        print("  cd build && make pynlm")
        raise RuntimeError("Build timed out")

# Build the extension before setup
try:
    build_extension()
except Exception as e:
    print(f"✗ Warning: Failed to build C++ extension: {e}")
    print("Trying to continue without building...")

setup(
    name="pynlm",
    version="0.1.0",
    description="Python bindings for NLM (Neural Learning Machine)",
    long_description="""
NLM (熙然) - Neural Learning Machine Python Bindings

This module provides Python bindings for the NLM C++ neural simulation framework,
allowing users to interact with the artificial brain system through Python.

The NLM simulates real spiking neural computation with:
- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)
- Integrated memory, prediction, cognition, and neuromodulation systems

For installation from source:
1. Ensure you have pybind11 and CMake installed
2. Run: pip install -e .
3. Or manually build with: python setup.py build_ext --inplace

For usage examples, see the documentation in the package.
""",
    long_description_content_type="text/markdown",
    author="NLM Authors",
    license="MIT",
    url="https://github.com/nlm-project/nlm",
    packages=find_packages(where="python"),
    package_dir={"": "python"},
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Programming Language :: C++",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Software Development :: Libraries :: Python Modules",
    ],
    python_requires=">=3.8",
    install_requires=[
        "numpy>=1.20",
    ],
    extras_require={
        "dev": [
            "pytest>=7.0",
            "black>=22.0",
            "isort>=5.0",
            "mypy>=0.990",
            "sphinx>=4.0",
            "sphinx-rtd-theme>=1.0",
        ],
        "test": [
            "pytest>=7.0",
            "numpy>=1.20",
        ],
    },
    entry_points={
        "console_scripts": [
            "pynlm-demo = python.main:main",
            "nlm-test = python.main:main",
        ],
    },
    zip_safe=False,
    include_package_data=True,
    package_data={
        "python": [
            "*.so",
            "*.dll",
            "*.dylib",
        ],
    },
)