# NLM Utilities Setup Configuration
# 
# This file defines the package structure, dependencies, and metadata for the NLM Utilities package.
# The NLM Utilities package provides comprehensive advanced utilities for the NLM (Neural Learning Machine)
# neural simulation framework, including performance monitoring, configuration management,
# debugging, export/import operations, and data analysis tools.

[build-system]
requires = ["setuptools>=61.0", "wheel"]
build-backend = "setuptools.build_meta"

[project]
name = "nlm-utilities"
version = "0.1.0"
description = "Advanced utilities for NLM neural simulation framework"
long_description = """
The NLM Utilities package provides comprehensive utilities for advanced users of the NLM (Neural Learning Machine) 
neural simulation framework. It includes performance monitoring, configuration management, debugging, 
export/import operations, and data analysis tools to enhance the development and analysis experience.
"""
authors = [
    {name = "NLM Developers", email = "contact@nlm.org"}
]
readme = "README.md"
license = {text = "MIT"}
keywords = [
    "neural",
    "simulation", 
    "brain",
    "AI",
    "neuroscience",
    "performance",
    "debugging",
    "analysis",
    "export",
    "import"
]
classifiers = [
    "Development Status :: 3 - Alpha",
    "Intended Audience :: Science/Research",
    "Topic :: Scientific/Engineering :: Artificial Intelligence",
    "Topic :: Software Development :: Libraries :: Python Modules",
    "License :: OSI Approved :: MIT License",
    "Programming Language :: Python :: 3",
    "Programming Language :: Python :: 3.8",
    "Programming Language :: Python :: 3.9",
    "Programming Language :: Python :: 3.10",
    "Programming Language :: Python :: 3.11",
    "Programming Language :: Python :: 3.12",
]
requires-python = ">=3.8"
dependencies = [
    "numpy>=1.20,<2.0",
    "pandas>=1.3,<3.0",
    "matplotlib>=3.3,<4.0",
    "scipy>=1.6,<2.0",
    "networkx>=2.6,<3.0",
    "h5py>=3.1,<4.0",
    "pyyaml>=5.4,<7.0",
    "jsonschema>=4.0.0",
    "psutil>=5.8.0",
]

[project.optional-dependencies]
dev = [
    "pytest>=7.0.0",
    "pytest-cov>=2.12.0",
    "black>=22.0.0",
    "flake8>=3.9.0",
    "mypy>=0.991.0",
]

[project.urls]
homepage = "https://github.com/nlm-project/nlm"
repository = "https://github.com/nlm-project/nlm"
"documentation" = "https://nlm.readthedocs.io"
bug-tracker = "https://github.com/nlm-project/nlm/issues"

[tool.setuptools]
packages = [
    "nlm_utilities",
    "nlm_utilities.performance",
    "nlm_utilities.configuration",
    "nlm_utilities.debugging",
    "nlm_utilities.export",
    "nlm_utilities.analysis"
]

[tool.setuptools.package-data]
nlm_utilities = ["*.md", "*.txt", "*.json", "*.yaml", "*.yml"]
nlm_utilities.performance = ["*.json"]
nlm_utilities.configuration = ["*.json"]

[tool.black]
line-length = 88
target-version = ['py38', 'py39', 'py310', 'py311', 'py312']
skip-string-normalization = false

[tool.mypy]
python_version = "3.8"
warn_return_any = true
warn_unused_configs = true
disallow_untyped_defs = false
ignore_missing_imports = true

[tool.pytest.ini_options]
testpaths = ["tests"]
python_files = ["test_*.py"]
python_classes = ["Test*"]
python_functions = ["test_*"]
