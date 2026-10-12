"""
Setup script for the enhanced NLM Python bindings.

This script sets up the enhanced Python bindings for the Neural Learning Machine (NLM)
project, providing high-level research APIs for brain simulation and analysis.
"""

import os
import sys
from setuptools import setup, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from skbuild import setup as skbuild_setup

# Read the README for long description
with open('README.md', 'r', encoding='utf-8') as f:
    long_description = f.read()

# Read the pyproject.toml for configuration
with open('pyproject.toml', 'r', encoding='utf-8') as f:
    content = f.read()

# Parse minimal configuration from pyproject.toml
version = '0.1.0'
description = 'Python bindings for NLM (Neural Learning Machine) - Enhanced Research API'

here = os.path.abspath(os.path.dirname(__file__))

# Define the Python package structure
packages = [
    'pynlm',
    'pynlm.research',
    'pynlm.viz',
    'pynlm.analysis',
    'pynlm.examples'
]

# Define package data
package_data = {
    'pynlm': [
        '*.py',
        '*.md',
        'research/*.py',
        'viz/*.py',
        'analysis/*.py',
        'examples/*.py',
        'examples/*.ipynb'
    ]
}

# Define scripts
scripts = []

# Define install requirements
install_requires = [
    'numpy>=1.20',
    'pandas>=1.3.0',
    'matplotlib>=3.5.0',
    'plotly>=5.0.0',
    'scikit-learn>=1.0.0',
    'scipy>=1.7.0',
    'statsmodels>=0.13.0'
]

# Define optional dependencies
extras_require = {
    'dev': [
        'pytest>=7.0',
        'pytest-cov>=2.12',
        'black>=22.0',
        'flake8>=4.0',
        'mypy>=0.991',
        'sphinx>=4.3',
        'sphinx-rtd-theme>=1.0'
    ],
    'examples': [
        'jupyter>=1.0',
        'nbformat>=5.0'
    ],
    'visualization': [
        'seaborn>=0.11.0',
        'bokeh>=2.4.0'
    ],
    'advanced_ml': [
        'tensorflow>=2.8.0',
        'torch>=1.10.0',
        'keras>=2.8.0'
    ]
}

# Define entry points
entry_points = {
    'console_scripts': [
        'nlm-research = pynlm.research:main',
        'nlm-viz = pynlm.viz:main',
        'nlm-analysis = pynlm.analysis:main'
    ]
}

# Main setup
if __name__ == '__main__':
    # Use scikit-build for C++ extensions
    setup(
        name='pynlm',
        version=version,
        description=description,
        long_description=long_description,
        long_description_content_type='text/markdown',
        author='NLM Authors',
        author_email='contact@nlm-project.org',
        url='https://github.com/nlm-project/nlm',
        packages=packages,
        package_dir={
            'pynlm': 'python',
            'pynlm.research': 'python',
            'pynlm.viz': 'python',
            'pynlm.analysis': 'python',
            'pynlm.examples': 'python/examples'
        },
        package_data=package_data,
        scripts=scripts,
        install_requires=install_requires,
        extras_require=extras_require,
        entry_points=entry_points,
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
            'Topic :: Scientific/Engineering :: Neuroscience'
        ],
        python_requires='>=3.8',
        zip_safe=False,
        platforms=['linux', 'macOS', 'windows'],
        include_package_data=True,
        options={
            'bdist_wheel': {
                'python_tag': 'py3',
                'abi_tag': 'none',
                'platform_tag': 'linux_x86_64'
            }
        }
    )