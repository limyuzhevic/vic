#!/usr/bin/env python3
"""
Setup script for NLM Python package.

This script handles the installation and configuration of the NLM Python bindings.
"""

import os
import sys
import platform
import subprocess
import shutil
from pathlib import Path

def check_python_version():
    """Check if Python version is compatible."""
    version = sys.version_info
    if version.major < 3 or (version.major == 3 and version.minor < 8):
        print("ERROR: Python 3.8 or higher is required")
        return False
    
    print(f"Python {version.major}.{version.minor}.{version.micro}")
    return True
def check_pybind11():
    """Check if pybind11 is available."""
    try:
        import pybind11
        print(f"pybind11 version: {pybind11.__version__}")
        return True
    except ImportError:
        print("ERROR: pybind11 not found")
        return False
def check_nlohmann_json():
    """Check if nlohmann/json is available."""
    try:
        import nlohmann_json
        print(f"nlohmann/json available")
        return True
    except ImportError:
        # Try to find it in system
        try:
            import pybind11.nlohmann_json
            print(f"nlohmann/json available via pybind11")
            return True
        except ImportError:
            print("WARNING: nlohmann/json not found - JSON config support will be limited")
            return False
def build_cpp_extensions():
    """Build C++ extensions if needed."""
    print("Building C++ extensions...")
    
    # Check if we're in a development environment
    current_dir = Path(__file__).parent
    src_dir = current_dir / "src"
    build_dir = current_dir / "build"
    
    if not build_dir.exists():
        build_dir.mkdir(parents=True)
    
    # Try to build with CMake
    cmake_args = [
        str(current_dir),
        "-DCMAKE_BUILD_TYPE=Release",
        "-DPYTHON_EXECUTABLE=" + sys.executable,
    ]
    
    try:
        # Run CMake
        result = subprocess.run(
            ["cmake"] + cmake_args,
            cwd=str(build_dir),
            capture_output=True,
            text=True
        )
        
        if result.returncode != 0:
            print(f"CMake configuration failed: {result.stderr}")
            return False
        
        # Run make
        result = subprocess.run(
            ["make", "-j4"],
            cwd=str(build_dir),
            capture_output=True,
            text=True
        )
        
        if result.returncode != 0:
            print(f"Build failed: {result.stderr}")
            return False
        
        print("C++ extensions built successfully")
        return True
        
    except subprocess.CalledProcessError as e:
        print(f"Build failed: {e}")
        return False
    except FileNotFoundError:
        print("CMake not found - skipping build")
        return True  # Don't fail if CMake not available
def install_python_package():
    """Install the Python package."""
    print("Installing Python package...")
    
    # Install pybind11 if needed
    try:
        import pybind11
    except ImportError:
        print("Installing pybind11...")
        subprocess.check_call([sys.executable, "-m", "pip", "install", "pybind11"])
    
    # Install other dependencies
    dependencies = [
        "numpy",
        "pybind11",
    ]
    
    print(f"Installing dependencies: {', '.join(dependencies)}")
    for dep in dependencies:
        subprocess.check_call([sys.executable, "-m", "pip", "install", dep])
def create_package_files():
    """Create package metadata files."""
    print("Creating package metadata files...")
    
    # Get current directory
    current_dir = Path(__file__).parent
    
    # Create setup.py
    setup_py = current_dir / "setup.py"
    if not setup_py.exists():
        content = '''from setuptools import setup, find_packages
import os

# Read the README for long description
with open(os.path.join(os.path.dirname(__file__), 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

# Read requirements
with open(os.path.join(os.path.dirname(__file__), 'requirements.txt'), encoding='utf-8') as f:
    requirements = f.read().splitlines()

# Package metadata
setup(
    name="pynlm",
    version="0.1.0",
    author="NLM Authors",
    author_email="contact@nlm-project.org",
    description="Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/nlm-project/nlm",
    packages=find_packages(),
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Programming Language :: C++",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
    ],
    python_requires=">=3.8",
    install_requires=requirements,
    extras_require={
        "dev": [
            "pytest>=7.0",
            "numpy>=1.20",
            "scikit-build-core>=0.5.0",
            "pybind11>=2.11.0",
        ],
    },
    include_package_data=True,
    zip_safe=False,
    entry_points={
        "console_scripts": [
            "nlm-benchmark=pynlm.benchmark:main",
            "nlm-test=pynlm.test:main",
        ],
    },
)
'''
        setup_py.write_text(content)
    
    # Create requirements.txt
    requirements_txt = current_dir / "requirements.txt"
    if not requirements_txt.exists():
        content = """numpy>=1.20
pybind11>=2.11.0
scikit-build-core>=0.5.0
"""
        requirements_txt.write_text(content)
    
    # Create MANIFEST.in
    manifest_in = current_dir / "MANIFEST.in"
    if not manifest_in.exists():
        content = """
include *.md
include *.txt
include *.cpp
include *.hpp
include *.h
include CMakeLists.txt
src/**
python/**
tests/**
"""
        manifest_in.write_text(content)
def main():
    """Main setup function."""
    print("NLM Python Package Setup")
    print("=" * 50)
    
    # Check Python version
    if not check_python_version():
        return 1
    
    # Check dependencies
    if not check_pybind11():
        print("Continuing without pybind11...")
    
    check_nlohmann_json()
    
    # Install Python package
    install_python_package()
    
    # Create package files
    create_package_files()
    
    # Build C++ extensions
    if not build_cpp_extensions():
        print("WARNING: Failed to build C++ extensions")
    
    print("\nSetup completed successfully!")
    print("\nTo install the package:")
    print("  pip install .")
    print("\nTo run tests:")
    print("  python -m pytest tests/")
    print("\nTo run the NLM demo:")
    print("  nlm_phase6_demo")
    
    return 0
if __name__ == "__main__":
    sys.exit(main())
