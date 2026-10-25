# NLM Build and Development Guide

## Overview

This document provides comprehensive information for building, testing, and developing the NLM (Neural Learning Machine) project.

## Quick Start

### 1. Build the Project

```bash
# Create build directory
mkdir build

# Configure with CMake
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4
```

### 2. Run Tests

```bash
# Run all tests
cd build
ctest --output-on-failure
```

### 3. Run the Application

```bash
# Run the main NLM application
cd build
./nlm
```

### 4. Run Examples

```bash
# Run Phase 3 example
cd build
./nlm_phase3_demo

# Run Phase 4 example
cd build
./nlm_phase4_demo
```

## Build Options

### CMake Options

The project uses CMake with the following build options:

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | `Release` | Build type (Debug, Release, RelWithDebInfo, MinSizeRel) |
| `NLM_ENABLE_TESTS` | `ON` | Enable/disable tests |
| `NLM_ENABLE_DOCS` | `OFF` | Enable/disable documentation generation |

### Configure Command Examples

```bash
# Debug build with tests
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_TESTS=ON

# Release build without tests
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_TESTS=OFF

# Development build with debugging symbols
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

## Build System Structure

### Target Outputs

The build system creates the following targets:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main NLM application |
| `nlm_core` | Static Library | Core neural computation library |
| `nlm_agent` | Static Library | Agent system library |
| `nlm_world` | Static Library | World system library |
| `nlm_phase3` | Static Library | Phase 3 experiments |
| `nlm_phase4` | Static Library | Phase 4 experiments |
| `nlm` | Executable | Main NLM application |

### Configuration Files

The project supports multiple configuration file formats:

1. **Simple key=value format** (`configs/default.cfg`):
   ```
   neuron_count = 500
   region_count = 1
   simulation_timestep = 0.001
   ```

2. **JSON format** (`configs/default.json`):
   ```json
   {
       "neuron_count": 500,
       "region_count": 1,
       "simulation_timestep": 0.001
   }
   ```

3. **YAML format** (`configs/default.yaml`):
   ```yaml
   neuron_count: 500
   region_count: 1
   simulation_timestep: 0.001
   ```

## Testing

### Running Tests

The project includes comprehensive unit and integration tests:

```bash
# Build and run all tests
cd build
ctest --output-on-failure

# Run specific test targets
ctest -R test_config  # Config tests
ctest -R test_brain  # Brain tests
ctest -R test_neuron  # Neuron tests
ctest -R test_stdp   # STDP tests
```

### Test Categories

1. **Unit Tests**: Test individual components
   - `test_config.cpp`: Configuration system tests
   - `test_random.cpp`: Random number generator tests
   - `test_clock.cpp`: Simulation clock tests
   - `test_neuron.cpp`: Neuron dynamics tests
   - `test_synapse.cpp`: Synapse tests
   - `test_stdp.cpp`: STDP plasticity tests
   - `test_brain.cpp`: Brain integration tests

2. **Integration Tests**: Test system-wide functionality
   - Configuration and command-line interface
   - Complete experiment workflows
   - System integration tests

### Test Results

All tests should pass with the following expected results:

- ✅ Configuration system tests
- ✅ Random number generator tests
- ✅ Simulation clock tests
- ✅ Neuron dynamics tests
- ✅ Synapse tests
- ✅ STDP plasticity tests
- ✅ Brain integration tests
- ✅ Integration tests

## IDE Integration

### Visual Studio Code

The project includes Visual Studio Code configuration files:

#### Key Features:

1. **Tasks**: Automated build and test tasks
2. **Launch Configurations**: Debugging support for NLM and tests
3. **Settings**: C++ language configuration and code completion
4. **File Associations**: Proper file type associations

#### Usage:

1. **Open in VS Code**: `code .`
2. **Run Build Task**: `Ctrl+Shift+B` or run "Tasks: Run Build Task"
3. **Debug Application**: F5 or run "Debug: Launch NLM"
4. **Run Tests**: F5 or run "Debug: Debug Tests"

#### VS Code Commands:

- `Tasks: Run Build Task` - Build the project
- `Tasks: Run Test Task` - Run tests
- `Debug: Start Debugging` - Debug the application
- `Debug: Add Configuration` - Add new debug configurations

### Other IDEs

The project is compatible with other IDEs including:

- **CLion**: CMake-based IDE
- **Visual Studio**: Windows IDE
- **Eclipse**: CDT plugin
- **IntelliJ IDEA**: C++ plugin

## Development

### Code Style

The project follows C++20 standards with the following guidelines:

1. **Headers**: Include guards, namespace organization
2. **Source Files**: Implementation details
3. **Documentation**: Doxygen-style comments
4. **Error Handling**: Exception-based with clear error messages
5. **Testing**: Comprehensive unit and integration tests

### Building a Component

To add a new component:

1. **Create Source Files**: Add `.cpp` and `.hpp` files in the appropriate directory
2. **Update CMakeLists.txt**: Add the new files to the appropriate target
3. **Write Tests**: Add unit tests for the new component
4. **Document**: Add documentation for the new component

### Best Practices

1. **Use CMake Targets**: Leverage CMake's target-based approach
2. **Dependency Management**: Use `target_link_libraries` for dependencies
3. **Include Guards**: Use `#pragma once` in headers
4. **Namespace Organization**: Organize code in `nlm` namespace
5. **Error Handling**: Use exceptions for error handling
6. **Testing**: Write comprehensive unit tests

## Installation

### System Requirements

- C++20 compatible compiler (GCC 9+, Clang 10+, MSVC 19+)
- CMake 3.16 or higher
- Make (GNU make, ninja, etc.)
- pthread library (Linux/macOS)

### Installation Steps

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install to system (optional)
sudo make install
```

### Installation Options

The installation process can be customized with the following options:

```bash
# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local

# Install with documentation
cmake .. -DNLM_ENABLE_DOCS=ON

# Install in development mode
cmake .. -DCMAKE_BUILD_TYPE=Debug
```

## Configuration

### Configuration File Formats

The project supports multiple configuration file formats:

#### 1. Simple key=value format
```
# Configuration file for NLM
neuron_count = 1000
region_count = 2
simulation_timestep = 0.001
connection_probability = 0.1
random_seed = 42
```

#### 2. JSON format
```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "simulation_timestep": 0.001,
    "connection_probability": 0.1,
    "random_seed": 42
}
```

#### 3. YAML format
```yaml
neuron_count: 1000
region_count: 2
simulation_timestep: 0.001
connection_probability: 0.1
random_seed: 42
```

### Command-Line Options

The NLM application supports various command-line options:

```bash
# Help and version
./nlm --help          # Show help information
./nlm --version       # Show version information
./nlm --list          # List configuration options
./nlm --validate      # Validate current configuration

# Configuration file
./nlm --config=file.json

# Set configuration values
./nlm --neuron_count=1000 --region_count=2

# Run specific tests
./nlm --run_test=1    # Run test 1 (basic connectivity)
./nlm --run_test=2    # Run test 2 (plasticity experiment)
./nlm --run_test=3    # Run test 3 (STDP verification)

# Simulation parameters
./nlm --num_steps=5000 --step_duration=0.001

# Log configuration
./nlm --log_level=Debug --log_colors=false

# Output configuration
./nlm --output_mode=verbose --output_file=results.txt
```

### Configuration Examples

#### Basic Configuration
```bash
# Basic neural network configuration
./nlm \
    --neuron_count=1000 \
    --region_count=2 \
    --simulation_timestep=0.001 \
    --random_seed=42
```

#### Advanced Configuration
```bash
# Advanced configuration with experiments
./nlm \
    --config=experiments.json \
    --run_test=2 \
    --num_steps=5000 \
    --enable_plasticity=true \
    --stdp_ltp_weight=0.02 \
    --stdp_ltd_weight=0.015 \
    --log_level=Debug \
    --output_file=experiment_results.txt
```

## Troubleshooting

### Common Issues

#### Build Errors

**Issue:** CMake not found or version too old
**Solution:** Install CMake 3.16 or higher

```bash
# On Ubuntu/Debian
sudo apt update
sudo apt install cmake

# On macOS
brew install cmake

# On Windows
choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System'
```

**Issue:** Compiler not found or incompatible
**Solution:** Install C++20 compatible compiler

```bash
# On Ubuntu/Debian
sudo apt update
sudo apt install g++

# On macOS
xcode-select --install

# On Windows
choco install microsoft-visualcpp-build-tools
```

#### Runtime Errors

**Issue:** Missing dependencies
**Solution:** Ensure all required libraries are installed

**Issue:** Configuration file errors
**Solution:** Check configuration file syntax and paths

**Issue:** Permission denied
**Solution:** Check file permissions and installation directories

### Debug Information

If you encounter issues, you can enable additional debugging information:

```bash
# Debug build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4

# Run with debug output
cd build
./nlm --log_level=Debug
```

### Getting Help

For additional help and support:

1. **Documentation**: Check the `README.md` file for detailed information
2. **Issue Tracker**: Report issues on GitHub
3. **Community**: Join the NLM community for discussions
4. **Support**: Contact the project maintainers for specific issues

## License

This project is licensed under the MIT License. See the `LICENSE` file for more information.

## Acknowledgements

This project benefits from various open-source libraries and tools:

- CMake for build system
- C++20 standard library
- pthread library
- Various testing frameworks

## Contributors

See the `CONTRIBUTING.md` file for information on how to contribute to this project.

## Future Work

The NLM project has the following planned enhancements:

1. **Phase 3**: World interaction loop integration
2. **Phase 4**: Higher cognitive functions
3. **Phase 5**: Performance optimizations
4. **Phase 6**: Final integration and production readiness

## Conclusion

This build and development guide provides comprehensive information for working with the NLM project. The project is well-structured with extensive testing, documentation, and IDE support. The NLM system is ready for research and development in the field of artificial neural computation.

**Key Features:**
- ✅ Real spiking neural computation with LIF neurons
- ✅ Event-driven spike propagation with delays
- ✅ STDP and Hebbian plasticity rules
- ✅ Structural plasticity (synaptogenesis/pruning)
- ✅ Enhanced configuration system (JSON/YAML support)
- ✅ Improved logging and error handling
- ✅ Comprehensive testing framework
- ✅ Advanced command-line interface
- ✅ VS Code integration
- ✅ Cross-platform compatibility

The NLM project is a robust foundation for experimental artificial developmental brain research.
