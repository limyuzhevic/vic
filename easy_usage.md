# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

**Note:** NLM is in Phase 6: FINAL INTEGRATION. This means all the major systems (memory, prediction, cognition, neuromodulation, development) are now integrated and working together as a coherent artificial brain.

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

### Pattern 1: Run a Complete Agent Simulation

```python
def run_complete_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_complete_agent_simulation(1000)
```

### Pattern 2: Get Comprehensive Brain Stats

```python
# Just add these inside your loop:
print("=== Brain Statistics ===")
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
print("Memory traces:", brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else "N/A")
print("Episodic memory:", brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else "N/A")
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
```

### Pattern 3: Enable Advanced Brain Features

```python
# These are all optional and can be enabled at any time:
agent.enableRewardModulation(True)      # Learn from rewards and prediction errors
agent.enableStructuralPlasticity(True)  # Grow new connections and prune old ones
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new and novel things

# You can also disable features if needed:
# agent.enableRewardModulation(False)
# agent.enableStructuralPlasticity(False)
# agent.enableDevelopment(False)
# agent.enableCuriosity(False)
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)
- Returns: A brain instance ready for simulation

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!
- This sets up all the internal systems (memory, prediction, cognition, etc.)
- Returns: True if initialization succeeded

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron for spikes
- Sends signals between connected neurons
- Updates connections based on learning rules (STDP, Hebbian)
- Updates memory systems (working memory, episodic memory)
- Updates prediction system
- Updates cognitive systems (attention, planning, concept formation)
- Applies neuromodulation effects
- Applies developmental changes
- Returns: None

### `world.update(time)`
Updates the virtual world by `time` seconds.
- Advances the simulation clock
- Updates agent position and physics
- Generates new sensory input
- Returns: None

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, internal signals) to the brain.
- Injects current into sensory neurons based on input
- Stores input in working memory
- Updates prediction system
- Returns: None

### `agent.decodeMotorCommand()`
Reads the brain's motor neuron activity to decide what action to take.
- Uses action selection mechanisms (competing attentional processes)
- Returns: A motor command (MoveForward, TurnLeft, etc.)

### `world.applyMotorCommand(action, time)`
Executes an action in the world.
- Moves the agent
- Interacts with world objects
- Generates rewards and feedback
- Returns: Action result (reward, success, message)

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
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
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
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
```

### Project 3: Complete Agent with Learning

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

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run agent
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosities: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelties: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

print("Agent simulation complete!")
print(f"Final memory traces: {brain.getWorkingMemory().getActiveTraces()}")
print(f"Final episodes stored: {brain.getEpisodicMemory().getEpisodeCount()}")
```

---

## Quick Reference

| What you want | Code | Description |
|--------------|------|-------------|
| Create brain | `pynlm.createBrain(config)` | Create a brain with configuration |
| Create default config | `pynlm.createDefaultConfig()` | Create brain with sensible defaults |
| Create world | `pynlm.createSimpleWorld()` | Create simulation world |
| Create agent | `pynlm.createAgentBrain(brain)` | Create agent interface |
| Start brain | `brain.initialize()` | Initialize brain systems |
| Make brain think | `brain.step(step)` | Run one simulation step |
| Update world | `world.update(time)` | Advance world simulation |
| Get brain stats | `brain.getFiringNeuronCount()` | Get various brain statistics |
| See world | `world.getSensoryPercept()` | Get sensory input from world |
| Make action | `agent.decodeMotorCommand()` | Get motor command from brain |
| Apply action | `world.applyMotorCommand(action, time)` | Execute action in world |
| Enable learning | `agent.enableRewardModulation(True)` | Enable reward learning |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps (try 100+ steps)
- Check if there are any errors in the logs

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Ensure the agent has motor neurons configured

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if you're getting sensory input from the world

**"Memory systems not working"**
- Memory systems should work automatically if enabled
- Check if you're checking the right memory system methods
- Memory stats are available via `brain.getWorkingMemory()` and `brain.getEpisodicMemory()`

---

## Advanced Features

### Custom Configuration

```python
# Create custom configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)  # More neurons
config.set("region_count", 2)     # Multiple regions
config.set("connection_probability", 0.2)  # Higher connectivity

brain = pynlm.createBrain(config)
brain.initialize()
```

### World Configuration

```python
world = pynlm.createSimpleWorld()
world.configure(
    width=30,                    # Larger world
    height=30,
    visionWidth=16,              # Wider vision
    visionHeight=16
)
world.setMaxEnergy(200.0)         # Higher energy
world.setEnergyDecayRate(0.01)    # Slower energy decay
```

### Agent Configuration

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable specific subsystems
agent.enableRewardModulation(True)  # Reward learning
agent.enableCuriosity(True)         # Exploration
agent.enableDevelopment(True)       # Brain maturation
agent.enableStructuralPlasticity(True)  # Connection growth
```

---

## Next Steps

When you're comfortable:

1. **Read HOW_TO_USE.md** for more detailed documentation and examples
2. **Read docs/ARCHITECTURE.md** to understand how it all works
3. **Experiment with different configurations** and see what happens
4. **Try different combinations** of brain features
5. **Use checkpoints** to save and load brain states
6. **Look at the examples** in the experiments folder for more complex scenarios

**Important:** NLM is in Phase 6: FINAL INTEGRATION. This means all the major systems are now connected and working together. You can use all the features without worrying about missing components!

That's it! You're now ready to use NLM and create your own artificial brains with learning, memory, and development!
