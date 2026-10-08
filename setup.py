# NLM Python Package
# Version 0.1.0

from setuptools import setup, find_packages
import os

# Read the long description from README.md
def read_file(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        return f.read()

# Read the version from pyproject.toml
def read_version():
    version_file = os.path.join(os.path.dirname(__file__), 'pyproject.toml')
    with open(version_file, 'r') as f:
        for line in f:
            if 'version = "' in line:
                return line.split('=')[1].strip().strip('"')
    return '0.1.0'

# Read requirements
def read_requirements():
    requirements = [
        'numpy>=1.20',
        'scikit-build-core>=0.5.0',
        'pybind11>=2.11.0'
    ]
    return requirements

setup(
    name='pynlm',
    version=read_version(),
    description='Enhanced Python bindings for NLM (Neural Learning Machine)',
    long_description=read_file('README.md'),
    long_description_content_type='text/markdown',
    author='NLM Authors',
    author_email='',
    url='https://github.com/nlm-project/nlm',
    packages=find_packages(),
    install_requires=read_requirements(),
    extras_require={
        'dev': [
            'pytest>=7.0',
            'numpy>=1.20',
            'matplotlib>=3.5.0',
            'jupyter>=1.0.0'
        ],
        'examples': [
            'matplotlib>=3.5.0',
            'jupyter>=1.0.0'
        ]
    },
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
        'Topic :: Scientific/Engineering :: Artificial Neural Networks',
        'Topic :: Software Development :: Libraries :: Python Bindings'
    ],
    keywords='neural simulation spiking brain learning neuroscience',
    python_requires='>=3.8',
    zip_safe=False,
    platforms='any',
    project_urls={
        'Documentation': 'https://nlm.readthedocs.io',
        'Repository': 'https://github.com/nlm-project/nlm',
        'Bug Reports': 'https://github.com/nlm-project/nlm/issues',
        'Source': 'https://github.com/nlm-project/nlm'
    }
)