# NLM Python API - Comprehensive Examples

This document provides comprehensive examples for using the improved NLM Python bindings. These examples showcase the new Pythonic APIs and demonstrate common patterns.

## Quick Start Examples

### Example 1: Silent Brain
```python
import pynlm

# Simple brain creation and initialization
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run thinking for 100 steps
for i in range(100):
    brain.step(i)

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
print(f"Neuron count: {brain.getTotalNeuronCount()}")
```

### Example 2: Enhanced World Setup
```python
import pynlm

# Create world with intuitive setup
world = pynlm.createSimpleWorld()
world.setup(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(10, 10)

# Configure world properties
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.01)
world.setRandomSeed(42)

print(world.describe())
```

### Example 3: Agent with All Learning Enabled
```python
import pynlm

# Create quick agent setup with learning
brain, world, agent = pynlm.createQuickAgent(
    neuron_count=500,
    steps=100,
    enable_learning=True,
    enable_reward_modulation=True,
    enable_curiosity=True,
    enable_development=True,
    enable_structural_plasticity=True
)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Print monitoring data every 10 steps
    if step % 10 == 0:
        stats = brain.getStats()
        monitoring = agent.getMonitoring()
        print(f"Step {step}: {stats['firing_neurons']} neurons firing, "
              f"curiosity: {monitoring['curiosity_level']:.3f}")
```

## Advanced Examples

### Example 4: Batch Configuration
```python
import pynlm

# Create configuration with batch settings
config = pynlm.createDefaultConfig()
config.batch_set({
    'brain.neuron_count': 2000,
    'brain.connection_probability': 0.1,
    'plasticity.stdp.enable': True,
    'plasticity.stdp.learning_rate': 0.001,
    'plasticity.hebbian.enable': True,
    'neuromod.dopamine.scale': 1.0,
    'neuromod.curiosity.enable': True,
    'neuromod.novelty.enable': True
})

# Validate configuration
errors = config.validate()
if errors:
    print("Configuration errors:", errors)
else:
    print("Configuration is valid")

# Get all config as Python dict
full_config = config.getAsDict()
print(f"Neuron count: {full_config.get('brain.neuron_count', 'Not set')}")
```

### Example 5: Complete Agent Simulation
```python
import pynlm

def run_comprehensive_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with all subsystems enabled."""
    
    # 1. Create and configure brain
    config = pynlm.createDefaultConfig()
    config.batch_set({
        'brain.neuron_count': 1000,
        'brain.connection_probability': 0.05,
        'plasticity.stdp.enable': True,
        'plasticity.hebbian.enable': True,
        'neuromod.dopamine.scale': 1.0,
        'neuromod.curiosity.enable': True
    })
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # 3. Setup world with agent
    world.setup(width=20, height=20, visionWidth=8, visionHeight=8)
    world.setAgentStart(10.0, 10.0)
    
    agent.initialize(world)
    
    # 4. Enable all learning subsystems
    agent.enableAll(True)
    brain.enableLearning()
    
    # 5. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input through agent
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Apply reward modulation based on internal signals
        internal = percept.getInternal()
        reward = internal[0] if internal else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            stats = brain.getStats()
            monitoring = agent.getMonitoring()
            print(f"Step {step}:")
            print(f"  Neurons: {stats['firing_neurons']}/{stats['neurons']} firing")
            print(f"  Curiosity: {monitoring['curiosity_level']:.3f}")
            print(f"  Novelty: {monitoring['novelty_level']:.3f}")
            print(f"  Prediction Error: {monitoring['prediction_error']:.3f}")
            print(f"  Development Stage: {monitoring['development_stage']}")
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'final_stats': brain.getStats(),
        'final_monitoring': agent.getMonitoring()
    }

# Run the simulation
result = run_comprehensive_agent_simulation(1000)
print(f"Simulation complete!")
print(f"Final brain stats: {result['final_stats']}")
```

### Example 6: Exploration World
```python
import pynlm

# Create exploration world with custom settings
world = pynlm.createExplorationWorld(
    width=15.0,
    height=15.0,
    visionWidth=6,
    visionHeight=6
)

# Add resources and hazards
from pynlm import WorldObject, WorldObjectType

world.addObject(WorldObject(3.0, 3.0, WorldObjectType.Resource, value=10.0))
world.addObject(WorldObject(12.0, 12.0, WorldObjectType.Hazard, value=-5.0))
world.addObject(WorldObject(8.0, 8.0, WorldObjectType.Resource, value=5.0))

print(world.describe())

# Check if positions are valid
print(f"Position (3,3) valid: {world.isValidPosition(3.0, 3.0)}")
print(f"Position (15,15) valid: {world.isValidPosition(15.0, 15.0)}")  # Should be invalid (wall)
```

### Example 7: Demo Simulation
```python
import pynlm

# Run quick demo simulation
stats = pynlm.runDemoSimulation(steps=500)

print("Demo simulation completed!")
print(f"Final statistics: {stats}")
```

## Method Chaining Examples

### SimpleWorld Method Chaining
```python
import pynlm

# Chaining configuration methods
world = pynlm.createSimpleWorld()
world.setup(
    width=25.0,
    height=15.0,
    visionWidth=10,
    visionHeight=8,
    maxEnergy=200.0,
    energyDecay=0.005,
    startX=12.5,
    startY=7.5,
    seed=123
)

print(world.describe())
```

### Brain Convenience Methods
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable all learning with one call
brain.enableLearning()

# Get comprehensive stats
stats = brain.getStats()
print(f"Brain has {stats['neurons']} neurons")
print(f"Firing: {stats['firing_neurons']} neurons")
print(f"E/I ratio: {stats['e_i_ratio']:.3f}")
```

### Agent Convenience Methods
```python
import pynlm

brain, world, agent = pynlm.createQuickAgent()

# Setup with custom settings using kwargs
agent.setup(world, enable_reward_modulation=True, enable_curiosity=True)

# Enable all subsystems
agent.enableAll(True)

# Get monitoring data
monitoring = agent.getMonitoring()
print(f"Neuromodulation level: {monitoring['neuromodulation_level']:.3f}")
print(f"Curiosity level: {monitoring['curiosity_level']:.3f}")
```

## Configuration Management Examples

### Working with Config
```python
import pynlm

# Create config and batch set values
config = pynlm.createDefaultConfig()
config.batch_set({
    'brain.neuron_count': 1500,
    'brain.synapse_density': 0.08,
    'simulation.timestep': 0.001
})

# Modify individual values
config.set('plasticity.stdp.learning_rate', 0.002)
config.set('neuromod.curiosity.threshold', 0.5)

# Query configuration
if config.has('brain.neuron_count'):
    print(f"Neuron count: {config.getOr('brain.neuron_count', 1000)}")

# Get all configuration as dictionary
all_config = config.getAsDict()
print(f"Number of config keys: {len(all_config)}")

# Save configuration
config.saveToFile("my_config.json")
print("Configuration saved to my_config.json")

# Load configuration (with error handling)
new_config = pynlm.createDefaultConfig()
if new_config.loadFromFile("my_config.json"):
    print("Configuration loaded successfully")
else:
    print("Failed to load configuration")
```

## Troubleshooting Examples

### Common Issues and Solutions

#### Issue: Brain not doing anything
```python
import pynlm

# Make sure to initialize
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()  # CRITICAL - forget this and brain does nothing!

# Run enough steps to see activity
for i in range(1000):
    brain.step(i)
    if brain.getTotalSpikeCount() > 0:
        print(f"First spike at step {i}")
        break
```

#### Issue: Agent not moving
```python
import pynlm

# Make sure world is updated and agent has sensory input
brain, world, agent = pynlm.createQuickAgent()

for step in range(100):
    world.update(0.1)  # Update world
    agent.processSensoryInput(world.getSensoryPercept())  # Give brain input
    brain.step(step)  # Brain thinks
    # Note: decodeMotorCommand would need to be called for action
```

#### Issue: Configuration validation
```python
import pynlm

config = pynlm.createDefaultConfig()
errors = config.validate()
if errors:
    print("Configuration issues found:")
    for error in errors:
        print(f"  - {error}")
    # Fix configuration
    config.set('brain.neuron_count', 1000)
```

## Performance and Monitoring

### Monitoring Brain Performance
```python
import pynlm
import time

brain, world, agent = pynlm.createQuickAgent(neuron_count=2000)
brain.initialize()

# Time the simulation
start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

print(f"Simulation took {end_time - start_time:.2f} seconds")
print(f"Steps per second: {1000/(end_time - start_time):.0f}")

# Get performance statistics
stats = brain.getStats()
print(f"Performance: {stats['firing_rate']:.2f} Hz average firing rate")
```

### Learning Progress Tracking
```python
import pynlm

brain, world, agent = pynlm.createQuickAgent()
brain.enableLearning()
agent.enableAll(True)

# Track learning over time
learning_data = {
    'steps': [],
    'firing_rates': [],
    'curiosity_levels': [],
    'novelty_levels': []
}

for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    if step % 50 == 0:
        learning_data['steps'].append(step)
        learning_data['firing_rates'].append(brain.getAverageFiringRate())
        monitoring = agent.getMonitoring()
        learning_data['curiosity_levels'].append(monitoring['curiosity_level'])
        learning_data['novelty_levels'].append(monitoring['novelty_level'])

# Analyze learning progress
import matplotlib.pyplot as plt
plt.figure(figsize=(12, 8))

plt.subplot(3, 1, 1)
plt.plot(learning_data['steps'], learning_data['firing_rates'])
plt.title('Firing Rate Over Time')
plt.ylabel('Hz')

plt.subplot(3, 1, 2)
plt.plot(learning_data['steps'], learning_data['curiosity_levels'])
plt.title('Curiosity Level Over Time')
plt.ylabel('Level')

plt.subplot(3, 1, 3)
plt.plot(learning_data['steps'], learning_data['novelty_levels'])
plt.title('Novelty Level Over Time')
plt.ylabel('Level')

plt.tight_layout()
plt.show()
```

## Migration Guide

### From Old API to New API

#### Old Way (Verbose)
```python
# Old way - verbose and repetitive
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.setAgentStart(10, 10)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
```

#### New Way (Concise)
```python
# New way - concise and intuitive
brain, world, agent = pynlm.createQuickAgent(
    neuron_count=1000,
    enable_learning=True,
    enable_reward_modulation=True,
    enable_curiosity=True,
    enable_development=True,
    enable_structural_plasticity=True
)

world.setup(width=20, height=20, visionWidth=8, visionHeight=8)
```

### Summary of Improvements

1. **Configuration**: `config.batch_set()` replaces multiple `config.set()` calls
2. **World Setup**: `world.setup()` replaces separate `configure()` and `setAgentStart()` calls
3. **Agent Initialization**: `agent.setup()` combines initialization and subsystem enabling
4. **Learning**: `brain.enableLearning()` enables all learning subsystems at once
5. **Statistics**: `brain.getStats()` and `agent.getMonitoring()` return comprehensive data
6. **Introspection**: `brain.describe()` and `world.describe()` provide human-readable status
7. **Error Handling**: Built-in configuration validation and better error messages
8. **Factory Functions**: `createQuickAgent()`, `createExplorationWorld()`, `runDemoSimulation()`

These improvements make the NLM Python API much more intuitive and Pythonic while maintaining backward compatibility.