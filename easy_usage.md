# NLM - Enhanced Usage Guide for Beginners and Advanced Users

## What is NLM?

NLM is an experimental brain simulation project that creates artificial developmental brains. It's **NOT**:
- A transformer or LLM
- A chatbot  
- A deep learning model
- A pretrained AI system

NLM **IS**:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Project Overview

NLM implements a complete spiking neural network simulator with:
- **80+ C++ source files** with comprehensive documentation
- **Python bindings** (`pynlm`) for easy interaction
- **Phase 6: FINAL INTEGRATION** - all brain systems working together
- **Performance optimizations** including SIMD vectorization
- **Memory management** with pools and event queues

## Core Systems Integrated

1. **Memory Systems**: Working memory, episodic memory, associative memory
2. **Prediction System**: Neural prediction with error computation
3. **Cognition Systems**: Neural planner, concept formation, attention
4. **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
5. **Plasticity**: STDP, Hebbian learning, structural plasticity
6. **Development System**: Synaptogenesis, pruning, maturation
7. **Sensory/Motor**: Vision, audio, motor control
8. **Environment**: Interactive simulation world

## Quick Start (Enhanced)

### Example 1: Basic Brain Creation

```python
import pynlm

# Quick configuration setup
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.connection_probability", 0.1)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

### Example 2: Advanced Configuration

```python
import pynlm
import json

# Method 1: Create from Python dictionary
config_dict = {
    "brain.neuron_count": 1000,
    "brain.region_count": 2,
    "plasticity.stdp.enable": True,
    "neuromod.curiosity.enable": True,
    "development.enable": True,
    "checkpoint.enable": True,
    "replay.enable": True
}

config = pynlm.createConfigFromDict(config_dict)

# Method 2: Create from JSON-like string
json_config = """
brain.neuron_count = 800
brain.region_count = 1
plasticity.stdp.learning_rate = 0.001
neuromod.dopamine.scale = 1.5
"""

config2 = pynlm.createConfigFromJson(json_config)

# Validate configuration
try:
    if pynlm.validateConfig(config2):
        print("Configuration is valid!")
except Exception as e:
    print(f"Configuration error: {e}")
```

### Example 3: Complete Agent with Learning

```python
import pynlm

def run_advanced_simulation(num_steps=1000):
    """Advanced simulation with learning and development."""
    
    # Initialize brain with custom configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.05)
    config.set("development.initial_stage", "CriticalPeriod")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent.initialize(world)
    
    # Enable advanced learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True) 
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Starting advanced simulation...")
    
    for step in range(num_steps):
        # World update
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply learning signals
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Development update
        agent.updateDevelopment(0.1)
        
        # Progress monitoring
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Reward history: {agent.getNeuromodulationLevel():.3f}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_advanced_simulation(1000)
print("Simulation complete!")
```

### Example 4: Checkpoint and Persistence

```python
import pynlm
import os

# Simulation with checkpointing
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

checkpoint_file = "nlm_checkpoint.bin"

# Run first half of simulation
for step in range(500):
    brain.step(step)
    
    # Save checkpoint periodically
    if step % 100 == 99:
        brain.save(checkpoint_file)
        print(f"Checkpoint saved at step {step}")

# Save final state
brain.save("nlm_final.bin")

# Reload from checkpoint
new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
new_brain.initialize()
new_brain.load(checkpoint_file)

print(f"Reloaded brain has {new_brain.getTotalNeuronCount()} neurons")
print(f"Total spikes: {new_brain.getTotalSpikeCount()}")

# Clean up
os.remove(checkpoint_file)
os.remove("nlm_final.bin")
```

## Performance Monitoring

### Built-in Statistics

```python
def monitor_brain_performance(brain, agent, world, duration=1000):
    """Monitor brain performance with detailed metrics."""
    
    stats = {
        'firing_rates': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'development_stages': [],
        'total_rewards': [],
        'spike_counts': []
    }
    
    for step in range(duration):
        brain.step(step)
        
        # Collect metrics
        stats['firing_rates'].append(brain.getAverageFiringRate())
        if agent:
            stats['curiosity_levels'].append(agent.getCuriosityLevel())
            stats['novelty_levels'].append(agent.getNoveltyLevel())
            stats['development_stages'].append(brain.getDevelopmentalStage())
            stats['total_rewards'].append(agent.getNeuromodulationLevel())
        
        stats['spike_counts'].append(brain.getTotalSpikeCount())
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    return stats

# Generate performance report
stats = monitor_brain_performance(brain, agent, world, 1000)

print("Performance Summary:")
print(f"Average firing rate: {sum(stats['firing_rates'])/len(stats['firing_rates']):.2f} Hz")
print(f"Max curiosity: {max(stats['curiosity_levels']):.3f}")
print(f"Total spikes: {stats['spike_counts'][-1]}")
```

## Debugging and Troubleshooting

### Common Issues and Solutions

**"Brain isn't doing anything"**
```python
# Check initialization
brain = pynlm.createBrain(config)
if not brain.initialize():
    print("Brain initialization failed!")
    # Check config values
    print("Config:", config.summary())

# Ensure steps are running
for step in range(100):
    brain.step(step)
    # Add sensory input if needed
    # brain.receiveSensoryInput(sensory_input)
```

**"Agent isn't responding to environment"**
```python
# Debug agent initialization
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

if not agent.initialize(world):
    print("Agent initialization failed")

# Check world state
print(f"World size: {world.getWidth()}x{world.getHeight()}")
print(f"Agent position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
```

**"Memory usage high"**
```python
# Monitor memory usage (if available)
import psutil
import os

process = psutil.Process(os.getpid())
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")

# Force garbage collection
import gc
gc.collect()
```

## Advanced Features

### 1. Configuration Templates

```python
def create_expert_config(expertise_type="general"):
    """Create pre-configured expert brains."""
    
    templates = {
        "explorer": {
            "brain.neuron_count": 2000,
            "neuromod.curiosity.enable": True,
            "neuromod.novelty.enable": True,
            "development.enable": True,
            "plasticity.structural.enable": True
        },
        "hunter": {
            "brain.neuron_count": 1000,
            "neuromod.dopamine.scale": 2.0,
            "neuromod.curiosity.enable": False,
            "plasticity.stdp.enable": True
        },
        "social": {
            "brain.neuron_count": 1500,
            "brain.region_count": 3,
            "neuromod.dopamine.scale": 1.0,
            "neuromod.curiosity.enable": True
        }
    }
    
    base_config = pynlm.createDefaultConfig()
    
    if expertise_type in templates:
        for key, value in templates[expertise_type].items():
            # Configure type conversion based on key
            if key.endswith(".enable"):
                base_config.set(key, str(value).lower() == "true")
            elif key.endswith(".scale") or key.endswith(".rate"):
                base_config.set(key, float(value))
            else:
                base_config.set(key, value)
    
    return base_config

# Use a specialized brain
explorer_brain = pynlm.createBrain(create_expert_config("explorer"))
```

### 2. Batch Processing

```python
def run_batch_simulation(config, num_episodes=10, steps_per_episode=500):
    """Run multiple simulation episodes efficiently."""
    
    results = []
    
    for episode in range(num_episodes):
        print(f"Running episode {episode + 1}/{num_episodes}")
        
        # Create fresh brain for each episode
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning for this episode
        agent.enableRewardModulation(True)
        
        # Run episode
        for step in range(steps_per_episode):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Record results
        results.append({
            'episode': episode + 1,
            'total_spikes': brain.getTotalSpikeCount(),
            'final_firing_rate': brain.getAverageFiringRate(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel()
        })
        
        # Save brain state
        brain.save(f"batch_episode_{episode + 1:03d}.bin")
        
        print(f"Episode {episode + 1} complete: {results[-1]['total_spikes']} spikes")
    
    return results

# Run batch simulation
batch_results = run_batch_simulation(config)
```

## API Reference

### Key Classes

| Class | Purpose | Key Methods |
|-------|---------|-------------|
| `Brain` | Core neural computation | `initialize()`, `step()`, `produceAction()` |
| `Config` | Configuration management | `set()`, `get()`, `loadFromFile()`, `saveToFile()` |
| `AgentBrain` | Brain-environment interface | `processSensoryInput()`, `decodeMotorCommand()` |
| `SimpleWorld` | Simulation environment | `update()`, `applyMotorCommand()`, `getSensoryPercept()` |

### Helper Functions

- `pynlm.createDefaultConfig()` - Create standard configuration
- `pynlm.createConfigFromDict()` - Create config from Python dict
- `pynlm.createConfigFromJson()` - Create config from JSON string
- `pynlm.validateConfig()` - Validate configuration
- `pynlm.createBrain()` - Create neural brain
- `pynlm.createAgentBrain()` - Create agent interface
- `pynlm.createSimpleWorld()` - Create simulation world

## Next Steps

1. **Explore Documentation**: Read `docs/ARCHITECTURE.md` for system details
2. **Run Experiments**: Try `nlm_phase6_demo` for Phase 6 integration test
3. **Advanced Usage**: Use `nlm` command-line tool for C++ interface
4. **Performance Tuning**: Adjust configuration parameters for your use case
5. **Contribute**: Extend Python bindings with your own functions

## Getting Help

- **Documentation**: `README.md`, `HOW_TO_USE.md`, `docs/` directory
- **Examples**: `easy_usage.md`, `HOW_TO_USE.md`
- **Experiments**: `src/experiments/` directory
- **Source Code**: `src/` directory with comprehensive comments

Enjoy exploring the artificial brain simulation!
