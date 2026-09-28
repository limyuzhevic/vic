# NLM Build Configuration Guide

## Overview

This guide provides comprehensive information about building, configuring, and optimizing the NLM (Neural Learning Machine) project from source. It covers both C++ and Python builds, build configurations, troubleshooting, and best practices.

## Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)
- **Ninja**: Optional, faster build system (recommended for large projects)

### Basic Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Advanced Build Configurations

#### Standard Build Types

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release build (optimized, no debug symbols)
cmake .. -DCMAKE_BUILD_TYPE=Release

# RelWithDebInfo (optimized with debug info)
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo

# MinSizeRel (optimized for minimum size)
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
```

#### Compiler and Toolchain Options

```bash
# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install

# Build with static linking
cmake .. -DCMAKE_STATIC_LINK_CXX_STDLIBS=ON

# Build with position independent code (required for shared libraries)
cmake .. -DCMAKE_POSITION_INDEPENDENT_CODE=ON

# Build with RPATH for runtime library paths
cmake .. -DCMAKE_INSTALL_RPATH="$ORIGIN"

# Enable testing
cmake .. -DBUILD_TESTING=ON

# Enable sample applications
cmake .. -DBUILD_EXAMPLES=ON

# Set installation directory for Python bindings
cmake .. -DPYTHON_EXECUTABLE=/usr/bin/python3
```

#### Using Ninja Build System (Recommended)

```bash
# Install ninja if not present
# Ubuntu/Debian: sudo apt-get install ninja-build
# macOS: brew install ninja
# Windows: choco install ninja

# Configure with Ninja
cmake .. -G "Ninja"

# Build
cmake --build . -- -j$(nproc)

# Install
cmake --install .
```

#### Parallel Build Optimization

```bash
# Determine optimal job count
# macOS:
make -j$(sysctl -n hw.ncpu)

# Linux:
make -j$(nproc)

# Windows (Visual Studio):
# Use MSBuild: msbuild NLM.sln /p:Configuration=Release /m
```

### Build Outputs and Artifacts

The build process generates the following outputs:

#### Compiled Libraries
- `libnlm_core.a` - Core neural computation libraries
- `libnlm_agent.a` - Agent system interfaces
- `libnlm_world.a` - World simulation engine

#### Executable Files
- `bin/nlm` - Main brain simulation
- `bin/nlm_phase3_demo` - Phase 3 demonstration
- `bin/nlm_phase4_demo` - Phase 4 demonstration
- `bin/nlm_phase6_demo` - Phase 6 integration demonstration
- `bin/nlm_test` - Unit test suite

#### Data and Configuration
- `configs/` - Default configuration files
- `examples/` - Sample configuration and data files
- `docs/` - Documentation files

### Build Troubleshooting

#### Common Build Issues and Solutions

**Issue: CMake cannot find Python**
```bash
# Install Python development packages
# Ubuntu/Debian:
sudo apt-get install python3-dev python3-pip

# macOS:
brew install python

# Then configure CMake with explicit Python path
cmake .. -DPython_EXECUTABLE=$(which python3)
```

**Issue: Compiler version too old**
```bash
# Update compiler
curl -sSL https://gcc.gnu.org/gcc/snapshots/gcc-13.0.0/gcc-13.0.0.tar.xz | tar xzf
sudo apt-get install g++-13

# Use new compiler
cmake .. -DCMAKE_CXX_COMPILER=g++-13
```

**Issue: Memory problems during build**
```bash
# Split build into multiple jobs with memory limits
make -j4

# Or use ccache to speed up rebuilds
ccache cmake ..
ccache make -j$(nproc)
```

**Issue: Missing dependencies**
```bash
# Install common dependencies
# Ubuntu/Debian:
sudo apt-get install build-essential cmake libpython3-dev libboost-all-dev

# macOS:
brew install cmake python3 boost

# Then retry build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Build Performance Optimization

#### Incremental Builds

For development workflow, use incremental builds:

```bash
# Initial configuration
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# After making changes, rebuild only affected targets
make -j$(nproc) target_name

# Clean build if needed
cmake .. -DCMAKE_BUILD_TYPE=Debug
make clean
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

#### Using ccache for Faster Rebuilds

```bash
# Install ccache
# Ubuntu/Debian: sudo apt-get install ccache
# macOS: brew install ccache

# Configure CMake to use ccache
cmake .. -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

# Rebuild
ccache cmake ..
ccache make -j$(nproc)
```

---

## Building the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy cmake

# Build and install
pip install .

# Or install in development mode (recommended for development)
pip install -e .

# Verify installation
python3 -c "import pynlm; print(pynlm.__version__)"
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Python Environment Setup

```bash
# Create virtual environment for NLM
python3 -m venv nlm_env
source nlm_env/bin/activate

# Install NLM and dependencies
pip install .

# Test installation
python3 -c "
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
print(f'Created brain with {brain.getTotalNeuronCount()} neurons')
"
```

---

## Development Workflow

### Typical Development Cycle

1. **Initial Setup**
   ```bash
   # Clone repository
   git clone https://github.com/yourusername/nlm.git
   cd nlm

   # Create build directory
   mkdir build
   cd build

   # Configure and build
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   make -j$(nproc)
   ```

2. **Code Development**
   - Edit source files in `src/`
   - Update headers in `include/nlm/`
   - Modify configuration in `configs/`
   - Update documentation in `docs/`

3. **Build and Test**
   ```bash
   # Build only changed targets
   make -j$(nproc) component_name

   # Run specific tests
   ./nlm_test --filter test_component

   # Run integration tests
   ./nlm_phase6_demo
   ```

4. **Validation**
   - Run unit tests: `ctest --output-on-failure`
   - Run integration tests: `./nlm_phase6_demo`
   - Check for memory leaks: `valgrind ./nlm_test`
   - Profile performance: `perf ./nlm_phase6_demo`

5. **Documentation**
   - Update API documentation
   - Add examples to documentation
   - Update build instructions

### Version Control Best Practices

#### Git Configuration

```bash
# Set up git user
git config user.name "Your Name"
git config user.email "your.email@example.com"

# Create feature branches
git checkout -b feature/improvement-name

# Commit with descriptive message
git add src/
git commit -m "Improve neural connectivity initialization

- Add validation for configuration parameters
- Check for null pointers in memory systems
- Improve error messages and logging

Co-authored-by: Your Name <your.email@example.com>"

# Push to remote
git push origin feature/improvement-name
```

#### Commit Message Guidelines

**Subject line**: Brief summary (50 characters max)
**Body**: Detailed description, including:
- What was changed
- Why it was changed
- How it was changed
- Any trade-offs or considerations
- Testing instructions

Example:
```
Improve neural connectivity error handling

- Add parameter validation for brain initialization
- Check for null memory system pointers
- Provide detailed error messages for configuration issues
- Maintain backward compatibility with existing configs

Testing:
- All existing unit tests pass
- Integration tests demonstrate improved robustness
```

### Testing Strategy

#### Unit Testing

Each component has corresponding unit tests:

```bash
# Run all unit tests
ctest --output-on-failure

# Run specific test suite
ctest -R test_neuron
ctest -R test_config
ctest -R test_brain
```

#### Integration Testing

```bash
# Run integration demo
./nlm_phase6_demo

# Run experiment scripts
python3 examples/run_experiment.py

# Test specific scenarios
python3 examples/test_scenario.py --scenario=learning
```

#### Performance Testing

```bash
# Benchmark brain simulation
./benchmark --steps 10000 --neurons 1000

# Profile memory usage
valgrind --tool=massif ./nlm_phase6_demo

# Profile execution time
perf ./nlm_phase6_demo
```

---

## Configuration Management

### Configuration Files

The NLM project uses multiple configuration file types:

#### C++ Configuration (`.cfg` format)

```ini
# configs/default.cfg

# Basic simulation parameters
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log
```

#### JSON Configuration

```json
{
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1,
    "connection_probability": 0.05,
    "initial_weight_mean": 0.5,
    "initial_weight_std": 0.1
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": {
      "enable": true
    }
  },
  "neuromod": {
    "dopamine": {
      "scale": 1.0
    }
  }
}
```

### Configuration Validation

```python
import pynlm

# Create and validate configuration
config = pynlm.createDefaultConfig()

# Check configuration has required keys
required_keys = ["brain.neuron_count", "brain.synapse_density"]
for key in required_keys:
    if not config.has(key):
        print(f"Warning: Missing required config key: {key}")

# Validate parameter ranges
neuron_count = config.getOr("brain.neuron_count", 1000)
if neuron_count <= 0:
    print("Error: neuron_count must be positive")
    sys.exit(1)

# Create brain with validated config
brain = pynlm.createBrain(config)
brain.initialize()
```

---

## Best Practices

### Build Optimization

1. **Use Ninja Build System**: Faster than Make for large projects
2. **Enable ccache**: Speeds up incremental builds
3. **Use Release Builds for Production**: Better performance
4. **Set appropriate parallelism**: Match number of CPU cores
5. **Clean builds for testing**: Use `make clean` to avoid cached issues

### Code Quality

1. **Validate all input parameters**: Check for null pointers, valid ranges
2. **Provide meaningful error messages**: Help users understand issues
3. **Maintain backward compatibility**: Don't break existing configurations
4. **Write comprehensive tests**: Unit tests for all major components
5. **Document APIs**: Provide clear docstrings and examples

### Development Environment

1. **Use virtual environments**: Isolate Python dependencies
2. **Version control**: Use Git for all code changes
3. **Continuous Integration**: Set up automated testing
4. **Documentation**: Keep documentation in sync with code
5. **Code reviews**: Review all significant changes

---

## Troubleshooting

### Common Issues and Solutions

#### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check configuration parameters

#### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Validate motor command generation

#### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration values

#### "Segmentation faults"
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Use debug build for better error reporting

### Getting Help

1. **Documentation**: Check `docs/` directory for comprehensive guides
2. **API Reference**: See header files in `include/nlm/`
3. **Examples**: Look in `examples/` directory for sample applications
4. **Issues**: File issues on GitHub with detailed reproduction steps
5. **Discussion**: Join community forums for technical discussions

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [PERFORMANCE.md](docs/PERFORMANCE.md) - Performance optimization
- [SCALING.md](docs/SCALING.md) - Scaling and deployment

The NLM project is continuously evolving. Check the documentation for the latest updates and best practices.