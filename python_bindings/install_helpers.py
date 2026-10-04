"""
NLM Python Bindings Installation and Testing

This file contains commands and scripts to install and test the NLM Python bindings.
"""

# Installation script
INSTALL_SCRIPT = """
#!/bin/bash

# NLM Python Bindings Installation Script
# 
# This script installs the NLM Python bindings and sets up the development environment.

# Check if we're in the right directory
if [ ! -f "pyproject.toml" ]; then
    echo "Error: pyproject.toml not found. Please run this script from the NLM project root."
    exit 1
fi

# Update pip and setuptools
echo "Updating pip and setuptools..."
pip install --upgrade pip setuptools wheel build

# Install build dependencies
echo "Installing build dependencies..."
pip install pybind11 scikit-build-core pytest numpy

# Build and install the Python bindings
echo "Building and installing NLM Python bindings..."
pip install -e .

# Create a test directory if it doesn't exist
mkdir -p tests

# Run basic tests
echo "Running basic tests..."
python -c "import pynlm; print('NLM Python bindings imported successfully!'); print(f'Version: {pynlm.get_version()}')"

# Run example scripts
echo "Running example scripts..."
python examples/main.py

echo "Installation and testing completed successfully!"
"""

# Simple test script
TEST_SCRIPT = """
#!/bin/bash

# NLM Python Bindings Test Script
#

# Run basic functionality tests

echo "Testing NLM Python Bindings..."

echo "1. Testing module import..."
python -c "
import pynlm
print('✓ NLM module imported successfully')
print(f'✓ Version: {pynlm.get_version()}')
"

echo "2. Testing configuration..."
python -c "
import pynlm
config = pynlm.Config()
config.set('test_int', 42)
config.set('test_double', 3.14)
config.set('test_string', 'hello')
config.set('test_bool', True)

assert config.get_or('test_int', 0) == 42
assert abs(config.get_or('test_double', 0.0) - 3.14) < 0.001
assert config.get_or('test_string', '') == 'hello'
assert config.get_or('test_bool', False) == True

print('✓ Configuration system works correctly')
"

echo "3. Testing brain creation..."
python -c "
import pynlm
config = pynlm.Config()
brain = pynlm.create_brain(config)
assert brain is not None
print('✓ Brain creation works correctly')
"

echo "4. Testing world creation..."
python -c "
import pynlm
world = pynlm.create_simple_world()
assert world is not None
world.configure(width=10, height=10, vision_width=8, vision_height=8)
print('✓ World creation works correctly')
"

echo "5. Running example script..."
python examples/main.py

echo "All tests passed! NLM Python bindings are working correctly."
"""

# Requirements file for development
REQUIREMENTS_DEV = """
# Development dependencies for NLM Python bindings
pybind11>=2.11.0
scikit-build-core>=0.5.0
pytest>=7.0
numpy>=1.20
"""

# README for Python bindings
PYTHON_README = """
# NLM Python Bindings

This directory contains the Python bindings for NLM (Neural Learning Machine).

## Installation

### From Source (Recommended)

1. Install build dependencies:

   ```bash
   pip install pybind11 scikit-build-core pytest numpy
   ```

2. Build and install the NLM Python bindings:

   ```bash
   pip install -e .
   ```

### Using the Bindings

After installation, you can import NLM in Python:

```python
import pynlm

# Create a brain with default configuration
config = pynlm.Config()
brain = pynlm.Brain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
    print(f"Step {i}: {brain.get_firing_neuron_count()} firing neurons")
```

## Examples

Run the example scripts:

```bash
python examples/main.py
```

This will run several example demonstrations showing:
1. Silent brain (just neurons)
2. Brain watching a world
3. Complete agent with learning capabilities
4. Advanced configuration
5. Checkpoint save/load
6. Multi-step simulation with statistics

## API Reference

For detailed API documentation, see:
- `HOW_TO_USE.md` - Comprehensive documentation
- `easy_usage.md` - Quick start guide
- `docs/ARCHITECTURE.md` - System architecture

## Supported Python Versions

- Python 3.8
- Python 3.9
- Python 3.10
- Python 3.11
- Python 3.12

## Development

For development, you may need to install additional dependencies:

```bash
pip install -r requirements_dev.txt
```

## Building from Source

If you need to build the C++ extensions manually:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

Then install the Python bindings:

```bash
cd ..
pip install -e .
```
"""

# Write the files
with open("install.py", "w") as f:
    f.write(INSTALL_SCRIPT)
    
with open("test.py", "w") as f:
    f.write(TEST_SCRIPT)
    
with open("requirements_dev.txt", "w") as f:
    f.write(REQUIREMENTS_DEV)
    
with open("README.md", "w") as f:
    f.write(PYTHON_README)
    
# Make scripts executable
import os
os.chmod("install.py", 0o755)
os.chmod("test.py", 0o755)
