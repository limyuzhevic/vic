from setuptools import setup, find_packages
import subprocess
import os

def get_version():
    """Get version from pyproject.toml"""
    try:
        import tomli
        with open("pyproject.toml", "rb") as f:
            pyproject = tomli.load(f)
        return pyproject["project"]["version"]
    except (ImportError, FileNotFoundError, KeyError):
        return "0.1.0"

def get_long_description():
    """Get long description from README.md"""
    readme_path = "README.md"
    if os.path.exists(readme_path):
        with open(readme_path, "r", encoding="utf-8") as f:
            return f.read()
    return "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

def check_build_dependencies():
    """Check if build dependencies are available"""
    try:
        import pybind11
        import scikit_build_core
        return True
    except ImportError:
        print("Warning: pybind11 or scikit-build-core not found.")
        print("Build dependencies will be installed automatically.")
        return False

# Get version and long description
version = get_version()
long_description = get_long_description()

# Check build dependencies
build_deps_ok = check_build_dependencies()

setup(
    name="pynlm",
    version=version,
    author="NLM Authors",
    author_email="authors@nlm-project.org",
    description="Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework",
    long_description=long_description,
    long_description_content_type="text/markdown",
    license="MIT",
    keywords="neural simulation spiking brain learning",
    url="https://github.com/nlm-project/nlm",
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
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    include_package_data=True,
    zip_safe=False,
    install_requires=[],
    extras_require={
        "dev": [
            "pytest>=7.0",
            "numpy>=1.20",
            "scikit-build-core>=0.5.0",
            "pybind11>=2.11.0",
            "pytest-cov",
            "black",
            "isort",
            "flake8",
        ],
        "test": [
            "pytest>=7.0",
            "numpy>=1.20",
            "scikit-build-core>=0.5.0",
            "pybind11>=2.11.0",
        ],
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
    project_urls={
        "Homepage": "https://github.com/nlm-project/nlm",
        "Documentation": "https://nlm.readthedocs.io",
        "Repository": "https://github.com/nlm-project/nlm.git",
    },
)