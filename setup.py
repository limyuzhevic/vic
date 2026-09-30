"""
Setup configuration for NLM Python API.

This module provides comprehensive setup and configuration for the NLM (Neural Learning Machine)
Python API, including high-level abstractions for simulations and experiments.
"""

import os
from setuptools import setup, find_packages
from setuptools.command.install import install
from typing import Dict, Any
class EnhancedInstallCommand(install):
    """Enhanced install command that creates necessary directory structure."""
    
    def run(self):
        # Call parent install
        super().run()
        
        # Create necessary directories
        directories = [
            "python/nlm",
            "python/examples",
            "python/tests",
            "docs/api_reference",
            "docs/tutorials"
        ]
        
        for directory in directories:
            os.makedirs(directory, exist_ok=True)
            
        # Create __init__.py files
        init_files = [
            "python/nlm/__init__.py",
            "python/__init__.py"
        ]
        
        for init_file in init_files:
            if not os.path.exists(init_file):
                with open(init_file, 'w') as f:
                    f.write("# NLM Python API - auto-generated")
def load_requirements() -> list:
    """Load requirements from requirements.txt if it exists."""
    requirements = [
        "numpy>=1.20.0",
        "psutil>=5.8.0",
        "matplotlib>=3.3.0",
        "PyYAML>=5.3.1"
    ]
    
    requirements_file = os.path.join(os.path.dirname(__file__), 'requirements.txt')
    if os.path.exists(requirements_file):
        with open(requirements_file, 'r') as f:
            requirements.extend(f.read().splitlines())
    
    return requirements
def get_long_description() -> str:
    """Get long description from README.md."""
    readme_file = os.path.join(os.path.dirname(__file__), 'README.md')
    if os.path.exists(readme_file):
        with open(readme_file, 'r') as f:
            return f.read()
    return ""
setup(
    name="nlm",
    version="2.0.0",
    description="Neural Learning Machine - Enhanced Python API for AI Simulation",
    long_description=get_long_description(),
    long_description_content_type="text/markdown",
    author="NLM Development Team",
    author_email="dev@nlm.ai",
    url="https://github.com/neural-learning-machine/nlm",
    packages=find_packages(where="python"),
    package_dir={"": "python"},
    install_requires=load_requirements(),
    extras_require={
        "dev": [
            "pytest>=6.0.0",
            "pytest-cov>=2.10.0",
            "black>=20.8b0",
            "isort>=5.0.0",
            "flake8>=3.8.0",
            "mypy>=0.800"
        ],
        "docs": [
            "sphinx>=3.0.0",
            "sphinx-rtd-theme>=0.5.0",
            "sphinx-autodoc-typehints>=1.11.0"
        ],
        "examples": [
            "jupyter>=1.0.0"
        ]
    },
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Science/Research",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Scientific/Engineering :: Artificial Life"
    ],
    python_requires=">=3.7",
    keywords="neural-networks, ai, simulation, machine-learning, cognitive-model",
    project_urls={
        "Documentation": "https://docs.nlm.ai",
        "Source": "https://github.com/neural-learning-machine/nlm",
        "Tracker": "https://github.com/neural-learning-machine/nlm/issues"
    },
    cmdclass={
        'install': EnhancedInstallCommand
    }
)
