# benchmark.cfg
# ====================

## Configuration Description

Benchmark Configuration
Designed for performance testing and comparison studies.

### Purpose

This configuration template is optimized for performance benchmarking and comparative analysis. It provides standardized parameters for fair comparison across different system configurations and testing scenarios.

### Key Characteristics

- **Standard brain size (1000 neurons)**: Consistent computational base
- **Baseline connectivity**: Standard connectivity for fair comparison
- **Baseline plasticity**: Standard learning rate for baseline measurement
- **Standard environment**: Consistent world dimensions for comparison
- **Performance logging**: Comprehensive logging for performance analysis
- **Performance-focused**: Optimized for benchmark testing

### Use Cases

- Performance benchmarking
- Comparative testing
- System optimization studies
- Performance regression testing
- Standardized evaluation protocols

### Configuration Parameters

| Parameter | Value | Description |
|-----------|-------|-------------|
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
| `environment.width` | 10 | Environment width |
| `environment.height` | 10 | Environment height |
| `environment.visionWidth` | 8 | Agent vision width |
| `environment.visionHeight` | 8 | Agent vision height |
| `simulation.max_steps` | 1000 | Maximum simulation steps |
| `logging.level` | INFO | Log level |
| `logging.to_file` | true | Log file name |
| `logging.filename` | benchmark_nlm.log | Log file name |

### Performance Characteristics

- **Memory usage**: ~200-300 MB
- **Processing requirements**: Standard computational demand
- **Execution speed**: Consistent performance for comparison
- **Scalability**: Suitable for standard testing environments

### Performance Expectations

- **Approximate memory usage**: 200-300 MB
- **Estimated simulation speed**: 500-800 steps/second
- **Expected firing rate**: 100-150 Hz
- **Scalability**: Suitable for standard testing environments

### Recommendations

1. **Use when**: Performance benchmarking, comparative testing, system evaluation
2. **Adjust when**: Need different baseline configurations
3. **Troubleshooting**: Use for identifying performance regressions
4. **Research focus**: Ideal for establishing performance baselines

### Validation

This configuration has been validated with:
- Performance benchmarking (example5)
- System optimization studies
- Comparative testing protocols
- Performance regression analysis

### Example Usage

```bash
# Create brain with benchmark configuration
./nlmc create-brain 1000 --config-file configs/benchmark.cfg

# Run benchmark simulation
./nlmc run-sim configs/benchmark.cfg --steps 500

# Run performance analysis
python examples/example5_benchmarking.py
```

### Migration Notes

- This configuration is compatible with all NLM versions
- No breaking changes from earlier versions
- Compatible with all example scripts

### Benchmark Tips

1. **For fair comparisons**: Use this configuration as baseline
2. **For performance analysis**: Enable comprehensive logging
3. **For optimization studies**: Use this configuration as reference
4. **For system evaluation**: Use this configuration for consistent testing

### Configuration Validation

```python
import pynlm

config = pynlm.createDefaultConfig()
config.load("configs/benchmark.cfg")

# Validate benchmark settings
if config.get("brain.neuron_count", 0) != 1000:
    print("Warning: Neuron count differs from benchmark standard")

if config.get("simulation.max_steps", 0) > 2000:
    print("Warning: Simulation steps may be too high for standard benchmarking")

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
