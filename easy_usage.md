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

## Advanced Usage Examples

### Example 1: Using Advanced Scenarios

```python
import pynlm
from pynlm import AdvancedUtils

# Create an exploration scenario (high novelty, learning-focused)
brain = AdvancedUtils.create_advanced_scenario(
    scenario_type="exploration",
    neuron_count=2000,
    region_count=3,
    simulation_steps=1000
)

# Run simulation with batch processing
from pynlm import BatchProcessor
processor = AdvancedUtils.create_batch_processor(
    num_steps=1000,
    batch_size=100
)

for i in range(processor.get_total_batches()):
    results = processor.process_batch(brain)
    summary = results.get_summary()
    print(f"Batch {i}: {summary}")
```

### Example 2: Performance Monitoring and Data Export

```python
import pynlm
import tempfile
import os

# Create a complex brain with all systems enabled
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create performance monitor
from pynlm import AdvancedUtils
from pynlm import BatchProcessor

processor = AdvancedUtils.create_batch_processor(num_steps=500, batch_size=50)

print("Running simulation with performance monitoring...")
for i in range(processor.get_total_batches()):
    results = processor.process_batch(brain)
    print(f"Batch {i}: {results.get_summary()}")

# Export brain data
with tempfile.TemporaryDirectory() as tmpdir:
    data_file = os.path.join(tmpdir, "brain_data.csv")
    AdvancedUtils.export_brain_data(brain, data_file, "csv")
    print(f"Brain data exported to: {data_file}")
```

### Example 3: Debugging and Validation

```python
import pynlm
from pynlm import DebugTools

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Validate brain state
validation_result = DebugTools.validate_brain_state(brain)
print(validation_result.get_report())

# Run a few simulation steps
for step in range(50):
    brain.step(step)

# Validate again after simulation
validation_result = DebugTools.validate_brain_state(brain)
print("\nAfter simulation:")
print(validation_result.get_report())
```

### Example 4: Scenario Comparison

```python
import pynlm
from pynlm import ScenarioManager

# Compare different scenario types
scenario_types = ["exploration", "exploitation", "development", "stable", "balanced"]
results = ScenarioManager.run_scenario_comparison(scenario_types)

# Compare and save results
comparison = ScenarioManager.compare_scenarios(
    results, 
    output_file="scenario_comparison.json"
)

print("Best performing scenario:", comparison["best_scenario"])
print("Most stable scenario:", comparison["most_stable_scenario"])
```

### Example 5: Complete Agent with Advanced Features

```python
import pynlm
from pynlm import AdvancedUtils

# Setup complete agent with all features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Starting advanced agent simulation...")
print(f"Initial neurons: {brain.getTotalNeuronCount()}")
print(f"Initial developmental stage: {brain.getDevelopmentalStage()}")

# Run simulation with performance monitoring
from pynlm import BatchProcessor
processor = AdvancedUtils.create_batch_processor(num_steps=1000, batch_size=100)

for batch_idx in range(processor.get_total_batches()):
    print(f"\n--- Batch {batch_idx + 1} ---")
    results = processor.process_batch(brain, agent, world)
    
    summary = results.get_summary()
    print(f"Spikes in batch: {summary['total_spike_increase']}")
    print(f"Average firing change: {summary['average_firing_change']:.2f}")
    print(f"Current developmental stage: {brain.getDevelopmentalStage()}")
    
    # Print brain status periodically
    if batch_idx % 2 == 0:
        brain.logStatus()

print("\nSimulation complete!")
print(f"Final spike count: {brain.getTotalSpikeCount()}")
print(f"Final firing rate: {brain.getAverageFiringRate():.4f}")
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

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
