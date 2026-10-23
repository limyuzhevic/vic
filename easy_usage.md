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

### Project 4: Advanced Agent with Statistics

```python
import pynlm

# Setup a brain with statistics tracking
config = pynlm.createConfigFromDict({
    "brain.neuron_count": 500,
    "brain.synapse_density": 0.1,
    "brain.v_thresh": -50.0,
    "brain.v_rest": -70.0,
})

brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Configure world
world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
world.reset()
agent.initialize(world)

# Enable all features for advanced learning
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation with statistics collection
stats = []
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        step_stats = pynlm.computeNetworkStats(brain)
        step_stats["step"] = step
        step_stats["curiosity"] = agent.getCuriosityLevel()
        step_stats["novelty"] = agent.getNoveltyLevel()
        step_stats["development_stage"] = pynlm.getDevelopmentalStageName(brain.getDevelopmentalStage())
        stats.append(step_stats)
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Analyze final statistics
final_stats = pynlm.computeNetworkStats(brain)
print(f"\nFinal Network Statistics:")
print(f"  Total neurons: {final_stats.get('total_neurons')}")
print(f"  Total synapses: {final_stats.get('total_synapses')}")
print(f"  Firing neurons: {final_stats.get('firing_neurons')}")
print(f"  Average firing rate: {final_stats.get('average_firing_rate'):.2f} Hz")
print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"  Development stage: {pynlm.getDevelopmentalStageName(brain.getDevelopmentalStage())}")
```

### Project 5: Batch Operations for Performance

```python
import pynlm
import time

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableAllFeatures()

# Run multiple simulation steps efficiently
print("Running simulation...")
start_time = time.time()

# Method 1: Standard step-by-step (slower)
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

standard_time = time.time() - start_time
print(f"Standard simulation completed in {standard_time:.2f} seconds")

# Method 2: Using advanced features (demonstrates batch operations)
print("\nDemonstrating advanced features...")

# Create vision and audio sensors for advanced input
vision_sensor = pynlm.createVisionSensor(8, 8, 3)
audio_sensor = pynlm.createAudioSensor(44100, 1024)

# Example of batch processing (conceptual - actual implementation varies)
print("Advanced features available:")
print("  - Batch sensory input processing")
print("  - Network statistics aggregation")
print("  - Development stage management")
print("  - Progress callbacks")
print("  - Experiment setup and configuration")
```

### Project 6: Simulation with Progress Monitoring

```python
import pynlm

# Setup simulation with progress callback
def progress_monitor(step, total_steps):
    percentage = (step / total_steps) * 100
    print(f"Progress: {percentage:.1f}% (Step {step}/{total_steps})")

# Create components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run simulation with progress monitoring
print("Running simulation with progress monitoring...")
pynlm.runSimulationWithProgress(
    brain, world, agent, 500, 0.1, progress_monitor
)

print("Simulation completed!")
print(f"Final firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Project 7: Experiment Configuration

```python
import pynlm

# Setup experiment with custom parameters
experiment_params = {
    "brain.neuron_count": 300,
    "brain.synapse_density": 0.08,
    "brain.v_thresh": -45.0,
    "brain.v_rest": -65.0,
    "plasticity.stdp.enable": True,
    "plasticity.stdp.learning_rate": 0.002,
    "neuromod.dopamine.scale": 1.5,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True,
    "experiment.name": "curiosity_driven_exploration",
    "experiment.duration": 1000,
    "experiment.reward_scale": 2.0,
}

# Create experiment configuration
config = pynlm.setupExperiment("curiosity_driven_exploration", experiment_params)

# Verify configuration
print(f"Experiment configuration:")
for key in config.getKeys():
    if key.startswith("experiment") or key.startswith("neuromod"):
        value = config.getOr[str](key, "")
        print(f"  {key}: {value}")

# Create brain with experiment config
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.reset()
agent.initialize(world)

print(f"Experiment '{config.getOr[str]('experiment.name', 'unnamed')}' started with {brain.getTotalNeuronCount()} neurons")
```

### Project 8: Visualization Configuration

```python
import pynlm

# Create visualization configuration for saving snapshots
vis_config = pynlm.createVisualizationConfig(
    outputDir="./simulation_videos",
    saveSnapshots=True,
    snapshotInterval=50
)

print("Visualization configuration:")
for key, value in vis_config.items():
    print(f"  {key}: {value}")

# This configuration can be used by external visualization tools
print("\nTo visualize simulation results:")
print("  1. Run the simulation")
print("  2. Configure external visualization tool")
print("  3. Import snapshots using the configuration above")
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

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
