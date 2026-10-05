#!/usr/bin/env python3
"""
Setup script for NLM (Neural Learning Machine) Python package.

This script provides modern package setup with comprehensive metadata,
dependency management, and configuration for building and distributing
the NLM neural simulation framework.
"""

import os
import sys
import platform
import subprocess
from pathlib import Path
from typing import List, Dict, Any, Optional

try:
    from setuptools import setup, find_packages
    from setuptools.command.build_ext import build_ext
    from setuptools.command.install import install
    from distutils.command.build import build
    import pybind11
except ImportError as e:
    print(f"Required dependency not available: {e}")
    print("Please install setuptools, pybind11, and other build dependencies")
    sys.exit(1)

# Package metadata
PACKAGE_NAME = "nlm"
VERSION = "0.1.0"
DESCRIPTION = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"
LONG_DESCRIPTION = """
NLM (Neural Learning Machine) is a comprehensive neural simulation framework
with advanced features including event-driven dynamics, integrated memory systems,
prediction and planning, neuromodulation, development, and curiosity-driven learning.

This Python package provides enhanced bindings for the NLM C++ framework with:
- Automatic numpy array conversion for Vision and Audio sensory inputs
- Numpy-based visualization functions for brain state
- Numpy-based data export/import functionality
- Comprehensive type hints and documentation using pybind11's API
- Python-specific debugging and timing utilities
- Improved error formatting for Python users

The package includes modules for:
- Core configuration and type management
- Brain simulation with regions, neurons, and synapses
- Agent interaction with world environments
- World simulation with objects and dynamics
- Visualization and data analysis utilities
- Configuration management with file and environment support
"""

AUTHOR = "NLM Authors"
AUTHOR_EMAIL = "nlm-authors@example.com"
URL = "https://github.com/nlm-project/nlm"
LICENSE = "MIT"

# Python version requirements
PYTHON_REQUIRES = ">=3.8"

# Dependencies
INSTALL_REQUIRES = [
    "numpy>=1.20",
    "pybind11>=2.11.0",
    "scikit-build-core>=0.5.0",
]

EXTRAS_REQUIRE = {
    "dev": [
        "pytest>=7.0",
        "pytest-cov",
        "black",
        "flake8",
        "mypy",
        "sphinx",
        "sphinx-rtd-theme",
    ],
    "visualization": [
        "matplotlib>=3.0",
        "seaborn>=0.11.0",
    ],
    "jupyter": [
        "jupyter",
        "ipykernel",
    ],
}

# Package classifiers
CLASSIFIERS = [
    "Development Status :: 3 - Alpha",
    "Intended Audience :: Science/Research",
    "Intended Audience :: Developers",
    "Topic :: Scientific/Engineering :: Artificial Intelligence",
    "Topic :: Scientific/Engineering :: Artificial Intelligence",
    "Topic :: Software Development :: Libraries :: Python Modules",
    "License :: OSI Approved :: MIT License",
    "Programming Language :: Python :: 3",
    "Programming Language :: Python :: 3.8",
    "Programming Language :: Python :: 3.9",
    "Programming Language :: Python :: 3.10",
    "Programming Language :: Python :: 3.11",
    "Programming Language :: Python :: 3.12",
    "Programming Language :: C++",
    "Operating System :: POSIX :: Linux",
    "Operating System :: MacOS",
    "Operating System :: Microsoft :: Windows",
    "Environment :: Console",
    "Environment :: X11 Applications",
    "Framework :: NumPy",
]

# Package structure
PACKAGES = find_packages(include=["nlm*", "tests*"], exclude=["tests.*", "*test*"])

# Package data
PACKAGE_DATA = {
    "nlm": [
        "py.typed",  # For type checking
        "*.json",
        "*.yaml",
        "*.yml",
    ],
    "nlm.*": [
        "*.json",
        "*.yaml",
        "*.yml",
    ],
}

# Entry points for command-line tools
ENTRY_POINTS = {
    "console_scripts": [
        "nlm-simulate = nlm.command_line:main",
        "nlm-export = nlm.command_line:export_command",
        "nlm-import = nlm.command_line:import_command",
        "nlm-config = nlm.command_line:config_command",
    ],
    "gui_scripts": [
        "nlm-explorer = nlm.visualization.gui:main",
    ],
}

# Package metadata
METADATA = {
    "name": PACKAGE_NAME,
    "version": VERSION,
    "description": DESCRIPTION,
    "long_description": LONG_DESCRIPTION,
    "long_description_content_type": "text/markdown",
    "author": AUTHOR,
    "author_email": AUTHOR_EMAIL,
    "url": URL,
    "license": LICENSE,
    "python_requires": PYTHON_REQUIRES,
    "install_requires": INSTALL_REQUIRES,
    "extras_require": EXTRAS_REQUIRE,
    "packages": PACKAGES,
    "package_data": PACKAGE_DATA,
    "classifiers": CLASSIFIERS,
    "entry_points": ENTRY_POINTS,
    "keywords": [
        "neural",
        "simulation",
        "spiking",
        "brain",
        "learning",
        "ai",
        "neuro",
        "neuroscience",
        "cognitive",
        "ml",
        "deep-learning",
    ],
    "project_urls": {
        "Homepage": URL,
        "Documentation": "https://nlm.readthedocs.io",
        "Repository": "https://github.com/nlm-project/nlm.git",
        "Issues": "https://github.com/nlm-project/nlm/issues",
        "Discussions": "https://github.com/nlm-project/nlm/discussions",
    },
    "setup_requires": [
        "pybind11>=2.11.0",
        "scikit-build-core>=0.5.0",
    ],
    "cmake_args": [
        "-DCMAKE_CXX_STANDARD=20",
        "-DCMAKE_CXX_STANDARD_REQUIRED=ON",
        "-DPYTHON_EXECUTABLE={python}",
        "-DNLM_BUILD_PYTHON_BINDINGS=ON",
        "-DNLM_BUILD_TESTS=OFF",
    ],
}

# Custom build commands

class CustomBuildExt(build_ext):
    """Custom build extension command with additional checks and optimizations."""
    
    def run(self) -> None:
        """Run the build extension command."""
        # Check for required dependencies
        self._check_dependencies()
        
        # Run the standard build_ext
        super().run()
    
    def _check_dependencies(self) -> None:
        """Check for required system dependencies."""
        # Check for C++ compiler
        try:
            subprocess.run(["gcc", "--version"], 
                         capture_output=True, check=True)
        except (subprocess.CalledProcessError, FileNotFoundError):
            print("Warning: C++ compiler not found. Please install gcc or clang.")
        
        # Check for Python headers
        try:
            import sysconfig
            include_dir = sysconfig.get_path('include')
            if not os.path.exists(include_dir):
                print(f"Warning: Python include directory not found: {include_dir}")
        except Exception as e:
            print(f"Warning: Could not check Python headers: {e}")

class CustomInstall(install):
    """Custom install command with additional configuration."""
    
    def run(self) -> None:
        """Run the install command."""
        # Run the standard install
        super().run()
        
        # Create necessary directories
        self._create_directories()
    
    def _create_directories(self) -> None:
        """Create necessary directories for the package."""
        # Create configuration directory
        config_dir = Path.home() / ".config" / "nlm"
        config_dir.mkdir(parents=True, exist_ok=True)
        
        # Create logs directory
        logs_dir = Path.home() / ".local" / "share" / "nlm" / "logs"
        logs_dir.mkdir(parents=True, exist_ok=True)
        
        # Create data directory
        data_dir = Path.home() / ".local" / "share" / "nlm" / "data"
        data_dir.mkdir(parents=True, exist_ok=True)
        
        print(f"Created configuration directory: {config_dir}")
        print(f"Created logs directory: {logs_dir}")
        print(f"Created data directory: {data_dir}")

# Custom build command for documentation
class BuildDocumentation(build):
    """Custom build command for documentation."""
    
    def run(self) -> None:
        """Run the documentation build."""
        print("Building documentation...")
        
        # Check if sphinx is available
        try:
            import sphinx
            
            # Build documentation if Makefile exists
            if os.path.exists("docs/Makefile"):
                subprocess.run(["make", "html"], cwd="docs", check=True)
                print("Documentation built successfully in docs/_build/html/")
            else:
                print("Warning: docs/Makefile not found, skipping documentation build")
                
        except ImportError:
            print("Warning: sphinx not available, skipping documentation build")
        
        # Call parent build
        super().run()

# Custom command for checking installation
cmdclass = {
    'build_ext': CustomBuildExt,
    'install': CustomInstall,
    'build': BuildDocumentation,
}

# Post-installation configuration
def post_install() -> None:
    """Run post-installation configuration."""
    print("Running post-installation configuration...")
    
    # Create default configuration file
    config_dir = Path.home() / ".config" / "nlm"
    config_file = config_dir / "nlm_config.py"
    
    if not config_file.exists():
        config_content = '''"""
Default NLM configuration file.

This file contains default configuration values for the NLM Python package.
It is located in the user's home directory at ~/.config/nlm/nlm_config.py
"""

# Default configuration values
CONFIG = {
    "simulation_steps": 1000,
    "enable_reward_modulation": True,
    "enable_structural_plasticity": True,
    "enable_development": True,
    "enable_curiosity": False,
    "thread_safe": False,
    "precision_mode": "single",
    "memory_pool_enabled": False,
    "brain_name": "default_brain",
    "random_seed": 42,
    "max_energy": 100.0,
    "energy_decay_rate": 0.01,
}

# Environment variable overrides
ENV_PREFIX = "NLM_"

# File format support
SUPPORTED_FORMATS = ["json", "py", "ini", "cfg", "toml"]
'''
        
        with open(config_file, 'w') as f:
            f.write(config_content)
        
        print(f"Created default configuration file: {config_file}")
    
    # Create initialization script
    init_script = Path.home() / ".local" / "bin" / "nlm-init"
    init_script.parent.mkdir(parents=True, exist_ok=True)
    
    if not init_script.exists():
        script_content = '''#!/bin/bash
# NLM initialization script

echo "Initializing NLM neural simulation framework..."

# Set up environment variables
export NLM_HOME="$HOME/.local/share/nlm"
export NLM_CONFIG="$HOME/.config/nlm/nlm_config.py"

# Create necessary directories
mkdir -p "$NLM_HOME/logs"
mkdir -p "$NLM_HOME/data"
mkdir -p "$NLM_HOME/checkpoints"
mkdir -p "$NLM_HOME/models"

echo "NLM initialized successfully."
echo "You can now run: nlm-simulate --help"
'''
        
        with open(init_script, 'w') as f:
            f.write(script_content)
        
        os.chmod(init_script, 0o755)
        print(f"Created initialization script: {init_script}")
    
    print("Post-installation configuration completed.")

# Main setup function
def setup_package() -> None:
    """Main setup function for the NLM package."""
    try:
        # Check if we're running in development mode
        if 'develop' in sys.argv:
            # Development installation
            setup(
                cmdclass=cmdclass,
                **METADATA
            )
        else:
            # Normal installation
            setup(
                cmdclass=cmdclass,
                **METADATA
            )
        
        # Run post-installation configuration
        post_install()
        
        print(f"\nSuccessfully installed {PACKAGE_NAME} version {VERSION}")
        print("\nNext steps:")
        print(f"  1. Run: nlm-init")
        print(f"  2. Run: nlm-simulate --help")
        print(f"  3. Run: pip install -e .[dev]  # for development")
        
    except Exception as e:
        print(f"Error during setup: {e}")
        sys.exit(1)

if __name__ == "__main__":
    setup_package()
