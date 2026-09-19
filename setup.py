from setuptools import setup, find_packages
from pathlib import Path
import sys

# Read pyproject.toml content to get metadata
toml_content = Path("pyproject.toml").read_text()

# Parse pyproject.toml manually (simple parsing)
version = "0.1.0"
for line in toml_content.split('\n'):
    if 'version =' in line:
        version = line.split('=')[1].strip().strip('"')
    elif 'name =' in line and 'project' in line:
        name = line.split('=')[1].strip().strip('"')

# Read README for long description
readme_path = Path("README.md")
if readme_path.exists():
    long_description = readme_path.read_text()
else:
    long_description = "Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework"

setup(
    name="pynlm",
    version=version,
    author="NLM Authors",
    description="Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework",
    long_description=long_description,
    long_description_content_type="text/markdown",
    packages=find_packages(where=".", include=["python", "tests"]),
    package_dir={"": "."},
    python_requires=">=3.8",
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
    keywords="neural, simulation, spiking, brain, learning",
    url="https://github.com/nlm-project/nlm",
    include_package_data=True,
    zip_safe=False,
)