from setuptools import setup, find_packages
import os
import sys

def read_requirements(filename):
    """Read requirements from file."""
    if not os.path.exists(filename):
        return []
    with open(filename, 'r') as f:
        return [line.strip() for line in f if line.strip() and not line.startswith('#')]

def read_version():
    """Read version from pyproject.toml or set default."""
    # Try to read version from pyproject.toml
    pyproject_path = os.path.join(os.path.dirname(__file__), 'pyproject.toml')
    if os.path.exists(pyproject_path):
        try:
            import tomli as tomllib
        except ImportError:
            try:
                import tomllib
            except ImportError:
                print("Warning: tomli or tomllib not available, using default version")
                return "0.1.0"
        
        with open(pyproject_path, 'rb') as f:
            data = tomllib.load(f)
            return data.get('project', {}).get('version', '0.1.0')
    
    return "0.1.0"

def read_description():
    """Read description from pyproject.toml."""
    pyproject_path = os.path.join(os.path.dirname(__file__), 'pyproject.toml')
    if os.path.exists(pyproject_path):
        try:
            import tomli as tomllib
        except ImportError:
            try:
                import tomllib
            except ImportError:
                return "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"
        
        with open(pyproject_path, 'rb') as f:
            data = tomllib.load(f)
            return data.get('project', {}).get('description', 
                                               "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework")
    
    return "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

# Read version and description
version = read_version()
description = read_description()

# Define packages
packages = find_packages()

# Python module setup
setup(
    name="pynlm",
    version=version,
    description=description,
    long_description="""
NLM (Neural Learning Machine) Python Bindings
===============================================

A Python binding for the NLM C++ neural simulation framework. This package provides
access to a sophisticated artificial brain simulation with neural development, learning,
memory, and cognition capabilities.

Features:
- Neural network simulation with biological learning rules
- Neuromodulation (dopamine, curiosity, novelty detection)
- Working memory and episodic memory systems
- Developmental stages and synaptic plasticity
- Environment interaction and motor control
- Prediction and planning capabilities
- Checkpoint saving and loading for persistence
- Complete Python API for easy experimentation

Usage:
    import pynlm
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    for i in range(100):
        brain.step(i)
    
Quick examples and tutorials are available in the documentation.
""",
    long_description_content_type="text/markdown",
    author="NLM Authors",
    author_email="nlm@example.com",
    url="https://github.com/nlm-project/nlm",
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
        "Topic :: Scientific/Engineering :: Artificial Neural Networks",
        "Framework :: Pybind11",
    ],
    keywords=[
        "neural", "simulation", "spiking", "brain", "learning", "ai",
        "computational-neuroscience", "artificial-intelligence", "machine-learning"
    ],
    packages=packages,
    include_package_data=True,
    install_requires=[],
    extras_require={
        "dev": [
            "pytest>=7.0",
            "numpy>=1.20",
            "scipy>=1.7",
            "matplotlib>=3.3",
            "jupyter>=1.0",
        ],
        "research": [
            "numpy>=1.20",
            "scipy>=1.7",
            "pandas>=1.3",
            "matplotlib>=3.3",
            "plotly>=5.0",
        ],
        "visualization": [
            "numpy>=1.20",
            "matplotlib>=3.3",
            "pyqtgraph>=0.13",
        ],
    },
    python_requires=">=3.8",
    entry_points={
        "console_scripts": [
            "nlm-demo = pynlm.utils.demo:main",
            "nlm-benchmark = pynlm.utils.benchmark:main",
            "nlm-explore = pynlm.utils.explore:main",
        ],
    },
    zip_safe=False,
    project_urls={
        "Documentation": "https://nlm.readthedocs.io",
        "Repository": "https://github.com/nlm-project/nlm",
        "Issues": "https://github.com/nlm-project/nlm/issues",
        "Discussions": "https://github.com/nlm-project/nlm/discussions",
    },
)
