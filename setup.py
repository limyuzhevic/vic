from setuptools import setup, find_packages
import subprocess
import sys
import os

# Read the README for long description
with open('README.md', 'r', encoding='utf-8') as f:
    long_description = f.read()

# Add command line option for enhanced bindings
if '--enhanced' in sys.argv:
    os.environ['NLM_ENHANCED_BINDINGS'] = '1'
    sys.argv.remove('--enhanced')

setup(
    name="pynlm",
    version="0.1.0",
    description="Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework",
    long_description=long_description,
    long_description_content_type="text/markdown",
    author="NLM Authors",
    license="MIT",
    packages=find_packages(),
    install_requires=[],
    extras_require={
        "dev": ["pytest>=7.0", "numpy>=1.20"],
    },
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Programming Language :: C++",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
    ],
    url="https://github.com/nlm-project/nlm",
    project_urls={
        "Documentation": "https://nlm.readthedocs.io",
        "Repository": "https://github.com/nlm-project/nlm.git",
    },
    python_requires=">=3.8",
    zip_safe=False,
    include_package_data=True,
    package_data={
        "": ["*.cpp", "*.hpp", "*.h"],
    },
)