from setuptools import setup, find_packages
import os

# Read the long description from README
with open("README.md", "r", encoding="utf-8") as f:
    long_description = f.read()

setup(
    name="pynlm-enhanced",
    version="1.0.0",
    description="Enhanced Python API for NLM (Neural Learning Machine)",
    long_description=long_description,
    long_description_content_type="text/markdown",
    author="NLM Development Team",
    author_email="nlm@example.com",
    url="https://github.com/nlm-project/nlm-python-enhanced",
    packages=find_packages(),
    package_dir={"": "."},
    include_package_data=True,
    install_requires=[
        "numpy>=1.21.0",
        "matplotlib>=3.5.0; python_version >= '3.6'",
        "pandas>=1.3.0",
        "scipy>=1.7.0",
    ],
    extras_require={
        "dev": [
            "pytest>=6.0",
            "pytest-cov>=2.12",
            "black>=21.12b0",
            "flake8>=4.0.0",
            "mypy>=0.910",
        ],
        "docs": [
            "sphinx>=4.0",
            "sphinx-rtd-theme>=0.5",
            "nbsphinx>=0.8",
        ],
        "full": [
            "pyyaml>=6.0",
            "jinja2>=3.0",
            "jupyter>=1.0",
            "plotly>=5.0",
            "seaborn>=0.11",
        ]
    },
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Science/Research",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Scientific/Engineering :: Neuroscience",
        "Topic :: Software Development :: Libraries",
    ],
    keywords="neural-networks, spiking-neural-networks, brain-simulation, ai, neuroscience",
    python_requires=">=3.7",
    entry_points={
        "console_scripts": [
            "nlm-enhanced=nlmenhanced.__main__:main",
        ],
    },
    project_urls={
        "Bug Reports": "https://github.com/nlm-project/nlm-python-enhanced/issues",
        "Documentation": "https://nlm-project.github.io/nlm-python-enhanced/",
        "Source Code": "https://github.com/nlm-project/nlm-python-enhanced",
        "Examples": "https://github.com/nlm-project/nlm-python-enhanced/tree/main/examples",
    },
)
