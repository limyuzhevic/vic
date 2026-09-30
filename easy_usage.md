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

### Advanced Projects

#### Project 4: Advanced Memory Management

```python
import pynlm

# Setup with enhanced configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 3000)
config.set("region_count", 3)
config.set("replay_interval", 25)
config.set("consolidation_interval", 300)

brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=12, visionHeight=12)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run for extended period
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Check memory statistics
stats = pynlm.getMemoryStats(brain)
print(f"Working memory utilization: {stats.get('working_memory_utilization', 0):.2%}")
print(f"Episodic memory: {stats.get('episodic_memory_current_episodes', 0)} episodes stored")

# Trigger memory consolidation
pynlm.triggerMemoryConsolidation(brain, 0.7)
print("Memory consolidation completed")

# Trigger replay
replay_count = pynlm.triggerReplay(brain, 3)
print(f"Replayed {replay_count} episodes")
```

#### Project 5: Neuromodulation Control

```python
import pynlm

# Create brain with advanced configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 4000)
config.set("region_count", 4)

brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all neuromodulators
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Set developmental profile (accelerated, extended, or adult)
agent.setDevelopmentProfile("accelerated")  # Options: accelerated, extended, adult

# Run simulation with enhanced learning
for step in range(300):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Monitor neuromodulation levels
neuro_mod_level = agent.getNeuromodulationLevel()
print(f"Overall neuromodulation level: {neuro_mod_level:.3f}")
print(f"Dopamine level: {agent.getDopamineLevel():.3f}")
print(f"Acetylcholine level: {agent.getAcetylcholineLevel():.3f}")
print(f"Norepinephrine level: {agent.getNorepinephrineLevel():.3f}")
print(f"Serotonin level: {agent.getSerotoninLevel():.3f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
```

#### Project 6: Advanced Simulation with Custom Parameters

```python
import pynlm

# Setup with advanced configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("region_count", 2)
config.set("connection_probability", 0.2f)

brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Custom parameters for enhanced learning
custom_params = {
    "dopamine_boost": 0.3,
    "acetylcholine_boost": 0.2,
    "learning_rate": 0.05,
    "exploration_bonus": 0.15
}

# Run advanced simulation with custom parameters
metrics = pynlm.runAdvancedSimulation(brain, agent, world, 250, custom_params)
total_reward, success_rate, avg_firing_rate = metrics

print(f"Simulation Results:")
print(f"  Average Reward per Step: {total_reward:.3f}")
print(f"  Success Rate: {success_rate:.2%}")
print(f"  Average Firing Rate: {avg_firing_rate:.3f}")

# Get performance metrics
performance = pynlm.getPerformanceMetrics(brain)
print(f"\nPerformance Metrics:")
for key, value in performance.items():
    print(f"  {key}: {value:.3f}")
```

#### Project 7: Agent Behavior Customization

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

# Set different behavior profiles based on experiment goals
experiment_goals = ["exploratory", "exploitative", "balanced"]

for goal in experiment_goals:
    print(f"\n=== Testing {goal} agent ===")
    
    # Reset for new experiment
    agent.reset()
    agent.setAgentBehaviorProfile(goal)
    
    # Run short simulation
    for step in range(50):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check behavior metrics
    curiosity = agent.getCuriosityLevel()
    reward_learning = "enabled" if agent.isRewardModulationEnabled() else "disabled"
    development = "enabled" if agent.isDevelopmentEnabled() else "disabled"
    
    print(f"  Curiosity: {curiosity:.3f}")
    print(f"  Reward learning: {reward_learning}")
    print(f"  Development: {development}")
```

#### Project 8: Debugging and Monitoring

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run with debugging at specific steps
for step in range(100):
    if step % 25 == 0:
        debug_info = pynlm.debugSimulationStep(brain, step)
        print(f"\n--- Debug Info at Step {step} ---")
        for key, value in debug_info.items():
            if isinstance(value, (int, float)):
                print(f"  {key}: {value:.3f}")
            else:
                print(f"  {key}: {value}")
    
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("\n=== Final Status ===")
final_debug = pynlm.debugSimulationStep(brain, 99)
print(f"Total steps completed: {final_debug.get('step', 0)}")
print(f"Final firing rate: {final_debug.get('firing_rate', 0):.3f}")
print(f"Total spikes: {final_debug.get('spike_count', 0)}")
print(f"Active neurons: {final_debug.get('active_neurons', 0)}")
```
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
