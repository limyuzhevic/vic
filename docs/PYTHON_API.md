# NLM Python Bindings - Complete Documentation

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python bindings. It covers both the core bindings and the enhanced API.

## Overview

The NLM Python bindings provide access to the full NLM neural simulation framework through Python. This allows users to:

- Create and simulate neural brains
- Configure simulation parameters
- Run experiments and demonstrations
- Analyze simulation results
- Integrate NLM with Python-based workflows

## Installation and Setup

### Prerequisites

1. **Build NLM with Python bindings:**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Install Python package:**
   ```bash
   pip install pynlm
   ```

   For development:
   ```bash
   pip install -e .
   ```

3. **Optional dependencies:**
   ```bash
   pip install matplotlib numpy scipy pandas
   ```

### Environment Setup

Create a Python environment for NLM:

```bash
python -m venv nlm_env
source nlm_env/bin/activate  # On Windows: nlm_env\Scripts\activate
pip install pynlm matplotlib numpy
```

## Basic Usage Examples

### 1. Hello World - Basic Neural Simulation

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain()
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Run a simple simulation
for step in range(100):
    brain.step(step, step * 0.001)
    
print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
```

### 2. Configuration and Customization

```python
import pynlm

# Create configuration with custom parameters
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000, pynlm.ConfigSource.Runtime)
config.set("region_count", 3, pynlm.ConfigSource.Runtime)
config.set("random_seed", 12345, pynlm.ConfigSource.Runtime)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Custom brain: {brain.getTotalNeuronCount()} neurons, {brain.getRegionCount()} regions")
```

### 3. Simulation with Input

```python
import pynlm

# Create world for agent-world interaction
world = pynlm.createSimpleWorld()
world.configure(64, 64, 16, 16)

# Create agent brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent_brain = pynlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Run interaction loop
for episode in range(5):
    print(f"\nEpisode {episode + 1}:")
    
    # Reset for new episode
    world.reset()
    agent_brain.reset()
    brain.reset()
    brain.initialize()
    
    # Run steps
    for step in range(50):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent_brain.processSensoryInput(percept)
        
        # Run neural simulation
        for neural_step in range(5):
            brain.step(neural_step, neural_step * 0.001)
        
        # Produce action
        action = brain.produceAction()
        if action:
            print(f"  Step {step}: Action {action.getType()}")
        
        # Update world
        world.update(0.01)
```

### 4. Advanced Usage with Memory Systems

```python
import pynlm

# Create brain with memory systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Store experiences
working_memory.storeExperience(42.0f, "initial_learning")
episodic_memory.recordExperience(42.0f, [1.0, 2.0, 3.0], "test_experience")
associative_memory.associate("hunger", "seek_food")

# Run simulation with memory integration
for step in range(200):
    brain.step(step, step * 0.001)
    
    # Inject learning signals
    if step % 20 == 0:
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0f)

print(f"Memory systems integrated. Total spikes: {brain.getTotalSpikeCount()}")
```

## Enhanced Python API

The enhanced API provides additional functionality and a more user-friendly interface:

### Quick Start

```python
# Import enhanced API
from pynlm_enhanced import NLM, Config, create_demo_simulation

# Create NLM instance
nlm = NLM()

# Create configuration with template
config = nlm.create_config("learning", neuron_count=500)

# Create brain
brain = nlm.create_brain(config)
brain.initialize()

# Run basic simulation
stats = nlm.run_basic_simulation(100)
print(f"Simulation completed: {stats['total_steps']} steps")
```

### Configuration Management

```python
from pynlm_enhanced import Config, get_template

# Create default configuration
config = Config()

# Apply template
config.apply_template("research")

# Customize parameters
config.set("neuron_count", 2000)
config.set("enable_curiosity", True)

# Save configuration
config.save("my_experiment.cfg")

# Load configuration
loaded_config = Config.load("my_experiment.cfg")
```

### Simulation Management

```python
from pynlm_enhanced import NLM

# Create NLM with custom configuration
nlm = NLM()
nlm.config.apply_template("advanced")

# Run world interaction simulation
world_stats = nlm.run_world_interaction(episodes=10, steps_per_episode=200)

# Run learning simulation
learning_stats = nlm.run_learning_simulation(episodes=5, steps_per_episode=100)

# Save simulation state
nlm.save_simulation("experiment_state.nlm")

# Load simulation state
nlm.load_simulation("experiment_state.nlm")
```

### Context Management

```python
from pynlm_enhanced import NLM

# Use NLM as context manager (automatic cleanup)
with NLM() as nlm:
    brain = nlm.create_brain()
    brain.initialize()
    
    # Run simulation
    stats = nlm.run_basic_simulation(50)
    
    # Brain automatically cleaned up when exiting context

# NLM resources are now cleaned up
```

## API Reference

### Core Classes

#### Config

Configuration management system for NLM parameters.

**Methods:**
- `createDefaultConfig()` → Config: Create default configuration
- `set(key, value, source=ConfigSource.Runtime)` → Config: Set configuration value
- `get(key)` → optional[T]: Get configuration value with type
- `getOr(key, default)` → T: Get with default value
- `has(key)` → bool: Check if key exists
- `remove(key)` → void: Remove configuration key
- `getKeys()` → vector[str]: Get all configuration keys
- `summary()` → str: Get configuration summary string
- `save(filepath)` → void: Save configuration to file
- `load(filepath)` → Config: Load configuration from file

#### Brain

Central neural simulation brain class.

**Methods:**
- `initialize()` → bool: Initialize brain with configuration
- `step(step, time)` → void: Perform simulation step
- `receiveSensoryInput(input)` → void: Inject sensory input
- `injectCurrent(neuron, current)` → void: Inject current to specific neuron
- `injectCurrentToNeurons(type, current)` → void: Inject to all neurons of type
- `produceAction()` → Action: Generate motor action
- `reset()` → void: Reset brain state
- `save(filepath)` → bool: Save brain state to file
- `load(filepath)` → bool: Load brain state from file
- `getTotalNeuronCount()` → size_t: Get total neuron count
- `getTotalSynapseCount()` → size_t: Get total synapse count
- `getActiveNeuronCount()` → size_t: Get active neuron count
- `getFiringNeuronCount()` → size_t: Get firing neuron count
- `getAverageFiringRate()` → float: Get average firing rate
- `getExcitationInhibitionRatio()` → float: Get E/I ratio
- `getTotalSpikeCount()` → size_t: Get total spike count
- `getDevelopmentalStage()` → DevelopmentalStage: Get developmental stage
- `logStatus()` → void: Log brain status

#### World

Simple world simulation environment.

**Methods:**
- `configure(width, height, vision_width, vision_height)` → void: Configure world dimensions
- `setAgentStart(x, y)` → void: Set agent starting position
- `update(timestep)` → void: Update world state
- `applyMotorCommand(command, time)` → ActionResult: Apply motor command
- `getSensoryPercept()` → SensoryPercept: Get sensory percept
- `getAgentBody()` → AgentBody: Get agent body state
- `addObject(obj)` → void: Add world object
- `removeObject(x, y)` → void: Remove object at position

#### Agent

Agent brain interface connecting NLM brain to world.

**Methods:**
- `initialize(world)` → void: Initialize with world
- `processSensoryInput(percept)` → void: Process sensory percept
- `decodeMotorCommand()` → Action: Decode brain activity to motor command
- `applyRewardModulation(reward, predicted_reward)` → void: Apply reward-based modulation
- `updateDevelopment(timestep)` → void: Update development system
- `enableRewardModulation(enable)` → void: Enable/disable reward modulation
- `enableStructuralPlasticity(enable)` → void: Enable/disable structural plasticity
- `enableDevelopment(enable)` → void: Enable/disable development
- `enableCuriosity(enable)` → void: Enable/disable curiosity
- `getDevelopmentalStage()` → DevelopmentalStage: Get developmental stage
- `getNeuromodulationLevel()` → float: Get neuromodulation level
- `getCuriosityLevel()` → float: Get curiosity level
- `getNoveltyLevel()` → float: Get novelty level
- `getPredictionError()` → float: Get prediction error

### Enumerations

#### NeuronType
- `Excitatory`: Excitatory neurons
- `Inhibitory`: Inhibitory neurons  
- `Modulatory`: Modulatory neurons
- `Sensory`: Sensory neurons
- `Motor`: Motor neurons
- `Internal`: Internal neurons

#### ActionType
- `MoveForward`, `MoveBackward`: Basic movements
- `TurnLeft`, `TurnRight`: Rotational movements
- `Look`, `LookUp`, `LookDown`: Gazing directions
- `Interact`, `Eat`, `Drink`: Interaction actions
- `Rest`, `Wait`: Inactivity actions
- `Custom`: Custom action type

#### WorldObjectType
- `Empty`: Empty space
- `Resource`: Resource object (food, water, etc.)
- `Hazard`: Hazardous object
- `Wall`: Wall/obstacle
- `Marker`: Marker object

#### DevelopmentalStage
- `Initial`: Initial development stage
- `CriticalPeriod`: Critical period for plasticity
- `Maturation`: Maturation phase
- `Adult`: Adult stage
- `Aging`: Aging phase

### Configuration Templates

The enhanced API provides predefined configuration templates:

#### "basic"
- neuron_count: 500
- region_count: 2
- connection_probability: 0.15
- simulation_timestep: 0.001
- random_seed: 42

#### "learning"
- neuron_count: 1000
- region_count: 3
- connection_probability: 0.2
- simulation_timestep: 0.001
- random_seed: 123
- enable_curiosity: true

#### "research"
- neuron_count: 5000
- region_count: 5
- connection_probability: 0.25
- simulation_timestep: 0.0002
- random_seed: 999
- enable_curiosity: true

## Error Handling

### Common Exceptions

#### ValidationError
Raised when configuration validation fails.

```python
try:
    config = Config({"neuron_count": -1})
except ValidationError as e:
    print(f"Invalid configuration: {e}")
```

#### NLMError
Base class for NLM-specific errors.

#### NLMSimulationError
Raised when simulation fails.

#### NLMResourceError
Raised when resource management fails.

#### NLMEnvironmentError
Raised when environment setup fails.

### Error Handling Best Practices

```python
import pynlm_enhanced

try:
    # Create brain with configuration
    config = pynlm_enhanced.Config({"neuron_count": 100})
    brain = pynlm_enhanced.EnhancedBrain(config)
    brain.initialize()
    
    # Run simulation
    stats = brain.run_simulation(100)
    
except pynlm_enhanced.ValidationError as e:
    print(f"Configuration error: {e}")
except pynlm_enhanced.NLMSimulationError as e:
    print(f"Simulation error: {e}")
except pynlm_enhanced.NLMResourceError as e:
    print(f"Resource error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Tips

### Configuration Optimization

```python
# Use efficient configuration loading
config = pynlm_enhanced.Config.load("optimized_config.cfg")

# Set appropriate parameters for your use case
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)
config.set("random_seed", 42)
```

### Memory Management

```python
# Explicit cleanup
brain.cleanup()
world.cleanup()
agent.cleanup()

# Or use context managers
with pynlm_enhanced.EnhancedBrain(config) as brain:
    # Brain automatically cleaned up
    pass
```

### Simulation Control

```python
# Simulate in batches to monitor progress
batch_size = 50
for batch in range(total_batches):
    start_time = time.time()
    
    for step in range(batch_size):
        brain.step(current_step, current_time)
        current_step += 1
        current_time += config.get("simulation_timestep", 0.001)
    
    elapsed = time.time() - start_time
    print(f"Batch {batch} completed in {elapsed:.2f} seconds")
```

## Advanced Features

### Custom Sensory Input

```python
import pynlm

class CustomSensory(pynlm.SensoryInput):
    def __init__(self, data):
        super().__init__()
        self._data = data
    
    def getType(self):
        return "Custom"
    
    def getData(self):
        return self._data
    
    def getDimensions(self):
        return len(self._data)

# Use custom sensory input
sensory = CustomSensory([0.1, 0.5, 0.9, 0.2])
brain.receiveSensoryInput(sensory)
```

### Multi-threaded Simulation

```python
import pynlm_enhanced
import threading
import queue

def simulation_worker(brain, steps, results_queue):
    """Worker function for multi-threaded simulation"""
    try:
        for step in range(steps):
            brain.step(step, step * 0.001)
            time.sleep(0.001)  # Small delay
        results_queue.put(("success", brain.getTotalSpikeCount()))
    except Exception as e:
        results_queue.put(("error", str(e)))

# Multi-threaded simulation
config = pynlm_enhanced.Config({"neuron_count": 500})
brain = pynlm_enhanced.EnhancedBrain(config)
brain.initialize()

# Create multiple worker threads
num_threads = 4
steps_per_thread = 100
results_queue = queue.Queue()

threads = []
for i in range(num_threads):
    thread = threading.Thread(target=simulation_worker, 
                            args=(brain, steps_per_thread, results_queue))
    threads.append(thread)
    thread.start()

# Wait for all threads to complete
for thread in threads:
    thread.join()

# Collect results
print("Simulation results:")
while not results_queue.empty():
    status, result = results_queue.get()
    print(f"  {status}: {result}")
```

### Visualization (requires matplotlib)

```python
import pynlm_enhanced
import matplotlib.pyplot as plt
import numpy as np

def visualize_brain_dynamics(brain, steps=500):
    """Visualize brain dynamics over time"""
    # Collect metrics
    spike_counts = []
    firing_rates = []
    excitation_inhibition_ratios = []
    
    for step in range(steps):
        brain.step(step, step * 0.001)
        
        spike_counts.append(brain.brain.getTotalSpikeCount())
        firing_rates.append(brain.brain.getAverageFiringRate())
        excitation_inhibition_ratios.append(brain.brain.getExcitationInhibitionRatio())
        
        # Inject random input to drive dynamics
        if np.random.random() < 0.1:  # 10% chance of input
            brain.inject_current(pynlm.NeuronType.Sensory, 10.0)
    
    # Create subplots
    fig, axes = plt.subplots(3, 1, figsize=(12, 10))
    
    # Spike count over time
    axes[0].plot(spike_counts, 'b-')
    axes[0].set_title('Total Spike Count Over Time')
    axes[0].set_xlabel('Simulation Step')
    axes[0].set_ylabel('Spike Count')
    axes[0].grid(True, alpha=0.3)
    
    # Firing rate over time
    axes[1].plot(firing_rates, 'r-')
    axes[1].set_title('Average Firing Rate Over Time')
    axes[1].set_xlabel('Simulation Step')
    axes[1].set_ylabel('Firing Rate (Hz)')
    axes[1].grid(True, alpha=0.3)
    
    # Excitatory/Inhibitory ratio
    axes[2].plot(excitation_inhibition_ratios, 'g-')
    axes[2].set_title('Excitatory/Inhibitory Ratio Over Time')
    axes[2].set_xlabel('Simulation Step')
    axes[2].set_ylabel('E/I Ratio')
    axes[2].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.show()

# Usage
config = pynlm_enhanced.Config({"neuron_count": 300})
brain = pynlm_enhanced.EnhancedBrain(config)
brain.initialize()

visualize_brain_dynamics(brain, steps=300)
```

## Demo System

The NLM demo system provides pre-configured demonstrations:

### Quick Demo

```bash
python3 quick_demo.py
```

This runs a comprehensive demonstration of NLM capabilities.

### Modular Demo System

```bash
python3 nlm_demo.py --mode basic --steps 200
python3 nlm_demo.py --mode intermediate --episodes 10
python3 nlm_demo.py --mode advanced --steps 500
python3 nlm_demo.py --mode world --episodes 5 --steps 100
```

### Demo Modes

1. **Basic**: Simple neural simulation with basic features
2. **Intermediate**: Adds world interaction and agent interface
3. **Advanced**: Includes learning, development, and curiosity
4. **World**: Focus on agent-world interaction
5. **Learning**: Demonstrates learning across multiple episodes

## Configuration Files

### Example Configuration Files

Configuration files are stored in the `configs/` directory:

- `basic_config.cfg`: Basic simulation configuration
- `example_config.md`: Configuration file documentation

### Configuration File Format

Configuration files are JSON format:

```json
{
    "nlm_version": "enhanced_1.0",
    "config": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.15,
        "simulation_timestep": 0.001,
        "random_seed": 42,
        "enable_plasticity": true,
        "enable_neuromodulation": true,
        "enable_development": true,
        "enable_curiosity": true,
        "world_width": 64,
        "world_height": 64
    }
}
```

### Loading Configuration

```python
import pynlm_enhanced

# Load configuration from file
config = pynlm_enhanced.Config.load("my_config.cfg")

# Create NLM with loaded configuration
nlm = pynlm_enhanced.NLM()
```

## Examples Directory

The `examples/` directory contains additional examples:

- `examples/nlm_python_examples.py`: Comprehensive Python examples
- `examples/`: Additional example scripts

## Testing

### Running Tests

Run the test suite with:

```bash
python3 test_nlm_bindings.py
python3 test_nlm_bindings.py --quick
python3 test_nlm_bindings.py --verbose
```

### Test Categories

1. **Configuration tests**: Configuration validation and management
2. **Brain tests**: Enhanced brain functionality
3. **World tests**: Enhanced world functionality
4. **Agent tests**: Enhanced agent functionality
5. **Integration tests**: Component integration
6. **Performance tests**: Performance characteristics

## Troubleshooting

### Common Issues

#### "ModuleNotFoundError: No module named 'pynlm'"
**Solution:** Build NLM with Python bindings:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install pynlm
```

#### "ImportError: cannot import name 'pynlm_enhanced'"
**Solution:** Check if the enhanced API file exists:
```bash
ls -la python/
```

#### Simulation Too Slow
**Solution:** Reduce complexity:
- Decrease neuron count
- Increase time step
- Use simpler configuration

### Debug Mode

Enable debug output:

```python
import pynlm_enhanced
import logging

# Enable logging
logging.basicConfig(level=logging.DEBUG)

# Create and run simulation
config = pynlm_enhanced.Config({"neuron_count": 100})
brain = pynlm_enhanced.EnhancedBrain(config)
brain.initialize()

# Add debug prints
stats = brain.run_simulation(50)
```

## Migration Guide

### From Old API to Enhanced API

#### Old API
```python
import pynlm

# Basic usage
brain = pynlm.createBrain()
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Manual cleanup
brain.reset()
```

#### Enhanced API
```python
from pynlm_enhanced import NLM, Config

# Enhanced usage
nlm = NLM()
config = nlm.create_config("basic")
brain = nlm.create_brain()

# Automatic cleanup
with NLM() as nlm:
    brain = nlm.create_brain()
    # Brain automatically cleaned up
```

### Configuration Migration

```python
# Old configuration
config = {"neuron_count": 500, "region_count": 2}

# New configuration (with defaults)
config = Config({"neuron_count": 500, "region_count": 2})

# Apply template
config.apply_template("basic")
```

## Support and Resources

### Documentation

- **Main Documentation**: `docs/` directory
- **Python API**: `docs/PYTHON_API.md`
- **Architecture**: `docs/ARCHITECTURE.md`
- **Experiments**: `docs/EXPERIMENTS.md`

### Examples

- **Quick Demo**: `quick_demo.py`
- **Modular Demo**: `nlm_demo.py`
- **Comprehensive Examples**: `examples/nlm_python_examples.py`

### Support

- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Community Forum**: https://community.nlm.org
- **Documentation**: https://nlm.readthedocs.io

### Contributing

- **Pull Requests**: https://github.com/nlm-project/nlm/pulls
- **Code Style**: Follow NLM coding standards
- **Testing**: Add comprehensive tests for new features

## Version Information

```python
import pynlm_enhanced

print(f"NLM Enhanced API Version: {pynlm_enhanced.__version__}")
print(f"NLM Core Version: {pynlm.__version__}")
```

## License

This documentation is licensed under the MIT License. The NLM framework is part of the NLM project.

For more information, see the LICENSE file in the project root.