# NLM Learning Progression Guide

## Overview

This guide provides a structured learning path for NLM (Neural Learning Machine), from beginner to advanced levels. Each level builds upon the previous one, introducing new concepts and capabilities.

## Level 1: The Foundations (Beginner)

### What You'll Learn
- Basic brain simulation concepts
- How neurons and synapses work
- Simple agent simulation loop

### Prerequisites
- Python 3.8+
- NLM installed (`pip install pynlm`)

### Tutorial 1.1: Silent Brain

#### Example: Basic Brain Simulation

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run for 100 simulation steps
for step in range(100):
    brain.step(step)

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

#### Key Concepts
- `createBrain()` creates a new neural network
- `brain.initialize()` starts the brain
- `brain.step()` processes one time step

### Tutorial 1.2: Brain in a World

#### Example: Agent in Environment

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation loop
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

#### Key Concepts
- `createSimpleWorld()` creates the environment
- `world.configure()` sets dimensions
- The simulation loop follows the brain-world interaction pattern

## Level 2: Core Patterns (Intermediate)

### What You'll Learn
- Common simulation patterns
- Brain statistics and monitoring
- Enabling learning features

### Tutorial 2.1: Complete Simulation Pattern

#### Reusable Simulation Function

```python
def run_simulation(brain, world, agent, num_steps):
    """Complete agent simulation with brain and world."""
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # 4. Run brain step
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it:
# run_simulation(brain, world, agent, 1000)
```

### Tutorial 2.2: Monitoring and Statistics

#### Monitoring Brain Activity

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with monitoring
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Monitor brain activity
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
```

### Tutorial 2.3: Enable Learning Features

#### Setting Up Learning

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Run simulation
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
```

## Level 3: Factory Functions (Intermediate-Advanced)

### What You'll Learn
- New improved factory functions
- Context managers for resource management
- Error handling and validation

### Tutorial 3.1: Using Factory Functions

#### Creating Complete Agents with One Call

```python
import pynlm

# Method 1: Simple agent factory
brain, world, agent = pynlm.createSimpleAgent(width=100, height=100, 
                                              vision_width=20, vision_height=20)

# Method 2: Default agent (pre-configured)
brain, world, agent = pynlm.createDefaultAgent()

# Method 3: Training agent (advanced features)
brain, world, agent = pynlm.createTrainingAgent(width=200, height=200)

# Method 4: Challenge agent (with obstacles)
brain, world, agent = pynlm.createChallengeAgent(width=300, height=300)

# Method 5: Experiment agent (custom config)
brain, world, agent = pynlm.createExperimentAgent(
    width=400, height=400, config_file="my_config.json"
)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Tutorial 3.2: Context Managers

#### Automatic Resource Management

```python
import pynlm

# Method 1: Manual initialization
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
# Need to remember to reset or reinitialize

# Method 2: Using context manager
with pynlm.BrainContextManager(brain) as brain_ctx:
    for i in range(100):
        brain_ctx.step(i)
# Automatically reset when exiting context

# Method 3: Complete agent with context manager
with pynlm.createSimpleAgent() as (brain, world, agent):
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
# Everything automatically cleaned up

# Method 4: World context manager
with pynlm.SimpleWorldContextManager(world) as world_ctx:
    for step in range(100):
        world_ctx.update(0.1)
        # Use world_ctx.getSensoryPercept(), etc.
```

### Tutorial 3.3: Error Handling

#### Robust Simulation with Error Handling

```python
import pynlm

def run_robust_simulation(width=100, height=100, num_steps=1000):
    """Run simulation with proper error handling."""
    brain = world = agent = None
    
    try:
        # Create agent using factory function
        brain, world, agent = pynlm.createSimpleAgent(width, height)
        
        # Run simulation loop
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Check for errors periodically
            if brain.getFiringNeuronCount() == 0 and step > 100:
                print("Warning: No neurons firing - possible initialization issue")
                
    except pynlm.NLMException as e:
        print(f"NLM error: {e}")
        raise
    except Exception as e:
        print(f"Unexpected error: {e}")
        raise
    finally:
        # Cleanup
        if agent:
            agent.reset()
        if world:
            world.reset()
        print("Simulation completed or terminated")

# Run robust simulation
try:
    run_robust_simulation()
except Exception as e:
    print(f"Simulation failed: {e}")
```

## Level 4: Advanced Features (Advanced)

### What You'll Learn
- Advanced configuration options
- Custom agents and experiments
- Performance optimization

### Tutorial 4.1: Advanced Configuration

#### Custom Configuration

```python
import pynlm
import json

# Create custom configuration
config = pynlm.Config()

# Set brain parameters
config.set("brain.neuron_count", 2000)           # More neurons
config.set("brain.region_count", 8)              # More brain regions
config.set("brain.v_thresh", -45.0)              # Different threshold
config.set("brain.tau_mem", 15.0)                # Slower membrane
config.set("brain.initial_weight_mean", 0.3)     # Different initial weights

# Set learning parameters
config.set("plasticity.stdp.enable", True)       # Enable STDP
config.set("plasticity.stdp.learning_rate", 0.001)  # Learning rate
config.set("plasticity.hebbian.enable", True)    # Enable Hebbian

# Set neuromodulation
config.set("neuromod.dopamine.scale", 1.5)       # Stronger dopamine
config.set("neuromod.curiosity.enable", True)    # Enable curiosity
config.set("neuromod.novelty.enable", True)      # Enable novelty

# Set development
config.set("development.enabled", True)          # Enable development
config.set("development.critical_period", True)   # Critical period

# Save configuration
config.saveToFile("advanced_config.json")

# Load configuration
new_config = pynlm.Config()
new_config.loadFromFile("advanced_config.json")

# Create brain with custom config
brain = pynlm.createBrain(new_config)
brain.initialize()
```

### Tutorial 4.2: Custom Experiment Setup

#### Building Complex Experiments

```python
import pynlm
from typing import List, Tuple, Callable

def create_custom_experiment(
    config_file: str = None,
    world_width: float = 200,
    world_height: float = 200,
    vision_size: int = 30,
    enable_all_features: bool = True
) -> Tuple[pynlm.Brain, pynlm.SimpleWorld, pynlm.AgentBrain]:
    """Create a custom experiment with advanced setup."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    if config_file:
        if not config.loadFromFile(config_file):
            raise ValueError(f"Failed to load config file: {config_file}")
    
    if enable_all_features:
        # Override with advanced settings
        config.set("brain.neuron_count", 3000)
        config.set("brain.region_count", 10)
        config.set("world.max_energy", 200.0)
        config.set("world.energy_decay_rate", 0.01)
        config.set("development.enabled", True)
        config.set("development.critical_period", True)
        config.set("neuromodulation.reward_modulation", True)
        config.set("neuromodulation.curiosity", True)
        config.set("neuromodulation.novelty", True)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    if not brain.initialize():
        raise RuntimeError("Failed to initialize brain")
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(world_width, world_height, vision_size, vision_size)
    world.setAgentStart(world_width/2, world_height/2)
    
    # Enable challenge environment features
    if enable_all_features:
        world.setMaxEnergy(200.0)
        world.setEnergyDecayRate(0.01)
        
        # Add environment objects
        world.addObject(pynlm.WorldObject.create_resource(50, 50, 10.0))
        world.addObject(pynlm.WorldObject.create_resource(150, 150, 5.0))
        world.addObject(pynlm.WorldObject.create_hazard(100, 100, 3.0))
        world.addObject(pynlm.WorldObject.create_marker(180, 180))
    
    # Create agent brain
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning features
    if enable_all_features:
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    
    return brain, world, agent

def run_experiment_loop(
    brain: pynlm.Brain,
    world: pynlm.SimpleWorld,
    agent: pynlm.AgentBrain,
    num_steps: int = 1000,
    print_interval: int = 100
):
    """Run experiment loop with detailed monitoring."""
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress
        if step % print_interval == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev stage: {brain.getDevelopmentalStage()}")
            print(f"  World time: {world.getSimulationTime():.1f}")
            print()

def main():
    """Main experiment execution."""
    try:
        print("Creating custom experiment...")
        brain, world, agent = create_custom_experiment(
            enable_all_features=True,
            world_width=300,
            world_height=300,
            vision_size=40
        )
        
        print("Running experiment loop...")
        run_experiment_loop(brain, world, agent, num_steps=2000, print_interval=200)
        
        print("\nExperiment completed successfully!")
        
    except Exception as e:
        print(f"Experiment failed: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
```

## Quick Reference Guide

### Learning Path

1. **Start with simple examples**: Silent brain, agent in world
2. **Master core patterns**: Complete simulation loop, monitoring
3. **Use factory functions**: `createSimpleAgent()`, `createTrainingAgent()`
4. **Apply context managers**: Automatic resource management
5. **Implement error handling**: Robust simulation code
6. **Advanced customization**: Config files, complex experiments

### Common Commands

| What you want | Simple | Advanced |
|---------------|--------|----------|
| Create brain | `pynlm.createBrain(pynlm.createDefaultConfig())` | `pynlm.createBrain(config)` |
| Create agent | `pynlm.createAgentBrain(brain)` | `pynlm.createSimpleAgent()` |
| Create world | `pynlm.createSimpleWorld()` | `pynlm.createChallengeAgent()` |
| Run simulation | Manual loop | `run_simulation(brain, world, agent, 1000)` |
| Context manager | Manual cleanup | `with pynlm.createSimpleAgent():` |

### Error Codes

| Error | Cause | Solution |
|-------|-------|----------|
| NLMException | NLM-specific error | Check configuration and state |
| ValueError | Invalid parameter | Check parameter types and ranges |
| KeyError | Missing config key | Check config or use defaults |
| RuntimeError | Initialization failed | Check system resources |

## Next Steps

After completing this guide:

1. **Read `HOW_TO_USE.md`** for detailed technical reference
2. **Explore `docs/ARCHITECTURE.md`** to understand system design
3. **Try `docs/EXPERIMENTS.md`** for experiment examples
4. **Look at `docs/SCIENCE.md`** for scientific background

Enjoy building your neural brain! 🧠✨