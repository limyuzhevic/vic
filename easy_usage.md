# NLM - Easy Usage Guide for Beginners

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

---

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

---

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

---

## Advanced Features

For more sophisticated usage, check out the `advanced_api.py` module which provides:

- **AdvancedBrain**: Enhanced brain interface with Pythonic features
- **InteractiveAgent**: Interactive agent with experience tracking
- **NLMConfigurator**: Configuration manager with presets
- **NLMExperiment**: Experiment management system
- **Utility functions**: Brain validation, benchmarking, and research helpers

```python
# Advanced usage example
import pynlm
from advanced_api import create_advanced_brain, NLMConfigurator

# Create advanced brain with preset configuration
brain = create_advanced_brain(NLMConfigurator.create_config('research'))

# Run simulation with detailed statistics
for step in range(1000):
    brain.simulate(1, lambda s, stats: print(f"Step {s}: {stats['firing_neurons']} neurons firing"))
```

---

## Advanced Tutorials

### Tutorial 1: Building a Learning Agent

This tutorial shows how to create an agent that learns from experience using NLM's advanced features.

```python
import pynlm
from advanced_api import AdvancedBrain, InteractiveAgent, NLMConfigurator

# 1. Configure the brain for learning
config = NLMConfigurator.create_config('research')
brain = AdvancedBrain(config)

# 2. Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()
agent = InteractiveAgent(brain, world)

# 3. Enable learning subsystems
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# 4. Run learning episode
for step in range(500):
    # Environment update
    world.update(0.1)
    
    # Agent perceives world
    percept = world.getSensoryPercept()
    
    # Brain processes input and decides action
    action = agent.step(percept)
    
    # Learn from experience
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.apply_reward(reward)

print("Learning episode complete!")
print(f"Final statistics: {brain.get_stats()}")
```

### Tutorial 2: Research Experiment Framework

Set up systematic research experiments with reproducible results.

```python
from advanced_api import NLMExperiment, NLMConfigurator

# Create experiment configuration
config = NLMConfigurator.create_config('research', 
                                      brain.neuron_count=2000,
                                      plasticity.stdp.learning_rate=0.005)

# Initialize experiment components
brain = AdvancedBrain(config)
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50)
agent = InteractiveAgent(brain, world)

# Create and run experiment
experiment = NLMExperiment(brain, world, agent)
results = experiment.run_experiment(episodes=20, steps_per_episode=100)

print(f"Experiment results: {results}")
```

### Tutorial 3: Brain Configuration Presets

Use built-in configuration presets or create custom ones.

```python
from advanced_api import NLMConfigurator

# Access all available presets
presets = NLMConfigurator.get_presets()
print("Available presets:", list(presets.keys()))

# Create brain with custom configuration
custom_config = NLMConfigurator.create_config(
    brain.neuron_count=750,
    plasticity.stdp.enable=True,
    plasticity.stdp.learning_rate=0.02,
    neuromod.curiosity.enable=True
)

brain = AdvancedBrain(custom_config)
```

## What Each Part Does

### `advanced_api.py` Module

The `advanced_api.py` module provides enhanced interfaces:

- **AdvancedBrain**: Wraps the basic brain with Pythonic features
- **InteractiveAgent**: Enhanced agent with history tracking and experience management
- **NLMConfigurator**: Configuration manager with validation and presets
- **NLMExperiment**: Experiment runner with systematic testing
- **Utility functions**: Brain validation, benchmarking, and research helpers

### Integration with Basic NLM

The advanced API integrates seamlessly with the existing NLM system:

1. **Backward compatible**: All existing code continues to work
2. **Enhanced features**: New Pythonic interfaces for better usability
3. **Research tools**: Built-in support for systematic experiments
4. **Configuration management**: Presets and validation
5. **Performance monitoring**: Benchmarking and statistics

## Quick Reference - Advanced API

| What you want | Advanced API |
|--------------|--------------|
| Create brain | `create_advanced_brain(config)` |
| Run simulation | `brain.simulate(steps, callback)` |
| Get stats | `brain.get_stats()` |
| Create agent | `InteractiveAgent(brain, world)` |
| Run experiment | `NLMExperiment(brain, world, agent).run_experiment(episodes, steps)` |
| Use preset | `NLMConfigurator.create_config('research')` |

---

## Advanced Features and Extensions

### 1. Plugin Architecture (Future Enhancement)

The NLM framework is designed for future plugin support:

- **Pluggable plasticity rules**: Custom learning mechanisms
- **Custom neuron models**: Alternative neuron dynamics
- **External memory systems**: Integration with databases
- **Visualization plugins**: Custom output formats

### 2. Performance Monitoring

Built-in monitoring and debugging tools:

```python
# Monitor brain performance
benchmark = benchmark_brain(brain, 1000)
print(f"Performance: {benchmark['steps_per_second']:.1f} steps/sec")

# Validate brain state
issues = validate_brain_state(brain)
if issues:
    print("Issues found:", issues)
```

### 3. Research Tools

Tools for systematic research:

- **Experiment tracking**: Automated result collection
- **Parameter sweeps**: Grid search for hyperparameter optimization
- **Reproducibility**: Built-in checkpointing and configuration management
- **Statistical analysis**: Built-in metrics and aggregation

## Troubleshooting

**"Advanced API not working"**
- Ensure you're using the latest version
- Check that all dependencies are installed
- Try using the basic API first, then upgrade

**"Configuration presets not found"**
- Check that you're importing from the correct module
- Verify configuration file is accessible

**"Performance issues"**
- Use smaller configurations for development
- Monitor benchmarks to identify bottlenecks
- Consider parallelization for large simulations

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try the `advanced_api.py` module for enhanced features
5. Explore the example scripts in `examples/` directory

That's it! You're now ready to use NLM at both basic and advanced levels.
