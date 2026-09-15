# NLM Python API Documentation

## Overview

This document provides comprehensive documentation for the improved NLM Python bindings. The Python API has been significantly enhanced to provide both a simple, beginner-friendly interface and powerful advanced features for research and experimentation.

## Key Improvements

### 1. Pythonic API Design
- **Simple interface** for getting started with minimal code
- **Advanced features** for research and experimentation
- **Consistent naming conventions** (snake_case for functions, CamelCase for classes)
- **Comprehensive documentation** with examples

### 2. Enhanced Error Handling
- Custom exception hierarchy for different error types
- Better error messages with context
- Graceful failure handling

### 3. Convenience Functions
- High-level abstractions for common tasks
- Simulation management
- Monitoring and visualization
- Comparison and benchmarking tools

### 4. Advanced Features
- Development tracking
- Multiple configuration presets
- Visualization utilities
- Batch processing capabilities

## Installation

### From Source (Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy matplotlib

# Install in development mode
pip install -e .

# Or install the package
pip install .
```

### Verification

```python
import nlm
print(f"NLM version: {nlm.__version__}")

# Quick test
brain = nlm.create_brain()
brain.initialize()
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

## Getting Started Guide

### The Simplest Possible Example

```python
import nlm

# Create and initialize brain
brain = nlm.create_brain()
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent in World Simulation

```python
import nlm

# Setup configuration and brain
config = nlm.create_default_config()
brain = nlm.create_brain(config)
brain.initialize()

# Create agent and world
agent = nlm.create_agent(brain)
world = nlm.create_simple_world()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all subsystems for learning
nlm.enable_all_features(agent)

# Run episode
stats = nlm.run_episode(brain, world, agent, max_steps=1000)
print(f"Episode completed: {stats['steps']} steps")
```

## Core Classes

### Brain Class

The `Brain` class is the central neural simulation engine. It handles:
- LIF neuron dynamics
- Synaptic connections and plasticity
- Memory systems
- Neuromodulation

#### Basic Usage

```python
import nlm

# Create configuration
config = nlm.create_default_config()

# Create and initialize brain
brain = nlm.create_brain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f}")
```

#### Key Methods

| Method | Description |
|--------|-------------|
| `initialize()` | Initialize brain with configuration |
| `step(step_number)` | Run one simulation step |
| `step(step_number, time)` | Run step with timestamp |
| `reset()` | Reset brain state |
| `receiveSensoryInput(input)` | Inject sensory data |
| `produceAction()` | Generate motor action |
| `save("checkpoint.bin")` | Save brain state |
| `load("checkpoint.bin")` | Load brain state |
| `getTotalNeuronCount()` | Get total neuron count |
| `getFiringNeuronCount()` | Get currently firing neurons |

### AgentBrain Class

The `AgentBrain` class connects the neural brain to the world simulation:
- Sensory transduction (world → neural)
- Motor decoding (neural → world)
- Reward modulation
- Development tracking

#### Basic Usage

```python
import nlm

# Create brain and agent
brain = nlm.create_brain()
agent = nlm.create_agent(brain)

# Create and configure world
world = nlm.create_simple_world()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent
agent.initialize(world)

# Process sensory input and decode motor commands
for step in range(100):
    world.update(0.1)
    percept = world.get_sensory_percept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
```

### SimpleWorld Class

The `SimpleWorld` class provides a 2D simulation environment:
- Agent positioning and movement
- Sensory inputs (vision, touch, internal, proprioception)
- Action execution
- World objects

#### Basic Usage

```python
import nlm

# Create world
world = nlm.create_simple_world()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Set agent start position
world.set_agent_start(10.0, 10.0)

# Update world
for step in range(100):
    world.update(0.1)
    percept = world.get_sensory_percept()
    # Use percept for decision making
```

### Config Class

The `Config` class manages brain configuration:
- Neural parameters
- Plasticity rules
- Neuromodulation settings
- File I/O

#### Basic Usage

```python
import nlm

# Create default config
config = nlm.create_default_config()

# Modify parameters
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

# Save/Load
config.saveToFile("config.json")
config2 = nlm.create_default_config()
config2.loadFromFile("config.json")
```

## Enumerations

### NeuronType
```python
import nlm

# Neuron types
pynlm.NeuronType.Excitatory      # Excitatory neurons
pynlm.NeuronType.Inhibitory      # Inhibitory neurons
pynlm.NeuronType.Sensory         # Sensory neurons
pynlm.NeuronType.Motor           # Motor neurons
pynlm.NeuronType.Modulatory      # Modulatory neurons
pynlm.NeuronType.Internal        # Internal neurons
```

### DevelopmentalStage
```python
import nlm

# Developmental stages
pynlm.DevelopmentalStage.Initial          # Initial state
pynlm.DevelopmentalStage.CriticalPeriod   # Critical learning period
pynlm.DevelopmentalStage.Maturation       # Maturation phase
pynlm.DevelopmentalStage.Adult            # Adult state
pynlm.DevelopmentalStage.Aging            # Aging phase
```

### MotorCommand
```python
import nlm

# Motor commands
pynlm.MotorCommand.MoveForward   # Move forward
pynlm.MotorCommand.MoveBackward  # Move backward
pynlm.MotorCommand.TurnLeft      # Turn left
pynlm.MotorCommand.TurnRight     # Turn right
pynlm.MotorCommand.Interact      # Interact with world
pynlm.MotorCommand.Wait          # Wait/do nothing
```

## Advanced Features

### 1. Episode Running

The `run_episode` function provides a high-level interface for complete simulations:

```python
import nlm

# Setup
brain = nlm.create_brain()
world = nlm.create_simple_world()
agent = nlm.create_agent(brain)

# Configure world
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
world.reset()
agent.initialize(world)

# Enable learning
nlm.enable_all_features(agent)

# Run episode
stats = nlm.run_episode(brain, world, agent, max_steps=1000)
print(f"Episode completed in {stats['steps']} steps")
print(f"Final firing rate: {stats['avg_firing_rate']:.2f}")
print(f"Final curiosity: {stats['curiosity_level']:.3f}")
```

### 2. Brain Monitoring

Monitor brain activity over time:

```python
import nlm

brain = nlm.create_brain()
brain.initialize()

# Monitor for 100 steps
monitor_data = nlm.monitor_brain(brain, interval=10, duration=100)

# Analyze results
for snapshot in monitor_data:
    print(f"Step {snapshot['step']}: {snapshot['firing_rate']:.2f} Hz")
```

### 3. Brain Comparison

Compare two brains side by side:

```python
import nlm

# Create two brains with different configurations
config1 = nlm.create_minimal_config()
brain1 = nlm.create_brain(config1)
brain1.initialize()

config2 = nlm.create_advanced_config()
brain2 = nlm.create_brain(config2)
brain2.initialize()

# Compare them
comparison = nlm.compare_brains(brain1, brain2)
print(f"Neuron count difference: {comparison['total_neurons']['difference']}")
print(f"Firing rate ratio: {comparison['average_firing_rate']['brain1'] / comparison['average_firing_rate']['brain2']:.2f}")
```

### 4. Visualization

Visualize brain state as text or JSON:

```python
import nlm

brain = nlm.create_brain()
brain.initialize()

# Get text visualization
text_viz = nlm.visualize_brain(brain, format='text')
print(text_viz)

# Get JSON representation
json_viz = nlm.visualize_brain(brain, format='json')
# Save to file
nlm.visualize_brain(brain, format='json', output_file='brain_state.json')
```

### 5. Benchmarking

Benchmark brain performance:

```python
import nlm

brain = nlm.create_brain()
brain.initialize()

# Benchmark performance
bench_results = nlm.benchmark_brain(brain, duration=5.0, timestep=0.1)
print(f"Performance: {bench_results['steps_per_second']:.0f} steps/second")
```

### 6. Configuration Presets

Use different configuration presets:

```python
import nlm

# Minimal configuration for fast testing
config = nlm.create_minimal_config()
brain = nlm.create_brain(config)

# Advanced configuration for research
config = nlm.create_advanced_config()
brain = nlm.create_brain(config)
```

## Error Handling

The NLM Python bindings provide comprehensive error handling:

```python
import nlm

try:
    # Attempt to create brain with invalid configuration
    config = nlm.create_default_config()
    config.set("invalid.parameter", 999)  # Invalid parameter name
    brain = nlm.create_brain(config)
    brain.initialize()
except nlm.NLMConfigError as e:
    print(f"Configuration error: {e}")
except nlm.NLMInitializationError as e:
    print(f"Initialization error: {e}")
except nlm.NLMSimulationError as e:
    print(f"Simulation error: {e}")
except nlm.NLMBrainError as e:
    print(f"General NLM error: {e}")
```

### Common Errors

| Error Type | When it occurs | Solution |
|------------|----------------|----------|
| `NLMConfigError` | Invalid configuration parameters | Use valid parameter names from documentation |
| `NLMInitializationError` | Brain initialization failure | Check configuration validity, ensure dependencies are met |
| `NLMSimulationError` | Simulation failure | Check for valid inputs, handle exceptions in simulation loop |
| `NLMMemoryError` | Memory-related issues | Reduce brain size or increase available memory |
| `NLMInvalidActionError` | Invalid motor actions | Use valid `MotorCommand` values |

## Development Tools

### Helper Functions

The Python module provides several helper functions:

```python
import nlm

# Get version information
sys_info = nlm.get_version_info()
print(f"NLM version: {sys_info['package_version']}")

# Run a complete simulation (high-level convenience)
brain = nlm.create_brain()
world = nlm.create_simple_world()
agent = nlm.create_agent(brain)

nlm.run_simulation(brain, world, agent, num_steps=100)
print("Simulation completed successfully!")
```

### Statistics Collection

Collect comprehensive brain statistics:

```python
import nlm

brain = nlm.create_brain()
brain.initialize()

# Get brain stats as tuple
stats = nlm.get_brain_stats(brain)
(total_neurons, total_synapses, active_neurons, 
 firing_neurons, avg_firing_rate, total_spikes,
 ei_ratio, developmental_stage) = stats

print(f"Total neurons: {total_neurons}")
print(f"Average firing rate: {avg_firing_rate:.2f} Hz")
```

## Best Practices

### 1. Always Initialize Before Use
```python
brain = nlm.create_brain(config)
brain.initialize()  # Required!
brain.step(0)       # Now we can use it
```

### 2. Use Appropriate Configuration
```python
# For research: Use advanced configuration
config = nlm.create_advanced_config()

# For testing: Use minimal configuration  
config = nlm.create_minimal_config()

# For defaults: Use standard configuration
config = nlm.create_default_config()
```

### 3. Handle Exceptions
```python
try:
    # Your simulation code
    pass
except nlm.NLMBrainError as e:
    print(f"NLM error: {e}")
    # Handle error appropriately
```

### 4. Monitor Performance
```python
import nlm
import time

brain = nlm.create_brain()
brain.initialize()

start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

print(f"Performance: {(end_time - start_time)/1000:.3f} seconds per step")
```

### 5. Save/Load States
```python
import nlm

brain = nlm.create_brain()
brain.initialize()

# Save brain state
brain.save("checkpoint.bin")

# Continue simulation...

# Later, load saved state
brain2 = nlm.create_brain()
brain2.load("checkpoint.bin")
```

## Examples

### 1. Simple Brain without World

```python
import nlm

# Create and run simple brain
brain = nlm.create_brain()
brain.initialize()

for i in range(100):
    brain.step(i)
    if i % 20 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")
```

### 2. Complete Agent Environment

```python
import nlm

def create_agent_simulation(config_name="advanced"):
    """Create a complete agent simulation."""
    if config_name == "minimal":
        config = nlm.create_minimal_config()
    elif config_name == "advanced":
        config = nlm.create_advanced_config()
    else:
        config = nlm.create_default_config()
    
    # Create brain
    brain = nlm.create_brain(config)
    brain.initialize()
    
    # Create agent
    agent = nlm.create_agent(brain)
    
    # Create world
    world = nlm.create_simple_world()
    world.configure(width=25, height=25, visionWidth=9, visionHeight=9)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable all features
    nlm.enable_all_features(agent)
    
    return brain, agent, world

# Run simulation
brain, agent, world = create_agent_simulation()
stats = nlm.run_episode(brain, world, agent, max_steps=500)
print(f"Simulation complete! Final curiosity: {stats['curiosity_level']:.3f}")
```

### 3. Brain Development Tracking

```python
import nlm

# Create brain with development enabled
config = nlm.create_advanced_config()
brain = nlm.create_brain(config)
brain.initialize()

# Create agent
agent = nlm.create_agent(brain, enable_development=True)

# Run monitoring with development updates
world = nlm.create_simple_world()
world.configure(width=20, height=20)
world.reset()
agent.initialize(world)

for step in range(100):
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print developmental stage
    if step % 10 == 0:
        print(f"Step {step}, Stage: {brain.getDevelopmentalStage()}")
        
    # Normal simulation
    world.update(0.1)
    agent.processSensoryInput(world.get_sensory_percept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.get_simulation_time())
```

### 4. Advanced Usage with Custom Monitoring

```python
import nlm

brain = nlm.create_brain()
brain.initialize()

def custom_monitor(snapshot):
    """Custom monitoring callback."""
    if snapshot['firing_rate'] > 10.0:
        print(f"High activity at step {snapshot['step']}: {snapshot['firing_rate']:.2f} Hz")

# Use custom monitoring
monitor_data = nlm.monitor_brain(brain, interval=5, duration=50, callback=custom_monitor)
```

## Troubleshooting

### Common Issues and Solutions

| Problem | Symptoms | Solution |
|---------|----------|----------|
| "My brain isn't doing anything" | Zero neurons firing | Check that `brain.initialize()` is called first |
| "The agent isn't moving" | Motor commands all Wait | Check that `world.applyMotorCommand()` is called |
| "Everything is 0" | All neuron counts are 0 | Run more steps to allow brain to "warm up" |
| "ImportError: No module named 'pynlm'" | Module not found | `pip install .` or `pip install -e .` |
| "Segmentation fault" | Crash on import/use | Check for valid configuration, call `initialize()` |

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

## Advanced Features for Research

### 1. Custom Agent Brains

```python
import nlm

class CustomAgentBrain(nlm.AgentBrain):
    """Custom agent with specialized behavior."""
    
    def custom_process_sensory(self, percept):
        """Custom sensory processing."""
        # Custom implementation here
        super().processSensoryInput(percept)
        
    def custom_decode_motor(self):
        """Custom motor decoding."""
        # Custom implementation here
        return self.decodeMotorCommand()

# Use custom agent
brain = nlm.create_brain()
agent = CustomAgentBrain(brain)
```

### 2. Batch Processing

```python
import nlm

# Create multiple brains for batch processing
brains = []
for i in range(5):
    config = nlm.create_advanced_config()
    brain = nlm.create_brain(config)
    brain.initialize()
    brains.append(brain)

# Run batch simulation
for i, brain in enumerate(brains):
    for step in range(100):
        brain.step(step)
    print(f"Brain {i} completed")
```

### 3. Parameter Sweeps

```python
import nlm

# Parameter sweep for research
results = []
for learning_rate in [0.001, 0.01, 0.1]:
    for plasticity in [0.5, 1.0, 2.0]:
        config = nlm.create_advanced_config()
        config.set("learning_rate", learning_rate)
        config.set("plasticity", plasticity)
        
        brain = nlm.create_brain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(200):
            brain.step(step)
            
        results.append({
            'learning_rate': learning_rate,
            'plasticity': plasticity,
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount()
        })

# Analyze results
for result in results:
    print(f"LR={result['learning_rate']}, Plasticity={result['plasticity']}: "
          f"FR={result['final_firing_rate']:.2f}")
```

### 4. Export/Import Brain States

```python
import nlm

# Export brain state for analysis
brain = nlm.create_brain()
brain.initialize()

# Save brain state
brain.save("research_brain.bin")

# Export visual representation
text_repr = nlm.visualize_brain(brain, format='text')
with open("brain_text_representation.txt", 'w') as f:
    f.write(text_repr)

# Export JSON for analysis
json_repr = nlm.visualize_brain(brain, format='json')
with open("brain_state.json", 'w') as f:
    f.write(json_repr)
```

## Performance Tips

### 1. Use Appropriate Configuration
```python
# For maximum performance
config = nlm.create_minimal_config()

# For research accuracy  
config = nlm.create_advanced_config()

# For balanced performance
config = nlm.create_default_config()
```

### 2. Monitor Memory Usage
```python
import nlm
import psutil
import os

process = psutil.Process(os.getpid())

brain = nlm.create_brain()
brain.initialize()

print(f"Memory before simulation: {process.memory_info().rss / 1024 / 1024:.1f} MB")

for step in range(1000):
    brain.step(step)
    
print(f"Memory after simulation: {process.memory_info().rss / 1024 / 1024:.1f} MB")
```

### 3. Parallel Processing
```python
import nlm
from concurrent.futures import ThreadPoolExecutor

# Process multiple simulations in parallel
def run_simulation(sim_id):
    brain = nlm.create_brain()
    brain.initialize()
    
    for step in range(100):
        brain.step(step)
        
    return sim_id, brain.getAverageFiringRate()

# Run simulations in parallel
with ThreadPoolExecutor(max_workers=4) as executor:
    futures = [executor.submit(run_simulation, i) for i in range(10)]
    results = [future.result() for future in futures]

for sim_id, firing_rate in results:
    print(f"Simulation {sim_id}: {firing_rate:.2f} Hz")
```

## Future Enhancements

The Python bindings are actively developed. Future enhancements may include:

1. **Advanced Visualization**: Integration with matplotlib and other plotting libraries
2. **Neural Network Analysis**: Tools for analyzing neural connectivity patterns
3. **Machine Learning Integration**: Interfaces with popular ML frameworks
4. **Distributed Computing**: Support for multi-GPU and distributed simulations
5. **Web Interfaces**: Web-based visualization and control
6. **Real-time Visualization**: Live plotting of neural activity
7. **Custom Kernels**: Support for user-defined neural dynamics

## Support

For support, questions, or reporting issues:

1. Check the [NLM documentation](https://nlm.readthedocs.io)
2. Report issues on GitHub
3. Join the NLM research community
4. Check for updates in the release notes

## License

NLM Python Bindings are distributed under the MIT License. See LICENSE file for details.

---

*Last updated: [Current Date]*
*NLM Python Bindings v0.1.0*  
*For latest updates and documentation, visit the NLM project website.*