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

### Advanced Example: Using Enhanced Python Features

```python
import pynlm

# Create a complete agent system using the enhanced helper function
brain, agent, world = pynlm.createCompleteAgentSystem(
    neuronCount=1000, 
    regionCount=2
)

# Enable all learning features
agent.enableAllFeatures()

# Run a simulation with all features enabled
print("Running advanced simulation...")
pynlm.runSimulationWithLearning(brain, world, agent, 200)

# Get system statistics
stats = pynlm.getSystemStats(brain, agent)
print("System Statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")

# Test Phase 6 features
print("\nTesting Phase 6 features...")
apple_pattern = pynlm.encodeText("apple")
print(f"Encoded 'apple' to {len(apple_pattern)} neurons")

thought_trace = agent.getThoughtTrace()
print(f"Thought trace: {thought_trace}")

internal_rates = agent.getInternalFiringRates()
print(f"Internal firing rates: {len(internal_rates)} neurons")

# Test checkpoint functionality
import tempfile
import os

with tempfile.NamedTemporaryFile(suffix='.bin', delete=False) as tmp:
    filepath = tmp.name

print(f"\nTesting checkpoint save/load to {filepath}")
if brain.save(filepath):
    print("✓ Checkpoint saved successfully")
    
    # Create new brain and load
    config = pynlm.createDefaultConfig()
    new_brain = pynlm.createBrain(config)
    new_brain.initialize()
    
    if new_brain.load(filepath):
        print("✓ Checkpoint loaded successfully")
        print(f"✓ Loaded {new_brain.getTotalNeuronCount()} neurons")
    else:
        print("✗ Failed to load checkpoint")
else:
    print("✗ Failed to save checkpoint")

# Clean up
if os.path.exists(filepath):
    os.unlink(filepath)

# Print final brain status
print("\nFinal brain status:")
pynlm.printBrainStatus(brain)

print("\n✅ Advanced simulation complete!")
```

---

## Enhanced AgentBrain Methods

The enhanced Python bindings include additional methods for more advanced use:

### Phase 6 Advanced Features

#### `encodeText(text)`
Encode a text string into neural patterns for text association learning:
```python
# Encode words for text association learning
apple_pattern = pynlm.encodeText("apple")
word_pattern = pynlm.encodeText("hello")
```

#### `getThoughtTrace()`
Get a human-readable record of the most recent brain loop:
```python
# Get the thought trace (what was perceived, what was planned,
# what action was taken, and the outcome)
thought = agent.getThoughtTrace()
print(thought)  # e.g., "Percept: [0.1, 0.8, ...] | Plan: MoveForward | Action: MoveForward | Reward: 0.5"
```

#### `getLearnedPreference()` and `setLearnedPreference()`
Get/set the learned preference trait (Phase 5 feature):
```python
# Get the personality/learning bias developed over time
preference = agent.getLearnedPreference()
print(f"Preference: {preference:.3f}")

# Adjust learned preference
agent.setLearnedPreference(0.3)
```

#### `getInternalFiringRates()`
Get internal firing rates for text association recall:
```python
# Get internal neuron firing rates (for text-scene association recall)
rates = agent.getInternalFiringRates()
for i, rate in enumerate(rates):
    print(f"Internal neuron {i}: {rate:.3f} Hz")
```

### Enhanced Diagnostic Methods

#### `getMotorGroupActivities()`
Get detailed motor group activity statistics:
```python
# Get detailed motor group statistics
motor_groups = agent.getMotorGroupActivities()
for group in motor_groups:
    print(f"{group.name}: activity={group.activity:.3f}, firing_rate={group.firingRate:.1f} Hz, ")
```

#### `getSensoryInputSize()` and `getMotorOutputSize()`
Get expected input/output sizes:
```python
# Get expected sensory input and motor output sizes
input_size = agent.getSensoryInputSize()
output_size = agent.getMotorOutputSize()
print(f"Sensory input size: {input_size}")
print(f"Motor output size: {output_size}")
```

### Development and Learning Statistics

#### `getNeuromodulationLevel()`
Get current neuromodulation level:
```python
neuro_level = agent.getNeuromodulationLevel()
print(f"Neuromodulation level: {neuro_level:.3f}")
```

#### `getCuriosityLevel()` and `getNoveltyLevel()`
Get exploration motivation and novelty detection:
```python
curiosity = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()
print(f"Curiosity: {curiosity:.3f}, Novelty: {novelty:.3f}")
```

---

## Convenience Functions

The enhanced bindings include several convenience functions for common scenarios:

### `runSimulation(brain, world, agent, num_steps)`
Run a basic agent simulation:
```python
# Run a simple simulation
pynlm.runSimulation(brain, world, agent, 1000)
```

### `runSimulationWithLearning(brain, world, agent, num_steps)`
Run a complete simulation with all learning features enabled:
```python
# Run simulation with all learning features
pynlm.runSimulationWithLearning(brain, world, agent, 500)
```

### `createCompleteAgentSystem(neuron_count=1000, region_count=2)`
Create a complete agent system with brain, agent, and world:
```python
# Create a ready-to-use agent system
brain, agent, world = pynlm.createCompleteAgentSystem(
    neuronCount=2000,  # Number of neurons
    regionCount=3     # Number of brain regions
)
```

### `getSystemStats(brain, agent)`
Get comprehensive system statistics:
```python
# Get detailed system statistics
stats = pynlm.getSystemStats(brain, agent)
for category, metrics in stats.items():
    print(f"{category}:")
    for key, value in metrics.items():
        print(f"  {key}: {value}")
```

### `printBrainStatus(brain)`
Print detailed brain status information:
```python
# Print comprehensive brain status
pynlm.printBrainStatus(brain)
```

### `enableAllFeatures(agent)` and `disableAllFeatures(agent)`
Enable or disable all learning features:
```python
# Enable all learning features
pynlm.enableAllFeatures(agent)

# Disable all learning features (for reset simulations)
pynlm.disableAllFeatures(agent)
```

### Enumeration Helpers

Get lists of available options:
```python
# Get developmental stages
development_stages = pynlm.createDevelopmentStages()
print("Development stages:", development_stages)

# Get neuron types
neuron_types = pynlm.createNeuronTypes()
print("Neuron types:", neuron_types)

# Get action types
action_types = pynlm.createActionTypes()
print("Action types:", action_types)

# Get world object types
object_types = pynlm.createWorldObjectTypes()
print("World object types:", object_types)
```

---

## Checkpoint Enhanced Features

The enhanced config system provides better JSON support for checkpoint files:

### JSON Configuration Files
```python
# Create JSON config files (new enhanced format)
import json
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("learning.rate", 0.001)

# Save to JSON (human-readable)
config.saveToFile("brain_config.json")

# Load from JSON
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("brain_config.json")
```

### Complex Data Types in Config
```python
# Configuration can now store complex data types
config.set("vector_param", [1.0, 2.0, 3.0, 4.0])
config.set("string_list", ["hello", "world", "test"])
config.set("matrix", [[1, 2, 3], [4, 5, 6]])
```

---

## Advanced Pattern: Reinforcement Learning Agent

Here's a complete example of an agent using all enhanced features:

```python
import pynlm
import random

def train_rl_agent(epochs=10):
    """Train an agent using enhanced NLM features"""
    print("Starting reinforcement learning training...")
    
    # Create enhanced agent system
    brain, agent, world = pynlm.createCompleteAgentSystem(
        neuronCount=1500,
        regionCount=3
    )
    
    # Configure world
    world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
    world.reset()
    
    # Enable all features for learning
    pynlm.enableAllFeatures(agent)
    
    # Training loop
    for epoch in range(epochs):
        world.reset()
        step = 0
        episode_reward = 0.0
        
        while step < 500 and step < 100:  # Max steps
            # Get random action for training
            action_type = random.choice(list(pynlm.ActionType))
            action = pynlm.Action(action_type)
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward
            percept = world.getSensoryPercept()
            reward = 0.0
            if not percept.getInternal().empty():
                reward = percept.getInternal()[0]
            
            episode_reward += reward
            
            # Learn from experience
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Get next percept
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            step += 1
            
            if step % 50 == 0:
                print(f"  Epoch {epoch}: Step {step}, Reward: {episode_reward:.2f}")
        
        # Print epoch summary
        stats = pynlm.getSystemStats(brain, agent)
        print(f"Epoch {epoch}: Completed {step} steps, Total reward: {episode_reward:.2f}, "
              f"Firing neurons: {stats['firing_neurons']}")
        
        # Save checkpoint occasionally
        if epoch % 5 == 0:
            filepath = f"checkpoint_epoch_{epoch}.bin"
            if brain.save(filepath):
                print(f"  ✓ Checkpoint saved: {filepath}")
    
    # Save final model
    final_brain_path = "final_model.bin"
    if brain.save(final_brain_path):
        print(f"✓ Final model saved: {final_brain_path}")
    
    print(f"\n✅ Training complete! Final preference: {agent.getLearnedPreference():.3f}")
    
    # Print final system status
    print("\nFinal system status:")
    pynlm.printBrainStatus(brain)
    
    return brain, agent, world

# Run the training
brain, agent, world = train_rl_agent(epochs=5)
```
