# NLM Phase 6 - Advanced Command Line Interface

This document describes the advanced command-line interface for NLM Phase 6, the final integration of all artificial brain systems.

## Overview

The NLM Phase 6 system now includes an enhanced command-line interface that provides:
- Advanced configuration options
- Multiple experiment modes
- Performance optimization controls
- Memory system testing capabilities
- Neuromodulation and cognition system testing

## Installation

After building NLM, the enhanced CLI is available through the `nlm_cli.sh` script in the scripts directory.

```bash
# Make the CLI script executable
chmod +x scripts/nlm_cli.sh

# Use the enhanced CLI
./scripts/nlm_cli.sh [options]

# Or use the built-in help
./scripts/nlm_cli.sh --help
```

## Core Options

### Basic Usage

```bash
nlm_cli.sh                    # Run with default Phase 6 configuration
nlm_cli.sh --help             # Show help information
nlm_cli.sh --version          # Show version information
```

### Configuration Options

```bash
nlm_cli.sh --config myconfig.cfg     # Load configuration from file
nlm_cli.sh --neurons 500            # Set number of neurons
nlm_cli.sh --regions 2              # Set number of brain regions
nlm_cli.sh --timestep 0.001          # Set simulation timestep
nlm_cli.sh --seed 42                 # Set random seed
```

## Advanced Options

### Experiment Modes

The enhanced CLI supports running specific experiment modes:

```bash
# Test only memory systems
nlm_cli.sh --memory-only

# Test only cognition systems
nlm_cli.sh --cognition-only

# Test only neuromodulation systems
nlm_cli.sh --neuromodulation-only

# Run complete integration validation
nlm_cli.sh --validate

# Run specific experiment
nlm_cli.sh --experiment memory
nlm_cli.sh --experiment cognition
nlm_cli.sh --experiment neuromodulation
nlm_cli.sh --experiment integration
```

### Performance Options

```bash
# Enable parallel processing
nlm_cli.sh --threads 4

# Enable sparse connectivity optimization
nlm_cli.sh --sparse

# Enable SIMD vectorization
nlm_cli.sh --vectorize

# Run benchmarks
nlm_cli.sh --benchmark
```

### Memory and Persistence Options

```bash
# Enable checkpointing
nlm_cli.sh --checkpoint brain_state.bin

# Save brain state and exit
nlm_cli.sh --save current_brain.bin

# Load brain state and continue
nlm_cli.sh --load saved_brain.bin

# Enable replay functionality
nlm_cli.sh --replay

# Enable developmental plasticity
nlm_cli.sh --development
```

## Preset Configurations

The enhanced CLI includes preset configurations for different use cases:

### Fast Preset (--preset fast)

Quick test with minimal configuration:
```bash
nlm_cli.sh --preset fast
# Equivalent to: --neurons 100 --regions 1 --timestep 0.01 --simulation 1000
```

### Balanced Preset (--preset balanced)

Standard configuration for most use cases:
```bash
nlm_cli.sh --preset balanced
# Equivalent to: --neurons 1000 --regions 2 --timestep 0.001 --simulation 10000
```

### Deep Preset (--preset deep)

Comprehensive configuration with all features:
```bash
nlm_cli.sh --preset deep
# Equivalent to: --neurons 5000 --regions 4 --timestep 0.0005 --simulation 50000 --development --replay --checkpoint brain_deep.bin
```

## Configuration Files

### Basic Configuration

Create a basic configuration file (e.g., `basic.cfg`):

```ini
# Basic neural configuration
neuron_count = 1000
region_count = 1
connection_probability = 0.1
simulation_timestep = 0.001
random_seed = 42
```

### Advanced Configuration

Create an advanced configuration file (e.g., `advanced.cfg`):

```ini
# Advanced neural configuration
neuron_count = 5000
region_count = 4
connection_probability = 0.15
simulation_timestep = 0.0005
random_seed = 123

# Memory systems
working_memory_capacity = 500
episodic_memory_capacity = 5000
consolidation_interval = 2000
replay_interval = 200

# Neuromodulation
dopamine_baseline = 0.1
curiosity_baseline = 0.1

# Development
critical_period_start = 100
critical_period_end = 500

# Performance
sparse_connectivity_enabled = true
simd_vectorization_enabled = true
multithreading_enabled = true
max_threads = 4

# Persistence
checkpoint_enabled = true
checkpoint_path = ./deep_brain.bin
```

### Configuration File Loading

```bash
# Load configuration from file
nlm_cli.sh --config advanced.cfg

# Load configuration and override specific options
nlm_cli.sh --config advanced.cfg --neurons 2000 --regions 2
```

## Output Options

### Verbosity Control

```bash
# Quiet mode (minimal output)
nlm_cli.sh --quiet

# Debug mode (maximum output)
nlm_cli.sh --debug

# Normal verbose output (default)
nlm_cli.sh
```

### Output Redirection

```bash
# Save output to file
nlm_cli.sh --debug > nlm_output.txt 2>&1

# Show only errors
nlm_cli.sh --quiet 2>&1 | grep ERROR
```

## Examples

### Basic Usage Examples

```bash
# Quick help
./scripts/nlm_cli.sh --help

# Show all configuration options
./scripts/nlm_cli.sh --config-help

# Run with preset configuration
./scripts/nlm_cli.sh --preset balanced

# Test memory systems only
./scripts/nlm_cli.sh --memory-only

# Save brain state
./scripts/nlm_cli.sh --save research_brain.bin

# Load brain state and continue
./scripts/nlm_cli.sh --load research_brain.bin
```

### Advanced Usage Examples

```bash
# Run comprehensive experiment with all features
./scripts/nlm_cli.sh --preset deep --debug

# Run integration validation with minimal output
./scripts/nlm_cli.sh --validate --quiet

# Save benchmark results
./scripts/nlm_cli.sh --benchmark --save benchmark_results.txt

# Test with custom configuration
./scripts/nlm_cli.sh --config custom.cfg --neurons 2000 --regions 3 --development
```

## Advanced Configuration Options

### Memory System Configuration

- `working_memory_capacity`: Capacity of working memory (default: 100)
- `episodic_memory_capacity`: Capacity of episodic memory (default: 1000)
- `consolidation_interval`: Steps between memory consolidation (default: 1000)
- `replay_interval`: Steps between memory replay (default: 100)

### Neuromodulation Configuration

- `dopamine_baseline`: Baseline dopamine level (default: 0.1)
- `dopamine_peak`: Peak dopamine level (default: 1.0)
- `curiosity_baseline`: Baseline curiosity level (default: 0.1)
- `novelty_sensitivity`: Sensitivity to novel stimuli (default: 0.5)

### Development Configuration

- `critical_period_start`: Start of critical period (default: 50)
- `critical_period_end`: End of critical period (default: 200)
- `maturation_progress_rate`: Rate of developmental maturation (default: 0.01)

### Performance Configuration

- `sparse_connectivity_enabled`: Enable sparse connectivity optimization (default: true)
- `simd_vectorization_enabled`: Enable SIMD vectorization (default: true)
- `multithreading_enabled`: Enable multithreaded processing (default: true)
- `max_threads`: Maximum number of threads (default: 4)

### Persistence Configuration

- `checkpoint_enabled`: Enable checkpoint save/load (default: true)
- `checkpoint_path`: Path for checkpoint files (default: ./checkpoint_test.bin)
- `auto_save_interval`: Steps between automatic saves (default: 1000)

## Troubleshooting

### Common Issues

#### "Configuration file not found"

```bash
# Solution: Use absolute path or place file in current directory
nlm_cli.sh --config /path/to/config/file.cfg
nlm_cli.sh --config ./config/advanced.cfg
```

#### "Too many threads specified"

```bash
# Solution: Check available CPU cores and adjust
nlm_cli.sh --threads 4  # Typical configuration for quad-core system
```

#### "Memory usage too high"

```bash
# Solution: Reduce neuron count or disable some features
nlm_cli.sh --preset fast    # Minimal configuration
nlm_cli.sh --memory-only    # Test only memory systems
```

### Error Messages

- `--config file.cfg: No such file or directory`
  - Ensure the configuration file exists and is readable

- `--preset unknown_preset`
  - Use `--preset help` to see available presets

- `--experiment unknown_mode`
  - Use `--experiment memory|cognition|neuromodulation|integration`

## Further Reading

### Documentation

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - Detailed system architecture
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 integration results
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background and methodology
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experimental protocols and procedures

### Research Papers

For academic references and detailed methodology:
- See `docs/SCIENCE.md` for comprehensive research background
- Consult `docs/PHASE6_FINAL_RESULTS.md` for experimental results

### Code Examples

For code examples and API documentation:
- See `examples/` directory (if available)
- Review the `Phase6Demo.cpp` and `Phase6IntegratedExperiment.cpp` source files

## Contributing

### Command Line Enhancements

To contribute to the enhanced command-line interface:

1. Add new experiment modes or configuration options
2. Enhance existing help text and documentation
3. Add new preset configurations
4. Improve argument parsing and validation

### Configuration Enhancements

To contribute to configuration improvements:

1. Add new configuration sections for additional systems
2. Enhance existing configuration options with more granularity
3. Add configuration validation and error checking
4. Add support for configuration inheritance and overriding

### Performance Improvements

To contribute to performance improvements:

1. Optimize command-line argument parsing
2. Add performance profiling options
3. Improve memory usage reporting
4. Add parallel processing controls

## Support

### Getting Help

For help with the enhanced command-line interface:

1. Check the `--help` option for basic usage
2. Use `--config-help` for configuration options
3. Consult the documentation in the `docs/` directory
4. Check GitHub issues for known problems and solutions

### Reporting Issues

When reporting issues with the enhanced CLI:

1. Include the full command line you used
2. Include any error messages or stack traces
3. Describe the expected vs. actual behavior
4. Provide configuration files if applicable

### Bug Reports

Known issues and limitations:

- Some configuration options may require specific combinations
- Performance options may vary based on system capabilities
- Configuration file parsing may have limitations with complex values
- The CLI script itself is a wrapper around the main NLM executable

## Version Information

This documentation is for NLM Phase 6 Enhanced CLI.
For the latest version information, see the `--version` option or the project documentation.

## License

This enhanced command-line interface is part of the NLM Phase 6 project, licensed under the MIT License.
See the LICENSE file for details.