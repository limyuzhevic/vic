# NLM (Neural Learning Machine) - Complete Documentation

## Overview

NLM is a comprehensive brain-inspired spiking neural network simulator written in C++20 with Python bindings. It implements Phase 2-6 experiments, providing a complete neural simulation framework from basic neuron dynamics to advanced cognitive systems.

## Quick Start

### Minimal Working Example

```python
import pynlm

# Create and initialize a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete! Neurons: {brain.getTotalNeuronCount()}")
```

### Complete Agent Simulation

```python
import pynlm

# 1. Create configuration
config = pynlm.createDefaultConfig()

# 2. Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# 3. Create agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# 4. Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# 5. Run simulation
for step in range(1000):
    world.update(0.1)                    # Update world
    percept = world.getSensoryPercept()  # What agent sees
    agent.processSensoryInput(percept)   # Brain processes input
    brain.step(step)                      # Brain thinks
    action = agent.decodeMotorCommand() # Brain decides
    world.applyMotorCommand(action, world.getSimulationTime())  # Act

print("Agent simulation complete!")
```

## Project Structure

### Core Components

| Component | Purpose | Key Classes | Files |
|-----------|---------|-------------|-------|
| **Brain** | Neural computation engine | `Brain`, `Neuron`, `Synapse` | `src/brain/`, `src/dynamics/` |
| **Memory** | Storage and recall systems | `WorkingMemory`, `EpisodicMemory` | `src/memory/` |
| **Neuromodulation** | Learning signals | `Dopamine`, `Curiosity`, `Novelty` | `src/neuromodulation/` |
| **Cognition** | Higher-level processing | `NeuralPlanner`, `ConceptFormation` | `src/cognition/` |
| **World** | Environment simulation | `SimpleWorld`, `Observation` | `src/world/` |
| **Agent** | Brain-world interface | `AgentBrain`, `SensoryPercept` | `src/agent/` |

### Python Bindings

The Python bindings (`pynlm` module) provide access to all core functionality:

```python
import pynlm

# Core types
pynlm.NeuronType, pynlm.SynapseType, pynlm.ActionType

# Creation functions
pynlm.createDefaultConfig()
pynlm.createBrain(config)
pynlm.createSimpleWorld()
pynlm.createAgentBrain(brain)

# Classes with bindings
pynlm.Config, pynlm.Brain, pynlm.SensoryInput
pynlm.Action, pynlm.WorldObject, pynlm.AgentBrain
```

## Usage Examples

### Example 1: Basic Neural Simulation

```python
import pynlm

# Create configuration with custom settings
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 2)
config.set("brain.connection_probability", 0.15)

# Initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with progress tracking
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")

print("Simulation complete!")
```

### Example 2: Complete Agent with World

```python
def run_agent_episode(world_width=20, world_height=20, steps=500):
    """Run a complete agent simulation with world interaction."""
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(
        width=world_width,
        height=world_height,
        visionWidth=8,
        visionHeight=8
    )
    world.reset()
    world.setAgentStart(world_width//2, world_height//2)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run episode
    total_reward = 0.0
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Action selection
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
        
        total_reward += reward
        
        # Print statistics every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Reward: {reward:.3f} | Total: {total_reward:.3f}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    print(f"Episode complete! Total reward: {total_reward:.3f}")
    return total_reward

# Run the episode
run_agent_episode(world_width=30, world_height=30, steps=1000)
```

### Example 3: Configuration Management

```python
import pynlm
import json

# Create configuration from scratch
config = pynlm.createDefaultConfig()

# Modify parameters
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 3)
config.set("brain.connection_probability", 0.2)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("memory.enable_working", True)
config.set("memory.capacity", 500)
config.set("memory.enable_episodic", True)
config.set("memory.max_episodes", 5000)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("development.enable", True)
config.set("development.synaptogenesis_rate", 0.001)
config.set("development.pruning_rate", 0.0001)

# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile("my_config.json")

# Create brain with loaded configuration
brain = pynlm.createBrain(loaded_config)
brain.initialize()

# Use brain...
```

### Example 4: Advanced Brain Features

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access brain statistics
stats = brain.getBrainStats()  # String with detailed stats
print("Brain Statistics:")
print(stats)

# Check brain status
brain.logStatus()

# Access integrated systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
planning_system = brain.getPlanner()
attention_system = brain.getAttention()

# Check system status
print(f"Working Memory: {'Active' if working_memory else 'Not available'}")
print(f"Episodic Memory: {'Active' if episodic_memory else 'Not available'}")
print(f"Planning System: {'Active' if planning_system else 'Not available'}")
print(f"Attention System: {'Active' if attention_system else 'Not available'}")

# Create agent with enhanced features
agent = pynlm.createAgentBrain(brain)

# Enable all features for comprehensive simulation
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Simulate episode
world = pynlm.createSimpleWorld()
agent.initialize(world)

agent.simulateEpisode(world, steps=2000, enable_reward_modulation=True)
```

### Example 5: Experiment Runner

```python
import pynlm
from nlm import ExperimentRunner, Experiment

# Create experiment runner
runner = ExperimentRunner()

# Create experiment
experiment = runner.createExperiment("NeuralLearningExperiment")
experiment.setSeed(42)

# Configure experiment
experiment.setConfigValue("brain.neuron_count", "1000")
experiment.setConfigValue("brain.connection_probability", "0.15")

# Run simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

runner.runExperiment(experiment, brain, world, 1000)

# Get results
metrics = experiment.getMetrics()
for name, value in metrics:
    print(f"{name}: {value}")

# Save results
runner.saveResults("experiment_results.json")
```

### Example 6: Memory System Analysis

```python
import pynlm

# Create brain with memory systems enabled
config = pynlm.createDefaultConfig()
config.set("memory.enable_working", True)
config.set("memory.capacity", 200)
config.set("memory.enable_episodic", True)
config.set("memory.max_episodes", 1000)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation to populate memory
for step in range(100):
    brain.step(step)

# Get memory statistics
agent = pynlm.createAgentBrain(brain)
memory_stats = agent.getMemoryStats()
print("Memory System Statistics:")
print(memory_stats)

# Check working memory
working_mem = brain.getWorkingMemory()
if working_mem:
    print(f"Working memory traces: {working_mem.getActiveTraces()}")
    print(f"Working memory capacity: {working_mem.getCapacity()}")

# Check episodic memory
episodic_mem = brain.getEpisodicMemory()
if episodic_mem:
    print(f"Episodic episodes: {episodic_mem.getEpisodeCount()}")
    print(f"Max episodes: {episodic_mem.getMaxEpisodes()}")

# Replay some episodes
if episodic_mem and episodic_mem.getEpisodeCount() > 0:
    episodes_to_replay = episodic_mem.getEpisodesForReplay(3)
    print(f"Episodes available for replay: {len(episodes_to_replay)}")
```

### Example 7: Neuromodulation Analysis

```python
import pynlm

# Create brain with neuromodulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Run simulation with sensory input
for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Apply reward modulation
    reward = 0.5 if step % 50 == 0 else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)

# Get neuromodulation statistics
neuro_stats = agent.getNeuromodulationStats()
print("Neuromodulation System Statistics:")
print(neuro_stats)

# Get brain statistics
brain_stats = agent.getBrainStats()
print("\nBrain Statistics:")
print(brain_stats)

# Check individual neuromodulators
print(f"\nDopamine level: {agent.getNeuromodulationLevel():.3f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
print(f"Developmental stage: {agent.getDevelopmentalStage()}")
```

## Configuration File Examples

### Basic Configuration (`basic_config.json`)

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "time_constant": 20.0
        },
        "hebbian": {
            "enable": true
        }
    },
    "memory": {
        "working_memory": {
            "enable": true,
            "capacity": 100
        }
    }
}
```

### Advanced Configuration (`advanced_config.json`)

```json
{
    "brain": {
        "neuron_count": 5000,
        "region_count": 4,
        "connection_probability": 0.2
    },
    "plasticity": {
        "stdp": {"enable": true, "learning_rate": 0.01},
        "hebbian": {"enable": true},
        "structural": {"enable": true, "synaptogenesis_rate": 0.001}
    },
    "memory": {
        "working_memory": {"enable": true, "capacity": 500},
        "episodic_memory": {"enable": true, "max_episodes": 10000}
    },
    "neuromodulation": {
        "dopamine": {"baseline": 0.0},
        "curiosity": {"enable": true, "threshold": 0.5},
        "novelty": {"enable": true}
    },
    "development": {
        "enable": true,
        "synaptogenesis_rate": 0.001,
        "pruning_rate": 0.0001
    }
}
```

## Command Line Interface

### Basic Usage

```bash
# Help
./nlm --help

# Show version
./nlm --version

# Run Phase 2 demo
./nlm --phase 2

# Run Phase 6 integration demo
./nlm --demo

# Run Phase 6 integration test
./nlm --integration

# Create configuration
./nlm --python create_config.py

# Save brain to file
./nlm --export-python brain_data.py --output brain_state.pickle
```

### Configuration Examples

```bash
# Create with specific parameters
./nlm --set brain.neuron_count=2000 \
      --set brain.connection_probability=0.2 \
      --set memory.capacity=200

# Load from file and override
./nlm --config base.cfg \
      --set brain.neuron_count=3000 \
      --set brain.region_count=2

# Run experiment with custom settings
./nlm --phase 6 \
      --neuron-count 1000 \
      --max-steps 5000 \
      --enable-checkpointing \
      --enable-replay
```

## Advanced Features

### Episode Simulation

The AgentBrain class provides enhanced episode simulation:

```python
# Create agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Simulate complete episode
agent.simulateEpisode(world, steps=2000, enable_reward_modulation=True)

# Get brain statistics
stats = agent.getBrainStats()
print(stats)
```

### Memory System Integration

```python
# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()

# Check memory status
if working_memory:
    print(f"Working memory traces: {working_memory.getActiveTraces()}")
if episodic_memory:
    print(f"Episodic episodes: {episodic_memory.getEpisodeCount()}")

# Get comprehensive memory statistics
agent = pynlm.createAgentBrain(brain)
memory_stats = agent.getMemoryStats()
print(memory_stats)
```

### Neuromodulation Analysis

```python
# Get neuromodulation statistics
agent = pynlm.createAgentBrain(brain)
neuro_stats = agent.getNeuromodulationStats()
print(neuro_stats)

# Check individual neuromodulators
print(f"Dopamine level: {agent.getNeuromodulationLevel():.3f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
```

## Troubleshooting

### Common Issues

1. **Brain not initializing**
   - Check that `brain.initialize()` is called before `brain.step()`
   - Ensure configuration is valid
   - Verify system resources are available

2. **Agent not processing input**
   - Check that `agent.initialize(world)` is called
   - Ensure `agent.processSensoryInput(percept)` is called with valid percept
   - Verify world provides sensory data

3. **Memory systems not working**
   - Check that memory systems are enabled in configuration
   - Verify brain has memory system access methods
   - Ensure simulation runs long enough to populate memory

4. **Performance issues**
   - Use `nlm --profile` to identify bottlenecks
   - Check memory usage with `nlm --monitor-memory`
   - Reduce network size for testing: `--neuron-count 100`

### Error Messages

- **"Brain initialization failed"**: Check configuration and system resources
- **"Sensory input not available"**: Ensure world provides percept data
- **"Memory system not available"**: Enable memory systems in configuration
- **"Configuration file not found"**: Use absolute path or ensure file exists

## Performance Tips

1. **For small networks**: Use Phase 2 or Phase 3 experiments
2. **For large networks**: Use Phase 5 (optimized) or Phase 6 (integrated)
3. **For memory-intensive tasks**: Enable specific memory systems only
4. **For development**: Use Phase 6 demo with smaller settings
5. **For production**: Use optimized configuration with all features

## Further Reading

- [HOW_TO_USE.md](HOW_TO_USE.md) - Detailed usage instructions
- [easy_usage.md](easy_usage.md) - Beginner-friendly guide
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture
- [docs/SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [docs/EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 audit report

This documentation provides comprehensive coverage of the NLM framework, from basic usage to advanced features, and serves as a complete reference for working with artificial neural systems simulation.