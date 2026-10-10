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

### Example 3: Using Configuration to Customize Your Brain

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Customize brain parameters
config.setInt("brain.neuron_count", 500)      # Fewer neurons
config.setDouble("brain.synapse_density", 0.05)  # Less dense connections
config.setDouble("brain.v_thresh", -55.0)     # Lower threshold
config.set("plasticity.stdp.learning_rate", 0.0005)  # Slower learning

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Test it out
for i in range(50):
    brain.step(i)
    if i % 10 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")

print("Customized brain simulation complete!")
```

### Example 4: Advanced Agent with Learning

```python
import pynlm
import time

def run_advanced_agent_simulation(num_steps=200):
    """Run a complete agent simulation with advanced learning features."""
    
    # 1. Create configuration with advanced settings
    config = pynlm.createDefaultConfig()
    config.setInt("brain.neuron_count", 2000)     # More neurons
    config.setDouble("brain.synapse_density", 0.2)   # More connections
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("neuromod.dopamine.scale", 1.5)      # Higher dopamine influence
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable advanced subsystems
    agent.enableLearning(True)                    # Enable all learning
    agent.enableCuriosity(True)                   # Explore new things
    
    # 7. Run simulation loop
    start_time = time.time()
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
        action = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress periodically
        if step % 50 == 0:
            status = agent.getStatus()
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Learning enabled: {agent.isLearning()}")
            print(f"  Exploration score: {agent.getExplorationScore():.3f}")
            print(f"  Developmental stage: {status['development_stage']}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print()
    
    end_time = time.time()
    print(f"Simulation completed in {end_time - start_time:.2f} seconds!")
    return brain, world, agent

# Run the advanced simulation
brain, world, agent = run_advanced_agent_simulation(300)
```

### Example 5: Configuration File I/O

```python
import pynlm
import os

# Create and save a configuration
config = pynlm.createDefaultConfig()
config.setInt("brain.neuron_count", 1000)
config.setDouble("brain.synapse_density", 0.1)
config.set("plasticity.stdp.learning_rate", 0.001)

# Save configuration to file
config.saveToFile("my_config.txt")
print("Configuration saved to my_config.txt")

# Load configuration from file
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.txt")

# Verify it loaded correctly
print(f"Loaded configuration keys: {config2.getKeys()}")
print(f"Neuron count: {config2.getIntOr('brain.neuron_count', 100)}")
print(f"Synapse density: {config2.getDoubleOr('brain.synapse_density', 0.1)}")

# Clean up
os.remove("my_config.txt")
```

### Example 6: Batch Processing with Multiple Brains

```python
import pynlm
import time

def create_brain_from_config(config_file=None):
    """Create a brain from configuration file or default."""
    config = pynlm.createDefaultConfig()
    
    if config_file and os.path.exists(config_file):
        config.loadFromFile(config_file)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain

def run_brain_batch(config_files=None, steps_per_brain=100):
    """Run multiple brains in sequence."""
    if config_files is None:
        config_files = []  # Use default config
    
    brains = []
    for i, config_file in enumerate(config_files):
        print(f"Creating brain {i+1}...")
        brain = create_brain_from_config(config_file)
        brains.append(brain)
    
    print(f"Running {len(brains)} brains for {steps_per_brain} steps each...")
    
    for i, brain in enumerate(brains):
        print(f"  Brain {i+1}: ", end="")
        start_time = time.time()
        
        for step in range(steps_per_brain):
            brain.step(step)
        
        end_time = time.time()
        print(f"Completed {steps_per_brain} steps in {end_time - start_time:.2f}s")
        print(f"  Final spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print()
    
    return brains

# Run batch processing
brains = run_brain_batch(steps_per_brain=200)
print(f"Successfully processed {len(brains)} brains!")
```

### Example 7: Complex Agent Behavior with Exploration

```python
import pynlm
import random

def create_exploratory_agent(seed=None):
    """Create an agent with strong exploration tendencies."""
    if seed is not None:
        random.seed(seed)
    
    config = pynlm.createDefaultConfig()
    
    # Configure for exploration
    config.setInt("brain.neuron_count", 1500)
    config.setDouble("brain.synapse_density", 0.3)  # More connections
    config.set("plasticity.stdp.learning_rate", 0.002)  # Faster learning
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enableLearning(True)
    agent.enableCuriosity(True)
    
    return brain, agent

def run_exploration_simulation(world, agent, num_steps=400):
    """Run simulation with exploration focus."""
    # Set up world with interesting features
    world.configure(width=40, height=40, visionWidth=16, visionHeight=16)
    world.reset()
    world.setAgentStart(20.0, 20.0)
    
    agent.initialize(world)
    
    exploration_stats = {
        'high_curiosity_steps': 0,
        'novel_actions': set(),
        'total_reward': 0.0
    }
    
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        
        # Check if this is a novel action (for statistics)
        action_name = agent.getActionName(action)
        if action_name not in exploration_stats['novel_actions']:
            exploration_stats['novel_actions'].add(action_name)
        
        # Apply action and get reward
        world.applyMotorCommand(action, world.getSimulationTime())
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        exploration_stats['total_reward'] += reward
        
        # Check exploration metrics
        if agent.getExplorationScore() > 0.7:
            exploration_stats['high_curiosity_steps'] += 1
        
        # Print exploration insights periodically
        if step % 100 == 0:
            print(f"Step {step}: Exploration score {agent.getExplorationScore():.3f}, "
                  f"Novel actions: {len(exploration_stats['novel_actions'])}, "
                  f"Total reward: {exploration_stats['total_reward']:.2f}")
    
    return exploration_stats

# Create and run exploratory agent
print("Creating exploratory agent...")
brain, agent = create_exploratory_agent(seed=42)

print("Setting up world...")
world = pynlm.createSimpleWorld()

print("Running exploration simulation...")
stats = run_exploration_simulation(world, agent, 400)

print("\nExploration Statistics:")
print(f"  Steps with high curiosity: {stats['high_curiosity_steps']}")
print(f"  Unique novel actions: {len(stats['novel_actions'])}")
print(f"  All novel actions: {', '.join(stats['novel_actions'])}")
print(f"  Total reward accumulated: {stats['total_reward']:.2f}")
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

### Project 3: Complete Agent with Advanced Learning

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
| Get configuration value | `config.getIntOr("key", default)` |
| Set configuration value | `config.setInt("key", value)` |
| Enable learning | `agent.enableLearning(True)` |
| Get agent status | `agent.getStatus()` |
| Get exploration score | `agent.getExplorationScore()` |
| Process vision data | `agent.processVisionInput(data, width, height, channels)` |
| Create action from string | `agent.createActionFromString("MoveForward")` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if the brain has enough neurons configured

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify that the brain is producing actions

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if your configuration has enough neurons and connections

**"Configuration errors"**
- Use `config.getIntOr("key", default)` for safe access
- Check spelling of configuration keys
- Valid types: int, double, string, bool

**"Agent learning problems"**
- Enable learning: `agent.enableLearning(True)`
- Check curiosity levels: `agent.getCuriosityLevel()`
- Verify reward signals are being produced

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try creating your own custom sensory inputs
5. Implement advanced learning algorithms
6. Add your own world types
7. Create multi-agent systems

That's it! You're now ready to use NLM with confidence and advanced capabilities.