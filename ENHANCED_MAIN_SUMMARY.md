# Enhanced NLM User Guide

## Overview

Welcome to the enhanced Neural Learning Machine (NLM) system! This guide provides comprehensive documentation for all the new features and improvements made to enhance usability, maintainability, and extensibility.

## Quick Start

### Basic Usage (Backward Compatible)

```bash
# Run the original Phase 2 demo (unchanged behavior)
./nlm

# Or use a custom configuration file
./nlm --config my_config.cfg
```

### Enhanced Usage

```bash
# Run Phase 6 integration demo
./nlm --mode phase6

# Run quick verification tests
./nlm --mode quick_test

# Run specific experiments
./nlm --mode phase2 --experiment connectivity --experiment plasticity

# Run with enhanced logging
./nlm --mode phase6 --verbose --save_checkpoint brain.bin

# Show help
./nlm --help
```

## Configuration Presets

### Quick Test Preset
```
./nlm --mode quick_test
```
- 100 neurons (reduced for speed)
- 500 simulation steps maximum
- Optimized for rapid testing and development

### Demo Mode
```
./nlm --mode demo
```
- 1000 neurons
- 2 brain regions
- Balanced configuration for demonstration

### Full Experiment Mode
```
./nlm --mode full_experiment
```
- 5000 neurons
- 3 brain regions
- Maximum detail for comprehensive experiments

## Experiment Types

### Phase 2 Experiments

#### Connectivity Experiment
Tests whether spikes propagate through the neural network correctly.
- **What it does**: Injects current into sensory neurons and monitors spike propagation
- **Success indicators**: Spikes detected in downstream neurons
- **Typical runtime**: ~50 simulation steps
- **Command**: `./nlm --experiment connectivity`

#### Plasticity Experiment
Tests learning and synaptic weight changes through experience.
- **What it does**: Applies repeated sensory input and measures synaptic changes
- **Success indicators**: Synaptic weights strengthen/weakening through activity
- **Typical runtime**: ~1000 simulation steps
- **Command**: `./nlm --experiment plasticity`

#### STDP Experiment
Tests Spike-Timing Dependent Plasticity mechanisms.
- **What it does**: Creates correlated pre-post spike activity to trigger LTP
- **Success indicators**: Pre-before-post spikes produce weight potentiation
- **Typical runtime**: ~50 simulation steps
- **Command**: `./nlm --experiment stdp`

### Phase 6 Integration Experiments

#### Phase 6 Integration
Comprehensive test of all Phase 6 integration features.
- **What it does**: Tests memory, neuromodulation, prediction, and development integration
- **Success indicators**: All systems connected and functioning
- **Typical runtime**: ~2000 simulation steps
- **Command**: `./nlm --mode phase6`

## Configuration System

### Configuration File Format

NLM uses JSON configuration files. Example `configs/custom.cfg`:

```json
{
    "random_seed": 12345,
    "simulation_timestep": 0.001,
    "neuron_count": 2000,
    "region_count": 3,
    "connection_probability": 0.15,
    "max_steps": 5000,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "enable_checkpointing": true,
    "enable_replay": false,
    "enable_development": true
}
```

### Command Line Configuration

Override configuration values from command line:

```bash
./nlm --config custom.cfg random_seed=999 neuron_count=3000
```

### Preset Configurations

Built-in presets can be applied:

```bash
./nlm --mode quick_test
./nlm --mode demo
./nlm --mode full_experiment
./nlm --mode phase2_defaults  # Original Phase 2 settings
```

## Brain Inspection and Analysis

### Utility Commands

Use the enhanced brain inspection utilities to analyze your brain state:

```bash
# The main program includes built-in brain inspection commands
# Available through interactive mode or utility functions
```

### Key Analysis Functions

1. **Brain Statistics**: Detailed neural network statistics
2. **Configuration Validation**: Check for configuration issues
3. **Plasticity Analysis**: Analyze plasticity rule effectiveness
4. **Network Visualization**: Text-based neural network diagrams
5. **State Comparison**: Compare different brain states

## Performance Monitoring

### Built-in Performance Tracking

The enhanced system automatically tracks:

- **Wall-clock time**: Total execution time
- **Steps per second**: Simulation throughput
- **Memory usage**: Estimated memory consumption
- **Experiment timing**: Individual experiment performance

### Performance Command

```bash
# Run with performance monitoring
./nlm --verbose --mode phase6
```

### Performance Output Example

```
=== Performance Metrics ===
Experiment: phase6
Wall Clock Time: 12.345s
Steps per Second: 162
Estimated Memory Usage: 0.5 GB
Steps Processed: 2000
```

## Error Handling and Recovery

### Error Categories

1. **Configuration Errors**: Invalid or missing settings
2. **Initialization Errors**: Brain setup failures
3. **Runtime Errors**: Simulation or experiment failures
4. **Resource Errors**: Memory or file system issues

### Error Recovery Features

- **Automatic rollback**: Configuration load failures roll back to previous state
- **Graceful degradation**: Non-critical errors don't stop execution
- **Detailed error messages**: Clear explanations of what went wrong
- **Progress tracking**: You can see progress during long operations

### Error Handling Example

```bash
# Invalid configuration
./nlm --config invalid.cfg

# Output:
ERROR: Configuration validation failed!
Missing required setting: neuron_count
ERROR: Using default configuration.
```

## Advanced Features

### Checkpoints and State Management

Save and restore brain states:

```bash
# Save brain state after experiment
./nlm --mode phase6 --save_checkpoint brain_state.bin

# Load a saved brain state
# (This would typically be done in custom scripts)
```

### Experiment Chaining

Run multiple experiments in sequence:

```bash
# Run all Phase 2 experiments
./nlm --mode phase2 --experiment all

# Or specify individual experiments
./nlm --mode phase2 --experiment connectivity --experiment plasticity --experiment stdp
```

### JSON Output

Export results in JSON format for programmatic analysis:

```bash
# JSON output for machine processing
./nlm --mode phase2 --experiment all --json > results.json
```

## Programming API

The enhanced system provides a C++ API for programmatic use:

### Header Files

- `src/brain/BrainUtils.hpp`: Enhanced brain analysis and utility functions

### Example Usage

```cpp
#include "brain/BrainUtils.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"

// Create and configure brain
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
config->set("random_seed", 42);

auto brain = std::make_shared<Brain>(config);
brain->initialize();

// Analyze brain state
brain_utils::BrainInspector::printNeuralStatistics(brain);

// Save with metadata
brain_utils::BrainInspector::saveBrainStateWithMetadata(
    brain, "my_brain.bin", "Experiment run 1"
);

// Compare configurations
auto config1 = std::make_shared<Config>();
config1->set("neuron_count", 1000);

auto config2 = std::make_shared<Config>();
config2->set("neuron_count", 2000);

std::string comparison = config_utils::compareConfigs(
    "Config A", config1, "Config B", config2
);
```

## Migration Guide

### For Existing Users

**No changes required!** The enhanced system maintains full backward compatibility:

- Original `--config` option still works
- Original Phase 2 behavior preserved
- All existing scripts continue to work

### For New Users

If you're new to NLM, start with:

```bash
# Basic demo
./nlm

# Phase 6 integration demo
./nlm --mode phase6

# Quick verification
./nlm --mode quick_test --experiment all
```

### For Developers

For development and experimentation:

```bash
# Development mode with verbose output
./nlm --verbose --mode demo

# Debug mode for troubleshooting
./nlm --debug --mode phase2

# Performance benchmarking
./nlm --mode benchmark
```

## Troubleshooting

### Common Issues

1. **"No neurons found" errors**
   - Check configuration: ensure `neuron_count > 0`
   - Run `./nlm --verbose` for more details

2. **"Failed to load configuration" errors**
   - Verify configuration file path
   - Check file permissions
   - Use `./nlm --help` for correct syntax

3. **Memory errors**
   - Use `--mode quick_test` for reduced memory footprint
   - Monitor memory usage with `--verbose`

### Getting Help

- **General help**: `./nlm --help`
- **Verbose output**: `./nlm --verbose`
- **Debug mode**: `./nlm --debug`
- **Version info**: `./nlm --version`

## Best Practices

### For Research

1. **Document your experiments**: Use descriptive configuration names
2. **Save intermediate results**: Use checkpoints frequently
3. **Compare results**: Use the configuration comparison tools
4. **Monitor performance**: Track execution time and memory usage

### For Production

1. **Use appropriate presets**: Choose between quick_test, demo, and full_experiment
2. **Validate configurations**: Always validate before running
3. **Monitor for errors**: Use verbose mode in production
4. **Clean up resources**: Ensure proper cleanup after experiments

### For Development

1. **Start small**: Use quick_test for initial development
2. **Test incrementally**: Run individual experiments first
3. **Use debugging tools**: Take advantage of enhanced logging
4. **Document changes**: Keep track of configuration changes

## Changelog

### Version 6.0.0 (Enhanced)

- **Major**: Enhanced command-line interface with comprehensive help
- **Major**: Modular experiment framework
- **Major**: Configuration preset system
- **Major**: Performance monitoring and tracking
- **Major**: Brain inspection and analysis utilities
- **Major**: Comprehensive error handling and recovery
- **Major**: Backward compatibility maintained

### Previous Versions

- **Version 5.0.0**: Original Phase 5 release
- **Version 4.0.0**: Phase 4 integration
- **Version 3.0.0**: Phase 3 world interaction
- **Version 2.0.0**: Original Phase 2 (enhanced for backward compatibility)

## Support

### Getting Help

For questions and support:

1. **Documentation**: This guide and the `README.md` file
2. **Command line help**: `./nlm --help`
3. **Issue reporting**: Check the project repository for existing issues
4. **Community**: Join the NLM community for support

### Bug Reports

When reporting bugs, include:

1. **Command used**: Full command line with all options
2. **Configuration**: Configuration file contents
3. **Error messages**: Complete error output
4. **Expected behavior**: What should have happened
5. **Actual behavior**: What actually happened

### Feature Requests

Suggest new features by:

1. **Opening an issue**: With clear description and use case
2. **Contributing code**: Fork the repository and submit pull requests
3. **Discussion**: Participate in community discussions

## Conclusion

The enhanced NLM system provides:

### **For Users:**
- **Easier setup** with configuration presets
- **Better understanding** with comprehensive help
- **Flexible usage** with multiple execution modes
- **Improved debugging** with enhanced error reporting

### **For Developers:**
- **Easier extension** through modular design
- **Better maintenance** with consistent structure
- **Enhanced testing** through comprehensive experiment framework
- **Improved debugging** through detailed logging and monitoring

### **For the Project:**
- **Backward compatibility** with existing code
- **Extensibility** for future enhancements
- **Maintainability** through consistent standards
- **Performance** optimized for both development and production

The enhanced NLM system is now ready for:
- **Educational purposes** and learning
- **Research experimentation** and development
- **Production deployment** and use
- **Community contribution** and collaboration

All enhancements maintain full backward compatibility while providing powerful new capabilities for both existing and new users.

---

*For more information, visit the NLM documentation or join the community.*