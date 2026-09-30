# NLM Configuration File Examples

This document describes the configuration file format used by the NLM (熙然) brain simulation system.

## Configuration Format

NLM uses a simple key=value format for configuration files:

```
# Comment line
key = value
key2 = "quoted value"
key3 = 'single quoted value'
```

## Required Configuration Keys

### Neural Parameters
- `neuron_count`: Number of neurons in the simulation (default: 1000)
- `region_count`: Number of neural regions (default: 1)
- `connection_probability`: Probability of connections between neurons (default: 0.1)

### Timing Parameters
- `simulation_timestep`: Time step for simulation in seconds (default: 0.001)
- `stdp_tau`: STDP time constant in milliseconds (default: 20.0)

### Plasticity Parameters
- `stdp_ltp_weight`: STDP LTP weight (default: 0.02)
- `stdp_ltd_weight`: STDP LTD weight (default: 0.015)
- `synaptogenesis_rate`: Rate of new synapse formation (default: 0.0001)
- `pruning_rate`: Rate of synapse elimination (default: 0.00001)

### Memory Parameters
- `replay_interval`: Steps between memory replay (default: 100)
- `consolidation_interval`: Steps between memory consolidation (default: 1000)

### Random Seed
- `random_seed`: Seed for random number generator (default: 42)

## Example Configuration Files

### Basic Configuration
```
# NLM Brain Configuration - Basic Setup
neuron_count = 500
region_count = 1
connection_probability = 0.15

simulation_timestep = 0.001
random_seed = 42

# STDP Plasticity
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural Plasticity
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory Systems
replay_interval = 100
consolidation_interval = 1000
```

### High-Performance Configuration
```
# NLM Brain Configuration - High Performance
neuron_count = 2000
region_count = 2
connection_probability = 0.08

simulation_timestep = 0.0005
random_seed = 12345

# Enhanced Plasticity
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.018
stdp_tau = 25.0

synaptogenesis_rate = 0.00015
pruning_rate = 0.000008

# Extended Memory
replay_interval = 200
consolidation_interval = 2000
```

### Research Configuration
```
# NLM Brain Configuration - Research Mode
neuron_count = 100
region_count = 1
connection_probability = 0.20

simulation_timestep = 0.001
random_seed = 999

# Experimental Plasticity
stdp_ltp_weight = 0.015
stdp_ltd_weight = 0.01
stdp_tau = 15.0

synaptogenesis_rate = 0.00005
pruning_rate = 0.00002

# Fast Memory Replay
replay_interval = 50
consolidation_interval = 500
```

## Configuration File Location

The NLM system looks for configuration files in:

1. Current directory: `configs/default.cfg`
2. User-specified path: `./nlm --config /path/to/config.cfg`
3. Command line arguments override file settings

## Command Line Configuration Override

You can override configuration values using command line arguments:

```bash
./nlm --neuron_count=1000 --random_seed=123 --stdp_ltp_weight=0.03
```

Supported formats:
- `--key=value` (quoted values supported)
- `--key value` (unquoted values)

## Validation and Error Handling

The configuration system includes:

1. **File validation**: Checks file existence and readability
2. **Size limits**: Configuration files limited to 1MB
3. **Type validation**: Ensures values can be parsed correctly
4. **Range validation**: Validates parameter ranges for biological plausibility
5. **Error reporting**: Detailed error messages for configuration issues

## Configuration Validation

The system validates the following constraints:

- `neuron_count` must be positive (recommended: 100-10000)
- `region_count` must be positive (recommended: 1-10)
- `connection_probability` must be between 0.0 and 1.0
- `simulation_timestep` must be positive (recommended: 0.0001-0.01)
- STDP weights must be positive
- Synaptogenesis and pruning rates should be very small (0.000001-0.001)

## Advanced Configuration Options

### Development Mode
```
# Development/Testing Configuration
neuron_count = 50
region_count = 1
connection_probability = 0.30

simulation_timestep = 0.01  # Larger timestep for faster testing
random_seed = 42
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.008
stdp_tau = 10.0

# Fast development loops
synaptogenesis_rate = 0.001  # Higher for exploration
pruning_rate = 0.0001

replay_interval = 20
consolidation_interval = 100
```

### Production Mode
```
# Production/Deployment Configuration
neuron_count = 5000
region_count = 3
connection_probability = 0.05

simulation_timestep = 0.0001  # Precise timing
simulation_timestep = 0.0001
random_seed = 2024
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

synaptogenesis_rate = 0.00001  # Stable, low plasticity
pruning_rate = 0.000001

replay_interval = 500
consolidation_interval = 5000
```

## Configuration Management

### Creating New Configuration Files

1. Copy an existing configuration as a template
2. Modify values according to your needs
3. Test with: `./nlm --config your_config.cfg`
4. Save working configurations for future use

### Configuration Best Practices

1. **Version control**: Store configuration files in your project repository
2. **Documentation**: Include comments explaining each parameter's purpose
3. **Testing**: Test configurations with different parameter combinations
4. **Backup**: Keep backup copies of important configurations
5. **Performance tuning**: Experiment with different parameter values for optimal performance

## Configuration Examples Summary

| Configuration | Neurons | Regions | Timestep | Plasticity | Use Case |
|---------------|---------|---------|----------|------------|----------|
| Basic | 500 | 1 | 0.001 | Standard | Simple testing |
| Research | 100 | 1 | 0.001 | Experimental | Scientific studies |
| Production | 5000 | 3 | 0.0001 | Stable | Long-running simulations |
| Performance | 2000 | 2 | 0.0005 | Enhanced | High-performance computing |
| Development | 50 | 1 | 0.01 | Fast | Rapid prototyping |

## Configuration File Tips

1. **Comments**: Use `#` for comments, `//` also supported
2. **Whitespace**: Flexible whitespace around `=` operator
3. **Quoting**: Quote values with spaces or special characters
4. **Scientific notation**: Use `1.5e-3` for very small values
5. **Units**: All time values in seconds, weight values unitless

## Troubleshooting

### Common Configuration Issues

**Problem**: `Failed to open config file`
**Solution**: Check file path and permissions

**Problem**: `Invalid config line: no '=' found`
**Solution**: Ensure proper `key = value` format

**Problem**: `Config file too large`
**Solution**: Reduce file size or split into multiple files

**Problem**: `Type mismatch for config key`
**Solution**: Check that values match expected types (int, double, bool, string)

### Configuration Validation Commands

You can validate your configuration by running:
```bash
# Create a test configuration
touch test_config.cfg

# Run with validation enabled
./nlm --config test_config.cfg --validate-only
```

## Further Reading

For more information about NLM configuration and tuning, refer to:
- `docs/ARCHITECTURE.md` - System architecture
- `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 integration documentation
- `docs/SCIENCE.md` - Scientific background and parameter recommendations
- `README.md` - Building and running instructions

## Configuration File Template

Use this template as a starting point for your own configurations:

```
# NLM Configuration Template
# ===================================
# Replace these values with your configuration
# ===================================

# Basic neural network parameters
neuron_count = 1000
region_count = 1
connection_probability = 0.1

# Timing and simulation
# Time step in seconds (typically 0.001 for millisecond precision)
simulation_timestep = 0.001

# Random seed for reproducible results
random_seed = 42

# STDP (Spike-Timing-Dependent Plasticity) parameters
# LTP: Long-Term Potentiation (learning when pre fires before post)
# LTD: Long-Term Depression (learning when pre fires after post)
stdp_ltp_weight = 0.02      # LTP magnitude (positive)
stdp_ltd_weight = 0.015     # LTD magnitude (positive)
stdp_tau = 20.0             # Time window for STDP in milliseconds

# Structural plasticity (formation/elimination of synapses)
synaptogenesis_rate = 0.0001  # Rate of new synapse formation
pruning_rate = 0.00001        # Rate of synapse elimination

# Memory system parameters
replay_interval = 100         # Steps between memory replay
consolidation_interval = 1000 # Steps between memory consolidation

# Optional: Add your custom configuration here
# example_custom_parameter = 1.5
```

## Configuration Files Location

Configuration files can be placed in:
- `configs/` directory (project default)
- Current working directory
- Any path specified via `--config` option

## Configuration File Permissions

Configuration files should be readable by the user running the simulation. On Unix-like systems:

```bash
chmod 644 configs/my_config.cfg
```

## Configuration File Backups

It's recommended to keep backup copies of working configurations:

```bash
# Create backup
cp configs/default.cfg configs/default.cfg.backup.$(date +%Y%m%d)

# Restore from backup
cp configs/default.cfg.backup.20240101 configs/default.cfg
```

## Configuration File Examples in Repository

You can find additional configuration examples in:
- `examples/` directory (if exists)
- Documentation in `docs/` directory
- Test configuration files in `tests/` directory

## Configuration File Validation Script

Here's a simple script to validate configuration files:

```bash
#!/bin/bash
# validate_config.sh - Validate NLM configuration file

CONFIG_FILE="$1"

if [ -z "$CONFIG_FILE" ]; then
    echo "Usage: $0 <config_file>"
    exit 1
fi

if [ ! -f "$CONFIG_FILE" ]; then
    echo "Error: Config file '$CONFIG_FILE' not found"
    exit 1
fi

echo "Validating configuration file: $CONFIG_FILE"
echo "File size: $(wc -l < "$CONFIG_FILE") lines"

# Check for basic format
echo "Checking format..."
grep -q "^[[:space:]]*#" "$CONFIG_FILE" && echo "✓ Comments found" || echo "⚠ No comments found"
grep -q "^[[:space:]]*[^[:space:]=#;/]+[[:space:]]*=" "$CONFIG_FILE" && echo "✓ Key-value pairs found" || echo "✗ Invalid format"

# Check critical parameters
if grep -q "neuron_count" "$CONFIG_FILE"; then
    echo "✓ neuron_count parameter found"
else
    echo "⚠ neuron_count parameter not found (using default)"
fi

echo "Validation complete"
```

This configuration system provides a flexible and robust way to configure NLM brain simulations for different use cases, from simple testing to production deployments. The validation and error handling ensure that configuration issues are caught early, preventing runtime problems.
