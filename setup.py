#!/usr/bin/env python3
"""
Setup script for NLM Python bindings.

This script builds the NLM Python extension module from the C++ bindings.
"""

import os
import sys
import subprocess
import platform
from pathlib import Path
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
class NLMBuildExt(build_ext):
    """
    Custom build extension for NLM Python bindings.
    
    This class handles building the NLM C++ extension module.
    """
    
    def run(self):
        """Run the build extension process."""
        self._ensure_build_environment()
        super().run()
    
    def _ensure_build_environment(self):
        """Ensure build environment is ready."""
        # Check for CMake
        if not self._check_cmake_available():
            self._install_cmake()
        
        # Check for C++ compiler
        self._check_compiler()
    
    def _check_cmake_available(self) -> bool:
        """Check if CMake is available."""
        try:
            subprocess.run(['cmake', '--version'], 
                          capture_output=True, check=True)
            return True
        except (subprocess.CalledProcessError, FileNotFoundError):
            return False
    
    def _install_cmake(self):
        """Install CMake if not available."""
        print("CMake not found. Attempting to install...")
        
        if platform.system() == 'Linux':
            subprocess.run(['sudo', 'apt-get', 'update'], check=False)
            subprocess.run(['sudo', 'apt-get', 'install', '-y', 'cmake'], check=False)
        elif platform.system() == 'Darwin':
            subprocess.run(['brew', 'install', 'cmake'], check=False)
        else:
            raise RuntimeError("CMake installation not supported on this platform")
    
    def _check_compiler(self):
        """Check if C++ compiler is available."""
        try:
            subprocess.run(['g++', '--version'], capture_output=True, check=True)
        except (subprocess.CalledProcessError, FileNotFoundError):
            raise RuntimeError("C++ compiler (g++) not found. Please install a C++ compiler.")
    
    def build_extensions(self):
        """Build the NLM extension module."""
        # Create build directory
        build_dir = Path('build')
        build_dir.mkdir(exist_ok=True)
        
        # Change to build directory
        original_cwd = os.getcwd()
        os.chdir(build_dir)
        
        try:
            # Run CMake configuration
            subprocess.run([
                'cmake', '..', 
                '-DCMAKE_BUILD_TYPE=Release',
                '-DPYTHON_EXECUTABLE=' + sys.executable
            ], check=True)
            
            # Build the extension
            subprocess.run(['make', '-j4'], check=True)
            
        finally:
            os.chdir(original_cwd)
        
        # Copy the built extension to the package
        self._copy_extension_to_package()
    
    def _copy_extension_to_package(self):
        """Copy the built extension to the package directory."""
        build_dir = Path('build')
        
        # Find the extension file
        extensions = list(build_dir.glob('**/*.so')) + list(build_dir.glob('**/*.pyd'))
        if not extensions:
            # Try to find it in the lib directory
            lib_dir = build_dir / 'lib'
            if lib_dir.exists():
                extensions = list(lib_dir.glob('*.so')) + list(lib_dir.glob('*.pyd'))
        
        if not extensions:
            raise RuntimeError("Could not find built extension module")
        
        # Use the first extension found
        extension_file = extensions[0]
        
        # Determine the target directory
        if sys.platform == 'win32':
            target_dir = Path('pynlm')
        else:
            target_dir = Path('pynlm')
        
        target_dir.mkdir(exist_ok=True)
        
        # Copy the extension
        if sys.platform == 'win32':
            # Windows
            target_file = target_dir / f'{extension_file.stem}.pyd'
        else:
            # Linux/macOS
            target_file = target_dir / f'{extension_file.stem}.so'
        
        subprocess.run(['cp', str(extension_file), str(target_file)], check=True)
        
        print(f"Extension copied to: {target_file}")
# Define the NLM extension module
nlm_extension = Extension(
    'pynlm',
    sources=[],
    define_macros=[('NLM_PYTHON_BINDINGS', '1')],
    language='c++',
    extra_compile_args=['-std=c++20', '-O3', '-Wall', '-Wextra'],
    extra_link_args=[],
)

# Read the version from pyproject.toml
def get_version():
    """Get version from pyproject.toml."""
    version = '0.1.0'
    try:
        with open('pyproject.toml', 'r') as f:
            for line in f:
                if 'version' in line and '"' in line:
                    version = line.strip().split('=')[1].strip().strip('"')
                    break
    except FileNotFoundError:
        pass
    return version
# Read the README for long description
def get_long_description():
    """Get long description from README."""
    try:
        with open('README.md', 'r') as f:
            return f.read()
    except FileNotFoundError:
        return "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"
# Setup the package
setup(
    name='nlm',
    version=get_version(),
    author='NLM Authors',
    description=__doc__.strip(),
    long_description=get_long_description(),
    long_description_content_type='text/markdown',
    url='https://github.com/nlm-project/nlm',
    packages=['pynlm', 'pynlm.submodules'],
    package_dir={'pynlm': 'pynlm'},
    ext_modules=[nlm_extension],
    cmdclass={'build_ext': NLMBuildExt},
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
        'Programming Language :: C++',
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
    ],
    python_requires='>=3.8',
    install_requires=[
        'numpy>=1.20',
    ],
    extras_require={
        'dev': [
            'pytest>=7.0',
            'numpy>=1.20',
        ],
        'test': [
            'pytest>=7.0',
            'numpy>=1.20',
        ],
    },
    include_package_data=True,
    zip_safe=False,
)