#!/usr/bin/env python3
"""
Setup script for NLM Python package.

This script provides backward compatibility with traditional setup.py while
using the modern pyproject.toml configuration.
"""

from setuptools import setup
from pathlib import Path
import re

def get_version():
    """Get version from pyproject.toml."""
    content = Path("pyproject.toml").read_text()
    match = re.search(r'version\s*=\s*"([^"]+)"', content)
    return match.group(1) if match else "0.1.0"

def get_requirements():
    """Get requirements from pyproject.toml."""
    content = Path("pyproject.toml").read_text()
    requirements = []
    
    # Extract core dependencies
    match = re.search(r'dependencies\s*=\s*\[(.*?)\]', content, re.DOTALL)
    if match:
        deps_text = match.group(1)
        # Simple parsing - in production, use a proper TOML parser
        for line in deps_text.split('\n'):
            line = line.strip()
            if line and not line.startswith('#'):
                requirements.append(line.strip('", '))
    
    return requirements

def main():
    """Main setup function."""
    setup(
        name="nlm",
        version=get_version(),
        description="Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework with enhanced Pythonic API",
        long_description=Path("README.md").read_text(),
        long_description_content_type="text/markdown",
        author="NLM Authors",
        author_email="contact@nlm-project.org",
        url="https://github.com/nlm-project/nlm",
        license="MIT",
        python_requires=">=3.8",
        packages=[
            "nlm",
            "nlm.tests",
        ],
        package_dir={
            "nlm": "nlm",
            "nlm.tests": "tests",
        },
        install_requires=get_requirements(),
        extras_require={
            "dev": [
                "pytest>=7.0,<8.0",
                "pytest-asyncio>=0.19,<1.0",
                "pytest-cov>=3.0,<5.0",
                "black>=22.0,<24.0",
                "isort>=5.10,<6.0",
                "mypy>=0.991,<1.4",
                "flake8>=5.0,<7.0",
                "pylint>=2.15,<3.0",
                "pre-commit>=3.0,<4.0",
            ],
            "visualization": [
                "numpy>=1.20,<2.0",
                "matplotlib>=3.5,<4.0",
                "pillow>=9.0,<11.0",
            ],
            "scientific": [
                "numpy>=1.20,<2.0",
                "pandas>=1.3,<2.0",
                "scipy>=1.7,<2.0",
            ],
            "ml": [
                "torch>=1.13,<3.0; platform_system != 'Windows'",
                "tensorflow>=2.10,<3.0; platform_system != 'Windows'",
            ],
            "jupyter": [
                "jupyter>=1.0,<2.0",
                "ipykernel>=6.0,<7.0",
                "ipywidgets>=7.0,<9.0",
            ],
        },
        entry_points={
            "console_scripts": [
                "nlm=nlm.cli:main",
                "nlm-demo=nlm.examples:main",
            ],
        },
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
            "Topic :: Software Development :: Libraries :: Python Modules",
            "Framework :: Pytest",
        ],
        keywords=["neural", "simulation", "spiking", "brain", "learning", "ai"],
        project_urls={
            "Homepage": "https://github.com/nlm-project/nlm",
            "Documentation": "https://nlm.readthedocs.io",
            "Repository": "https://github.com/nlm-project/nlm.git",
            "Issues": "https://github.com/nlm-project/nlm/issues",
            "Discussions": "https://github.com/nlm-project/nlm/discussions",
        },
        include_package_data=True,
        zip_safe=False,
    )

if __name__ == "__main__":
    main()
