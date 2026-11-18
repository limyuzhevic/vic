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

## Advanced Examples

### Advanced Example 1: Learning with Environmental Interaction

```python
import pynlm

# Create a more sophisticated configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Create world with more resources and hazards
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Starting advanced learning simulation...")
print("Configuration:")
print(f"  - {brain.getTotalNeuronCount()} neurons")
print(f"  - {brain.getTotalSynapseCount()} synapses")
print(f"  - World size: {world.getWidth()}x{world.getHeight()}")

# Advanced simulation with learning curves
learning_data = []
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Calculate reward based on agent's performance
    energy = world.getAgentBody().energy
    reward = energy / 100.0
    predicted_reward = 0.0  # Could use prediction system
    agent.applyRewardModulation(reward, predicted_reward)
    
    # Log every 50 steps
    if step % 50 == 0:
        stats = pynlm.getBrainStats(brain)
        learning_data.append({
            'step': step,
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'energy': energy,
            'development_stage': str(brain.getDevelopmentalStage())
        })
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
              f"curiosity={agent.getCuriosityLevel():.3f}, "
              f"energy={energy:.1f}")

print("Advanced simulation complete!")
print(f"Peak firing neurons: {max(d['firing_neurons'] for d in learning_data)}")
print(f"Average curiosity: {sum(d['curiosity'] for d in learning_data) / len(learning_data):.3f}")
```

### Advanced Example 2: Multi-Agent Scenario

```python
import pynlm

# Setup multiple agents sharing the same brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
brain = pynlm.createBrain(config)
brain.initialize()

# Create a shared world
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, visionWidth=20, visionHeight=20)
world.reset()

# Create multiple agents (they share the brain but have different configurations)
agents = []
for i in range(3):
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agents.append(agent)

print("Starting multi-agent simulation...")

# Run multiple agents in parallel (staggered)
for episode in range(10):
    print(f"Episode {episode + 1}/10")
    
    # Reset world for new episode
    world.reset()
    
    # Each agent gets a turn
    for agent_idx, agent in enumerate(agents):
        print(f"  Agent {agent_idx + 1}:")
        
        for step in range(100):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Different rewards for different agents (could be based on position, behavior, etc.)
            reward = world.getAgentBody().energy / 100.0
            if agent_idx == 0:
                # Agent 1 prefers resources
                reward *= 1.5
            elif agent_idx == 1:
                # Agent 2 avoids hazards
                if "hazard" in str(world.getAgentBody().health).lower():
                    reward *= 0.5
            # Agent 3 has neutral preferences
            
            agent.applyRewardModulation(reward, 0.0)
            
            if step % 25 == 0:
                print(f"    Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                      f"curiosity={agent.getCuriosityLevel():.3f}")

print("Multi-agent simulation complete!")
```

### Advanced Example 3: Text-Based Learning (Phase 6 Feature)

```python
import pynlm

# Create brain with text capability
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2500)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=40, height=40, visionWidth=24, visionHeight=24)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

print("Starting text-based learning simulation...")

# Text words to be learned (could come from the environment)
words_to_learn = ["apple", "banana", "cherry", "date", "elderberry", "fig", "grape"]

# Simulation with text inputs
for episode in range(20):
    print(f"Episode {episode + 1}/20")
    
    # Reset and pick a random word
    world.reset()
    target_word = words_to_learn[episode % len(words_to_learn)]
    print(f"  Learning word: '{target_word}'")
    
    for step in range(200):
        world.update(0.1)
        
        # Process visual input
        percept = world.getSensoryPercept()
        
        # Simulate text input (in real usage, text would come from world)
        if step % 50 == 0:
            # Inject text pattern (Phase 6 feature)
            text = target_word if step % 100 != 0 else "wrong"
            print(f"    Processing text: '{text}'")
        
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Reward based on whether the word matches
        if step % 100 == 0:
            # Simple reward: correct word = positive, wrong = negative
            if "apple" in target_word:
                reward = 5.0  # Positive for 'apple'
            else:
                reward = -2.0  # Negative for other words
            agent.applyRewardModulation(reward, 0.0)
            print(f"    Reward: {reward:.1f}")

print("Text-based learning simulation complete!")
print(f"Text neurons: {agent.getInternalFiringRates()}")
```

### Advanced Example  bladml

```python
import pynlm

# Create a specialized configuration for cognitive research
config = pynlm.createDefaultConfig()

# Research-grade configuration
config.set("brain.neuron_count", 4000)
config.set("brain.synapse_density", 0.05)  # Sparse for research
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.001)
config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.00005)
config.set("plasticity.structural.pruning_rate", 0.000005)

# Neuromodulation for advanced research
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.scale", 2.0)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.sensitivity", 1.0)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=60, height=60, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all research features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Starting cognitive research simulation...")
print(f"Research brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
print(f"Configuration: High-density research setup with precise plasticity parameters")

# Research-grade simulation with detailed monitoring
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Research-level monitoring
    if step % 100 == 0:
        stats = pynlm.getBrainStats(brain)
        print(f"Step {step}:")
        for key, value in stats.items():
            print(f"  {key}: {value}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Prediction error: {agent.getPredictionError():.3f}")

print("Research simulation complete!")
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
