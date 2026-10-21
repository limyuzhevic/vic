# NLM Python Bindings - Complete Implementation

This is a complete implementation of the NLM (Neural Learning Machine) Python bindings,
demonstrating all the improvements made to the C++ NLM framework.

## Overview

The NLM Python bindings provide a comprehensive interface to the Neural Learning Machine
framework, enabling researchers and developers to:

- Create and configure neural simulations
- Run experiments programmatically
- Analyze network properties
- Export/import configurations
- Visualize results
- Monitor performance

## Key Improvements Implemented

### 1. Configuration System

#### Before: Limited to simple key=value format
```python
# Simple text-based config only
config = Config()
config.loadFromFile("config.txt")  # Only .txt files
```

#### After: Full JSON/YAML support with validation
```python
# Multiple format support
config = Config()
config.loadFromFile("config.json")  # JSON
config.loadFromFile("config.yaml")  # YAML
config.loadFromFile("config.txt")   # Legacy text format

# Advanced configuration management
config.set("neuron_count", 1000)      # Auto-type conversion
config.set("log_level", "INFO")       # String handling
config.set("random_seed", 42)         # Integer
config.saveToFile("advanced_config.json")  # Pretty JSON output
```

### 2. Command Line Interface

#### Before: No CLI tools

#### After: Comprehensive CLI with advanced features
```bash
# Configuration management
nlm-cli config init --output config.json
nlm-cli config list --show-values
nlm-cli config get neuron_count
nlm-cli config set neuron_count 2000

# Simulation control
nlm-cli --config config.json sim demo
nlm-cli --config config.json sim experiment --steps 5000

# Analysis and export
nlm-cli --config config.json analyze topology
nlm-cli --config config.json validate
nlm-cli --config config.json export spikes --output spikes.json
```

### 3. Python Integration

#### Before: No Python bindings

#### After: Full Python API with C++ bindings
```python
import nlm

# Configuration management
config = nlm.get_default_config()
config.set("neuron_count", 2000)
config.save_to_file("my_config.json")

# Brain creation
brain = nlm.create_brain("config.json")

# Simulation
spike_count = nlm.run_simple_simulation(brain, 1000)

# Analysis
stats = nlm.get_performance_stats()
print(f"Simulations run: {stats['simulations_run']}")
```

### 4. Utility Functions

#### Before: Basic functionality only

#### After: Comprehensive utility library
```python
# String utilities
import nlm.Utils

safe_name = nlm.Utils.sanitize_filename("test file.txt")
timestamped = nlm.Utils.timestamped_filename("results", "csv")

# File utilities
nlm.Utils.create_directory("output")
nlm.Utils.file_exists("config.json")

# Format utilities
formatted_time = nlm.Utils.format_time(3661.5)
formatted_bytes = nlm.Utils.format_bytes(1048576)
```

### 5. Performance Monitoring

#### Before: No performance tracking

#### After: Built-in performance monitoring
```python
stats = nlm.get_performance_stats()
print(f"Configs loaded: {stats['configs_loaded']}")
print(f"Simulations run: {stats['simulations_run']}")
print(f"Memory usage: {stats['memory_usage_mb']} MB")
```

### 6. Error Handling

#### Before: Basic error handling

#### After: Comprehensive error handling
```python
try:
    config = nlm.Config()
    config.load_from_file("config.json")
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Installation

### Standard Installation

```bash
pip install pynlm
```

### Installation with All Dependencies

```bash
pip install "pynlm[viz,advanced,test,doc]"
```

### Development Installation

```bash
pip install -e .[dev]
```

### Environment Setup

```bash
# Create virtual environment
python -m venv nlm_env
source nlm_env/bin/activate

# Install
pip install "pynlm[viz,advanced,test,doc]"
```

## Quick Start Guide

### 1. Basic Usage

```python
#!/usr/bin/env python3
"""
Basic NLM Usage Example
"""

import nlm

print("NLM Python Bindings - Basic Example")
print("=" * 50)

# 1. Create configuration
config = nlm.get_default_config()
config.set("neuron_count", 2000)
config.set("simulation_timestep", 0.0005)

# 2. Save configuration
config.save_to_file("basic_config.json")
print(f"Saved configuration with {config.get('neuron_count')} neurons")

# 3. Load configuration
new_config = nlm.Config()
new_config.load_from_file("basic_config.json")
print(f"Loaded configuration with {new_config.get('neuron_count')} neurons")

# 4. Create brain
brain = nlm.create_brain("basic_config.json")
if brain:
    print("Successfully created NLM brain")
    
    # 5. Run simulation
    spike_count = nlm.run_simple_simulation(brain, 1000)
    print(f"Simulation completed: {spike_count} spikes")
    
    # 6. Check performance
    stats = nlm.get_performance_stats()
    print(f"Performance: {stats['simulations_run']} simulations run")
else:
    print("Failed to create brain")
```

### 2. Advanced Configuration

```python
#!/usr/bin/env python3
"""
Advanced NLM Configuration Example
"""

import nlm

print("Advanced NLM Configuration")
print("=" * 50)

# Create research configuration
research_config = nlm.get_research_config()
print("Research configuration created:")
print(f"  Neurons: {research_config.get('neuron_count')}")
print(f"  Timestep: {research_config.get('simulation_timestep')}")
print(f"  Log level: {research_config.get('log_level')}")

# Create realtime configuration
realtime_config = nlm.get_realtime_config()
print("\nRealtime configuration created:")
print(f"  Neurons: {realtime_config.get('neuron_count')}")
print(f"  Timestep: {realtime_config.get('simulation_timestep')}")
print(f"  Log level: {realtime_config.get('log_level')}")

# Save configurations
research_config.save_to_file("research_config.json")
realtime_config.save_to_file("realtime_config.json")
print("\nConfigurations saved to files")

# Utility functions demo
print("\nUtility Functions:")
print(f"  Formatted number: {nlm.Utils.format_number(1234567.89)}")
print(f"  Formatted time: {nlm.Utils.format_time(3661.5)}")
print(f"  Timestamp: {nlm.Utils.generate_timestamp()}")
print(f"  Safe filename: {nlm.Utils.sanitize_filename('test file.txt')}")
```

### 3. Command Line Usage

```bash
# Help
nlm-cli --help
nlm-cli config --help
nlm-cli sim --help

# Version
nlm-cli version

# Configuration management
nlm-cli config init --output config.json
nlm-cli config list
nlm-cli config get neuron_count
nlm-cli config set neuron_count 2000

# Simulation
nlm-cli --config config.json sim demo
nlm-cli --config config.json sim experiment --steps 5000

# Analysis
nlm-cli --config config.json analyze topology
nlm-cli --config config.json validate

# Export
nlm-cli --config config.json export spikes --output spikes.json

# Validation
nlm-cli --config config.json validate --output validated_config.yaml
```

## API Reference

### Core Classes

#### `Config`
Configuration management class with support for multiple formats.

**Methods:**
- `load_from_file(filepath)` - Load configuration from file
- `save_to_file(filepath)` - Save configuration to file  
- `set(key, value, source)` - Set configuration value
- `get(key)` - Get configuration value
- `has(key)` - Check if key exists
- `clear()` - Clear all configuration
- `get_keys()` - Get all configuration keys
- `summary()` - Get configuration summary

**Examples:**
```python
config = nlm.Config()
config.load_from_file("config.json")
config.set("neuron_count", 1000)
value = config.get("neuron_count")
print(f"Neuron count: {value}")
```

#### `Logger`
Global logging system with multiple backends.

**Methods:**
- `info(message)` - Log info message
- `debug(message)` - Log debug message
- `warning(message)` - Log warning
- `error(message)` - Log error
- `critical(message)` - Log critical message
- `is_enabled(level)` - Check if level is enabled

**Examples:**
```python
logger = nlm.Logger()
logger.info("Starting simulation")
logger.debug("Debug information")
```

### Configuration Presets

#### `get_default_config()`
Return default configuration optimized for general use.

#### `get_research_config()`
Return configuration optimized for research experiments.

#### `get_realtime_config()`
Return configuration optimized for real-time applications.

#### `get_development_config()`
Return configuration optimized for development and testing.

### Utility Functions

#### String Utilities
- `sanitize_filename(name)` - Create safe filename
- `timestamped_filename(base, extension)` - Create filename with timestamp
- `format_number(num, precision)` - Format number with precision
- `format_time(seconds)` - Format time in human-readable format
- `generate_timestamp()` - Generate timestamp string

#### File Utilities
- `file_exists(path)` - Check if file exists
- `directory_exists(path)` - Check if directory exists
- `create_directory(path)` - Create directory if it doesn't exist
- `split_string(str, delimiter)` - Split string by delimiter
- `string_contains(str, substr)` - Check if string contains substring

### Performance Monitoring

#### `get_performance_stats()`
Get current performance statistics.

#### `reset_performance_stats()`
Reset performance statistics to zero.

### Error Handling

#### `NLMError`
Base exception class for all NLM errors.

#### `ConfigurationError`
Exception for configuration-related errors.

#### `SimulationError`
Exception for simulation-related errors.

#### `ValidationError`
Exception for validation-related errors.

## Configuration File Formats

### JSON Format
```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "log_level": "INFO",
    "log_to_file": false,
    "plasticity_learning_rate": 0.01
}
```

### YAML Format
```yaml
random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
region_count: 2
connection_probability: 0.15
log_level: INFO
log_to_file: false
plasticity_learning_rate: 0.01
```

### Text Format (Legacy)
```
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.15
log_level = INFO
log_to_file = false
```

## Environment Detection

The package automatically detects the environment and adjusts behavior:

```python
import platform
import sys

# Platform detection
is_windows = platform.system().lower() == "windows"
is_mac = platform.system().lower() == "darwin"
is_linux = platform.system().lower() == "linux"

# Python version detection
is_python3 = sys.version_info.major == 3
is_python38_plus = sys.version_info >= (3, 8)

# Feature detection
features = nlm.check_features()
# {"json_export": true, "yaml_import": false, "advanced_visualization": false, ...}
```

## Advanced Usage Examples

### 1. Configuration Validation

```python
def validate_configuration(config_file):
    """Validate a configuration file."""
    config = nlm.Config()
    
    if not config.load_from_file(config_file):
        raise nlm.ConfigurationError(f"Could not load config from {config_file}")
    
    # Check required keys
    required_keys = ["neuron_count", "simulation_timestep", "random_seed"]
    for key in required_keys:
        if not config.has(key):
            raise nlm.ConfigurationError(f"Missing required key: {key}")
    
    # Validate value ranges
    neuron_count = config.get("neuron_count")
    if neuron_count <= 0:
        raise nlm.ConfigurationError("neuron_count must be positive")
    
    timestep = config.get("simulation_timestep")
    if timestep <= 0 or timestep > 1.0:
        raise nlm.ConfigurationError("simulation_timestep must be between 0 and 1")
    
    print("Configuration validation successful")
    return True
```

### 2. Batch Processing

```python
def process_multiple_configs(config_files, output_dir):
    """Process multiple configuration files."""
    nlm.Utils.create_directory(output_dir)
    
    results = []
    for config_file in config_files:
        try:
            config = nlm.Config()
            if config.load_from_file(config_file):
                # Run simulation with this config
                brain = nlm.create_brain(config_file)
                spike_count = nlm.run_simple_simulation(brain, 1000)
                
                # Save results
                result_file = nlm.Utils.timestamped_filename(
                    f"results_{Path(config_file).stem}", "json"
                )
                
                result_data = {
                    "config_file": config_file,
                    "spike_count": spike_count,
                    "neuron_count": config.get("neuron_count"),
                    "timestamp": nlm.Utils.generate_timestamp()
                }
                
                with open(result_file, 'w') as f:
                    json.dump(result_data, f, indent=2)
                
                results.append(result_data)
                print(f"Processed {config_file}: {spike_count} spikes")
            
        except Exception as e:
            print(f"Error processing {config_file}: {e}")
    
    return results
```

### 3. Performance Monitoring

```python
def monitor_performance():
    """Monitor and display performance statistics."""
    stats = nlm.get_performance_stats()
    
    print("NLM Performance Statistics")
    print("=" * 40)
    print(f"Configs loaded: {stats['configs_loaded']}")
    print(f"Simulations run: {stats['simulations_run']}")
    print(f"Memory usage: {stats['memory_usage_mb']} MB")
    print(f"Average simulation time: {stats['avg_simulation_time_ms']} ms")
    
    if stats['simulations_run'] > 0:
        avg_spikes_per_simulation = stats.get('total_spikes', 0) / stats['simulations_run']
        print(f"Average spikes per simulation: {avg_spikes_per_simulation:.1f}")
    
    return stats
```

## Troubleshooting

### Common Issues

#### Issue: Python bindings not available
**Solution:**
```bash
# Reinstall with development dependencies
pip install -e .[dev]

# Or install from source
python setup.py build_ext --inplace
```

#### Issue: Configuration file not found
**Solution:**
```python
import os
if not os.path.exists("config.json"):
    print("Creating default configuration...")
    config = nlm.get_default_config()
    config.save_to_file("config.json")
```

#### Issue: Missing dependencies
**Solution:**
```bash
# Install optional dependencies
pip install "pynlm[viz]"      # For visualization
pip install "pynlm[advanced]" # For advanced features
pip install "pynlm[test]"     # For testing
```

### Debug Mode

```python
# Enable debug logging
import logging
logging.basicConfig(level=logging.DEBUG)

# Or set NLM logging level
import nlm
logger = nlm.Logger()
logger.set_level(nlm.LogLevel.Debug)
```

## Testing

### Running Tests

```bash
# Install test dependencies
pip install "pynlm[test]"

# Run unit tests
pytest tests/

# Run specific test module
pytest tests/test_config.py
pytest tests/test_cli.py

# Run tests with coverage
pytest --cov=nlm --cov-report=html
```

### Test Coverage

The test suite includes:
- Configuration loading/saving tests
- CLI command tests
- Error handling tests
- Performance monitoring tests
- Utility function tests

## Development

### Contributing

1. Fork the repository
2. Create a feature branch
3. Implement changes
4. Add tests
5. Run tests locally
6. Submit pull request

### Coding Standards

- Follow PEP 8 for Python code
- Use type hints where appropriate
- Include docstrings for all public functions
- Write comprehensive tests
- Use logging instead of print statements

### Build Process

```bash
# Build C++ extensions
python setup.py build_ext --inplace

# Install package
pip install -e .

# Build documentation
sphinx-build -b html docs/ docs/_build/
```

## Future Enhancements

### Planned Features

1. **Real-time Visualization** - Interactive network visualization
2. **Parallel Processing** - Multi-core and distributed simulation support
3. **Machine Learning Integration** - Neural network model training
4. **Web API** - REST API for remote simulation control
5. **GPU Acceleration** - CUDA-based simulation acceleration
6. **Advanced Dynamics** - More sophisticated neural models
7. **Distributed Computing** - MPI and cluster support
8. **Export/Import Wizard** - GUI tool for configuration management

### Performance Improvements

1. **Memory Optimization** - Reduced memory footprint for large networks
2. **Simulation Speed** - Optimized integration algorithms
3. **Parallel Processing** - SIMD and multi-threading support
4. **Caching** - Results and configurations caching
5. **Compression** - Compressed data storage formats

## License

MIT License - See LICENSE file for details.

## Copyright

Copyright (c) 2024 NLM Project Authors

## Acknowledgements

- NLM C++ Framework Team
- Pybind11 Contributors
- Python Software Foundation
- Scientific Python Ecosystem
- Research Community Contributors

## References

1. NLM Project Documentation - https://nlm.readthedocs.io
2. NLM GitHub Repository - https://github.com/nlm-project/nlm
3. Pybind11 Documentation - https://pybind11.readthedocs.io
4. Python Language Reference - https://docs.python.org/3/
5. NumPy Documentation - https://numpy.org/doc/
6. Matplotlib Documentation - https://matplotlib.org/stable/index.html

## Change Log

### Version 0.1.0 (2024)
- Initial release
- Basic configuration management
- Simple CLI tools
- Python bindings for core functionality
- Performance monitoring
- Error handling
- Utility functions

### Version 0.2.0 (Planned)
- Advanced visualization support
- Real-time simulation capabilities
- Parallel processing support
- Machine learning integration
- Web API interface
- GPU acceleration support

### Version 1.0.0 (Planned)
- Production-ready
- Comprehensive documentation
- Full feature set
- Performance optimizations
- Enterprise support

---

*This implementation demonstrates the improved NLM Python bindings with comprehensive configuration management, CLI tools, utility functions, and performance monitoring capabilities.*
