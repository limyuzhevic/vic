# NLM User Guide - Phase 2: Real Neural Computation

## Introduction

NLM (Neural Learning Machine) is an experimental artificial developmental brain implementing real neural computation with Leaky Integrate-and-Fire (LIF) neurons, event-driven spike propagation, STDP and Hebbian plasticity, and structural plasticity.

## Command-Line Interface

The improved NLM implementation now provides enhanced command-line argument parsing for advanced users:

### Basic Usage
```bash
nlm                    # Run all tests with default settings
```

### Help and Usage
```bash
nlm --help             # Display comprehensive help information
nlm -h                 # Short help
```

### Configuration Options
```bash
# Load custom configuration file
nlm --config=myconfig.cfg
nlm -c myconfig.cfg

# Run specific test modes
nlm --test-mode=connectivity    # Only connectivity test
nlm --test-mode=plasticity       # Only plasticity experiment
nlm --test-mode=stdp             # Only STDP verification
nlm --test-mode=all              # All tests (default)

# Control simulation parameters
nlm --steps=5000                # Increase simulation steps
nlm --seed=123                  # Set random seed for reproducibility
nlm --neuron-count=1000         # Increase neuron count
nlm --region-count=3            # Multi-region configuration
nlm --checkpoint-dir=./checkpoints # Custom checkpoint directory

# Verbosity and output control
nlm --verbose                   # Enable debug logging
nlm --quiet                     # Suppress informational messages
```

### Configuration File Format

NLM configuration files use a simple key=value format:

```ini
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 2
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Neuromodulation settings
dopamine_baseline = 0.1

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0
```

### Configuration File Best Practices

1. **File Location**: Place configuration files in the `configs/` directory
2. **Key Names**: Use snake_case for configuration keys (e.g., `random_seed`)
3. **Comments**: Use `#` for comments
4. **Value Types**:
   - Numbers: `integer`, `float`
   - Strings: `"string"` (quoted)
   - Arrays: `[1, 2, 3]` or `["a", "b", "c"]`
5. **Duplicate Keys**: Later values override earlier ones
6. **Validation**: Invalid values are logged with warnings

### Advanced Configuration

#### Command Line Argument Precedence
```bash
# Command line arguments override config file values
nlm --config=small.cfg --neuron-count=2000
# neuron_count will be 2000, not 500 from small.cfg
```

#### Configuration Sources
1. **Default Values**: Hardcoded in main.cpp
2. **Config File**: `configs/default.cfg` or specified with `--config`
3. **Command Line**: Parsed after config file
4. **Runtime**: Programmatically set during execution

### Error Handling and Logging

NLM provides comprehensive logging with different severity levels:

- **Debug**: Detailed debugging information (requires `--verbose`)
- **Info**: General information messages
- **Warning**: Warnings about potential issues
- **Error**: Errors that prevent execution
- **Critical**: Critical system errors

### Example Advanced Usage

#### Reproducible Research Experiments
```bash
# Reproducible with fixed seed and more steps
nlm --seed=42 --steps=10000 --test-mode=plasticity

# Multi-region experiment
nlm --region-count=5 --neuron-count=2000 --test-mode=all

# Custom checkpoint for long experiments
nlm --checkpoint-dir=./experiments/exp_001 --verbose
```

#### Testing and Development
```bash
# Run only specific test to speed up development
nlm --test-mode=connectivity

# Debug configuration issues
nlm --config=debug.cfg --verbose

# Verify STDP parameters
nlm --test-mode=stdp --verbose
```

### Configuration Debugging

If you encounter configuration issues:

1. **Check syntax**: Ensure proper formatting in config files
2. **Verbose logging**: Use `--verbose` for detailed configuration messages
3. **Test manually**: Use the standalone Config tests:

```bash
# To run Config tests specifically (after building):
./test_config
```

4. **Common issues**:
   - Missing required keys
   - Incorrect value types
   - Duplicate keys
   - File permissions

### Performance Tuning

#### Recommended Settings for Different Use Cases

**Research Experiments**:
```bash
nlm --seed=42 --steps=50000 --neuron-count=2000 --region-count=4
```

**Development/Testing**:
```bash
nlm --test-mode=connectivity --verbose
```

**Real-time Applications**:
- Use smaller neuron counts
- Optimize checkpoint directories
- Consider limiting history size

### Configuration Migration

#### From Older Versions

If upgrading from an older version, note these changes:

1. **New command-line options**: `--test-mode`, `--steps`, `--verbose`, etc.
2. **Config file format**: No changes (backwards compatible)
3. **Error handling**: Improved validation and logging
4. **Default values**: Some defaults changed for better behavior

#### Example Migration Script
```bash
#!/bin/bash
# Backup old config
mv configs/default.cfg configs/default.cfg.backup

# Create new config with improved settings
cat > configs/default.cfg << EOF
# Improved configuration for NLM
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000  # Increased for better statistics
region_count = 2     # Multi-region for diversity
connection_probability = 0.15  # Better connectivity

# Enhanced plasticity
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Neuromodulation
dopamine_baseline = 0.1

# Logging
log_level = INFO
log_to_file = true
log_filename = nlm.log

# Performance
max_simulation_steps = 20000
simulation_time_limit = 0.0
EOF

echo "Configuration updated. Run: nlm --test-mode=all --verbose"
```

## Troubleshooting

### Common Issues

1. **Configuration File Not Found**
   - Error: "Failed to open config file for reading"
   - Solution: Check file path and permissions
   - Usage: `nlm --config=./myconfig.cfg`

2. **Invalid Command Line Arguments**
   - Error: "Unknown command line argument"
   - Solution: Use `nlm --help` for available options

3. **Tests Failing**
   - Solution: Check verbose output (`--verbose`) for error details
   - Common issue: Configuration values not properly loaded

4. **Memory Issues**
   - Solution: Reduce `--neuron-count` and `--steps`

### Getting Help

- **Command line help**: `nlm --help`
- **Configuration help**: See configuration file format above
- **Bug reports**: Check verbose output for error details
- **Documentation**: See `docs/` directory for detailed technical information

## Next Steps

For more advanced usage:

1. **Explore Phase 6 integration**: `./nlm_phase6_demo`
2. **Advanced configuration**: Modify config files in `configs/`
3. **Performance optimization**: Study benchmark results
4. **Research applications**: Use `--seed` for reproducible experiments
5. **Development**: Use `--verbose` and `--test-mode` for testing

## Further Improvements

The command-line interface can be further enhanced with:

1. **Config file validation**: Schema validation for config files
2. **Config file templates**: Predefined templates for common use cases
3. **Interactive mode**: Command-line REPL for runtime configuration
4. **Environment variables**: Support for environment variable configuration
5. **Web interface**: Web-based configuration and monitoring
6. **Plugin system**: Pluggable configuration modules

## References

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [Phase 6 Integration Audit](docs/PHASE6_FINAL_AUDIT.md)
- [Technical Reference](docs/SCIENCE.md)

Current improvements focus on:
- ✅ Enhanced error handling and validation
- ✅ Better command-line argument parsing
- ✅ Improved configuration file handling
- ✅ Comprehensive logging and debugging
- ✅ User-friendly documentation and guides
- ✅ Backward compatibility with existing configurations
