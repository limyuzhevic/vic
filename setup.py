# setup.py

import os
import platform
import subprocess
import sys
from pathlib import Path
from setuptools import setup, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from distutils.version import LooseVersion

# Add the src directory to the path so we can find the C++ libraries
sys.path.insert(0, str(Path(__file__).parent / "src"))

class PyBind11BuildExt(build_ext):
    """Custom build extension for pybind11."""
    
    def run(self):
        # First, try to build the core C++ library
        try:
            self.build_core_library()
        except Exception as e:
            print(f"Warning: Failed to build core library: {e}")
            print("Continuing with pybind11-only build...")
            
        # Then run the pybind11 build
        super().run()
        
    def build_core_library(self):
        """Build the core NLM C++ library."""
        build_dir = Path(__file__).parent / "build"
        build_dir.mkdir(exist_ok=True)
        
        # Configure with CMake
        cmake_args = [
            "cmake",
            "-S", str(Path(__file__).parent),
            "-B", str(build_dir),
            "-DCMAKE_BUILD_TYPE=Release",
            "-DBUILD_SHARED_LIBS=OFF",
            "-DBUILD_TESTING=OFF",
            "-DNLM_BUILD_PYTHON_BINDINGS=ON",
        ]
        
        if platform.system() == "Windows":
            cmake_args.extend([
                "-G", "Visual Studio 16 2019",
                "-A", "x64",
            ])
        else:
            cmake_args.extend([
                "-G", "Unix Makefiles",
                "-j", str(os.cpu_count() or 2),
            ])
            
        print("Configuring NLM core library...")
        result = subprocess.run(cmake_args, capture_output=True, text=True)
        if result.returncode != 0:
            raise RuntimeError(f"CMake configuration failed: {result.stderr}")
            
        print("Building NLM core library...")
        build_args = ["cmake", "--build", str(build_dir)]
        
        if platform.system() == "Windows":
            build_args.extend(["--config", "Release"])
        else:
            build_args.extend(["--", "-j", str(os.cpu_count() or 2)])
            
        result = subprocess.run(build_args, capture_output=True, text=True)
        if result.returncode != 0:
            raise RuntimeError(f"Build failed: {result.stderr}")

class InstallCommand(install):
    """Custom install command with post-install steps."""
    
    def run(self):
        # Run the standard install
        super().run()
        
        # Post-install steps
        self.post_install()
        
    def post_install(self):
        """Post-installation steps."""
        # Create example directory
        examples_dir = Path(__file__).parent / "examples"
        examples_dir.mkdir(exist_ok=True)
        
        # Create documentation
        docs_dir = Path(__file__).parent / "docs"
        docs_dir.mkdir(exist_ok=True)
        
        # Create tests directory
        tests_dir = Path(__file__).parent / "tests"
        tests_dir.mkdir(exist_ok=True)
        
        # Generate package metadata
        self.generate_metadata()
        
    def generate_metadata(self):
        """Generate package metadata files."""
        # Generate requirements.txt
        requirements = [
            "pybind11>=2.11.0",
            "numpy>=1.20",
            "psutil>=5.8.0",
            "termcolor>=1.1.0",
        ]
        
        requirements_path = Path(__file__).parent / "requirements.txt"
        with open(requirements_path, 'w') as f:
            f.write("\n".join(requirements))
        
        # Generate environment.yml for conda
        env_yml = Path(__file__).parent / "environment.yml"
        with open(env_yml, 'w') as f:
            f.write(f"""name: nlm
channels:
  - conda-forge
  - defaults

dependencies:
  - python>=3.8
  - pybind11>=2.11.0
  - numpy>=1.20
  - psutil>=5.8.0
  - termcolor>=1.1.0
  - cmake>=3.16
  - make
  - gcc_linux-64
  
  build:
  - pip install -e .
""")

# Define package requirements
REQUIREMENTS = [
    "pybind11>=2.11.0",
    "numpy>=1.20",
    "psutil>=5.8.0",
    "termcolor>=1.1.0",
]

# Define package metadata
SETUP_CONFIG = {
    "name": "pynlm",
    "version": "0.1.0",
    "description": "Python bindings for NLM (Neural Learning Machine) - Enhanced Pythonic API",
    "long_description": """
NLM (Neural Learning Machine) is an experimental artificial developmental brain project.
This package provides enhanced Python bindings for the NLM C++ neural simulation framework.

Key Features:
• Pythonic API with intuitive interfaces for brain, world, and agent
• Enhanced configuration management with validation
• Advanced memory systems (working and episodic memory)
• Neuromodulation and development systems
• Performance monitoring and utilities
• Context managers for automatic resource cleanup
• Comprehensive error handling and validation
• Examples and tutorials for easy getting started
    """,
    "author": "NLM Research Team",
    "author_email": "research@nlm.org",
    "url": "https://github.com/nlm-project/nlm",
    "license": "MIT",
    "classifiers": [
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
    ],
    "keywords": "neural network, brain, simulation, spiking neural, AI, machine learning, neuroscience",
    "packages": find_packages(include=["nlm", "nlm.*", "examples", "tests", "docs"]),
    "package_dir": {
        "nlm": "python",
        "examples": "examples",
        "tests": "tests",
        "docs": "docs",
    },
    "install_requires": REQUIREMENTS,
    "extras_require": {
        "dev": [
            "pytest>=7.0",
            "pytest-cov>=2.12.1",
            "black>=22.0",
            "flake8>=4.0.0",
            "mypy>=0.991",
            "pre-commit>=2.20.0",
        ],
        "docs": [
            "sphinx>=4.4.0",
            "sphinx-rtd-theme>=1.0.0",
            "sphinx-autodoc-typehints>=1.11.1",
        ],
        "perf": [
            "memory-profiler>=0.5.11",
            "cProfile2>=15.0.4",
        ],
    },
    "python_requires": ">=3.8",
    "entry_points": {
        "console_scripts": [
            "nlm-demo = examples.basic_usage:main",
            "nlm-test = tests.test_basic_api:main",
            "nlm-config = examples.config_tutorial:main",
        ],
    },
    "ext_modules": [],
    "cmdclass": {
        "build_ext": PyBind11BuildExt,
        "install": InstallCommand,
    },
    "zip_safe": False,
    "include_package_data": True,
    "exclude_package_data": {
        "nlm": ["__pycache__", "*.pyc", "*.pyo"],
    },
    "data_files": [
        ("bin", ["bin/nlm_demo.py"]),
        ("share/nlm/examples", ["examples/*.py"]),
        ("share/nlm/docs", ["docs/*.md"]),
    ],
    "project_urls": {
        "Documentation": "https://nlm.readthedocs.io",
        "Source": "https://github.com/nlm-project/nlm",
        "Tracker": "https://github.com/nlm-project/nlm/issues",
    },
}

def main():
    """Main setup function."""
    try:
        # Check if we're in a development environment
        if not any(Path(__file__).parent / f for f in ["CMakeLists.txt", "pyproject.toml", "pybind11"]):
            print("Warning: Could not find CMakeLists.txt or pyproject.toml")
            print("Proceeding with pybind11-only installation...")
            
        # Run setup
        setup(**SETUP_CONFIG)
        
    except Exception as e:
        print(f"Error during setup: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()