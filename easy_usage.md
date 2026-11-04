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

### Example 3: Advanced Learning with Development

```python
import pynlm
import time

def run_learning_simulation(num_steps=2000):
    """Complete learning simulation with all features enabled."""
    
    # 1. Create brain with learning configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500)
    config.set("plasticity_learning_rate", 0.01)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world with resources and hazards
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Add some resources and hazards
    world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0))
    world.addObject(pynlm.WorldObject(15.0, 15.0, pynlm.WorldObjectType.Hazard, 0.0, 2.0))
    
    # 3. Create agent with all learning enabled
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
    # 4. Run simulation with monitoring
    print("Starting learning simulation...")
    start_time = time.time()
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward from world
        reward = 0.0
        for obj in world.getAgentBody().getNearbyObjects(1.0):
            if obj.type == pynlm.WorldObjectType.Resource:
                reward += obj.value
            elif obj.type == pynlm.WorldObjectType.Hazard:
                reward -= 1.0
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {agent.getDevelopmentalStage()}")
            print(f"  Reward: {reward:.2f}")
    
    elapsed = time.time() - start_time
    print(f"\nSimulation complete in {elapsed:.2f} seconds!")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"Final development stage: {agent.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_learning_simulation(1000)
```

---

## Common Patterns

### Pattern 1: Run a Complete Agent Simulation

```python
def run_agent_simulation(world_width=15, world_height=15, steps=500):
    """Complete agent simulation with brain and world."""
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=world_width, height=world_height, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop
    for step in range(steps):
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
        if percept.getInternal() and not percept.getInternal().empty():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    return brain, agent, world

# Use it like this:
brain, agent, world = run_agent_simulation(world_width=10, world_height=10, steps=1000)
```

### Pattern 2: Quick Testing and Benchmarking

```python
def quick_brain_test(neuron_count=100, steps=50):
    """Quick test of brain functionality."""
    import time
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", neuron_count)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
    
    elapsed = time.time() - start_time
    
    print(f"Quick test completed:")
    print(f"  Neurons: {neuron_count}")
    print(f"  Steps: {steps}")
    print(f"  Time: {elapsed:.3f}s")
    print(f"  Average time per step: {elapsed/steps*1000:.2f}ms")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    return brain

# Run quick test
brain = quick_brain_test(neuron_count=200, steps=100)
```

### Pattern 3: Multi-Agent Environment

```python
def multi_agent_environment():
    """Create an environment with multiple agents."""
    import random
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 200)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Add resources
    for _ in range(5):
        x = random.uniform(2, world.getWidth()-2)
        y = random.uniform(2, world.getHeight()-2)
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=random.uniform(5.0, 15.0)))
    
    # Add hazards
    for _ in range(2):
        x = random.uniform(2, world.getWidth()-2)
        y = random.uniform(2, world.getHeight()-2)
        world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, 0.0, 1.5))
    
    # Create multiple agents
    agents = []
    for i in range(3):
        brain = pynlm.createBrain(config)
        brain.initialize()
        agent = pynlm.createAgentBrain(brain)
        
        # Set different starting positions
        start_x = random.uniform(5, world.getWidth()-5)
        start_y = random.uniform(5, world.getHeight()-5)
        world.setAgentStart(start_x, start_y)
        
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agents.append((brain, agent))
    
    # Run simulation
    for step in range(300):
        world.update(0.1)
        
        # Each agent acts in sequence
        for brain, agent in agents:
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 50 == 0:
            print(f"Step {step} completed")
    
    return world, agents

# Run multi-agent simulation
world, agents = multi_agent_environment()
```

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

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that the world is providing sensory input

### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Ensure the agent's motor system is working

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that the world has objects to interact with

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more detailed technical information
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Check out `docs/SCIENCE.md` for scientific background

That's it! You're now ready to use NLM and create your own artificial brains!
