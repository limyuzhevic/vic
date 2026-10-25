# NLM - Easy Usage Guide for Beginners

## Enhanced Usage with Pythonic Features

The enhanced bindings provide Pythonic APIs for better resource management and error handling:

```python
import pynlm_enhanced as nlm

# Create enhanced brain with context manager
with nlm.create_enhanced_brain() as brain:
    brain.initialize()
    for step in range(100):
        brain.step(step)
        print(f"Step {step}: {brain.get_total_neuron_count()} neurons")
```

## Property-Based Access

Access brain properties using Pythonic property names:

```python
brain = nlm.create_enhanced_brain()
brain.initialize()

# Use property-based access instead of verbose methods
print(f"Neurons: {brain.get_total_neuron_count()}")
print(f"Firing rate: {brain.get_average_firing_rate():.3f}")
print(f"Developmental stage: {brain.get_developmental_stage()}")
```

## Context Managers for Resource Management

```python
import pynlm_enhanced as nlm

# World with automatic cleanup
with nlm.create_enhanced_world() as world:
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Agent with automatic cleanup
    with nlm.create_enhanced_agent_brain(brain) as agent:
        agent.initialize(world)
        
        # Your simulation code here
        for step in range(100):
            world.update(0.1)
            agent.process_sensory_input(world.get_sensory_percept())
            brain.step(step)
            action = agent.decode_motor_command()
            world.apply_motor_command(action, world.get_simulation_time())
```

## Enhanced Error Handling

```python
import pynlm_enhanced as nlm

try:
    brain = nlm.create_enhanced_brain()
    brain.initialize()
    brain.step(0)
    
    # This will throw a SimulationError with helpful message
    brain.step(1)  # Continue simulation
    
except nlm.SimulationError as e:
    print(f"Simulation error: {e}")
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except nLM.NLMError as e:
    print(f"NLM error: {e}")
```

## Quick Start (Copy & Paste)

### Enhanced Brain Simulation

```python
import pynlm_enhanced as nlm

# Step 1: Create enhanced brain with context manager
with nlm.create_enhanced_brain() as brain:
    # Step 2: Brain is automatically initialized
    brain.initialize()
    
    # Step 3: Make it think for 100 steps
    for step in range(100):
        brain.step(step)  # One step of thinking
    
    print(f"Done! Your brain thought {brain.get_total_spike_count()} times")
    print(f"Firing rate: {brain.get_average_firing_rate():.3f}")
```

### Complete Agent Simulation with Enhanced Features

```python
import pynlm_enhanced as nlm
import pynlm_utils as utils

# Create enhanced components
with nlm.create_enhanced_brain() as brain:
    with nlm.create_enhanced_world() as world:
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        with nlm.create_enhanced_agent_brain(brain) as agent:
            agent.initialize(world)
            
            # Enable learning features
            agent.enable_reward_modulation(True)
            agent.enable_curiosity(True)
            agent.enable_development(True)
            
            # Run simulation with progress tracking and statistics
            stats = utils.SimulationStats()
            tracker = utils.ProgressTracker(50, "Agent Simulation")
            
            for step in range(50):
                world.update(0.1)
                
                # Get sensory input and process
                percept = world.get_sensory_percept()
                agent.process_sensory_input(percept)
                
                # Brain thinks
                brain.step(step)
                
                # Get action and apply it
                action = agent.decode_motor_command()
                world.apply_motor_command(action, world.get_simulation_time())
                
                # Apply reward modulation
                reward = percept.get_internal()[0] if percept.get_internal() else 0.0
                agent.apply_reward_modulation(reward, 0.0)
                
                # Update development
                agent.update_development(0.1)
                
                # Track statistics
                stats.add_step_stats(
                    step,
                    brain.get_firing_neuron_count(),
                    brain.get_average_firing_rate(),
                    agent.get_curiosity_level(),
                    agent.get_novelty_level(),
                    agent.get_prediction_error(),
                    agent.get_neuromodulation_level(),
                    brain.get_developmental_stage(),
                    0.1
                )
                
                tracker.update(step, f"Firing: {brain.get_firing_neuron_count()}")
            
            tracker.finish()
            
            # Print simulation results
            print(f"\nSimulation Results:")
            print(f"Total steps: {stats.get_total_steps()}")
            print(f"Average firing rate: {stats.get_average_firing_rate():.3f}")
            print(f"Max firing neurons: {stats.get_max_firing_neurons()}")
            print(f"Average curiosity: {stats.get_average_curiosity():.3f}")
            print(f"Average novelty: {stats.get_average_novelty():.3f}")
            print(f"Average prediction error: {stats.get_average_prediction_error():.3f}")
            print(f"Simulation time: {stats.get_total_simulation_time():.2f}s")
```

### Configuration with Enhanced Config

```python
import pynlm_enhanced as nlm

# Create enhanced configuration
config = nlm.create_enhanced_config()

# Load from file with validation
try:
    config.load_from_file("config.json")
    if config.validate():
        print("Configuration loaded and validated successfully")
    else:
        print("Configuration is invalid")
except nlm.ConfigurationError as e:
    print(f"Failed to load configuration: {e}")

# Set configuration values
config.set_int("brain.neuron_count", 2000)
config.set_double("brain.synapse_density", 0.15)
config.set_double("brain.v_thresh", -50.0)
config.set_double("brain.v_rest", -70.0)
config.set_double("brain.tau_mem", 20.0)

# Save configuration
config.save_to_file("my_config.json")

# Get configuration summary
print(config.summary())
```

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

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
