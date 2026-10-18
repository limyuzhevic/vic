"""
NLM Python Package Setup
========================

This package provides Python bindings for the NLM (Neural Learning Machine) framework.
NLM is an experimental computational brain project that implements a neural system
capable of learning, memory, and adaptive behavior.

Version: 0.1.0
Author: NLM Authors
License: MIT
"""

import os
import sys
import platform
from setuptools import setup, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from distutils.version import LooseVersion
import subprocess

class CustomBuildExt(build_ext):
    """Custom build extension to handle pybind11 compilation"""
    
    def run(self):
        try:
            # Check if pybind11 is available
            import pybind11
            print(f"Using pybind11 from: {pybind11.get_include()}")
        except ImportError:
            print("pybind11 not found in Python environment")
            print("Trying to install pybind11...")
            subprocess.check_call([sys.executable, "-m", "pip", "install", "pybind11"])
        
        # Run the standard build extension
        super().run()

class CustomInstall(install):
    """Custom install to handle module installation"""
    
    def run(self):
        # Run the standard install
        super().run()
        
        # Print installation success message
        print("\n" + "="*60)
        print("NLM Python Package Installation Complete!")
        print("="*60)
        print("\nTo get started with NLM Python bindings:")
        print("1. Import the package: import pynlm")
        print("2. Create a brain: brain = pynlm.createBrain(pynlm.createDefaultConfig())")
        print("3. Initialize: brain.initialize()")
        print("4. Run simulation: brain.step(step)")
        print("\nFor advanced features, see:")
        print("- examples/ directory for example scripts")
        print("- easy_usage.md for beginner guide")
        print("- HOW_TO_USE.md for comprehensive documentation")
        print("="*60)

def get_version():
    """Get version from pyproject.toml or default"""
    try:
        # Read version from pyproject.toml
        with open("pyproject.toml", "r") as f:
            for line in f:
                if "version =" in line:
                    return line.split('=')[1].strip().strip('"\'')
    except:
        pass
    return "0.1.0"

def check_python_version():
    """Check Python version compatibility"""
    if sys.version_info < (3, 8):
        print("Warning: Python 3.8+ is recommended for NLM Python bindings")
    return True

def check_dependencies():
    """Check required dependencies"""
    required = []
    
    # Try to import pybind11
    try:
        import pybind11
        print(f"✓ pybind11 found: {pybind11.__version__}")
    except ImportError:
        print("✗ pybind11 not found - will attempt to install")
        required.append("pybind11")
    
    # Try to import numpy (optional)
    try:
        import numpy
        print(f"✓ numpy found: {numpy.__version__}")
    except ImportError:
        print("⚠ numpy not found - optional dependency")
    
    return required

def main():
    print("NLM Python Package Setup")
    print("=" * 40)
    
    # Check Python version
    check_python_version()
    
    # Check dependencies
    deps = check_dependencies()
    
    # Get version
    version = get_version()
    print(f"Version: {version}")
    
    # Define package metadata
    setup(
        name="pynlm",
        version=version,
        description="Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework",
        long_description=open("README.md").read() if os.path.exists("README.md") else "",
        long_description_content_type="text/markdown",
        author="NLM Authors",
        license="MIT",
        url="https://github.com/nlm-project/nlm",
        packages=find_packages(),
        package_data={
            "": ["*.so", "*.pyd", "*.py", "*.md", "*.txt"],
        },
        include_package_data=True,
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
        install_requires=deps if deps else [],
        extras_require={
            "dev": [
                "pytest>=7.0",
                "numpy>=1.20",
                "matplotlib>=3.5",
                "notebook>=7.0",
            ],
            "examples": [
                "matplotlib>=3.5",
                "numpy>=1.20",
            ],
        },
        cmdclass={
            "build_ext": CustomBuildExt,
            "install": CustomInstall,
        },
        zip_safe=False,
        entry_points={
            "console_scripts": [
                "nlm-demo = python.demo:main",
                "nlm-test = python.test_enhanced_bindings:run_all_tests",
            ],
        },
    )

if __name__ == "__main__":
    main()