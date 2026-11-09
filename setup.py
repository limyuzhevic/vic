# Python package for NLM (Neural Learning Machine)
# Built with scikit-build-core

import os
import sys
from setuptools import setup
from pathlib import Path

# Read the README for the long description
readme_path = Path(__file__).parent / "README.md"
if readme_path.exists():
    with open(readme_path, "r", encoding="utf-8") as f:
        long_description = f.read()
else:
    long_description = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Define package data
package_data = {
    "pynlm": ["*.so", "*.pyd"]  # Compiled Python extension files
}

# Define dependencies
dependencies = [
    # Core dependencies - none currently
]

# Define optional dependencies (dev dependencies)
optional_dependencies = {
    "dev": [
        "pytest>=7.0",
        "numpy>=1.20",
    ],
}

# Define classifiers
classifiers = [
    "Development Status :: 3 - Alpha",
    "Intended Audience :: Science/Research",
    "License :: OSI Approved :: MIT License",
    "Programming Language :: Python :: 3",
    "Programming Language :: Python :: 3.8",
    "Programming Language :: Python :: 3.9",
    "Programming Language :: Python :: 3.10",
    "Programming Language :: Python :: 3.11",
    "Programming Language :: Python :: 3.12",
    "Topic :: Software Development :: Libraries :: Python Extensions",
]

# The setup function - will use scikit-build-core's build system
setup(
    name="pynlm",
    version="0.1.0",
    description="Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework",
    long_description=long_description,
    long_description_content_type="text/markdown",
    author="NLM Authors",
    license="MIT",
    keywords="neural, simulation, spiking, brain, learning",
    url="https://github.com/nlm-project/nlm",
    packages=["pynlm"],
    package_data=package_data,
    include_package_data=True,
    install_requires=dependencies,
    extras_require=optional_dependencies,
    classifiers=classifiers,
    python_requires=">=3.8",
    zip_safe=False,
)