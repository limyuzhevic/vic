# NLM - Easy Usage Guide for Beginners (Enhanced)

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste - Basic)

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

### NEW: Quick Agent Setup with Enhanced API

```python
import pynlm

# One-line setup for a complete agent with learning
agent = pynlm.create_development_agent(
    stages=["initial", "critical_period"],
    curiosity_enabled=True,
    plasticity_multiplier=1.5
)

# World setup (simplified)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Complete simulation
agent.initialize(world)
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Enhanced agent simulation complete!")
```

---

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## NEW: Common Patterns with Enhanced API

### Pattern 1: Run a Complete Simulation with Helper

```python
import pynlm

# Create agent with enhanced API
agent = pynlm.create_curiosity_agent(
    intrinsic_reward_weight=0.7,
    novelty_threshold=0.05
)

# Setup world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
agent.initialize(world)

# Run complete simulation with monitoring
result = pynlm.run_complete_simulation(
    agent,
    world,
    num_steps=500,
    headless=False,
    save_interval=100,
    output_file="simulation.json"
)
```

### Pattern 2: Training with Enhanced API

```python
import pynlm

# Create development agent
agent = pynlm.create_development_agent()

# Setup environment
world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=12, visionHeight=12)

# Train with evaluation
metrics = agent.train_with_preset(
    world,
    preset_name="curiosity_driven",
    num_episodes=200,
    evaluation_interval=50,
    save_interval=100
)

print(f"Training complete! Mean reward: {metrics['mean_reward']:.2f}")
```

### Pattern 3: Get Brain Stats (Enhanced)

```python
import pynlm

# Create enhanced agent
agent = pynlm.create_standard_agent()
brain = agent.getBrain()
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Simulation loop with monitoring
for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Avg firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.2f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.2f}")
```

### Pattern 4: Enable Brain Features (Enhanced)

```python
import pynlm

# Create enhanced agent
agent = pynlm.create_standard_agent()

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Additional exploration strategies (if available)
# exploration_manager = pynlm.ExplorationManager(agent, world)
# exploration_manager.set_strategy(pynlm.ExplorationStrategy.UCB)
```

---

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

### NEW: Enhanced Agent Features

- `agent.create_from_preset("preset_name")` - Create agent from preset configuration
- `agent.train_with_preset(world, "preset_name", num_episodes)` - Train with preset
- `agent.evaluate_with_benchmark(environments)` - Evaluate against benchmark suite
- `agent.is_developing` - Check if agent is developing
- `agent.curiosity_level` - Get curiosity level
- `agent.novelty_score` - Get novelty score
- `agent.reward_prediction_error` - Get reward prediction error

---

## Mini Projects (Enhanced)

### Project 1: Silent Brain (Basic)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World (Enhanced)

```python
import pynlm

# Setup with enhanced API
agent = pynlm.create_standard_agent()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Watch the world with monitoring
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
print("Watched world for 30 steps")
print(f"Firing rate: {agent.getBrain().getAverageFiringRate():.2f}")
```

### Project 3: Complete Agent with Learning (Enhanced)

```python
import pynlm

# Setup with learning capabilities
agent = pynlm.create_development_agent(
    stages=["initial", "critical_period", "maturation"],
    curiosity_enabled=True,
    plasticity_multiplier=1.2
)

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable advanced learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run enhanced agent simulation
metrics = agent.train_with_preset(
    world,
    preset_name="development",
    num_episodes=100
)

print("Enhanced agent simulation complete!")
print(f"Performance metrics: {metrics}")
```

---

## NEW: Quick Reference (Enhanced)

| What you want | Code (Basic) | Code (Enhanced) |
|--------------|--------------|----------------|
| Create brain | `pynlm.createBrain(config)` | `pynlm.create_development_agent()` |
| Start brain | `brain.initialize()` | `agent.initialize(world)` |
| Make brain think | `brain.step(step)` | `agent.processSensoryInput(percept)` |
| Create world | `pynlm.createSimpleWorld()` | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` | `pynlm.create_development_agent()` |
| Get brain stats | `brain.getFiringNeuronCount()` | `agent.getBrain().getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` | `percept = world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` | `action = agent.decodeMotorCommand()` |
| Train agent | - | `agent.train_with_preset(world, "curiosity_driven", 200)` |
| Evaluate agent | - | `agent.evaluate_with_benchmark([env1, env2])` |

---

## NEW: Configuration Presets

The enhanced API includes preset configurations:

- `"development"` - Developmental agent with maturation stages
- `"curiosity_driven"` - Curiosity and exploration focused
- `"reward_based"` - Reward and reinforcement learning
- `"critical_period"` - Critical period learning
- `"rl_integrated"` - Full reinforcement learning
- `"standard"` - Standard agent

```python
# Use any preset
agent = pynlm.create_development_agent()
agent = pynlm.create_curiosity_agent()
agent = pynlm.create_reward_agent()
```

---

## NEW: Helper Classes

The enhanced API includes helper classes for advanced users:

- `pynlm.SimulationManager` - Comprehensive simulation management
- `pynlm.TrainingLoop` - Training loop with evaluation
- `pynlm.ExplorationManager` - Exploration strategy management
- `pynlm.BenchmarkSuite` - Benchmarking framework
- `pynlm.AgentFactory` - Agent factory functions
- `pynlm.EnhancedAgentBrain` - Enhanced agent with additional features

```python
# Use helper classes
simulation = pynlm.run_complete_simulation(agent, world, 500)
metrics = pynlm.TrainingLoop(agent, world).train(100)
```

---

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

**NEW: Enhanced Troubleshooting**

**"My enhanced agent isn't learning"**
- Check that learning subsystems are enabled: `agent.enableRewardModulation(True)`
- Verify the preset is appropriate for your task
- Try shorter training episodes with more frequent evaluation

**"Simulation is crashing"**
- Try reducing the simulation time steps
- Use headless mode for faster execution
- Check that your environment is properly configured

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different enhanced configurations!
4. Try using the helper classes for more complex simulations
5. Use preset configurations to quickly set up different agent types

That's it! You're now ready to use NLM with the enhanced API for faster development and more powerful simulations.
