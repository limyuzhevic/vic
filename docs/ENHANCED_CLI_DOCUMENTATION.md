# NLM - Enhanced CLI and Python Tools Documentation

**Version:** 2.0.0 (Phase 6 - Final Integration)
**Author:** NLM Development Team
**Last Updated:** 2026-10-12

## Overview

This document provides comprehensive information about the enhanced command-line interface and Python tools for NLM (Neural Learning Machine), Phase 6 - Final Integration.

## New Features Summary

### 1. Enhanced Command-Line Interface (C++)

#### 1.1 Help and Version Information
- **`--help` / `-h`**: Display comprehensive help information with all available commands and options
- **`--version`**: Show detailed version information including build details and features

#### 1.2 Configuration Validation
- **`--validate` / `-v`**: Validate configuration files and display key configuration parameters
- Automatic configuration file loading with error handling

#### 1.3 Comprehensive Command Line Operations
- **`--benchmark` / `-b`**: Run performance benchmarks with detailed metrics
- **`--demo MODE` / `-d MODE`**: Run demonstration modes (connectivity, plasticity, stdp)
- **`--analyze` / `-a`**: Perform comprehensive brain state analysis
- **`--export-config FILE` / `-e FILE`**: Export current configuration to file
- **`--import-config FILE` / `-i FILE`**: Import configuration from file
- **`--compare-configs FILE1 FILE2` / `-C FILE1 FILE2`**: Compare two configuration files
- **`--monitor` / `-m`**: Start monitoring mode
- **`--profile` / `-p`**: Profile performance
- **`--watch`**: Watch files for changes
- **`--debug` / `-g`**: Enable debug mode
- **`--trace`**: Enable detailed tracing
- **`--test`**: Run tests
- **`--log-level LEVEL`**: Set logging level (DEBUG, INFO, WARNING, ERROR)

### 2. Python CLI Tools (New Python Interface)

#### 2.1 Comprehensive Python CLI Wrapper

The new Python CLI provides enhanced functionality for NLM with the following key features:

- **`pynlm` module**: Enhanced Python bindings for NLM
- **`nlm-python-cli` script**: Comprehensive command-line interface
- **Configuration management**: Export, import, validation, and comparison
- **Brain analysis**: Comprehensive state analysis and health monitoring
- **Performance monitoring**: Real-time and historical performance tracking
- **Development tools**: Template generation, backup/restore, debugging

#### 2.2 Python CLI Usage Examples

```bash
# Basic usage
nlm --help                    # Show help
nlm --version                 # Show version
nlm --python-cli --analyze     # Use Python CLI features

# Configuration management
nlm --python-cli --validate --config myconfig.cfg
nlm --python-cli --export-config exported_config.json
nlm --python-cli --import-config backup_config.json
nlm --python-cli --compare-configs config1.json config2.json

# Brain analysis
nlm --python-cli --analyze
nlm --python-cli --demo connectivity
nlm --python-cli --demo plasticity
nlm --python-cli --demo stdp

# Configuration templates
nlm --create-template my_template.json

# Testing and development
nlm --python-cli --test
nlm --python-cli --debug
nlm --python-cli --trace
```

### 3. Configuration File Management

#### 3.1 Configuration Validation

Configuration files are validated for:
- Required keys presence
- Data type consistency
- Value range validation
- File format validation

Example:
```bash
nlm --validate --config myconfig.cfg
```

#### 3.2 Configuration Export

Export current configuration with optional metadata:
```bash
nlm --export-config config_export.json
```

Advanced export with metadata:
```bash
nlm --python-cli --export-config config_export.json --include-metadata
```

#### 3.3 Configuration Import

Import configuration from file with validation:
```bash
nlm --import-config external_config.json
```

Import with merge option:
```bash
nlm --python-cli --import-config config.json --merge
```

#### 3.4 Configuration Comparison

Compare two configuration files:
```bash
nlm --compare-configs config_a.json config_b.json
```

Detailed comparison with Python CLI:
```bash
nlm --python-cli --compare-configs config1.json config2.json --detailed
```

### 4. Brain State Analysis and Monitoring

#### 4.1 Brain State Analysis

Perform comprehensive analysis of brain state:
```bash
nlm --analyze
```

Python CLI enhanced analysis:
```bash
nlm --python-cli --analyze
```

Analysis includes:
- Neural activity statistics
- Synaptic connectivity metrics
- Performance indicators
- Health status assessment
- Stability metrics

#### 4.2 Brain Monitoring

Monitor brain performance in real-time:
```bash
nlm --monitor
```

Python CLI monitoring with alerts:
```bash
nlm --python-cli --monitor
```

#### 4.3 Brain Health Reports

Generate comprehensive health reports:
```bash
# C++ CLI
nlm --generate-report health_report.txt

# Python CLI
nlm --python-cli --generate-report health_report.json
```

### 5. Development Tools

#### 5.1 Template Generation

Create configuration templates:
```bash
nlm --create-template template.json
```

The template includes:
- All core simulation settings
- Neural dynamics parameters
- Plasticity mechanisms
- Development system
- Neuromodulation settings
- Memory systems
- Prediction system
- Cognition systems
- Environment settings
- Logging configuration
- Visualization settings
- I/O configuration

#### 5.2 Testing Tools

Run comprehensive tests:
```bash
nlm --test
```

#### 5.3 Configuration Backups

Create configuration backups:
```bash
nlm --backup-configuration backups/
```

Restore from backup:
```bash
nlm --restore-configuration backups/config_20261012.bak
```

### 6. Configuration Template Reference

#### 6.1 Core Simulation Settings

```yaml
simulation:
  random_seed: 42
  simulation_timestep: 0.001
  neuron_count: 1000
  region_count: 2
  connection_probability: 0.1
  max_simulation_steps: 10000
  simulation_time_limit: 0.0
```

#### 6.2 Neural Dynamics

```yaml
dynamics:
  lif_threshold: -55.0
  lif_reset_potential: -65.0
  lif_leak_coefficient: 0.01
  refractory_period: 2.0
  membrane_time_constant: 20.0
```

#### 6.3 Plasticity Mechanisms

```yaml
plasticity:
  plasticity_learning_rate: 0.01
  stdp_ltp_weight: 0.01
  stdp_ltd_weight: 0.012
  stdp_time_constant: 20.0
  stdp_stdp_enabled: true
  hebbian_enabled: true
  structural_enabled: true
  synaptogenesis_rate: 0.001
  pruning_rate: 0.0001
```

#### 6.4 Development System

```yaml
development:
  developmental_stage: "Adult"
  plasticity_modulation_by_age: true
  excitability_changes_with_development: true
  critical_period_enabled: false
```

#### 6.5 Neuromodulation

```yaml
neuromodulation:
  dopamine_baseline: 0.1
  dopamine_effect_on_excitability: true
  curiosity_baseline: 0.05
  curiosity_effect_on_exploration: true
  novelty_sensitivity: 0.1
  reward_prediction_error_enabled: true
```

#### 6.6 Memory Systems

```yaml
memory:
  working_memory_capacity: 100
  working_memory_decay_rate: 0.01
  episodic_memory_enabled: true
  associative_memory_enabled: true
  memory_consolidation_enabled: true
```

### 7. Performance Benchmarks

#### 7.1 Benchmark Results

Benchmarks provide detailed performance metrics:
- Simulation steps per second
- Neural activity statistics
- Memory usage
- Processing time
- Network connectivity metrics

Example output:
```
Benchmark Results:
  Steps: 1000
  Initial spikes: 0
  Final spikes: 147
  Spike rate: 0.147 spikes/step
  Neurons: 1000
  Simulation time: 1.000s
  Performance: 1000.0 steps/second
```

#### 7.2 Demo Modes

Three demonstration modes are available:

**Connectivity Demo**: Tests basic neural connectivity and spike propagation
**Plasticity Demo**: Demonstrates learning through experience and synaptic changes
**STDP Demo**: Verifies spike-timing-dependent plasticity

### 8. Error Handling and Troubleshooting

#### 8.1 Common Errors

**Configuration File Not Found**
- Error: "Failed to load configuration from: configs/default.cfg"
- Solution: Ensure configuration file exists or use `--config` option

**Invalid Configuration**
- Error: Configuration validation failed
- Solution: Check configuration file format and required keys

**Python Module Not Available**
- Error: "pynlm module not available"
- Solution: Install Python bindings: `pip install -e .`

**Permission Denied**
- Error: "Permission denied" when writing files
- Solution: Check file permissions or run with appropriate privileges

#### 8.2 Logging Levels

Log levels are available:
- **DEBUG**: Detailed debug information
- **INFO**: Standard operational information
- **WARNING**: Potential issues or warnings
- **ERROR**: Error conditions
- **CRITICAL**: Critical errors

Set log level:
```bash
nlm --log-level DEBUG
```

### 9. Python API Reference

#### 9.1 Core Classes

- **`pynlm.Config`**: Configuration management
- **`pynlm.Brain`**: Neural simulation brain
- **`pynlm.AgentBrain`**: Agent brain interface
- **`pynlm.SimpleWorld`**: Simple world environment

#### 9.2 Key Methods

**Configuration Methods:**
- `loadFromFile(filepath)`: Load configuration from file
- `saveToFile(filepath)`: Save configuration to file
- `has(key)`: Check if key exists
- `getKeys()`: Get all configuration keys
- `summary()`: Get configuration summary

**Brain Methods:**
- `initialize()`: Initialize brain
- `step()`: Perform simulation step
- `receiveSensoryInput(input)`: Inject sensory input
- `produceAction()`: Generate motor action
- `logStatus()`: Log brain status
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state

**Utility Functions:**
- `createDefaultConfig()`: Create default configuration
- `createBrain(config)`: Create brain with configuration
- `createSimpleWorld()`: Create simple world
- `createAgentBrain(brain)`: Create agent brain interface

### 10. Installation and Setup

#### 10.1 Prerequisites

- C++ compiler with C++20 support
- CMake 3.10 or higher
- Python 3.7 or higher
- pybind11

#### 10.2 Build Instructions

```bash
# Build NLM
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build Python bindings
pip install -e .
```

#### 10.3 Installation

```bash
# Install system-wide
pip install nlm

# Or install from source
pip install -e /path/to/nlm
```

### 11. Examples Gallery

#### 11.1 Basic Usage Example

```bash
# Create a simple configuration template
nlm --create-template basic_config.json

# Validate the configuration
nlm --validate --config basic_config.json

# Run a basic benchmark
nlm --benchmark

# Analyze brain state
nlm --analyze
```

#### 11.2 Advanced Configuration Example

```bash
# Create an advanced configuration with all settings
nlm --create-template advanced_config.json

# Customize the configuration
# Edit advanced_config.json with your parameters

# Export the customized configuration
nlm --export-config my_custom_config.json

# Import and validate
nlm --validate --config my_custom_config.json

# Run a demo with the configuration
nlm --demo plasticity --config my_custom_config.json
```

#### 11.3 Python CLI Example

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Generate analysis
analysis = pynlm.analyze_brain_state(brain, verbose=True)

# Generate report
pynlm.generate_brain_state_report(brain, "brain_report.json")
```

### 12. Performance Optimization

#### 12.1 Optimization Recommendations

- Use `--benchmark` to identify performance bottlenecks
- Monitor memory usage with `--monitor`
- Use appropriate log levels to reduce overhead
- Configure appropriate simulation parameters

#### 12.2 Configuration Optimization

- Adjust `neuron_count` based on available memory
- Use appropriate `max_simulation_steps` for your use case
- Configure `log_level` to match your needs
- Enable/disable visualization based on requirements

### 13. Advanced Topics

#### 13.1 Configuration Migration

To migrate from older versions:
1. Export current configuration: `nlm --export-config old_config.json`
2. Create new configuration with templates: `nlm --create-template new_config.json`
3. Import and validate: `nlm --validate --config new_config.json`

#### 13.2 Custom Extensions

The NLM system supports custom extensions through:
- Custom configuration files
- Custom brain implementations
- Custom world environments
- Custom action types

### 14. Troubleshooting

#### 14.1 Build Issues

If build fails, check:
- Compiler version (C++20 required)
- CMake version (3.10+ required)
- Required dependencies

#### 14.2 Runtime Issues

If runtime issues occur, check:
- Configuration files
- File permissions
- System resources
- Network connectivity (for remote features)

### 15. Support and Community

#### 15.1 Support Resources

- **Documentation**: This document
- **Issue Tracker**: GitHub issues
- **Community Forum**: NLM community
- **Email Support**: support@nlm.org

#### 15.2 Getting Help

For help with NLM:
```bash
# Get help with specific commands
nlm --help [command-name]

# Get detailed information
nlm --version

# Get sample configurations
nlm --create-template sample_config.json
```

### 16. License and Copyright

NLM is licensed under the MIT License.

Copyright (c) 2024-2026 NLM Development Team

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Appendix A: Quick Reference

### A.1 Command Line Summary

```bash
# Core commands
nlm --help                    # Help and usage
nlm --version                 # Version info
nlm --validate FILE           # Validate configuration
nlm --benchmark               # Performance benchmark
nlm --demo MODE               # Run demo (connectivity, plasticity, stdp)

# Configuration management
nlm --export-config FILE      # Export configuration
nlm --import-config FILE      # Import configuration
nlm --compare-configs F1 F2   # Compare configurations

# Analysis and monitoring
nlm --analyze                 # Analyze brain state
nlm --monitor                 # Monitor performance
nlm --generate-report FILE    # Generate reports

# Development tools
nlm --test                    # Run tests
nlm --create-template FILE    # Create configuration template
nlm --backup-configuration DIR  # Create backups

# Advanced options
nlm --debug                   # Debug mode
nlm --trace                   # Trace mode
nlm --log-level LEVEL         # Set logging level
```

### A.2 Python CLI Summary

```bash
# Use enhanced Python CLI features
nlm --python-cli [command]

# Configuration operations
nlm --python-cli --validate --config FILE
nlm --python-cli --export-config FILE
nlm --python-cli --import-config FILE

# Analysis and monitoring
nlm --python-cli --analyze
nlm --python-cli --monitor
nlm --python-cli --generate-report FILE

# Development tools
nlm --python-cli --test
nlm --python-cli --create-template FILE
nlm --python-cli --backup-configuration DIR
```

### A.3 Configuration Template Usage

```bash
# Create a template with samples
nlm --create-template my_template.json

# View template structure
cat my_template.json | jq .

# Customize template
# Edit my_template.json with your parameters

# Use template as base
nlm --validate --config my_template.json
nlm --export-config customized_config.json
```

## Conclusion

The enhanced NLM command-line interface and Python tools provide comprehensive functionality for:

1. **Configuration Management**: Export, import, validation, and comparison
2. **Brain Simulation**: Real-time simulation with multiple plasticity mechanisms
3. **Analysis and Monitoring**: Comprehensive brain state analysis and performance monitoring
4. **Development Tools**: Template generation, backup/restore, and testing
5. **Documentation and Examples**: Comprehensive documentation and usage examples

These tools make NLM more accessible to users of all skill levels, from beginners to advanced researchers and developers. The enhanced CLI provides powerful command-line options while the Python tools offer programmatic access for custom applications and research.

For questions or support, please refer to the Support section or visit the NLM documentation website.
