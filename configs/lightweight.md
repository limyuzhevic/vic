# lightweight.cfg
# ===========================

## Configuration Description

Lightweight Configuration
Optimized for minimal resource usage and quick testing.

### Purpose

This configuration template is designed for resource-constrained environments, quick prototyping, and testing scenarios where minimal computational requirements are prioritized. It provides the most efficient resource usage while maintaining core NLM functionality.

### Key Characteristics

- **Small brain (300 neurons)**: Minimal computational requirements
- **Minimal connectivity (0.05)**: Low memory footprint
- **Conservative plasticity**: Conservative learning for stability
- **Quick simulation (500 steps)**: Fast execution for rapid iteration
- **Minimal logging**: Limited logging to reduce overhead
- **Disabled visualization**: Disabled for performance

### Use Cases

- Quick prototyping and testing
- Resource-constrained environments
- Memory-limited systems
- Initial development and debugging
- Educational demonstrations

### Configuration Parameters

| Parameter | Value | Description |
|-----------|-------|-------------|
| `brain.neuron_count` | 300 | Number of neurons in the brain |
| `brain.region_count` | 1 | Number of brain regions |
| `brain.synapse_density` | 0.05 | Connection probability between neurons |
| `plasticity.stdp.learning_rate` | 0.02 | STDP learning rate |
| `plasticity.stdp.ltp_weight` | 0.015 | LTP weight |
| `plasticity.stdp.ltd_weight` | 0.018 | LTD weight |
| `plasticity.stdp.time_constant` | 20.0 | STDP time constant |
| `development.initial_plasticity` | 0.5 | Initial plasticity level |
| `development.synaptogenesis_rate` | 0.0005 | Synaptogenesis rate |
| `development.pruning_rate` | 0.00005 | Pruning rate |
| `environment.width` | 8 | Environment width |
| `environment.height` | 8 | Environment height |
| `environment.visionWidth` | 6 | Agent vision width |
| `environment.visionHeight` | 6 | Agent vision height |
| `simulation.max_steps` | 500 | Maximum simulation steps |
| `logging.level` | WARNING | Log level |
| `logging.to_file` | false | Log to file |
| `logging.filename` | lightweight_nlm.log | Log file name |

### Resource Characteristics

- **Memory usage**: ~50-100 MB
- **Processing requirements**: Low computational demand
- **Execution speed**: Fast simulation
- **Scalability**: Suitable for embedded systems and mobile devices

### Performance Expectations

- **Approximate memory usage**: 50-100 MB
- **Estimated simulation speed**: 300-500 steps/second
- **Expected firing rate**: 40-80 Hz
- **Scalability**: Suitable for resource-constrained systems

### Recommendations

1. **Use when**: Quick testing, prototyping, or resource-constrained environments
2. **Adjust when**: Need more computational power, increase neuron count
3. **Troubleshooting**: If too slow, consider increasing simulation timestep
4. **Development**: Use this as baseline before scaling to larger configurations

### Validation

This configuration has been validated with:
- Basic brain creation and initialization
- Quick functionality testing
- Memory efficiency analysis
- System compatibility testing

### Example Usage

```bash
# Create brain with lightweight configuration
./nlmc create-brain 300 --config-file configs/lightweight.cfg

# Run quick simulation
./nlmc run-sim configs/lightweight.cfg --steps 200

# Test with example 1
python examples/example1_basic_brain.py
```

### Migration Notes

- This configuration is compatible with all NLM versions
- No breaking changes from earlier versions
- Compatible with all example scripts

### Lightweight Tips

1. **For fastest performance**: Keep all parameters at default values
2. **For memory-constrained systems**: Disable logging and visualization
3. **For quick testing**: Use this configuration for rapid iteration
4. **For development**: Start with this configuration, scale up as needed

### Configuration Validation

```python
import pynlm

config = pynlm.createDefaultConfig()
config.load("configs/lightweight.cfg")

# Validate lightweight settings
if config.get("brain.neuron_count", 0) > 500:
    print("Warning: Neuron count may be too high for lightweight configuration")

if config.get("simulation.max_steps", 0) > 1000:
    print("Warning: Simulation steps may be too high for lightweight configuration")

# Validate configuration
if config.validate():
    print("Configuration is valid")
else:
    print("Configuration validation errors:")
    for error in config.get_validation_errors():
        print(f"  - {error}")
```

---

*This configuration template is part of the NLM (Neural Learning Machine) project.*
