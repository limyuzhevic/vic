from setuptools import setup, find_packages

# Read the long description from README.md
with open("README_UTILITIES.md", "r", encoding="utf-8") as f:
    long_description = f.read()

# Try to read version from a version file
try:
    with open("pynlm/__init__.py", "r", encoding="utf-8") as f:
        content = f.read()
        # Extract version using simple regex
        import re
        version_match = re.search(r'__version__\s*=\s*[\'"]([^\'"]+)[\'"]', content)
        if version_match:
            version = version_match.group(1)
        else:
            version = "2.0.0"
except Exception:
    version = "2.0.0"

setup(
    name="nlm",
    version=version,
    description="Neural Learning Machine - Advanced Brain Simulation Framework",
    long_description=long_description,
    long_description_content_type="text/markdown",
    author="NLM Project",
    author_email="nlm-project@example.com",
    url="https://github.com/nlm-project/nlm",
    packages=find_packages(),
    include_package_data=True,
    package_data={
        "pynlm": ["*.py"],
        "": ["*.md", "*.txt"],
    },
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Scientific/Engineering :: Neuroscience",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Operating System :: OS Independent",
    ],
    python_requires=">=3.8",
    install_requires=[
        "numpy>=1.20",
        "matplotlib>=3.3",
        "PyYAML>=5.3",
    ],
    extras_require={
        "dev": [
            "pytest>=7.0",
            "scikit-build-core>=0.5.0",
            "pybind11>=2.11.0",
            "sphinx>=4.0",
            "sphinx-rtd-theme>=0.5.1",
        ],
        "full": [
            "numpy>=1.20",
            "matplotlib>=3.3",
            "PyYAML>=5.3",
            "pandas>=1.3",
            "seaborn>=0.11",
            "scikit-learn>=1.0",
        ],
    },
    entry_points={
        "console_scripts": [
            "nlm-utils=nlm_utils:main",
            "nlm=nlm:main",
        ],
    },
    scripts=[
        "nlm-utils",
        "nlm",
    ],
    keywords=[
        "neural-networks",
        "brain-simulation",
        "spiking-neurons",
        "computational-neuroscience",
        "ai-research",
        "machine-learning",
        "neuroinformatics",
    ],
)
