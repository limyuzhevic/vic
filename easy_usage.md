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

## Advanced Usage Guide

This section covers advanced usage patterns and techniques for experienced NLM users.

### Advanced Project: Learning with Structured Connectivity

```python
import pynlm

# Create a brain with specific connectivity patterns
config = pynlm.createDefaultConfig()
config.set("connection_probability", 0.2)
config.set("excitatory_proportion", 0.85)

brain = pynlm.createBrain(config)
brain.initialize()

# Use advanced batch operations to inject patterns
import pynlm

# Get all sensory neurons for targeted stimulation
sensory_neurons = brain.getRegions()[0].getAllNeurons()
sensory_ids = [neuron.getId() for neuron in sensory_neurons if neuron.getType() == pynlm.NeuronType.Sensory]

# Inject correlated patterns to stimulate specific learning
pynlm.batchInjectCurrent(brain, sensory_ids[:10], 20.0, True)
pynlm.batchInjectCurrent(brain, sensory_ids[10:20], 15.0, False)

print("Applied structured learning patterns")
```

### Advanced Agent Configuration

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run extended training with monitoring
for episode in range(50):
    # Reset world and agent for each episode
    world.reset()
    agent.reset()
    agent.initialize(world)
    
    # Track learning metrics
    episode_reward = 0.0
    episode_steps = 0
    
    for step in range(200):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward
        reward = 1.0 if action.getType() == pynlm.ActionType.Interact else 0.0
        episode_reward += reward
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        episode_steps += 1
    
    # Get advanced metrics
    working_memory = brain.getWorkingMemoryTraces()
    episodic_memories = brain.getEpisodicMemoryCount()
    prediction_metrics = brain.getPredictionErrorMetrics()
    cognition_metrics = brain.getCognitionMetrics()
    
    print(f"Episode {episode}: Reward={episode_reward/episode_steps:.3f}, "
          f"WorkingMem={working_memory}, Episodic={episodic_memories}, "
          f"Novelty={prediction_metrics.get('novelty_score', 0):.3f}")
```

### Advanced Configuration Management

```python
import pynlm

# Create and configure brain with advanced settings
config = pynlm.createDefaultConfig()

# Basic configuration
config.set("neuron_count", 500)
config.set("region_count", 3)
config.set("simulation_timestep", 0.001)
config.set("random_seed", 12345)

# Advanced connectivity patterns
config.set("connection_probability", 0.15)
config.set("excitatory_proportion", 0.9)

# Neuromodulation parameters
config.set("reward_modulation_enabled", True)
config.set("curiosity_enabled", True)
config.set("structural_plasticity_rate", 0.0001)

# Development parameters
config.set("development_enabled", True)
config.set("developmental_stage", "critical_period")

# Prediction system parameters
config.set("prediction_horizon", 10)
config.set("prediction_confidence_threshold", 0.7)

# Save advanced configuration
config.saveToFile("/tmp/advanced_config.txt")
print("Advanced configuration saved")

# Load and merge with defaults
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("/tmp/advanced_config.txt")

# Merge configurations (advanced config overrides defaults)
config2.merge(config)

# Validate configuration
validation_result = config2.validate()
if validation_result.valid:
    print("Configuration validation passed")
else:
    print(f"Validation failed: {validation_result.errorMessage}")
```

### Advanced Memory and Development

```python
import pynlm

# Create brain with memory and development
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("development_enabled", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Configure world
world = pynlm.createSimpleWorld()
world.configure(width=40, height=40, visionWidth=20, visionHeight=20)
world.reset()

# Create agent with full capabilities
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run development-focused training
print("Running developmental learning simulation...")
for step in range(500):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes
    brain.step(step)
    
    # Get action and apply to world
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Calculate and apply reward
    reward = world.computeReward(brain.getRegions()[0].get())
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development system
    agent.updateDevelopment(0.1)
    
    # Check development stage changes
    developmental_stage = agent.getDevelopmentalStage()
    if step % 100 == 0:
        print(f"Step {step}: Developmental stage = {developmental_stage}")

# Analyze developmental results
print(f"Final developmental age: {agent.getDevelopmentalStage()}")
print(f"Final neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Final prediction error: {agent.getPredictionError():.3f}")

# Get memory statistics
working_memory_traces = brain.getWorkingMemoryTraces()
episodic_memories = brain.getEpisodicMemoryCount()
print(f"Working memory traces: {working_memory_traces}")
print(f"Episodic memories stored: {episodic_memories}")
```

### Advanced Experimentation with Checkpoints

```python
import pynlm
import time

# Create brain for experimentation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Configure for long-running experiment
config.set("checkpoint_dir", "/tmp/nlm_experiments")
config.set("checkpoint_frequency", 100)
config.set("checkpoint_save_memory", True)

# Create world and agent
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableDevelopment(True)

# Run experiment with checkpointing
experiment_start = time.time()
checkpoint_count = 0

for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    reward = world.computeReward(brain.getRegions()[0].get())
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
    
    # Manual checkpointing for critical points
    if step % 250 == 0:
        checkpoint_path = f"/tmp/experiment_checkpoint_{step}.bin"
        if brain.save(checkpoint_path):
            checkpoint_count += 1
            print(f"Checkpoint saved at step {step}")
            
            # Load from checkpoint and verify
            brain2 = pynlm.createBrain(config)
            brain2.initialize()
            
            if brain2.load(checkpoint_path):
                print(f"Checkpoint loaded successfully")
                # Note: Neural simulation state restoration is complex
                # but structural information should be restored
            
            std::filesystem::remove(checkpoint_path)

experiment_time = time.time() - experiment_start
print(f"Experiment completed in {experiment_time:.2f} seconds")
print(f"Checkpoints saved: {checkpoint_count}")
```

### Advanced Python Function Usage

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Test all advanced Python functions
print("=== Testing Advanced Python Functions ===")

# Get all active neurons
active_neurons = pynlm.getAllActiveNeurons(brain)
print(f"Active neurons: {len(active_neurons)}")

# Get working memory traces
memory_traces = pynlm.getWorkingMemoryTraces(brain)
print(f"Working memory traces: {memory_traces}")

# Get episodic memory count
episodic_count = pynlm.getEpisodicMemoryCount(brain)
print(f"Episodic memory count: {episodic_count}")

# Get prediction error metrics
error_metrics = pynlm.getPredictionErrorMetrics(brain)
print(f"Prediction error metrics: {error_metrics}")

# Get cognition metrics
cognition_metrics = pynlm.getCognitionMetrics(brain)
print(f"Cognition metrics: {cognition_metrics}")

# Test brain creation with random connectivity
config2 = pynlm.createDefaultConfig()
config2.set("connection_probability", 0.3)
config2.set("excitatory_proportion", 0.8)

brain2 = pynlm.createBrainWithRandomConnectivity(config2, 0.3, 0.8)
print(f"Brain with custom connectivity created: {brain2.getTotalNeuronCount()} neurons")

print("=== All Advanced Functions Tested Successfully ===")
```

---

## Advanced Constants Reference

These constants provide enhanced access to key NLM identifiers and values:

```python
import pynlm

# Neuron IDs
invalid_neuron_id = pynlm.INVALID_NEURON_ID  # 0

# Synapse IDs  
invalid_synapse_id = pynlm.INVALID_SYNAPSE_ID  # 0

# Region IDs
invalid_region_id = pynlm.INVALID_REGION_ID  # 0

# Population IDs
invalid_population_id = pynlm.INVALID_POPULATION_ID  # 0

# Action Types
move_forward = pynlm.ActionType.MoveForward
move_backward = pynlm.ActionType.MoveBackward
turn_left = pynlm.ActionType.TurnLeft
turn_right = pynlm.ActionType.TurnRight
look = pynlm.ActionType.Look
interact = pynlm.ActionType.Interact
rest = pynlm.ActionType.Rest
wait = pynlm.ActionType.Wait

# Neuron Types
excitatory = pynlm.NeuronType.Excitatory
inhibitory = pynlm.NeuronType.Inhibitory
sensory = pynlm.NeuronType.Sensory
motor = pynlm.NeuronType.Motor
internal = pynlm.NeuronType.Internal

# Developmental Stages
initial = pynlm.DevelopmentalStage.Initial
critical_period = pynlm.DevelopmentalStage.CriticalPeriod
maturation = pynlm.DevelopmentalStage.Maturation
adult = pynlm.DevelopmentalStage.Adult
aging = pynlm.DevelopmentalStage.Aging
```

---

## Next Steps for Advanced Users

When you're ready to go deeper:

1. **Read `HOW_TO_USE.md`** for comprehensive implementation details
2. **Explore `docs/ARCHITECTURE.md`** for deep technical understanding  
3. **Experiment with different configurations** using the advanced examples above
4. **Join the NLM community** for advanced support and collaboration
5. **Contribute to the project** with your own advanced use cases

The advanced examples above should give you a solid foundation for working with the full capabilities of NLM's integrated artificial brain system.
