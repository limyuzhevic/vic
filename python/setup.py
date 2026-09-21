from setuptools import setup, find_packages
import os

def get_requirements():
    requirements = []
    
    # Read base requirements from pyproject.toml
    pyproject_content = {}
    
    # Create a minimal setup for the package
    return requirements

setup(
    name="pynlm",
    version="0.1.0",
    description="Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework",
    author="NLM Authors",
    packages=find_packages(),
    install_requires=[],
    extras_require={
        "dev": [
            "pytest>=7.0",
            "numpy>=1.20",
        ]
    },
    python_requires=">=3.8",
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
    package_data={
        "": ["*.so", "*.pyd", "*.dll"],
    },
)