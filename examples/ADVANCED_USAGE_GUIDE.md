# Advanced NLM Usage Guide

This guide provides advanced usage examples and power user features for the NLM (Neural Learning Machine) framework.

## Overview

The NLM framework has been significantly improved with better configuration management, enhanced Python bindings, and advanced features for power users. This guide demonstrates how to use these new capabilities.

## Key Improvements

### 1. Enhanced Configuration System
- **Nested Configuration Support**: Configure brain, plasticity, neuromodulation, and development settings using dot notation
- **Multiple File Formats**: Support for JSON, YAML, and legacy key=value configuration files
- **Command Line Integration**: Full command-line argument support with type conversion
- **Configuration Validation**: Better error handling and validation

### 2. Improved Python API
- **Factory Functions**: Simple configuration creation with `createDefaultConfig()`, `createBrain()`, etc.
- **Enhanced Methods**: New methods for nested configuration access, batch operations, and analysis
- **Better Error Handling**: More informative error messages and exception handling

### 3. Advanced Features
- **Configuration Management**: Save/load configurations, compare settings
- **Batch Processing**: Run multiple simulations with parameter variations
- **Performance Benchmarking**: Measure and compare performance across different configurations
- **Advanced Analysis**: Compute statistics, detect patterns, and analyze simulation results

## Quick Start Examples

### 1. Advanced Configuration

Create a complex configuration with nested settings:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Configure brain components
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 3)
config.set("brain.synapse_density", 0.15)

# Configure plasticity systems
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)

# Configure neuromodulation
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Save to JSON file
config.saveToFile("advanced_config.json")
```

### 2. Batch Simulation

Run multiple simulations with different parameters:

```python
def run_batch_simulation():
    parameters = [
        {"seed": 42, "neurons": 1000, "steps": 5000},
        {"seed": 123, "neurons": 2000, "steps": 10000},
        {"seed": 456, "neurons": 3000, "steps": 15000},
    ]
    
    results = []
    
    for params in parameters:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", params["neurons"])
        config.set("random_seed", params["seed"])
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(params["steps"]):
            brain.step(step)
        
        result = {
            "seed": params["seed"],
            "neurons": params["neurons"],
            "final_spikes": brain.getTotalSpikeCount(),
            "duration": "record timing"
        }
        
        results.append(result)
    
    return results
```

### 3. Advanced Analysis

Analyze simulation results in detail:

```python
def analyze_simulation(brain, num_steps):
    spike_history = []
    firing_rate_history = []
    
    for step in range(num_steps):
        brain.step(step)
        
        if step % 100 == 0:
            spike_history.append(brain.getTotalSpikeCount())
            firing_rate_history.append(brain.getFiringNeuronCount())
    
    # Calculate statistics
    avg_spikes = sum(spike_history) / len(spike_history) if spike_history else 0
    max_spikes = max(spike_history) if spike_history else 0
    spike_std = (sum((x - avg_spikes) ** 2 for x in spike_history) / len(spike_history)) ** 0.5 if spike_history else 0
    
    analysis = {
        "average_spikes": avg_spikes,
        "max_spikes": max_spikes,
        "spike_std": spike_std,
        "simulation_duration": num_steps
    }
    
    return analysis
```

### 4. Configuration Benchmarking

Compare performance across different configurations:

```python
def benchmark_configurations():
    configs = [
        {"name": "Small", "neurons": 500, "steps": 1000},
        {"name": "Medium", "neurons": 2000, "steps": 5000},
        {"name": "Large", "neurons": 5000, "steps": 10000},
    ]
    
    results = []
    
    for config_def in configs:
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", config_def["neurons"])
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time the simulation
        import time
        start_time = time.time()
        
        for step in range(config_def["steps"]):
            brain.step(step)
        
        end_time = time.time()
        
        result = {
            "config_name": config_def["name"],
            "steps_per_second": config_def["steps"] / (end_time - start_time),
            "spikes_per_second": brain.getTotalSpikeCount() / (end_time - start_time)
        }
        
        results.append(result)
    
    return results
```

## Command Line Interface

The NLM framework now has an enhanced command-line interface:

```bash
# Show help
./nlm --help

# Create a brain with specific settings
./nlm --neuron-count=2000 --random-seed=12345 --stdp-enable --reward-enable

# Load configuration from file
./nlm --config=my_config.json --timestep=0.005

# Show version
./nlm --version
```

## File Format Support

The configuration system supports multiple file formats:

### JSON Format
```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    }
}
```

### YAML Format
```yaml
brain:
  neuron_count: 1000
  region_count: 1
plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
```

### Legacy Key=Value Format
```
# configs/default.cfg
brain.neuron_count = 1000
brain.region_count = 1
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
```

## Advanced Usage Examples

The `examples/advanced_usage.py` file contains comprehensive examples demonstrating all the advanced features, including batch processing, benchmarking, and detailed analysis.

## Configuration Key Reference

### Brain Configuration
- `brain.neuron_count`: Number of neurons
- `brain.region_count`: Number of brain regions
- `brain.synapse_density`: Synaptic connection density
- `brain.v_thresh`: Neuron firing threshold
- `brain.v_rest`: Resting potential
- `brain.v_reset`: Reset potential after spike

### Plasticity Configuration
- `plasticity.stdp.enable`: Enable spike-timing-dependent plasticity
- `plasticity.stdp.learning_rate`: STDP learning rate
- `plasticity.stdp.tau_plus`: STDP potentiation time constant
- `plasticity.stdp.tau_minus`: STDP depression time constant
- `plasticity.hebbian.enable`: Enable Hebbian learning
- `plasticity.structural.enable`: Enable structural plasticity

### Neuromodulation Configuration
- `neuromod.curiosity.enable`: Enable curiosity-driven exploration
- `neuromod.novelty.enable`: Enable novelty detection
- `neuromod.dopamine.scale`: Dopamine modulation scale

### Development Configuration
- `development.initial_stage`: Initial developmental stage
- `development.critical_period_duration`: Critical period duration
- `development.maturation_rate`: Maturation rate

## Performance Tips

1. **Use Nested Keys**: For complex configurations, use nested keys (e.g., `brain.neuron_count`) for better organization.

2. **Batch Processing**: For large parameter sweeps, use the batch processing examples to run multiple simulations efficiently.

3. **Configuration Reuse**: Save successful configurations to JSON files for reuse across experiments.

4. **Performance Monitoring**: Use benchmarking to identify optimal configuration parameters for your use case.

5. **Advanced Analysis**: Use the analysis tools to detect patterns and optimize simulation parameters.

## Troubleshooting

### Common Issues

1. **Configuration File Not Found**: Ensure the configuration file path is correct and the file exists.

2. **Invalid Configuration Values**: Check that all configuration values are valid (numbers, booleans, strings).

3. **Performance Issues**: For large simulations, ensure you have sufficient memory and consider using smaller timesteps.

4. **Python Module Not Found**: Ensure the pynlm module is properly installed and in your Python path.

### Error Messages

If you encounter errors, check:
- Configuration file syntax (JSON/YAML)
- Valid Python version (3.8+)
- Required dependencies are installed
- File permissions for reading/writing configuration files

## Getting Help

For more information:

1. **Documentation**: See the `docs/` directory for detailed documentation
2. **Examples**: Check the `examples/` directory for additional usage examples
3. **API Reference**: Use Python help() or consult the pybind11 documentation for API details
4. **Issues**: Report issues and feature requests on the NLM GitHub repository

## Future Enhancements

The NLM framework continues to evolve. Future enhancements may include:

- Advanced visualization tools for brain activity
- Machine learning integration for parameter optimization
- Distributed computing support for large-scale simulations
- Enhanced debugging and profiling tools
- Integration with other neuroscience frameworks

## License

This advanced usage guide is part of the NLM framework, licensed under the MIT License. See the LICENSE file for details.
