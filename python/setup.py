from setuptools import setup, find_packages
import os
import sys
import subprocess
import platform

# Read the long description from README.md
with open("python/README.md", "r", encoding="utf-8") as f:
    long_description = f.read()

# Try to read version from pyproject.toml or use default
def get_version():
    try:
        # Read version from pyproject.toml
        with open("pyproject.toml", "r") as f:
            for line in f:
                if "version =" in line:
                    version = line.split('=')[1].strip().strip('" ')
                    return version
    except Exception:
        pass
    return "0.1.0"

version = get_version()

# Try to determine Python extension needs
def get_extensions():
    # Check if we can find pybind11 and CMake
    try:
        import pybind11
        pybind11_include = pybind11.get_include()
        return True
    except ImportError:
        print("Warning: pybind11 not found. Python bindings may not be available.")
        return False

# Check if we're in a build directory
in_build = os.path.exists("build") and os.path.exists("include")

# Define package data
package_data = {
    "": [
        "*.md",
        "*.txt",
        "*.rst",
    ],
}

setup(
    name="pynlm",
    version=version,
    description="Python bindings for NLM (Neural Learning Machine) neural simulation framework",
    long_description=long_description,
    long_description_content_type="text/markdown",
    author="NLM Authors",
    author_email="",
    url="https://github.com/nlm-project/nlm",
    license="MIT",
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
        "Topic :: Software Development :: Libraries :: Python Modules",
    ],
    keywords="neural simulation brain learning spiking neurons plasticity",
    python_requires=">=3.8",
    packages=find_packages(),
    package_data=package_data,
    install_requires=[],
    extras_require={
        "dev": [
            "pytest>=7.0",
            "numpy>=1.20",
            "sphinx>=4.0",
            "myst-parser>=0.15",
            "matplotlib>=3.0",
        ],
        "docs": [
            "sphinx>=4.0",
            "myst-parser>=0.15",
            "sphinx-rtd-theme>=0.5",
        ],
        "test": [
            "pytest>=7.0",
            "numpy>=1.20",
        ],
    },
    # Python extension module
    ext_modules=[],
    # CMake build support
    cmdclass={
        "build_ext": None,  # Will be handled by scikit-build
    },
    # Entry points
    entry_points={
        "console_scripts": [
            "nlm-demo=examples:main",
            "nlm-explore=examples:run_all_examples",
        ],
    },
    # Metadata for package managers
    zip_safe=True,
    include_package_data=True,
    # Platform-specific settings
    platforms=["Linux", "MacOS", "Windows"],
)