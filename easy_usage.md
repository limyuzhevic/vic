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

## Advanced Usage: Learning and Memory

### Example 3: Learning Brain with Memory

```python
import pynlm

# Create configuration for learning
config = pynlm.createDefaultConfig()
config.set_int("brain.neuron_count", 2000)
config.set_double("plasticity.stdp.learning_rate", 0.01)
config.set_bool("neuromod.curiosity.enable", True)
config.set_bool("neuromod.novelty.enable", True)
config.set_bool("memory.working_memory.capacity", 200)
config.set_bool("memory.episodic.max_episodes", 500)

# Create brain with learning capabilities
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with learning
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)  # Enable reward-based learning
agent.enableCuriosity(True)         # Enable curiosity-driven exploration
agent.enableDevelopment(True)       # Enable developmental changes

# Create environment
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run learning simulation
for step in range(200):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process and learn
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Act
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning if curiosity or reward is enabled
    if agent.isCuriosityEnabled() or agent.isRewardModulationEnabled():
        # Simulate reward based on world state
        reward = percept.getInternal()[0] if not percept.getInternal().empty() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    # Print progress
    if step % 40 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  Prediction error: {agent.getPredictionError():.3f}")
        print(f"  Developmental stage: {agent.getDevelopmentalStage()}")

print("Learning simulation complete!")
```

### Example 4: Complete Agent with All Features

```python
import pynlm

def run_advanced_agent_simulation(num_steps=500):
    """Run a complete agent simulation with all NLM features enabled."""
    
    # 1. Create configuration with all systems enabled
    config = pynlm.createDefaultConfig()
    
    # Brain configuration
    config.set_int("brain.neuron_count", 3000)
    config.set_int("brain.region_count", 2)
    config.set_double("brain.connection_probability", 0.15)
    
    # Learning configuration  
    config.set_bool("plasticity.stdp.enable", True)
    config.set_double("plasticity.stdp.learning_rate", 0.005)
    config.set_bool("plasticity.hebbian.enable", True)
    config.set_bool("plasticity.structural.enable", True)
    
    # Neuromodulation
    config.set_bool("neuromod.curiosity.enable", True)
    config.set_bool("neuromod.novelty.enable", True)
    config.set_double("neuromod.curiosity.threshold", 0.3)
    
    # Memory systems
    config.set_int("memory.working_memory.capacity", 300)
    config.set_int("memory.episodic.max_episodes", 1000)
    
    # Development
    config.set_double("development.initial_plasticity_rate", 1.0)
    config.set_double("development.critical_period_length", 500.0)
    
    # Simulation
    config.set_double("simulation.timestep", 0.001)
    config.set_int("simulation.max_steps", num_steps)
    
    # 2. Create and initialize all components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # Configure world
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable all advanced subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 3. Run the simulation loop
    print("Starting advanced agent simulation...")
    print(f"Configuration: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
    for step in range(num_steps):
        # Update world environment
        world.update(0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input through agent
        agent.processSensoryInput(percept)
        
        # Run brain simulation step
        brain.step(step)
        
        # Decode action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning and development if enabled
        if agent.isRewardModulationEnabled():
            # Calculate reward based on world state
            reward = 0.0
            if not percept.getInternal().empty():
                reward = percept.getInternal()[0];
            
            # Get predicted reward for comparison
            predicted_reward = 0.0
            if not percept.getInternal().empty():
                predicted_reward = percept.getInternal()[0] * 0.8; // Simple prediction
            
            agent.applyRewardModulation(reward, predicted_reward);
        
        if agent.isDevelopmentEnabled():
            agent.updateDevelopment(0.1);
        
        # Print detailed statistics every 50 steps
        if step % 50 == 0:
            print(f"\nStep {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount():6d}")
            print(f"  Total spikes: {brain.getTotalSpikeCount():6d}")
            print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():8.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():8.3f}")
            print(f"  Novelty:   {agent.getNoveltyLevel():8.3f}")
            print(f"  Prediction error: {agent.getPredictionError():8.3f}")
            print(f"  Dev stage: {agent.getDevelopmentalStage()}")
            print(f"  Memory episodes: {agent.getEpisodicMemory() ? agent.getEpisodicMemory()->getEpisodeCount() : 0}")
    
    print(f"\nSimulation completed!")
    print(f"Final statistics:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Final firing rate: {brain.getAverageFiringRate():.3f} Hz")
    print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Final developmental stage: {agent.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run the advanced simulation
if __name__ == "__main__":
    advanced_brain, advanced_agent, advanced_world = run_advanced_agent_simulation(500)
```

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

### Project 3: Complete Learning Agent

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

### Core Classes

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

### Advanced Features

| Feature | Code |
|---------|------|
| Enable learning | `agent.enableRewardModulation(True)` |
| Enable curiosity | `agent.enableCuriosity(True)` |
| Enable development | `agent.enableDevelopment(True)` |
| Get curiosity level | `agent.getCuriosityLevel()` |
| Get novelty level | `agent.getNoveltyLevel()` |
| Get prediction error | `agent.getPredictionError()` |
| Save brain state | `brain.save("checkpoint.bin")` |
| Load brain state | `brain.load("checkpoint.bin")` |

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

---

## Advanced Configuration Examples

### High-Performance Configuration

```python
config = pynlm.createDefaultConfig()
config.set_int("brain.neuron_count", 500)      # Fewer neurons for speed
config.set_int("brain.region_count", 2)        # Fewer regions
config.set_double("brain.connection_probability", 0.05)  # Fewer connections
config.set_bool("plasticity.structural.enable", False)  # Disable structural plasticity
config.set_int("memory.working_memory.capacity", 50)     # Smaller memory
```

### Research Configuration

```python
config = pynlm.createDefaultConfig()
config.set_int("brain.neuron_count", 5000)     # More neurons for research
config.set_int("brain.region_count", 4)       # More regions
config.set_double("brain.connection_probability", 0.2)    # More connections
config.set_double("plasticity.stdp.learning_rate", 0.0005) # Slower learning
config.set_bool("plasticity.structural.enable", True)     # Full structural plasticity
config.set_int("memory.working_memory.capacity", 500)     # Larger memory
config.set_int("memory.episodic.max_episodes", 10000)     # More memory episodes
config.set_bool("neuromod.curiosity.enable", True)        # Full neuromodulation
config.set_bool("neuromod.novelty.enable", True)
```

### Development Configuration

```python
config = pynlm.createDefaultConfig()
config.set_int("brain.neuron_count", 1000)
config.set_double("development.initial_plasticity_rate", 1.0)  # High initial plasticity
config.set_double("development.critical_period_length", 1000.0)  # Longer critical period
config.set_double("simulation.timestep", 0.001)  # Fine time resolution
```

---

## Configuration Templates

The NLM API includes several pre-configured templates for common use cases:

- `set_default_brain_config()` - Balanced, full-featured brain
- `set_performance_config()` - Optimized for speed
- `set_research_config()` - Optimized for research

These can be called directly on any `Config` object:

```python
config = pynlm.createDefaultConfig()
config.set_default_brain_config()  # Use default settings
# or
config.set_performance_config()   # Use performance settings
# or
config.set_research_config()      # Use research settings
```

This ensures you get sensible defaults for any application.