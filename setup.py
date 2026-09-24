from setuptools import setup, find_packages
import os
import re

# Read the README for long description
with open(os.path.join(os.path.dirname(__file__), 'README.md'), 'r', encoding='utf-8') as f:
    readme = f.read()

# Read the version from __init__.py
version = '0.1.0'
version_file = os.path.join(os.path.dirname(__file__), 'python', '__init__.py')
if os.path.exists(version_file):
    with open(version_file, 'r', encoding='utf-8') as f:
        content = f.read()
        # Extract version from __version__ = "x.x.x"
        match = re.search(r'__version__\s*=\s*["\']([^"\']+)["\']', content)
        if match:
            version = match.group(1)

# Read the requirements
requirements = []
requirements_file = os.path.join(os.path.dirname(__file__), 'requirements.txt')
if os.path.exists(requirements_file):
    with open(requirements_file, 'r', encoding='utf-8') as f:
        requirements = [line.strip() for line in f if line.strip() and not line.startswith('#')]

setup(
    name='pynlm',
    version=version,
    author='NLM Research Project',
    author_email='research@nlm.org',
    description='Python bindings for NLM (Neural Learning Machine) - an experimental artificial brain simulation system',
    long_description=readme,
    long_description_content_type='text/markdown',
    url='https://github.com/nlm-project/nlm',
    packages=find_packages(include=['python', 'python.*']),
    package_dir={'': 'python'},
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
        'Topic :: Scientific/Engineering :: Neural Networks',
        'Topic :: Software Development :: Libraries',
    ],
    python_requires='>=3.8',
    install_requires=requirements,
    extras_require={
        'dev': [
            'pytest>=7.0',
            'pytest-cov>=2.12',
            'black>=22.0',
            'flake8>=4.0',
            'mypy>=0.910',
            'sphinx>=4.0',
            'sphinx-rtd-theme>=0.5',
        ],
        'test': [
            'pytest>=7.0',
            'pytest-cov>=2.12',
        ],
    },
    entry_points={
        'console_scripts': [
            'nlm-demo=python.scripts.demo:main',
            'nlm-explore=python.scripts.explore:main',
        ],
    },
    include_package_data=True,
    package_data={
        'python': [
            '*.md',
            '*.txt',
            'scripts/*.py',
        ],
    },
    zip_safe=False,
    keywords=['neural', 'simulation', 'spiking', 'brain', 'learning', 'ai', 'neuroscience'],
)
