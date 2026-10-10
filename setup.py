from setuptools import setup, find_packages
import subprocess
import sys
import os

# Package metadata
NAME = "pynlm"
VERSION = "0.1.0"
DESCRIPTION = "Python bindings for NLM (Neural Learning Machine)"
AUTHOR = "NLM Authors"
LICENSE = "MIT"
PYTHON_REQUIRES = ">=3.8"

# Dependencies
INSTALL_REQUIRES = [
    "numpy>=1.20",
]

EXTRAS_REQUIRE = {
    "dev": [
        "pytest>=7.0",
        "scikit-build-core>=0.5.0",
        "pybind11>=2.11.0",
        "black>=22.0",
        "ruff>=0.1.0",
        "mypy>=0.991",
    ],
    "test": [
        "pytest>=7.0",
        "pytest-cov",
        "numpy>=1.20",
    ],
}

# Long description from README.md
long_description = DESCRIPTION
if os.path.exists("README.md"):
    with open("README.md", "r", encoding="utf-8") as f:
        long_description = f.read()

# Package data
PACKAGE_DATA = {
    "": ["*.md", "*.txt"],
    "pynlm": ["*.py"],
}

# Entry points
ENTRY_POINTS = {
    "console_scripts": [
        "nlm-benchmark = pynlm.benchmark:main",
        "nlm-demo = pynlm.demo:main",
    ],
}

def setup_package():
    """Set up the Python package."""
    setup(
        name=NAME,
        version=VERSION,
        description=DESCRIPTION,
        long_description=long_description,
        long_description_content_type="text/markdown",
        author=AUTHOR,
        license=LICENSE,
        python_requires=PYTHON_REQUIRES,
        install_requires=INSTALL_REQUIRES,
        extras_require=EXTRAS_REQUIRE,
        packages=find_packages(include=["pynlm", "pynlm.*"]),
        package_data=PACKAGE_DATA,
        include_package_data=True,
        zip_safe=False,
        classifiers=[
            "Development Status :: 3 - Alpha",
            "Intended Audience :: Science/Research",
            "Intended Audience :: Developers",
            "License :: OSI Approved :: MIT License",
            "Operating System :: POSIX :: Linux",
            "Operating System :: MacOS :: MacOS X",
            "Programming Language :: Python :: 3",
            "Programming Language :: Python :: 3.8",
            "Programming Language :: Python :: 3.9",
            "Programming Language :: Python :: 3.10",
            "Programming Language :: Python :: 3.11",
            "Programming Language :: Python :: 3.12",
            "Programming Language :: C++",
            "Topic :: Scientific/Engineering :: Artificial Intelligence",
            "Topic :: Scientific/Engineering :: Neuroscience",
            "Topic :: Software Development :: Libraries",
            "Topic :: Software Development :: Python Modules",
        ],
        keywords=[
            "neural",
            "simulation",
            "spiking",
            "brain",
            "learning",
            "ai",
            "neuroscience",
            "neural-networks",
            "brain-simulator",
        ],
        url="https://github.com/nlm-project/nlm",
        project_urls={
            "Documentation": "https://nlm.readthedocs.io",
            "Source": "https://github.com/nlm-project/nlm",
            "Issues": "https://github.com/nlm-project/nlm/issues",
        },
        entry_points=ENTRY_POINTS,
    )

if __name__ == "__main__":
    setup_package()
