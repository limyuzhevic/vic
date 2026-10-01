# high_perf.cfg
# ====================

## Configuration Description

High Performance Configuration
Optimized for speed and efficiency in NLM simulations.

### Purpose

This configuration template is designed for production environments, benchmarking, and large-scale simulations where performance is the primary concern. It provides a balance between computational power and efficiency.

### Key Characteristics

- **Large brain (2000 neurons)**: High computational capacity for complex simulations
- **High connectivity (0.15)**: Increased connectivity for better information processing
- **Fast plasticity**: Moderate learning rate for balanced learning speed
- **Optimized environment**: Larger world for agent exploration
- **Performance logging**: Detailed logging for performance analysis

### Use Cases

- Production NLM systems
- Performance benchmarking
- Large-scale simulation experiments
- Research requiring high computational power

### Configuration Parameters

| Parameter | Value | Description |
|-----------|-------|-------------|
| `brain.neuron_count` | 2000 | Number of neurons in the brain |
| `brain.region_count` | 4 | Number of brain regions |
| `brain.synapse_density` | 0.15 | Connection probability between neurons |
| `plasticity.stdp.learning_rate` | 0.005 | STDP learning rate |
| `plasticity.stdp.ltp_weight` | 0.008 | LTP weight |
| `plasticity.stdp.ltd_weight` | 0.009 | LTD weight |
| `plasticity.stdp.time_constant` | 20.0 | STDP time constant |
| `development.initial_plasticity` | 0.8 | Initial plasticity level |
| `development.synaptogenesis_rate` | 0.0005 | Synaptogenesis rate |
| `development.pruning_rate` | 0.00005 | Pruning rate |
| `environment.width` | 20 | Environment width |
| `environment.height` | 20 | Environment height |
| `environment.visionWidth` | 8 | Agent vision width |
| `environment.visionHeight` | 8 | Agent vision height |
| `simulation.max_steps` | 2000 | Maximum simulation steps |
| `logging.level` | INFO | Log level |
| `logging.to_file` | true | Log to file |
| `logging.filename` | high_perf_nlm.log | Log file name |

### Performance Expectations

- **Approximate memory usage**: 400-600 MB
- **Estimated simulation speed**: 500-800 steps/second
- **Expected firing rate**: 100-200 Hz
- **Scalability**: Suitable for multi-core systems

### Recommendations

1. **Use when**: Running production simulations or benchmarking
2. **Adjust when**: Memory constraints or specific performance requirements
3. **Troubleshooting**: If performance is slow, consider increasing simulation timestep

### Validation

This configuration has been validated with:
- Basic brain creation and initialization
- Agent simulation with world interaction
- Development and learning processes
- Performance benchmarking

### Example Usage

```bash
# Create brain with high_perf configuration
./nlmc create-brain 2000 --config-file configs/high_perf.cfg

# Run simulation
./nlmc run-sim configs/high_perf.cfg --steps 1000

# Benchmark performance
./nlmc benchmark configs/high_perf.cfg --iterations 5
```

### Migration Notes

- This configuration is compatible with all NLM versions
- No breaking changes from earlier versions
- Compatible with all example scripts

### Performance Tips

1. **For best performance**: Use on systems with multiple CPU cores
2. **For memory-constrained environments**: Consider `development.cfg` or `lightweight.cfg`
3. **For longest simulations**: Monitor memory usage and adjust configuration
4. **For quick testing**: Use `lightweight.cfg` with similar parameters

### Configuration Validation

```python
import pynlm

config = pynlm.createDefaultConfig()
config.load("configs/high_perf.cfg")

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
