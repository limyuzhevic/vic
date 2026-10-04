"""Setup script for NLM Python package."""

from setuptools import setup, find_packages
import os

# Read the long description from README.md
with open("README.md", "r", encoding="utf-8") as f:
    long_description = f.read()

# Try to read VERSION from VERSION file or setup.cfg
def get_version():
    # Check for VERSION file
    version_file = os.path.join(os.path.dirname(__file__), "VERSION")
    if os.path.exists(version_file):
        with open(version_file, "r") as f:
            return f.read().strip()
    
    # Check for pyproject.toml
    try:
        import tomli
        with open("pyproject.toml", "rb") as f:
            data = tomli.load(f)
        return data["project"]["version"]
    except (ImportError, FileNotFoundError, KeyError):
        pass
    
    # Default version
    return "0.1.0"

setup(
    name="pynlm",
    version=get_version(),
    description="Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework",
    long_description=long_description,
    long_description_content_type="text/markdown",
    author="NLM Authors",
    author_email="",
    url="https://github.com/nlm-project/nlm",
    packages=find_packages(),
    package_dir={"pynlm": "pynlm"},
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
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
    ],
    python_requires=">=3.8",
    install_requires=[
        "numpy>=1.20",
    ],
    extras_require={
        "dev": [
            "pytest>=7.0",
            "scikit-build-core>=0.5.0",
            "pybind11>=2.11.0",
        ],
    },
    entry_points={
        "console_scripts": [
            "nlm-example=examples:main",
        ],
    },
    include_package_data=True,
    package_data={
        "pynlm": [
            "*.py",
        ],
    },
    zip_safe=False,
)