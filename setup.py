#!/usr/bin/env python3
"""
Setup script for NLM - Legacy setup.py for backward compatibility
This provides the same functionality as pyproject.toml for older Python versions
"""

from setuptools import setup, find_packages
import os
import sys

# Read the contents of README file
with open(os.path.join(os.path.dirname(__file__), 'README.md'), 'r', encoding='utf-8') as f:
    long_description = f.read()

# Define package metadata
metadata = {
    'name': 'pynlm',
    'version': '0.1.0',
    'description': 'Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework',
    'long_description': long_description,
    'long_description_content_type': 'text/markdown',
    'author': 'NLM Authors',
    'license': 'MIT',
    'keywords': ['neural', 'simulation', 'spiking', 'brain', 'learning'],
    'classifiers': [
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
    'python_requires': '>=3.8',
    'install_requires': [
        'numpy>=1.20',
        'scipy>=1.8.0',
        'pandas>=1.5.0',
        'matplotlib>=3.5.0',
        'jupyter>=1.0.0',
    ],
    'extras_require': {
        'dev': [
            'pytest>=7.0',
            'black>=22.0',
            'flake8>=5.0',
            'mypy>=0.991',
        ],
        'full': [
            'numpy>=1.20',
            'scipy>=1.8.0',
            'pandas>=1.5.0',
            'matplotlib>=3.5.0',
            'jupyter>=1.0.0',
        ]
    },
    'packages': find_packages(),
    'package_dir': {'': 'python'},
    'include_package_data': True,
    'zip_safe': False,
    'url': 'https://github.com/nlm-project/nlm',
    'project_urls': {
        'Documentation': 'https://nlm.readthedocs.io',
        'Repository': 'https://github.com/nlm-project/nlm.git',
        'Bug Reports': 'https://github.com/nlm-project/nlm/issues',
    },
    'scripts': [
        'scripts/demo.py',
        'scripts/benchmark.py',
        'scripts/explore.py',
        'scripts/visualizer.py',
        'scripts/monitor.py',
        'scripts/setup.py',
    ],
}

# Main setup function
def setup_package():
    """Setup the NLM package"""
    
    print("=== NLM Package Setup ===")
    print("Name:", metadata['name'])
    print("Version:", metadata['version'])
    print("Description:", metadata['description'])
    print()
    
    # Check for required dependencies
    print("Checking dependencies...")
    try:
        import numpy
        print(f"  ✓ numpy {numpy.__version__}")
    except ImportError:
        print("  ✗ numpy not found - will be installed")
    
    try:
        import scipy
        print(f"  ✓ scipy {scipy.__version__}")
    except ImportError:
        print("  ✗ scipy not found - will be installed")
    
    try:
        import pandas
        print(f"  ✓ pandas {pandas.__version__}")
    except ImportError:
        print("  ✗ pandas not found - will be installed")
    
    try:
        import matplotlib
        print(f"  ✓ matplotlib {matplotlib.__version__}")
    except ImportError:
        print("  ✗ matplotlib not found - will be installed")
    
    try:
        import pybind11
        print(f"  ✓ pybind11 {pybind11.__version__}")
    except ImportError:
        print("  ✗ pybind11 not found - required for C++ bindings")
    
    try:
        import scikit_build_core
        print(f"  ✓ scikit-build-core {scikit_build_core.__version__}")
    except ImportError:
        print("  ✗ scikit-build-core not found - required for building")
    
    print("\nAll dependencies checked. Ready to install.")
    
    # Run setup
    setup(**metadata)

if __name__ == '__main__':
    setup_package()
