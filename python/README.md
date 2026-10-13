# NLM Python API - Complete Documentation

## Overview

The NLM Python API provides a high-level interface to the Neural Learning Machine, allowing users to create and simulate neural networks through Python while leveraging the full C++ computational engine for performance.

## Installation

```bash
pip install pynlm
# or for development:
pip install -e .
```

## Basic Usage

### Creating a Brain

```python
import nlm

# Create a brain with default configuration
brain = nlm.Brain()

# Create a brain with custom configuration
config = nlm.Config({
    'neuron_count': 1000,
    'region_count': 2,
    'simulation_timestep': 0.001,
    'random_seed': 42
})
brain = nlm.Brain(config)
```

### Running Simulation

```python
# Initialize the brain
brain.initialize()

# Run for a number of steps
for step in range(1000):
    brain.step(step, step * 0.001)
    
# Inject sensory input
brain.inject_current_to_sensory_neurons(30.0)

# Get statistics
print(f"Total spikes: {brain.get_total_spike_count()}")
print(f"Firing neurons: {brain.get_firing_neuron_count()}")
print(f"Average firing rate: {brain.get_average_firing_rate()}")
```

### Learning Experiment

```python
# Enable plasticity
brain.enable_plasticity('stdp', True)
brain.enable_plasticity('hebbian', True)
brain.enable_plasticity('structural', True)

# Apply learning stimulus
for step in range(500):
    brain.inject_current_to_neurons('sensory', 40.0)
    brain.step(step, step * 0.001)

# Check learning results
synapses = brain.get_synapses()
print(f"Total synapses: {len(synapses)}")

# Get learning statistics
learning_stats = brain.get_learning_statistics()
print(f"Learning occurred: {learning_stats['learning_occurred']}")
print(f"Mean weight change: {learning_stats['mean_weight_change']}")
```

## Configuration API

### Creating Configuration

```python
import nlm

# Default configuration
config = nlm.Config()

# Custom configuration with dictionary
config = nlm.Config({
    'neuron_count': 2000,
    'region_count': 5,
    'connection_probability': 0.2,
    'simulation_timestep': 0.0005,
    'random_seed': 123,
    'plasticity_learning_rate': 0.02,
    'stdp_ltp_weight': 0.03,
    'stdp_ltd_weight': 0.02,
    'development_synaptogenesis_rate': 0.002,
    'dopamine_baseline': 0.1,
    'reward_discount_factor': 0.95,
    'environment_name': 'GridWorld',
    'log_level': 'INFO'
})

# Configuration validation
if config.validate():
    print("Configuration is valid")
else:
    print("Configuration validation errors:", config.get_validation_errors())
```

### Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 1000 | Number of neurons in the brain |
| `region_count` | int | 2 | Number of brain regions |
| `connection_probability` | float | 0.15 | Probability of forming synapses |
| `simulation_timestep` | float | 0.001 | Simulation timestep in seconds |
| `random_seed` | int | 42 | Random seed for reproducibility |
| `plasticity_learning_rate` | float | 0.01 | Learning rate for plasticity |
| `stdp_ltp_weight` | float | 0.01 | STDP long-term potentiation weight |
| `stdp_ltd_weight` | float | 0.012 | STDP long-term depression weight |
| `development_synaptogenesis_rate` | float | 0.001 | Rate of new synapse formation |
| `dopamine_baseline` | float | 0.1 | Baseline dopamine level |
| `reward_discount_factor` | float | 0.99 | Reward discount factor |
| `log_level` | string | INFO | Logging level |

## Brain API

### Brain Class Methods

#### Initialization and Configuration

```python
# Initialize brain
brain = nlm.Brain(config)
brain.initialize()

# Configure brain parameters
brain.set_neuron_count(1500)
brain.set_simulation_timestep(0.002)
brain.set_random_seed(999)
```

#### Simulation Control

```python
# Run simulation
brain.reset()  # Reset to initial state
brain.initialize()  # Reinitialize

# Single step
brain.step(step, time)

# Run multiple steps
for step in range(total_steps):
    brain.step(step, step * timestep)
```

#### Input and Output

```python
# Inject different types of current
brain.inject_current_to_neurons('sensory', 50.0)
brain.inject_current_to_neurons('motor', 25.0)
brain.inject_current_to_neurons('internal', 10.0)

# Get specific neurons
neurons = brain.get_neurons('sensory')
neurons = brain.get_neurons_in_region(region_id)

# Produce motor action
action = brain.produce_action()
if action:
    print(f"Action type: {action.type}")
    print(f"Action strength: {action.strength}")
```

#### Memory Systems

```python
# Access memory systems
working_memory = brain.get_working_memory()
episodic_memory = brain.get_episodic_memory()
associative_memory = brain.get_associative_memory()

# Working memory operations
working_memory.store('sensor_data', [1.0, 0.5, 0.8])
retrieved = working_memory.retrieve('sensor_data')
working_memory.update('sensor_data', [0.9, 0.6, 0.7])
```

#### Neuromodulation

```python
# Apply neuromodulatory signals
dopamine = brain.get_dopamine()
dopamine.apply_reward(1.0)  # Reward signal
curiosity = brain.get_curiosity()
curiosity.apply_exploration_drive(0.5)
```

#### Plasticity Control

```python
# Enable/disable plasticity rules
brain.enable_stdp(True, True, False)  # (LTP, LTD, structural)
brain.enable_hebbian(True)
brain.enable_structural(True)

# Get plasticity statistics
plasticity_stats = brain.get_plasticity_statistics()
print(f"STDP LTP events: {plasticity_stats['stdp_ltp_events']}")
print(f"STDP LTD events: {plasticity_stats['stdp_ltd_events']}")
```

#### Development

```python
# Development system
development = brain.get_development_system()
print(f"Current developmental stage: {development.get_developmental_stage()}")

# Stage progression
brain.develop()
brain.develop_to_stage('adolescence')
```

#### Checkpointing

```python
# Save brain state
brain.save('brain_checkpoint.nlm')

# Load brain state
brain.load('saved_brain.nlm')
```

## Advanced Usage Examples

### 1. Learning Experiment with Memory

```python
import nlm
import time

# Create brain for learning experiment
config = nlm.Config({
    'neuron_count': 500,
    'region_count': 3,
    'simulation_timestep': 0.001
})

brain = nlm.Brain(config)
brain.initialize()

# Enable all plasticity
brain.enable_plasticity('stdp', True, True, True)
brain.enable_plasticity('hebbian', True)
brain.enable_plasticity('structural', True)

# Learning phase
print("=== Learning Phase ===")
initial_synapses = brain.get_total_synapse_count()
initial_weights = brain.get_synapse_weights()

for episode in range(10):
    # Present random input pattern
    input_pattern = [0.5, 0.8, 0.3, 0.9, 0.6]
    brain.inject_current_to_sensory_neurons(input_pattern)
    
    # Run simulation
    for step in range(100):
        brain.step(episode * 100 + step, (episode * 100 + step) * 0.001)
    
    # Store episode in episodic memory
    brain.get_episodic_memory().store_episode({
        'input': input_pattern,
        'step': step,
        'reward': 1.0
    })

# Evaluate learning
final_synapses = brain.get_total_synapse_count()
final_weights = brain.get_synapse_weights()

learning_stats = brain.calculate_learning_metrics(initial_weights, final_weights)
print(f"Learning metrics: {learning_stats}")
print(f"Synapse formation rate: {(final_synapses - initial_synapses) / initial_synapses * 100:.1f}%")
```

### 2. World Interaction Simulation

```python
import nlm

# Create environment
config = nlm.Config({
    'environment_name': 'GridWorld',
    'environment_width': 10,
    'environment_height': 10,
    'neuron_count': 300
})

brain = nlm.Brain(config)
brain.initialize()

# Enable curiosity and reward systems
brain.get_curiosity().enable()
brain.get_dopamine().enable_reward_modulation()

# Interaction loop
print("=== World Interaction ===")
for episode in range(20):
    # Reset brain for new episode
    brain.reset()
    brain.initialize()
    
    # Track episode performance
    episode_reward = 0.0
    episode_steps = 0
    
    # Run episode
    while episode_steps < 100 and episode_reward < 10.0:
        # Get current observation
        observation = brain.get_environment().get_observation()
        
        # Apply sensory input
        brain.inject_sensory_input(observation)
        
        # Plan action
        action = brain.plan_action()
        
        # Execute action
        reward = brain.execute_action(action)
        episode_reward += reward
        
        # Learn from experience
        brain.learn_from_experience(action, reward)
        
        # Step simulation
        brain.step(episode_steps, episode_steps * 0.001)
        episode_steps += 1
    
    # Store experience in memory
    brain.get_episodic_memory().store_episode({
        'episode': episode,
        'reward': episode_reward,
        'steps': episode_steps,
        'action': action
    })
    
    # Apply development changes
    brain.develop()
    
    print(f"Episode {episode + 1}: Reward={episode_reward:.2f}, Steps={episode_steps}")

# Analyze learned behavior
learning_analysis = brain.analyze_behavior()
print(f"\nBehavior Analysis:")
print(f"Exploration rate: {learning_analysis['exploration_rate']:.2f}")
print(f"Exploitation rate: {learning_analysis['exploitation_rate']:.2f}")
print(f"Memory consolidation: {learning_analysis['memory_consolidation']:.2f}")
```

### 3. Parameter Tuning Experiment

```python
import nlm
import numpy as np

# Parameter sweep
parameter_combinations = [
    {'neuron_count': 500, 'connection_prob': 0.1},
    {'neuron_count': 1000, 'connection_prob': 0.15},
    {'neuron_count': 1500, 'connection_prob': 0.2},
    {'neuron_count': 2000, 'connection_prob': 0.25},
]

results = []

for params in parameter_combinations:
    print(f"\nTesting parameters: {params}")
    
    config = nlm.Config({
        'neuron_count': params['neuron_count'],
        'connection_probability': params['connection_prob'],
        'simulation_timestep': 0.001,
        'random_seed': 42
    })
    
    # Run multiple trials for statistical significance
    trial_results = []
    for trial in range(3):
        brain = nlm.Brain(config)
        brain.initialize()
        
        # Learning trial
        brain.enable_plasticity('stdp', True, True, True)
        brain.enable_plasticity('hebbian', True)
        
        for step in range(200):
            brain.inject_current_to_sensory_neurons(30.0)
            brain.step(step, step * 0.001)
        
        # Collect results
        trial_results.append({
            'spike_count': brain.get_total_spike_count(),
            'learning_occurred': brain.check_learning_occurred(),
            'final_weight_mean': brain.get_mean_synapse_weight(),
            'memory_episodes': brain.get_episodic_memory().get_episode_count()
        })
    
    # Average results
    avg_spikes = np.mean([r['spike_count'] for r in trial_results])
    avg_learning = np.mean([r['learning_occurred'] for r in trial_results])
    avg_weight_change = np.mean([r['final_weight_mean'] for r in trial_results])
    
    results.append({
        'parameters': params,
        'avg_spikes': avg_spikes,
        'avg_learning': avg_learning,
        'avg_weight_change': avg_weight_change
    })
    
    print(f"  Average spikes: {avg_spikes:.0f}")
    print(f"  Learning rate: {avg_learning*100:.1f}%")
    print(f"  Weight change: {avg_weight_change:.3f}")

# Analyze parameter effects
print("\n=== Parameter Analysis ===")
for result in results:
    print(f"Neurons: {result['parameters']['neuron_count']}, "
          f"Connection Prob: {result['parameters']['connection_prob']}, "
          f"Learning: {result['avg_learning']*100:.1f}%, "
          f"Weight Change: {result['avg_weight_change']:.3f}")
```

## Configuration File API

### Creating Configuration Files

```python
import nlm

# Create configuration from dictionary
config_dict = {
    'neuron_count': 1000,
    'region_count': 2,
    'connection_probability': 0.15,
    'simulation_timestep': 0.001,
    'random_seed': 42
}

config = nlm.Config(config_dict)

# Save configuration to file
config.save_to_file('my_config.cfg')

# Load configuration from file
config2 = nlm.Config().load_from_file('my_config.cfg')

# Validate configuration
errors = config.validate()
if errors:
    print("Configuration errors:", errors)
else:
    print("Configuration is valid")
```

### Configuration Schema

```python
# Get available configuration parameters
available_params = nlm.Config.get_available_parameters()
print("Available parameters:", list(available_params.keys()))

# Get parameter information
param_info = nlm.Config.get_parameter_info('neuron_count')
print(f"Parameter info: {param_info}")
```

## Data Structures and Classes

### Neuron Types

```python
import nlm

# Neuron type constants
print(f"Sensory neuron type: {nlm.NeuronType.SENSOR}")
print(f"Motor neuron type: {nlm.NeuronType.MOTOR}")
print(f"Internal neuron type: {nlm.NeuronType.INTERNAL}")
print(f"Excitatory synapse type: {nlm.SynapseType.EXCITATORY}")
print(f"Inhibitory synapse type: {nlm.SynapseType.INHIBITORY}")
```

### Action Class

```python
# Action objects returned by brain.produce_action()
action = brain.produce_action()
print(f"Action type: {action.type}")
print(f"Action strength: {action.strength}")
print(f"Action direction: {action.direction}")
```

### Memory Classes

```python
# Working memory
working_memory = nlm.NeuralWorkingMemory()
working_memory.store('temporary_data', value)
retrieved_value = working_memory.retrieve('temporary_data')

# Episodic memory
episodic_memory = nlm.NeuralEpisodicMemory()
episodic_memory.store_episode({
    'input': [1.0, 0.5, 0.8],
    'output': [0.3, 0.7],
    'reward': 1.0
})
episode = episodic_memory.retrieve_episode(0)

# Associative memory
associative_memory = nlm.NeuralAssociativeMemory()
associative_memory.associate('input1', 'output1')
association_strength = associative_memory.get_association_strength('input1', 'output1')
```

## Visualization and Monitoring

### Basic Monitoring

```python
import nlm

brain = nlm.Brain()
brain.initialize()

# Monitor simulation
for step in range(100):
    brain.step(step, step * 0.001)
    
    if step % 10 == 0:
        status = brain.get_simulation_status()
        print(f"Step {step}: Spikes={status['spike_count']}, "
              f"Firing={status['firing_neuron_count']}, "
              f"Memory={status['working_memory_size']}")

# Get performance metrics
performance = brain.get_performance_metrics()
print(f"Performance metrics: {performance}")
```

### Logging Configuration

```python
# Configure logging
nlm.Config.set_log_level('DEBUG')
nlm.Config.enable_console_logging(True)
nlm.Config.enable_file_logging('nlm_simulation.log')

# Get log statistics
log_stats = nlm.Logger.get_statistics()
print(f"Log statistics: {log_stats}")
```

## Error Handling

### Common Exceptions

```python
import nlm

try:
    config = nlm.Config({'neuron_count': -100})  # Invalid value
    print("Should not reach here")
except ValueError as e:
    print(f"Configuration error: {e}")

try:
    brain = nlm.Brain()
    brain.initialize()
    brain.step(0, 0.0)  # Should not crash
    print("Step completed successfully")
except RuntimeError as e:
    print(f"Runtime error: {e}")
```

### Validation Functions

```python
# Validate specific values
if nlm.Config.validate_neuron_count(100):
    print("100 neurons is valid")
else:
    print("100 neurons is invalid")

# Check parameter bounds
param_min, param_max = nlm.Config.get_parameter_bounds('connection_probability')
print(f"Connection probability bounds: {param_min} to {param_max}")
```

## Performance Tips

### Optimizing Performance

```python
# Use batches for large simulations
config = nlm.Config({
    'neuron_count': 5000,
    'simulation_timestep': 0.001
})

brain = nlm.Brain(config)
brain.initialize()

# Process in batches to manage memory
batch_size = 1000
for batch_start in range(0, 10000, batch_size):
    batch_end = min(batch_start + batch_size, 10000)
    
    for step in range(batch_start, batch_end):
        brain.step(step, step * 0.001)
    
    # Clear working memory between batches if needed
    # brain.get_working_memory().clear()

# Use checkpointing for long simulations
if step % 1000 == 0:
    brain.save(f'checkpoint_{step}.nlm')
```

### Memory Management

```python
# Monitor memory usage
brain = nlm.Brain()
brain.initialize()

memory_stats = brain.get_memory_usage()
print(f"Memory usage: {memory_stats}")

# Clear memory when not needed
brain.clear_temporary_memory()
```

## Migration Guide

### From C++ to Python

If you're migrating from C++ NLM usage:

**C++**
```cpp
auto config = std::make_shared<Config>();
config->set("neuron_count", 1000);
auto brain = std::make_shared<Brain>(config);
brain->initialize();
brain->step(0, 0.0);
```

**Python**
```python
config = nlm.Config({'neuron_count': 1000})
brain = nlm.Brain(config)
brain.initialize()
brain.step(0, 0.0)
```

### Configuration Mapping

| C++ Parameter | Python Parameter |
|---------------|------------------|
| `neuron_count` | `neuron_count` |
| `region_count` | `region_count` |
| `simulation_timestep` | `simulation_timestep` |
| `random_seed` | `random_seed` |
| `connection_probability` | `connection_probability` |

## Changelog

### Version 0.1.0
- Initial release
- Basic brain simulation API
- Configuration management
- Memory systems support
- Plasticity controls

### Future Features
- Visualization tools
- Advanced environment support
- Multi-agent systems
- Distributed computing support

## Support and Community

For questions and support:
- Check the NLM documentation
- Join the NLM community forum
- Report issues on GitHub
- Contribute to the Python bindings

## License

MIT License - See LICENSE file for details.
