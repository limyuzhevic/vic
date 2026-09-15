# Advanced Usage Guide with Pythonic API

## New Pythonic API Introduction

The NLM Python bindings have been enhanced with a more Pythonic API that makes the system easier to use and understand. Instead of the previous CamelCase naming convention, the new API uses snake_case which is more Pythonic and easier to remember.

## New Convenience Functions

### Configuration Helpers

```python
# Old API
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.saveToFile("config.json")

# New API
config = pynlm.create_default_config()
config.set("brain.neuron_count", 1000)
pynlm.save_config_to_file(config, "config.json")

# Or load from file
config = pynlm.load_config_from_file("config.json")
```

### World Setup

```python
# Old API
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# New API
world = pynlm.create_world()
pynlm.setup_world(world, 20, 20, 8, 8, 10.0, 10.0)
```

### Agent Helpers

```python
# Enable all learning systems at once (convenience function)
pynlm.enable_all_systems(agent)

# Check which systems are enabled
systems = pynlm.is_learning_enabled(agent)
print("Reward modulation:", systems["reward_modulation"])
print("Development:", systems["development"])
print("Curiosity:", systems["curiosity"])
```

### Simulation Helpers

```python
# Run simple brain simulation
pynlm.run_simulation(brain, 1000)

# Run complete brain-world-agent simulation
pynlm.run_world_simulation(brain, world, agent, 1000)

# Run with custom reward function
def custom_reward():
    return agent.get_curiosity_level() * 10.0

pynlm.run_simulation_with_reward(brain, world, agent, 100, custom_reward)
```

### Statistics

```python
# Get comprehensive brain statistics as a dictionary
stats = pynlm.get_brain_stats(brain)
print(f"Neurons: {stats['total_neurons']}")
print(f"Firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"Development stage: {stats['development_stage']}")

# Get memory system statistics
wm_stats = pynlm.get_working_memory_stats(brain)
ep_stats = pynlm.get_episodic_memory_stats(brain)

print(f"Working memory active traces: {wm_stats['active_traces']}")
print(f"Episodic memory episodes: {ep_stats['episode_count']}")
```

### Constants for Easy Access

```python
# Old way: pynlm.NeuronType.Excitatory
# New way: pynlm.NEURON_TYPES["excitatory"]

exc_type = pynlm.NEURON_TYPES["excitatory"]
hyp_type = pynlm.NEURON_TYPES["inhibitory"]

# Development stages
initial_stage = pynlm.DEVELOPMENTAL_STAGES["initial"]
adult_stage = pynlm.DEVELOPMENTAL_STAGES["adult"]
```

## Advanced Example: Complete Agent with Learning

```python
import pynlm

def run_advanced_agent_simulation(config, world_config, simulation_steps):
    """Advanced agent simulation with comprehensive learning and monitoring."""
    
    # Create brain with Pythonic API
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Create world with Pythonic helper
    world = pynlm.create_world()
    pynlm.setup_world(world, world_config["width"], world_config["height"], 
                     world_config["visionWidth"], world_config["visionHeight"],
                     world_config["agentX"], world_config["agentY"])
    
    # Create agent interface
    agent = pynlm.create_agent(brain)
    agent.initialize(world)
    
    # Enable all learning systems at once
    pynlm.enable_all_systems(agent)
    
    # Track simulation statistics
    simulation_stats = {
        "steps_completed": 0,
        "total_spikes": 0,
        "neurons_firing": 0,
        "learning_events": 0,
        "curiosity_levels": []
    }
    
    # Custom reward function based on agent's curiosity and world state
    def get_custom_reward():
        # Combine curiosity with world exploration
        curiosity_reward = agent.get_curiosity_level() * 2.0
        # Additional reward for novelty exploration
        novelty_reward = agent.get_novelty_level() * 1.0
        
        simulation_stats["learning_events"] += 1
        simulation_stats["curiosity_levels"].append(agent.get_curiosity_level())
        
        return curiosity_reward + novelty_reward
    
    # Run simulation with Pythonic helper
    pynlm.run_simulation_with_reward(
        brain, world, agent, simulation_steps, get_custom_reward
    )
    
    # Get final comprehensive statistics
    final_stats = pynlm.get_brain_stats(brain)
    final_stats.update(simulation_stats)
    
    # Memory system status
    final_stats["working_memory"] = pynlm.get_working_memory_stats(brain)
    final_stats["episodic_memory"] = pynlm.get_episodic_memory_stats(brain)
    
    return final_stats, brain, world, agent

# Usage example
if __name__ == "__main__":
    # Configuration
    config = pynlm.create_default_config()
    
    world_config = {
        "width": 30,
        "height": 30,
        "visionWidth": 8,
        "visionHeight": 8,
        "agentX": 15.0,
        "agentY": 15.0
    }
    
    # Run advanced simulation
    stats, brain, world, agent = run_advanced_agent_simulation(
        config, world_config, 500
    )
    
    # Print results
    print("=== Advanced Agent Simulation Results ===")
    print(f"Steps completed: {stats['steps_completed']}")
    print(f"Total spikes: {stats['total_spikes']}")
    print(f"Learning events: {stats['learning_events']}")
    print(f"Average curiosity: {sum(stats['curiosity_levels'])/len(stats['curiosity_levels']):.3f}")
    print(f"Working memory traces: {stats['working_memory']['active_traces']}")
    print(f"Episodic memory episodes: {stats['episodic_memory']['episode_count']}")
    
    # System health check
    systems_enabled = pynlm.is_learning_enabled(agent)
    print("\nLearning Systems Status:")
    for system, enabled in systems_enabled.items():
        status = "ENABLED" if enabled else "DISABLED"
        print(f"  {system}: {status}")
```

## Comparison Table: Old vs New API

| Feature | Old API (CamelCase) | New API (Pythonic) |
|---------|---------------------|--------------------|
| Create brain | `pynlm.createBrain(config)` | `pynlm.create_brain(config)` |
| Create config | `pynlm.createDefaultConfig()` | `pynlm.create_default_config()` |
| Create world | `pynlm.createSimpleWorld()` | `pynlm.create_world()` |
| Create agent | `pynlm.createAgentBrain(brain)` | `pynlm.create_agent(brain)` |
| Setup world | Manual (3 lines) | `pynlm.setup_world(world, w, h, vw, vh, ax, ay)` |
| Enable all systems | 4 separate calls | `pynlm.enable_all_systems(agent)` |
| Get brain stats | Individual calls | `pynlm.get_brain_stats(brain)` |
| Load config | Manual | `pynlm.load_config_from_file("file.json")` |
| Save config | Manual | `pynlm.save_config_to_file(config, "file.json")` |
| Constants | `pynlm.NeuronType.Excitatory` | `pynlm.NEURON_TYPES["excitatory"]` |

## Why the Pythonic API?

1. **Consistency**: Uses snake_case like other Python libraries
2. **Brevity**: Many functions do multiple operations in one call
3. **Discoverability**: Constants in dictionaries are easier to browse
4. **Convenience**: Helper functions eliminate common boilerplate code
5. **Integration**: Better fits with Python ecosystem

## Migration Guide

### Basic Migration

**Old:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Run simulation
for i in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

**New:**
```python
import pynlm

brain = pynlm.create_brain(pynlm.create_default_config())
world = pynlm.create_world()
pynlm.setup_world(world, 10, 10, 8, 8, 5.0, 5.0)
agent = pynlm.create_agent(brain)
agent.initialize(world)

# Run simulation with helper
pynlm.run_world_simulation(brain, world, agent, 100)
```

### Advanced Migration

**Old:**
```python
# Enable learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Check status
print("Reward:", agent.isRewardModulationEnabled())
print("Development:", agent.isDevelopmentEnabled())
```

**New:**
```python
# Enable all at once
pynlm.enable_all_systems(agent)

# Check status in one call
systems = pynlm.is_learning_enabled(agent)
print("Reward:", systems["reward_modulation"])
print("Development:", systems["development"])
```

### Migration Benefits

1. **Less boilerplate**: Many common operations are now one function call
2. **Better error handling**: Built-in validation and error reporting
3. **More discoverable**: Dictionary constants are easier to browse
4. **More Pythonic**: Follows Python conventions (snake_case, PEP8)
5. **More consistent**: All helper functions follow similar patterns

## Quick Start Summary

### New Users:
```python
# Most common operations in 1-2 lines
brain = pynlm.create_brain(pynlm.create_default_config())
pynlm.run_simulation(brain, 100)  # Done!
```

### Power Users:
```python
# Advanced features with minimal code
stats = pynlm.run_simulation_with_reward(
    brain, world, agent, 500, 
    lambda: agent.get_curiosity_level() * 10.0
)
```

The Pythonic API makes NLM easier to use while maintaining all the power of the original C++ backend.
