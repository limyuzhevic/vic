"""
Setup script for the enhanced NLM Python package.
"""

from setuptools import setup, find_packages
from pathlib import Path

# Read the long description from README.md
def read_readme():
    readme_path = Path(__file__).parent / "README.md"
    if readme_path.exists():
        with open(readme_path, "r", encoding="utf-8") as f:
            return f.read()
    return ""

def read_pyproject_toml():
    """Read configuration from pyproject.toml if available"""
    import tomllib
    
    pyproject_path = Path(__file__).parent / "pyproject.toml"
    if pyproject_path.exists():
        with open(pyproject_path, "rb") as f:
            return tomllib.load(f)
    return {}

# Try to read pyproject.toml for package metadata
try:
    pyproject = read_pyproject_toml()
    project_config = pyproject.get("project", {})
except Exception:
    project_config = {}

# Get package name and version
package_name = project_config.get("name", "pynlm")
version = project_config.get("version", "0.1.0-enhanced")
description = project_config.get("description", 
    "Enhanced Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework with high-level APIs and improved user experience")

# Read long description
long_description = read_readme()
if not long_description:
    long_description = description

# Read requirements
try:
    pyproject = read_pyproject_toml()
    optional_deps = pyproject.get("project", {}).get("optional-dependencies", {})
    dev_deps = optional_deps.get("dev", [])
except Exception:
    dev_deps = []

# Define package data
package_data = {
    'nlm': ['*.py', '*.pyi'],
}

# Define entry points
entry_points = {
    'console_scripts': [
        'nlm-sim = nlm.advanced_cli:cli',
        'nlm-explore = nlm.advanced_cli:cli',
        'nlm-learn = nlm.advanced_cli:cli',
        'nlm-checkpoint = nlm.advanced_cli:cli',
    ],
    'gui_scripts': [
        'nlm-visualizer = nlm.gui:launch_visualizer',
    ],
}

# Package configuration
setup(
    name=package_name,
    version=version,
    description=description,
    long_description=long_description,
    long_description_content_type="text/markdown",
    author="NLM Authors",
    license="MIT",
    keywords=["neural", "simulation", "spiking", "brain", "learning", "AI", "neuroscience"],
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Programming Language :: C++",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Scientific/Engineering :: Neuroscience",
        "Framework :: Pybind11",
    ],
    python_requires=">=3.8",
    packages=find_packages(exclude=["tests", "tests.*", "*.tests", "*.tests.*"]),
    package_data=package_data,
    include_package_data=True,
    install_requires=[],  # Core dependencies are only C++ libraries
    extras_require={
        "dev": dev_deps,
    },
    entry_points=entry_points,
    zip_safe=False,
    project_urls={
        "Homepage": "https://github.com/nlm-project/nlm",
        "Documentation": "https://nlm.readthedocs.io",
        "Repository": "https://github.com/nlm-project/nlm.git",
        "Bug Tracker": "https://github.com/nlm-project/nlm/issues",
    },
)
