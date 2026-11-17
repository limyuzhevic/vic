# NLM (Neural Learning Machine) - Easy Usage Guide

## Quick Start - 5 Minutes

Welcome to NLM! This guide will help you get started with the neural simulation framework in just 5 minutes.

### Step 1: Install (If not already installed)

```bash
pip install nlm
```

### Step 2: Basic Neural Brain Setup

```python
from nlm import createBrain, createDefaultConfig

# Create a brain with optimal Phase 2 settings
brain = createBrain()

# Initialize the brain
brain.initialize()

print("✓ Brain system ready!")
print(f"  - Neurons: {brain.getNeuronCount()}")
print(f"  - Regions: {brain.getRegionCount()}")
print(f"  - Synapses: {brain.getSynapseCount()}")
```

### Step 3: Create a World

```python
from nlm import createSimpleWorld

# Create a simulated world
world = createSimpleWorld()
world.configure(800, 600, 80, 60)  # World size with vision
world.setAgentStart(400, 300)     # Agent starting position

print("✓ World created!")
print(f"  - Size: {world.getWidth()}x{world.getHeight()}")
print(f"  - Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
```

### Step 4: Create Agent Brain

```python
from nlm import createAgentBrainWithWorld

# Connect brain to world
agent_brain = createAgentBrainWithWorld(brain, world)
agent_brain.initializeWithWorld(world)

# Enable advanced learning features
agent_brain.enableAllFeatures()

print("✓ Agent brain ready with advanced learning!")
print(f"  - Reward modulation: {'enabled' if agent_brain.isRewardModulationEnabled() else 'disabled'}")
print(f"  - Structural plasticity: {'enabled' if agent_brain.isStructuralPlasticityEnabled() else 'disabled'}")
print(f"  - Development system: {'enabled' if agent_brain.isDevelopmentEnabled() else 'disabled'}")
print(f"  - Curiosity system: {'enabled' if agent_brain.isCuriosityEnabled() else 'disabled'}")
```

### Step 5: Run a Simulation

```python
# Run a simple simulation loop
print("Starting simulation...")

for step in range(100):
    # Get sensory input from the world
    percept = world.getSensoryPercept()
    
    # Process with agent brain
    agent_brain.processSensoryInput(percept)
    
    # Generate action
    action = agent_brain.decodeMotorCommand()
    
    # Apply action in the world
    result = world.applyMotorCommand(action, step)
    
    # Apply reward modulation for learning
    if result.success:
        agent_brain.applyModulation(result.reward, result.reward)
    
    # Update development every 10 steps
    if step % 10 == 0:
        agent_brain.updateDevelopmentSystem(0.1)

print("✓ Simulation complete!")
print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
print(f"  - Firing rate: {brain.getAverageFiringRate():.4f}")
print(f"  - Curiosity level: {agent_brain.getCuriosityLevel():.4f}")
print(f"  - Novelty level: {agent_brain.getNoveltyLevel():.4f}")
```

## Common Patterns

### 1. Simple Reflex Agent

```python
from nlm import createBrain, createSimpleWorld, createAgentBrainWithWorld

# Create complete system
brain = createBrain()
world = createSimpleWorld()
agent = createAgentBrainWithWorld(brain, world)

# Setup
agent.initializeWithWorld(world)

# Run reflex loop
for step in range(50):
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step)
```

### 2. Learning Agent with Memory

```python
from nlm import createTrainingSetup

# Create complete training environment (all features enabled)
env = createTrainingSetup()

brain = env["brain"]
world = env["world"]
agent = env["agentBrain"]

# Training loop
for episode in range(5):
    # Reset for new episode
    agent.resetEpisode()
    world.reset()
    
    # Learn from experience
    for step in range(200):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, episode * 200 + step)
        
        if result.success:
            # Learn from reward
            agent.applyModulation(result.reward, result.predicted_reward)
```

### 3. Personality Transfer

```python
from nlm import createBrain, createSimpleWorld, createAgentBrainWithWorld

# Create first agent
agent1 = createAgentBrainWithWorld(createBrain(), createSimpleWorld())
agent1.initializeWithWorld(createSimpleWorld())

# Let it learn and develop
for step in range(100):
    agent1.updateDevelopmentSystem(0.1)

# Export its personality
personality = agent1.exportPersonality()

# Create new agent with same personality
agent2 = createAgentBrainWithWorld(createBrain(), createSimpleWorld())
agent2.initializeWithWorld(createSimpleWorld())
agent2.importPersonality(personality)

print("Agent personality successfully transferred!")
```

### 4. Network Analysis

```python
from nlm import createBrain

# Create and initialize brain
brain = createBrain()
brain.initialize()

# Analyze network properties
print("Network Analysis:")
print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
print(f"  - Active neurons: {brain.getActiveNeuronCount()}")
print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  - Average firing rate: {brain.getAverageFiringRate():.4f}")
print(f"  - Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.4f}")
print(f"  - Pending spikes: {brain.getPendingSpikeEventCount()}")

# Get developmental status
print(f"  - Developmental stage: {brain.getDevelopmentalStage()}")
print(f"  - Neuromodulation level: {brain.getNeuromodulationLevel():.4f}")
```

## Configuration Examples

### 1. Quick Testing Setup

```python
from nlm import createBrain

brain = createBrain()
brain.getConfig().setInt("neuron_count", 100)  # Fewer neurons for speed
brain.getConfig().setInt("region_count", 1)     # Single region
brain.initialize()
```

### 2. Large-Scale Simulation

```python
from nlm import createBrain

brain = createBrain()
brain.getConfig().setInt("neuron_count", 2000)  # Many neurons
brain.getConfig().setInt("region_count", 5)      # Multiple regions
brain.getConfig().setDouble("simulation_timestep", 0.0005)  # Faster simulation
brain.initialize()
```

### 3. High Plasticity Environment

```python
from nlm import createBrain

brain = createBrain()
brain.getConfig().setDouble("stdp_ltp_weight", 0.05)    # Stronger LTP
brain.getConfig().setDouble("stdp_ltd_weight", 0.03)    # Stronger LTD
brain.getConfig().setDouble("synaptogenesis_rate", 0.001)  # More synapse formation
brain.initialize()
```

## Pythonic Interface

### Type-Safe Configuration

```python
from nlm import createDefaultConfig

config = createDefaultConfig()

# Set values with proper type checking
config.setString("agent_name", "TestAgent")
config.setInt("max_steps", 1000)
config.setDouble("learning_rate", 0.01)
config.setBool("enable_visualization", True)

# Get values with defaults
name = config.getString("agent_name", "DefaultAgent")
steps = config.getInt("max_steps", 100)
rate = config.getDouble("learning_rate", 0.5)
viz = config.getBool("enable_visualization", False)

print(f"Agent: {name}")
print(f"Steps: {steps}")
print(f"Learning rate: {rate}")
print(f"Visualization: {viz}")
```

### Batch Operations

```python
from nlm import createDefaultConfig

config = createDefaultConfig()

# Set multiple values at once
updates = [
    ("random_seed", 42),
    ("simulation_timestep", 0.001),
    ("neuron_count", 500),
    ("region_count", 2),
    ("connection_probability", 0.2)
]

config.setMultiple(updates)

# Merge with another config
other_config = createDefaultConfig()
other_config.setString("author", "Research Team")
config.mergeFromConfig(other_config)

print("Batch configuration applied successfully!")
```

## Troubleshooting

### "Brain not initialized"
```python
# Ensure you call initialize()
brain = createBrain()
brain.initialize()  # <-- This line is required!
```

### "No neurons firing"
```python
# Increase excitation or reduce inhibition
brain.getConfig().setDouble("stdp_ltp_weight", 0.1)  # Stronger excitation
brain.getConfig().setDouble("stdp_ltd_weight", 0.01) # Weaker inhibition
```

### "Memory full"
```python
# Clear working memory
brain = createBrain()
brain.initialize()
brain.getWorkingMemory().clear()  # Or use brain.getWorkingMemory().reset()
```

### "Performance too slow"
```python
# Use simpler configuration
brain = createBrain()
brain.getConfig().setInt("neuron_count", 100)  # Fewer neurons
brain.getConfig().setInt("region_count", 1)      # Fewer regions
brain.initialize()
```

## Resources

- **Documentation**: See `README.md` for comprehensive documentation
- **Examples**: Check the `examples/` directory for more complex scenarios
- **Community**: Join the NLM discussion forum for help and collaboration
- **GitHub**: Report issues and contribute at the NLM repository

## Quick Commands

```python
# One-liner: Create and run simple agent
from nlm import createTrainingSetup
env = createTrainingSetup()
agent = env["agentBrain"]
for step in range(100):
    percept = env["world"].getSensoryPercept()
    agent.processSensoryInput(percept)
    action = agent.decodeMotorCommand()
    env["world"].applyMotorCommand(action, step)
```

Happy neural simulation! 🚀