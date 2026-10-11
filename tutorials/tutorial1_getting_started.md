# Tutorial 1: Getting Started with NLM Python API

## Overview
This tutorial introduces you to the NLM Python API, focusing on the new convenience functions and enhanced features.

## Prerequisites
- Python 3.8+
- NLM installed (`pip install pynlm`)

## Basic Concepts

### 1. Brain - The Thinking Engine
The brain is the core neural network that processes information and makes decisions.

### 2. World - The Environment
The world represents the simulated environment where the brain interacts.

### 3. Agent - The Interface
The agent connects the brain to the world, handling sensory input and motor output.

## Tutorial Steps

### Step 1: Create a Simple Brain

```python
import pynlm

# Create a basic brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
```

### Step 2: Create a World

```python
# Create a simple 2D world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

print(f"World created: {world.getWidth()}x{world.getHeight()} cells")
```

### Step 3: Create an Agent

```python
# Create an agent that connects brain to world
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("Agent created and initialized")
```

### Step 4: Run a Simulation

```python
# Run a complete simulation episode
for step in range(100):
    # 1. Update the world
    world.update(0.1)
    
    # 2. Get what the agent sees
    percept = world.getSensoryPercept()
    
    # 3. Process sensory input in the brain
    agent.processSensoryInput(percept)
    
    # 4. Brain thinks and processes
    brain.step(step)
    
    # 5. Get action from brain
    action = agent.decodeMotorCommand()
    
    # 6. Execute action in world
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation completed successfully!")
```

### Step 5: Enable Advanced Features

```python
# Enable learning and development capabilities
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)              # Explore new things
agent.enableDevelopment(True)           # Brain matures over time
agent.enableStructuralPlasticity(True)  # Grow new connections

print("Advanced features enabled")
```

## Complete Example: Hello World

```python
import pynlm

# Create a complete brain-world-agent system
config = pynlm.createDevelopmentConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run the simulation
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"Curiosity: {agent.getCuriosityLevel():.2f}")

# Get final statistics
stats = pynlm.get_simulation_stats(brain, world)
print(f"\nFinal Statistics:")
print(f"  Total Neurons: {stats['total_neurons']}")
print(f"  Firing Neurons: {stats['firing_neurons']}")
print(f"  Average Firing Rate: {stats['average_firing_rate']:.2f} Hz")
print(f"  E/I Balance: {stats['excitatory_inhibition_ratio']:.3f}")
print(f"  Total Spikes: {stats['total_spikes']}")
print(f"  Simulation Time: {stats['simulation_time']:.1f}s")
```

## Next Steps

1. **Tutorial 2: Configuration Presets** - Learn about different configuration options
2. **Tutorial 3: Advanced Usage** - Explore expert-level features
3. **Tutorial 4: Custom Environments** - Create specialized world environments

## Troubleshooting

- **ImportError: No module named 'pynlm'**: Install with `pip install pynlm`
- **Brain initialization failed**: Ensure `brain.initialize()` is called before any simulation steps
- **Agent not responding**: Check that `agent.initialize(world)` is called
- **World boundaries**: Ensure `world.configure()` is called before `world.reset()`

## Key Takeaways

1. Always call `brain.initialize()` before running simulation steps
2. Use `createDevelopmentConfig()` for learning-focused simulations
3. The complete loop is: world.update() → agent.processSensoryInput() → brain.step() → agent.decodeMotorCommand() → world.applyMotorCommand()
4. Monitor statistics to understand brain behavior during simulation
5. Enable learning features (reward modulation, curiosity) for realistic behavior