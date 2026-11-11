# NLM Python Module - README

## Overview

This is the Python wrapper module for the NLM (Neural Learning Machine) framework. It provides Python bindings to the complex C++ neural simulation framework, making it accessible to Python users while maintaining access to all the advanced functionality of the NLM brain model.

## Installation

### Prerequisites

1. **NLM C++ library**: The C++ neural simulation framework must be built and installed
   - Clone the NLM repository
   - Build with CMake: `mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4`
   - This creates the `pynlm` Python extension module

2. **Python environment**: Python 3.8 or higher

### Installation Steps

1. **Build NLM with Python bindings**:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(python3 -c "import sys; print(sys.executable)")
   make -j4
   ```

2. **Install the Python module**:
   ```bash
   pip install -e .
   ```

3. **Alternatively, use the built module directly**:
   ```bash
   cd /path/to/nlm/build
   python -c "import sys; sys.path.insert(0, '.'); import pynlm; print('Import successful')"
   ```

## Basic Usage

### 1. Import the module

```python
import pynlm
```

### 2. Create a Brain

```python
# Method 1: Using default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Method 2: Using custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("connection_probability", 0.15)
brain = pynlm.createBrain(config)
```

### 3. Initialize and Run the Brain

```python
# Initialize the brain
brain.initialize()

# Run for some steps
for step in range(100):
    brain.step(step)
    
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### 4. Create an Agent-World System

```python
# Create brain, world, and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(1000):
    world.update(0.001)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step * 0.001)
```

### 5. Use the High-Level Convenience Functions

```python
# Run a complete simulation automatically
brain_config = pynlm.createDefaultConfig()
brain_config.set("neuron_count", 500)
world_config = {"width": 20, "height": 20, "vision_width": 8, "vision_height": 8}

agent = pynlm.create_complete_agent(brain_config, world_config)
results = pynlm.run_simple_simulation(brain_config, world_config.get('world'), agent, 1000)

print("Simulation Results:")
print(f"  Final firing rate: {results['final_firing_rate']:.2f} Hz")
print(f"  Max firing rate: {results['max_firing_rate']:.2f} Hz")
print(f"  Total spikes: {results['total_spikes']}")
```

## Advanced Usage

### 1. Reward System Integration

```python
# The reward system is automatically integrated with the AgentBrain
agent = pynlm.createAgentBrain(brain)

# Access the reward system
reward_system = agent.rewardSystem
reward_system.setValue(1.0)  # Set current reward
reward_system.add(0.5)      # Add to accumulated reward
reward_system.reset()       # Reset all values

# Check reward statistics
print(f"Total accumulated: {reward_system.getTotalAccumulated()}")
print(f"Prediction error: {reward_system.getPredictionError()}")
print(f"Expected value: {reward_system.getExpectedValue()}")
print(f"Average history: {reward_system.getAverageHistory()}")
```

### 2. Memory System Access

```python
# Access memory systems through the brain
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Active traces: {working_memory.getActiveTraces()}")
    print(f"Capacity: {working_memory.getCapacity()}")
    print(f"Current size: {working_memory.getCurrentSize()}")

episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"Episode count: {episodic_memory.getEpisodeCount()}")
    if episodic_memory.getEpisodeCount() > 0:
        episode = episodic_memory.getRecentEpisodes(1)[0]
        print(f"Last episode reward: {episode.reward}")
```

### 3. Neuromodulation Systems

```python
# Access neuromodulation systems
dopamine = brain.getDopamine()
if dopamine:
    print(f"Dopamine level: {dopamine.getLevel()}")
    print(f"Plasticity factor: {dopamine.getPlasticityFactor()}")

curiosity = brain.getCuriosity()
if curiosity:
    print(f"Curiosity level: {curiosity.getCuriosityLevel()}")

novelty = brain.getNovelty()
if novelty:
    print(f"Novelty level: {novelty.getNoveltyLevel()}")
```

### 4. Development System

```python
# Access development system
development = brain.getDevelopmentSystem()
if development:
    print(f"Current stage: {brain.getDevelopmentalStage().name}")
    
# Set development stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
print(f"Updated stage: {brain.getDevelopmentalStage().name}")
```

### 5. Checkpointing

```python
# Save and load checkpoints
brain.save("my_checkpoint.bin")

# Create new brain and load
new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
new_brain.initialize()
new_brain.load("my_checkpoint.bin")

print(f"Restored brain with {new_brain.getTotalNeuronCount()} neurons")
```

## Python Types and Enums

### Neuron Types

- `pynlm.NeuronType.Excitatory`
- `pynlm.NeuronType.Inhibitory`
- `pynlm.NeuronType.Modulatory`
- `pynlm.NeuronType.Sensory`
- `pynlm.NeuronType.Motor`
- `pynlm.NeuronType.Internal`

### Synapse Types

- `pynlm.SynapseType.Excitatory`
- `pynlm.SynapseType.Inhibitory`
- `pynlm.SynapseType.Modulatory`
- `pynlm.SynapseType.Electrical`
- `pynlm.SynapseType.GapJunction`

### Developmental Stages

- `pynlm.DevelopmentalStage.Initial`
- `pynlm.DevelopmentalStage.CriticalPeriod`
- `pynlm.DevelopmentalStage.Maturation`
- `pynlm.DevelopmentalStage.Adult`
- `pynlm.DevelopmentalStage.Aging`

### ID Classes

- `pynlm.NeuronId`: Unique identifier for neurons
- `pynlm.SynapseId`: Unique identifier for synapses
- `pynlm.RegionId`: Unique identifier for brain regions
- `pynlm.PopulationId`: Unique identifier for neuron populations

## Error Handling

The module provides custom exception classes:

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
```

## Module Information

### Version
```python
import pynlm
print(pynlm.get_version())
# Output: "0.1.0 (Phase 6 integration)"
```

### Available Features
```python
import pynlm
features = pynlm.list_available_features()
for feature in features:
    print(f"✓ {feature}")
```

## Examples

### Simple Pattern Learning

```python
import pynlm

# Create an experimental brain
brain = pynlm.create_experimental_brain(neuron_count=200, connection_probability=0.1)
brain.initialize()

# Create a world and agent
world = pynlm.create_experimental_world(15, 15, 8, 8)
agent = pynlm.create_complete_agent(pynlm.create_default_config())

# Run learning simulation
print("Running pattern learning simulation...")
for step in range(500):
    world.update(0.001)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step * 0.001)

print(f"Learning completed after {step} steps")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Advanced Agent Behavior

```python
import pynlm
import random

# Create a more sophisticated simulation
config = pynlm.create_default_config()
config.set("neuron_count", 1000)
world = pynlm.create_simple_world(30, 30, 16, 16)
agent = pynlm.create_complete_agent(config, world)

# Enable all advanced features
agent.enableStructuralPlasticity(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run with detailed monitoring
print("Starting advanced simulation...")
simulation_results = []

for step in range(2000):
    # Update world
    world.update(0.001)
    
    # Get perception
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain_step = step
    brain.step(brain_step)
    
    # Get action with curiosity
    action = agent.decodeMotorCommand()
    
    # Apply action and get reward
    action_result = world.applyMotorCommand(action, brain_step * 0.001)
    
    # Record statistics
    if step % 100 == 0:
        sim_data = {
            'step': step,
            'firing_rate': brain.getAverageFiringRate(),
            'spikes': brain.getTotalSpikeCount(),
            'reward': action_result.reward,
            'dopamine': agent.getNeuromodulationLevel(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel()
        }
        simulation_results.append(sim_data)
        
        print(f"Step {step}: FR={sim_data['firing_rate']:.2f} Hz, "
              f"Spikes={sim_data['spikes']}, "
              f"Reward={sim_data['reward']:.2f}, "
              f"DA={sim_data['dopamine']:.2f}, "
              f"Curiosity={sim_data['curiosity']:.2f}")

print("\nSimulation complete!")
print(f"Final statistics:")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
print(f"  Final dopamine level: {agent.getNeuromodulationLevel():.2f}")
print(f"  Final curiosity level: {agent.getCuriosityLevel():.2f}")
```

## Troubleshooting

### Common Issues

**Issue 1: Module not found**
```bash
# Check if the module is in the Python path
python -c "import pynlm; print('Import successful')"

# If not, build the module first
cd /path/to/nlm/build
python -c "import sys; sys.path.insert(0, '.'); import pynlm; print('Import successful')"
```

**Issue 2: C++ extension not compiled**
```bash
# Build the C++ extension
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

**Issue 3: Python version mismatch**
```bash
# Check Python version
python --version
# Should be 3.8 or higher
```

**Issue 4: Missing dependencies**
```bash
# Install Python dependencies
pip install -r requirements.txt
```

### Debug Mode

```python
import pynlm

# Enable debug logging
import logging
logging.basicConfig(level=logging.DEBUG)

# Create and run a simple simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
print("Debug simulation complete")
```

## Files Included

This module includes:

- `pynlm.py`: Main Python wrapper module
- `README.md`: This documentation file
- `examples/`: Example scripts and use cases
- `tests/`: Unit tests for the Python interface

## Support and Resources

### Documentation

- [NLM Architecture Documentation](./docs/ARCHITECTURE.md)
- [Phase 6 Integration Audit](./docs/PHASE6_FINAL_AUDIT.md)
- [Science and Research Documentation](./docs/SCIENCE.md)

### GitHub Repository

For additional documentation, examples, and contributions, visit:
https://github.com/nlm-project/nlm

### Issue Tracking

For bugs and feature requests:
- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Feature requests: https://github.com/nlm-project/nlm/discussions

### Contact

For technical support:
- NLM Development Team: nlm-support@research.org
- Documentation: nlm-docs@research.org

## License

MIT License - See LICENSE file for details.

---

*This module is part of the NLM (熙然) project - an experimental artificial developmental brain framework. The Python module provides an accessible interface to the complex C++ neural simulation capabilities while maintaining full access to all advanced features.*
