from setuptools import setup, find_packages
import os
import sys

# Read the long description from README
with open("README_COMPLETE.md", "r", encoding="utf-8") as f:
    long_description = f.read()

# Read version from __init__.py
try:
    with open("python/pynlm/__init__.py", "r") as f:
        for line in f:
            if line.startswith("__version__"):
                version = line.split("=")[1].strip().strip('"\'')
                break
        else:
            version = "0.1.0"
except:
    version = "0.1.0"

# Check if we're in the right directory
if not os.path.exists("python"):
    print("ERROR: python directory not found. This setup.py should be run from the project root.")
    sys.exit(1)

setup(
    name="pynlm",
    version=version,
    author="NLM Authors",
    author_email="contact@nlm-project.org",
    description="Python bindings for NLM (Neural Learning Machine) - Experimental Artificial Developmental Brain",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/nlm-project/nlm",
    project_urls={
        "Documentation": "https://nlm.readthedocs.io",
        "Repository": "https://github.com/nlm-project/nlm",
        "Bug Tracker": "https://github.com/nlm-project/nlm/issues",
    },
    packages=find_packages(where="python"),
    package_dir={"": "python"},
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
        "Topic :: Scientific/Engineering :: Neuroscience",
        "Operating System :: POSIX :: Linux",
        "Operating System :: MacOS",
        "Operating System :: Microsoft :: Windows",
    ],
    python_requires=">=3.8",
    install_requires=[
        "numpy>=1.20",
        "scipy>=1.7.0",
    ],
    extras_require={
        "dev": [
            "pytest>=7.0",
            "pytest-cov>=2.12",
            "black>=21.0",
            "flake8>=3.9",
            "mypy>=0.910",
            "sphinx>=4.0",
            "sphinx-rtd-theme>=0.5",
        ],
        "visualization": [
            "matplotlib>=3.3",
            "plotly>=5.0",
            "jupyter>=1.0",
        ],
        "performance": [
            "numba>=0.55",
            "cython>=3.0",
            "pypy>=7.3",
        ],
    },
    entry_points={
        "console_scripts": [
            "nlm-demo=pynlm.demo:main",
            "nlm-explore=pynlm.explorer:main",
        ],
    },
    include_package_data=True,
    package_data={
        "pynlm": [
            "data/*.json",
            "data/*.yaml",
            "data/*.cfg",
            "docs/*.md",
        ],
    },
    data_files=[
        ("share/doc/pynlm", ["README_COMPLETE.md", "LICENSE"]),
    ],
    keywords=["neural", "simulation", "spiking", "brain", "learning", "ai", "neuroscience"],
    license="MIT",
    zip_safe=False,
    platforms=["Linux", "MacOS", "Windows"],
)