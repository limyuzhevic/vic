from setuptools import setup, find_packages
import os
import re

# Read the README for the long description
with open('README.md', 'r', encoding='utf-8') as f:
    readme = f.read()

# Read the version from pyproject.toml
def get_version():
    for line in open('pyproject.toml', 'r'):
        if line.strip().startswith('version = '):
            return line.split('=')[1].strip().strip('"')
    return '0.1.0'

# Read requirements from pyproject.toml
def get_requirements():
    # Since this is a C++/Python bindings project, there are no Python dependencies
    # But we might have optional dependencies
    return [
        # No required Python dependencies
    ]

# Read package description
def get_description():
    return "Python bindings for NLM (Neural Learning Machine) neural simulation framework"

# Extract keywords from description
def get_keywords():
    return [
        "neural", "simulation", "spiking", "brain", "learning",
        "artificial intelligence", "neuromorphic", "computational neuroscience"
    ]

# Get version
version = get_version()

# Package configuration
setup(
    # Basic package information
    name="pynlm",
    version=version,
    description=get_description(),
    long_description=readme,
    long_description_content_type="text/markdown",
    
    # Package discovery
    packages=find_packages(include=["python", "python.*"]),
    
    # Package structure
    package_dir={
        "": "python",
    },
    
    # Package data
    package_data={
        "": ["*.py", "*.md", "*.txt"],
    },
    
    # Dependencies
    install_requires=get_requirements(),
    
    # Optional dependencies
    extras_require={
        "dev": [
            "pytest>=7.0",
            "numpy>=1.20",
            "matplotlib>=3.0",  # For visualization
            "jupyter>=1.0",     # For notebooks
        ],
        "docs": [
            "sphinx>=4.0",
            "sphinx-rtd-theme>=1.0",
            "nbsphinx>=0.8",
        ],
        "examples": [
            "jupyter>=1.0",
            "matplotlib>=3.0",
        ],
    },
    
    # Python version requirement
    python_requires=">=3.8",
    
    # Platform compatibility
    platforms=["Linux", "MacOS X", "Windows"],
    
    # Classifications
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
        "Topic :: Software Development :: Libraries :: Python Modules",
        "Operating System :: POSIX :: Linux",
        "Operating System :: MacOS",
        "Operating System :: Microsoft :: Windows",
    ],
    
    # Authors
    author="NLM Authors",
    author_email="",
    
    # URLs
    url="https://github.com/nlm-project/nlm",
    repository="https://github.com/nlm-project/nlm.git",
    documentation="https://nlm.readthedocs.io",
    
    # Keywords
    keywords=["neural", "simulation", "spiking", "brain", "learning"],
    
    # License
    license="MIT",
    
    # Entry points
    entry_points={
        "console_scripts": [
            "nlm-benchmark=python.benchmark_pynlm:main",
            "nlm-test=python.test_pynlm:run_all_basic_tests",
        ],
    },
    
    # Project classifiers for PyPI
    project_urls={
        "Documentation": "https://nlm.readthedocs.io",
        "Source Code": "https://github.com/nlm-project/nlm",
        "Issue Tracker": "https://github.com/nlm-project/nlm/issues",
        "Discussions": "https://github.com/nlm-project/nlm/discussions",
        "Examples": "https://github.com/nlm-project/nlm/tree/main/examples",
    },
)
