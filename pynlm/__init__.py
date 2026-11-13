# NLM - Python Neural Learning Machine Package

## Version

This module version 0.1.0, built from the NLM C++ neural simulation framework.

## Overview

The pynlm package provides Python bindings for the NLM (熙然) neural simulation framework.
It enables users to create, simulate, and interact with virtual brains through an intuitive Python API.

## Key Features

- **Brain Simulation**: Create and simulate neural networks with learning capabilities
- **Environment Integration**: Simulate agents interacting with dynamic environments
- **Sensory-Motor Loop**: Complete perception-action cycles for autonomous agents
- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Reward-based learning, curiosity-driven exploration
- **Development**: Age-dependent plasticity and brain maturation

## Main Classes

### Brain (Central Neural Simulation)
The `pynlm.Brain` class provides the core neural simulation capabilities:

```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step)
action = brain.produceAction()
```

### Config (Configuration Management)
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
```

### SimpleWorld (Environment)
```python
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
percept = world.getSensoryPercept()
```

### AgentBrain (Agent Interface)
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, time)
```

## Usage Examples

See `examples/` directory for comprehensive examples, or `easy_usage.md` for beginner-friendly tutorials.

## Installation

This package is built from source using scikit-build. To build and install:

```bash
mkdir build
cd build
cmake ..
make install
```

Or use pip to install from source:

```bash
pip install .
```

## Module Structure

```
pynlm/
├── __init__.py              # Core module exports
├── config.py                # Pythonic Config wrapper
├── brain.py                 # Enhanced Brain API
├── world.py                 # World simulation interface
├── agent.py                 # AgentBrain wrapper
├── examples/                # Complete example scripts
├── utils.py                 # Helper utilities
└── typing.py                # Type definitions and hints
```

## Advanced Features

### Memory Systems
- Working memory: `brain.getWorkingMemory()`
- Episodic memory: `brain.getEpisodicMemory()`
- Associative memory: `brain.getAssociativeMemory()`

### Neuromodulation
- Reward modulation: `agent.applyRewardModulation(reward, predicted)`
- Curiosity level: `agent.getCuriosityLevel()`
- Novelty detection: `agent.getNoveltyLevel()`

### Prediction Systems
- Prediction system: `brain.getPredictionSystem()`
- Prediction error: `agent.getPredictionError()`

### Development
- Developmental stage: `brain.getDevelopmentalStage()`
- Age-based plasticity: Automatic with development enabled

## Integration Patterns

### Pattern 1: Standard Agent Simulation
```python
# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Pattern 2: Sensorimotor Learning
```python
# Create agent with specific capabilities
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.region_count", 2)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning for exploration
agent.enableRewardModulation(True)  # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)  # Brain matures over time

# Run learning episode
for step in range(500):
    world.update(0.05)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Report progress periodically
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Pattern 3: Batch Processing
```python
import pynlm
import time

def run_simulation_series(config_dict, num_steps, enable_features=True):
    """Run multiple simulations with given configurations"""
    
    results = []
    
    for i, config_dict in enumerate(config_dict):
        print(f"Running simulation {i+1}...")
        
        # Create brain
        config = pynlm.createDefaultConfig()
        for key, value in config_dict.items():
            config.set(key, value)
            
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create environment
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Configure agent features
        if enable_features:
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
        
        # Run simulation
        start_time = time.time()
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        end_time = time.time()
        
        # Collect results
        results.append({
            'simulation_id': i,
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'duration': end_time - start_time,
            'final_stage': brain.getDevelopmentalStage()
        })
    
    return results

# Example usage
configs = [
    {"brain.neuron_count": 100, "brain.region_count": 1},
    {"brain.neuron_count": 500, "brain.region_count": 3},
    {"brain.neuron_count": 1000, "brain.region_count": 5},
]

results = run_simulation_series(configs, 100)
for result in results:
    print(f"Simulation {result['simulation_id']}: {result['total_spikes']} spikes in {result['duration']:.2f}s")
```

## Utility Functions

### `pynlm.createDefaultConfig()`
Create a default configuration object.

### `pynlm.createBrain(config)`
Create a new brain with the given configuration.

### `pynlm.createSimpleWorld()`
Create a new simple world environment.

### `pynlm.createAgentBrain(brain)`
Create an agent brain interface for the given brain.

## Error Handling

The module includes comprehensive error handling for common scenarios:

```python
import pynlm

try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
except RuntimeError as e:
    print(f"Brain initialization failed: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Monitoring

Track simulation performance using these methods:

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Get performance metrics
neuron_count = brain.getTotalNeuronCount()
synapse_count = brain.getTotalSynapseCount()
active_neurons = brain.getActiveNeuronCount()
firing_neurons = brain.getFiringNeuronCount()
avg_firing_rate = brain.getAverageFiringRate()
spike_count = brain.getTotalSpikeCount()
```

## Memory Management

The module uses automatic memory management:

- Brain objects are managed by Python's garbage collection
- Checkpointing allows saving and loading brain states
- Memory systems integrate with neural processing

## Advanced Configuration

### Config Value Types
The configuration system supports these value types:

```python
config.set("brain.neuron_count", 1000)           # int
config.set("simulation.learning_rate", 0.01)     # double
config.set("agent.enable_curiosity", True)        # bool
config.set("brain.name", "MyBrain")              # string
config.set("sensor.channels", [1, 2, 3])         # vector of ints
config.set("weights", [0.1, 0.5, 0.9])           # vector of doubles
config.set("tags", ["tag1", "tag2", "tag3"])     # vector of strings
```

### Development Stages
```python
brain = pynlm.createBrain(config)
brain.initialize()

# Development stages:
# - Initial: Newborn brain
# - CriticalPeriod: High plasticity phase
# - Maturation: Brain development
# - Adult: Fully developed
# - Aging: Declining performance

stage = brain.getDevelopmentalStage()
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
```

## Compatibility

This module is compatible with Python 3.8+ and uses pybind11 for C++/Python bindings.

## Getting Help

- **Documentation**: `easy_usage.md` (beginner-friendly)
- **Detailed API**: `HOW_TO_USE.md`
- **Architecture**: `docs/ARCHITECTURE.md`
- **Examples**: `examples/` directory

## License

MIT License - see `LICENSE` file for details.

## Authors

Research project by the NLM team. See `docs/` for scientific background and methodology.

## Acknowledgments

Based on the original NLM C++ neural simulation framework with enhanced Python bindings.