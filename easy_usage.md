# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns
- **Prediction** = The brain anticipating what will happen next
- **Attention** = The brain focusing on what's important
- **Development** = The brain maturing over time
- **Neuromodulation** = The brain using signals like dopamine to guide learning

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

### Example 3: Using Memory Systems

```python
import pynlm

# Create brain with memory systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access working memory
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory has {working_memory.getActiveTraces()} active traces")
    print(f"Working memory capacity: {working_memory.getCapacity()}")
    print(f"Working memory decay rate: {working_memory.getDecayRate()}")
    
    # Store some information
    test_pattern = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]
    working_memory.store(test_pattern, 1.0)
    
    # Retrieve information
    retrieved = working_memory.retrieve()
    print(f"Retrieved {len(retrieved)} values")

# Access episodic memory
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"Episodic memory has {episodic_memory.getEpisodeCount()} episodes")
    print(f"Replay enabled: {episodic_memory.isReplayEnabled()}")
    
    # Store an experience
    from datetime import datetime
    current_time = datetime.now().timestamp()
    
    episode = nlm.EpisodicMemoryItem()
    episode.timestamp = current_time
    episode.sensoryState = test_pattern
    episode.positionX = 5.0
    episode.positionY = 5.0
    episode.action = nlm.ActionType.MoveForward
    episode.reward = 1.0
    
    episodic_memory.storeEpisode(episode)

# Access associative memory
associative_memory = brain.getAssociativeMemory()
if associative_memory:
    print(f"Associative memory has {associative_memory.getAssociationCount()} associations")
    
    # Create association between two patterns
    pattern_a = [0.1, 0.2, 0.3, 0.4]
    pattern_b = [0.5, 0.6, 0.7, 0.8]
    
    associative_memory.associate(pattern_a, pattern_b, 0.8)
    strength = associative_memory.getAssociationStrength(pattern_a, pattern_b)
    print(f"Association strength: {strength}")

print("Memory systems test completed!")
```

### Example 4: Using Prediction System

```python
import pynlm

# Create brain with prediction system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access prediction system
prediction_system = brain.getPredictionSystem()
if prediction_system:
    print("Prediction system available")
    # Can add prediction-related functionality here
    pass

print("Prediction system test completed!")
```

### Example 5: Using Cognition Systems

```python
import pynlm

# Create brain with cognition systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access neural planner
planner = brain.getPlanner()
if planner:
    print("Neural planner available")
    # Can add planning functionality here
    pass

# Access concept formation
concept_formation = brain.getConceptFormation()
if concept_formation:
    print("Concept formation system available")
    # Can add concept formation functionality here
    pass

# Access attentional selection
attention = brain.getAttention()
if attention:
    print("Attentional selection system available")
    # Can add attention functionality here
    pass

# Access development system
development_system = brain.getDevelopmentSystem()
if development_system:
    print(f"Development system at stage: {brain.getDevelopmentalStage()}")

print("Cognition systems test completed!")
```

### Example 6: Using Neuromodulation Systems

```python
import pynlm

# Create brain with neuromodulation systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access dopamine system
dopamine = brain.getDopamine()
if dopamine:
    print("Dopamine neuromodulator available")
    # Can add dopamine-related functionality here
    pass

# Access curiosity system
curiosity = brain.getCuriosity()
if curiosity:
    print(f"Curiosity level: {curiosity.getCuriosityLevel():.3f}")
    # Can add curiosity functionality here
    pass

# Access novelty system
novelty = brain.getNovelty()
if novelty:
    print("Novelty detection system available")
    # Can add novelty functionality here
    pass

# Access prediction error signal
prediction_error = brain.getPredictionErrorSignal()
if prediction_error:
    print(f"Prediction error: {prediction_error.getPredictionError():.3f}")
    # Can add prediction error functionality here
    pass

print("Neuromodulation systems test completed!")
```

### Example 7: Using Spike and Plasticity Systems

```python
import pynlm

# Create brain with spike and plasticity systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access spike system
spike_system = brain.getSpikeSystem()
if spike_system:
    print(f"Spike system has {spike_system.getPendingSpikeEventCount()} pending events")
    # Can add spike system functionality here
    pass

# Access STDP plasticity system
stdp = brain.getSTDP()
if stdp:
    print("STDP plasticity system available")
    # Can add STDP functionality here
    pass

# Access Hebbian plasticity system
hebbian = brain.getHebbian()
if hebbian:
    print("Hebbian plasticity system available")
    # Can add Hebbian functionality here
    pass

# Access structural plasticity system
structural = brain.getStructuralPlasticity()
if structural:
    print("Structural plasticity system available")
    # Can add structural plasticity functionality here
    pass

print("Spike and plasticity systems test completed!")
```

### Example 8: Using AgentBrain Advanced Features

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Access advanced AgentBrain functionality
if hasattr(agent, 'getThoughtTrace'):
    thought_trace = agent.getThoughtTrace()
    print(f"Thought trace available: {len(thought_trace)} chars")
    # Can add thought trace functionality here
    pass

if hasattr(agent, 'getMotorGroupActivities'):
    activities = agent.getMotorGroupActivities()
    print(f"Motor group activities: {len(activities)} groups")
    # Can add motor group analysis functionality here
    pass

if hasattr(agent, 'encodeText'):
    text_pattern = agent.encodeText("hello world")
    print(f"Text encoding successful, pattern length: {len(text_pattern)}")
    # Can add text-as-a-sense functionality here
    pass

if hasattr(agent, 'getInternalFiringRates'):
    internal_rates = agent.getInternalFiringRates()
    print(f"Internal firing rates: {len(internal_rates)} neurons")
    # Can add internal firing rate analysis functionality here
    pass

print("AgentBrain advanced features test completed!")
```

### Example 9: Enhanced Configuration

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("test.int", 42)
config.set("test.double", 3.14)
config.set("test.bool", True)
config.set("test.string", "hello")
config.set("test.vector.int", [1, 2, 3, 4, 5])
config.set("test.vector.double", [1.1, 2.2, 3.3])

# Get configuration values
int_value = config.getOr("test.int", 0)
double_value = config.getOr("test.double", 0.0)
bool_value = config.getOr("test.bool", False)
string_value = config.getOr("test.string", "")

# Check if configuration key exists
if config.has("test.int"):
    print("test.int exists")

# Get all keys
all_keys = config.getKeys()
print(f"All keys: {all_keys}")

# Remove configuration key
config.remove("test.int")

print("Configuration test completed!")
```

### Example 10: Enhanced Clone functionality

```python
import pynlm

# Test clone functionality
vision = pynlm.Vision(8, 8, 3)
cloned_vision = vision.clone()
print(f"Vision cloned: {vision.getWidth()}x{vision.getHeight()}")

# Create some vision data
test_data = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 
             0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.0,
             0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9,
             0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.0]
vision.setData(test_data)

# Clone with data
cloned_vision2 = vision.clone()
print(f"Vision with data cloned: {cloned_vision2.getWidth()}x{cloned_vision2.getHeight()}")

# Test Audio clone
audio = pynlm.Audio(44100, 1024)
cloned_audio = audio.clone()
print(f"Audio cloned: {audio.getSampleRate()} Hz, {audio.getNumSamples()} samples")

# Test InternalSignals clone
internal = pynlm.InternalSignals()
internal.addSignal(0.5)
internal.addSignal(0.7)
cloned_internal = internal.clone()
print(f"InternalSignals cloned with {len(internal.getData())} signals")

# Test Action clone
action = pynlm.Action(pynlm.ActionType.MoveForward)
cloned_action = action.clone()
print(f"Action cloned: type {action.getType()}")

print("Clone functionality test completed!")
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

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
