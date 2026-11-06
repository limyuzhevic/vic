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
   - Contains neurons that process information
   - Learns through plasticity rules (STDP, Hebbian, etc.)
   - Can store memories and develop over time

2. **World** - The environment the brain lives in
   - Simple 2D grid world with objects (resources, hazards)
   - Provides sensory input (vision, touch, internal signals)
   - Rewards/punishes agent actions

3. **Agent** - The bridge connecting brain to world
   - Processes sensory input from world
   - Controls brain learning via neuromodulation
   - Decodes motor commands from brain activity

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

### Pattern 1: Complete Agent with Learning

```python
def run_agent_simulation(brain, world, agent, num_steps):
    """Run a complete agent simulation with sensory processing, learning, and action."""
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    for step in range(num_steps):
        # 1. Update world state
        world.update(0.1)
        
        # 2. Get what the agent sees (sensory input)
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain what it perceives
        agent.processSensoryInput(percept)
        
        # 4. Brain processes the input and learns
        brain.step(step)
        
        # 5. Get action from brain's motor neurons
        action = agent.decodeMotorCommand()
        
        # 6. Execute action in the world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Learn from consequences
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # 8. Develop (brain matures over time)
        agent.updateDevelopment(0.1)

# Use it like this:
# run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Silent Brain (Test Neuron Activity)

```python
def test_brain_activity(brain, num_steps):
    """Test how active the brain is without any external input."""
    brain.initialize()
    
    for step in range(num_steps):
        brain.step(step)
        
        # Monitor brain activity
        firing_rate = brain.getAverageFiringRate()
        if step % 100 == 0:
            print(f"Step {step}: Firing rate = {firing_rate:.2f} Hz")
    
    return brain.getAverageFiringRate()

# Use it:
# test_brain_activity(brain, 500)
```

### Pattern 3: World Navigation Agent

```python
def run_navigation_simulation(brain, world, agent, target_steps=1000):
    """Agent learns to navigate and collect resources in the world."""
    
    # Configure world
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    episode = 0
    for step in range(target_steps):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get and execute action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn from outcome
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if step % 200 == 0:
            print(f"Episode {episode}, Step {step}: "
                  f"Firing neurons: {brain.getFiringNeuronCount()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.2f}, "
                  f"Reward: {reward:.2f}")
    
    return episode

# Use it:
# run_navigation_simulation(brain, world, agent, 2000)
```

### Pattern 4: Agent Statistics Monitor

```python
def monitor_agent(brain, agent, steps=100):
    """Monitor agent statistics and print them at regular intervals."""
    
    stats = []
    for step in range(steps):
        brain.step(step)
        
        # Collect statistics
        firing_neurons = brain.getFiringNeuronCount()
        total_spikes = brain.getTotalSpikeCount()
        avg_firing_rate = brain.getAverageFiringRate()
        developmental_stage = brain.getDevelopmentalStage()
        curiosity_level = agent.getCuriosityLevel()
        novelty_level = agent.getNoveltyLevel()
        dopamine_level = agent.getNeuromodulationLevel()
        
        stats.append({
            'step': step,
            'firing_neurons': firing_neurons,
            'total_spikes': total_spikes,
            'avg_firing_rate': avg_firing_rate,
            'developmental_stage': developmental_stage,
            'curiosity_level': curiosity_level,
            'novelty_level': novelty_level,
            'dopamine_level': dopamine_level
        })
        
        # Print every 10 steps
        if step % 10 == 0:
            print(f"Step {step}: "
                  f"Firing: {firing_neurons}, "
                  f"Firing Rate: {avg_firing_rate:.2f}, "
                  f"Curiosity: {curiosity_level:.2f}")
    
    return stats

# Use it:
# stats = monitor_agent(brain, agent, 500)
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)
- The brain starts with approximately 1000 neurons by default
- Neurons are organized into regions for specialized processing

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!
- Initializes neuron dynamics and synaptic connections
- Sets up internal memory systems
- Prepares the brain for simulation

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron for potential firing
- Propagates spikes between connected neurons
- Updates synaptic weights based on learning rules (STDP, Hebbian)
- Integrates with memory systems
- Applies neuromodulation signals

### `world.update(time)`
Updates the virtual world by `time` seconds.
- Moves agent based on its velocity and orientation
- Checks for object collisions and interactions
- Updates agent's energy and internal state
- Generates sensory input for the agent

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.
- Injects current into sensory neurons based on world state
- Encodes the agent's perception into neural activity
- Enables the brain to understand its environment

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.
- Analyzes firing rates of motor neuron populations
- Selects the best action based on neural activity
- Returns an action for the world to execute

---

## Mini Projects

### Mini Tool 1: Brain Activity Monitor

```python
import pynlm

# Create and initialize a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run for 50 steps and monitor activity
for i in range(50):
    brain.step(i)
    
    # Print brain statistics every 10 steps
    if i % 10 == 0:
        print(f"Step {i}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")

print("\nBrain activity test complete!")
```

### Mini Tool 2: Simple World Explorer

```python
import pynlm

# Setup brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Agent explores the world
for step in range(100):
    world.update(0.1)
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Agent decides and acts
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("\nWorld exploration complete!")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

### Mini Tool 3: Learning Agent

```python
import pynlm

# Create agent with learning capabilities
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Enable learning subsystems
agent.enableRewardModulation(True)  # Learn from rewards
agent.enableCuriosity(True)        # Explore novel things
agent.enableDevelopment(True)      # Brain matures over time

# Run learning simulation
for step in range(200):
    # In a real scenario, you'd have a world here
    # For this demo, we'll just simulate some basic activity
    brain.step(step)
    
    # Print learning progress
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.2f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.2f}")
        print(f"  Dopamine level: {agent.getNeuromodulationLevel():.2f}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")

print("\nLearning simulation complete!")
```

---

## Quick Reference

| What you want | Code |
|--------------|-----|
| Create a brain | `brain = pynlm.createBrain(config)` |
| Configure brain | `config = pynlm.createDefaultConfig()` |
| Start the brain | `brain.initialize()` |
| Make brain think | `brain.step(step_number)` |
| Create world | `world = pynlm.createSimpleWorld()` |
| Configure world | `world.configure(width, height, visionWidth, visionHeight)` |
| Reset world | `world.reset()` |
| Create agent | `agent = pynlm.createAgentBrain(brain)` |
| Initialize agent | `agent.initialize(world)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| Get average firing | `brain.getAverageFiringRate()` |
| See world | `percept = world.getSensoryPercept()` |
| Process perception | `agent.processSensoryInput(percept)` |
| Get action | `action = agent.decodeMotorCommand()` |
| Apply action | `world.applyMotorCommand(action, time)` |
| Get learning stats | `agent.getCuriosityLevel()`, `agent.getNoveltyLevel()` |

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
