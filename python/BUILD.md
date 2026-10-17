# Enhanced NLM Python Bindings - Build Instructions

This document provides build instructions for the enhanced NLM Python bindings.

## Overview

The enhanced NLM Python bindings provide advanced functionality for neural simulation and machine learning applications. This build system supports both the original and enhanced Python bindings.

## Prerequisites

### Required Dependencies

```bash
# Install system dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install -y \
    cmake \
    g++ \
    python3-dev \
    python3-pip \
    git

# Install Python dependencies
pip install pybind11>=2.11.0
pip install nlohmann-json>=3.11.0
pip install scikit-build-core>=0.5.0
```

### Additional Dependencies (Optional)

For development and testing:

```bash
pip install pytest>=7.0
pip install numpy>=1.20
pip install matplotlib>=3.5.0
pip install scipy>=1.7.0
pip install plotly>=5.0.0
pip install jupyter>=1.0.0
```

For machine learning support:

```bash
pip install scikit-learn>=1.0.0
pip install tensorflow>=2.8.0
pip install torch>=1.9.0
```

For documentation:

```bash
pip install sphinx>=4.0.0
sphinx-rtd-theme>=1.0.0
myst-parser>=0.15.0
```

## Building the Enhanced Bindings

### Standard Build (Original Bindings Only)

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

### Building with Enhanced Bindings

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_ENABLE_ENHANCED_PYTHON_BINDINGS=ON
cmake --build . --parallel
```

### Building with All Options

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DNLM_ENABLE_ENHANCED_PYTHON_BINDINGS=ON \
    -DNLM_ENABLE_PYTHON_BINDINGS=ON
cmake --build . --parallel
```

## Python Installation

### Install Python Bindings

```bash
# Install from source
cd build
pip install -e .

# Or install in development mode
pip install -e .[dev]

# Install with extended ML support
pip install -e .[extended]

# Install with documentation support
pip install -e .[docs]
```

### Verify Installation

```python
# Import the enhanced bindings
import pynlm_enhanced as nlm

# Check version
print(f"NLM Enhanced Python Bindings version: {nlm.__version__}")

# Create configuration
config = nlm.createDefaultConfig()
print("✓ Enhanced configuration created")

# Create brain
brain = nlm.Brain(config)
print("✓ Brain created")

# Create enhanced world
world = nlm.EnhancedWorld()
print("✓ Enhanced world created")

# Create advanced agent
agent = nlm.AdvancedAgentBrain(brain)
print("✓ Advanced agent created")

# Create visualization helper
viz = nlm.VisualizationHelper()
print("✓ Visualization helper created")

print("\n✅ All enhanced bindings imported successfully!")
```

## Running Examples

### Basic Example

```bash
# Run basic example
cd examples
python example_basic.py
```

### Enhanced API Example

```bash
# Run enhanced API example
cd examples
python example_enhanced_api.py
```

### Test Suite

```bash
# Run test suite
cd tests
python test_enhanced_api.py
```

## Building Documentation

### Build Documentation

```bash
# Build documentation
cd docs
sphinx-build -b html . _build/html

# View documentation
open _build/html/index.html
```

### Documentation Structure

- `EnhancedPythonAPI.md` - Main API documentation
- `examples/` - Example scripts
- `tests/` - Test suite
- `docs/` - Sphinx documentation

## Advanced Configuration

### CMake Options

The enhanced bindings can be controlled with these CMake options:

```bash
cmake .. \
    -DNLM_ENABLE_ENHANCED_PYTHON_BINDINGS=ON \
    -DNLM_ENABLE_PYTHON_BINDINGS=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DPYTHON_EXECUTABLE=/usr/bin/python3
```

### Python Configuration Options

The enhanced API provides several configuration options:

```python
import pynlm_enhanced as nlm

# Create advanced configuration
config = nlm.createDefaultConfig()

# Configure simulation
config.setInt("brain.neuron_count", 2000)
config.setFloat("simulation_timestep", 0.001)

# Configure plasticity
config.setBool("plasticity.stdp.enable", True)
config.setFloat("plasticity.stdp.learning_rate", 0.001)

# Configure neuromodulation
config.setBool("neuromod.curiosity.enable", True)
config.setBool("neuromod.novelty.enable", True)

# Save configuration
config.saveToFile("my_config.json")
```

## Performance Tuning

### Simulation Performance

```python
import pynlm_enhanced as nlm

# Create optimized configuration
config = nlm.createDefaultConfig()

# For faster simulation (smaller brain)
config.setInt("brain.neuron_count", 500)
config.setFloat("simulation_timestep", 0.01)

# For larger simulation (more neurons)
config.setInt("brain.neuron_count", 5000)
config.setFloat("simulation_timestep", 0.001)

# Monitor performance
from nlm.PerformanceMonitor import getCurrentMetrics
metrics = getCurrentMetrics()
print(f"CPU Usage: {metrics.cpuUsage:.1%}")
print(f"Memory Usage: {metrics.memoryUsage:.1%}")
print(f"Throughput: {metrics.throughput:.0f} steps/sec")
```

### Memory Management

```python
import gc
import pynlm_enhanced as nlm

# Clear references when done
brain = None
agent = None
world = None
gc.collect()

# Monitor memory usage
import psutil
process = psutil.Process()
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.1f} MB")
```

## Troubleshooting

### Common Issues

#### 1. Python Bindings Not Found

**Problem:** Cannot import `pynlm_enhanced`

**Solution:**

```bash
# Check if bindings are built
ls build/*.so

# Install bindings
pip install -e .

# Or rebuild
rm -rf build
mkdir build && cd build
cmake .. -DNLM_ENABLE_ENHANCED_PYTHON_BINDINGS=ON
cmake --build . --parallel
pip install -e .
```

#### 2. Missing Dependencies

**Problem:** Missing Python packages (numpy, pybind11, etc.)

**Solution:**

```bash
# Install dependencies
pip install numpy pybind11 scikit-build-core nlohmann-json
```

#### 3. Build Errors

**Problem:** CMake build errors

**Solution:**

```bash
# Check CMake version
cmake --version

# Ensure all dependencies are installed
# Try different build type
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --parallel
```

### Installation Issues

#### Virtual Environment Issues

```bash
# Create virtual environment
python3 -m venv nlm_env
source nlm_env/bin/activate

# Install in virtual environment
pip install -e .[dev,extended]
```

#### System Python vs User Python

```bash
# Install for user only
pip install --user -e .

# Or install globally (requires sudo)
sudo pip install -e .
```

## Development

### Modifying Enhanced Bindings

To modify the enhanced Python bindings:

1. Edit `python/enhanced_bindings.cpp`
2. Re-run CMake
3. Rebuild
4. Reinstall

### Adding New Features

To add new features to the enhanced API:

1. Add new classes/functions to `enhanced_bindings.cpp`
2. Update the `EnhancedPythonBindings` class
3. Update documentation in `EnhancedPythonAPI.md`
4. Add tests in `tests/test_enhanced_api.py`

### Testing Enhanced Features

```bash
# Run comprehensive tests
cd tests
python test_enhanced_api.py

# Run specific test category
python -m pytest tests/test_enhanced_api.py::test_enhanced_configuration -v

# Run with coverage
python -m pytest tests/test_enhanced_api.py --cov=pynlm_enhanced --cov-report=html
```

## Migration Guide

### From Basic to Enhanced API

If you're migrating from the basic NLM Python API to the enhanced version:

1. **Change imports:**
   - Basic: `import pynlm`
   - Enhanced: `import pynlm_enhanced as nlm`

2. **Update class names:**
   - Basic: `createBrain()`, `createDefaultConfig()`
   - Enhanced: `Brain`, `AdvancedConfig` (with additional methods)

3. **New classes available:**
   - `EnhancedWorld` - Enhanced world simulation
   - `AdvancedAgentBrain` - Advanced agent with ML algorithms
   - `VisualizationHelper` - Visualization tools
   - `PerformanceMonitor` - Performance monitoring

4. **New features:**
   - JSON configuration support
   - Advanced visualization
   - Machine learning integration
   - Performance monitoring
   - Comprehensive error handling

### Example Migration

```python
# Basic API (original)
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)

# Enhanced API (new)
import pynlm_enhanced as nlm

config = nlm.createDefaultConfig()
# Advanced configuration with JSON support
brain = nlm.Brain(config)  # Same interface, enhanced functionality
brain.initialize()
brain.step(0)

# Additional enhanced features
world = nlm.EnhancedWorld()
agent = nlm.AdvancedAgentBrain(brain)
viz = nlm.VisualizationHelper()
```

## Support

### Getting Help

- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Documentation**: https://nlm.readthedocs.io
- **Discussions**: https://github.com/nlm-project/nlm/discussions

### Technical Support

For technical support:
- Email: support@nlm-project.org
- GitHub: https://github.com/nlm-project/nlm

### Contributions

For contributors:
- Fork the repository
- Create pull requests
- Follow coding standards
- Update documentation

## License

This enhanced Python API is licensed under the MIT License. See LICENSE file for details.

## Copyright

© 2024 NLM Development Team

## Changelog

### Version 2.0.0-Enhanced
- Initial release of enhanced Python API
- Added advanced configuration with JSON support
- Added visualization and analysis tools
- Added machine learning integration
- Added performance monitoring
- Enhanced error handling and type safety
- Improved documentation and examples

### Version 1.0.0
- Original Python bindings
- Basic brain simulation
- Simple configuration
- Basic world and agent support

## Acknowledgments

- **pybind11**: Python bindings library
- **nlohmann-json**: JSON library for C++
- **scikit-build-core**: Modern Python packaging
- **NLM Development Team**: For the original neural simulation framework
- **Open Source Contributors**: For continuous improvement

## Contact

For questions or issues:
- **Website**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Email**: info@nlm-project.org

## Future Developments

The enhanced NLM Python API is under active development. Future features may include:

- **Advanced neural architectures**
- **Distributed simulation**
- **Real-time visualization**
- **Cloud integration**
- **Mobile support**
- **Plugin architecture**
- **Advanced ML integration**
- **Performance optimizations**

## Conclusion

The enhanced NLM Python API provides a comprehensive and powerful interface for neural simulation and machine learning applications. It combines the robustness of the original NLM framework with advanced features for scientific research and industrial applications.

For the latest documentation and features, visit the NLM documentation portal at https://nlm.readthedocs.io.

---

*This build document is under active development. Please report any issues or suggest improvements.*

For more information about the NLM project and its capabilities, visit the main documentation at https://nlm.readthedocs.io.