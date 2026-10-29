# NLM - Enhanced Usage Guide

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain simulation framework. This enhanced guide covers both beginner and advanced usage patterns, including the new advanced features we've added.

## Quick Start (Enhanced)

### Example 1: Advanced Brain Configuration

```python
import pynlm

# Create brain with advanced configuration
brain_config = {
    'neuron_count': 2000,
    'connection_probability': 0.2,
    'stdp_ltp_weight': 0.03,
    'stdp_ltd_weight': 0.025,
    'enable_hebbian': True,
    'enable_stdp': True,
    'enable_curiosity': True,
    'enable_reward_modulated': False,
    'enable_development': True
}

brain = pynlm.create_brain(brain_config)
brain.initialize()
```

### Example 2: Complete Agent with World

```python
import pynlm
import time

# Create world with advanced configuration
world_config = {
    'width': 20,
    'height': 20,
    'vision_width': 8,
    'vision_height': 8,
    'max_energy': 200.0,
    'energy_decay_rate': 0.01,
    'energy_gain_rate': 0.05
}

world = pynlm.create_simple_world(world_config)
world.set_agent_start(5.0, 5.0)

# Create brain with custom configuration
brain = pynlm.create_advanced_brain({
    'neuron_count': 1500,
    'connection_probability': 0.15,
    'enable_hebbian': True,
    'enable_stdp': True,
    'enable_curiosity': True,
    'enable_development': True
})

# Create agent with enhanced capabilities
agent_config = {
    'enable_reward_modulation': True,
    'enable_structural_plasticity': True,
    'enable_development': True,
    'enable_curiosity': True
}

agent = pynlm.create_agent_brain(brain, agent_config)
agent.initialize(world)

# Run advanced simulation
for step in range(1000):
    # Get sensory input from world
    percept = world.get_sensory_percept()
    
    # Process through agent
    agent.process_sensory_input(percept)
    
    # Brain computation
    brain.step(step)
    
    # Get motor command
    action = agent.decode_motor_command()
    
    # Apply action in world
    world.apply_motor_command(action, world.get_simulation_time())
    
    # Apply reward modulation
    reward = calculate_reward(percept, action)
    agent.apply_reward_modulation(reward, reward)
    
    # Update development
    agent.update_development(0.01)
    
    if step % 100 == 0:
        print(f"Step {step}: Firing rate={brain.get_average_firing_rate():.2f}, "
              f"Active neurons={brain.get_active_neuron_count()}")
```

### Example 3: Batch Processing

```python
import pynlm
import time

# Create multiple brains for batch processing
brains = []
world_configs = []

for i in range(5):
    brain = pynlm.create_advanced_brain({
        'neuron_count': 1000 + i * 500,
        'connection_probability': 0.1 + i * 0.02,
        'seed': 42 + i
    })
    brains.append(brain)
    
    world = pynlm.create_simple_world({
        'width': 10,
        'height': 10,
        'vision_width': 6,
        'vision_height': 6
    })
    world_configs.append(world)

# Run batch simulation
from pynlm import run_batch_simulation, generate_performance_report

simulation_params = {
    'steps': 2000,
    'monitoring': True,
    'save_checkpoint': False
}

results = run_batch_simulation(brains, world_configs, simulation_params)

# Generate performance report
report = generate_performance_report(results)
print(f"Batch simulation completed!")
print(f"Performance Grade: {report.performance_grade}")
print(f"Average Firing Rate: {report.average_average_firing_rate:.2f}")
```

## Advanced Features

### Configuration Management

The enhanced configuration system supports:

1. **Dictionary-based configuration**:
```python
config = {
    'neuron_count': 2000,
    'connection_probability': 0.2,
    'stdp_ltp_weight': 0.03,
    'stdp_ltd_weight': 0.025,
    'enable_hebbian': True,
    'enable_stdp': True,
    'enable_curiosity': False,
    'enable_reward_modulated': True,
    'enable_development': True
}

brain = pynlm.create_brain(config)
```

2. **Type-safe parameter access**:
```python
# Get parameters with type safety and defaults
neuron_count = brain.config.get('neuron_count', 1000)
connection_prob = brain.config.get('connection_probability', 0.15)
stdp_enabled = brain.config.get('enable_stdp', True)
```

3. **Configuration validation**:
```python
schema = {
    'neuron_count': int,
    'connection_probability': float,
    'enable_stdp': bool,
    'stdp_ltp_weight': float
}

brain.config.validate(schema)  # Raises exception if validation fails
```

### Brain Operations

#### Advanced Initialization
```python
# Create brain with advanced features
brain = pynlm.create_advanced_brain(config, enable_advanced_features=True)

# Configure plasticity rules
brain.config.set('hebbian_enabled', True)
brain.config.set('stdp_enabled', True)
brain.config.set('reward_modulated_enabled', True)
brain.config.set('curiosity_enabled', True)

# Initialize with advanced parameters
brain.initialize()
```

#### Enhanced Monitoring
```python
# Get detailed brain statistics
stats = {
    'total_neurons': brain.get_total_neuron_count(),
    'total_synapses': brain.get_total_synapse_count(),
    'firing_rate': brain.get_average_firing_rate(),
    'active_neurons': brain.get_active_neuron_count(),
    'total_spikes': brain.get_total_spike_count(),
    'development_stage': brain.get_developmental_stage(),
    'working_memory': brain.get_working_memory(),
    'episodic_memory': brain.get_episodic_memory(),
    'prediction_system': brain.get_prediction_system()
}
```

### Simulation Modes

#### Basic Mode
```python
brain = pynlm.create_brain({'enable_development': False})
brain.initialize()

# Simple simulation without advanced features
for step in range(1000):
    brain.step(step)
```

#### Learning Mode
```python
brain = pynlm.create_advanced_brain({
    'enable_hebbian': True,
    'enable_stdp': True,
    'enable_curiosity': True,
    'enable_development': True
})

# Run learning simulation with advanced features
for step in range(5000):
    if step % 10 == 0:
        brain.inject_current_to_neurons('sensory', 50.0)
    
    brain.step(step)
    
    if step % 1000 == 0:
        print(f"Learning step {step}: firing_rate={brain.get_average_firing_rate():.2f}")
```

#### STDP Mode
```python
brain = pynlm.create_advanced_brain({
    'enable_hebbian': False,
    'enable_stdp': True,
    'enable_reward_modulated': False
})

# Run STDP-specific learning
for step in range(3000):
    if step % 50 == 0:
        # Create correlated pre-post activity
        brain.inject_current_to_neurons('excitatory', 40.0)
        brain.step(step)
        
        brain.inject_current_to_neurons('inhibitory', 40.0)
        brain.step(step + 1)
    
    brain.step(step)
```

#### Connectivity Mode
```python
brain = pynlm.create_advanced_brain({
    'enable_hebbian': True,
    'enable_stdp': True,
    'enable_structural': True
})

# Analyze network connectivity
initial_synapses = brain.get_total_synapse_count()

for step in range(2000):
    brain.step(step)
    
    if step % 500 == 0:
        current_synapses = brain.get_total_synapse_count()
        print(f"Step {step}: {current_synapses} synapses")

final_synapses = brain.get_total_synapse_count()
print(f"Net synapse change: {final_synapses - initial_synapses}")
```

## Performance Monitoring

### Performance Statistics
```python
# Get comprehensive performance statistics
perf_stats = brain.get_performance_stats()
print(f"Performance Report:")
print(f"  Total Neurons: {perf_stats['total_neurons']}")
print(f"  Total Synapses: {perf_stats['total_synapses']}")
print(f"  Average Firing Rate: {perf_stats['average_firing_rate']:.2f}")
print(f"  Excitatory/Inhibitory Ratio: {perf_stats['excitatory_inhibition_ratio']:.2f}")
print(f"  Memory Integration: {perf_stats['memory_working_memory_integrated']}")
print(f"  Neuromodulation Integration: {perf_stats['neuromodulation_integrated']}")
```

### Batch Processing Performance
```python
from pynlm import run_batch_simulation, generate_performance_report

# Run multiple simulations in parallel
results = run_batch_simulation(brains, worlds, simulation_params)

# Generate comprehensive report
report = generate_performance_report(results)
print(f"=== BATCH SIMULATION REPORT ===")
print(f"Simulations: {report.total_simulations}")
print(f"Average Total Spikes: {report.average_total_spikes:.1f}")
print(f"Average Total Reward: {report.average_total_reward:.2f}")
print(f"Average Firing Rate: {report.average_average_firing_rate:.2f}")
print(f"Performance Grade: {report.performance_grade}")
print(f"=== END REPORT ===")
```

## Error Handling and Validation

### Configuration Validation
```python
# Validate configuration against schema
def validate_config(config):
    schema = {
        'neuron_count': (int, lambda x: x > 0),
        'connection_probability': (float, lambda x: 0 <= x <= 1),
        'enable_stdp': (bool, lambda x: True),
        'stdp_ltp_weight': (float, lambda x: x >= 0)
    }
    
    for key, (expected_type, validator) in schema.items():
        if key in config:
            value = config[key]
            if not isinstance(value, expected_type):
                raise ValueError(f"Key '{key}' expected type {expected_type.__name__}, got {type(value).__name__}")
            if not validator(value):
                raise ValueError(f"Key '{key}' value {value} failed validation")
    
    return True

# Usage
try:
    validate_config(config_dict)
    brain = pynlm.create_brain(config_dict)
    print("Configuration validated successfully")
except ValueError as e:
    print(f"Configuration error: {e}")
```

### Exception Handling
```python
import pynlm

try:
    brain = pynlm.create_brain({'neuron_count': -100})
    brain.initialize()
except pynlm.NLMConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.NLMRuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Advanced Configuration Options

### Complete Configuration Reference

```python
advanced_config = {
    # Core brain parameters
    'neuron_count': 2000,                    # Total neurons
    'region_count': 1,                       # Number of brain regions
    'connection_probability': 0.15,           # Synaptic connection probability
    'random_seed': 42,                       # Random seed for reproducibility
    
    # STDP parameters
    'stdp_ltp_weight': 0.02,                  # Long-term potentiation weight
    'stdp_ltd_weight': 0.015,                 # Long-term depression weight
    'stdp_tau': 20.0,                         # STDP time constant
    
    # Structural plasticity
    'synaptogenesis_rate': 0.0001,           # Rate of new synapse formation
    'pruning_rate': 0.00001,                 # Rate of synapse elimination
    
    # Neuron parameters
    'leak_conductance': 10.0,                # Membrane leak conductance
    'refractory_period': 5,                  # Refractory period in ms
    'threshold': -55.0,                      # Firing threshold (mV)
    'resting_potential': -70.0,             # Resting potential (mV)
    'reset_potential': -70.0,               # Reset potential (mV)
    
    # Plasticity flags
    'hebbian_enabled': True,                 # Enable Hebbian plasticity
    'stdp_enabled': True,                    # Enable STDP plasticity
    'reward_modulated_enabled': False,       # Enable reward-modulated plasticity
    'structural_enabled': True,              # Enable structural plasticity
    'development_enabled': True,             # Enable developmental processes
    
    # Neuromodulation
    'curiosity_enabled': False,              # Enable curiosity-driven learning
    
    # Performance optimization
    'enable_simd': True,                     # Enable SIMD optimizations
    'memory_pool_size': 512,                 # Memory pool size (MB)
}
```

## Troubleshooting

### Common Issues and Solutions

1. **Configuration Validation Errors**
```python
# Issue: Invalid configuration
config = {'neuron_count': -100}

# Solution: Validate before use
if 'neuron_count' in config and config['neuron_count'] <= 0:
    config['neuron_count'] = 1000  # Set reasonable default

brain = pynlm.create_brain(config)
```

2. **Memory Issues**
```python
# Issue: Out of memory for large simulations

# Solution: Use batch processing
brains = []
for i in range(5):
    brain = pynlm.create_brain({'neuron_count': 500})
    brains.append(brain)

results = run_batch_simulation(brains, worlds, {'steps': 1000})
```

3. **Performance Issues**
```python
# Issue: Simulation too slow

# Solution: Enable optimizations
brain = pynlm.create_advanced_brain({
    'enable_simd': True,
    'memory_pool_size': 1024,
    'neuron_count': 1000
})

# Use efficient simulation loops
for step in range(1000):
    brain.step(step)
```

## Best Practices

### 1. Use Configuration Files for Complex Simulations
```python
# Save configuration to file
config = {
    'neuron_count': 2000,
    'connection_probability': 0.15,
    'stdp_ltp_weight': 0.02,
    'stdp_ltd_weight': 0.015
}

brain.config.save_to_file('simulation_config.json')

# Load configuration
brain = pynlm.create_brain('simulation_config.json')
```

### 2. Implement Proper Error Handling
```python
import pynlm
import logging

logging.basicConfig(level=logging.INFO)

def run_simulation_with_error_handling():
    try:
        brain = pynlm.create_advanced_brain({
            'neuron_count': 1000,
            'enable_development': True
        })
        
        brain.initialize()
        
        for step in range(1000):
            brain.step(step)
            
    except pynlm.NLMConfigurationError as e:
        logging.error(f"Configuration error: {e}")
        return False
    except pynlm.NLMRuntimeError as e:
        logging.error(f"Runtime error: {e}")
        return False
    except Exception as e:
        logging.error(f"Unexpected error: {e}")
        return False
    
    return True
```

### 3. Use Performance Monitoring
```python
# Enable performance monitoring
from pynlm import create_performance_monitor

perf_monitor = create_performance_monitor()
brain = pynlm.create_advanced_brain({'enable_development': True})
perf_monitor.start_monitoring(brain)

# Run simulation
for step in range(1000):
    brain.step(step)

# Get performance statistics
stats = perf_monitor.get_statistics()
print(f"Simulation completed in {stats['total_time']:.2f} seconds")
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
```

## Migration Guide

### From Basic to Advanced

If you're migrating from basic NLM usage to the enhanced version:

1. **Update configuration format**:
   ```python
   # Basic: create_brain(neuron_count, config_file)
   # Enhanced: create_brain(config_dict)
   config = {'neuron_count': 1000, 'connection_probability': 0.15}
   brain = pynlm.create_brain(config)
   ```

2. **Enable advanced features**:
   ```python
   # Basic: No advanced features
   # Enhanced: create_advanced_brain with features
   brain = pynlm.create_advanced_brain(config, enable_advanced_features=True)
   ```

3. **Use enhanced monitoring**:
   ```python
   # Basic: No monitoring
   # Enhanced: Performance monitoring available
   stats = brain.get_performance_stats()
   ```

## Next Steps

1. **Explore batch processing**: Use `run_batch_simulation` for parallel simulations
2. **Implement custom learning**: Design your own learning rules using the enhanced API
3. **Add visualization**: Use the built-in visualization helpers for neural activity monitoring
4. **Performance optimization**: Leverage SIMD and memory pools for large-scale simulations
5. **Machine learning integration**: Use the built-in ML utilities for pattern discovery

This enhanced version provides powerful tools for neural simulation research while maintaining backward compatibility with the basic API.

---

**Need Help?**
- Check the documentation for detailed examples
- Use the advanced error handling and validation
- Leverage performance monitoring for optimization
- Explore batch processing for large-scale simulations

**Version**: 2.0.0 Enhanced Edition
**Documentation**: Complete with examples and troubleshooting
**Support**: Comprehensive error handling and validation