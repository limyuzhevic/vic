"""
Setup script for building and installing NLM Python bindings.

This script builds the C++ extensions and creates a proper Python package
structure for NLM (Neural Learning Machine) Python bindings.
"""

import os
import sys
import platform
import subprocess
from pathlib import Path
from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install

# Read the version from the bindings.cpp file
version = "0.1.0"

def get_long_description():
    """Read the README.md file for the long description."""
    readme_path = Path(__file__).parent / "README.md"
    if readme_path.exists():
        with open(readme_path, 'r', encoding='utf-8') as f:
            return f.read()
    return "NLM (Neural Learning Machine) Python Bindings"

class NLMBuildExt(build_ext):
    """Custom build extension for NLM Python bindings."""
    
    def run(self):
        """Run the build extension."""
        self.build_extensions()
    
    def build_extensions(self):
        """Build all extensions with platform-specific settings."""
        for extension in self.extensions:
            extension.extra_compile_args = self.get_compile_args()
            extension.extra_link_args = self.get_link_args()
        
        super().build_extensions()
    
    def get_compile_args(self):
        """Get platform-specific compiler arguments."""
        args = ["-std=c++20", "-Wall", "-Wextra", "-Wpedantic"]
        
        if platform.system() == "Windows":
            args.extend(["/W4", "/permissive-"])
        else:
            args.extend(["-O2", "-fPIC", "-fstack-protector"])
        
        return args
    
    def get_link_args(self):
        """Get platform-specific linker arguments."""
        args = []
        
        if platform.system() == "Windows":
            args.extend(["/LTCG"])
        else:
            args.extend(["-fPIC", "-Wl,--gc-sections"])
        
        return args

def check_pybind11():
    """Check if pybind11 is available."""
    try:
        import pybind11
        return True
    except ImportError:
        print("ERROR: pybind11 is required to build NLM Python bindings.")
        print("Please install it with: pip install pybind11")
        return False

def check_cpp_compiler():
    """Check if a C++ compiler is available."""
    try:
        subprocess.run(["g++", "--version"], check=True, capture_output=True)
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        try:
            subprocess.run(["clang++", "--version"], check=True, capture_output=True)
            return True
        except (subprocess.CalledProcessError, FileNotFoundError):
            print("ERROR: No C++ compiler found. Please install g++ or clang++.")
            return False

def read_requirements():
    """Read requirements from requirements.txt if it exists."""
    req_path = Path(__file__).parent / "requirements.txt"
    if req_path.exists():
        with open(req_path, 'r') as f:
            return [line.strip() for line in f if line.strip() and not line.startswith('#')]
    return []

# Define the main extension module
extensions = [
    Extension(
        "pynlm_core",
        sources=[
            str(Path(__file__).parent / "python" / "bindings.cpp"),
            str(Path(__file__).parent / "src" / "brain" / "Brain.cpp"),
            str(Path(__file__).parent / "src" / "core" / "Config" / "Config.cpp"),
            str(Path(__file__).parent / "src" / "agent" / "AgentBrain.cpp"),
            str(Path(__file__).parent / "src" / "world" / "SimpleWorld.cpp"),
            str(Path(__file__).parent / "src" / "sensory" / "SensoryInput.cpp"),
            str(Path(__file__).parent / "src" / "motor" / "Action.cpp"),
            str(Path(__file__).parent / "src" / "agent" / "AgentBody.cpp"),
            str(Path(__file__).parent / "src" / "agent" / "SensoryPercept.cpp"),
        ],
        include_dirs=[
            str(Path(__file__).parent / "src"),
            str(Path(__file__).parent / "python"),
            "/usr/local/include",
            "/usr/include",
        ],
        language="c++",
        py_limited_api=True,
    ),
]

# Custom install command to handle post-installation steps
class NLMInstall(install):
    """Custom install command for NLM Python bindings."""
    
    def run(self):
        """Run the custom installation."""
        # First run the standard installation
        super().run()
        
        # Set up the package structure
        self.setup_package_structure()
    
    def setup_package_structure(self):
        """Set up the Python package structure."""
        # Create the pynlm package directory
        package_dir = Path(self.install_lib) / "pynlm"
        package_dir.mkdir(parents=True, exist_ok=True)
        
        # Copy the Python wrapper files
        wrapper_src = Path(__file__).parent / "python" / "pynlm" / "__init__.py"
        if wrapper_src.exists():
            import shutil
            shutil.copy(wrapper_src, package_dir)
        
        # Print installation summary
        print(f"\nNLM Python Bindings installation complete!")
        print(f"Python package location: {package_dir}")
        print(f"Version: {version}")
        print(f"Description: {get_long_description()[:200]}...")

# Main setup configuration
setup(
    name="pynlm",
    version=version,
    author="NLM Project Team",
    author_email="nlm-project@example.com",
    description="Python bindings for NLM (Neural Learning Machine) neural simulation framework",
    long_description=get_long_description(),
    long_description_content_type="text/markdown",
    url="https://github.com/nlm-project/nlm",
    license=__license__,
    keywords=["neural", "simulation", "spiking", "brain", "learning", "ai"],
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: C++",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Software Development :: Libraries :: Python Modules",
    ],
    python_requires=">=3.8",
    install_requires=read_requirements(),
    setup_requires=["pybind11>=2.11.0", "scikit-build-core>=0.5.0"],
    build_requires=["pybind11>=2.11.0", "scikit-build-core>=0.5.0"],
    ext_modules=extensions,
    cmdclass={
        "build_ext": NLMBuildExt,
        "install": NLMInstall,
    },
    packages=find_packages(include=["pynlm", "pynlm.*"]),
    package_dir={
        "": "python",
        "pynlm": "pynlm",
    },
    include_package_data=True,
    zip_safe=False,
    options={
        "build_ext": {
            "build_type": "Release",
            "cmake_args": [],
        }
    },
)
