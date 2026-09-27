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
        agent.processSensoryInput(percept)
        
        # 3. Brain thinks
        brain.step(step)
        
        # 4. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 5. Do action in world
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

## Advanced Features

For more advanced features, check out the complete documentation:

- **Full API Reference**: [NLM_PYTHON_API.md](NLM_PYTHON_API.md) - Complete API documentation with examples
- **Advanced Examples**: [NLM_ADVANCED_EXAMPLES.md](NLM_ADVANCED_EXAMPLES.md) - Complex use cases and patterns
- **Configuration Presets**: [NLM_CONFIGURATION_PRESETS.md](NLM_CONFIGURATION_PRESETS.md) - Pre-configured settings for different use cases

### Quick Advanced Examples

#### Create a brain with learning enabled
```python
import pynlm

# Create brain with preset configuration
config = pynlm.create_preset_config("standard")
brain = pynlm.createBrain(config)
brain.initialize()

# Enable advanced features
brain.enableWorkingMemory(True)
brain.enableCuriosity(True)
brain.enableDevelopment(True)
```

#### Run a simulation with monitoring
```python
import pynlm

# Create brain with monitoring
config = pynlm.create_preset_config("advanced")
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.create_world_with_obstacles(30, 30, 10)
agent = pynlm.create_agent_with_monitoring(brain, world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation with error handling
for episode in range(10):
    world.reset()
    
    # Safe simulation step
    step_result = pynlm.safe_simulation_step(brain, agent, world, episode)
    if not step_result["success"]:
        print(f"Simulation error: {step_result['errors']}")
    
    # Monitor progress
    stats = pynlm.get_detailed_stats(brain, agent, world)
    print(f"Episode {episode}: {stats['brain']['average_firing_rate']:.2f} Hz")
```

#### Create custom configuration
```python
import pynlm

# Create custom configuration for reinforcement learning
def create_rl_config(learning_style="td_error"):
    config = pynlm.create_preset_config("standard")
    
    if learning_style == "td_error":
        config.set("enable_reward_modulation", True)
        config.set("enable_curiosity", True)
        config.set("discount_factor", 0.95)
    
    return config

# Use custom configuration
config = create_rl_config("td_error")
brain = pynlm.createBrain(config)
brain.initialize()
```

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

**"Errors in simulation"**
- Use safe simulation: `result = pynlm.safe_simulation_step(brain, agent, world, step)`
- Check for configuration errors: `validation = pynlm.validate_configuration(config)`

**"Performance issues"**
- Use smaller simulations first: `config = pynlm.create_preset_config("minimal")`
- Enable checkpointing for long simulations
- Use batch processing for multiple simulations

---

## Next Steps

When you're comfortable:

1. **Read** [NLM_PYTHON_API.md](NLM_PYTHON_API.md) for comprehensive API documentation
2. **Explore** [NLM_ADVANCED_EXAMPLES.md](NLM_ADVANCED_EXAMPLES.md) for advanced use cases
3. **Customize** [NLM_CONFIGURATION_PRESETS.md](NLM_CONFIGURATION_PRESETS.md) for your specific needs
4. **Experiment** with different configurations and features!

That's it! You're now ready to use NLM - both at a basic level and with advanced features!

## License

MIT License - See LICENSE file for details.

## Authors

Research project — See docs for scientific background.