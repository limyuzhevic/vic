# NLM (熙然) - Enhanced User Guide

**An Experimental Artificial Developmental Brain**

## Quick Start

Welcome to NLM (Neural Learning Machine)! This is a brain-inspired spiking neural network simulator that learns from experience.

### What Makes NLM Special?

✅ **Real LIF Neuron Dynamics** - Leaky Integrate-and-Fire neurons with biological accuracy  
✅ **Full Integration** - All cognitive systems: Memory, Neuromodulation, Prediction, Cognition, Development  
✅ **Event-Driven Processing** - Spike propagation with synaptic delays  
✅ **Multiple Plasticity Rules** - STDP, Hebbian, and structural plasticity  
✅ **Development Stages** - Brain matures through developmental phases  
✅ **Python API** - Easy-to-use Python interface for rapid prototyping  

## Installation

### Prerequisites

```bash
# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build the C++ extension
pip install .

# Or install in development mode
pip install -e .
```

### Build from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Quick Examples

### Example 1: Simple Brain Simulation

```python
import pynlm

# Create a default brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

### Example 2: Enhanced Phase 6 Configuration

```python
import pynlm

# Create Phase 6 configuration (final integration)
config = pynlm.createPhase6Config()

# Modify some settings
config.set("brain.neuron_count", 5000)
config.set("development.critical_period", 500)
config.set("neuromod.curiosity.threshold", 0.2)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()
```

### Example 3: Complete Agent with Learning

```python
import pynlm

# Setup components
config = pynlm.createPhase6Config()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create learning agent with all capabilities
agent = pynlm.createLearningAgent(brain, world, 
                                 enableRewardModulation=True,
                                 enableStructuralPlasticity=True,
                                 enableDevelopment=True,
                                 enableCuriosity=True)

# Run episode simulation
metrics = pynlm.simulateEpisode(agent, world, maxSteps=500)
for metric in metrics[:10]:  # Show first 10 metrics
    print(metric)
```

### Example 4: Advanced Configuration Management

```python
import pynlm

# Create configuration with helper functions
config = pynlm.createPhase6Config()

# Validate configuration
if config.has("brain.neuron_count"):
    neurons = config.getOr("brain.neuron_count", 1000)
    print(f"Using {neurons} neurons")

# Save configuration
config.saveToFile("my_config.cfg")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.cfg")
```

## Core Concepts

### 1. Brain System

The **Brain** class is the central neural simulation component:

- **Neurons**: LIF neurons with biological parameters
- **Synapses**: Connections with synaptic weights and plasticity
- **Regions**: Organized neural populations
- **Memory**: Working, episodic, and semantic memory systems
- **Development**: Plasticity modulated by developmental stage

### 2. World System

The **SimpleWorld** provides the simulation environment:

- **2D Environment**: Grid-based world with coordinates
- **Sensory Input**: Vision, audio, touch, and internal signals
- **Actions**: Motor commands for agent interaction
- **Physics**: Basic physics simulation for agents
- **Objects**: Interactive world objects

### 3. Agent System

The **AgentBrain** connects brain to world:

- **Sensory Processing**: Converts world input to neural signals
- **Motor Decoding**: Translates neural activity to actions
- **Learning Systems**: Reward modulation, structural plasticity, development, curiosity
- **Decision Making**: Cognitive mechanisms for action selection

### 4. Memory Systems

NLM implements multiple memory systems:

- **Working Memory**: Persistent activity with competition
- **Episodic Memory**: Experience encoding and replay
- **Semantic Memory**: Pattern associations
- **Procedural Memory**: Skills and habits

### 5. Neuromodulation

Biological neuromodulators:

- **Dopamine**: Reward prediction and learning
- **Curiosity**: Exploration and novelty detection
- **Novelty**: Surprise and prediction error
- **Prediction Error**: Forward modeling and error signals

### 6. Developmental Stages

Brain development phases:

- **Initial**: Basic neural circuits
- **Critical Period**: High plasticity for learning
- **Maturation**: Refinement of connections
- **Adult**: Stable adult brain
- **Aging**: Gradual decline and plasticity reduction

## API Reference

### Configuration

| Method | Description |
|--------|-------------|
| `createDefaultConfig()` | Create basic configuration |
| `createPhase6Config()` | Create Phase 6 (final integration) configuration |
| `config.has(key)` | Check if key exists |
| `config.getOr(key, default)` | Get value with default |
| `config.set(key, value)` | Set configuration value |

### Brain Operations

| Method | Description |
|--------|-------------|
| `brain.initialize()` | Initialize brain with configuration |
| `brain.step(step)` | Run one simulation step |
| `brain.step(step, time)` | Run step with timestamp |
| `brain.receiveSensoryInput(input)` | Inject sensory data |
| `brain.produceAction()` | Generate motor action |
| `brain.reset()` | Reset brain state |

### Agent Operations

| Method | Description |
|--------|-------------|
| `agent.initialize(world)` | Connect agent to world |
| `agent.processSensoryInput(percept)` | Process sensory data |
| `agent.decodeMotorCommand()` | Get motor action |
| `agent.applyRewardModulation(reward, predicted)` | Apply reward learning |
| `agent.updateDevelopment(timestep)` | Update development |

### Helper Functions

| Method | Description |
|--------|-------------|
| `runSimulation(brain, steps, verbose)` | Run simulation with logging |
| `createLearningAgent(brain, world, ...)` | Create agent with learning capabilities |
| `simulateEpisode(agent, world, steps)` | Run complete episode simulation |
| `createSensoryPercept(...)` | Create sensory percept from components |

## Performance Tips

### 1. Configuration Optimization

```python
# Use Phase 6 configuration for best performance
config = pynlm.createPhase6Config()

# Set appropriate parameters for your use case
config.set("brain.neuron_count", 2000)  # Fewer neurons = faster
config.set("simulation_timestep", 0.001)  # Adjust for accuracy/speed trade-off
```

### 2. Simulation Efficiency

```python
# For long simulations, consider batching
for batch in range(0, 10000, 1000):
    batch_steps = min(1000, 10000 - batch)
    metrics = pynlm.runSimulation(brain, batch_steps)
    print(f"Completed batch {batch}-{batch + batch_steps}")
```

### 3. Memory Management

```python
# Save brain state to resume later
brain.save("checkpoint.bin")

# Load from checkpoint
brain.load("checkpoint.bin")
```

### 4. Development Stage Optimization

```python
# Exploit critical period for faster learning
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Monitor development
for step in range(1000):
    brain.step(step)
    stage = brain.getDevelopmentalStage()
    if stage == pynlm.DevelopmentalStage.Adult:
        print("Development complete")
        break
```

## Troubleshooting

### Common Issues

**Issue: "Brain not responding"
**Solution:**
- Ensure `brain.initialize()` is called before `brain.step()`
- Check configuration values are appropriate
- Verify `brain.reset()` is called between episodes

**Issue: "Memory usage high"
**Solution:**
- Reduce `brain.neuron_count` 
- Use fewer regions
- Consider using checkpoint/restart instead of continuous simulation

**Issue: "Learning not occurring"
**Solution:**
- Ensure plasticity is enabled: `agent.enableRewardModulation(True)`
- Check that reward signals are being received
- Verify `brain.getDevelopmentalStage()` is appropriate

### Error Handling

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Brain initialized successfully")
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Advanced Topics

### 1. Custom Configurations

Create custom configuration files:

```json
{
    "brain": {
        "neuron_count": 3000,
        "region_count": 2,
        "connection_probability": 0.05
    },
    "plasticity": {
        "stdp": { "learning_rate": 0.001 },
        "hebbian": { "learning_rate": 0.0005 }
    },
    "development": {
        "critical_period": 200,
        "maturation_rate": 0.02
    }
}
```

### 2. Multi-Agent Systems

For advanced scenarios with multiple agents:

```python
# Create multiple brains with shared configuration
shared_config = pynlm.createPhase6Config()

agents = []
for i in range(5):
    brain = pynlm.createBrain(shared_config)
    brain.initialize()
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 6, 6)
    world.reset()
    world.setAgentStart(i * 2, i * 2)
    
    agent = pynlm.createLearningAgent(brain, world)
    agents.append(agent)
```

### 3. Performance Monitoring

Track simulation performance:

```python
import time

start_time = time.time()
for step in range(10000):
    brain.step(step)
    if step % 1000 == 0:
        print(f"Step {step}: {brain.getTotalSpikeCount()} spikes")
        print(f"Firing rate: {brain.getAverageFiringRate()} Hz")

end_time = time.time()
print(f"Total time: {end_time - start_time:.2f} seconds")
print(f"Average time per step: {(end_time - start_time) / 10000:.6f} seconds")
```

## Getting Help

### Documentation

- **Architecture Guide**: `docs/ARCHITECTURE.md` - Detailed system architecture
- **Science Documentation**: `docs/SCIENCE.md` - Scientific background
- **Experiments**: `docs/EXPERIMENTS.md` - Experiment descriptions
- **Phase 6 Audit**: `docs/PHASE6_FINAL_AUDIT.md` - Final integration documentation

### Community Resources

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Examples**: Check `examples/` directory for additional use cases

### Troubleshooting

- **Build Issues**: Check CMake version and compiler compatibility
- **Runtime Errors**: Ensure all initialization steps are followed
- **Performance Issues**: Profile with `perf` or `gprof` on the C++ side
- **Python API Issues**: Verify `pip install .` was successful

## Future Enhancements

The NLM project continues to evolve with:

### Planned Features

1. **Enhanced Visualizations**: Real-time brain activity monitoring
2. **Multi-Modal Sensing**: Advanced sensory modalities
3. **Social Learning**: Agent-to-agent knowledge transfer
4. **Emotions**: Affective computing and emotion simulation
5. **Long-term Memory**: Persistent memory storage
6. **Self-Reflection**: Metacognitive capabilities

### Performance Improvements

1. **GPU Acceleration**: CUDA support for parallel processing
2. **Adaptive Time Steps**: Variable time-stepping for efficiency
3. **Hierarchical Models**: Multiple levels of neural abstraction
4. **Sparse Representations**: Compressed neural representations

## Conclusion

NLM (Neural Learning Machine) provides a comprehensive framework for:

- ✅ Research in computational neuroscience
- ✅ Education in neural network principles
- ✅ Development of intelligent agent systems
- ✅ Simulation of cognitive development
- ✅ Exploration of learning mechanisms

The combination of real biological accuracy with user-friendly Python API makes NLM unique among neural network frameworks.

Start exploring your own artificial brain today! 🚀
