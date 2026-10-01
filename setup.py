#!/usr/bin/env python3
"""
Setup script for NLM Python package.

This script configures the NLM Python package for installation
using pybind11 and scikit-build-core.
"""

import sys
import os
from setuptools import setup, find_packages
from setuptools.command.build_py import build_py
from pathlib import Path

# Get the directory where this setup.py is located
THIS_DIR = Path(__file__).parent

# Read requirements from requirements.txt
REQUIREMENTS = []
REQUIREMENTS_FILE = THIS_DIR / "requirements.txt"
if REQUIREMENTS_FILE.exists():
    with open(REQUIREMENTS_FILE, 'r') as f:
        REQUIREMENTS = [line.strip() for line in f if line.strip() and not line.startswith('#')]

# Read version from __init__.py
VERSION = "0.1.0"
INIT_FILE = THIS_DIR / "__init__.py"
if INIT_FILE.exists():
    with open(INIT_FILE, 'r') as f:
        content = f.read()
        for line in content.split('\n'):
            if '__version__' in line and '=' in line:
                VERSION = line.split('=')[1].strip().strip('"\'')
                break

class BuildPyCommand(build_py):
    """Custom build command for Python package."""
    
    def run(self):
        # Ensure the Python bindings module can be built
        print("Building NLM Python package...")
        super().run()

setup(
    name="nlm",
    version=VERSION,
    description="NLM (Neural Learning Machine) Python Bindings",
    long_description=open(THIS_DIR / "README.md").read() if (THIS_DIR / "README.md").exists() else "",
    long_description_content_type="text/markdown",
    author="NLM Authors",
    author_email="authors@nlm-project.org",
    url="https://github.com/nlm-project/nlm",
    license="MIT",
    packages=find_packages() + ["python"],
    package_dir={"": str(THIS_DIR)},
    include_package_data=True,
    install_requires=REQUIREMENTS,
    extras_require={
        "dev": [
            "pytest>=7.0",
            "pytest-cov>=4.0",
            "pytest-asyncio>=0.21",
            "black>=23.0",
            "isort>=5.0",
            "mypy>=1.0",
            "flake8>=6.0",
        ],
        "docs": [
            "sphinx>=6.0",
            "sphinx-rtd-theme>=2.0",
            "sphinx-autodoc-typehints>=2.0",
        ],
        "examples": [
            "matplotlib>=3.7",
            "jupyter>=1.0",
        ],
    },
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Science/Research",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Scientific/Engineering :: Neural Networks",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Programming Language :: C++",
        "Operating System :: OS Independent",
    ],
    python_requires=">=3.8",
    zip_safe=False,
    keywords="neural network brain simulation spiking neural network nlm neuroscience",
    project_urls={
        "Documentation": "https://nlm.readthedocs.io",
        "Source": "https://github.com/nlm-project/nlm",
        "Tracker": "https://github.com/nlm-project/nlm/issues",
    },
    cmdclass={
        'build_py': BuildPyCommand,
    },
    entry_points={
        "console_scripts": [
            "nlm-python=nlm.main:main",
            "nlm-sim=nlm.examples:main",
        ],
    },
)
