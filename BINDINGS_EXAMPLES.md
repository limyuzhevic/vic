# NLM Python Bindings - Usage Examples and Tutorials

## Table of Contents

1. [Getting Started](#getting-started)
2. [Basic Brain Operations](#basic-brain-operations)
3. [Working with Memory Systems](#working-with-memory-systems)
4. [Plasticity and Learning](#plasticity-and-learning)
5. [Agent-World Integration](#agent-world-integration)
6. [Advanced Features (Phase 6)](#advanced-features-phase-6)
7. [Error Handling](#error-handling)
8. [Performance Tips](#performance-tips)
9. [Complete Examples](#complete-examples)

## Getting Started

### Quick Start for Beginners

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

# Get basic statistics
stats = pynlm.getBrainStats(brain)
print(f"Brain has {stats['total_neurons']} neurons")
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
```

### Factory Methods for Easy Creation

```python
import pynlm

# Create neuron
neuron = pynlm.createNeuron(pynlm.NeuronType.Excitatory)

# Create region
region = pynlm.createRegion("cortex", 500)

# Create population
population_id = pynlm.createPopulation(region, 100, pynlm.NeuronType.Excitatory)

# Create synapse
synapse = pynlm.createSynapse(source_neuron, target_neuron, 1.0)
```

## Basic Brain Operations

### Brain Lifecycle Management

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.1f)

brain = pynlm.createBrain(config)
brain.initialize()

# Simulation loop
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        stats = pynlm.getBrainStats(brain)
        print(f"Step {step}: {stats['firing_neurons']} neurons firing")

# Save brain state
brain.save("my_brain_checkpoint.bin")

# Reset and run again
brain.reset()
for step in range(100):
    brain.step(step)

# Load saved state
brain.load("my_brain_checkpoint.bin")
```

### Brain Statistics and Monitoring

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get comprehensive statistics
stats = pynlm.getBrainStats(brain)

print("=== BRAIN STATISTICS ===")
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")
print(f"Active neurons: {stats['active_neurons']}")
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"E/I ratio: {stats['excitatory_inhibition_ratio']:.2f}")
print(f"Total spike count: {stats['total_spike_count']}")
print(f"Development stage: {stats['development_stage']}")

# Working memory statistics
if 'working_memory_capacity' in stats:
    print(f"Working memory capacity: {stats['working_memory_capacity']}")
    print(f"Active traces: {stats['working_memory_active_traces']}")
```

### Spike System Monitoring

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get spike system and history
spike_system = brain.getSpikeSystem()
if spike_system:
    history = pynlm.getSpikeHistory(brain)
    print(f"Spike system has {len(history)} events")
    
    # Analyze recent spikes
    recent_spikes = history[-100:] if len(history) > 100 else history
    print(f"Recent spikes: {len(recent_spikes)}")
```

## Working with Memory Systems

### Working Memory

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get memory systems
memory = pynlm.getMemorySystems(brain)
working_memory = memory['working']

# Store neuron activation in working memory
neuron_id = brain.getRegionIds()[0]  # Get first neuron from first region
working_memory.storeToNeuron(neuron_id, 0.8f)

# Get active traces
active_traces = working_memory.getActiveTraces()
print(f"Working memory has {active_traces} active traces")

# Update working memory (this happens automatically during simulation)
# but you can also trigger updates manually
brain.step(0)  # This will update working memory
```

### Episodic Memory

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get episodic memory system
memory = pynlm.getMemorySystems(brain)
 episodic_memory = memory['episodic']

# The brain automatically stores episodes every 10 steps
# Check if we have episodes
episodes = episodic_memory.getEpisodes()
print(f"Episodic memory has {len(episodes)} episodes")

# Access specific episode data
if episodes:
    latest_episode = episodes[-1]
    print(f"Latest episode: step {latest_episode.timestamp}")
    print(f"Reward: {latest_episode.reward}")
    print(f"Active neurons: {len(latest_episode.activeNeurons)}")
```

### Associative Memory

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get associative memory system
memory = pynlm.getMemorySystems(brain)
associative_memory = memory['associative']

# The brain automatically forms associations during operation
# You can access and use associative memory patterns
```

## Plasticity and Learning

### Plasticity Systems Access

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get plasticity systems
plasticity = pynlm.getPlasticitySystems(brain)

stdp = plasticity['stdp']
hebbian = plasticity['hebbian']
structural = plasticity['structural']

print("Plasticity systems available:")
print(f"STDP: {'Enabled' if stdp else 'Disabled'}")
print(f"Hebbian: {'Enabled' if hebbian else 'Disabled'}")
print(f"Structural: {'Enabled' if structural else 'Disabled'}")
```

### STDP Learning

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get STDP system
plasticity = pynlm.getPlasticitySystems(brain)
stdp = plasticity['stdp']

# STDP happens automatically during simulation
# You can monitor STDP parameters
# Note: STDP parameters are configured during brain initialization
# but can be modified if needed
```

## Agent-World Integration

### Complete Agent Setup

```python
import pynlm

# Create complete agent setup
brain, agent, world = pynlm.createCompleteAgentSetup()

# Configure world
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Initialize agent with world
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Agent setup complete!")
```

### Episode Running

```python
import pynlm

# Create agent setup
brain, agent, world = pynlm.createCompleteAgentSetup()

# Run a complete episode
print("Running episode...")
pynlm.runEpisode(brain, agent, world, steps=500)

print("Episode completed!")

# Check brain statistics
stats = pynlm.getBrainStats(brain)
print(f"Brain learned from {stats['total_spike_count']} spikes")
```

### Sensory-Motor Loop

```python
import pynlm

# Create setup
brain, agent, world = pynlm.createCompleteAgentSetup()

# Manual sensory-motor loop for advanced control
for step in range(100):
    # 1. Update world
    world.update(0.1)
    
    # 2. Get what the agent sees
    percept = world.getSensoryPercept()
    
    # 3. Tell the brain
    agent.processSensoryInput(percept)
    
    # 4. Brain thinks
    brain.step(step)
    
    # 5. Get action from brain
    action = agent.decodeMotorCommand()
    
    # 6. Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # 7. Apply learning
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)

print("Custom sensory-motor loop completed!")
```

## Advanced Features (Phase 6)

### Text Encoding and Concept Formation

```python
import pynlm

# Create agent setup with advanced capabilities
brain, agent, world = pynlm.createCompleteAgentSetup()

# Enable advanced features
agent.enableCuriosity(True)

# Text encoding (Phase 6 advanced feature)
patterns = []
words = ["hello", "world", "learning", "brain"]
for word in words:
    pattern = pynlm.encodeText(word)
    patterns.append(pattern)
    print(f"'{word}' -> {pattern}")

# Check that different words encode differently
if len(set(tuple(p) for p in patterns)) == len(words):
    print("✓ All words encode uniquely")
else:
    print("✗ Word encoding collision detected")
```

### Novelty and Curiosity

```python
import pynlm

brain, agent, world = pynlm.createCompleteAgentSetup()

# Monitor curiosity levels
for step in range(50):
    # Update world and agent
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Check curiosity levels
    curiosity_level = agent.getCuriosityLevel()
    novelty_level = agent.getNoveltyLevel()
    
    if step % 10 == 0:
        print(f"Step {step}: Curiosity={curiosity_level:.3f}, Novelty={novelty_level:.3f}")
```

## Error Handling

### Configuration Errors

```python
import pynlm

# Configuration with errors
try:
    config = pynlm.createDefaultConfig()
    # This will fail because it tries to load invalid args
    config.loadFromArgs(["--invalid_option"])
    print("Configuration loaded successfully")
except pynlm.RuntimeError as e:
    print(f"Configuration error: {e}")
except Exception as e:
    print(f"Unexpected error: {type(e).__name__}: {e}")
```

### Memory Access Errors

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Safe memory system access
memory = pynlm.getMemorySystems(brain)

if 'working' in memory:
    working = memory['working']
    print("Working memory accessible")
else:
    print("Working memory not available")

# Check if spike system exists
if hasattr(brain, 'getSpikeSystem'):
    spike_system = brain.getSpikeSystem()
    if spike_system:
        print("Spike system available")
    else:
        print("Spike system exists but is not initialized")
```

### Graceful Degradation

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Gracefully handle missing features
features = {
    'working_memory': 'working' in pynlm.getMemorySystems(brain),
    'plasticity': 'stdp' in pynlm.getPlasticitySystems(brain),
    'spike_system': brain.getSpikeSystem() is not None,
    'novelty': brain.getNovelty() is not None,
    'curiosity': brain.getCuriosity() is not None,
}

print("Available features:")
for feature, available in features.items():
    status = "✓" if available else "✗"
    print(f"  {status} {feature}")
```

## Performance Tips

### Efficient Simulation

```python
import pynlm

# Optimized brain creation and simulation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Smaller for faster testing
config.set("brain.connection_probability", 0.05)

brain = pynlm.createBrain(config)
brain.initialize()

# Batch processing for statistics
batch_size = 100
for batch_start in range(0, 1000, batch_size):
    batch_end = min(batch_start + batch_size, 1000)
    
    for step in range(batch_start, batch_end):
        brain.step(step)
    
    # Get statistics for this batch
    stats = pynlm.getBrainStats(brain)
    print(f"Batch {batch_start}-{batch_end}: {stats['firing_neurons']} firing neurons")
```

### Memory Management

```python
import pynlm

# Monitor memory usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get memory statistics
stats = pynlm.getBrainStats(brain)
print(f"Total neurons: {stats['total_neurons']}")
print(f"Total synapses: {stats['total_synapses']}")

# For large brains, consider checkpointing
if stats['total_neurons'] > 1000:
    print("Large brain detected - consider using checkpoints")
    brain.save("large_brain_checkpoint.bin")
```

## Complete Examples

### 1. Silent Brain Example

```python
"""
Simple example that creates a brain and lets it "think"
without any world interaction.
"""
import pynlm

print("=== Silent Brain Example ===")

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run silent thinking
for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### 2. Brain Watching a World

```python
"""
Example showing a brain observing a world without taking actions.
"""
import pynlm

print("=== Brain Watching World Example ===")

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
```

### 3. Complete Agent Example

```python
"""
Complete example showing an agent learning from a world.
"""
import pynlm

print("=== Complete Agent Example ===")

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")

# Final statistics
stats = pynlm.getBrainStats(brain)
print(f"Learning completed with {stats['total_spike_count']} spikes")
```

### 4. Advanced Brain Research Example

```python
"""
Advanced example showing research capabilities with 
text encoding, curiosity, and complex learning.
"""
import pynlm

print("=== Advanced Brain Research Example ===")

# Setup research brain with all advanced features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Research setup complete - brain has all advanced features")

# Text research
print("\n--- Text Encoding Research ---")
patterns = {}
words = ["apple", "banana", "computer", "learning", "neuroscience"]
for word in words:
    pattern = pynlm.encodeText(word)
    patterns[word] = pattern
    print(f"{word:12} -> {pattern}")

# Check encoding quality
unique_patterns = len(set(tuple(p) for p in patterns.values()))
if unique_patterns == len(words):
    print("✓ Perfect word encoding (all unique)")
else:
    print(f"⚠ Encoding collisions: {len(words) - unique_patterns} words share patterns")

# Run exploration with curiosity
print("\n--- Curiosity-Driven Exploration ---")
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    
    if curiosity > 0.8 and novelty > 0.5:
        print(f"Step {step}: High curiosity ({curiosity:.2f}) and novelty ({novelty:.2f})")

print("\nAdvanced research simulation complete!")
```

## Quick Reference

### Common Operations

| Operation | Function | Description |
|-----------|----------|-------------|
| Create brain | `pynlm.createBrain(config)` | Create new brain instance |
| Initialize brain | `brain.initialize()` | Initialize brain state |
| Simulate step | `brain.step(step)` | Run one simulation step |
| Get stats | `pynlm.getBrainStats(brain)` | Get comprehensive statistics |
| Run episode | `pynlm.runEpisode(brain, agent, world)` | Run complete agent episode |
| Get memory | `pynlm.getMemorySystems(brain)` | Access all memory systems |
| Get plasticity | `pynlm.getPlasticitySystems(brain)` | Access all plasticity systems |
| Encode text | `pynlm.encodeText("text")` | Encode text as neural pattern |
| Save brain | `brain.save("file.bin")` | Save brain state to file |
| Load brain | `brain.load("file.bin")` | Load brain state from file |

### Common Errors and Solutions

| Error | Cause | Solution |
|-------|-------|----------|
| `RuntimeError: Configuration load failed` | Invalid command line args | Use proper config options or file loading |
| `AttributeError: 'Brain' object has no attribute 'getSpikeSystem'` | Older API version | Use `pynlm.getSpikeSystem(brain)` instead |
| `KeyError: 'working'` | Memory systems not initialized | Check if memory systems are available |
| `TypeError: object is not iterable` | Wrong API usage | Use correct function signatures |

## Best Practices

### For Beginners:
1. Always call `brain.initialize()` before using the brain
2. Use factory methods like `pynlm.createBrainWithDefaultConfig()`
3. Check system availability before using (`if system:`)
4. Use convenience methods for common workflows

### For Advanced Users:
1. Monitor curiosity and novelty levels for exploration
2. Use checkpointing for large-scale simulations
3. Combine multiple plasticity systems for complex learning
4. Implement custom sensory-motor loops for specific applications

### For Performance:
1. Use smaller brains for testing and debugging
2. Batch statistics collection instead of frequent calls
3. Monitor memory usage and implement checkpointing for large brains
4. Use appropriate timestep sizes for your simulation speed requirements

This comprehensive examples guide provides users with practical, working code examples for all major NLM Python API features, from basic setup to advanced research capabilities.