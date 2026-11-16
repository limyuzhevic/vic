# NLM Command Line Interface - Comprehensive Documentation

This document provides comprehensive documentation for the NLM Command Line Interface (CLI) enhancements.

## Overview

The NLM CLI provides advanced command-line tools for controlling and experimenting with the Neural Learning Machine (NLM). It includes:

- Phase selection and execution
- Configuration management
- Performance monitoring and profiling
- Brain state export/import
- Advanced debugging tools
- Research utilities

## Quick Start

### Basic Usage

```bash
# View help information
nlm-help

# Run a specific NLM phase
nlm-phase [phase] [config_file] [verbose] [headless]

# Manage configuration
nlm-config [action] [parameter] [value] [config_file]

# Profile performance
nlm-profile [duration] [output_file] [phase] [config_file]

# Interactive mode
nlm-interactive
```

### Available Phases

```bash
# Run NLM Phase 2: Real Neural Computation
nlm-phase 2

# Run NLM Phase 3: Environment Interaction
nlm-phase 3

# Run NLM Phase 4: Cognitive Behaviors
nlm-phase 4

# Run NLM Phase 5: Performance & Optimization
nlm-phase 5

# Run NLM Phase 6: Integration Test
nlm-phase 6

# Run Performance Benchmark
nlm-phase benchmark
```

## Detailed Command Reference

### nlm-phase

Run a specific NLM phase with advanced configuration options.

#### Syntax

```bash
nlm-phase [phase] [config_file] [verbose] [headless]
```

#### Parameters

| Parameter | Description | Default | Required |
|-----------oinho | ----------- | ------- | -------- |
| phase | NLM phase to run (2, 3, 4, 5, 6, or "benchmark") | 6 | Yes |
| config_file | Configuration file path | configs/default.cfg | No |
| verbose | Enable verbose output (true/false) | false | No |
| headless | Run in headless mode (true/false) | false | No |

#### Examples

```bash
# Run Phase 6 with default configuration
nlm-phase 6

# Run Phase 3 with custom configuration
nlm-phase 3 my_config.cfg true

# Run Phase 2 in headless mode
nlm-phase 2 headless=true
```

### nlm-config

Manage NLM configuration files with comprehensive validation and editing capabilities.

#### Syntax

```bash
nlm-config [action] [parameter] [value] [config_file]
```

#### Actions

| Action | Description | Parameters Required |
|--------|-------------|------------------- |
| list, show | Display current configuration | None |
| get, query <parameter> | Get parameter value | parameter |
| set, modify <param> <val> | Set parameter value | parameter, value |
| validate, check | Validate configuration file | None |

#### Configuration File Format

```ini
# Sample configuration file
brain.neuron_count = 1000
brain.synapse_density = 0.1
brain.connection_probability = 0.05

plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001

neuromod.dopamine.scale = 1.0
```

#### Examples

```bash
# List all configuration parameters
nlm-config list

# Get specific parameter value
nlm-config get brain.neuron_count

# Set parameter in configuration file
nlm-config set brain.neuron_count 2000 my_config.cfg

# Validate configuration file
nlm-config validate my_config.cfg
```

### nlm-export

Export brain state and research data in multiple formats.

#### Syntax

```bash
nlm-export <output_file> [format] [config_file]
```

#### Formats

| Format | Description | Use Case |
|--------|-------------|---------- |
| binary | Binary brain state | Production/backup |
| json | JSON format | Analysis/tools |
| xml | XML format | Documentation |
| graphics | Visualization data | Research display |

#### Examples

```bash
# Export brain state in binary format
nlm-export brain_state.bin binary

# Export in JSON format for analysis
nlm-export brain_state.json json

# Export with custom configuration
nlm-export research_data.xml graphics configs/expertise.cfg
```

### nlm-import

Import brain state and research data from various file formats.

#### Syntax

```bash
nlm-import <input_file> [config_file] [output_dir]
```

#### Examples

```bash
# Import brain state from binary file
nlm-import brain_state.bin configs/default.cfg ./restored_brain

# Import JSON research data
nlm-import research_data.json configs/expertise.cfg ./imported_data
```

### nlm-profile

Profile NLM performance with detailed metrics and analysis.

#### Syntax

```bash
nlm-profile [duration] [output_file] [phase] [config_file]
```

#### Parameters

| Parameter | Description | Default | Required |
|--------------|-------------|---------|---------|
| duration | Profile duration in seconds | 60 | No |
| output_file | Output file path for results | profile_results.csv | No |
| phase | NLM phase to profile | 6 | No |
| config_file | Configuration file path | configs/default.cfg | No |

#### Examples

```bash
# Profile Phase 6 for 2 minutes
nlm-profile 120 performance_results.csv 6

# Profile with custom configuration
nlm-profile 300 research_profile.txt 3 my_config.cfg
```

### nlm-debug

Advanced debugging tools for NLM brain state analysis.

#### Syntax

```bash
nlm-debug [brain_state] [analysis_type]
```

#### Analysis Types

| Analysis Type | Description |
|---------------|------------- |
| basic | Basic brain analysis |
| spikes | Spike pattern analysis |
| plasticity | Plasticity and learning analysis |
| memory | Memory system analysis |
| performance | Performance metrics analysis |
| integration | Integration system analysis |

#### Examples

```bash
# Basic brain debug analysis
nlm-debug brain_state.bin basic

# Spike pattern analysis
nlm-debug research_data.xml spikes

# Integration system analysis
nlm-debug experimental_brain.binary integration
```

## Advanced Features

### Configuration Schema Validation

The CLI includes comprehensive configuration validation:

- **Type checking**: Ensures parameters have correct data types
- **Range validation**: Validates parameter ranges
- **Duplicate detection**: Detects duplicate keys
- **Dependency validation**: Checks required dependencies

### Environment Variables

```bash
# Enable verbose output
export NLM_VERBOSE=true

# Set default configuration file
export NLM_CONFIG=configs/expertise.cfg

# Set profile output directory
export NLM_PROFILE_DIR=./research_profiles

# Enable debug mode
export NLM_DEBUG=true
```

### Command-line Options

```bash
# Show help information
nlm-help

# Initialize CLI tools and directories
nlm-init

# Show version information
nlm-version

# Show license information
nlm-license

# Show system information
nlm-system-info
```

## Research Utilities

### Data Collection and Analysis

| Utility | Purpose | Format |
|---------|---------|--------|
| `nlm-export` | Brain state export | Binary, JSON, XML, Graphics |
| `nlm-import` | Brain state import | Multiple formats |
| `nlm-profile` | Performance profiling | CSV, TXT |
| `nlm-debug` | Brain analysis | Text reports |

### Research Workflows

#### Workflow 1: Experiment Reproduction

```bash
# jąve 1. Configure experiment
nlm-config set brain.neuron_count 1000 configs/experiment1.cfg
nlm-config set brain.synapse_density 0.15 configs/experiment1.cfg

# Step 2. Run experiment
nlm-phase 6 configs/experiment1.cfg true false

# Step 3. Export results
nlm-export experiment1_results.json json

# Step 4. Profile performance
nlm-profile 300 experiment1_profile.csv 6 configs/experiment1.cfg
```

#### Workflow 2: Comparative Analysis

```bash
# Run multiple experiments with different parameters
nlm-phase 6 configs/base.cfg false false
nlm-export base_results.json json

nlm-config set brain.neuron_count 2000 configs/varied.cfg
nlm-phase 6 configs/varied.cfg false false
nlm-export varied_results.json json

# Compare results
nlm-debug base_results.json compare
nlm-debug varied_results.json compare
```

#### Workflow 3: Performance Optimization

```bash
# Profile current configuration
nlm-profile 300 current_perf.csv 6

# Optimize configuration
nlm-config set brain.synapse_density 0.05 configs/optimized.cfg
nlm-profile 300 optimized_perf.csv 6 configs/optimized.cfg

# Compare performance
nlm-debug current_perf.csv vs optimized_perf.csv
```

## Integration with Python API

The NLM CLI integrates seamlessly with the Python API `pynlm`:

### Export Brain State via CLI

```bash
# Export brain state for Python analysis
nlm-export python_research.bin binary configs/expertise.cfg

# Import into Python for analysis
python3 -c "import pynlm; brain = pynlm.loadBrain('python_research.bin')"
```

### CLI to Python Pipeline

```bash
# Run NLM experiment via CLI
nlm-phase 6 research_config.cfg true false

# Export results for Python analysis
nlm-export experiment_results.json json

# Process in Python
python3 process_results.py experiment_results.json
```

## Configuration File Templates

### Template 1: Basic Configuration

```ini
# Basic NLM Configuration
brain.neuron_count = 1000
brain.synapse_density = 0 septum
brain.connection_probability = 0.05

plasticity.stdp.enable = true
plasticity.stdp.learning_rate =  terning

neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true
```

### Template 2: Expert Configuration

```ini
# Expert NLM Configuration
brain.neuron_count = 5000
brain.synapse_density = 0.2
brain.connection_probability = 0.15
brain.v_thresh = -50.0
brain.v_rest = -70.0
brain.tau_mem = 20.0

plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.02
plasticity.stdp.tau_plus = 20.0
plasticity.stdp.tau_minus = 20.0

plasticity.hebbian.enable = true
plasticity.structural.enable = true

neuromod.dopamine.scale = 2.0
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

environment.max_energy = 100.0
environment.energy_decay_rate = 0.01
```

### Template 3: Research Configuration

```ini
# Research NLM Configuration
brain.neuron_count = 3000
brain.synapse_density = 0.1
brain.connection_probability = 0.08

plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001

neuromod.dopamine.scale = 1.5
neuromod.curiosity.enable = true
neuromod.novelty.enable = true

memory.working_memory.capacity = 10
memory.episodic_memory.capacity = 100

prediction.prediction_enabled = true
prediction.prediction_error = 0.1
```

## Error Handling and Diagnostics

### Common Error Messages

| Error Message | Cause | Solution |
|---------------|-------|---------- |
| "Config file not found" | Invalid config path | Use valid config file path | error |
| "Parameter not found" | Invalid parameter name | Use valid parameter name |
| "Configuration validation failed" | Invalid config syntax | Fix configuration file |
| "Export failed" | File permission issues | Check file permissions |
| "Import failed" | Corrupted file | Use valid import file |

### Troubleshooting

#### Configuration Issues

```bash
# Check for configuration syntax errors
nlm-config validate configs/broken.cfg

# Fix duplicate keys
# Manually edit configs/broken.cfg to remove duplicates
```

#### Performance Issues

```bash
# Check for performance bottlenecks
nlm-profile 300 performance_report.csv 6

# Optimize configuration
nlm-config set brain.synapse_density 0.05 configs/optimized.cfg
```

#### Import/Export Issues

```bash
# Check file permissions
chmod 644 brain_state.bin

# Verify file integrity
file brain_state.bin
ls -la brain_state.bin
```

## Migration Guide

### From Basic CLI to Advanced CLI

The NLM CLI has been significantly enhanced from the basic version. Here's what's changed:

#### New Features

| Feature | Basic CLI | Advanced CLI |
|---------|-----------|--------------|
| Phase Selection | `nlm 2` | `nlm-phase 2` |
| Configuration | `nlm-config get param` | `nlm-config get param` |
| Help System | `nlm --help` | `nlm-help` |
| Performance Profiling | Not available | `nlm-profile` |
| Brain State Export | Not available | `nlm-export` |
| Brain State Import | Not available | `nlm-import` |

#### Migration Steps

1. Replace `nlm 2` with `nlm-phase 2`
2. Use `nlm-help` instead of `nlm --help`
3. Add profiling with `nlm-profile`
4. Enable advanced features with environment variables
5. Use `nlm-config` for comprehensive configuration management

### From Python to CLI

If you're migrating from Python to CLI:

1. **Python API**: `pynlm.createBrain(config)`
   **CLI**: `nlm-phase 6 configs/expertise.cfg`

2. **Python API**: `brain.step(step)`
   **CLI**: `nlm-phase 6 --headless true`

3. **Python API**: `pynlm.saveBrain(filename)`
   **CLI**: `nlm-export filename binary`

## License and Copyright

### License

```
MIT License

Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## Acknowledgements

This CLI enhancement was developed as part of the NLM Phase 6 Final Integration project. It builds upon the foundation established in previous phases and incorporates advances in command-line interface design and user experience.

### Key Contributors

- NLM Core Development Team
- CLI Architecture Design
- Documentation and User Experience

### Dependencies

- pybind11 2.11.0+
- scikit-build-core
- CMake 3.16+
- C++20 compatible compiler

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 2.0.0 | 2026-11-16 | Initial advanced CLI release |
| 1.0.0 | 2026-10-01 | Basic CLI functionality |

## Future Plans

### Planned Enhancements

1. **Web-based GUI**: Interactive web interface for NLM control
2. **Docker Support**: Containerized deployment
3. **REST API**: Programmatic access via HTTP
4. **Machine Learning Integration**: Advanced ML model training
5. **Distributed Computing**: Multi-machine NLM simulation

### Research Extensions

1. **Multi-Agent Systems**: Support for competing/cooperating agents
2. **Cross-Platform Export**: Export to various scientific formats
3. **Real-time Visualization**: Live brain state visualization
4. **Cloud Integration**: Integration with cloud computing platforms

## Contact and Support

### Support Resources

- **Documentation**: https://nlm.research/docs/cli
- **GitHub**: https://github.com/nlm-project/nlm-cli
- **Issues**: https://github.com/nlm-project/nlm-cli/issues
- **Discussions**: https://github.com/nlm-project/nlm-cli/discussions

### Community

Join the NLM community for discussions, support, and collaboration:

- **GitHub Discussions**: Feature requests and feedback
- **Technical Support**: bug reports and feature requests
- **Research Collaboration**: Partner with NLM research team

### Getting Help

| Situation | Recommended Action |
|-----------|------------------- |
| CLI usage questions | Check documentation |
| Configuration issues | Use `nlm-config validate` |
| Performance problems | Use `nlm-profile` |
| Import/export issues | Check file permissions |
| Bug reports | Submit to GitHub Issues |
| Feature requests | Submit to GitHub Discussions |

This documentation is continuously updated as new features are added and improvements are made to the NLM CLI.