from setuptools import setup, find_packages
import os
import re

# Read version from bindings.cpp
def get_version():
    with open('python/bindings.cpp', 'r') as f:
        content = f.read()
        match = re.search(r'pynlm\.__version__\s*=\s*"([^"]+)"', content)
        if match:
            return match.group(1)
    return "0.1.0"

# Read README for long description
def get_long_description():
    with open('README.md', 'r') as f:
        return f.read()

# Read pynlm module docstring from bindings.cpp
def get_module_doc():
    with open('python/bindings.cpp', 'r') as f:
        content = f.read()
        # Extract module docstring
        match = re.search(r'PYBIND11_MODULE\(pynlm,\s*m\)\s*\{[^}]+m\.doc\(\)\s*=\s*R"pbdoc\([^}]+R"pbdoc', content, re.DOTALL)
        if match:
            doc_match = re.search(r'R"pbdoc\(([^)]+)R"pbdoc', match.group(0))
            if doc_match:
                return doc_match.group(1)
    return "NLM (Neural Learning Machine) Python Bindings - Neural Learning Machine Python Bindings"

# Extract authors from pyproject.toml
def get_authors():
    with open('pyproject.toml', 'r') as f:
        content = f.read()
        # Look for authors section
        author_match = re.search(r'authors\s*=\s*\[\s*\{name\s*=\s*"([^"]+)"', content)
        if author_match:
            return author_match.group(1)
    return "NLM Authors"

setup(
    name="pynlm",
    version=get_version(),
    author=get_authors(),
    description="Python bindings for NLM (Neural Learning Machine) - experimental artificial developmental brain simulation",
    long_description=get_long_description(),
    long_description_content_type="text/markdown",
    url="https://github.com/nlm-project/nlm",
    packages=find_packages(where="python"),
    package_dir={"": "python"},
    include_package_data=True,
    classifiers=[
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
        "Programming Language :: C++",
    ],
    python_requires=">=3.8",
    install_requires=[
        "numpy>=1.20",
    ],
    extras_require={
        "dev": [
            "pytest>=7.0",
        ],
    },
    entry_points={
        "console_scripts": [
            "nlm-python = pynlm.cli:main",
            "nlm-benchmark = pynlm.benchmark:main",
        ],
    },
    project_urls={
        "Documentation": "https://nlm.readthedocs.io",
        "Source": "https://github.com/nlm-project/nlm",
        "Issues": "https://github.com/nlm-project/nlm/issues",
        "Discussions": "https://github.com/nlm-project/nlm/discussions",
    },
    keywords=[
        "neural",
        "simulation",
        "spiking",
        "brain",
        "learning",
        "neurosimulation",
        "artificial intelligence",
        "neural network",
        "computational neuroscience",
    ],
)