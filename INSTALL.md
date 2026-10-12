# NLM - Installation Guide

NLM (熙然, "serene flow") is an experimental artificial developmental brain project. This guide covers both simple pip installation and advanced build options.

## Quick Installation (Recommended)

### For Most Users

```bash
# Install the NLM Python package
pip install nlm

# Verify installation
python -c "import nlm; print('NLM version:', nlm.__version__)"
```

### Basic Usage

```python
import nlm

# Create a simple brain
brain = nlm.create_brain()
brain.initialize()

# Run a simulation
for i in range(100):
    brain.step(i)

print(f"Brain simulation complete: {brain.get_total_spike_count()} spikes")
```

## Advanced Installation (C++ Access)

### Build from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DPYTHON_EXECUTABLE=$(which python3) \
    -DBUILD_PYTHON_BINDINGS=ON

# Build the project
make -j$(nproc)

# Build Python bindings
pip install -e .
```

### Build Options

During CMake configuration, you can enable/disable:

- `BUILD_PYTHON_BINDINGS` - Build Python bindings (default: ON)
- `BUILD_DEMOS` - Build demo executables (default: ON)
- `BUILD_TESTS` - Build test suite (default: ON)
- `BUILD_EXAMPLES` - Build example programs (default: ON)
- `NLM_ENABLE_PROFILING` - Enable performance profiling (default: OFF)
- `NLM_ENABLE_VALIDATION` - Enable runtime validation (default: OFF)

## Installation via Package Managers

### Conda

```bash
conda install -c conda-forge nlm
```

### Docker

```bash
# Use the official Docker image
docker pull nlm-project/nlm:latest
docker run -it nlm-project/nlm:latest

# Or build your own image
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y python3 python3-pip
RUN pip3 install nlm
```

## Development Installation

### For Developers

```bash
# Clone and install in development mode
git clone https://github.com/nlm-project/nlm.git
cd nlm
pip install -e .[dev]

# Install development dependencies
pip install pytest numpy matplotlib scipy
```

### Building Python Bindings

If you want to build the Python bindings with custom options:

```bash
# Build with scikit-build
setuptools_scm
pip install scikit-build-core pybind11
pip install -e .[dev]

# Configure build options
cat > pyproject.toml << 'EOF'
[build-system]
requires = ["scikit-build-core>=0.5.0", "pybind11>=2.11.0"]
build-backend = "scikit_build_core.build"

[project.optional-dependencies]
dev = [
    "pytest>=7.0",
    "numpy>=1.20",
]
EOF

pip install -e .[dev]
```

## Verify Installation

### Basic Verification

```python
import nlm

# Test core functionality
brain = nlm.create_brain()
print(f"Created brain with {brain.get_total_neuron_count()} neurons")

brain.initialize()
print("Brain initialized successfully")

# Test simulation
brain.step(0)
print("Brain step completed")

print("✓ Installation verified successfully!")
```

### Advanced Verification

```python
import nlm

# Test with configuration
config = nlm.create_config()
config.set('brain.neurons', 500)
config.set('brain.regions', 2)

brain = nlm.create_brain(config)
print(f"Configured brain: {brain.get_total_neuron_count()} neurons, {brain.get_region_count()} regions")

# Test with world and agent
world = nlm.create_world()
world.configure(width=10, height=10, vision_width=8, vision_height=8)

agent = nlm.create_agent(brain)
agent.initialize(world)

print("✓ Full system integration test passed")
```

## Troubleshooting

### Common Issues

#### "ModuleNotFoundError: No module named 'nlm'"

**Solution:**

```bash
# Check Python path
python -m pip list | grep nlm

# Reinstall if needed
pip install --force-reinstall nlm
```

#### "ImportError: libnlm.so: cannot open shared object file"

**Solution (Linux):**

```bash
# Find the library
find /usr/local/lib -name "*nlm*.so"

# Add to library path
export LD_LIBRARY_PATH=/path/to/nlm:$LD_LIBRARY_PATH

# Or install to standard location
pip install --no-deps .
```

**Solution (macOS):**

```bash
# Check library paths
find /usr/local/lib -name "*.dylib" | grep nlm

# Add to dynamic linker path
export DYLD_LIBRARY_PATH=/path/to/nlm:$DYLD_LIBRARY_PATH
```

#### Build Failures

If CMake or build fails:

```bash
# Clean build directory
rm -rf build
cd build

# Reconfigure with verbose output
cmake .. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON

# Check for specific errors
make help  # See available targets
```

### Advanced Usage

#### Development Setup

```bash
# For development, clone with submodules
git clone --recurse-submodules https://github.com/nlm-project/nlm.git
cd nlm

# Install development dependencies
pip install -e .[dev]

# Run tests
pytest tests/

# Run examples
python examples/quick_start.py
python examples/simulation.py
```

#### Custom Builds

To build with specific features:

```bash
# Build with all features enabled
cmake .. \
    -DNLM_ENABLE_PROFILING=ON \
    -DNLM_ENABLE_VALIDATION=ON \
    -DNLM_ENABLE_SIMD=ON \
    -DNLM_ENABLE_MULTITHREADING=ON

# Build with custom Python version
cmake .. -DPYTHON_EXECUTABLE=/usr/bin/python3.11
```

## Configuration File

The `pyproject.toml` file controls the Python packaging:

```toml
[build-system]
requires = ["scikit-build-core>=0.5.0", "pybind11>=2.11.0"]
build-backend = "scikit_build_core.build"

[project]
name = "nlm"
version = "1.0.0"
description = "Python bindings for NLM neural simulation framework"
requires-python = ">=3.8"
license = {text = "MIT"}

[project.optional-dependencies]
dev = [
    "pytest>=7.0",
    "numpy>=1.20",
    "matplotlib",
    "scipy",
]
```

## Next Steps

After installation:

1. **Read the documentation:** `docs/tutorials.md`
2. **Try examples:** `examples/quick_start.py`, `examples/simulation.py`
3. **Explore API:** `docs/api.md`
4. **Report issues:** Create GitHub issues at https://github.com/nlm-project/nlm/issues
5. **Contribute:** Read `CONTRIBUTING.md` for development guidelines

## Uninstall

```bash
pip uninstall nlm
```

## License

MIT License. See `LICENSE` file for details.
