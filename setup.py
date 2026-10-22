from setuptools import setup, find_packages
import os
import platform

# Read the contents of README file
with open("PYTHON_BINDINGS.md", "r", encoding="utf-8") as f:
    long_description = f.read()

# Read version from version file
version = "0.1.0"
if os.path.exists("VERSION"):
    with open("VERSION", "r") as f:
        version = f.read().strip()

# Determine the appropriate platform-specific packages
setup(
    name="pynlm",
    version=version,
    author="NLM Authors",
    author_email="info@nlm-project.org",
    description="Python bindings for NLM (Neural Learning Machine) - Advanced neural network simulation",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/nlm-project/nlm",
    project_urls={
        "Bug Reports": "https://github.com/nlm-project/nlm/issues",
        "Documentation": "https://nlm.readthedocs.io",
        "Source Code": "https://github.com/nlm-project/nlm",
    },
    packages=find_packages(include=["nlm", "nlm.*", "debug", "debug.*", "visualization", "visualization.*"]),
    package_dir={
        "": "src",
    },
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Scientific/Engineering :: Neural Networks",
        "Topic :: Software Development :: Libraries :: Python Modules",
    ],
    python_requires=">=3.8",
    install_requires=[
        "numpy>=1.20",
        "scipy>=1.6.0",
    ],
    extras_require={
        "visualization": [
            "matplotlib>=3.3.0",
            "plotly>=4.0.0",
            "seaborn>=0.11.0",
        ],
        "analysis": [
            "scikit-learn>=0.24.0",
            "pandas>=1.2.0",
            "jupyter>=1.0.0",
        ],
        "dev": [
            "pytest>=6.0",
            "pytest-cov>=2.10",
            "black>=20.8b1",
            "flake8>=3.8.0",
            "mypy>=0.800",
            "sphinx>=3.0.0",
            "sphinx-rtd-theme>=0.5.0",
        ],
        "full": [
            "matplotlib>=3.3.0",
            "plotly>=4.0.0",
            "seaborn>=0.11.0",
            "scikit-learn>=0.24.0",
            "pandas>=1.2.0",
            "jupyter>=1.0.0",
        ],
    },
    entry_points={
        "console_scripts": [
            "nlm-python=nlm.python_interface:main",
            "nlm-train=nlm.training_interface:main",
            "nlm-visualize=nlm.visualization_interface:main",
        ],
    },
    include_package_data=True,
    package_data={
        "": [
            "*.json",
            "*.cfg",
            "*.txt",
            "*.md",
        ],
    },
    zip_safe=False,
)
