"""
Setup script for NLM Python bindings.

This script provides Python package management for the NLM Python bindings,
allowing users to install and use the bindings as a proper Python package.
"""

from setuptools import setup, find_packages
from pathlib import Path
import re

# Read the README for the long description
this_dir = Path(__file__).parent
readme_path = this_dir / "README.md"
py_guide_path = this_dir / "docs" / "PYTHON_USER_GUIDE.md"
examples_dir = this_dir / "examples"

def read_file(path):
    """Read file content safely."""
    if path.exists():
        with open(path, 'r', encoding='utf-8') as f:
            return f.read()
    return ""

# Read version from __init__.py if it exists
version = "0.1.0"
init_path = this_dir / "nlm" / "__init__.py"
if init_path.exists():
    version_match = re.search(r"__version__\s*=\s*[\"']([^\"']+)[\"']", init_path.read_text())
    if version_match:
        version = version_match.group(1)

# Read descriptions
long_description = read_file(readme_path) + "\n\n" + read_file(py_guide_path)

# Define package requirements
install_requires = [
    # Core dependencies
    "numpy>=1.20",
    "pybind11>=2.11",
    
    # Optional dependencies for advanced features
    "scipy>=1.6",  # For scientific computing utilities
    "matplotlib>=3.3",  # For visualization
    "scikit-learn>=0.24",  # For machine learning integration
    "pandas>=1.2",  # For data manipulation
    "tqdm>=4.50",  # For progress bars in simulations
]

# Optional dependencies for development
extras_require = {
    'dev': [
        "pytest>=6.0",
        "pytest-cov>=2.10",
        "black>=20.8b1",
        "flake8>=3.8",
        "mypy>=0.800",
        "sphinx>=3.0",
        "sphinx-rtd-theme>=0.5",
    ],
    'testing': [
        "pytest>=6.0",
        "pytest-cov>=2.10",
        "coverage>=5.0",
    ],
    'visualization': [
        "matplotlib>=3.3",
        "seaborn>=0.11",
        "plotly>=4.13",
    ],
    'ml_integration': [
        "scikit-learn>=0.24",
        "tensorflow>=2.4",  # Optional for deep learning integration
        "torch>=1.9",  # Optional for PyTorch integration
    ],
    'research': [
        "jupyter>=1.0",
        "ipykernel>=5.3",
        "ipywidgets>=7.5",
    ],
}

# Package data
package_data = {
    'nlm': [
        'data/*.json',
        'data/*.cfg',
        'examples/*.py',
        'docs/*.md',
        '*.so',
        '*.pyd',
    ],
}

# Entry points
entry_points = {
    'console_scripts': [
        'nlm-demo = nlm.demo:main',
        'nlm-run = nlm.run:main',
        'nlm-explore = nlm.explore:main',
        'nlm-research = nlm.research:main',
    ],
    'gui_scripts': [
        'nlm-explorer = nlm.gui:main',
    ],
}

# Long description for PyPI
long_description += "\n\n"
long_description += "## Installation Examples\n\n"
long_description += """
### Basic Installation

```bash
pip install nlm
```

### Development Installation

```bash
pip install -e .
```

### Installing with Optional Dependencies

```bash
pip install nlm[dev]
pip install nlm[visualization]
pip install nlm[ml_integration]
```

### Installation from Source

```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
pip install .
```

## Quick Start

After installation, you can use NLM in your Python scripts:

```python
import nlm

# Create a simple brain
brain = nlm.createBrain(nlm.create_default_config())
brain.initialize()

# Run a simulation
world = nlm.create_simple_demo_world()
nlm.run_simulation(brain, world, 1000)

print("NLM simulation completed successfully!")
```

## Supported Platforms

- Linux (x86_64, ARM64)
- macOS (x86_64)
- Windows (x86_64)

## Dependencies

### Required Dependencies

- **Python 3.8+**: The NLM Python bindings require Python 3.8 or later
- **NumPy**: Essential for array operations and data handling
- **PyBind11**: C++ Python bindings framework

### Optional Dependencies

The following optional dependencies provide additional functionality:

- **SciPy**: Advanced scientific computing utilities
- **Matplotlib**: Plotting and visualization
- **Scikit-learn**: Machine learning integration
- **Pandas**: Data manipulation and analysis
- **Tqdm**: Progress bars for long-running simulations

### Development Dependencies

For contributing to the NLM project:

- **Pytest**: Testing framework
- **Black**: Code formatting
- **Flake8**: Code linting
- **MyPy**: Type checking
- **Sphinx**: Documentation generation
"""

# Setup configuration
setup(
    name="pynlm",
    version=version,
    author="NLM Research Team",
    author_email="nlm-research@example.com",
    description="Python bindings for NLM (Neural Learning Machine) neural simulation framework",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/nlm-project/nlm",
    project_urls={
        "Documentation": "https://nlm.readthedocs.io",
        "Source": "https://github.com/nlm-project/nlm",
        "Tracker": "https://github.com/nlm-project/nlm/issues",
        "Examples": "https://github.com/nlm-project/nlm/tree/main/examples",
    },
    packages=find_packages(),
    package_dir={"": "."},
    package_data=package_data,
    include_package_data=True,
    install_requires=install_requires,
    extras_require=extras_require,
    entry_points=entry_points,
    classifiers=[
        "Development Status :: 3 - Alpha",
        "Intended Audience :: Science/Research",
        "Intended Audience :: Developers",
        "Topic :: Scientific/Engineering :: Artificial Intelligence",
        "Topic :: Software Development :: Libraries :: Python Modules",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Operating System :: POSIX :: Linux",
        "Operating System :: MacOS",
        "Operating System :: Microsoft :: Windows",
    ],
    python_requires=">=3.8",
    keywords="neural network simulation brain spiking neural network artificial intelligence",
    zip_safe=False,
    long_description_is_markdown=True,
)
