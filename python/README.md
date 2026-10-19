# NLM Python API Documentation

## Overview

The NLM (Neural Learning Machine) Python API provides a Python interface to the NLM C++ neural simulation framework. This documentation provides comprehensive information about the API, including examples, API reference, and best practices.

## Installation

### Prerequisites

- Python 3.8 or later
- C++ compiler with C++20 support
- CMake 3.16 or later

### Building from Source

1. Clone the NLM repository:
```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

2. Create a build directory:
```bash
mkdir build
cd build
```

3. Configure CMake:
```bash
cmake .. -DPYTHON_EXECUTABLE=/usr/bin/python3
```

4. Build the Python module:
```bash
cmake --build . --target pynlm
```

5. Install the module:
```bash
cmake --build . --target install
```

### Using pip (if available)

If there's a setup.py or pyproject.toml, you can try:
```bash
pip install -e .
```

## Quick Start

### Basic Usage

Here's a simple example to get started:

```python
import pynlm

# Create default configuration
config = pynlm.make_config("experiment")
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)

# Create brain
brain = pynlm.make_brain(config)

# Initialize brain
if brain.initialize():
    print("Brain initialized successfully!")

# Run a few simulation steps
for step in range(10):
    brain.step(step, step * 0.001)
    print(f"Step {step}: Total spikes = {brain.get_total_spike_count()}")

# Create and configure world
world = pynlm.make_world()
world.configure(20, 20)
world.set_agent_start(10, 10)
world.reset()

# Create agent brain
agent = pynlm.make_agent_brain(brain)

print(f"Experiment completed. Final spikes: {brain.get_total_spike_count()}")
```

## API Reference

### Configuration Class

The `Config` class provides Pythonic configuration management.

#### Methods

```python
# Create configuration
config = pynlm.make_config("name")

# Set values
config.set("key", value, "source")  # source: "runtime", "file", "command_line"

# Get values
int_value = config.get_int("key", default)
double_value = config.get_double("key", default)
string_value = config.get_string("key", default)
bool_value = config.get_bool("key", default)

# Environment variables
config.load_from_env("NLM_")

# File I/O
config.load_from_json("config.json")
config.save_to_json("output.json")

# Validation
config.validate()

# Information
config.summary()
keys = config.get_keys()
config.has("key")
```

### Brain Class

The `Brain` class provides enhanced brain simulation capabilities.

#### Methods

```python
# Create brain
brain = pynlm.make_brain(config)

# Initialize
brain.initialize()

# Simulation
brain.step(step, time)

# Statistics
brain.get_total_neuron_count()
brain.get_total_synapse_count()
brain.get_active_neuron_count()
brain.get_firing_neuron_count()
brain.get_average_firing_rate()
brain.get_excitation_inhibition_ratio()
brain.get_total_spike_count()

# Input/Output
brain.receive_sensory_input(input)
action = brain.produce_action()

# State management
brain.reset()
brain.save("brain_state.json")
brain.load("brain_state.json")

# Development
brain.get_developmental_stage()
brain.set_developmental_stage("Stage")

# Status
status = brain.get_status()

# Neuromodulation
brain.apply_neuromodulation(signal)
brain.update_plasticity()
brain.develop()

# Current injection
brain.inject_current(neuron, current)
brain.inject_current_to_neurons(type, current)

# Regions
region = brain.get_region(region_id)
new_region_id = brain.add_region("name")
region_ids = brain.get_region_ids()
```

### SimpleWorld Class

The `SimpleWorld` class provides a simple 2D simulation environment.

#### Methods

```python
# Create world
world = pynlm.make_world()

# Configure
world.configure(width, height, vision_width=5, vision_height=5)

# State management
world.reset()
world.set_agent_start(x, y)

# Simulation
world.update(timestep=0.001)
world.apply_motor_command(command, current_time)

# Sensing
percept = world.get_sensory_percept()
body = world.get_agent_body()

# Objects
world.add_object(object)
world.remove_object(x, y)
world.is_valid_position(x, y)

# Information
world.get_dimensions()
world.get_simulation_time()
world.set_random_seed(seed)
world.get_random_seed()
world.get_max_energy()
world.set_max_energy(energy)
world.get_energy_decay_rate()
world.set_energy_decay_rate(rate)
```

### AgentBrain Class

The `AgentBrain` class provides the interface between the brain and the world.

#### Methods

```python
# Create agent brain
agent = pynlm.make_agent_brain(brain)

# Initialize with world
agent.initialize(world)

# Sensory processing
agent.process_sensory_input(percept)

# Motor control
command = agent.decode_motor_command()

# Reward processing
agent.apply_reward_modulation(reward, predicted_reward)

# Development
agent.update_development(timestep)

# Information
agent.get_developmental_stage()
agent.get_neuromodulation_level()
agent.get_curiosity_level()
agent.get_novelty_level()
agent.get_prediction_error()

# State management
agent.reset()

# Feature management
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

# Configuration checks
agent.is_reward_modulation_enabled()
agent.is_structural_plasticity_enabled()
agent.is_development_enabled()
agent.is_curiosity_enabled()
```

## Examples

### Complete Simulation Example

```python
import pynlm
import time

def run_simulation(steps=100, headless=False):
    """Run a complete NLM simulation."""
    print("Starting NLM simulation...")
    
    # Create configuration
    config = pynlm.make_config("simulation")
    config.set("neuron_count", 500)
    config.set("simulation_timestep", 0.001)
    config.set("random_seed", 42)
    
    # Create brain and world
    brain = pynlm.make_brain(config)
    world = pynlm.make_world()
    
    # Initialize
    if not brain.initialize():
        raise RuntimeError("Failed to initialize brain")
    
    world.configure(30, 30)
    world.set_agent_start(15, 15)
    world.reset()
    
    # Create agent
    agent = pynlm.make_agent_brain(brain)
    if not agent.initialize(world):
        raise RuntimeError("Failed to initialize agent")
    
    print(f"Simulation started with {brain.get_total_neuron_count()} neurons")
    print(f"Initial spikes: {brain.get_total_spike_count()}")
    
    # Performance tracking
    start_time = time.time()
    
    # Main simulation loop
    for step in range(steps):
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process sensory input
        agent.process_sensory_input(percept)
        
        # Get motor command
        action = agent.decode_motor_command()
        
        # Apply action to world
        world.apply_motor_command(action.motor_command)
        
        # Update development
        agent.update_development(0.001)
        
        # Apply reward (simplified)
        reward = 1.0 if action.success else -1.0
        agent.apply_reward_modulation(reward, 0.0)
        
        # Update world
        world.update(0.001)
        
        # Print progress
        if not headless and step % 10 == 0:
            print(f"Step {step}: "
                  f"Energy={world.get_agent_body().energy:.1f}, "
                  f"Spikes={brain.get_total_spike_count()}, "
                  f"Firing={brain.get_firing_neuron_count()}")
    
    end_time = time.time()
    
    # Get final statistics
    total_time = end_time - start_time
    steps_per_second = steps / total_time if total_time > 0 else 0
    
    print("\nSimulation completed!")
    print(f"Total time: {total_time:.2f} seconds")
    print(f"Steps per second: {steps_per_second:.2f}")
    print(f"Final spikes: {brain.get_total_spike_count()}")
    print(f"Final energy: {world.get_agent_body().energy}")
    
    # Save results
    brain.save(f"simulation_step_{steps}.json")
    world.save(f"world_step_{steps}.json")
    
    return brain, world, agent

# Example usage
if __name__ == "__main__":
    brain, world, agent = run_simulation(steps=100, headless=False)
```

### Advanced Configuration Example

```python
import pynlm
import os

def create_advanced_config():
    """Create an advanced configuration with environment variables."""
    # Create configuration
    config = pynlm.make_config("advanced_experiment")
    
    # Set basic parameters
    config.set("neuron_count", 2000)
    config.set("simulation_timestep", 0.0005)
    config.set("region_count", 3)
    config.set("connection_probability", 0.05)
    
    # Load from environment variables
    config.load_from_env("MYAPP_")
    
    # Override with environment variables if present
    if os.environ.get("MYAPP_NEURON_COUNT"):
        config.set("neuron_count", int(os.environ["MYAPP_NEURON_COUNT"]))
    
    if os.environ.get("MYAPP_SIMULATION_STEPS"):
        config.set("max_simulation_steps", int(os.environ["MYAPP_SIMULATION_STEPS"]))
    
    # Set specialized plasticity parameters
    config.set("stdp_ltp_weight", 0.03)
    config.set("stdp_ltd_weight", 0.025)
    config.set("stdp_time_constant", 25.0)
    
    # Set development parameters
    config.set("development_synaptogenesis_rate", 0.002)
    config.set("development_pruning_rate", 0.0005)
    
    # Set neuromodulation parameters
    config.set("dopamine_baseline", 0.1)
    config.set("reward_discount_factor", 0.95)
    
    # Set logging
    config.set("log_level", "DEBUG")
    config.set("log_to_file", True)
    config.set("log_filename", "advanced_simulation.log")
    
    # Validate configuration
    if not config.validate():
        print("Warning: Configuration validation failed")
    
    return config

# Use the advanced configuration
config = create_advanced_config()
print("Advanced configuration:")
print(config.summary())

# Create brain with advanced configuration
brain = pynlm.make_brain(config)
brain.initialize()
```

### Learning Experiment Example

```python
import pynlm

def run_learning_experiment(steps=500, target_spike_rate=0.1):
    """Run a learning experiment with STDP."""
    print("Starting learning experiment...")
    
    # Create configuration optimized for learning
    config = pynlm.make_config("learning_experiment")
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1)
    config.set("random_seed", 12345)
    
    # Enable plasticity
    config.set("plasticity_learning_rate", 0.01)
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.018)
    
    # Create brain
    brain = pynlm.make_brain(config)
    
    # Initialize
    if not brain.initialize():
        raise RuntimeError("Failed to initialize brain")
    
    # Create world
    world = pynlm.make_world()
    world.configure(40, 40)
    world.set_agent_start(20, 20)
    world.reset()
    
    # Create agent
    agent = pynlm.make_agent_brain(brain)
    agent.initialize(world)
    
    # Enable all plasticity mechanisms
    agent.enable_structural_plasticity(True)
    agent.enable_reward_modulation(True)
    
    # Learning experiment loop
    learning_data = {
        'step': [],
        'spike_count': [],
        'firing_rate': [],
        'reward': [],
        'weight_change': []
    }
    
    for step in range(steps):
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process sensory input
        agent.process_sensory_input(percept)
        
        # Get action
        action = agent.decode_motor_command()
        
        # Apply action
        world.apply_motor_command(action.motor_command)
        
        # Update development
        agent.update_development(0.001)
        
        # Calculate reward based on firing rate
        firing_rate = brain.get_average_firing_rate() / brain.get_total_neuron_count()
        reward = 1.0 if firing_rate > target_spike_rate else 0.0
        
        # Apply reward
        agent.apply_reward_modulation(reward, firing_rate)
        
        # Update world
        world.update(0.001)
        
        # Record data
        learning_data['step'].append(step)
        learning_data['spike_count'].append(brain.get_total_spike_count())
        learning_data['firing_rate'].append(firing_rate)
        learning_data['reward'].append(reward)
        
        # Estimate weight change (simplified)
        weight_change = agent.get_prediction_error()
        learning_data['weight_change'].append(weight_change)
        
        # Print progress
        if step % 50 == 0:
            print(f"Step {step}: Firing rate={firing_rate:.4f}, "
                  f"Reward={reward:.2f}, Weight change={weight_change:.4f}")
    
    print(f"\nLearning experiment completed!")
    print(f"Final firing rate: {learning_data['firing_rate'][-1]:.4f}")
    print(f"Total reward: {sum(learning_data['reward']):.2f}")
    print(f"Final weight change: {learning_data['weight_change'][-1]:.4f}")
    
    # Analyze learning
    initial_rate = learning_data['firing_rate'][0]
    final_rate = learning_data['firing_rate'][-1]
    rate_change = final_rate - initial_rate
    
    print(f"\nLearning Analysis:")
    print(f"Initial firing rate: {initial_rate:.4f}")
    print(f"Final firing rate: {final_rate:.4f}")
    print(f"Rate change: {rate_change:.4f}")
    print(f"Learning success: {abs(rate_change) > 0.01}")
    
    return learning_data, brain, world, agent

# Run learning experiment
learning_data, brain, world, agent = run_learning_experiment(steps=500)
```

## Performance Monitoring

The NLM Python API includes built-in performance monitoring tools:

```python
import pynlm
import time

# Create brain
config = pynlm.make_config("performance_test")
config.set("neuron_count", 2000)
brain = pynlm.make_brain(config)

# Initialize
brain.initialize()

# Performance monitoring
start_time = time.time()
step_count = 1000

for step in range(step_count):
    # Inject some activity
    for i in range(min(10, brain.get_total_neuron_count())):
        neuron_id = pynlm.NeuronId(i)
        brain.inject_current(neuron_id, 20.0)
    
    # Run simulation
    brain.step(step, step * 0.001)
    
    # Monitor progress
    if step % 100 == 0:
        print(f"Progress: {step}/{step_count} steps")

end_time = time.time()

# Get performance metrics
metrics = brain.get_performance_metrics(start_time, end_time)

print(f"Performance Results:")
print(f"Simulation time: {metrics.simulation_time:.2f} seconds")
print(f"Steps per second: {metrics.steps_completed / metrics.simulation_time:.2f}")
print(f"Neurons: {brain.get_total_neuron_count()}")
print(f"Synapses: {brain.get_total_synapse_count()}")
print(f"Total spikes: {brain.get_total_spike_count()}")
print(f"Average firing rate: {metrics.spikes_generated / brain.get_total_neuron_count() / metrics.simulation_time:.4f} Hz")
```

## Error Handling

The NLM Python API includes comprehensive error handling:

```python
import pynlm

def safe_simulation():
    try:
        # Create configuration
        config = pynlm.make_config("safe_experiment")
        
        # Create brain
        brain = pynlm.make_brain(config)
        
        # Initialize
        if not brain.initialize():
            raise pynlm.RuntimeError("Brain initialization failed")
        
        # Run simulation
        for step in range(100):
            brain.step(step, step * 0.001)
        
        print("Simulation completed successfully")
        return brain
        
    except pynlm.RuntimeError as e:
        print(f"Runtime error: {e}")
        return None
    except pynlm.ValueError as e:
        print(f"Value error: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {e}")
        return None

# Use safe simulation
brain = safe_simulation()
if brain is not None:
    print("Simulation successful!")
```

## Advanced Features

### Checkpointing

Save and restore brain states for long-running experiments:

```python
import pynlm

# Create and initialize brain
config = pynlm.make_config("checkpoint_experiment")
brain = pynlm.make_brain(config)

if brain.initialize():
    # Run some steps
    for step in range(50):
        brain.step(step, step * 0.001)
    
    # Save checkpoint
    brain.save("checkpoint_50.json")
    print("Checkpoint saved")
    
    # Later, restore from checkpoint
    brain.reset()
    if brain.load("checkpoint_50.json"):
        print("Checkpoint loaded successfully")
```

### Configuration Validation

Validate configuration before running simulations:

```python
import pynlm

config = pynlm.make_config("validation_experiment")
config.set("neuron_count", 100)
config.set("simulation_timestep", 0.001)
config.set("connection_probability", 0.2)  # Should be <= 1.0

# Validate configuration
if not config.validate():
    print("Configuration validation failed!")
else:
    print("Configuration is valid")
```

## Troubleshooting

### Common Issues

1. **Module not found**: Ensure the pynlm module is installed correctly
   ```bash
   pip install pynlm
   ```

2. **Import errors**: Check that all dependencies are installed
   ```bash
   pip install pybind11 numpy
   ```

3. **Memory issues**: Reduce neuron count for testing
   ```python
   config.set("neuron_count", 100)  # Reduced for testing
   ```

4. **Performance issues**: Check system resources
   ```bash
   free -h  # Check memory
   ```

### Debugging

Enable debug logging:
```python
import pynlm

config = pynlm.make_config("debug_experiment")
config.set("log_level", "DEBUG")
config.set("log_to_file", True)
config.set("log_filename", "debug.log")

brain = pynlm.make_brain(config)
```

## Migration Guide

### From C++ to Python API

If you're migrating from the C++ API to the Python API, here are some key differences:

| C++ API | Python API |
|---------|------------|
| `std::shared_ptr<Brain>` | `PythonBrain` |
| `Config::loadFromFile()` | `config.load_from_json()` |
| `brain->step(step)` | `brain.step(step, time)` |
| `brain->getTotalNeuronCount()` | `brain.get_total_neuron_count()` |
| `neuron->injectCurrent()` | `brain.inject_current()` |

### Naming Conventions

The Python API uses snake_case naming for methods and properties, following Python conventions:

- `get_total_neuron_count()` instead of `getTotalNeuronCount()`
- `load_from_json()` instead of `loadFromJson()`
- `set_neuron_count()` instead of `setNeuronCount()`

## License

The NLM Python API is distributed under the MIT License. See the LICENSE file for more information.

## Acknowledgements

This Python API is based on the original NLM C++ framework and provides an enhanced Python interface with improved documentation, examples, and functionality.

## Version Information

Current version: 1.0.0
Python bindings version: 1.0.0
C++ core version: Phase 2

For more information, visit the NLM project website or documentation.