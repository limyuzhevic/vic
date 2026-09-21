# NLM (Neural Learning Machine) - Beginner Tutorial

## Welcome to NLM!

NLM is a brain-inspired spiking neural network simulator that lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## What You'll Learn

In this tutorial, you'll:

1. Create a simple virtual brain
2. Make it think and learn
3. Run it in a simple world
4. See it make decisions

---

## Part 1: Your First Brain

Let's start with the simplest possible example - just a brain that thinks:

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
print("It just thought for 1 step!")
```

**What happened?**
- You created a brain with default settings (about 1000 neurons)
- You initialized it (started up the neural systems)
- You ran one step (the brain processed information)
- You checked how many neurons the brain has

---

## Part 2: A Brain That Learns

Now let's add learning to the brain:

```python
import pynlm

# Create brain with learning enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable reward-based learning
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)

# Run for many steps
for step in range(100):
    brain.step(step)
    
    # Print stats every 20 steps
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        print(f"Development stage: {brain.getDevelopmentalStage()}")

print("Learning complete!")
```

**What's new?**
- You created a brain and an agent interface
- You enabled reward-based learning (the brain can learn from rewards)
- You ran 100 steps of thinking and learning
- You tracked the brain's progress

---

## Part 3: Brain in a World

Now let's make your brain interact with a world:

```python
import pynlm

# === SETUP ===
# 1. Create the brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# 2. Create the world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# 3. Create the agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# === SIMULATION LOOP ===
for step in range(200):
    # 1. Update world (time passes)
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
    
    # 7. Print progress occasionally
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  - Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  - Curiosity level: {agent.getCuriosityLevel():.2f}")
        print(f"  - Agent position: ({agent.getBrain().getAgentBody().x:.1f}, {agent.getBrain().getAgentBody().y:.1f})")

print("Simulation finished!")
```

**The 6 Steps Explained:**

| Step | What Happens | Why It Matters |
|------|-------------|----------------|
| 1. Update world | Time passes, things change | Simulates real-time environment |
| 2. Get percept | Agent senses the world | Brain needs input to work |
| 3. Process input | Brain receives sensory data | Neural activity starts |
| 4. Brain thinks | Neurons process and learn | Core computation happens |
| 5. Get action | Brain decides what to do | Output from neural activity |
| 6. Apply action | World changes based on action | Real-world consequences |

---

## Part 4: Building a Complete Agent

Here's a complete working agent that explores its world:

```python
import pynlm

def run_explorer_simulation(num_steps=500):
    """Run a complete agent simulation with brain and world."""
    
    print("=== NLM Agent Explorer ===")
    print("Starting simulation...")
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize  
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(15.0, 15.0)  # Start in middle
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable learning systems
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
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
        
        # Apply reward modulation
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"\nStep {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Dev stage: {brain.getDevelopmentalStage()}")
            print(f"  Agent position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
            print(f"  Energy: {world.getAgentBody().energy:.1f}")
    
    print(f"\n=== Simulation Complete ===")
    print(f"Ran {num_steps} steps")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    return brain, agent, world

# Run the simulation
brain, agent, world = run_explorer_simulation(500)
```

---

## Common Patterns

### Pattern 1: Run a Simulation (Copy and paste this into your code!)

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
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your simulation loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional but useful:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

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

**"ImportError: No module named 'pynlm'"**
- Run: `pip install pynlm`
- Or: `pip install --force-reinstall .` if using local source

**"The brain crashes"**
- Always call `initialize()` before `step()`
- Make sure to call `reset()` before re-running simulations
- Check for memory issues with large brain sizes

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Create config | `pynlm.createDefaultConfig()` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details**
   - Covers all the technical details
   - Advanced usage patterns
   - Configuration options

2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
   - Deep dive into the neural architecture
   - Understanding different brain systems

3. **Experiment with different configurations**
   - Try different brain sizes
   - Adjust learning rates
   - Enable/disable different features

4. **Check out the Phase 6 demo**
   - Run `./nlm_phase6_demo` (if built from C++)
   - See all systems working together

You're now ready to use NLM! Create your first brain and let it learn.
