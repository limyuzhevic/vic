# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
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

### Example 3: Advanced Configuration

```python
import pynlm

# Create configuration with custom settings
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)           # More neurons
config.set("brain.connection_probability", 0.05) # Sparse connections
config.set("plasticity.stdp.enable", True)        # Enable STDP learning
config.set("neuromod.dopamine.scale", 2.0)       # Stronger dopamine
config.set("environment.width", 20)               # Larger world
config.set("environment.height", 20)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with learning enabled
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run advanced simulation
for step in range(1000):
    world.update(0.05)  # Larger timestep
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Log progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development: {brain.getDevelopmentalStage()}")

print("Advanced simulation complete!")
```

## Advanced Usage Examples

### Example 4: Checkpointing and Experiment Management

```python
import pynlm
import os

# Create a unique experiment directory
experiment_id = "nlm_experiment_001"
os.makedirs(experiment_id, exist_ok=True)

# Run multiple simulations
for episode in range(10):
    print(f"--- Episode {episode} ---")
    
    # Create fresh brain for each episode
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("random_seed", episode + 42)  # Different seed each episode
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable all features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run episode
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Save checkpoint
    checkpoint_path = f"{experiment_id}/episode_{episode}_checkpoint.bin"
    brain.save(checkpoint_path)
    print(f"  Checkpoint saved: {checkpoint_path}")
    
    # Log episode results
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")

print("Multi-episode experiment complete!")
```

### Example 5: Comparative Experiment

```python
import pynlm
import time

# Define different experimental conditions
conditions = [
    {"name": "Baseline", "neuron_count": 500, "plasticity": False},
    {"name": "Enhanced", "neuron_count": 1000, "plasticity": True},
    {"name": "High Plasticity", "neuron_count": 1500, "plasticity": True, "plasticity_rate": 0.5},
]

results = []

for condition in conditions:
    print(f"\n=== Running: {condition['name']} ===")
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", condition["neuron_count"])
    config.set("plasticity.stdp.enable", condition.get("plasticity", True))
    
    if "plasticity_rate" in condition:
        config.set("plasticity.learning_rate", condition["plasticity_rate"])
    
    start_time = time.time()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=12, height=12, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Configure based on condition
    if condition["name"] != "Baseline":
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    
    # Run simulation
    total_reward = 0
    for step in range(300):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        total_reward += reward
    
    end_time = time.time()
    
    # Collect results
    condition_result = {
        "name": condition["name"],
        "neurons": condition["neuron_count"],
        "total_spikes": brain.getTotalSpikeCount(),
        "average_firing_rate": brain.getAverageFiringRate(),
        "total_reward": total_reward,
        "development_stage": str(brain.getDevelopmentalStage()),
        "execution_time": end_time - start_time,
        "final_curiosity": agent.getCuriosityLevel(),
        "final_novelty": agent.getNoveltyLevel(),
    }
    
    results.append(condition_result)
    
    print(f"  Completed in {end_time - start_time:.2f}s")
    print(f"  Spikes: {condition_result['total_spikes']}")
    print(f"  Reward: {condition_result['total_reward']:.2f}")
    print(f"  Curiosity: {condition_result['final_curiosity']:.3f}")

# Print comparison
print("\n=== Experimental Results ===")
for result in results:
    print(f"{result['name']}: {result['total_spikes']} spikes, {result['total_reward']:.2f} reward, {result['execution_time']:.2f}s")
```

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in  
3. **Agent** - The bridge connecting brain to world

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons  
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

**"My experiment failed"**
- Check that all components are properly initialized
- Verify that configuration parameters are valid
- Try reducing simulation complexity

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more technical details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try building the C++ version for better performance
5. Create your own custom environments and agent behaviors

That's it! You're now ready to use NLM and create your own artificial brain experiments.
