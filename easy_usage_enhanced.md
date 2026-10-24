# NLM - Enhanced Easy Usage Guide

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## 🎯 NEW: Type Aliases for Better Code

The enhanced NLM bindings now include convenient type aliases to make your Python code cleaner and more readable:

```python
import pynlm

# Use type aliases for better code readability
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# These are all available (same as before but more explicit):
# pynlm.neuron_id, pynlm.synapse_id, pynlm.region_id, pynlm.population_id
# pynlm.action_type, pynlm.motor_command, pynlm.world_object_type
# pynlm.developmental_stage, pynlm.firing_state, pynlm.neuron_type
```

## 🚀 NEW: Convenience Methods for Configuration

Configuration is now much easier with new convenience methods:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Set value (original method still works)
config.set("brain.neuron_count", 1500)

# Get value (original method still works)
neuron_count = config.get("brain.neuron_count")

# NEW: Type-safe convenience methods
neuron_count = config.get_int("brain.neuron_count")  # Always returns int
learning_rate = config.get_float("plasticity.stdp.learning_rate", 0.001)  # With default
name = config.get_string("brain.name", "my_brain")  # With default
enabled = config.get_bool("plasticity.stdp.enable", True)  # With default

# Check if key exists
if config.has("brain.neuron_count"):
    print(f"We have {config.get_int('brain.neuron_count')} neurons!")
```

## 🏗️ NEW: Configuration Presets

Create optimized configurations for different use cases:

```python
import pynlm

# Agent-optimized configuration (for reinforcement learning)
agent_config = pynlm.create_agent_config()
print(f"Agent config uses {agent_config.get_int('brain.neuron_count')} neurons")
print(f"Learning rate: {agent_config.get_float('plasticity.stdp.learning_rate')}")

# Rendering-optimized configuration (for visualization)
render_config = pynlm.create_rendering_config()
print(f"Rendering config uses {render_config.get_int('brain.neuron_count')} neurons")
print(f"Visualization enabled: {render_config.get_bool('visualization.enabled')}")
```

## ⚡ NEW: Convenience Functions

Many common operations are now available as convenience functions:

### Run a Complete Simulation

```python
import pynlm

# Create all components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run complete simulation with one line
result_brain = pynlm.run_simulation(brain, world, agent, 500)
print(f"Simulation completed with {result_brain.getTotalSpikeCount()} spikes")
```

### Train an Agent

```python
import pynlm

# Setup training
config = pynlm.create_agent_config()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Train the agent (training modifies the brain in place)
trained_brain = pynlm.train_agent(brain, world, agent, 20, 50)
print("Agent training complete!")
```

## 🔄 NEW: Context Managers for Safe Resource Management

Use context managers for safe and clean resource management:

### Simulation Context Manager

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation safely with automatic cleanup
with pynlm.SimulationContext(brain, world, agent) as sim:
    sim.run(100)  # Run 100 steps
    stats = sim.get_stats()
    print(f"Simulation ran {stats['steps_run']} steps")

# Context automatically handles cleanup
```

### Configuration Context Manager

```python
import pynlm

config = pynlm.createDefaultConfig()

# Modify configuration with automatic rollback on error
with pynlm.ConfigContext(config) as cfg:
    cfg.set("brain.neuron_count", 3000)
    cfg.apply_preset("agent")
    print(f"Neuron count: {config.get_int('brain.neuron_count')}")

# Configuration is automatically cleaned up
```

## 💾 NEW: Checkpoint Management

Save and load complete simulation states:

```python
import pynlm
import tempfile

# Create and run a simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Simulation has {brain.getTotalSpikeCount()} spikes")

# Save checkpoint
with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as tmp:
    checkpoint_path = tmp.name

pynlm.save_checkpoint(brain, checkpoint_path)
print(f"Checkpoint saved to {checkpoint_path}")

# Create new brain and load checkpoint
new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
new_brain.initialize()
pynlm.load_checkpoint(new_brain, checkpoint_path)

print(f"Loaded brain has {new_brain.getTotalSpikeCount()} spikes (preserved!)")
```

## 🧪 NEW: Deep Copy Functionality

Create independent copies of brain objects:

```python
import pynlm

# Create original brain
original_brain = pynlm.createBrain(pynlm.createDefaultConfig())
original_brain.initialize()

# Modify original
for i in range(50):
    original_brain.step(i)

# Create deep copy (independent of original)
copied_brain = pynlm.deepcopy_brain(original_brain)

# Modify copy - original is unchanged!
for i in range(50, 100):
    copied_brain.step(i)

print(f"Original spikes: {original_brain.getTotalSpikeCount()}")
print(f"Copied spikes: {copied_brain.getTotalSpikeCount()}")
```

## 📊 NEW: Profiling and Benchmarking

Profile your simulations for performance optimization:

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Benchmark brain performance
metrics = pynlm.benchmark_brain(brain, 200, 50)
print(f"Steps per second: {metrics['steps_per_second']:.2f}")
print(f"Memory usage: {metrics['memory_usage']:.2f} MB")

# Profile complete simulation
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

profile = pynlm.profile_simulation(brain, world, agent, 100)
print(f"Total time: {profile['total_time']:.3f} seconds")
print(f"Brain time: {profile['brain_time']:.3f} seconds")
print(f"World time: {profile['world_time']:.3f} seconds")
```

## 🔍 NEW: Configuration Validation and Formatting

Validate and format your configurations:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Add some test configurations
config.set("valid.key1", "value1")
config.set("valid.key2", 123)
config.set("invalid..key", "problematic")  # Key with dots might be invalid

# Validate configuration
errors = pynlm.validate_config(config)
print("Validation errors:", errors)

# Format configuration for display
formatted = pynlm.format_config(config, indent=2)
print("Formatted config:")
print(formatted)
```

## 📚 NEW: Utility Functions and Information

Get version information and utilities:

```python
import pynlm

# Get version information
print(pynlm.get_version())

# Get complete default configuration as dictionary
import pprint
default_config = pynlm.get_default_config()
pprint.pprint(default_config)

# Use formatted config in your application
config_dict = {k: config.get(k) for k in config.getKeys()}
print("Your current config:", config_dict)
```

## 🎯 Complete Advanced Example

Here's a complete example showing all the new features:

```python
import pynlm
import tempfile

def advanced_agent_training():
    """Advanced agent training with all new features."""
    
    # 1. Create optimized configuration
    config = pynlm.create_agent_config()
    
    # 2. Create brain and enable learning
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Setup agent with world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    agent.initialize(world)
    
    # 4. Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # 5. Run training with context manager for safety
    with pynlm.SimulationContext(brain, world, agent) as sim:
        # Training loop
        for episode in range(10):
            print(f"Episode {episode + 1}/10")
            
            # Reset episode
            world.reset()
            agent.reset()
            
            # Run episode
            sim.run(100)
            
            # Print statistics
            stats = sim.get_stats()
            print(f"  Steps: {stats['steps_run']}, Firing rate: {stats['avg_firing_rate']:.2f}")
        
        # Save checkpoint
        with tempfile.NamedTemporaryFile(suffix='_checkpoint.json', delete=False) as tmp:
            checkpoint_path = tmp.name
        
        pynlm.save_checkpoint(brain, checkpoint_path)
        print(f"Checkpoint saved: {checkpoint_path}")
    
    # 6. Create independent copy for testing
    test_brain = pynlm.deepcopy_brain(brain)
    
    # 7. Get final statistics
    final_stats = {
        "total_spikes": brain.getTotalSpikeCount(),
        "avg_firing_rate": brain.getAverageFiringRate(),
        "curiosity": agent.getCuriosityLevel(),
        "novelty": agent.getNoveltyLevel(),
        "dev_stage": str(brain.getDevelopmentalStage()),
        "neuron_count": brain.getTotalNeuronCount(),
        "synapse_count": brain.getTotalSynapseCount()
    }
    
    return final_stats

# Run the advanced example
print("Running advanced agent training...")
results = advanced_agent_training()
print("\nTraining complete! Results:")
for key, value in results.items():
    print(f"  {key}: {value}")
```

## 🔧 Configuration Presets Reference

### Agent Preset (`create_agent_config()`)
- **brain.neuron_count**: 2000 (more neurons for complex behavior)
- **brain.synapse_density**: 0.15 (higher connectivity)
- **plasticity.stdp.enable**: true (enable spike-timing-dependent plasticity)
- **neuromod.dopamine.scale**: 1.5 (stronger reward signals)
- **neuromod.curiosity.enable**: true (enable curiosity-driven exploration)

### Rendering Preset (`create_rendering_config()`)
- **brain.neuron_count**: 500 (fewer neurons for faster rendering)
- **visualization.enabled**: true (enable visualization)
- **visualization.render_rate**: 30 (frames per second)

## 📖 API Reference Summary

### New Convenience Functions
- `pynlm.run_simulation(brain, world, agent, steps)` - Run complete simulation
- `pynlm.train_agent(brain, world, agent, episodes, steps_per_episode)` - Train agent
- `pynlm.create_agent_config()` - Create agent-optimized config
- `pynlm.create_rendering_config()` - Create visualization config
- `pynlm.save_checkpoint(brain, path, world)` - Save simulation state
- `pynlm.load_checkpoint(brain, path, world)` - Load simulation state
- `pynlm.benchmark_brain(brain, steps, warmup)` - Benchmark brain performance
- `pynlm.profile_simulation(brain, world, agent, steps)` - Profile complete simulation
- `pynlm.explore_states(brain, count)` - Explore neural states
- `pynlm.analyze_behavior(brain, world, agent, steps)` - Analyze behavior patterns
- `pynlm.get_version()` - Get version information
- `pynlm.get_default_config()` - Get default config as dictionary
- `pynlm.validate_config(config)` - Validate configuration
- `pynlm.format_config(config, indent)` - Format configuration
- `pynlm.deepcopy_brain(brain)` - Create deep copy

### New Context Managers
- `pynlm.SimulationContext(brain, world, agent)` - Safe simulation execution
- `pynlm.ConfigContext(config)` - Safe configuration modification

### New Type Aliases
- `pynlm.neuron_id`, `pynlm.synapse_id`, etc. - Type-safe aliases

### Enhanced Configuration Methods
- `config.get_int(key, default)` - Get integer value
- `config.get_float(key, default)` - Get float value
- `config.get_string(key, default)` - Get string value
- `config.get_bool(key, default)` - Get boolean value

## 🚀 Getting Started with New Features

1. **Read the examples** in this guide to see the new features in action
2. **Install the enhanced bindings** by rebuilding with the updated CMakeLists.txt
3. **Try the complete examples** to see how all features work together
4. **Use context managers** for production code to ensure proper cleanup
5. **Save checkpoints** during long training sessions
6. **Profile your simulations** to optimize performance

Enjoy the enhanced NLM Python bindings with all the new convenience features, improved type safety, and better resource management!