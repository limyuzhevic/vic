# enhanced_pynlm/setup.py
from setuptools import setup, find_packages
import os

# Read the README file
with open(os.path.join(os.path.dirname(__file__), 'README.md'), 'r', encoding='utf-8') as f:
    long_description = f.read()

setup(
    name="enhanced-pynlm",
    version="2.0.0",
    description="Enhanced Python API for NLM Neural Learning Machine with improved interfaces, context managers, and performance monitoring",
    long_description=long_description,
    long_description_content_type="text/markdown",
    author="Enhanced NLM Team",
    author_email="team@nlm-project.org",
    url="https://github.com/nlm-project/nlm",
    packages=find_packages(),
    install_requires=[
        "numpy>=1.20,<3.0",
        "scipy>=1.7.0; extra == 'data-analysis'",
        "matplotlib>=3.4.0; extra == 'visualization'",
        "seaborn>=0.11.0; extra == 'visualization'",
        "pandas>=1.3.0; extra == 'data-analysis'",
        "tqdm>=4.62.0; extra == 'progress'"
    ],
    extras_require={
        'visualization': [
            "matplotlib>=3.4.0",
            "seaborn>=0.11.0"
        ],
        'data-analysis': [
            "pandas>=1.3.0",
            "scipy>=1.7.0"
        ],
        'progress': [
            "tqdm>=4.62.0"
        ],
        'dev': [
            "pytest>=7.0",
            "pytest-cov>=2.12.0",
            "black>=22.0",
            "flake8>=4.0",
            "mypy>=0.910",
            "pre-commit>=2.15.0"
        ],
        'all': [
            "enhanced-pynlm[visualization]",
            "enhanced-pynlm[data-analysis]",
            "enhanced-pynlm[progress]",
            "enhanced-pynlm[dev]"
        ]
    },
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Science/Research",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Software Development :: Libraries",
        "Framework :: Pytest"
    ],
    python_requires=">=3.8",
    keywords="neural simulation spiking brain learning AI artificial intelligence",
    project_urls={
        "Homepage": "https://github.com/nlm-project/nlm",
        "Documentation": "https://nlm.readthedocs.io",
        "Repository": "https://github.com/nlm-project/nlm.git",
        "Issues": "https://github.com/nlm-project/nlm/issues"
    }
)
