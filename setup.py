#!/usr/bin/env python3
"""
Setup script for NLM Python package
"""

from setuptools import setup, find_packages
import os
import platform
import subprocess

# Read README for long description
with open('README.md', 'r', encoding='utf-8') as f:
    long_description = f.read()

# Read version from __init__.py
version = '0.1.0'

# Check if C++ extensions are available
try:
    # Try to build C++ extensions
    subprocess.run(['python', 'setup.py', 'build_ext', '--inplace'], check=True, capture_output=True)
    has_cpp_extensions = True
except Exception as e:
    print(f"Warning: Could not build C++ extensions: {e}")
    has_cpp_extensions = False

setup(
    name='pynlm',
    version=version,
    description='Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework',
    long_description=long_description,
    long_description_content_type='text/markdown',
    author='NLM Authors',
    author_email='nlm@example.com',
    url='https://github.com/nlm-project/nlm',
    packages=find_packages(include=['nlm*', 'pynlm', 'python']),
    package_data={
        '': ['*.py', '*.cpp', '*.hpp', '*.md', '*.txt', '*.json', '*.cfg'],
        'pynlm': ['*.so', '*.pyd'],
    },
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Science/Research',
        'Intended Audience :: Education',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
        'Programming Language :: C++',
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
        'Topic :: Scientific/Engineering :: Bioinformatics',
        'Topic :: Software Development :: Libraries :: Python Modules',
    ],
    keywords=['neural', 'simulation', 'spiking', 'brain', 'learning', 'ai', 'brain-emulation'],
    python_requires='>=3.8',
    install_requires=[
        'numpy>=1.20',
        'pandas>=1.3.0',
        'matplotlib>=3.3.0',
        'seaborn>=0.11.0',
        'scipy>=1.7.0',
        'scikit-learn>=1.0',
        'tqdm>=4.60.0',
        'joblib>=1.0.0',
    ],
    extras_require={
        'dev': [
            'pytest>=7.0',
            'pytest-cov>=3.0',
            'black>=21.12b0',
            'isort>=5.10.0',
            'flake8>=4.0.0',
            'mypy>=0.910',
            'sphinx>=4.0.0',
            'sphinx-rtd-theme>=1.0.0',
            'myst-parser>=0.15.0',
            'jupyter>=1.0.0',
            'ipykernel>=6.0.0',
        ],
        'full': [
            'tensorflow>=2.8.0',
            'torch>=1.9.0',
            'keras>=2.8.0',
            'xgboost>=1.5.0',
            'lightgbm>=3.2.0',
            'catboost>=0.26.0',
            'hmmlearn>=0.2.5',
            'statsmodels>=0.13.0',
            'gym>=0.21.0',
            'pyglet>=1.0.0',
        ],
        'debug': [
            'line_profiler>=2.1.0',
            'memory_profiler>=0.60.0',
            'python-profiler>=1.0.0',
        ],
        'visualization': [
            'plotly>=5.0.0',
            'plotly-express>=0.1.0',
            'kaleido>=0.2.1',
            'pyvis==0.1.0',
            'networkx>=2.6.0',
            'igraph>=0.10.0',
        ],
    },
    entry_points={
        'console_scripts': [
            'nlm = nlm_python_module:main',
            'nlm-explore = nlm_configuration_module:main',
            'nlm-analyze = nlm_analysis_module:main',
            'nlm-ml = nlm_ml_module:main',
            'nlm-debug = nlm_debug_module:main',
            'nlm-configure = nlm_configuration_module:main',
        ],
        'nlm.plugins': [
            'default = nlm.plugins.default',
            'neural = nlm.plugins.neural',
            'plasticity = nlm.plugins.plasticity',
            'neuromodulation = nlm.plugins.neuromodulation',
        ],
    },
    python_requires='>=3.8',
    zip_safe=False,
    include_package_data=True,
    project_urls={
        'Homepage': 'https://github.com/nlm-project/nlm',
        'Documentation': 'https://nlm.readthedocs.io',
        'Repository': 'https://github.com/nlm-project/nlm.git',
        'Issues': 'https://github.com/nlm-project/nlm/issues',
        'Changelog': 'https://github.com/nlm-project/nlm/releases',
    },
)
