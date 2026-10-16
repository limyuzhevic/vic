from setuptools import setup, find_packages
import os
import subprocess
import sys

# Read the README for long description
with open("README.md", "r", encoding="utf-8") as f:
    long_description = f.read()

# Get the version from pyproject.toml or hardcoded
version = "0.1.0"

# Check if we're building in development mode
build_dir = os.path.join(os.path.dirname(__file__), "build")
if os.path.exists(build_dir):
    # We're in a build directory, use development setup
    setup_requires = [
        "pybind11>=2.11.0",
        "scikit-build-core>=0.5.0",
    ]
    setup_kwargs = {
        "use_scm_version": {
            "version_file": "pynlm/_version.py",
        }
    }
else:
    # Regular installation
    setup_requires = []
    setup_kwargs = {}

setup(
    name="pynlm",
    version=version,
    author="NLM Authors",
    author_email="",
    description="Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/nlm-project/nlm",
    packages=find_packages() + ["pynlm"],
    package_dir={"": ".", "pynlm": "python"},
    include_package_data=True,
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
    python_requires=">=3.8",
    install_requires=[
        "numpy>=1.20",
    ],
    setup_requires=setup_requires,
    zip_safe=False,
    # scikit-build-core configuration
    options={"bdist_wheel": {"package_dir": {"": ".", "pynlm": "python"}}},
    **setup_kwargs,
)