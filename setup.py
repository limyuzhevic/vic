#!/usr/bin/env python3
"""
Simplified setup.py for NLM Python bindings.
Assumes the NLM C++ library is already built and available.
"""

import os
import sys
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

class CustomBuildExt(build_ext):
    """Custom build extension that handles pybind11 compilation"""
    
    def build_extension(self, ext):
        try:
            import pybind11
        except ImportError:
            print("Error: pybind11 is required. Install with: pip install pybind11")
            sys.exit(1)
        
        # Try to find the NLM library
        lib_paths = [
            './build/src',
            './build/src/..',
            '../build',
            './build',
        ]
        
        library_dirs = []
        for path in lib_paths:
            if os.path.exists(os.path.join(path, 'libnlm_core.a')):
                library_dirs.append(path)
            elif os.path.exists(os.path.join(path, 'libnlm_core.so')):
                library_dirs.append(path)
        
        if not library_dirs:
            print("Warning: Could not find NLM library. Please build it first:")
            print("  1. mkdir build && cd build")
            print("  2. cmake .. -DCMAKE_BUILD_TYPE=Release")
            print("  3. make -j4")
            print("\n  Then re-run this installation.")
        
        # Update extension with proper library linking
        ext.libraries = ['nlm_core']
        ext.library_dirs = library_dirs
        ext.include_dirs.append(pybind11.get_include())
        
        # Call the parent build extension
        super().build_extension(ext)

def get_extensions():
    """Get the list of extensions to build"""
    # Check if pybind11 is available
    try:
        import pybind11
    except ImportError:
        print("Error: pybind11 is required. Install with: pip install pybind11")
        sys.exit(1)
    
    # Define the extension module
    pynlm_module = Extension(
        name='pynlm._pynlm',
        sources=[
            'python/bindings.cpp',
        ],
        include_dirs=[
            pybind11.get_include(),
            '../src',
        ],
        define_macros=[
            ('VERSION_INFO', '1.0.0'),
        ],
        cxx_std=20,
        extra_compile_args=[
            '-std=c++20',
            '-Wall',
            '-Wextra',
            '-Wpedantic',
            '-O2',
        ],
        extra_link_args=[],
        language='c++',
    )
    
    return [pynlm_module]

setup(
    name='pynlm',
    version='0.1.0',
    description='Python bindings for NLM (Neural Learning Machine)',
    long_description='A comprehensive Python interface to the NLM C++ neural simulation framework',
    author='NLM Authors',
    license='MIT',
    ext_modules=get_extensions(),
    packages=['pynlm'],
    package_dir={'': 'python'},
    install_requires=[
        'numpy>=1.20',
        'pybind11>=2.11.0',
    ],
    setup_requires=[
        'pybind11>=2.11.0',
    ],
    python_requires='>=3.8',
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
    url='https://github.com/nlm-project/nlm',
    project_urls={
        'Homepage': 'https://nlm.readthedocs.io',
        'Repository': 'https://github.com/nlm-project/nlm.git',
        'Documentation': 'https://nlm.readthedocs.io',
    },
    cmdclass={
        'build_ext': CustomBuildExt,
    },
    zip_safe=False,
)