# -*- coding: utf-8 -*-
"""Setup script for NLM Python package."""

import os
from setuptools import setup, find_packages
from pathlib import Path

# Read the version from pyproject.toml
import tomli
with open("../pyproject.toml", "rb") as f:
    pyproject = tomli.load(f)

version = pyproject["project"]["version"]
description = pyproject["project"]["description"]
name = pyproject["project"]["name"]
license = pyproject["project"]["license"]["text"]
authors = [f"{a['name']}" for a in pyproject["project"]["authors"]]
keywords = pyproject["project"]["keywords"]
requires_python = pyproject["project"]["requires-python"]
dependencies = pyproject["project"].get("dependencies", [])
optional_dependencies = pyproject["project"]["optional-dependencies"]

# Package metadata
this_directory = Path(__file__).parent
long_description = ""
for readme_file in ["README.md", "../README.md", "../docs/README.md"]:
    readme_path = this_directory / readme_file
    if readme_path.exists():
        with open(readme_path, "r", encoding="utf-8") as f:
            long_description += f.read() + "\n\n"

# Define the package structure
package_data = {
    "": [
        "*.py",
        "*.md",
        "*.txt",
        "*.rst",
        "*.yaml",
        "*.yml",
    ]
}

# Create package setup
setup(
    name=name,
    version=version,
    description=description,
    long_description=long_description,
    long_description_content_type="text/markdown",
    license=license,
    author=authors[0] if authors else "NLM Authors",
    author_email="",
    url="https://github.com/nlm-project/nlm",
    packages=find_packages(),
    package_data=package_data,
    include_package_data=True,
    install_requires=dependencies,
    extras_require=optional_dependencies,
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
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
    ],
    python_requires=requires_python,
    keywords=keywords,
    project_urls={
        "Documentation": "https://nlm.readthedocs.io",
        "Repository": "https://github.com/nlm-project/nlm.git",
        "Issues": "https://github.com/nlm-project/nlm/issues",
    },
    platforms=["any"],
    zip_safe=True,
)