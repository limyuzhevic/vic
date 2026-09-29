# NLM Python Package Setup
# Modern Python packaging for NLM (Neural Learning Machine)

from setuptools import setup, find_packages
import os
import sys
from pathlib import Path

# Read version from version file
version = "0.1.0"
version_file = Path(__file__).parent / "VERSION"
if version_file.exists():
    with open(version_file, "r") as f:
        version = f.read().strip()

# Read long description
long_description = ""
readme_file = Path(__file__).parent / "README.md"
if readme_file.exists():
    with open(readme_file, "r", encoding="utf-8") as f:
        long_description = f.read()

# Read requirements
def read_requirements():
    req_file = Path(__file__).parent / "requirements.txt"
    if req_file.exists():
        with open(req_file, "r") as f:
            return [line.strip() for line in f 
                   if line.strip() and not line.startswith("#")]
    return []

requirements = read_requirements()

# Python version requirement
python_requires = ">=3.8"

# Package metadata
setup(
    name="nlm",
    version=version,
    author="NLM Research Team",
    author_email="research@nlm.org",
    description="Neural Learning Machine - A computational brain simulator",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/nlm/nlm",
    packages=find_packages(include=["nlm", "nlm.*"]),
    package_dir={
        "nlm": "python/nlm",
        "": "python",
    },
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Science/Research",
        "Intended Audience :: Developers",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Scientific/Engineering :: Neuroscience",
        "Topic :: Software Development :: Libraries",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
    ],
    python_requires=python_requires,
    install_requires=requirements,
    extras_require={
        "dev": [
            "pytest>=7.0.0",
            "pytest-cov>=3.0.0",
            "black>=22.0.0",
            "flake8>=5.0.0",
            "mypy>=1.0.0",
            "sphinx>=5.0.0",
            "sphinx-rtd-theme>=1.0.0",
        ],
        "docs": [
            "sphinx>=5.0.0",
            "sphinx-rtd-theme>=1.0.0",
        ],
        "examples": [
            "jupyter>=1.0.0",
            "matplotlib>=3.0.0",
        ],
    },
    entry_points={
        "console_scripts": [
            "nlm=nlm.cli:main",
            "nlm-bench=nlm.benchmark:main",
        ],
    },
    include_package_data=True,
    zip_safe=False,
    project_urls={
        "Bug Reports": "https://github.com/nlm/nlm/issues",
        "Documentation": "https://nlm.readthedocs.io/",
        "Source Code": "https://github.com/nlm/nlm",
    },
)