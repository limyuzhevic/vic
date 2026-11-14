# NLM Configuration Documentation

## Overview
This document describes all configuration parameters available in the NLM (Neural Learning Machine) project.

## Configuration File Formats

### Simple Format (Phase 1)
The basic configuration format used in `configs/default.cfg`:
```
key = value
```

### JSON Format
JSON configuration files (`.json` extension) support full JSON syntax:
```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "plasticity_learning_rate": 0.01,
    "development_synaptogenesis_rate": 0.001,
    "dopamine_baseline": 0.0,
    "reward_discount_factor": 0.99,
    "environment_name": "GridWorld",
    "log_level": "INFO",
    "max_simulation_steps": 10000,
    "visualization_enabled": false
}
```

### YAML Format
YAML configuration files (`.yaml` or `.yml` extension) use YAML syntax:
```yaml
random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
region_count: 2

plasticity:
    learning_rate: 0.01
    stdp_ltp_weight: 0.01
    stdp_ltd_weight: 0.012
    stdp_time_constant: 20.0

development:
    synaptogenesis_rate: 0.001
    pruning_rate: 0.0001

neuromodulation:
    dopamine_baseline: 0.0
    reward_discount_factor: 0.99

environment:
    name: GridWorld
    width: 10
    height: 10

logging:
    level: INFO
    to_file: false
    filename: nlm.log

simulation:
    max_steps: 10000
    time_limit: 0.0

visualization:
    enabled: false
    update_rate: 30.0
```

## Configuration Parameters

### Random and Simulation

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `random_seed` | int | 42 | Random seed for reproducible experiments |
| `simulation_timestep` | double | 0.001 | Simulation timestep in seconds |
| `max_simulation_steps` | int | 10000 | Maximum number of simulation steps |
| `simulation_time_limit` | double | 0.0 | Time limit in seconds (0 = unlimited) |

### Neural Architecture

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 1000 | Number of neurons in the brain |
| `region_count` | int | 2 | Number of brain regions |
| `connection_probability` | double | 0.1 | Connection probability for random connectivity |

### Plasticity

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity_learning_rate` | double | 0.01 | Global learning rate for plasticity |
| `stdp_ltp_weight` | double | 0.01 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | double | 0.012 | STDP long-term depression weight |
| `stdp_time_constant` | double | 20.0 | STDP time constant in milliseconds |

### Development

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development_synaptogenesis_rate` | double | 0.001 | Rate of synapse creation during development |
| `development_pruning_rate` | double | 0.0001 | Rate of synapse removal during development |

### Neuromodulation

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `dopamine_baseline` | double | 0.0 | Baseline dopamine level |
| `reward_discount_factor` | double | 0.99 | Discount factor for reward prediction |

### Environment

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `environment_name` | string | "GridWorld" | Name of the environment to simulate |
| `environment_width` | int | 10 | Width of the environment |
| `environment_height` | int | 10 | Height of the environment |

### Logging

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `log_level` | string | "INFO" | Logging level (DEBUG, INFO, WARNING, ERROR) |
| `log_to_file` | bool | false | Whether to log to file |
| `log_filename` | string | "nlm.log" | Log file name |

### Visualization

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `visualization_enabled` | bool | false | Whether visualization is enabled |
| `visualization_update_rate` | double | 30.0 | Visualization update rate in Hz |

## Command Line Arguments

Configuration can also be set via command line arguments:

### Short Form
```bash
./nlm --random_seed=123 --neuron_count=500 --log_level=DEBUG
```

### Long Form
```bash
./nlm --simulation_timestep=0.0005 --plasticity_learning_rate=0.02
```

### Combined
```bash
./nlm -seed 999 -steps 5000 -viz
```

## Configuration Sources (Precedence Order)

1. **Command Line Arguments** (highest priority)
2. **Configuration File** (`.cfg`, `.json`, `.yaml`)
3. **Default Values** (lowest priority)
4. **Runtime Overrides** (can be set programmatically)

## Configuration Validation

The configuration system validates parameter types and ranges:

- **Integer parameters**: Must be valid 32-bit integers
- **Float parameters**: Must be valid floating-point numbers
- **String parameters**: Must be valid UTF-8 strings
- **Boolean parameters**: Must be `true`/`false` (case-insensitive)
- **Vector parameters**: Must be valid JSON/YAML arrays

## Performance Considerations

- **Configuration Loading**: JSON and YAML files are parsed using standard libraries
- **Memory Usage**: Configuration values are stored in memory for fast access
- **File Access**: Configuration files are read at startup and cached
- **Thread Safety**: Configuration modifications are thread-safe

## Error Handling

The configuration system provides comprehensive error handling:

- **File Not Found**: Returns false when configuration file cannot be opened
- **Parse Errors**: Returns false with detailed error messages for malformed files
- **Type Errors**: Attempts automatic type conversion with fallbacks
- **Value Errors**: Validates parameter ranges and logs warnings for out-of-range values

## Best Practices

1. **Configuration Files**: Store configuration in environment-specific files
2. **Secret Management**: Never store sensitive data (API keys, passwords) in configuration files
3. **Version Control**: Do not commit sensitive configuration files to version control
4. **Documentation**: Document all configuration parameters and their effects
5. **Testing**: Test configuration loading with various file formats
6. **Validation**: Validate configuration before starting simulation
7. **Logging**: Enable debug logging when troubleshooting configuration issues

## Configuration Examples

### Minimal Configuration
```cfg
random_seed = 42
neuron_count = 1000
max_simulation_steps = 1000
```

### Full Configuration
```yaml
random_seed: 12345
simulation_timestep: 0.0001
neuron_count: 500
region_count: 4
plasticity:
    learning_rate: 0.02
    stdp_ltp_weight: 0.02
    stdp_ltd_weight: 0.024
    stdp_time_constant: 25.0
development:
    synaptogenesis_rate: 0.002
    pruning_rate: 0.0002
neuromodulation:
    dopamine_baseline: 0.1
    reward_discount_factor: 0.95
environment:
    name: "ComplexWorld"
    width: 20
    height: 20
logging:
    level: "DEBUG"
    to_file: true
    filename: "simulation.log"
simulation:
    max_steps: 50000
    time_limit: 1000.0
visualization:
    enabled: true
    update_rate: 60.0
```

## Related Documentation

- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture details
- [SCIENCE.md](SCIENCE.md) - Project background and scientific rationale
- [PHASE6_FINAL_AUDIT.md](PHASE6_FINAL_AUDIT.md) - Phase 6 implementation audit