from setuptools import setup, find_packages
import os
import sys

# Read the contents of README file
readme_path = os.path.join(os.path.dirname(__file__), "..", "..", "..", "HOW_TO_USE.md")
if os.path.exists(readme_path):
    with open(readme_path, "r") as f:
        long_description = f.read()
else:
    long_description = "Python bindings for NLM (Neural Learning Machine)"

# Check for platform-specific dependencies
install_requires = [
    "numpy>=1.21.0",
    "scikit-learn>=1.0.0",
]

# Platform-specific dependencies
if sys.platform == "linux":
    install_requires.extend([
        "pybind11>=2.11.0",
        "scikit-build-core>=0.10.0",
    ])
elif sys.platform == "win32":
    install_requires.extend([
        "pybind11>=2.11.0",
        "scikit-build-core>=0.10.0",
    ])
else:  # macOS and others
    install_requires.extend([
        "pybind11>=2.11.0",
        "scikit-build-core>=0.10.0",
    ])

setup(
    name="pynlm",
    version="0.1.0",
    description="Python bindings for NLM (Neural Learning Machine)",
    long_description=long_description,
    long_description_content_type="text/markdown",
    author="NLM Development Team",
    author_email="nlm-dev@example.com",
    url="https://github.com/example/nlm",
    packages=find_packages(include=["pynlm", "pynlm.*"]),
    install_requires=install_requires,
    extras_require={
        "dev": [
            "pytest>=7.0.0",
            "pytest-cov>=3.0.0",
            "black>=22.0.0",
            "flake8>=4.0.0",
            "mypy>=0.991",
            "sphinx>=4.0.0",
            "sphinx-rtd-theme>=1.0.0",
        ],
        "docs": [
            "sphinx>=4.0.0",
            "sphinx-rtd-theme>=1.0.0",
        ],
    },
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "Intended Audience :: Developers",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Scientific/Engineering :: Neuroscience",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
    ],
    python_requires=">=3.8",
    keywords="neural-networks, spiking-neural-networks, brain, neuroscience, ai",
    project_urls={
        "Bug Reports": "https://github.com/example/nlm/issues",
        "Source": "https://github.com/example/nlm",
    },
)