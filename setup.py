setup.py

A simple setup script for the NLM Python module.

This allows installing the module with pip:

```bash
pip install -e .
```

Or building a distribution:

```bash
python setup.py sdist bdist_wheel
```
"""

from setuptools import setup, find_packages

with open("README.md", "r", encoding="utf-8") as f:
    long_description = f.read()

setup(
    name="nlm",
    version="0.1.0",
    author="NLM Team",
    author_email="team@nlm.org",
    description="NLM (Neural Learning Machine) Python Bindings",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/nlm-project/nlm",
    packages=find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Development Status :: 4 - Beta",
    ],
    python_requires=">=3.8",
    install_requires=[
        "PyYAML>=5.4",
        "tomli>=2.0.0",
        "jsonschema>=4.0.0",
    ],
    extras_require={
        "dev": [
            "pytest>=7.0.0",
            "pytest-cov>=3.0.0",
            "black>=23.0.0",
            "isort>=5.0.0",
            "flake8>=6.0.0",
            "mypy>=1.0.0",
        ],
    },
)
