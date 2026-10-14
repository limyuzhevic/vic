"""
Setup script for NLM Python package.
This provides legacy support for installations with setuptools.
"""

from setuptools import setup, find_packages
import os
import sys

# Read version from pyproject.toml
def get_version():
    import tomli
    with open('pyproject.toml', 'rb') as f:
        data = tomli.load(f)
    return data['project']['version']

def get_requirements():
    requirements = []
    with open('requirements.txt', 'r') as f:
        for line in f:
            line = line.strip()
            if line and not line.startswith('#'):
                requirements.append(line)
    return requirements

def get_long_description():
    with open('README.md', 'r') as f:
        return f.read()

if __name__ == '__main__':
    setup(
        name='pynlm',
        version=get_version(),
        description='Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework',
        long_description=get_long_description(),
        long_description_content_type='text/markdown',
        author='NLM Authors',
        license='MIT',
        python_requires='>=3.8',
        packages=find_packages(),
        install_requires=get_requirements(),
        extras_require={
            'dev': [
                'pytest>=7.0',
                'numpy>=1.20',
                'scipy>=1.7.0',
                'matplotlib>=3.3.0',
                'jupyter>=1.0.0',
                'black>=21.0',
                'mypy>=0.910',
                'flake8>=3.8',
            ],
            'examples': [
                'jupyter>=1.0.0',
                'matplotlib>=3.3.0',
                'pandas>=1.3.0',
                'seaborn>=0.11.0',
            ],
            'debug': [
                'ipdb>=0.13.0',
                'pdbpp>=0.10.0',
            ],
        },
        classifiers=[
            'Development Status :: 3 - Alpha',
            'Intended Audience :: Science/Research',
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
            'Topic :: Scientific/Engineering :: Artificial Neural Networks',
            'Topic :: Software Development :: Libraries :: Python Modules',
        ],
        keywords=['neural', 'simulation', 'spiking', 'brain', 'learning', 'ai', 'neuroscience'],
        url='https://github.com/nlm-project/nlm',
        project_urls={
            'Documentation': 'https://nlm.readthedocs.io',
            'Repository': 'https://github.com/nlm-project/nlm.git',
        },
        include_package_data=True,
        package_data={
            '': ['*.json', '*.md', '*.txt'],
        },
        entry_points={
            'console_scripts': [
                'nlm-cli = nlm.cli:main',
                'nlm-explore = nlm.explore:main',
            ],
        },
        python_requires='>=3.8',
        zip_safe=False,
    )