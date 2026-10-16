# NLM Python Getting Started Guide

**Welcome to NLM (Neural Learning Machine)!** This guide will help you get up and running with creating and simulating neural networks in Python.

## Prerequisites

- Python 3.8+ installed
- NLM Python bindings installed (`pip install pynlm` or `pip install -e .` from the NLM repository)
- Basic understanding of neural networks and simulation concepts

## Quick Installation

If you haven't installed NLM yet:

```bash
# Install from source (recommended for development)
pip install -e .

# Or install from PyPI (if available)
pip install pynlm
```

Verify installation:
```python
import pynlm
print(f"NLM Version: {pynlm.__version__}")
```

## Project Structure

NLM's Python bindings follow a modular design:

```
pynlm/
├── brain.py           # Brain simulation and learning
├── world.py           # Environment simulation
├── agent.py           # Agent interface layer
├── config.py          # Configuration management
└── sensory.py         # Sensory input types
```

The module provides factory functions for easy object creation:
- `createBrain(config)` - Neural network
- `createSimpleWorld()` - Simulation environment
- `createAgentBrain(brain)` - Agent interface

## Tutorial

### Step 1: Create a Basic Brain

Let's start with the simplest possible neural network:

```python
import pynlm

# Create a brain with default settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### Step 2: Run a Simple Simulation

Make the brain process information:

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    
    # Log progress
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Simulation complete!")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
```

### Step 3: Create a World Environment

Add a 2D environment for the brain to interact with:

```python
import pynlm

# Create brain and initialize
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
world.reset()
world.setAgentStart(10.0, 10.0)  # Start agent in center

# Create agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("Environment created successfully")
print(f"World size: {world.getWidth()}x{world.getHeight()}")
```

### Step 4: Complete Agent Simulation

This pattern shows a complete brain-environment loop:

```python
import pynlm

def run_complete_simulation(num_steps=500):
    """Run a full agent simulation with learning."""
    
    # 1. Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 2. Enable learning features
    agent.enableRewardModulation(True)   # Learn from rewards
    agent.enableCuriosity(True)         # Explore new things
    agent.enableDevelopment(True)       # Brain matures
    
    # 3. Run simulation loop
    for step in range(num_steps):
        # Update world physics
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory information
        agent.processSensoryInput(percept)
        
        # Brain processes and learns
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward (simplified: based on position)
        body = world.getAgentBody()
        distance_to_center = ((body.x - 7.5) ** 2 + (body.y - 7.5) ** 2) ** 0.5
        reward = max(0, 10.0 - distance_to_center)
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print status
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Position: ({body.x:.1f}, {body.y:.1f})")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    
    print(f"\nSimulation complete after {num_steps} steps!")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    return brain, world, agent

# Run the simulation
brain, world, agent = run_complete_simulation(500)
```

### Step 5: Visualizing Agent Behavior

Add some basic visualization to see what's happening:

```python
import pynlm
import matplotlib.pyplot as plt

def track_agent_movement(brain, world, agent, num_steps=200):
    """Track and plot agent movement over time."""
    
    # Track positions
    x_positions = []
    y_positions = []
    firing_rates = []
    
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        body = world.getAgentBody()
        x_positions.append(body.x)
        y_positions.append(body.y)
        firing_rates.append(brain.getAverageFiringRate())
    
    # Create plot
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
    
    # Position plot
    ax1.plot(x_positions, y_positions, 'b-', alpha=0.7)
    ax1.scatter([x_positions[0]], [y_positions[0]], c='green', s=100, label='Start')
    ax1.scatter([x_positions[-1]], [y_positions[-1]], c='red', s=100, label='End')
    ax1.set_xlabel('X Position')
    ax1.set_ylabel('Y Position')
    ax1.set_title('Agent Movement Path')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    ax1.set_aspect('equal')
    
    # Firing rate plot
    ax2.plot(firing_rates, 'r-')
    ax2.set_xlabel('Step')
    ax2.set_ylabel('Average Firing Rate (Hz)')
    ax2.set_title('Neural Activity Over Time')
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.show()
    
    return x_positions, y_positions, firing_rates

# Run tracking simulation
brain, world, agent = run_complete_simulation(100)
positions, firing = track_agent_movement(brain, world, agent, 100)
```

## Common Patterns

### Pattern 1: Reset-Initialize-Simulate Cycle

```python
def training_episode():
    # Always reset before new episode
    brain.reset()
    world.reset()
    
    # Initialize with same starting conditions
    brain.initialize()
    world.setAgentStart(10.0, 10.0)
    agent.initialize(world)
    
    # Run episode
    for step in range(episode_length):
        # ... simulation loop ...
        
    return final_metrics
```

### Pattern 2: Configuration Management

```python
def create_custom_config():
    config = pynlm.createDefaultConfig()
    
    # Modify brain parameters
    config.set("brain.neuron_count", 500)
    config.set("brain.v_thresh", -45.0)  # Higher threshold
    
    # Adjust learning rates
    config.set("plasticity.stdp.learning_rate", 0.005)
    
    # Configure development
    config.set("development.enable", True)
    config.set("development.critical_period_length", 100)
    
    return config
```

### Pattern 3: Error Handling

```python
import pynlm

def safe_simulation_step(brain, world, agent):
    """Safely run a single simulation step with error handling."""
    try:
        # Update world
        world.update(0.1)
        
        # Get percept
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step_number)
        
        # Get and apply action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        return True, None
        
    except pynlm.NLMInvalidArgumentError as e:
        print(f"Invalid argument: {e}")
        return False, "Invalid simulation parameters"
        
    except pynlm.NLMNotInitializedError as e:
        print(f"Not initialized: {e}")
        return False, "Brain or world not initialized"
        
    except Exception as e:
        print(f"Unexpected error: {e}")
        return False, str(e)
```

## Troubleshooting

### "Brain isn't doing anything"
- Check that `brain.initialize()` was called
- Verify simulation steps are running
- Check neuron parameters in config

### "Agent won't move"
- Ensure `world.applyMotorCommand()` is being called
- Check that `world.update()` is called each step
- Verify action decoding is working

### "Everything is 0"
- Brains need time to "warm up"
- Some neurons need input to fire
- Check initial connectivity and weights

### "Memory or performance issues"
- Use `brain.save()` to checkpoint progress
- Consider larger networks with optimized connectivity
- Monitor `brain.getTotalNeuronCount()` for memory usage

## Next Steps

1. **Learn more**: Read `HOW_TO_USE.md` for advanced features
2. **See examples**: Check the `examples/` directory for complete demonstrations
3. **Explore**: Try different configurations and observe behavior changes
4. **Contribute**: Add your own simulation examples!

Enjoy building your neural systems with NLM!
