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

## Advanced Usage for Power Users

NLM provides advanced features for researchers and developers who want to create sophisticated neural simulations.

### Advanced Configuration Management

```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Set advanced parameters
config.set("neuron_count", 2000, nlm.ConfigSource.Runtime)  # More neurons
config.set("region_count", 4, nlm.ConfigSource.Runtime)     # More regions
config.set("connection_probability", 0.25, nlm.ConfigSource.Runtime)  # Higher connectivity
config.set("plasticity_learning_rate", 0.05, nlm.ConfigSource.Runtime)  # Faster learning

# Save configuration
config.saveToFile("advanced_config.json")

# Load configuration
generated_config = nlm.createDefaultConfig()
generated_config.loadFromFile("advanced_config.json")
```

### Advanced Brain Configuration

```python
import pynlm

# Create brain with advanced configuration
config = nlm.createDefaultConfig()

# Set NLM-specific parameters
config.set("random_seed", 42, nlm.ConfigSource.Runtime)                    # Reproducible results
config.set("simulation_timestep", 0.001, nlm.ConfigSource.Runtime)        # Higher precision
config.set("neuron_count", 2000, nlm.ConfigSource.Runtime)                # Larger network
config.set("region_count", 4, nlm.ConfigSource.Runtime)                   # Multiple regions
config.set("connection_probability", 0.25, nlm.ConfigSource.Runtime)     # Higher connectivity

# Create and initialize brain
brain = nlm.createBrain(config)
brain.initialize()

print("Advanced brain created with:", brain.getTotalNeuronCount(), "neurons")
print("Brain has:", brain.getTotalSynapseCount(), "synapses")
print("Brain regions:", brain.getRegionCount())
```

### Advanced Plasticity Configuration

```python
import pynlm

# Create brain
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Access and configure plasticity systems
stdp = brain.getSTDP()
hebbian = brain.getHebbian()

# Configure STDP parameters (advanced)
stdp.configure(
    ltpWeight=0.02,     # Stronger potentiation
    ltdWeight=0.015,    # Stronger depression
    tau=30.0           # Longer time window
)

# Configure Hebbian parameters
hebbian.setLearningRate(0.05)    # Faster Hebbian learning
hebbian.setMaxWeight(2.0)        # Allow stronger weights

# Enable advanced plasticity combinations
agent = nlm.createAgentBrain(brain)
agent.enableRewardModulation(True)    # Reward-based learning
agent.enableStructuralPlasticity(True) # Synapse growth/decay
agent.enableDevelopment(True)        # Developmental stages
agent.enableCuriosity(True)           # Exploration behavior

print("Advanced plasticity configured with:")
print("  STDP LTP:", stdp.getLTPWeight())
print("  STDP LTD:", stdp.getLTDWeight())
print("  Hebbian rate:", hebbian.getLearningRate())
```

### Advanced Learning Experiment

```python
import pynlm

# Create brain and agent
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)

# Set up advanced world
world.configure(width=30, height=30, visionWidth=16, visionHeight=1)
world.reset()

# Create advanced agent with learning
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run complex learning task
N_STEPS = 500
for step in range(N_STEPS):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with agent
    agent.processSensoryInput(percept)
    
    # Brain thinking
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action and get reward
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Reward-based learning
    if result.success:
        agent.applyRewardModulation(result.reward, 0.5)  # Moderate predicted reward
    else:
        agent.applyRewardModulation(-1.0, 0.8)  # Negative feedback
    
    # Update development
    agent.updateDevelopment(0.1)

print("Learning experiment completed!")
print("Final stats:")
print("  Total spikes:", brain.getTotalSpikeCount())
print("  Neurons firing:", brain.getFiringNeuronCount())
print("  Average firing rate:", brain.getAverageFiringRate())
print("  Reward modulation level:", agent.getNeuromodulationLevel())
```

### Memory and Cognition Systems

```python
import pnlmbrain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Access advanced memory systems
workingMemory = brain.getWorkingMemory()
episodicMemory = brain.getEpisodicMemory()
associativeMemory = brain.getAssociativeMemory()

# Access advanced cognition systems
planner = brain.getPlanner()
conceptFormation = brain.getConceptFormation()
attention = brain.getAttention()

# Working memory example
if workingMemory:
    # Store a pattern
    pattern = [0.1, 0.5, 0.9, 0.2]
    workingMemory.storePattern(pattern)
    
    # Retrieve a pattern
    retrieved = workingMemory.getPattern(0)
    print("Working memory retrieved pattern:", retrieved)

# Episodic memory example
if episodicMemory:
    # Store experience
    episodicMemory.storeExperience(
        perception=[0.3, 0.7, 0.5],
        reward=1.0
    )
    
    # Recall experience
    recalled = episodicMemory.recallExperience(0)
    print("Episodic memory recalled:", recalled)

# Concept formation example
if conceptFormation:
    # Form concept from patterns
    patterns = [
        [0.1, 0.2, 0.3],
        [0.15, 0.25, 0.35],
        [0.05, 0.15, 0.25]
    ]
    concept = conceptFormation.formConcept(patterns)
    print("Concept formed:", concept)
```

### Neuromodulation and Development

```python
import pynlm

# Create brain and agent
brain = nlm.createBrain(nlm.createDefaultConfig())
world = nlm.createSimpleWorld()
agent = nlm.createAgentBrain(brain)

agent.initialize(world)

# Configure neuromodulation
print("Initial neuromodulation levels:")
print("  Dopamine:", agent.getNeuromodulationLevel())
print("  Curiosity:", agent.getCuriosityLevel())
print("  Novelty:", agent.getNoveltyLevel())
print("  Prediction Error:", agent.getPredictionError())

# Simulate development over time
for day in range(10):
    # Update development
    agent.updateDevelopment(1.0)  # 1 day at a time
    
    print(f"\nDay {day + 1}:")
    print("  Developmental stage:", agent.getDevelopmentalStage())
    print("  Neuromodulation:", agent.getNeuromodulationLevel())
    print("  Curiosity:", agent.getCuriosityLevel())
    print("  Plasticity rate:", brain.getConfig().getOr<double>("development_synaptogenesis_rate", 0.0))
```

### Checkpointing and Persistence

```python
import pynlm
import os

# Create and simulate brain
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Run some simulation
for step in range(100):
    brain.step(step)

# Save brain state (checkpointing)
brain.save("checkpoint_state.bin")
print("Brain state saved to checkpoint_state.bin")

# Simulate further
for step in range(100, 200):
    brain.step(step)

# Save progress check point
brain.save("checkpoint_state_200.bin")
print("Progress checkpoint saved to checkpoint_state_200.bin")

# Load from checkpoint
if os.path.exists("checkpoint_state_200.bin"):
    newBrain = nlm.createBrain(nlm.createDefaultConfig())
    newBrain.load("checkpoint_state_200.bin")
    print("Brain state loaded from checkpoint")
    print("Loaded brain has:", newBrain.getTotalSpikeCount(), "spikes")

# Cleanup
if os.path.exists("checkpoint_state.bin"):
    os.remove("checkpoint_state.bin")
if os.path.exists("checkpoint_state_200.bin"):
    os.remove("checkpoint_state_200.bin")
```

### Visualization and Analysis

```python
import pynlm

# Create brain with visualization capabilities
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Get brain statistics
print("=== Brain Statistics ===")
print("Total neurons:", brain.getTotalNeuronCount())
print("Total synapses:", brain.getTotalSynapseCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Active neurons:", brain.getActiveNeuronCount())
print("Firing neurons:", brain.getFiringNeuronCount())
print("Average firing rate:", brain.getAverageFiringRate())
print("Excitatory/Inhibitory ratio:", brain.getExcitationInhibitionRatio())

# Get developmental information
print("\n=== Development Information ===")
print("Current stage:", brain.getDevelopmentalStage())
print("Simulation time:", brain.getConfig().getOr<double>("simulation_timestep", 0.001) + "s")

# Get configuration
print("\n=== Configuration ===")
print("Neuron count:", brain.getConfig().getOr<int64_t>("neuron_count", 500))
print("Region count:", brain.getConfig().getOr<int64_t>("region_count", 1))
print("Connection probability:", brain.getConfig().getOr<float>("connection_probability", 0.15))
```

### Advanced Testing and Benchmarking

```python
import pynlm
import time

# Create test brain
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Run benchmark
steps = 1000
startTime = time.time()

for step in range(steps):
    brain.step(step)

endTime = time.time()

duration = endTime - startTime
stepsPerSecond = steps / duration
spikesPerSecond = brain.getTotalSpikeCount() / duration

print("=== Benchmark Results ===")
print("Steps:", steps)
print("Duration:", duration, "seconds")
print("Steps per second:", stepsPerSecond)
print("Total spikes:", brain.getTotalSpikeCount())
print("Spikes per second:", spikesPerSecond)
print("Average firing rate:", brain.getAverageFiringRate(), "Hz")

# Test plasticity
print("\n=== Plasticity Test ===")
stdp = brain.getSTDP()
hebbian = brain.getHebbian()

# Configure for testing
stdp.configure(0.01, 0.012, 20.0)
hebbian.setLearningRate(0.01)

# Run plasticity test
for step in range(200):
    brain.step(step)

print("Plasticity test completed:")
print("STDP LTP:", stdp.getLTPWeight())
print("STDP LTD:", stdp.getLTDWeight())
print("Hebbian learning rate:", hebbian.getLearningRate())
```

### Multi-Agent System

```python
import pynlm

# Create multiple brains and agents
brain1 = nlm.createBrain(nlm.createDefaultConfig())
brain2 = nlm.createBrain(nlm.createDefaultConfig())

brain1.initialize()
brain2.initialize()

world = nlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
world.reset()

agent1 = nlm.createAgentBrain(brain1)
agent2 = nlm.createAgentBrain(brain2)

agent1.initialize(world)
agent2.initialize(world)

# Enable social learning for agent2 (observe other agent)
agent2.enableSocialLearning(True)

print("=== Multi-Agent System ===")
print("Agent 1 stats:", agent1.getSensoryInputSize(), agent1.getMotorOutputSize())
print("Agent 2 stats:", agent2.getSensoryInputSize(), agent2.getMotorOutputSize())
print("Social learning enabled:", agent2.isSocialLearningEnabled())

# Simple multi-agent test
for step in range(50):
    world.update(0.1)
    
    # Agent 1 acts
    percept1 = world.getSensoryPercept()
    agent1.processSensoryInput(percept1)
    brain1.step(step)
    action1 = agent1.decodeMotorCommand()
    world.applyMotorCommand(action1, world.getSimulationTime())
    
    # Update for agent 2 (social learning)
    agent2.observeOtherAgent(
        action1,           # Observed action
        world.getSensoryPercept(),  # Pre-percept
        percept1          # Post-percept
    )
```

### Error Handling and Validation

```python
import pynlm

# Create configuration
config = nlm.createDefaultConfig()

# Test configuration validation
print("=== Configuration Validation ===")

# Valid configuration
config.set("valid_key", "valid_value", nlm.ConfigSource.Runtime)
if config.has("valid_key"):
    print("✓ Valid key exists:", config.getOr<std::string>("valid_key", ""))

# Test missing key
if config.has("nonexistent_key"):
    print("✗ Nonexistent key should not exist")
else:
    print("✓ Nonexistent key correctly missing")

# Test data type validation
config.set("int_key", 42, nlm.ConfigSource.Runtime)
intVal = config.get<int>("int_key")
if (intVal.has_value()) {
    std::cout << "✓ Integer value:" << *intVal << std::endl;
}

# Test error handling
std::string errorMsg = config.getError();
if (errorMsg.empty()) {
    std::cout << "✓ No errors" << std::endl;
} else {
    std::cout << "✗ Error:" << errorMsg << std::endl;
}

config.clearError();
if (config.getError().empty()) {
    std::cout << "✓ Error cleared successfully" << std::endl;
}
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
