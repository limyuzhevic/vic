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

### Pattern 1: Run a Complete Agent Simulation

```python
def run_agent_simulation(brain, world, agent, num_steps):
    """Run a complete agent simulation with all subsystems."""
    for step in range(num_steps):
        # 1. Update world dynamics
        world.update(0.1)
        
        # 2. Get what the agent perceives
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input into brain
        agent.processSensoryInput(percept)
        
        # 4. Brain processes internal state
        brain.step(step)
        
        # 5. Get action from brain activity
        action = agent.decodeMotorCommand()
        
        # 6. Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Enable learning if configured
        if agent.isRewardModulationEnabled():
            # Get reward from world state (simple example)
            reward = 0.0
            if percept.getInternal():
                reward = percept.getInternal()[0]
            agent.applyRewardModulation(reward, 0.0)
        
        # 8. Update brain development
        if agent.isDevelopmentEnabled():
            agent.updateDevelopment(0.1)

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Advanced Agent with Learning

```python
def run_learning_agent_simulation(num_steps=2000, enable_all_features=True):
    """Run an agent with all learning capabilities enabled."""
    
    # 1. Create configuration with learning settings
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all learning subsystems
    if enable_all_features:
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    
    # 7. Store statistics for analysis
    stats = {
        'steps': 0,
        'firing_rates': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'predictions': [],
        'actions_taken': []
    }
    
    # 8. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        if agent.isDevelopmentEnabled():
            agent.updateDevelopment(0.1)
        
        # Collect statistics periodically
        if step % 100 == 0:
            stats['steps'] += 1
            stats['firing_rates'].append(brain.getAverageFiringRate())
            stats['curiosity_levels'].append(agent.getCuriosityLevel())
            stats['novelty_levels'].append(agent.getNoveltyLevel())
            stats['predictions'].append(agent.getPredictionError())
            stats['actions_taken'].append(motor_cmd)
        
        # Print progress
        if step % 500 == 0:
            print(f"Step {step}: "
                  f"Firing rate: {brain.getAverageFiringRate():.2f} Hz, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                  f"Novelty: {agent.getNoveltyLevel():.3f}, "
                  f"Action: {motor_cmd}")
    
    return stats

# Run advanced learning agent
if __name__ == "__main__":
    print("Running advanced NLM agent with learning...")
    results = run_learning_agent_simulation(2000, enable_all_features=True)
    print(f"\nSimulation complete!")
    print(f"Average firing rate: {sum(results['firing_rates'])/len(results['firing_rates']):.2f} Hz")
    print(f"Average curiosity: {sum(results['curiosity_levels'])/len(results['curiosity_levels']):.3f}")
```

### Pattern 3: Brain Exploration

```python
def explore_brain_configuration(base_config, neuron_variations):
    """Explore different brain configurations to find optimal settings."""
    
    results = []
    
    for variation in neuron_variations:
        # Create modified config
        config = pynlm.createDefaultConfig()
        
        # Copy base config settings
        for key in base_config.getKeys():
            config.set(key, base_config.get(key))
        
        # Apply variation
        config.set("brain.neuron_count", variation['neuron_count'])
        config.set("brain.synapse_density", variation['synapse_density'])
        config.set("plasticity.stdp.learning_rate", variation['learning_rate'])
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run test simulation
        for step in range(100):
            brain.step(step)
        
        # Record statistics
        result = {
            'neuron_count': variation['neuron_count'],
            'synapse_density': variation['synapse_density'],
            'learning_rate': variation['learning_rate'],
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio()
        }
        
        results.append(result)
        print(f"Completed: {result}")
    
    return results

# Example exploration
base_config = pynlm.createDefaultConfig()
variations = [
    {'neuron_count': 500, 'synapse_density': 0.05, 'learning_rate': 0.001},
    {'neuron_count': 1000, 'synapse_density': 0.1, 'learning_rate': 0.001},
    {'neuron_count': 1500, 'synapse_density': 0.15, 'learning_rate': 0.001},
    {'neuron_count': 2000, 'synapse_density': 0.2, 'learning_rate': 0.001},
]

exploration_results = explore_brain_configuration(base_config, variations)
```

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

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
