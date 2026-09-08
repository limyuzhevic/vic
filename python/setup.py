# NLM Python Bindings Setup Configuration

# This setup.py is designed for the NLM (Neural Learning Machine) project.
# It uses setuptools with scikit-build to handle the C++ extension built with pybind11.
# This setup enables installation via pip and provides proper package management.

import os
import sys
from setuptools import setup, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from pathlib import Path

# Project metadata
name = "pynlm"
version = "0.1.0"
description = "Python bindings for NLM (Neural Learning Machine) neural simulation framework"
author = "NLM Project Contributors"
author_email = "nlm-project@example.com"
license = "MIT"
url = "https://github.com/nlm-project/nlm"

# Read the long description from README.md
this_dir = Path(__file__).parent
long_description = (this_dir / "README.md").read_text(encoding='utf-8')
long_description_content_type = 'text/markdown'

# Dependencies for building and running
install_requires = [
    # Core dependencies for NLM Python interface
    # Add any pure Python dependencies here
]

# Build-time dependencies
build_requires = [
    'setuptools>=45.0',
    'wheel>=0.36.0',
    'scikit-build-core>=0.5.0',
    'pybind11>=2.11.0',
    'cmake>=3.16',
]

# Optional dependencies for development
extras_require = {
    'dev': [
        'pytest>=7.0',
        'numpy>=1.20',
        'matplotlib>=3.0',  # For visualization
        'notebook>=6.0',   # For interactive notebooks
        'jupyter>=1.0',
        'sphinx>=4.0',     # Documentation
        'myst-parser>=0.13', # Documentation
        'sphinx-rtd-theme>=0.5',
    ],
    'test': [
        'pytest>=7.0',
        'pytest-cov>=2.0',
        'numpy>=1.20',
        'coverage>=5.0',
    ],
    'docs': [
        'sphinx>=4.0',
        'myst-parser>=0.13',
        'sphinx-rtd-theme>=0.5',
        'nbsphinx>=0.8',   # For notebook documentation
        'jupyter>=1.0',
    ],
}

# Package structure
packages = find_packages(include=["python", "python.*"])

# Include Python source files
package_dir = {"": "python"}

# Entry points for command-line tools
try:
    from setuptools.command.develop import develop
    from setuptools.command.sdist import sdist
except ImportError:
    # Fallback for older setuptools
    develop = None
    sdist = None

# Custom build extension to ensure pybind11 is available
class CustomBuildExt(build_ext):
    """Custom build extension that ensures pybind11 is available."""
    
    def build_extensions(self):
        # Check for pybind11 availability
        try:
            import pybind11
            print(f"Found pybind11 version: {pybind11.__version__}")
        except ImportError:
            print("WARNING: pybind11 not found. Please install with: pip install pybind11")
            # We'll continue but the build may fail
        
        # Call parent implementation
        super().build_extensions()

# Custom install to provide helpful information
class CustomInstall(install):
    """Custom install that provides helpful information."""
    
    def run(self):
        install.run(self)
        
        # Print helpful message after installation
        if self.root is None:
            print("\n" + "="*60)
            print("NLM Python Bindings Installation Complete!")
            print("="*60)
            print("\nNext steps:")
            print("1. Test the installation:")
            print("   python -c \"import pynlm; print('pynlm version:', pynlm.__version__)\"")
            print("\n2. Run the test suite:")
            print("   python test_bindings.py")
            print("\n3. Create a simple simulation:")
            print("   See examples in the repository or the documentation")
            print("\n4. Check the documentation:")
            print("   See python/README.md for API reference")
            print("="*60 + "\n")

# Package data - include all necessary files
package_data = {
    "python": [
        "*.py",
        "*.pyx",
        "*.pxd",
        "LICENSE",
        "README.md",
        "CHANGELOG.md",
        "docs/*.md",
    ]
}

# Entry points
entry_points = {
    "console_scripts": [
        "nlm-test=test_bindings:main",
        "pynlm-info=python.nlm:info",
    ],
}

# Scripts for the package
scripts = [
    "python/scripts/nlm-console.py",
]

# Setup configuration
setup(
    name=name,
    version=version,
    description=description,
    long_description=long_description,
    long_description_content_type=long_description_content_type,
    author=author,
    author_email=author_email,
    license=license,
    url=url,
    packages=packages,
    package_dir=package_dir,
    package_data=package_data,
    install_requires=install_requires,
    setup_requires=build_requires,
    extras_require=extras_require,
    cmdclass={
        'build_ext': CustomBuildExt,
        'install': CustomInstall,
    },
    ext_modules=[],  # Will be handled by scikit-build
    zip_safe=False,
    python_requires=">=3.8",
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
        "Programming Language :: Python :: 3.13",
        "Programming Language :: C++",
        "Framework :: PyQt",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Scientific/Engineering :: Neuroscience",
        "Topic :: Software Development :: Libraries :: Python Modules",
        "Operating System :: OS Independent",
        "Natural Language :: English",
    ],
    keywords=[
        "neural-networks",
        "brain-simulation",
        "spiking-neural-networks",
        "artificial-neural-networks",
        "deep-learning",
        "neuroscience",
        "computational-neuroscience",
        "brain-in-a-box",
        "neural-computation",
        "biomimetic",
        "learning-algorithms",
        "plasticity",
        "memory-systems",
        "cognition",
    ],
    project_urls={
        "Documentation": "https://nlm.readthedocs.io",
        "Source Code": "https://github.com/nlm-project/nlm",
        "Bug Reports": "https://github.com/nlm-project/nlm/issues",
        "Discussions": "https://github.com/nlm-project/nlm/discussions",
        "Research Paper": "https://arxiv.org/abs/xxxx.xxxxx",
    },
)
