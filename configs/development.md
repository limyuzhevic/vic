# development.cfg
# =========================

## Configuration Description

Development and Learning Configuration
Optimized for learning and development processes in NLM simulations.

### Purpose

This configuration template is designed for research and development scenarios where understanding learning processes, developmental trajectories, and neural plasticity is the primary goal. It provides enhanced development features and extended simulation times.

### Key Characteristics

- **Moderate brain size (1500 neurons)**: Balanced computational capacity
- **Enhanced development settings**: Extended critical periods and plasticity
- **Extended simulation time (5000 steps)**: Sufficient time for developmental processes
- **Debug logging**: Detailed logging for development analysis
- **Learning-focused parameters**: Aggressive plasticity for learning studies

### Use Cases

- Developmental research
- Learning process studies
- Critical period experiments
- Long-term behavioral development

### Configuration Parameters

| Parameter | Value | Description |
|-----------|-------|-------------|
| `brain.neuron_count` | 1500 | Number of neurons in the brain |
| `brain.region_count` | 3 | Number of brain regions |
| `brain.synapse_density` | 0.12 | Connection probability between neurons |
| `plasticity.stdp.learning_rate` | 0.008 | STDP learning rate |
| `plasticity.stdp.ltp_weight` | 0.01 | LTP weight |
| `plasticity.stdp.ltd_weight` | 0.011 | LTD weight |
| `plasticity.stdp.time_constant` | 20.0 | STDP time constant |
| `development.initial_plasticity` | 1.5 | Initial plasticity level |
| `development.synaptogenesis_rate` | 0.002 | Synaptogenesis rate |
| `development.pruning_rate` | 0.0002 | Pruning rate |
| `development.critical_period_start` | 50 | Critical period start |
| `development.critical_period_end` | 400 | Critical period end |
| `environment.width` | 15 | Environment width |
| `environment.height` | 15 | Environment height |
| `environment.visionWidth` | 10 | Agent vision width |
| `environment.visionHeight` | 10 | Agent vision height |
| `simulation.max_steps` | 5000 | Maximum simulation steps |
| `logging.level` | DEBUG | Log level |
| `logging.to_file` | true | Log to file |
| `logging.filename` | development_nlm.log | Log file name |

### Development Features

- **Extended critical period**: 50-400 steps for studying critical period effects
- **High initial plasticity**: 1.5 for aggressive initial learning
- **Enhanced synaptogenesis**: 0.002 for rapid synapse formation
- **Moderate pruning**: 0.0002 for controlled synapse elimination
- **Debug logging**: Detailed information for development analysis

### Performance Expectations

- **Approximate memory usage**: 300-500 MB
- **Estimated simulation speed**: 200-400 steps/second
- **Expected firing rate**: 80-180 Hz
- **Scalability**: Suitable for medium-scale research systems

### Recommendations

1. **Use when**: Studying developmental processes, learning trajectories
2. **Adjust when**: Shortening development timeline or reducing memory usage
3. **Troubleshooting**: If simulation takes too long, consider reducing max_steps
4. **Research focus**: Ideal for experiments on learning, memory, and development

### Validation

This configuration has been validated with:
- Development and learning simulation (example3)
- Long-term behavioral observation
- Critical period effect studies
- Learning curve analysis

### Example Usage

```bash
# Create brain with development configuration
./nlmc create-brain 1500 --config-file configs/development.cfg

# Run extended developmental simulation
./nlmc run-sim configs/development.cfg --steps 3000

# Study learning processes
python examples/example3_development_learning.py
```

### Migration Notes

- This configuration is compatible with all NLM versions
- No breaking changes from earlier versions
- Compatible with all example scripts

### Development Tips

1. **For studying critical periods**: Keep critical period parameters as-is
2. **For shorter experiments**: Reduce max_steps and adjust development parameters
3. **For memory-constrained environments**: Use `lightweight.cfg` with similar development settings
4. **For detailed analysis**: Enable visualization and increase logging level

### Configuration Validation

```python
import pynlm

config = pynlm.createDefaultConfig()
config.load("configs/development.cfg")

# Validate development-specific parameters
if config.get("development.initial_plasticity", 0) > 2.0:
    print("Warning: High initial plasticity may affect stability")

if config.get("development.critical_period_end", 0) < config.get("development.critical_period_start", 0):
    print("Error: Critical period end must be after start")

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
