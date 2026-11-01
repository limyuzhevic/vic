# Advanced NLM Usage Guide

This guide provides advanced usage patterns for the NLM Python API. It builds upon the basic examples in `easy_usage.md` and covers more sophisticated usage scenarios for power users.

## Advanced Topics

### 1. JSON Configuration Management

NLM now supports advanced JSON configuration files, making it easier to configure complex brain simulations.

#### Configuration from JSON String

```python
import pynlm

# Advanced configuration with multiple parameters
advanced_config = """
{
    "brain": {
        "neuron_count": 5000,
        "region_count": 3,
        "connection_probability": 0.15,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.08
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.002,
            "tau_plus": 25.0,
            "tau_minus": 25.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.001
        }
    },
    "neuromodulation": {
        "dopamine": {
            "scale": 1.5
        },
        "curiosity": {
            "enable": true
        }
    }
}
"""

config = pynlm.createConfigFromJSON(advanced_config)
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Configuration from File

```python
# Save configuration to file
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.001)
config.saveToFile("my_config.json")

# Load configuration from file
config = pynlm.createConfigFromFile("my_config.json")
```

### 2. Batch Operations and Performance Monitoring

For large-scale simulations, batch operations provide better performance monitoring and control.

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)

brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)

# Enable all advanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation in batches with performance monitoring
batch_size = 1000
num_batches = 20

for batch in range(num_batches):
    batch_start_time = world.getSimulationTime()
    batch_steps = min(batch_size, 50000 - batch * batch_size)
    
    for step in range(batch_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(batch * batch_size + step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    batch_end_time = world.getSimulationTime()
    batch_duration = batch_end_time - batch_start_time
    
    # Get performance statistics
    firing_rate = brain.getAverageFiringRate()
    neurons_firing = brain.getFiringNeuronCount()
    total_spikes = brain.getTotalSpikeCount()
    curiosity_level = agent.getCuriosityLevel()
    novelty_level = agent.getNoveltyLevel()
    
    print(f"Batch {batch + 1}: {batch_steps} steps in {batch_duration:.2f}s, "
          f"firing_rate={firing_rate:.1f}Hz, curiosity={curiosity_level:.3f}")
```

### 3. Reinforcement Learning with Reward Shaping

Implement sophisticated reinforcement learning scenarios with custom reward functions.

```python
def get_reward(action, percept, agent):
    """Custom reward function that balances exploration and exploitation"""
    reward = 0.0
    
    # Progress reward
    if action == pynlm.MotorCommand.MoveForward:
        reward += 1.0
    
    # Exploration bonus based on curiosity
    if agent.getCuriosityLevel() > 0.5:
        reward += 0.5
        
    # Hazard avoidance penalty
    vision = percept.getVision()
    if any(v < 0.3 for v in vision):
        reward -= 2.0
        
    # Resource discovery reward
    if any(v > 0.7 for v in vision):
        reward += 1.5
        
    return reward

# Training loop with reward shaping
brain = pynlm.createBrain()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30)

agent.enableRewardModulation(True)
agent.enableCuriosity(True)

world.reset()
total_reward = 0.0

for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    
    reward = get_reward(action, percept, agent)
    predicted_reward = agent.getPredictionError()
    agent.applyRewardModulation(reward, predicted_reward)
    
    world.applyMotorCommand(action, world.getSimulationTime())
    total_reward += reward
    
    if step % 100 == 0:
        print(f"Step {step}: reward={reward:.2f}, "
              f"curiosity={agent.getCuriosityLevel():.3f}, "
              f"total={total_reward:.2f}")
```

### 4. Checkpoint Persistence and Brain Saving

Save and load brain states for long-running simulations and persistence across sessions.

```python
import pynlm
from pathlib import Path

# Create and train a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some training steps
for step in range(500):
    brain.step(step)

# Save checkpoint
checkpoint_file = "trained_brain.bin"
brain.save(checkpoint_file)
print(f"Saved checkpoint to {checkpoint_file}")

# Create new brain and load checkpoint
config2 = pynlm.createDefaultConfig()
brain2 = pynlm.createBrain(config2)

if brain2.load(checkpoint_file):
    print("Checkpoint loaded successfully!")
    print(f"Loaded brain has {brain2.getTotalNeuronCount()} neurons")
else:
    print("Failed to load checkpoint!")

# Clean up
Path(checkpoint_file).unlink(missing_ok=True)
```

### 5. Advanced World Interaction

Create complex environments with multiple object types and interaction patterns.

```python
# Create advanced world with multiple object types
world = pynlm.createSimpleWorld()
world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
world.setMaxEnergy(200.0)

# Add various world objects
world.addObject(pynlm.WorldObject(x=10.0, y=10.0, type=pynlm.WorldObjectType.Resource, value=10.0))
world.addObject(pynlm.WorldObject(x=30.0, y=30.0, type=pynlm.WorldObjectType.Resource, value=15.0))
world.addObject(pynlm.WorldObject(x=20.0, y=20.0, type=pynlm.WorldObjectType.Hazard, value=5.0))
world.addObject(pynlm.WorldObject(x=5.0, y=35.0, type=pynlm.WorldObjectType.Wall, value=1.0))

# Configure advanced world parameters
world.configureAdvanced(
    width=40, height=40, visionWidth=16, visionHeight=16,
    description="Complex exploration environment",
    energyDecayRate=0.95,
    maxEnergy=200.0
)

# Create agent with world
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
```

### 6. Advanced Configuration Validation

Validate configuration parameters before creating a brain.

```python
def validate_config(config):
    """Validate configuration parameters"""
    errors = []
    
    # Check required parameters
    if not config.has("brain.neuron_count"):
        errors.append("brain.neuron_count is required")
    elif config.get("brain.neuron_count") < 100:
        errors.append("brain.neuron_count must be at least 100")
        
    if not config.has("brain.synapse_density"):
        errors.append("brain.synapse_density is required")
    elif config.get("brain.synapse_density") < 0.0 or config.get("brain.synapse_density") > 1.0:
        errors.append("brain.synapse_density must be between 0.0 and 1.0")
    
    # Check consistency
    if config.has("brain.neuron_count") and config.has("brain.region_count"):
        neurons_per_region = config.get("brain.neuron_count") / config.get("brain.region_count")
        if neurons_per_region < 10:
            errors.append("Too few neurons per region for meaningful simulation")
    
    return errors

# Example usage
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.region_count", 5)
config.set("brain.synapse_density", 0.1)

validation_errors = validate_config(config)
if validation_errors:
    print("Configuration errors:")
    for error in validation_errors:
        print(f"  - {error}")
else:
    print("Configuration is valid!")
```

### 7. Performance Optimization Techniques

Advanced techniques for optimizing simulation performance.

```python
def optimize_for_performance(brain, agent, world):
    """Apply performance optimizations to a running simulation"""
    
    # Set appropriate timestep for the simulation scale
    simulation_timestep = 0.001  # 1ms for biological accuracy
    world.update(simulation_timestep)
    
    # Adjust brain parameters for performance
    config = brain.getConfig()
    
    # Optimize neuron count for performance
    current_neuron_count = brain.getTotalNeuronCount()
    if current_neuron_count > 2000:
        # Reduce neuron count for faster simulation
        config.set("brain.neuron_count", 2000)
        # Reinitialize brain (would need to recreate in practice)
    
    # Enable performance monitoring
    def get_performance_stats():
        """Get current performance statistics"""
        return {
            "neurons": brain.getTotalNeuronCount(),
            "firing_rate": brain.getAverageFiringRate(),
            "energy": agent.getBrain().getLearnedPreference(),
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel()
        }
    
    return get_performance_stats

# Usage
stats = optimize_for_performance(brain, agent, world)
print("Current performance stats:", stats())
```

## Best Practices for Advanced Usage

### 1. Configuration Management

- Use JSON files for complex configurations
- Validate configuration before brain initialization
- Keep configuration files separate from code
- Use descriptive parameter names

### 2. Simulation Management

- Use batch operations for large simulations
- Monitor performance regularly
- Implement checkpointing for long runs
- Use appropriate timesteps for your simulation scale

### 3. Memory Management

- Enable checkpointing to save brain state
- Monitor memory usage in long-running simulations
- Use cleanup functions appropriately
- Consider checkpoint frequency based on simulation complexity

### 4. Error Handling

- Always validate inputs before use
- Implement proper error handling for file I/O
- Check for simulation stability
- Monitor for unusual behavior patterns

## Troubleshooting

### Common Issues and Solutions

1. **Configuration Loading Errors**
   ```python
   # Solution: Use try-catch and validate file existence
   try:
       config = pynlm.createConfigFromFile("config.json")
   except Exception as e:
       print(f"Failed to load configuration: {e}")
   ```

2. **Simulation Performance Issues**
   ```python
   # Solution: Check neuron count and adjust configuration
   if brain.getTotalNeuronCount() > 2000:
       print("Consider reducing neuron count for better performance")
   ```

3. **Checkpoint Loading Failures**
   ```python
   # Solution: Check file existence and format
   if not brain.load("checkpoint.bin"):
       print("Checkpoint file not found or corrupted")
   ```

## Advanced Features Reference

### Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.region_count` | int | 2 | Number of brain regions |
| `brain.synapse_density` | float | 0.1 | Connection probability |
| `plasticity.stdp.enable` | bool | true | Enable STDP plasticity |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian plasticity |
| `neuromodulation.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromodulation.novelty.enable` | bool | true | Enable novelty detection |
| `development.synaptogenesis_rate` | float | 0.001 | Rate of new synapse formation |
| `development.pruning_rate` | float | 0.0001 | Rate of synapse removal |

### Performance Monitoring APIs

- `brain.getAverageFiringRate()` - Current average firing rate
- `brain.getTotalSpikeCount()` - Total spikes since initialization
- `agent.getCuriosityLevel()` - Current curiosity level
- `agent.getNoveltyLevel()` - Current novelty detection level
- `agent.getNeuromodulationLevel()` - Current neuromodulation level

### Advanced World Configuration

```python
# Advanced world configuration
world.configureAdvanced(
    width=100, height=100,
    visionWidth=32, visionHeight=32,
    description="Large scale environment",
    energyDecayRate=0.99,
    maxEnergy=1000.0
)
```

This guide provides a foundation for advanced NLM usage. As the framework evolves, more advanced features and optimization techniques will be added.

## See Also

- [basic_usage.md](./basic_usage.md) - Basic usage examples
- [architecture.md](./architecture.md) - System architecture documentation
- [api_reference.md](./api_reference.md) - Complete API documentation
