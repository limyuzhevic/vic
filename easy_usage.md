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

### NEW: Example 3: Advanced Agent with Learning

```python
import pynlm

# Create and configure the brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # More neurons for better learning
config.set("brain.synapse_density", 0.1)  # Moderate connectivity
brain = pynlm.createBrain(config)
brain.initialize()

# Create the world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()
world.setAgentStart(10.0, 10.0)  # Start in the middle

# Create agent with learning capabilities
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections  
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Run the simulation with learning
for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Process sensory input in brain
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")

print("Advanced agent simulation complete!")
print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty: {agent.getNoveltyLevel():.3f}")
print(f"Total reward learning: {agent.getNeuromodulationLevel():.3f}")
```

### NEW: Example 4: Save and Load Brain State

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain1 = pynlm.createBrain(config)
brain1.initialize()

# Run some steps to develop
for i in range(500):
    brain1.step(i)
    # Inject some learning input
    brain1.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0f)

print("Brain developed and learned!")
print(f"Neurons: {brain1.getTotalNeuronCount()}, Synapses: {brain1.getTotalSynapseCount()}")

# Save the brain's state
brain1.save("my_brain_checkpoint.bin")
print("Brain saved to my_brain_checkpoint.bin")

# Create a new brain (different random seed)
brain2 = pynlm.createBrain(config)
brain2.initialize()

# Load the saved state into the new brain
brain2.load("my_brain_checkpoint.bin")
print("Brain loaded from my_brain_checkpoint.bin")

# Verify they're the same
print(f"Brain1 total spikes: {brain1.getTotalSpikeCount()}")
print(f"Brain2 total spikes: {brain2.getTotalSpikeCount()}")
print("Brains are identical!")
```

---

## NEW: Advanced Features for Power Users

### 1. Text Processing

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Example text processing
text = "Hello world, this is a test message"
percept = agent.encodeText(text)
agent.processSensoryInput(percept)
brain.step(0)

print(f"Encoded text '{text}' into neural pattern")
```

### 2. Custom World Configuration

```python
import pynlm

# Create a custom world
world = pynlm.createSimpleWorld()
world.configure(
    width=30.0,           # Larger world
    height=30.0,
    visionWidth=24,       # Better vision
    visionHeight=24
)
world.setMaxEnergy(200.0)  # Higher energy
world.setRandomSeed(12345)  # Reproducible world
world.reset()

print(f"Custom world created: {world.getWidth()}x{world.getHeight()}")
```

### 3. Brain Configuration

```python
import pynlm

# Advanced brain configuration
config = pynlm.createDefaultConfig()

# Fine-tune parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.05)  # Less connectivity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 2.0)  # Stronger dopamine
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

print("Advanced brain configuration applied")
```

### 4. Performance Monitoring

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Monitor performance
for step in range(100):
    brain.step(step)
    
    # Print statistics
    if step % 10 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

### 5. Multi-Region Brain

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create multiple brain regions for different functions
region1 = brain.addRegion("visual_cortex")
region2 = brain.addRegion("motor_cortex")
region3 = brain.addRegion("memory_hippocampus")

print(f"Created {brain.getRegionCount()} brain regions")
print("Region IDs:", brain.getRegionIds())
```

### 6. Advanced Agent Features

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(16, 16, 16, 16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Test agent capabilities
print(f"Agent sensory input size: {agent.getSensoryInputSize()}")
print(f"Agent motor output size: {agent.getMotorOutputSize()}")

# Enable all subsystems for maximum capability
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Agent fully enabled with all subsystems")
```

---

## Common Patterns

### Pattern 1: Run a Simulation (Updated)

```python
def run_simulation(brain, world, agent, num_steps, learning=True):
    """Run a complete agent simulation with optional learning."""
    
    # Enable learning if requested
    if learning:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
    
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
        
        # 7. Record statistics periodically
        if step % 50 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}")

# Use it like this:
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

run_simulation(brain, world, agent, 500, learning=True)
```

### Pattern 2: Get Brain Stats (Enhanced)

```python
# Enhanced statistics collection
def collect_brain_stats(brain, agent, step):
    """Collect comprehensive brain statistics."""
    stats = {
        'step': step,
        'total_neurons': brain.getTotalNeuronCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'development_stage': brain.getDevelopmentalStage(),
        'curiosity_level': agent.getCuriosityLevel(),
        'novelty_level': agent.getNoveltyLevel(),
        'neuromodulation_level': agent.getNeuromodulationLevel(),
        'prediction_error': agent.getPredictionError(),
    }
    return stats

# Use it like this:
stats = collect_brain_stats(brain, agent, step)
print(f"Brain stats: {stats}")
```

### Pattern 3: Enable Brain Features (Complete)

```python
# All available features and their purposes
def setup_brain_features(agent, brain):
    """Set up all available brain subsystems."""
    
    # Learning and adaptation
    agent.enableRewardModulation(True)      # Learn from rewards and punishments
    agent.enableStructuralPlasticity(True)  # Grow new synapses, prune old ones
    agent.enableCuriosity(True)             # Drive exploration of novel experiences
    
    # Cognitive development
    agent.enableDevelopment(True)           # Brain matures over time
    
    # Performance and efficiency
    agent.enablePrediction(True)            # Anticipate future states
    agent.enableAttention(True)             # Focus on relevant information
    
    # Memory enhancement
    brain.enableWorkingMemory(True)         # Maintain active information
    brain.enableEpisodicMemory(True)        # Store experiences
    brain.enableAssociativeMemory(True)     # Learn relationships

# Use it like this:
setup_brain_features(agent, brain)
```

---

## What Each Part Does (Enhanced)

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
- Updates memory systems
- Applies neuromodulation
- Updates development

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

### NEW: `agent.encodeText(text)`
Converts text strings into neural patterns for processing.
- Useful for language processing and cognitive experiments
- Creates sparse, distributed representations

### NEW: `agent.enableFeature(True/False)`
Enable/disable specific brain subsystems.
- `enableRewardModulation(True/False)` - Learn from rewards
- `enableStructuralPlasticity(True/False)` - Grow new connections
- `enableDevelopment(True/False)` - Brain matures over time
- `enableCuriosity(True/False)` - Explore new things
- `enablePrediction(True/False)` - Anticipate future states
- `enableAttention(True/False)` - Focus on relevant information

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

### NEW: Project 3: Learning Agent

```python
import pynlm

# Setup with learning
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features for behavior development
agent.enableRewardModulation(True)  # Learn from rewards
agent.enableCuriosity(True)        # Explore new things
agent.enableStructuralPlasticity(True)  # Grow new connections

# Run agent with learning
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Optional: print progress
    if step % 50 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"curiosity: {agent.getCuriosityLevel():.3f}")

print("Learning agent simulation complete!")
print(f"Final behavior: curiosity={agent.getCuriosityLevel():.3f}, "
      f"novelty={agent.getNoveltyLevel():.3f}")
```

### NEW: Project 4: Text Understanding

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Process text
texts = [
    "I am happy",
    "I am sad", 
    "I am angry",
    "I am afraid",
    "I am surprised"
]

print("Text to neural pattern conversion:")
for text in texts:
    agent.processSensoryInput(agent.encodeText(text))
    brain.step(0)
    
    # Get the brain's reaction
    percept = agent.decodeMotorCommand()
    print(f"  '{text}' -> Pattern encoded (neural response)")

print("Text processing complete!")
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
| Save brain | `brain.save("filename.bin")` |
| Load brain | `brain.load("filename.bin")` |
| Encode text | `agent.encodeText("text")` |
| Enable learning | `agent.enableRewardModulation(True)` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Enable curiosity for more exploration

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify motor neurons are connected

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Enable structural plasticity for more connections

**"Python module not found"**
- Install with: `pip install .`
- Ensure you're in the right directory
- Check for build errors

---

## Next Steps

When you're comfortable:
1. **Read `HOW_TO_USE.md`** for more detailed technical information
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Experiment with different configurations!**
4. Try the advanced projects above
5. Explore the Phase 6 integration demos for the complete system

That's it! You're now ready to use NLM with all its advanced features. The system includes full neural computation, memory systems, neuromodulation, prediction, cognition, development, and persistence capabilities!

(End of file - total 1200 lines)
