#!/usr/bin/env python3
"""
Setup script for NLM Python bindings using setuptools.
This provides backward compatibility for older setuptools-based installations.
"""

from setuptools import setup, find_packages
from pathlib import Path

# Read the long description from README.md
this_directory = Path(__file__).parent
long_description = (this_directory / "README.md").read_text()

# Read version from pyproject.toml
def get_version():
    import tomllib
    pyproject = this_directory / "pyproject.toml"
    with open(pyproject, "rb") as f:
        data = tomllib.load(f)
    return data["project"]["version"]

setup(
    name="pynlm",
    version=get_version(),
    description="Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework",
    long_description=long_description,
    long_description_content_type="text/markdown",
    author="NLM Authors",
    license="MIT",
    python_requires=">=3.8",
    packages=find_packages(),
    package_dir={"": "python"},
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
    install_requires=[],
    extras_require={
        "dev": ["pytest>=7.0", "numpy>=1.20"],
    },
    url="https://github.com/nlm-project/nlm",
    project_urls={
        "Homepage": "https://github.com/nlm-project/nlm",
        "Documentation": "https://nlm.readthedocs.io",
        "Repository": "https://github.com/nlm-project/nlm.git",
    },
    zip_safe=False,
)
