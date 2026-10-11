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

### Pattern 4: Using Configuration Presets

```python
def create_development_config():
    """Create a configuration optimized for developmental learning"""
    return pynlm.createDevelopmentConfig()

def create_standard_config():
    """Create a standard configuration for typical use cases"""
    return pynlm.createStandardConfig()

def create_high_performance_config():
    """Create a configuration optimized for performance"""
    return pynlm.createHighPerformanceConfig()

def create_minimal_config():
    """Create a minimal configuration for basic simulation"""
    return pynlm.createMinimalConfig()

# Choose the right configuration for your needs
if using_learning:
    config = pynlm.createDevelopmentConfig()
else:
    config = pynlm.createMinimalConfig()
```

### Pattern 5: Complete System Setup

```python
def create_complete_simulation():
    """Create a complete brain-world-agent system with optimal settings"""
    config = pynlm.createDevelopmentConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    return brain, world, agent

def run_training_episode(steps=1000, enable_learning=True):
    """Run a complete training episode with automatic setup"""
    # 1. Create everything
    config = pynlm.createDevelopmentConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 2. Enable learning if requested
    if enable_learning:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
    
    # 3. Run the episode
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    return brain, world, agent
```

### Pattern 6: Using the One-Liner Convenience Function

```python
# The easiest way to get started - everything is set up for you!
brain, world, agent = pynlm.create_complete_simulation()

# Run for 100 steps
pynlm.run_episode(brain, world, agent, 100)

# Get statistics
stats = pynlm.get_simulation_stats(brain, world)
print(f"Simulation completed: {stats['total_neurons']} neurons, {stats['firing_neurons']} firing")
```

### Pattern 7: Configuration Analysis

```python
def analyze_configuration(config):
    """Analyze a configuration and provide recommendations"""
    # Get brain stats
    config_dict = config.getKeys()
    
    recommendations = []
    
    if "learning.rate" not in config_dict:
        recommendations.append("Consider adding a learning rate for better learning")
    
    if "brain.neuron_count" in config_dict and config_dict["brain.neuron_count"] > 10000:
        recommendations.append("High neuron count - consider performance implications")
    
    if not recommendations:
        print("Configuration looks good!")
    else:
        print("Recommendations:")
        for rec in recommendations:
            print(f"  - {rec}")
```
