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

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. **EXPERIMENT WITH IMPROVEMENTS:** Try the new features!
   - JSON configuration: `config.loadFromJSON("config.json")`
   - Batch operations: `config.setMultiple([("key1", "value1"), ("key2", "value2")])`
   - Statistics: `brain.getStatistics()` returns a dictionary of brain stats
   - Learning control: `agent.enableAllLearning()` / `disableAllLearning()`
   - Validation: `if not config.validate(): print(config.getValidationErrors())`

That's it! You're now ready to use NLM with all the improvements.

## Mini Projects (Enhanced)

### Project 1: Silent Brain (Just Neurons) - UNCHANGED

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World (Enhanced)

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

# Enhanced with statistics and validation
print("Running enhanced simulation...")
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
print("Neuron count:", brain.getTotalNeuronCount())
print("Synapse count:", brain.getTotalSynapseCount())
```

### Project 3: Complete Agent (Enhanced with new features)

```python
import pynlm

def run_enhanced_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with all new improvements."""
    
    # 1. Create configuration with JSON support
    config = pynlm.createDefaultConfig()
    
    # Enable enhanced learning
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.dopamine.scale", 1.0)
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all learning (NEW FEATURE!)
    agent.enableAllLearning()
    
    print("Enhanced simulation started...")
    print(f"Initial stats: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
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
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Enhanced logging with statistics (NEW FEATURE!)
        if step % 100 == 0:
            stats = brain.getStatistics()
            agent_stats = agent.getAgentStatistics()
            print(f"Step {step}:")
            print(f"  Neurons firing: {stats['firing_neurons']}")
            print(f"  Firing rate: {stats['firing_rate']:.2f} Hz")
            print(f"  Curiosity: {agent_stats['curiosity']:.3f}")
            print(f"  Novelty: {agent_stats['novelty']:.3f}")
            print(f"  Dev Stage: {agent_stats['development_stage']}")
            print(f"  Total spikes: {stats['total_spikes']}")
    
    print("Enhanced agent simulation complete!")
    
    # Save final state (NEW FEATURE!)
    brain.save("enhanced_simulation_checkpoint.bin")
    
    # Show final statistics
    print("\n=== FINAL STATISTICS ===")
    final_stats = brain.getStatistics()
    for key, value in final_stats.items():
        print(f"{key}: {value}")
    
    return True

# Run the enhanced simulation
run_enhanced_agent_simulation(1000)
```

---

## Quick Reference (Enhanced)

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| **ENHANCED**: Get stats | `brain.getStatistics()` |
| **ENHANCED**: Validate config | `if config.validate(): ...` |
| **ENHANCED**: Batch config | `config.setMultiple([...])` |
| **ENHANCED**: Learning phases | `agent.enableAllLearning()` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| **ENHANCED**: Agent stats | `agent.getAgentStatistics()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| **ENHANCED**: Save state | `brain.save("checkpoint.bin")` |
| **ENHANCED**: Load state | `brain.load("checkpoint.bin")` |
| **ENHANCED**: JSON config | `config.loadFromJSON("config.json")` |
| **ENHANCED**: Batch load | `config.setMultiple([...])` |

---

## Troubleshooting (Enhanced)

### Build Issues

**"JSON/YAML support not available"**
```bash
# Install required dependencies for JSON/YAML parsing
pip install pybind11 nlohmann-json
```

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- **NEW**: Use `brain.getStatistics()` to check brain state

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- **NEW**: Use `agent.getAgentStatistics()` to check agent state

**"Configuration validation failed"**
- Check error messages from `config.getValidationErrors()`
- Ensure required keys are present

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- **NEW**: Use `brain.getStatistics()` to verify brain is working
- Some neurons need input to fire - make sure sensory input is connected

---

## New Features Summary

### Configuration System
- **JSON/YAML support**: Load/save configurations in modern formats
- **Validation**: Check configuration integrity with detailed error reporting
- **Batch operations**: Set multiple configuration values at once
- **Type-safe getters**: Get configuration values with proper type conversion

### Python API Enhancements
- **Statistics**: Get comprehensive brain and agent statistics as dictionaries
- **Learning control**: Fine-grained control over learning subsystems
- **Context managers**: Automatic resource management for brain instances
- **Error handling**: Better Python exception handling for C++ errors

### Checkpoint System
- **Full functionality**: Checkpointing now works reliably (was previously non-functional)
- **Integrity validation**: Check checkpoint integrity before loading
- **Error reporting**: Detailed error messages for checkpoint issues

### Learning and Development
- **Learning phases**: Enable/disable learning subsystems independently
- **Fine-grained control**: Control reward modulation, structural plasticity, development, and curiosity separately
- **Batch enable/disable**: `enableAllLearning()` / `disableAllLearning()` convenience methods

These enhancements make NLM more powerful, reliable, and easier to use for research and development while maintaining backward compatibility.

---

## Final Note

The NLM project has been significantly improved with modern Pythonic features while maintaining backward compatibility. All existing code will continue to work, while new features provide enhanced functionality for advanced users.

You now have access to:
- **Modern configuration formats** (JSON/YAML)
- **Comprehensive validation** and error reporting
- **Rich Python API** with statistics and convenience methods
- **Reliable checkpointing** system
- **Fine-grained learning control**

These improvements make NLM a more productive tool for neural research and development while preserving all existing functionality.

That's it! You're now ready to use NLM with all the improvements!
