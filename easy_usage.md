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

---

## Advanced Examples (Building on Basic Knowledge)

Now that you understand the basics, let's look at more complex examples that utilize all the features.

### Example 1: Learning through Play

This example shows how an agent learns to explore and discover resources in its environment using curiosity and reward modulation.

```python
import pynlm
import time
import random

def run_learning_simulation():
    # Create brain with larger capacity for learning
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.dopamine.scale", 2.0)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.setMaxEnergy(1000.0)
    world.reset()
    
    # Place some resources in the world
    agent_start_x, agent_start_y = 15.0, 15.0
    world.setAgentStart(agent_start_x, agent_start_y)
    
    # Add resources randomly
    for _ in range(10):
        x = random.uniform(0, 30)
        y = random.uniform(0, 30)
        obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=5.0)
        world.addObject(obj)
    
    # Add hazards (optional)
    for _ in range(3):
        x = random.uniform(0, 30)
        y = random.uniform(0, 30)
        hazard = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=-2.0)
        world.addObject(hazard)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Starting learning simulation...")
    print("Features enabled: Reward modulation, Structural plasticity, Development, Curiosity")
    
    for step in range(1000):
        world.update(0.05)
        
        # Get what agent sees
        percept = world.getSensoryPercept()
        
        # Brain processes sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Brain decides action
        action = agent.decodeMotorCommand()
        
        # World applies action and returns result
        action_result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward with prediction error
        agent.applyRewardModulation(
            action_result.reward, 
            brain.getPredictionError()
        )
        
        # Update development
        agent.updateDevelopment(0.05)
        
        # Print stats every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Agent energy: {world.getAgentBody().energy:.1f}")
            print(f"  Agent age: {world.getAgentBody().age}")
    
    print("\nLearning simulation complete!")
    print(f"Final brain size: {brain.getTotalNeuronCount()} neurons")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Run the example
run_learning_simulation()
```

### Example 2: Memory System Demonstration

This example demonstrates how different memory systems work together in NLM.

```python
import pynlm

def demonstrate_memory_systems():
    """Demonstrate working memory, episodic memory, and associative memory."""
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.curiosity.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Create a memory episode
    print("Demonstrating memory systems...")
    
    for episode in range(3):
        print(f"\n--- Episode {episode + 1} ---")
        
        for step in range(50):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            agent.updateDevelopment(0.1)
            
            if step % 10 == 0:
                # Monitor memory state
                print(f"Step {step}: Firing {brain.getFiringNeuronCount()} neurons, "
                      f"Memory patterns active: {brain.getActiveNeuronCount()}")
                print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
                print(f"  Prediction error: {agent.getPredictionError():.3f}")
    
    # Demonstrate checkpointing
    print("\n--- Demonstrating Checkpointing ---")
    brain.save("memory_demo_checkpoint.bin")
    print(f"Saved brain state with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain has {brain.getTotalSynapseCount()} synapses")
    
    # Modify brain
    for _ in range(200):
        world.reset()
        agent.reset()
        for step in range(30):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
    
    # Load checkpoint
    brain.load("memory_demo_checkpoint.bin")
    print("Loaded brain state - demonstration complete")

# Run memory demonstration
demonstrate_memory_systems()
```

### Example 3: Neuromodulation Integration

This example shows how different neuromodulators work together to control behavior.

```python
import pynlm

def demonstrate_neuromodulation():
    """Demonstrate integration of neuromodulator systems."""
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1200)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.002)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(12.5, 12.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print("Neuromodulation Integration Demo")
    print("===============================")
    
    # Initial state
    print(f"Initial neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
    print(f"Initial curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"Initial novelty: {agent.getNoveltyLevel():.3f}")
    
    for step in range(300):
        world.update(0.08)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Monitor neuromodulation
        if step % 50 == 0:
            print(f"\nStep {step}:")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
            print(f"  Developmental Stage: {brain.getDevelopmentalStage()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        
        # Generate action
        action = agent.decodeMotorCommand()
        action_result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation
        agent.applyRewardModulation(action_result.reward, brain.getPredictionError())
        agent.updateDevelopment(0.08)
        
        # Add some interesting environmental changes to trigger neuromodulation
        if step == 50:
            print(f"\nEnvironmental change at step 50 - novel stimulus!")
            # This would trigger novelty detection
        elif step == 150:
            print(f"\nEnvironmental change at step 150 - reward opportunity!")
            # This would trigger reward-based learning
    
    print("\nNeuromodulation demonstration complete!")
    print(f"Final state: Neuromodulation={agent.getNeuromodulationLevel():.3f}, "
          f"Curiosity={agent.getCuriosityLevel():.3f}")

# Run neuromodulation demo
demonstrate_neuromodulation()
```

### Example 4: Simulation Control and Performance

This example demonstrates advanced simulation control features and performance monitoring.

```python
import pynlm
import time
import statistics

def advanced_simulation_control():
    """Advanced simulation with step-by-step control and performance monitoring."""
    
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2500)
    config.set("plasticity.structural.enable", True)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(20.0, 20.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Advanced Simulation Control Demo")
    print("================================")
    
    # Performance monitoring
    step_times = []
    firing_rates = []
    curiosity_levels = []
    
    # Phase 1: Exploration (curiosity-driven)
    print("\nPhase 1: Exploration Phase")
    print("-" * 30)
    
    for step in range(200):
        start_time = time.time()
        
        world.update(0.05)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.0, brain.getPredictionError())
        agent.updateDevelopment(0.05)
        
        step_time = time.time() - start_time
        step_times.append(step_time)
        firing_rates.append(brain.getAverageFiringRate())
        curiosity_levels.append(agent.getCuriosityLevel())
        
        if step % 40 == 0:
            print(f"Step {step}: Time={step_time*1000:.1f}ms, "
                  f"Firing={brain.getAverageFiringRate():.2f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    # Phase 2: Exploitation (reward-driven)
    print("\nPhase 2: Exploitation Phase")
    print("-" * 30)
    
    for step in range(200, 400):
        start_time = time.time()
        
        # Add rewards for good actions
        world.update(0.05)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        action_result = world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(action_result.reward, brain.getPredictionError())
        agent.updateDevelopment(0.05)
        
        step_time = time.time() - start_time
        step_times.append(step_time)
        firing_rates.append(brain.getAverageFiringRate())
        curiosity_levels.append(agent.getCuriosityLevel())
        
        if step % 40 == 0:
            print(f"Step {step}: Time={step_time*1000:.1f}ms, "
                  f"Firing={brain.getAverageFiringRate():.2f}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Reward={action_result.reward}")
    
    # Performance analysis
    print("\nPerformance Analysis")
    print("=" * 30)
    print(f"Average step time: {statistics.mean(step_times)*1000:.2f}ms")
    print(f"Min step time: {min(step_times)*1000:.2f}ms")
    print(f"Max step time: {max(step_times)*1000:.2f}ms")
    print(f"Average firing rate: {statistics.mean(firing_rates):.2f} Hz")
    print(f"Firing rate std: {statistics.stdev(firing_rates):.2f} Hz")
    print(f"Average curiosity: {statistics.mean(curiosity_levels):.3f}")
    
    # Demonstrate checkpoint at peak performance
    brain.save("performance_demo_checkpoint.bin")
    print(f"\nCheckpoint saved at peak performance: {brain.getTotalNeuronCount()} neurons")
    
    print("\nAdvanced simulation complete!")
    print("Simulation controlled and monitored successfully.")

# Run advanced simulation
advanced_simulation_control()
```

---

## Next Steps

When you're comfortable:
1. Read `ADVANCED_USAGE.md` for comprehensive advanced documentation
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
