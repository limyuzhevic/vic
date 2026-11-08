# NLM - Enhanced Python API Usage Guide

## Overview

This guide provides comprehensive examples and documentation for using the enhanced NLM Python API. The new Python bindings offer powerful features including:

- Pythonic properties and convenience methods
- Comprehensive statistics and validation tools
- Advanced configuration management
- Simulation utilities and helpers

## Quick Start (Enhanced)

### Example 1: Basic Brain with Statistics

```python
import pynlm

# Create brain with configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get comprehensive statistics
stats = brain.getStats()
print(f"Neurons: {stats['neuron_count']}")
print(f"Firing neurons: {stats['firing_count']}")
print(f"Total spikes: {stats['spike_count']}")
print(f"Average firing rate: {stats['avg_firing_rate']:.2f} Hz")
print(f"E/I ratio: {stats['ei_ratio']:.2f}")
print(f"Developmental stage: {stats['developmental_stage']}")

# Validate brain state
validation = brain.validate()
if not validation['valid']:
    print("Validation errors:", list(validation['errors']))
if validation['warnings']:
    print("Warnings:", list(validation['warnings']))
```

### Example 2: Agent with Enhanced Processing

```python
import pynlm
import math

# Setup complete agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all processing systems (convenience method)
agent.configure_processing(
    reward_modulation=True,
    structural_plasticity=True,
    development=True,
    curiosity=True
)

# Enhanced simulation loop
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input and update development
    agent.update_all(percept, 0.1)
    
    # Decode motor command
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward if any
    if step % 100 == 0:
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.apply_reward(reward)
    
    # Print system status periodically
    if step % 100 == 0:
        status = agent.get_system_status()
        print(f"Step {step}: {status['stats']['curiosity_level']:.3f} curiosity")
        print(f"  Developmental stage: {status['developmental_stage']}")

print("Simulation complete!")
```

### Example 3: Action Management

```python
import pynlm

# Create action with parameters
action = pynlm.Action(pynlm.ActionType.MoveForward)
action.setParameter("speed", 1.0)
action.setParameter("duration", 2.0)

# Access parameters conveniently
speed = action.getParameter("speed")
duration = action.getParameter("duration")

print(f"Action: {action.getType()}")
print(f"Speed: {speed}, Duration: {duration}")

# Execute action with result
result = action.execute()
print(f"Execution success: {result['success']}")
print(f"Action name: {result['action']}")

# Clone action
cloned_action = action.clone()
print(f"Cloned action type: {cloned_action.getType()}")
```

### Example 4: World with Pythonic Features

```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
world.reset()

# Set agent start
agent_start_x, agent_start_y = 15.0, 15.0
world.setAgentStart(agent_start_x, agent_start_y)

# Pythonic world properties
center_x, center_y = world.center
print(f"World center: ({center_x}, {center_y})")

# Calculate distances and headings
distance = world.getDistance(0, 0, center_x, center_y)
heading = world.getHeading(0, 0, center_x, center_y)
print(f"Distance to center: {distance:.2f}")
print(f"Heading to center (rad): {heading:.2f}")

# Add and check objects
world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0))
world.addObject(pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Hazard, value=5.0))

print(f"Agent at valid position: {world.isAgentAt(center_x, center_y)}")
print(f"Agent bounds: ({world.getAgentBounds()[0]}, {world.getAgentBounds()[1]})")

# World statistics
print(f"World size: {world.getWidth()} x {world.getHeight()}")
print(f"Max energy: {world.getMaxEnergy()}")
```

### Example 5: Configuration Management

```python
import pynlm

# Create and configure using Python API
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 1.2)
config.set("neuromod.curiosity.enable", True)

# Save and reload configuration
config.saveToFile("my_config.json")

# Create a new config and load saved one
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")

print(f"Original neuron count: {config.get('brain.neuron_count')}")
print(f"Loaded neuron count: {config2.get('brain.neuron_count')}")

# Get configuration keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Configuration summary
print(f"Configuration summary:\n{config.summary()}")
```

### Example 6: Advanced Agent with Text Association

```python
import pynlm

# Setup agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable text processing (Phase 6 feature)
agent.enableCuriosity(True)

# Encode text (creates internal representation)
text_pattern = pynlm.encodeText("hello world")
print(f"Text pattern encoded: {text_pattern}")

# Create a simple world for text association
for i, char in enumerate("hello"):
    x = 5 + i * 2
    y = 5
    world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Marker, value=1.0))

# Run simulation with text
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Check if agent has learned text association
    internal_rates = agent.getInternalFiringRates()
    if any(rate > 0.5 for rate in internal_rates):
        print(f"Step {step}: Possible text association detected!")
        break

print("Text association simulation complete")
```

## Advanced Features

### 1. Statistics Export

```python
# Get CSV format for data analysis
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

csv_data = brain.getStatsCSV()
print("Brain statistics (CSV):")
print(csv_data)

# Parse CSV data (can be imported into pandas, etc.)
lines = csv_data.strip().split('\n')
for line in lines:
    key, value = line.split(',')
    print(f"{key}: {value}")
```

### 2. Batch Processing

```python
import pynlm

def run_simulation_batch(num_simulations, steps_per_sim):
    """Run multiple simulations with different random seeds"""
    results = []
    
    for sim_id in range(num_simulations):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps_per_sim):
            brain.step(step)
        
        stats = brain.getStats()
        stats['simulation_id'] = sim_id
        results.append(stats)
    
    return results

# Run batch simulation
batch_results = run_simulation_batch(10, 500)

# Analyze results
total_spikes = sum(r['spike_count'] for r in batch_results)
avg_firing_rate = sum(r['avg_firing_rate'] for r in batch_results) / len(batch_results)

print(f"Batch results:")
print(f"Total simulations: {len(batch_results)}")
print(f"Average firing rate: {avg_firing_rate:.2f} Hz")
print(f"Total spikes across all sims: {total_spikes}")
```

### 3. Error Handling and Validation

```python
import pynlm

# Create brain and validate
def safe_simulation(config, steps):
    """Run simulation with comprehensive error handling"""
    try:
        brain = pynlm.createBrain(config)
        
        # Validate before initialization
        validation = brain.validate()
        if validation['warnings']:
            print("Warnings:", [str(w) for w in validation['warnings']])
        
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
            
            # Validate at checkpoints
            if step % 100 == 0:
                step_validation = brain.validate()
                if not step_validation['valid']:
                    print(f"Invalid state at step {step}")
        
        return True
        
    except Exception as e:
        print(f"Simulation failed: {e}")
        return False

# Usage with error handling
config = pynlm.createDefaultConfig()
success = safe_simulation(config, 1000)
if success:
    print("Simulation completed successfully")
else:
    print("Simulation had errors")
```

## API Reference Summary

### Key New Features:

1. **Brain Statistics Methods:**
   - `brain.getStats()` - Returns comprehensive statistics as dictionary
   - `brain.getStatsCSV()` - Returns statistics as CSV string
   - `brain.validate()` - Validates brain state

2. **Agent Processing Methods:**
   - `agent.configure_processing()` - Enable/disable all processing systems
   - `agent.update_all()` - Convenience method for sensory processing and development
   - `agent.get_system_status()` - Get complete system status
   - `agent.clone()` - Clone agent brain
   - `agent.copy_config()` - Copy configuration from another agent

3. **Action Management:**
   - `action.getParameter()` - Get parameter by index
   - `action.setParameter()` - Set parameter by index
   - `action.addParameter()` - Add new parameter
   - `action.execute()` - Execute action with result
   - `action.clone()` - Clone action

4. **World Pythonic Features:**
   - `world.center` - Get center position as property
   - `world.getDistance()` - Calculate distance between points
   - `world.getHeading()` - Calculate heading angle
   - `world.isAgentAt()` - Check if agent can be at position
   - `world.getAgentBounds()` - Get agent bounds

5. **Enhanced Configuration:**
   - Better type conversion and error handling
   - Configuration validation
   - Enhanced save/load capabilities

## Performance Tips

1. **Use batch operations:** Combine multiple operations where possible
2. **Reuse configurations:** Create and reuse configuration objects
3. **Monitor statistics:** Use `brain.getStats()` to track simulation progress
4. **Implement checkpoints:** Use `brain.save()` and `brain.load()` for long simulations
5. **Validate regularly:** Use `brain.validate()` to catch issues early

## Troubleshooting

### Common Issues:

1. **"My brain isn't doing anything"**
   - Did you call `brain.initialize()` before `brain.step()`?
   - Try increasing the number of steps
   - Check `brain.validate()` for warnings

2. **Import errors**
   - Ensure `pynlm` is properly installed
   - Try: `pip install --force-reinstall .`

3. **Segmentation faults**
   - Always call `brain.initialize()` before `brain.step()`
   - Ensure `brain.reset()` is called before re-running simulations
   - Check configuration parameters

### Getting Help:

- Check the `NLM/Phase6_INTEGRATION.md` for Phase 6 specific features
- Review the `docs/ARCHITECTURE.md` for system architecture
- Look at the `examples/` directory for more complete examples

This enhanced API provides a much more powerful and user-friendly interface to the NLM framework while maintaining full compatibility with the existing API.