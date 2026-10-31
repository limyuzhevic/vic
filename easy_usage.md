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
print("Total synapses:", brain.getTotalSynapseCount())
print("Firing neurons:", brain.getFiringNeuronCount())
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
### Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if neurons have enough input
- Verify configuration settings

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Ensure agent has initialized with world
- Verify motor command decoding

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if reward modulation is enabled
- Verify neural parameters are reasonable

**"Configuration errors"**
- Check that configuration values are properly set
- Use `config.getOr()` to provide defaults
- Validate configuration before use with `config.validate()`

---

## Advanced Configuration Patterns

### Pattern 1: Custom Configuration with Validation

```python
def setup_advanced_brain():
    import pynlm
    
    # Create custom configuration
    config = pynlm.createDefaultConfig()
    
    # Override specific parameters
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.dopamine.scale", 2.0)
    config.set("neuromod.curiosity.enable", True)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with error handling
    agent = pynlm.createAgentBrain(brain)
    
    # Validate configuration
    if not agent.getBrain().getConfig().validate():
        errors = agent.getBrain().getConfig().getValidationErrors()
        print(f"Configuration errors: {errors}")
        return None
    
    return brain, agent
```

### Pattern 2: Complex Simulation with Monitoring

```python
def run_monitored_simulation(num_steps=1000, monitor_interval=100):
    import pynlm
    import time
    
    print("Starting advanced NLM simulation with monitoring...")
    
    # Setup components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Simulation monitoring
    start_time = time.time()
    spike_history = []
    firing_rate_history = []
    reward_history = []
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Record metrics
        spike_history.append(brain.getTotalSpikeCount())
        firing_rate_history.append(brain.getFiringNeuronCount())
        reward_history.append(reward)
        
        # Monitor progress
        if step % monitor_interval == 0:
            elapsed = time.time() - start_time
            print(f"Step {step}/{num_steps} ({elapsed:.1f}s)")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Avg firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Reward: {reward:.3f}")
            print()
    
    # Final statistics
    end_time = time.time()
    total_time = end_time - start_time
    
    print("Simulation complete!")
    print(f"Total time: {total_time:.1f}s")
    print(f"Average steps/sec: {num_steps/total_time:.1f}")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty: {agent.getNoveltyLevel():.3f}")
    
    return spike_history, firing_rate_history, reward_history
```

### Pattern 3: Checkpoint and Resume with Error Handling

```python
def run_simulation_with_checkpoints(num_steps=500, checkpoint_interval=100):
    import pynlm
    import os
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    checkpoint_file = "nlm_checkpoint.bin"
    
    # Try to load previous checkpoint
    if os.path.exists(checkpoint_file):
        print("Loading checkpoint...")
        try:
            brain.load(checkpoint_file)
            agent.reset()
            agent.getBrain().initialize()
            print("Checkpoint loaded successfully!")
        except Exception as e:
            print(f"Failed to load checkpoint: {e}")
            print("Starting fresh...")
    
    # Run simulation
    for step in range(num_steps):
        # World update
        world.update(0.1)
        
        # Sensor processing
        agent.processSensoryInput(world.getSensoryPercept())
        
        # Brain processing
        brain.step(step)
        
        # Action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Save checkpoint periodically
        if step % checkpoint_interval == 0 and step > 0:
            print(f"Saving checkpoint at step {step}...")
            try:
                brain.save(checkpoint_file)
                print("Checkpoint saved successfully!")
            except Exception as e:
                print(f"Failed to save checkpoint: {e}")
    
    print("Simulation complete!")
    return brain, agent, world
```

---

## Advanced Configuration Options

### Configuration Validation Examples

```python
import pynlm

config = pynlm.createDefaultConfig()

# Safe configuration with validation
config.setValidated("brain.neuron_count", 1000)  # Validates range
config.setStringValidated("brain.name", "mybrain")  # Validates string
config.setNumericValidated("brain.learning_rate", 0.001)  # Validates numeric range

# Custom validation rules (advanced feature)
config.addValidationRule({
    "key": "brain.neuron_count",
    "type": "range",
    "description": "Number of neurons (1000-10000)",
    "defaultValue": 1000,
    "range": (1000.0, 10000.0)
})

# Validate configuration
if not config.validate():
    print("Validation errors:", config.getValidationErrors())

# Load from environment variables
config.loadFromEnvironment("NLM_*")
```

### Error Handling Best Practices

```python
def safe_brain_operation(config, steps):
    """Example of safe brain operations with error handling."""
    import pynlm
    
    try:
        # Validate configuration
        if not config.validate():
            raise ValueError(f"Invalid config: {config.getValidationErrors()}")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(steps):
            try:
                brain.step(step)
            except Exception as e:
                print(f"Error at step {step}: {e}")
                # Continue with error recovery or break
                break
                
        return brain
        
    except Exception as e:
        print(f"Brain operation failed: {e}")
        return None

# Usage
config = pynlm.createDefaultConfig()
brain = safe_brain_operation(config, 1000)
```

---

## Performance Tips

### For Large Networks

```python
# Optimize performance for large networks
config.set("brain.neuron_count", 5000)  # Larger network
config.set("brain.synapse_density", 0.05)  # Fewer connections
config.set("simulation.timestep", 0.01)  # Larger timestep
```

### Memory Management

```python
# Monitor memory usage
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Memory neurons: {brain.getTotalNeuronCount()}")
        print(f"  Memory synapses: {brain.getTotalSynapseCount()}")
        print(f"  Active neurons: {brain.getFiringNeuronCount()}")
```

---

## Quick Reference

| Feature | Method | Description |
|---------|--------|-------------|
| Create brain | `pynlm.createBrain(config)` | Creates a brain with configuration |
| Initialize | `brain.initialize()` | Prepares brain for simulation |
| Run simulation | `brain.step(step)` | Runs one simulation step |
| Create world | `pynlm.createSimpleWorld()` | Creates environment |
| Create agent | `pynlm.createAgentBrain(brain)` | Connects brain to world |
| Get stats | `brain.getTotalNeuronCount()` | Brain statistics |
| See world | `world.getSensoryPercept()` | Get sensory input |
| Make action | `agent.decodeMotorCommand()` | Get motor command |
| Enable learning | `agent.enableRewardModulation(True)` | Turn on learning features |

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md`** for more details on building and installing
2. **Read `docs/ARCHITECTURE.md`** to understand system architecture
3. **Experiment with different configurations** and validation rules
4. **Try advanced patterns** like checkpointing and monitoring
5. **Build your own agents** with custom behaviors and learning rules

That's it! You're now ready to use NLM with confidence, including proper error handling, configuration validation, and advanced monitoring techniques.
