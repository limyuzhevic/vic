#!/usr/bin/env python3
"""
Setup script for NLM Python bindings.
"""

import os
import sys
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

# Try to import Cython, fall back to pre-generated C files
try:
    from Cython.Build import cythonize
    USE_CYTHON = True
except ImportError:
    USE_CYTHON = False

# Read the README for the long description
with open('README.md', 'r', encoding='utf-8') as f:
    long_description = f.read()

# Read requirements
requirements = []
if os.path.exists('requirements.txt'):
    with open('requirements.txt', 'r') as f:
        requirements = [line.strip() for line in f if line.strip()]

# NLM extension module
nlm_extension = Extension(
    'pynlm',
    sources=['pynlm.pyx'],
    include_dirs=[
        '../src',
        '../src/core',
        '../src/brain',
        '../src/plasticity',
        '../src/memory',
        '../src/prediction',
        '../src/cognition',
        '../src/neuromodulation',
        '../src/sensory',
        '../src/motor',
        '../src/environment',
        '../src/world',
        '../src/agent',
        '../src/experiments',
        '../src/performance',
    ],
    define_macros=[('CYTHON', '1'), ('NPY_NO_DEPRECATED_API', 'NPY_1_7_API_VERSION')],
    language='c++',
    extra_compile_args=['-std=c++20', '-O3', '-ffast-math', '-march=native'],
    extra_link_args=[],
    optional=True,
)

class BuildExtension(build_ext):
    """Custom build extension to handle Cython dependencies."""
    
    def run(self):
        if USE_CYTHON:
            self.run_command('cythonize')
        super().run()
    
    def build_extension(self, ext):
        # Handle Cython compilation
        if USE_CYTHON and hasattr(ext, 'pyx_sources'):
            ext.sources = ext.pyx_sources()
        
        # Call parent build method
        super().build_extension(ext)

# Package configuration
setup(
    name='pynlm',
    version='0.1.0',
    author='NLM Authors',
    author_email='nlm-project@example.com',
    description='Python bindings for NLM (Neural Learning Machine)',
    long_description=long_description,
    long_description_content_type='text/markdown',
    url='https://github.com/nlm-project/nlm',
    license='MIT',
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
    keywords=['neural', 'simulation', 'spiking', 'brain', 'learning', 'ai', 'neuroscience'],
    packages=['pynlm', 'pynlm.examples'],
    package_dir={
        'pynlm': 'python',
        'pynlm.examples': 'python/examples',
    },
    package_data={
        'pynlm': ['*.pyx', '*.pxd', '*.cpp', '*.hpp'],
        'pynlm.examples': ['*.py'],
    },
    include_package_data=True,
    install_requires=requirements,
    extras_require={
        'dev': [
            'pytest>=7.0',
            'numpy>=1.20',
            'cython>=3.0',
        ],
        'visualization': [
            'matplotlib>=3.0',
            'networkx>=2.0',
        ],
        'advanced': [
            'scikit-learn>=1.0',
            'pandas>=1.0',
        ],
    },
    python_requires='>=3.8',
    setup_requires=['cython>=3.0'] if USE_CYTHON else [],
    cmdclass={
        'build_ext': BuildExtension,
    },
    ext_modules=cythonize([nlm_extension]) if USE_CYTHON else [nlm_extension],
    zip_safe=False,
    entry_points={
        'console_scripts': [
            'nlm=pynlm.cli:main',
        ],
    },
)
