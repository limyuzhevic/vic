# experimental.cfg
# ====================

## Configuration Description

Experimental Configuration
Designed for testing specific hypotheses and extreme parameter conditions.

### Purpose

This configuration template is optimized for experimental scenarios where testing specific hypotheses, extreme parameter values, or novel conditions is the primary goal. It provides enhanced logging, extended simulation times, and challenging environmental conditions.

### Key Characteristics

- **Medium brain size (1000 neurons)**: Balanced computational capacity
- **High connectivity (0.25)**: Maximum connectivity for exploration
- **Aggressive plasticity**: Extreme parameter values for experimental conditions
- **Extended simulation time (3000 steps)**: Sufficient time for complex experiments
- **Detailed logging**: Comprehensive logging for experimental analysis
- **Enhanced visualization**: Detailed observation capabilities

### Use Cases

- Scientific hypothesis testing
- Parameter extreme value experiments
- Novelty and curiosity studies
- Environmental challenge experiments
- Detailed behavioral observation

### Configuration Parameters

| Parameter | Value | Description |
|-----------|-------|-------------|
| `brain.neuron_count` | 1000 | Number of neurons in the brain |
| `brain.region_count` | 2 | Number of brain regions |
| `brain.synapse_density` | 0.25 | Connection probability between neurons |
| `plasticity.stdp.learning_rate` | 0.05 | STDP learning rate |
| `plasticity.stdp.ltp_weight` | 0.1 | LTP weight |
| `plasticity.stdp.ltd_weight` | 0.12 | LTD weight |
| `plasticity.stdp.time_constant` | 15.0 | STDP time constant |
| `development.initial_plasticity` | 2.0 | Initial plasticity level |
| `development.synaptogenesis_rate` | 0.01 | Synaptogenesis rate |
| `development.pruning_rate` | 0.001 | Pruning rate |
| `environment.width` | 25 | Environment width |
| `environment.height` | 25 | Environment height |
| `environment.visionWidth` | 12 | Agent vision width |
| `environment.visionHeight` | 12 | Agent vision height |
| `simulation.max_steps` | 3000 | Maximum simulation steps |
| `logging.level` | INFO | Log level |
| `logging.to_file` | true | Log to file |
| `logging.filename` | experimental_nlm.log | Log file name |
| `visualization.enabled` | true | Enable visualization |
| `visualization.update_rate` | 30.0 | Visualization update rate |

### Experimental Features

- **High connectivity**: Maximum exploration potential
- **Extreme plasticity**: Aggressive learning for hypothesis testing
- **Extended critical periods**: Enhanced developmental effects
- **Challenging environment**: Large world for exploration
- **Detailed logging**: Comprehensive data collection
- **Visualization enabled**: Real-time observation capabilities

### Performance Expectations

- **Approximate memory usage**: 400-600 MB
- **Estimated simulation speed**: 150-300 steps/second
- **Expected firing rate**: 60-150 Hz
- **Scalability**: Suitable for dedicated research systems

### Recommendations

1. **Use when**: Testing specific hypotheses, parameter extremes, or novel conditions
2. **Adjust when**: Reducing memory usage or shortening experiment duration
3. **Troubleshooting**: Monitor memory usage closely with high connectivity
4. **Research focus**: Ideal for experimental validation and hypothesis testing

### Validation

This configuration has been validated with:
- Experiment design simulation (example4)
- Parameter sweep experiments
- Extreme value testing
- Detailed data collection and analysis

### Example Usage

```bash
# Create brain with experimental configuration
./nlmc create-brain 1000 --config-file configs/experimental.cfg

# Run extended experimental simulation
./nlmc run-sim configs/experimental.cfg --steps 2000

# Run experiment management example
python examples/example4_experiment_management.py --name experimental_test
```

### Migration Notes

- This configuration is compatible with all NLM versions
- No breaking changes from earlier versions
- Compatible with all example scripts

### Experimental Tips

1. **For extreme experiments**: Keep all parameters at extreme values
2. **For memory-constrained environments**: Disable visualization and reduce connectivity
3. **For hypothesis testing**: Use this configuration as baseline
4. **For data collection**: Enable all logging features

### Configuration Validation

```python
import pynlm

config = pynlm.createDefaultConfig()
config.load("configs/experimental.cfg")

# Validate experimental settings
if config.get("brain.synapse_density", 0) > 0.2:
    print("Warning: High connectivity may lead to high memory usage")

if config.get("plasticity.stdp.learning_rate", 0) > 0.02:
    print("Warning: High learning rate may cause instability")

if config.get("simulation.max_steps", 0) < 1000:
    print("Warning: Low simulation steps may limit experimental conclusions")

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
