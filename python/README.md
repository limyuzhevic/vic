# NLM Python Bindings

## Overview

This directory contains Python bindings for the NLM (Neural Learning Machine) C++ neural simulation framework. The bindings provide a Python interface to the complete NLM system with an emphasis on ease of use for beginners and extensibility for advanced users.

## Installation

### Build from Source

1. **Ensure C++ build is complete**:
   ```bash
   # Build the C++ core library
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Install Python bindings**:
   ```bash
   cd python
   pip install -e .
   ```

### Prerequisites

- C++ compiler (g++ 11+ or clang++ 12+)
- CMake 3.16+
- Python 3.8+
- pybind11 (automatically installed via scikit-build)

## Quick Start

### Basic Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize a brain
brain = pynlm.createBrain(config)
brain.initialize()

# Make the brain think for 100 steps
for i in range(100):
    brain.step(i)

print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Example

```python
import pynlm

# Setup configuration and brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and configure it
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent that connects brain to world
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation loop
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyAction(action, world.getSimulationTime())

print("Simulation complete!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Developmental stage: {agent.getDevelopmentalStage()}")
```

## API Reference

### Core Types

- `NeuronId` - Unique identifier for neurons
- `SynapseId` - Unique identifier for synapses
- `RegionId` - Unique identifier for brain regions
- `PopulationId` - Unique identifier for neuron populations

### Enums

- `NeuronType` - Types of neurons (Excitatory, Inhibitory, Sensory, Motor, etc.)
- `SynapseType` - Types of synaptic connections
- `DevelopmentalStage` - Brain development stages
- `FiringState` - Neuron firing states
- `ActionType` - Types of actions/ behaviors
- `MotorCommand` - Low-level motor commands

### Core Classes

#### Config
- `createDefaultConfig()` - Create default configuration
- `config.set(key, value)` - Set configuration values
- `config.loadFromFile(filepath)` - Load from file
- `config.loadFromArgs(argc, argv)` - Load from command line

#### Brain
- `brain.initialize()` - Initialize brain
- `brain.step(step, time)` - Advance simulation
- `brain.getTotalNeuronCount()` - Get neuron count
- `brain.getFiringNeuronCount()` - Get firing neuron count
- `brain.getTotalSpikeCount()` - Get total spike count
- `brain.getAverageFiringRate()` - Get average firing rate
- `brain.save(filepath)` - Save brain state
- `brain.load(filepath)` - Load brain state

#### SimpleWorld
- `world.configure(width, height, visionWidth, visionHeight)` - Configure world
- `world.update(timestep)` - Update world
- `world.getSensoryPercept()` - Get sensory input
- `world.getAgentBody()` - Get agent body state
- `world.applyAction(action, time)` - Apply action

#### AgentBrain
- `agent.initialize(world)` - Initialize agent
- `agent.processSensoryInput(percept)` - Process sensory input
- `agent.decodeMotorCommand()` - Decode motor command
- `agent.enableRewardModulation(bool)` - Enable/disable reward modulation
- `agent.enableCuriosity(bool)` - Enable/disable curiosity
- `agent.enableDevelopment(bool)` - Enable/disable development

## High-Level API

### pynlm.NLMConfig
Configuration builder with fluent API:

```python
# Create default config
config = pynlm.NLMConfig.create_default()

# Create learning-optimized config
config = pynlm.NLMConfig.create_learning_config()

# Create simulation config
config = pynlm.NLMConfig.create_simulation_config(
    timestep=0.001,
    neurons=1000,
    regions=2
)
```

### pynlm.NLMNeuron
High-level neuron wrapper:

```python
# Access neuron properties
neuron = pynlm.NLMNeuron(brain)
print(f"ID: {neuron.id}")
print(f"Type: {neuron.type}")
print(f"Membrane potential: {neuron.membrane_potential} mV")
print(f"Is firing: {neuron.is_firing}")

# Control neuron
neuron.inject_current(10.0)
```

### pynlm.NLMWorld
World/simulation wrapper:

```python
world = pynlm.NLMWorld(world_instance)
world.configure(20, 20, 8, 8)
world.update(0.1)
percept = world.getPercept()
```

### pynlm.NLMEnvironment
Complete agent-environment interaction:

```python
env = pynlm.NLMEnvironment(brain, world)
step_result = env.run_step(0.1)
episode_data = env.run_episode(max_steps=1000, verbose=True)
stats = env.get_brain_stats()
```

### pynlm.NLMResearch
Research and experiment tools:

```python
# Create experiment
experiment = pynlm.NLMResearch.create_experiment(
    brain, world, "memory_test", max_steps=500
)

# Run experiment
results = experiment.run()
summary = experiment.get_summary()
```

## Convenience Functions

### Basic Functions

```python
# Create brain with custom parameters
brain = pynlm.create_brain(
    neuron_count=1000,
    region_count=2,
    connection_probability=0.2,
    random_seed=42
)

# Create default environment
brain, world, env = pynlm.create_default_environment()

# Run demonstrations
demo1 = pynlm.run_demo_silent_brain()
demo2 = pynlm.run_demo_brain_watching_world()
demo3 = pynlm.run_demo_complete_agent()
```

## Examples

### Silent Brain
```python
import pynlm

brain = pynlm.create_brain()
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Silent brain test complete: {brain.getTotalSpikeCount()} spikes")
```

### Brain Watching World
```python
import pynlm

brain = pynlm.create_brain()
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch for 50 steps
for i in range(50):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)

print(f"Watched world for 50 steps")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Complete Agent with Learning
```python
import pynlm

# Setup with learning configuration
config = pynlm.NLMConfig.create_learning_config()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation with periodic stats
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyAction(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

## Tutorials

### 1. Quick Start Tutorial
1. **Install NLM**: Follow installation instructions above
2. **Create your first brain**: Use the basic example
3. **Run a simple simulation**: Watch neurons firing
4. **Explore the world**: Add objects and see the agent interact
5. **Enable learning**: Try reward modulation and curiosity

### 2. Agent Behavior Tutorial
1. **Understand perception**: Learn how agents receive sensory input
2. **Design motor commands**: Create action sequences
3. **Implement learning**: Use neuromodulation systems
4. **Debug your agent**: Monitor brain states and statistics

### 3. Research Protocol Tutorial
1. **Create experiments**: Design controlled tests
2. **Analyze results**: Use built-in analysis tools
3. **Benchmark performance**: Compare system configurations
4. **Document findings**: Export and visualize results

## Advanced Usage

### Custom Configuration
```python
config = pynlm.Config()
config.set("random_seed", 12345)
config.set("simulation_timestep", 0.01)
config.set("neuron_count", 2000)
config.set("region_count", 3)
config.set("connection_probability", 0.25)
config.set("stdp_ltp_weight", 0.05)
config.set("synaptogenesis_rate", 0.0001)
```

### Multi-Agent Systems
```python
# Create multiple agents sharing the same brain
shared_brain = pynlm.create_brain()

agent1 = pynlm.createAgentBrain(shared_brain)
agent2 = pynlm.createAgentBrain(shared_brain)

# Initialize with different worlds
world1 = pynlm.createSimpleWorld()
world2 = pynlm.createSimpleWorld()

agent1.initialize(world1)
agent2.initialize(world2)

# Run parallel simulations
for step in range(100):
    # Update both worlds
    world1.update(0.1)
    world2.update(0.1)
    
    # Each agent processes its own percept
    agent1.processSensoryInput(world1.getSensoryPercept())
    agent2.processSensoryInput(world2.getSensoryPercept())
    
    # Shared brain computation
    shared_brain.step(step)
    
    # Different actions
    action1 = agent1.decodeMotorCommand()
    action2 = agent2.decodeMotorCommand()
    
    world1.applyAction(action1, world1.getSimulationTime())
    world2.applyAction(action2, world2.getSimulationTime())
```

### Performance Optimization
```python
# Enable performance features
brain.set("enable_vectorization", True)
brain.set("enable_parallel_processing", True)
brain.set("enable_memory_pools", True)

# Batch operations
brains = [pynlm.create_brain() for _ in range(10)]
for b in brains:
    b.initialize()

# Parallel simulation
import concurrent.futures

def run_simulation(brain):
    for step in range(100):
        brain.step(step)
    return brain.getTotalSpikeCount()

with concurrent.futures.ThreadPoolExecutor() as executor:
    results = list(executor.map(run_simulation, brains))
```

## Error Handling

```python
import pynlm

try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.NLMException as e:
    print(f"NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Installation Troubleshooting

### Common Issues

1. **C++ bindings not found**:
   ```bash
   # Ensure you're in the python directory
   cd python
   pip install -e .
   ```

2. **Missing CMake or compiler**:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential cmake
   
   # macOS
   xcode-select --install
   brew install cmake
   ```

3. **Python version mismatch**:
   ```bash
   # Ensure Python 3.8+ is installed
   python3 --version
   
   # Install if needed
   sudo apt-get install python3
   ```

4. **Pybind11 version issues**:
   ```bash
   # Upgrade pip and setuptools
   pip install --upgrade pip setuptools
   
   # Install with specific pybind11 version
   pip install pybind11>=2.11.0
   ```

## Building Documentation

The documentation is built using Sphinx. To build the documentation:

```bash
cd docs
make html
```

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Contributing

Contributions are welcome! Please follow the project's coding standards and submit pull requests.

## Contact

For questions, issues, or collaboration opportunities:
- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Documentation: https://nlm.readthedocs.io
- Discord/Slack: (if available)

## Acknowledgments

This project builds on decades of research in computational neuroscience and artificial intelligence.
