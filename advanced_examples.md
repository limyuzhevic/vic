# NLM Python Bindings - Advanced Usage Examples

This document provides advanced usage examples for the NLM Python bindings, showcasing the new features and improvements made to enhance the Python API.

## 1. Context Manager Usage

The NLM classes now support Python context managers for automatic resource management and cleanup:

```python
import pynlm

# Using context manager for automatic cleanup
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    for step in range(100):
        brain.step(step)
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Context manager ensures proper cleanup even if an exception occurs
try:
    with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
        brain.initialize()
        raise RuntimeError("Simulated error")
except RuntimeError as e:
    print(f"Error caught: {e}")
    # Brain is still properly cleaned up
```

## 2. Fluent Interface

Configuration and other objects now support fluent interface methods for chaining:

```python
import pynlm

# Build configuration with fluent interface
config = (pynlm.createDefaultConfig()
          .enableRewardModulation(True)
          .enableCuriosity(True)
          .enableDevelopment(True)
          .enableStructuralPlasticity(True)
          .set("brain.neuron_count", 2000))

brain = pynlm.createBrain(config)
brain.initialize()

# Chain brain operations
(brain.step(0)
 .enableRewardModulation(True)
 .enableCuriosity(True))

# Configure world with fluent interface
world = (pynlm.createSimpleWorld()
         .configure(width=30, height=30, visionWidth=8, visionHeight=8)
         .setMaxEnergy(200.0))

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

## 3. Advanced Exception Handling

The bindings now provide specific exception types for better error handling:

```python
import pynlm

config = pynlm.createDefaultConfig()

try:
    config.loadFromFile("nonexistent_config.json")
except pynlm.FileNotFoundError as e:
    print(f"Configuration file not found: {e}")
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")

# Simulation error handling
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    # Simulate something that might fail
    brain.step(1000)
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
```

## 4. Batch Operations

Process multiple operations efficiently with batch methods:

```python
import pynlm

# Create multiple regions efficiently
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Add multiple regions in one call
region_ids = [brain.addRegion(f"cortex_{i}") for i in range(10)]

# Process batch sensory inputs
sensory_batch = []
for i in range(100):
    percept = world.getSensoryPercept()
    sensory_batch.append(percept)

processed_count = agent.processSensoryBatch(sensory_batch)
print(f"Processed {processed_count} sensory inputs")

# Batch configuration settings
config = pynlm.createDefaultConfig()
for i in range(5):
    config.set(f"synapse.weight_{i}", 0.1 * i)
```

## 5. Advanced Agent Configuration

Use advanced agent configuration and monitoring:

```python
import pynlm
import time

# Create and configure agent with advanced features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()

agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Configure agent parameters
agent.setPlasticityModifier(1.5)
agent.setSensoryNoveltyDecay(0.95)
agent.setMotorCommandSelectionStrategy("curiosity")

# Run advanced simulation with monitoring
start_time = time.time()
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Prediction error: {agent.getPredictionError():.3f}")

elapsed = time.time() - start_time
print(f"\nSimulation completed in {elapsed:.2f} seconds")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
```

## 6. Memory and State Management

Use advanced memory and state management features:

```python
import pynlm

# Create brain and save snapshots
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run initial simulation
for step in range(100):
    brain.step(step)

# Save brain state
saved = brain.save("initial_state.bin")
print(f"Saved initial state: {saved}")

# Create snapshot and restore later
snapshot = brain.createSnapshot()

# Modify brain
for step in range(100, 200):
    brain.step(step)

# Restore from snapshot
brain.restoreSnapshot(snapshot)
print("Restored from snapshot")

# Verify restoration
assert brain.getTotalSpikeCount() < 150, "Should have lower spike count after restoration"
print("State restoration verified")
```

## 7. Configuration Management

Advanced configuration loading and management:

```python
import pynlm
import json

# Create configuration with predefined values
defaults = [
    ("brain.neuron_count", "2000"),
    ("brain.synapse_density", "0.15"),
    ("plasticity.rate", "0.005"),
    ("neuromodulation.curiosity.enable", "true"),
    ("development.enabled", "true"),
    ("memory.episodic.enable", "true")
]

config = pynlm.createDefaultConfigWith(defaults)
print(f"Configuration keys: {len(config.getKeys())}")

# Load from JSON
with open("custom_config.json", "w") as f:
    json.dump({
        "brain": {"neuron_count": 3000},
        "plasticity": {"rate": 0.01},
        "neuromodulation": {"curiosity": {"enable": true}}
    }, f)

config2 = pynlm.createDefaultConfig()
config2.loadFromFile("custom_config.json")
print(f"Loaded config neuron count: {config2.getBrainNeuronCount()}")

# Export configuration
config.saveToFile("exported_config.json")
print("Configuration exported to JSON")
```

## 8. Event-Driven Programming

Use event-driven features for advanced simulations:

```python
import pynlm

def on_spike_detected(neuron_id, timestamp):
    print(f"Spike detected at neuron {neuron_id} at time {timestamp}")

def on_learning_event(event_type):
    print(f"Learning event: {event_type}")

def on_development_stage_changed(stage):
    print(f"Development stage changed to: {stage}")

# Create brain with event handlers
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Register event handlers
brain.registerSpikeHandler(on_spike_detected)
brain.registerLearningEventHandler(on_learning_event)
brain.registerDevelopmentEventHandler(on_development_stage_changed)

# Run simulation that triggers events
for step in range(50):
    brain.step(step)
```

## 9. Type Safety and Validation

Use type checking and validation features:

```python
import pynlm

# Create validation errors
config = pynlm.createDefaultConfig()

# Set invalid values to trigger validation
config.set("brain.neuron_count", -100)  # Should be validated
if not config.has("brain.neuron_count"):
    print("Invalid configuration value rejected")

# Use type-safe configuration access
neuron_count = config.getBrainNeuronCount()
if neuron_count is not None:
    print(f"Neuron count: {neuron_count}")
else:
    print("Default neuron count used")

# Validate state
if brain.validateState():
    print("Brain state is valid")
else:
    print(f"Validation errors: {brain.getValidationErrors()}")
```

## 10. Quick Start Utilities

Use convenience functions for common tasks:

```python
import pynlm

# Quick simulation helper
spike_count = pynlm.run_quick_simulation(100)
print(f"Quick simulation resulted in {spike_count} spikes")

# Create configuration with common presets
quick_config = pynlm.createDefaultConfigWith([
    ("brain.neuron_count", "500"),
    ("plasticity.rate", "0.001"),
    ("neuromodulation.curiosity.enable", "false")
])

# Complete agent setup helper
def create_agent_world(config, width=15, height=15):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width, height, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    return brain, world, agent

# Use helper
config = pynlm.createDefaultConfig()
brain, world, agent = create_agent_world(config)

print("Agent world created successfully")
```

## 11. Debugging and Monitoring

Use debugging and monitoring features:

```python
import pynlm

# Create brain with monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Monitor execution time
start_time = brain.getExecutionTime()
for step in range(100):
    brain.step(step)

end_time = brain.getExecutionTime()
print(f"Simulation took {end_time - start_time:.2f} time units")

# Check memory usage
memory_usage = brain.getMemoryUsage()
print(f"Memory usage: {memory_usage} bytes")

# Get neural activity patterns
activity_pattern = brain.getNeuralActivityPattern()
print(f"Neural activity range: {min(activity_pattern):.3f} to {max(activity_pattern):.3f}")

# Debug region activity
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    activity = brain.getRegionActivity(region_id)
    print(f"Region {region_id} ({region.getName()}): {sum(activity):.1f} average activity")
```

## 12. Performance Optimization

Use performance optimization features:

```python
import pynlm

# Create optimized brain configuration
optimized_config = pynlm.createDefaultConfigWith([
    ("brain.neuron_count", "1000"),
    ("plasticity.structural.enable", "true"),
    ("memory.working.enable", "true"),
    ("memory.episodic.enable", "true")
])

# Use batch operations for better performance
brain = pynlm.createBrain(optimized_config)
brain.initialize()

# Process in larger chunks for better performance
chunk_size = 100
for chunk_start in range(0, 1000, chunk_size):
    chunk_end = min(chunk_start + chunk_size, 1000)
    for step in range(chunk_start, chunk_end):
        brain.step(step)
    
    # Log chunk performance
    print(f"Processed chunk {chunk_start}-{chunk_end-1}: "
          f"{brain.getFiringNeuronCount()} neurons firing")
```

## 13. Advanced API Examples

### Example: Multi-Agent Simulation

```python
def run_multi_agent_simulation(num_agents=3, steps_per_agent=100):
    """Run simulation with multiple agents competing for resources"""
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
    world.setRandomSeed(42)
    
    # Create agents with different configurations
    agents = []
    for i in range(num_agents):
        config = pynlm.createDefaultConfig()
        # Different agent personalities
        if i == 0:
            config.enableCuriosity(True)
            config.enableRewardModulation(True)
        elif i == 1:
            config.enableCuriosity(False)
            config.enableRewardModulation(True)
        else:
            config.enableCuriosity(True)
            config.enableRewardModulation(False)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agents.append(agent)
    
    # Run simulation
    for step in range(steps_per_agent):
        world.update(0.1)
        
        for i, agent in enumerate(agents):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 50 == 0:
            print(f"Step {step} completed")
    
    return agents, brain, world

# Run example
agents, final_brain, final_world = run_multi_agent_simulation()
print(f"Simulation completed with {len(agents)} agents")
print(f"Final brain had {final_brain.getTotalSpikeCount()} spikes")
```

### Example: Learning Environment

```python
def create_learning_environment():
    """Create an environment where the agent learns to navigate"""
    
    # Create world with learning resources
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=6, visionHeight=6)
    world.setAgentStart(5.0, 5.0)
    
    # Add learning resources
    for x in range(10, 21, 3):
        for y in range(10, 21, 3):
            resource_value = 10.0 + (x + y) * 0.5
            world.addObject(pynlm.WorldObject(x, y, 
                                             pynlm.WorldObjectType.Resource, 
                                             value=resource_value))
    
    # Add hazards
    for x in [20, 25, 30]:
        for y in [20, 25, 30]:
            world.addObject(pynlm.WorldObject(x, y, 
                                             pynlm.WorldObjectType.Hazard, 
                                             value=-5.0))
    
    return world

# Create learning agent
learning_config = pynlm.createDefaultConfig()
learning_config.enableRewardModulation(True)
learning_config.enableCuriosity(True)
learning_config.enableDevelopment(True)

brain = pynlm.createBrain(learning_config)
brain.initialize()

world = create_learning_environment()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Learning simulation
for step in range(300):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward from environment
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)
    
    if step % 50 == 0:
        print(f"Step {step}: Curiosity={agent.getCuriosityLevel():.3f}, "
              f"Novelty={agent.getNoveltyLevel():.3f}, "
              f"Reward={reward:.2f}")

print("Learning environment simulation completed")
```

## Conclusion

These advanced examples showcase the enhanced NLM Python bindings with:

1. **Context managers** for automatic resource management
2. **Fluent interfaces** for method chaining
3. **Custom exceptions** for better error handling
4. **Batch operations** for improved performance
5. **Advanced configuration** options
6. **Event-driven programming** capabilities
7. **Type safety** and validation
8. **Performance monitoring** tools
9. **Debugging and optimization** features
10. **Complex multi-agent and learning** scenarios

The enhanced Python API provides powerful tools for creating sophisticated neural simulations while maintaining Pythonic code quality and ease of use.