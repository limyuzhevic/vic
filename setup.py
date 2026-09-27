#!/usr/bin/env python3
"""
Setup script for NLM Python bindings package.

This script provides package metadata and installation instructions for the
enhanced NLM Python bindings.
"""

from setuptools import setup, find_packages
import os

def read_readme():
    """Read the README file for package description."""
    readme_path = os.path.join(os.path.dirname(__file__), 'README.md')
    if os.path.exists(readme_path):
        with open(readme_path, 'r', encoding='utf-8') as f:
            return f.read()
    return ""

def get_requirements():
    """Get Python dependencies."""
    requirements = []
    
    # Add any Python-only dependencies here
    # For example:
    # requirements.append("numpy>=1.20")
    # requirements.append("scipy>=1.8")
    
    return requirements

setup(
    name="nlm-neural-learning-machine",
    version="0.1.0",
    description="Python bindings for NLM (Neural Learning Machine) - Experimental artificial developmental brain framework",
    long_description=read_readme(),
    long_description_content_type="text/markdown",
    author="NLM Project Authors",
    author_email="info@nlm-project.org",
    url="https://github.com/nlm-project/nlm",
    packages=find_packages(),
    install_requires=get_requirements(),
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
        "Topic :: Scientific/Engineering :: Neuroscience",
    ],
    python_requires=">=3.8",
    keywords="neural simulation brain learning plasticity neuromodulation cognition development",
    project_urls={
        "Documentation": "https://nlm.readthedocs.io",
        "Source": "https://github.com/nlm-project/nlm",
        "Issues": "https://github.com/nlm-project/nlm/issues",
        "Research": "https://nlm.readthedocs.io/research",
    },
    zip_safe=False,
    include_package_data=True,
    package_data={
        "": ["*.py", "*.md", "*.txt"],
    },
)