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

## Enhanced Usage - New Features

NLM now includes **enhanced Python bindings** with advanced neuroscience research and experimentation capabilities!

### 🚀 New Enhanced Controllers

In addition to the basic Brain, World, and Agent interfaces, NLM now provides specialized controllers for advanced functionality:

```python
import pynlm

# Create enhanced controllers for advanced features
plasticity = pynlm.create_enhanced_plasticity_controller()
neuromod = pynlm.create_enhanced_neuromodulation_controller()
state = pynlm.create_enhanced_state_manager()
development = pynlm.create_enhanced_development_controller()
config = pynlm.create_enhanced_configuration_manager()
```

### Example 2: Enhanced Brain in a Simple World

Here's an example using the enhanced features:

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

# Create enhanced controllers
plasticity = pynlm.create_enhanced_plasticity_controller()
neuromod = pynlm.create_enhanced_neuromodulation_controller()
state = pynlm.create_enhanced_state_manager()
development = pynlm.create_enhanced_development_controller()

# Enable enhanced features
plasticity.enable_stdp(True)
plasticity.enable_hebbian(True)
neuromod.enable_curiosity(True)
neuromod.enable_novelty_detection(True)
development.enable_aging_simulation(True)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    neuromod.update_neuromodulators(0.1)  # Update neuromodulators
    development.update_development(0.1)  # Update development
    
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action
    
    # Save checkpoint periodically
    if step % 10 == 0:
        state.create_checkpoint(f"checkpoint_{step}")

print("Enhanced simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
print("Curiosity level:", neuromod.get_curiosity_level())
print("Novelty level:", neuromod.get_novelty_level())
```

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

**NEW**: Enhanced controllers for advanced neuroscience research:

4. **PlasticityController** - Control STDP, Hebbian, and structural plasticity
5. **NeuromodulationController** - Fine-tune reward, curiosity, and novelty
6. **StateManager** - Advanced checkpointing and state management
7. **DevelopmentController** - Control developmental stages and aging
8. **ConfigurationManager** - Hierarchical configuration and presets

---

## Common Patterns

### Pattern 1: Enhanced Research Simulation

```python
def run_enhanced_research_simulation(brain, world, agent, num_steps):
    # Create enhanced controllers
    plasticity = pynlm.create_enhanced_plasticity_controller()
    neuromod = pynlm.create_enhanced_neuromodulation_controller()
    state = pynlm.create_enhanced_state_manager()
    development = pynlm.create_enhanced_development_controller()
    
    # Enable research features
    plasticity.enable_stdp(True)
    plasticity.enable_hebbian(True)
    neuromod.enable_curiosity(True)
    neuromod.enable_novelty_detection(True)
    development.enable_aging_simulation(True)
    
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Update enhanced controllers
        neuromod.update_neuromodulators(0.1)
        development.update_development(0.1)
        
        # 5. Brain thinks
        brain.step(step)
        
        # 6. Get action from brain
        motor_cmd = agent.decodeMotorCommand()
        
        # 7. Do action in world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # 8. Save checkpoint periodically
        if step % 100 == 0:
            state.create_checkpoint(f"step_{step}")
        
        # 9. Print enhanced statistics
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity: {neuromod.get_curiosity_level():.3f}")
            print(f"  Novelty: {neuromod.get_novelty_level():.3f}")
            print(f"  Development stage: {brain.get_developmental_stage()}")
    
    # Save final state
    state.save_brain_state("final_state.bin")

# Use it like this:
run_enhanced_research_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Enhanced Brain Stats

```python
# Just add these inside your enhanced loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())

# Enhanced statistics
if 'neuromod' in locals():
    print("Curiosity level:", neuromod.get_curiosity_level())
    print("Novelty level:", neuromod.get_novelty_level())
    print("Reward prediction error:", neuromod.get_reward_prediction_error())

if 'development' in locals():
    print("Developmental stage:", development.get_developmental_stage())
    print("Plasticity rate:", development.get_plasticity_rate())

if 'state' in locals():
    print("State size:", state.get_state_size())
    print("Compression ratio:", state.get_compression_ratio())
```

### Pattern 3: Enable All Enhanced Features

```python
# These are all optional enhanced features:
agent.enable_reward_modulation(True)      # Learn from rewards
agent.enable_structural_plasticity(True)  # Grow new connections
agent.enable_development(True)           # Brain matures over time
agent.enable_curiosity(True)             # Explore new things

# Create enhanced controllers
plasticity = pynlm.create_enhanced_plasticity_controller()
plasticity.enable_stdp(True)
plasticity.enable_hebbian(True)

neuromod = pynlm.create_enhanced_neuromodulation_controller()
neuromod.enable_curiosity(True)
neuromod.enable_novelty_detection(True)

state = pynlm.create_enhanced_state_manager()
development = pynlm.create_enhanced_development_controller()
config = pynlm.create_enhanced_configuration_manager()
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time.

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

### **Enhanced Features**:

#### `plasticity = pynlm.create_enhanced_plasticity_controller()`
Advanced plasticity control system.

#### `neuromod = pynlm.create_enhanced_neuromodulation_controller()`
Fine-grained neuromodulation control.

#### `state = pynlm.create_enhanced_state_manager()`
Comprehensive state management and checkpointing.

#### `development = pynlm.create_enhanced_development_controller()`
Precise developmental stage control.

#### `config = pynlm.create_enhanced_configuration_manager()`
Advanced configuration management.

---

## Mini Projects

### Mini Project 1: Enhanced Silent Brain

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create enhanced controllers
plasticity = pynlm.create_enhanced_plasticity_controller()
neuromod = pynlm.create_enhanced_neuromodulation_controller()

# Enable enhanced features
plasticity.enable_stdp(True)
neuromod.enable_curiosity(True)

for i in range(10):
    brain.step(i)
    neuromod.update_neuromodulators(0.1)

print("Enhanced silent brain test done!")
print(f"Curiosity level: {neuromod.get_curiosity_level()}")
```

### Mini Project 2: Enhanced Brain Watching a World

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

# Create enhanced controllers
plasticity = pynlm.create_enhanced_plasticity_controller()
neuromod = pynlm.create_enhanced_neuromodulation_controller()
state = pynlm.create_enhanced_state_manager()

# Enable enhanced features
plasticity.enable_stdp(True)
neuromod.enable_novelty_detection(True)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    neuromod.update_neuromodulators(0.1)
    
    # Save checkpoint
    if i % 5 == 0:
        state.save_incremental(f"watch_{i}.bin", f"step_{i}")

print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
print("Curiosity level:", neuromod.get_curiosity_level())
```

### Mini Project 3: Complete Enhanced Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable ALL enhanced features
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

# Create enhanced controllers
plasticity = pynlm.create_enhanced_plasticity_controller()
neuromod = pynlm.create_enhanced_neuromodulation_controller()
state = pynlm.create_enhanced_state_manager()
development = pynlm.create_enhanced_development_controller()
config = pynlm.create_enhanced_configuration_manager()

# Enable enhanced features
plasticity.enable_stdp(True)
plasticity.enable_hebbian(True)
neuromod.enable_curiosity(True)
neuromod.enable_novelty_detection(True)
development.enable_aging_simulation(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    neuromod.update_neuromodulators(0.1)
    development.update_development(0.1)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Save checkpoint
    if step % 20 == 0:
        state.create_checkpoint(f"agent_step_{step}")
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Curiosity: {neuromod.get_curiosity_level():.3f}")
        print(f"  Novelty: {neuromod.get_novelty_level():.3f}")

print("Enhanced agent simulation complete!")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create enhanced plasticity controller | `pynlm.create_enhanced_plasticity_controller()` |
| Create enhanced neuromodulation controller | `pynlm.create_enhanced_neuromodulation_controller()` |
| Create enhanced state manager | `pynlm.create_enhanced_state_manager()` |
| Create enhanced development controller | `pynlm.create_enhanced_development_controller()` |
| Create enhanced configuration manager | `pynlm.create_enhanced_configuration_manager()` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Enable reward modulation | `agent.enableRewardModulation(True)` |
| Enable curiosity | `neuromod.enable_curiosity(True)` |
| Enable novelty detection | `neuromod.enable_novelty_detection(True)` |

---

## Troubleshooting

**"My enhanced simulation isn't working"**
- Did you call `brain.initialize()`?
- Did you enable the features you want?
- Try printing the enhanced controller statistics

**"The enhanced agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Did you enable all the enhanced features?
- Check the neuromodulation and development controllers

**"Everything is 0"**
- Enhanced controllers need time to "warm up" - try more steps
- Make sure sensory input is connected to neuromodulation
- Check that development is enabled for maturation effects

---

## Next Steps

When you're comfortable with the enhanced features:

1. **Read `HOW_TO_USE.md` for more details** - Technical information about all features
2. **Read `docs/ARCHITECTURE.md` to understand how it all works** - Deep dive into system architecture
3. **Experiment with different enhanced configurations!** - Try different scenarios
4. **Check out `README_ENHANCED.md`** - Comprehensive guide to all enhanced features

That's it! You're now ready to use NLM with enhanced capabilities for advanced neuroscience research and experimentation!

## Quick Start with Enhanced Features

### One Line Enhanced Simulation

```python
import pynlm

# One line to create a virtual brain with ALL enhanced features
brain = pynlm.createBrain(pynlm.create_advanced_config())
brain.initialize()

# Create enhanced controllers for research
plasticity = pynlm.create_enhanced_plasticity_controller()
neuromod = pynlm.create_enhanced_neuromodulation_controller()
state = pynlm.create_enhanced_state_manager()
development = pynlm.create_enhanced_development_controller()

# Enable ALL advanced features
plasticity.enable_stdp(True)
plasticity.enable_hebbian(True)
neuromod.enable_curiosity(True)
neuromod.enable_novelty_detection(True)
development.enable_aging_simulation(True)

# Run enhanced simulation
for i in range(100):
    brain.step(i)
    neuromod.update_neuromodulators(0.1)
    development.update_development(0.1)

print("Enhanced brain has", brain.getTotalNeuronCount(), "neurons!")
print("Research complete with enhanced features!")
```

That's it! You're now ready to use NLM with enhanced capabilities for advanced neuroscience research and experimentation!

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
