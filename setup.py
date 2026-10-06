#!/usr/bin/env python3
"""
Setup script for NLM Python bindings package
"""

from setuptools import setup, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from pathlib import Path
import sys
import subprocess
import os

class PybindExtension(build_ext):
    """Custom extension builder for pybind11 extensions"""
    
    def build_extension(self, ext):
        # This is a simplified version - in practice, you'd want to use
        # scikit-build or pybind11's proper build system
        extdir = Path(self.get_ext_fullpath(ext.name)).parent
        
        # Ensure the extension directory exists
        extdir.mkdir(parents=True, exist_ok=True)
        
        # Basic pybind11 extension compilation
        # In a real implementation, you'd use CMake or similar
        compile_args = ['-std=c++20', '-O3']
        
        # Add include directories
        include_dirs = [str(Path(__file__).parent / 'python')]
        
        # Compile the extension
        self.compiler.compile(
            sources=[ext.sources[0]],
            output_dir=str(extdir),
            include_dirs=include_dirs,
            extra_compile_args=compile_args,
            language='c++'
        )
        
        # Create the Python module
        module_path = extdir / f"{ext.name}.so"
        if module_path.exists():
            # Make it executable
            module_path.chmod(0o755)
class EnhancedInstall(install):
    """Enhanced install command with additional setup"""
    
    def run(self):
        # Run the standard install
        super().run()
        
        # Install test dependencies
        self._install_test_dependencies()
    
    def _install_test_dependencies(self):
        """Install test dependencies if available"""
        try:
            # Check if we can install test dependencies
            subprocess.check_call(
                [sys.executable, '-m', 'pip', 'install', '-r', 'requirements.txt'],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL
            )
        except (subprocess.CalledProcessError, FileNotFoundError):
            # If requirements.txt doesn't exist or pip fails, continue anyway
            pass

setup(
    name="pynlm",
    version="0.1.0",
    author="NLM Authors",
    author_email="",
    description="Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework",
    long_description=open("README.md").read() if Path("README.md").exists() else "",
    long_description_content_type="text/markdown",
    url="https://github.com/nlm-project/nlm",
    packages=find_packages(include=["python", "python.*"]),
    package_dir={
        "": "python",
    },
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
        "Programming Language :: C++",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
    ],
    python_requires=">=3.8",
    install_requires=[],
    setup_requires=[
        "pybind11>=2.11.0",
        "scikit-build-core>=0.5.0",
    ],
    extras_require={
        "dev": [
            "pytest>=7.0",
            "numpy>=1.20",
            "pandas>=1.3.0",
            "matplotlib>=3.5.0",
            "seaborn>=0.11.0",
            "memory_profiler>=0.5.0",
            "scikit-learn>=1.0.0",
            "pyyaml>=6.0",
            "pytest-html>=3.2.0",
            "pytest-cov>=3.0.0",
            "hypothesis>=6.0.0",
            "mock>=4.0.0",
            "black>=22.0.0",
            "ruff>=0.1.0",
        ],
        "test": [
            "pytest>=7.0",
            "numpy>=1.20",
            "pandas>=1.3.0",
        ],
    },
    cmdclass={
        "build_ext": PybindExtension,
        "install": EnhancedInstall,
    },
    zip_safe=False,
    keywords=["neural", "simulation", "spiking", "brain", "learning"],
    project_urls={
        "Homepage": "https://github.com/nlm-project/nlm",
        "Documentation": "https://nlm.readthedocs.io",
        "Repository": "https://github.com/nlm-project/nlm.git",
    },
)
