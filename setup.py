"""
NLM Python package setup.

This file provides the package metadata and configuration for the NLM Python package.
"""

from setuptools import setup, find_packages
import os
import sys

# Read the README file
with open('README.md', 'r', encoding='utf-8') as f:
    long_description = f.read()

# Read requirements if they exist
try:
    with open('requirements.txt', 'r') as f:
        requirements = [line.strip() for line in f if line.strip() and not line.startswith('#')]
except FileNotFoundError:
    requirements = [
        'numpy>=1.20.0',
        'pandas>=1.3.0',
        'matplotlib>=3.3.0',
        'seaborn>=0.11.0',
        'plotly>=5.0.0',
        'pybind11>=2.6.0',
        'pyyaml>=6.0',
        'scikit-learn>=0.24.0',
        'scipy>=1.6.0',
        'psutil>=5.8.0',
        'tqdm>=4.62.0',
        'joblib>=1.0.0'
    ]

# Package metadata
setup(
    name='pynlm',
    version='1.0.0',
    author='NLM Team',
    author_email='info@nlm.ai',
    description='Comprehensive Python interface to NLM (Neural Learning Machine) neural simulation framework',
    long_description=long_description,
    long_description_content_type='text/markdown',
    url='https://github.com/nlm/pynlm',
    packages=find_packages(),
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Science/Research',
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
        'Topic :: Scientific/Engineering :: Neuroscience',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Operating System :: OS Independent',
        'Natural Language :: English'
    ],
    python_requires='>=3.8',
    install_requires=requirements,
    extras_require={
        'dev': [
            'pytest>=6.0.0',
            'pytest-cov>=2.12.0',
            'black>=21.0.0',
            'flake8>=3.9.0',
            'mypy>=0.910',
            'sphinx>=4.0.0',
            'sphinx-rtd-theme>=0.5.0',
            'jupyter>=1.0.0'
        ],
        'visualization': [
            'plotly>=5.0.0',
            'plotly-express>=0.1.2'
        ],
        'analysis': [
            'scikit-learn>=1.0.0',
            'scipy>=1.7.0',
            'statsmodels>=0.13.0'
        ],
        'performance': [
            'psutil>=5.8.0',
            'memory-profiler>=0.5.0'
        ]
    },
    package_data={
        'pynlm': [
            '*.pyx',
            '*.pxd',
            '*.cpp',
            'data/*.json',
            'data/*.yaml',
            'data/*.cfg'
        ],
        'examples': [
            '*.py',
            '*.md',
            '*.json',
            '*.yaml'
        ]
    },
    include_package_data=True,
    zip_safe=False,
    keywords=[
        'neuroscience',
        'neural-networks',
        'brain-model',
        'simulation',
        'machine-learning',
        'ai-research',
        'computational-neuroscience'
    ],
    project_urls={
        'Documentation': 'https://nlm.ai/docs',
        'Source': 'https://github.com/nlm/pynlm',
        'Issues': 'https://github.com/nlm/pynlm/issues',
        'Paper': 'https://arxiv.org/abs/nlm-2024',
        'Forum': 'https://forum.nlm.ai'
    }
)