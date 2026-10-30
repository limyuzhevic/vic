#!/usr/bin/env python3
"""
Setup script for NLM Python package
"""

import sys
import os
from setuptools import setup, find_packages

def read_file(filename):
    """Read file content"""
    with open(filename, 'r', encoding='utf-8') as f:
        return f.read()

def get_install_requirements():
    """Get installation requirements"""
    requirements = []
    
    # Read requirements from requirements.txt if it exists
    requirements_file = os.path.join(os.path.dirname(__file__), 'requirements.txt')
    if os.path.exists(requirements_file):
        with open(requirements_file, 'r', encoding='utf-8') as f:
            requirements = [line.strip() for line in f if line.strip() and not line.startswith('#')]
    
    return requirements

def main():
    """Main setup function"""
    setup(
        name='pynlm',
        version='1.0.0-enhanced',
        description='Enhanced Python bindings for NLM (Neural Learning Machine)',
        long_description=read_file('README.md') + '\n\n' + read_file('python/nlm_cli.py') + '\n\n' + \
                         '=== NLM Command Line Interface (CLI) ===\n\n' + \
                         'Usage examples:\n' + \
                         '  python -m pynlm create-brain --neuron-count 5000\n' + \
                         '  python -m pynlm create-system\n' + \
                         '  python -m pynlm run --steps 5000 --verbose\n' + \
                         '  python -m pynlm benchmark --neurons 1000,5000,10000\n' + \
                         '  python -m pynlm analyze\n',
        long_description_content_type='text/markdown',
        author='NLM Authors',
        author_email='nlm@example.com',
        url='https://github.com/nlm-project/nlm',
        packages=find_packages(),
        include_package_data=True,
        package_data={
            'pynlm': [
                '*.so',
                '*.py',
                'python/',
            ],
        },
        install_requires=get_install_requirements(),
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
            'Topic :: Scientific/Engineering :: Artificial Intelligence',
            'Topic :: Scientific/Engineering :: Neural Networks',
        ],
        keywords=['neural', 'simulation', 'spiking', 'brain', 'learning', 'ai'],
        scripts=[
            'python/nlm_cli.py',
        ],
        entry_points={
            'console_scripts': [
                'nlm=pynlm.nlm_cli:main',
                'nlm-create-brain=pynlm.nlm_cli:main',
                'nlm-run=pynlm.nlm_cli:main',
                'nlm-analyze=pynlm.nlm_cli:main',
                'nlm-benchmark=pynlm.nlm_cli:main',
            ],
        },
        project_urls={
            'Documentation': 'https://nlm.readthedocs.io',
            'Source': 'https://github.com/nlm-project/nlm',
            'Issues': 'https://github.com/nlm-project/nlm/issues',
        },
    )

if __name__ == '__main__':
    main()