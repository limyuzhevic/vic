#!/usr/bin/env python3
"""
Setup script for NLM Python package.

This script provides backward compatibility and additional functionality
for installing and using the NLM (Neural Learning Machine) Python package.
"""

import sys
import os
import subprocess
import json
from pathlib import Path

def check_python_version():
    """Check if Python version is supported."""
    version = sys.version_info
    if version.major < 3 or (version.major == 3 and version.minor < 8):
        print("ERROR: NLM requires Python 3.8 or higher")
        print(f"Current Python version: {version.major}.{version.minor}.{version.micro}")
        return False
    print(f"✓ Python {version.major}.{version.minor}.{version.micro} is supported")
    return True

def check_cmake():
    """Check if CMake is available."""
    try:
        subprocess.run(["cmake", "--version"], 
                      capture_output=True, check=True)
        print("✓ CMake is available")
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("✗ CMake is not available. Please install CMake.")
        return False

def check_pybind11():
    """Check if pybind11 is available."""
    try:
        import pybind11
        print(f"✓ pybind11 version {pybind11.__version__} is available")
        return True
    except ImportError:
        print("✗ pybind11 is not available. Please install pybind11.")
        return False

def check_scikit_build_core():
    """Check if scikit-build-core is available."""
    try:
        import scikit_build_core
        print(f"✓ scikit-build-core is available")
        return True
    except ImportError:
        print("✗ scikit-build-core is not available. Please install scikit-build-core.")
        return False

def create_requirements():
    """Create requirements file."""
    requirements = {
        "numpy>=1.20; python_version>='3.8'",
        "scikit-build-core>=0.5.0",
        "pybind11>=2.11.0",
        "wheel",
        "setuptools",
    }
    
    with open("requirements.txt", "w") as f:
        for req in sorted(requirements):
            f.write(f"{req}\n")
    
    print("✓ Created requirements.txt")

def create_setup_cfg():
    """Create setup.cfg for additional configuration."""
    setup_cfg_content = """[metadata]
name = pynlm
version = 0.1.0
description = Python bindings for NLM (Neural Learning Machine) C++ neural simulation framework
description-file = README.md
long-description = file: README.md
long-description-content-type = text/markdown
author = NLM Authors
author-email = contact@kilo.ai
license = MIT
license-file = LICENSE
classifiers =
    Development Status :: 3 - Alpha
    Intended Audience :: Science/Research
    Intended Audience :: Education
    Intended Audience :: Developers
    License :: OSI Approved :: MIT License
    Operating System :: POSIX :: Linux
    Operating System :: MacOS :: MacOS X
    Operating System :: Microsoft :: Windows
    Programming Language :: Python :: 3
    Programming Language :: Python :: 3.8
    Programming Language :: Python :: 3.9
    Programming Language :: Python :: 3.10
    Programming Language :: Python :: 3.11
    Programming Language :: Python :: 3.12
    Programming Language :: C++
    Topic :: Scientific/Engineering :: Artificial Intelligence
keywords = neural, simulation, spiking, brain, learning, ai, cognitive, neuroscience

[options]
packages = find:
install_requires =
    numpy>=1.20; python_version>='3.8'
python_requires = >=3.8
include_package_data = True
setup_requires =
    setuptools>=40.8.0
    wheel>=0.34.0

[options.packages.find]
where = .
exclude =
    tests
    docs
    examples
    build
    dist
    *test*
    *example*

[options.extras_require]
dev =
    pytest>=7.0
    numpy>=1.20
    matplotlib>=3.3
    jupyter>=1.0
    sphinx>=4.0
    sphinx-rtd-theme>=0.5
    myst-parser>=0.15
    black>=21.0
    flake8>=3.8
    isort>=5.0
    mypy>=0.800
examples =
    jupyter>=1.0
    matplotlib>=3.3
    pandas>=1.3

[bdist_wheel]
universal = 0

[build_ext]
extensions = pynlm

[tool:pytest.ini_options]
testpaths = tests, examples
python_files = test_*.py *_test.py
python_classes = Test*
python_functions = test_*

[tool:black]
line-length = 88
target-version = ['py38', 'py39', 'py310', 'py311', 'py312']
include = '\\.pyi?$'
exclude = '''
/(
    \\.eggs
  | \\.git
  | \\.hg
  | \\.mypy_cache
  | \\.tox
  | \\.venv
  | _build
  | buck-out
  | build
  | dist
  | examples
  | docs
)/
'''

[tool.isort]
skip_gitignore = true
profile = "black"
multi_line_output = 3
include_trailing_comma = true
force_grid_wrap = 0
use_parentheses = true
line_length = 88
"""
    
    with open("setup.cfg", "w") as f:
        f.write(setup_cfg_content)
    
    print("✓ Created setup.cfg")

def create_dev_setup():
    """Create development environment setup."""
    dev_setup = """# Development setup for NLM Python package

## Prerequisites

- Python 3.8 or higher
- CMake 3.16 or higher
- C++ compiler with C++20 support
- pip, setuptools, wheel

## Installation

### For development (recommended):

```bash
# Clone the repository
 git clone https://github.com/nlm-project/nlm.git
cd nlm

# Install the package in development mode
 pip install -e .

# Or install with all dependencies
 pip install -e .[dev]
```

### For production:

```bash
pip install .
```

## Testing

```bash
# Run unit tests
 pytest

# Run all tests (including examples)
 pytest --cov=pynlm --cov-report=html

# Run tests with verbose output
 pytest -v

# Run specific test files
 pytest tests/test_brain.py
```

## Running Examples

```bash
# Run the example gallery
 python nlm_examples.py

# Run specific examples
 python -c "from nlm_examples import example_3_agent_environment_simulation; example_3_agent_environment_simulation()"
```

## Using the Command Line Interface

```bash
# Show help
 nlm-cli --help

# Create an advanced brain
 nlm-cli create --advanced --neurons 1000

# Run a simulation
 nlm-cli run --steps 500 --logging

# Explore brain properties
 nlm-cli explore --steps 100
```

## Advanced Usage

```python
# Import advanced functions
from pynlm_advanced import (
    create_advanced_brain,
    create_agent_world,
    create_development_experiment,
    run_simulation_with_logging
)

# Create and use an advanced brain
brain, config = create_advanced_brain(
    neuron_count=1500,
    enable_curiosity=True,
    enable_memory=True,
    enable_development=True
)

stats = run_simulation_with_logging(
    brain, max_steps=1000, log_interval=100,
    enable_progress=True, enable_detailed_stats=True
)
```

## Configuration

The package uses a configuration system that supports:

- JSON configuration files
- Command-line arguments
- Runtime configuration changes
- Source tracking for configuration values

## Development

### Building from source:

```bash
# Create build directory
 mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the package
 make -j$(nproc)

# Install Python bindings
 pip install -e ..
```

### Adding new configuration options:

1. Add to `src/core/Config/Config.hpp`:
   - Define the configuration type
   - Add validation logic

2. Add to `src/core/Config/Config.cpp`:
   - Implement parsing logic
   - Add default values

3. Update Python bindings if needed:
   - Add Python API for new configuration
   - Update convenience functions

## Contributing

### Code Quality:

- Follow black formatting (line length: 88)
- Use isort for import sorting
- Write comprehensive docstrings
- Include type hints where possible

### Testing:

- Write unit tests for all new functionality
- Include examples in the test suite
- Test edge cases and error conditions
- Ensure test coverage > 80%

### Documentation:

- Update README.md with new features
- Add examples to nlm_examples.py
- Document configuration options
- Update API documentation
"""
    
    with open("DEVELOPMENT.md", "w") as f:
        f.write(dev_setup)
    
    print("✓ Created DEVELOPMENT.md")

def main():
    """Main setup script."""
    print("NLM Python Package Setup Script")
    print("=" * 40)
    
    # Check requirements
    checks = [
        ("Python Version", check_python_version),
        ("CMake", check_cmake),
        ("pybind11", check_pybind11),
        ("scikit-build-core", check_scikit_build_core),
    ]
    
    all_passed = True
    for check_name, check_func in checks:
        print(f"\n{check_name}:")
        if not check_func():
            all_passed = False
    
    if not all_passed:
        print("\n❌ Setup failed. Please fix the issues above.")
        return 1
    
    # Create additional files
    print("\nCreating development files...")
    create_requirements()
    create_setup_cfg()
    create_dev_setup()
    
    print("\n" + "=" * 40)
    print("✅ Setup completed successfully!")
    print("\nNext steps:")
    print("1. Install the package: pip install -e .")
    print("2. Run tests: pytest")
    print("3. Run examples: python nlm_examples.py")
    print("4. Use CLI: nlm-cli --help")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
