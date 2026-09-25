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

## New High-Level Convenience Functions

The updated NLM Python bindings now include powerful high-level functions that make it easier to:

- Create complete agent setups with a single function call
- Run simulations with built-in learning and statistics
- Create worlds with obstacles and resources
- Get detailed simulation statistics

### Example 3: Using High-Level Convenience Functions

```python
import pynlm

# Create a complete agent with learning features
brain, world, agent = pynlm.createLearningAgent(width=20, height=20)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation with statistics
stats = pynlm.runSimulationWithStats(brain, world, agent, 500)
print(stats.toString())

# Create world with obstacles and resources
world = pynlm.createWorldWithObstacles(
    width=30, height=30,
    obstacles=[(5, 5), (10, 10), (15, 15)]
)

# Use simulation runner for progress tracking
runner = pynlm.createSimulationRunner(width=25, height=25)
runner.runWithProgress(1000, progressCallback=lambda progress: print(f"Progress: {progress * 100:.0f}%"))

# Get simulation statistics
brain = runner.getBrain()
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Example 4: Advanced Agent Control

```python
import pynlm

# Create agent with custom settings
brain, world, agent = pynlm.createLearningAgent(
    width=40, height=40,
    enableStructuralPlasticity=True,
    enableNovelty=True,
    enableDevelopment=True
)

# Run simple agent controller
controller = pynlm.createSimpleAgentController(width=40, height=40)
controller.run()

print(f"Simulation results:")
print(f"  Average firing rate: {controller.getAverageFiringRate():.2f}")
print(f"  Total spikes: {controller.getTotalSpikeCount()}")
print(f"  Region count: {controller.getRegionCount()}")
```

### Example 5: Training and Saving

```python
import pynlm

# Train a brain and save it
brain = pynlm.trainAndSaveBrain(
    filepath="trained_brain.pkl",
    width=20, height=20,
    trainingSteps=2000
)

# Load the trained brain
loaded_brain = pynlm.loadTrainedBrain("trained_brain.pkl")
loaded_brain.initialize()

# Run the trained brain
for step in range(100):
    loaded_brain.step(step)
```

### Example 6: Exploration Simulation

```python
import pynlm

# Run exploration simulation
brain, world, agent = pynlm.runExplorationSimulation(
    worldWidth=50, worldHeight=50,
    steps=1000,
    enableCuriosity=True
)

print("Exploration complete!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Example 7: Debug and Diagnostic Tools

```python
import pynlm

# Create a demo environment
demo_brain, demo_world, demo_agent = pynlm.createDemoEnvironment()

# Print detailed statistics
pynlm.printBrainStats(demo_brain)

# Run a quick simulation
demo_agent.enableRewardModulation(True)
for step in range(50):
    demo_world.update(0.1)
    demo_agent.processSensoryInput(demo_world.getSensoryPercept())
    demo_brain.step(step)
    action = demo_agent.decodeMotorCommand()
    demo_world.applyMotorCommand(action, demo_world.getSimulationTime())

print("Demo simulation complete!")
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

## New High-Level Functions Reference

### Factory Functions

| Function | Description |
|----------|-------------|
| `pynlm.createLearningAgent()` | Create agent with learning features |
| `pynlm.createSimulationRunner()` | Create simulation runner with progress tracking |
| `pynlm.createWorldWithObstacles()` | Create world with obstacles |
| `pynlm.createWorldWithResources()` | Create world with resources |
| `pynlm.createSimpleAgentController()` | Create simple agent controller |

### Simulation Utilities

| Function | Description |
|----------|-------------|
| `pynlm.runSimulation()` | Run complete simulation loop |
| `pynlm.runSimulationWithStats()` | Run simulation and collect statistics |
| `pynlm.runExplorationSimulation()` | Run exploration with curiosity |

### Training and Checkpointing

| Function | Description |
|----------|-------------|
| `pynlm.trainAndSaveBrain()` | Train and save brain to file |
| `pynlm.loadTrainedBrain()` | Load trained brain from file |

### Debug and Diagnostics

| Function | Description |
|----------|-------------|
| `pynlm.printBrainStats()` | Print detailed brain statistics |
| `pynlm.createDemoEnvironment()` | Create demo environment |
| `pynlm.validateAgentSetup()` | Validate agent setup |

### Information

| Function | Description |
|----------|-------------|
| `pynlm.getVersion()` | Get version information |
| `pynlm.getFeatures()` | Get list of features |
| `pynlm.getExamplePatterns()` | Get example patterns |

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM with all its new high-level convenience functions!
