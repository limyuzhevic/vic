# default.cfg
# =================

## Configuration Description

Default Configuration
Balanced configuration for general-purpose NLM simulations and testing.

### Purpose

This configuration template serves as the baseline configuration for NLM simulations. It provides a balanced set of parameters that work well for most use cases while maintaining good performance and stability. It is designed to be a reliable starting point for users new to NLM.

### Key Characteristics

- **Standard brain size (1000 neurons)**: Balanced computational capacity
- **Standard connectivity (0.1)**: Balanced connectivity for general use
- **Standard plasticity**: Moderate learning rate for balanced learning
- **Standard environment**: Standard world dimensions for compatibility
- **Standard logging**: Basic logging for system monitoring
- **Reference configuration**: Used as baseline for other templates

### Use Cases

- General-purpose simulations
- Standard testing and validation
- Learning NLM basics
- System initialization and setup
- Development and debugging

### Configuration Parameters

| Parameter | Value | Description |
|-----------|-------|-------------|
| `random_seed` | 42 | Random seed for reproducible experiments |
| `simulation_timestep` | 0.001 | Simulation timestep in seconds |
| `brain.neuron_count` | 1000 | Number of neurons in the brain |
| `brain.region_count` | 2 | Number of brain regions |
| `brain.synapse_density` | 0.1 | Connection probability between neurons |
| `plasticity.stdp.learning_rate` | 0.01 | STDP learning rate |
| `plasticity.stdp.ltp_weight` | 0.01 | LTP weight |
| `plasticity.stdp.ltd_weight` | 0.012 | LTD weight |
| `plasticity.stdp.time_constant` | 20.0 | STDP time constant |
| `development.initial_plasticity` | 1.0 | Initial plasticity level |
| `development.synaptogenesis_rate` | 0.001 | Synaptogenesis rate |
| `development.pruning_rate` | 0.0001 | Pruning rate |
| `development.critical_period_start` | 100 | Critical period start |
| `development.critical_period_end` | 500 | Critical period end |
| `environment.name` | GridWorld | Name of environment |
| `environment.width` | 10 | Environment width |
| `environment.height` | 10 | Environment height |
| `environment.visionWidth` | 8 | Agent vision width |
| `environment.visionHeight` | 8 | Agent vision height |
| `log_level` | INFO | Log level |
| `log_to_file` | false | Log to file |
| `log_filename` | nlm.log | Log file name |
| `max_simulation_steps` | 10000 | Maximum simulation steps |
| `simulation_time_limit` | 0.0 | Simulation time limit (0 = unlimited) |
| `visualization_enabled` | false | Enable visualization |
| `visualization_update_rate` | 30.0 | Visualization update rate |

### General Characteristics

- **Balanced performance**: Good performance across most use cases
- **Standard parameters**: Widely compatible parameter set
- **Reference configuration**: Used as baseline for other templates
- **Well-tested**: Extensively tested with all examples
- **Stable behavior**: Predictable and reliable simulation behavior

### Performance Expectations

- **Approximate memory usage**: 200-300 MB
- **Estimated simulation speed**: 500-800 steps/second
- **Expected firing rate**: 100-150 Hz
- **Scalability**: Suitable for most testing environments

### Recommendations

1. **Use when**: General-purpose testing, learning NLM, initial setup
2. **Use as baseline**: Compare other configurations against this one
3. **For troubleshooting**: Use this configuration for debugging
4. **For development**: Start with this configuration and customize as needed

### Validation

This configuration has been validated with:
- All example scripts
- Basic brain creation and initialization
- Agent simulation and world interaction
- Development and learning processes
- Configuration comparisons

### Example Usage

```bash
# Create brain with default configuration
./nlmc create-brain 1000 --config-file configs/default.cfg

# Run standard simulation
./nlmc run-sim configs/default.cfg --steps 1000

# Run all examples
python examples/example1_basic_brain.py
python examples/example2_agent_simulation.py
python examples/example3_development_learning.py
python examples/example4_experiment_management.py
python examples/example5_benchmarking.py
```

### Migration Notes

- This configuration is compatible with all NLM versions
- No breaking changes from earlier versions
- Compatible with all example scripts
- Used as baseline for other configuration templates

### Configuration Tips

1. **For beginners**: Use this configuration for learning NLM basics
2. **For standard testing**: Use this configuration for routine testing
3. **For configuration comparison**: Use this as baseline for comparison
4. **For troubleshooting**: Use this configuration for debugging

### Configuration Validation

```python
import pynlm

config = pynlm.createDefaultConfig()

# Validate default configuration
print("Default configuration validation:")
print(f"  Neuron count: {config.get('brain.neuron_count', 'N/A')}")
print(f"  Environment: {config.get('environment.name', 'N/A')} {config.get('environment.width', 'N/A')}x{config.get('environment.height', 'N/A')}")
print(f"  Plasticity learning rate: {config.get('plasticity.stdp.learning_rate', 'N/A')}")
print(f"  Development initial plasticity: {config.get('development.initial_plasticity', 'N/A')}")

# Validate configuration
if config.validate():
    print("\nConfiguration is valid - ready for use!")
else:
    print("\nConfiguration validation errors:")
    for error in config.get_validation_errors():
        print(f"  - {error}")
```

---

*This configuration template is part of the NLM (Neural Learning Machine) project.*
