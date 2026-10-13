from setuptools import setup, find_packages
import os
import re

def get_version():
    """Get version from __init__.py"""
    init_file = os.path.join(os.path.dirname(__file__), 'python', '__init__.py')
    with open(init_file, 'r') as f:
        content = f.read()
        match = re.search(r'__version__\s*=\s*["\']([^"\']+)["\']', content)
        if match:
            return match.group(1)
    return '0.1.0'

def get_long_description():
    """Get long description from README.md"""
    readme_file = os.path.join(os.path.dirname(__file__), 'README.md')
    with open(readme_file, 'r', encoding='utf-8') as f:
        return f.read()

def get_install_requires():
    """Get installation requirements"""
    return [
        'pybind11>=2.11.0',  # For C++ extensions
        'numpy>=1.20',       # For numerical operations
    ]

def get_extras_require():
    """Get optional dependencies"""
    return {
        'dev': [
            'pytest>=7.0',
            'pytest-cov>=3.0',
            'coverage>=6.0',
            'black>=22.0',
            'flake8>=4.0',
            'mypy>=0.950',
            'sphinx>=4.0',
            'sphinx-rtd-theme>=1.0',
        ],
        'docs': [
            'sphinx>=4.0',
            'sphinx-rtd-theme>=1.0',
            'sphinx-autodoc-typehints>=1.11',
            'myst-parser>=0.17',
        ],
        'examples': [
            'matplotlib>=3.5',
            'jupyter>=1.0',
        ],
    }

setup(
    name='pynlm',
    version=get_version(),
    description='Python interface to NLM (Neural Learning Machine) neural simulation framework',
    long_description=get_long_description(),
    long_description_content_type='text/markdown',
    author='NLM Project',
    author_email='contact@nlm.org',
    url='https://github.com/nlm-project/nlm',
    packages=find_packages(where='python'),
    package_dir={'': 'python'},
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
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
        'Topic :: Scientific/Engineering :: Neuroscience',
        'Topic :: Software Development :: Libraries :: Python Modules',
    ],
    keywords=[
        'neural-network',
        'spiking-neural-network',
        'artificial-intelligence',
        'neuroscience',
        'brain-simulation',
        'learning-algorithms',
        'plasticity',
        'memory-systems',
        'cognitive-architecture',
    ],
    python_requires='>=3.8',
    install_requires=get_install_requires(),
    extras_require=get_extras_require(),
    include_package_data=True,
    package_data={
        'pynlm': [
            'py.typed',
            '*.pyi',
        ],
    },
    entry_points={
        'console_scripts': [
            'nlm-cli=nlm.cli:main',
            'nlm-explore=nlm.explore:main',
            'nlm-demo=nlm.examples:main',
        ],
    },
    project_urls={
        'Documentation': 'https://docs.nlm.org',
        'Source': 'https://github.com/nlm-project/nlm',
        'Bug Reports': 'https://github.com/nlm-project/nlm/issues',
        'Discussions': 'https://github.com/nlm-project/nlm/discussions',
        'Funding': 'https://nlm.org/donate',
    },
    zip_safe=False,
    setup_requires=['pybind11>=2.11.0'],
)
