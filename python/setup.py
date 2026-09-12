#!/usr/bin/env python3
"""Setup script for NLM Python package"""

from setuptools import setup, find_packages
from pathlib import Path
import subprocess
import os

# Read README for long description
readme_path = Path(__file__).parent / "README.md"
long_description = readme_path.read_text() if readme_path.exists() else ""

try:
    # Try to read version from pyproject.toml
    import tomllib
    with open("pyproject.toml", "rb") as f:
        pyproject = tomllib.load(f)
        version = pyproject["project"]["version"]
except Exception:
    version = "0.1.0"

# Check if the C++ extension can be built
def check_build():
    """Check if the C++ extension can be built"""
    try:
        # Try to import pynlm to see if it's already built
        import pynlm
        return True
    except ImportError:
        # Check if there's a build directory
        if os.path.exists("build"):
            return True
        return False

# Read requirements
install_requires = [
    "numpy>=1.20",
]

# Optional development dependencies
dev_requires = [
    "pytest>=7.0",
    "pytest-cov",
    "matplotlib>=3.0",
    "jupyter>=1.0",
]

# Main setup
setup(
    name="pynlm",
    version=version,
    description="Python bindings for NLM (Neural Learning Machine) - Experimental Artificial Developmental Brain",
    long_description=long_description,
    long_description_content_type="text/markdown",
    author="NLM Authors",
    author_email="research@nlm-project.org",
    url="https://github.com/nlm-project/nlm",
    packages=find_packages(include=["python", "python.*"]),
    package_data={
        "python": [
            "*.py",
            "examples.py",
        ],
        "": [
            "*.json",
            "*.cfg",
            "*.txt",
            "*.md",
        ],
    },
    include_package_data=True,
    install_requires=install_requires,
    extras_require={
        "dev": dev_requires,
        "all": install_requires + dev_requires,
    },
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "License :: OSI Approved :: MIT License",
    ],
    python_requires=">=3.8",
    zip_safe=False,
    entry_points={
        "console_scripts": [
            "nlm-demo = python.examples:main",
            "nlm-info = python.info:main",
        ],
    },
)
