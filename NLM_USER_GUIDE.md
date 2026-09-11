# NLM (Neural Learning Machine) - Comprehensive User Guide

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain simulator that integrates multiple neural systems into a coherent artificial brain architecture. The system implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation and multiple plasticity mechanisms.

## Quick Start Examples

### Example 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Example 2: Simple Agent in a World

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent simulation complete!")
```

### Example 3: Advanced Integration Experiment

```python
import pynlm

# Run Phase 6 integration test
def run_integration_test():
    """Run the complete integration test for all brain systems."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 2)
    
    # Initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with all subsystems
    agent = pynlm.createAgentBrain(brain)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print("Integration test complete!")
    
    return True

run_integration_test()
```

## Advanced Features

### 1. Working Memory Integration

Working memory in NLM uses neural persistent activity and recurrent connections to maintain information. This allows the brain to maintain representations of recent experiences for immediate use.

```python
# Store and retrieve from working memory
brain = pynlm.createBrain(config)
brain.initialize()

# Working memory stores patterns as neural activity
memory_patterns = brain.getWorkingMemory().retrieve()
# memory_patterns is a vector of activation values
```

### 2. Episodic Memory for Experience Storage

Episodic memory stores experiences as neural activity patterns that can be retrieved later for learning and prediction.

```python
# Access episodic memory
episodic = brain.getEpisodicMemory()
if episodic:
    episode_count = episodic.getEpisodeCount()
    print(f"Stored {episode_count} episodes")
    
    # Retrieve similar past experiences
    recent_episodes = episodic.getRecentEpisodes(5)
```

### 3. Prediction System for Temporal Processing

The prediction system learns temporal sequences and generates predictions about future sensory states.

```python
# Access prediction system
prediction = brain.getPredictionSystem()
if prediction:
    error = prediction.getPredictionError()
    confidence = prediction.getPredictionErrorSignal()
    print(f"Prediction error: {error:.3f}, confidence: {confidence:.3f}")
```

### 4. Neuromodulation for Learning Control

Dopamine, novelty, and curiosity systems modulate plasticity and neural excitability based on experience.

```python
# Access neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()

if dopamine:
    dopamine_level = dopamine.getLevel()
    print(f"Dopamine level: {dopamine_level:.3f}")
```

### 5. Cognitive Systems for Higher Processing

Neural planner, concept formation, and attention systems enable more sophisticated cognitive processing.

```python
# Access cognitive systems
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()

if planner:
    print(f"Planner active: {planner.getPlanningDepth()} levels deep")
```

## Configuration Options

### Brain Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## System Integration Capabilities

### Working Memory Flow

1. **Input Processing**: Sensory input injected into sensory neurons
2. **Memory Storage**: Active neurons stored in working memory
3. **Maintenance**: Recurrent connections maintain memory traces
4. **Competition**: Attentional selection resolves competing memories
5. **Retrieval**: Winning traces influence neural processing

### Prediction and Learning Flow

1. **Sequence Learning**: Neural prediction system learns temporal patterns
2. **Prediction Error**: Comparison of predicted vs actual sensory states
3. **Neuromodulation**: Prediction error modulates dopamine and curiosity
4. **Action Planning**: Neural planner uses predictions to select actions
5. **Consequence Learning**: Action-outcome associations stored

### Memory Consolidation Flow

1. **Experience Encoding**: Current neural activity stored as episodic memory
2. **Concept Formation**: Stable patterns extracted from episodic data
3. **Planning**: Concepts guide action selection and future predictions
4. **Replay**: Important memories reactivated during consolidation
5. **Development**: Plasticity rates modulate with developmental stage

## Performance Monitoring

```python
# Monitor brain statistics
brain.logStatus()

# Get key metrics
stats = {
    "total_neurons": brain.getTotalNeuronCount(),
    "total_synapses": brain.getTotalSynapseCount(),
    "active_neurons": brain.getActiveNeuronCount(),
    "firing_neurons": brain.getFiringNeuronCount(),
    "total_spikes": brain.getTotalSpikeCount(),
    "avg_firing_rate": brain.getAverageFiringRate(),
    "e_i_ratio": brain.getExcitationInhibitionRatio()
}

for key, value in stats.items():
    print(f"{key}: {value}")
```

## Building and Running

### Prerequisites

- C++20 compatible compiler (GCC 10+, Clang 12+)
- CMake 3.16 or higher
- Python 3.8+
- pybind11 2.11.0+

### Build Instructions

```bash
# Build the C++ library
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install pybind11 scikit-build-core
pip install -e .
```

### Running Demos

```bash
# Run Phase 6 integration demo
cd build
./nlm_phase6_demo

# Or run the main executable
./nlm --help
```

## Python API Reference

### Core Classes

#### Brain
- `createBrain(config)`: Create a new brain with configuration
- `initialize()`: Initialize the brain
- `step(step)`: Run one simulation step
- `save(path)`: Save brain state to checkpoint
- `load(path)`: Load brain state from checkpoint
- All get methods for statistics and system access

#### Config
- `createDefaultConfig()`: Create default configuration
- `set(key, value)`: Set configuration parameter
- `get(key)`: Get configuration parameter

#### AgentBrain
- `createAgentBrain(brain)`: Create agent interface
- `initialize(world)`: Initialize agent with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command from neural activity
- `applyRewardModulation(reward, predicted)`: Apply reward modulation
- All enable/disable methods for subsystems

#### SimpleWorld
- `createSimpleWorld()`: Create simple world environment
- `configure(width, height, visionWidth, visionHeight)`: Configure world dimensions
- `update(dt)`: Update world state
- `reset()`: Reset world to initial state

### Commonly Used Methods

```python
# Create and initialize brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 10, 10)
world.reset()
agent.initialize(world)

# Run complete agent simulation
for i in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Optional: apply reward modulation
    # reward = world.computeReward(agent.getBrain().getRegions()[0].get())
    # agent.applyRewardModulation(reward, 0.0)

print(f"Simulation complete. Stats:")
print(f"  Neurons: {brain.getTotalNeuronCount()}")
print(f"  Firing: {brain.getFiringNeuronCount()}")
print(f"  Avg rate: {brain.getAverageFiringRate():.2f}")
```

## Troubleshooting

### Common Issues and Solutions

**Issue: Brain not doing anything (all zeros)**
```
# Problem: Brain needs time to "warm up"
# Solution: Run more simulation steps
```

**Issue: Agent not moving**
```
# Problem: Need to call world.applyMotorCommand()
# Solution: Ensure motor command is applied
world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

**Issue: ImportError - No module named 'pynlm'**
```
# Problem: Python module not installed
# Solution: Install with pip
pip install -e .
```

**Issue: Build errors**
```
# Problem: CMake/Python bindings issues
# Solution: Ensure dependencies are installed
pip install pybind11 scikit-build-core
```

### Debug Information

```python
# Enable logging
import logging
logging.basicConfig(level=logging.INFO)

# Check brain status
brain.logStatus()
```

## Further Reading

- **ARCHITECTURE.md**: Detailed system architecture documentation
- **SCIENCE.md**: Scientific background and neural mechanisms
- **EXPERIMENTS.md**: Experiment descriptions and results
- **LIMITATIONS.md**: Current limitations and future directions

## Getting Involved

### Research Opportunities

1. **Memory Integration**: Enhance connectivity between working, episodic, and associative memory
2. **Prediction Accuracy**: Improve temporal prediction capabilities
3. **Cognitive Architecture**: Add more sophisticated planning and reasoning
4. **Developmental Dynamics**: Model more realistic developmental trajectories
5. **Neuromodulation**: Integrate additional neuromodulators (acetylcholine, norepinephrine)

### Performance Optimization

1. **Parallel Processing**: Implement SIMD vectorization
2. **Memory Management**: Optimize neural memory pools
3. **Event System**: Improve spike event processing efficiency
4. **Checkpointing**: Add incremental checkpointing for long simulations

### Software Development

1. **Python API**: Add more Python-friendly methods
2. **Visualization**: Integrate with visualization frameworks
3. **Testing**: Add comprehensive unit and integration tests
4. **Documentation**: Expand API documentation and examples

## License

MIT License. See LICENSE file for details.

## Copyright

NLM Project 2024-2026. Research project investigating computational brain-like systems.
