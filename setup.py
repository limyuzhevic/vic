"""
Python package setup for NLM pynlm module.

This file ensures proper installation and packaging of the NLM Python bindings.
"""

from setuptools import setup, find_packages
import os

# Read the README file for long description
with open("README.md", "r", encoding="utf-8") as f:
    long_description = f.read()

# Try to find version from NLM version
version = "0.1.0"
try:
    # Try to extract version from pyproject.toml
    with open("pyproject.toml", "r", encoding="utf-8") as f:
        content = f.read()
        for line in content.splitlines():
            if "version" in line and "=" in line and not line.strip().startswith("#"):
                version = line.split("=")[0].strip().strip('"\'')
                version = line.split("=")[1].strip().strip('"\'').strip(",")
                if version.replace(".", "").isdigit():
                    break
except Exception:
    pass
nlm_requires = []
if os.path.exists("build/nlm_core"):
    nlm_requires.append("nlm_core")

setup(
    name="pynlm",
    version=version,
    author="NLM Authors",
    author_email="nlm@example.com",
    description="Python bindings for NLM (Neural Learning Machine)",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/nlm-project/nlm",
    packages=find_packages(),
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
    extras_require={
        "dev": [
            "pytest>=7.0",
            "matplotlib>=3.0",
            "scipy>=1.0",
        ],
        "visualize": [
            "matplotlib>=3.0",
            "plotly>=4.0",
        ],
        "analysis": [
            "scipy>=1.0",
            "pandas>=1.0",
            "matplotlib>=3.0",
        ],
    },
    package_data={
        "pynlm": [
            "*.py",
        ],
    },
    include_package_data=True,
    zip_safe=False,
    entry_points={
        "console_scripts": [
            "pynlm-demo = pynlm:main",
            "nlm-benchmark = pynlm.benchmark:main",
            "nlm-experiment = pynlm.experiment:main",
        ],
    },
    platforms=["Linux", "MacOS", "Windows"],
)
