# NLM - Easy Usage Guide for Beginners (ENHANCED)

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

## NEW: Enhanced NLM Python API Features

The NLM Python API has been significantly enhanced with:

### 🔧 **Improved Error Handling**
- Custom exception classes for better debugging
- Descriptive error messages with context
- Specific error types for different failure scenarios

### 🧠 **Enhanced Brain Control**
- Direct neuron property access and manipulation
- Synapse management capabilities
- Working memory system integration
- Prediction system controls

### 🛠️ **Convenience Methods**
- Factory methods for common operations
- Type conversion helpers
- Configuration validation utilities
- Simulation management functions

### 📚 **Better Documentation**
- Comprehensive docstrings
- Practical examples for all new features
- Step-by-step tutorials
- Troubleshooting guide

---

## The 3 Things You Need to Know (ENHANCED)

1. **Brain** - The virtual brain that thinks (Enhanced with direct control)
2. **World** - The environment the brain lives in (Enhanced with better APIs)
3. **Agent** - The bridge connecting brain to world (Enhanced with learning systems)

---

## Quick Start (NEW & IMPROVED)

### Example 1: Simplest Brain (IMPROVED)

```python
import pynlm

# Step 1: Make a brain (using enhanced convenience methods)
brain = pynlm.createSimpleBrain(pynlm.createDefaultConfig())

# Step 2: Brain is automatically initialized

# Step 3: Make it think for 100 steps
pynlm.run_brain_step(brain, 100)

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Example 2: Enhanced Brain in a World (NEW)

```python
import pynlm

# Create everything with enhanced methods
brain = pynlm.createSimpleBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Initialize with enhanced initialization
world.configure(20, 20, 8, 8)
world.reset()
agent.initialize(world)

# Enable learning systems (NEW - these weren't in the original easy version)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation loop with enhanced monitoring
for step in range(50):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Show progress (NEW - enhanced monitoring)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"curiosity: {agent.getCuriosityLevel():.3f}")

print("Enhanced simulation finished!")
```

### Example 3: Configuration with Dictionary (NEW)

```python
import pynlm

# Create a custom configuration using Python dictionary (NEW FEATURE)
config_dict = {
    "brain.neuron_count": 2000,
    "brain.synapse_density": 0.15,
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True,
    "neuromod.dopamine.scale": 1.5,
    "neuromod.curiosity.enable": True
}

config = pynlm.config_from_dict(config_dict)
print(config.summary())

# Create brain with custom config
brain = pynlm.createSimpleBrain(config)
```

### Example 4: Type Conversion (NEW)

```python
import pynlm

# Convert neuron IDs (NEW FEATURE)
source_id = pynlm.to_neuron_id(42)
target_id = pynlm.to_neuron_id(100)
print(f"Source ID: {source_id.value}")
print(f"Target ID: {target_id.value}")

# Convert vectors (NEW FEATURE)
python_list = [0.1, 0.5, 1.0, 0.8]
float_vec = pynlm.to_float_vector(python_list)
print(f"Float vector: {float_vec}")
```

### Example 5: Configuration Validation (NEW)

```python
import pynlm

# Validate configuration (NEW FEATURE)
config = pynlm.createDefaultConfig()
errors = pynlm.validate_config(config)

if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")
else:
    print("✅ Configuration is valid!")
```

### Example 6: Complete Learning Agent (NEW - Advanced)

```python
import pynlm

def run_learning_agent():
    """Run a complete learning agent with all enhanced features."""
    
    # 1. Setup with enhanced methods
    config = pynlm.createDefaultConfig()
    brain = pynlm.createSimpleBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Configure world
    world.configure(15, 15, 8, 8)
    world.reset()
    agent.initialize(world)
    
    # 2. Enable ALL learning systems (ENHANCED FEATURE)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # 3. Enhanced simulation loop
    for step in range(1000):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks with enhanced tracking
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Enhanced monitoring (NEW FEATURE)
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  🧠 Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  🎯 Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  🆕 Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  🧬 Development stage: {brain.getDevelopmentalStage()}")
            print(f"  📊 Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    print("🎉 Complete learning simulation finished!")

# Run the enhanced learning simulation
run_learning_agent()
```

### Example 7: Error Handling (ENHANCED)

```python
import pynlm

# Use enhanced error handling (NEW FEATURE)
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createSimpleBrain(config)
    
    # This should work fine
    pynlm.run_brain_step(brain, 50)
    print("✅ Simulation completed successfully")
    
except pynlm.ConfigError as e:
    print(f"❌ Configuration error: {e}")
except pynlm.BrainError as e:
    print(f"❌ Brain error: {e}")
except pynlm.NLMError as e:
    print(f"❌ General NLM error: {e}")
    
# Try with null config to demonstrate error handling
try:
    brain = pynlm.createSimpleBrain(None)
except pynlm.ConfigError as e:
    print(f"✅ Properly caught config error: {e}")
```

---

## Common Patterns (ENHANCED)

### Pattern 1: Enhanced Simulation Run

```python
def run_enhanced_simulation(brain, world, agent, num_steps):
    """Enhanced simulation with better error handling and monitoring."""
    
    for step in range(num_steps):
        try:
            # 1. Update world
            world.update(0.1)
            
            # 2. Get what the agent sees
            percept = world.getSensoryPercept()
            
            # 3. Tell the brain
            agent.processSensoryInput(percept)
            
            # 4. Brain thinks with enhanced tracking
            brain.step(step)
            
            # 5. Get action from brain
            action = agent.decodeMotorCommand()
            
            # 6. Do action in world
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # 7. Enhanced neuromodulation
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # 8. Update development
            agent.updateDevelopment(0.1)
            
            # 9. Enhanced progress tracking
            if step % 100 == 0:
                print(f"Progress: Step {step}, "
                      f"Firing: {brain.getFiringNeuronCount()}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}")
                      
        except (pynlm.ConfigError, pynlm.BrainError, pynlm.NLMError) as e:
            print(f"Error at step {step}: {e}")
            break
    
    print(f"Simulation completed: {step+1}/{num_steps} steps")

# Use the enhanced pattern
run_enhanced_simulation(brain, world, agent, 1000)
```

### Pattern 2: Configuration Management (NEW)

```python
def create_research_config():
    """Create a configuration optimized for research."""
    
    config_dict = {
        "brain.neuron_count": 5000,
        "brain.synapse_density": 0.2,
        "plasticity.stdp.enable": True,
        "plasticity.hebbian.enable": True,
        "plasticity.structural.enable": True,
        "neuromod.dopamine.scale": 2.0,
        "neuromod.curiosity.enable": True,
        "neuromod.novelty.enable": True,
        "development.stage": "Maturation",
        "world.max_energy": 1000.0,
        "world.energy_decay": 0.001
    }
    
    return pynlm.config_from_dict(config_dict)

# Use the research configuration
research_config = create_research_config()
brain = pynlm.createSimpleBrain(research_config)
```

### Pattern 3: Type Conversion Utilities (NEW)

```python
def prepare_neuron_connections(neuron_ids):
    """Prepare neuron connections with enhanced type conversion."""
    
    connections = []
    for source_id, target_id in neuron_ids:
        # Convert to NeuronId using enhanced helpers
        src = pynlm.to_neuron_id(source_id)
        tgt = pynlm.to_neuron_id(target_id)
        
        connections.append((src, tgt))
    
    return connections

# Use the connection preparation
neuron_pairs = [(0, 1), (1, 2), (2, 0)]
connections = prepare_neuron_connections(neuron_pairs)
```

---

## Mini Projects (ENHANCED)

### Mini Project 1: Silent Brain with Enhanced Monitoring

```python
import pynlm

brain = pynlm.createSimpleBrain(pynlm.createDefaultConfig())

for i in range(10):
    brain.step(i)

print("🧠 Silent brain test done!")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Mini Project 2: Enhanced Brain Watching a World

```python
import pynlm

# Setup with enhanced features
brain = pynlm.createSimpleBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

world.configure(20, 20, 8, 8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Watch the world with enhanced monitoring
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
    if i % 10 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing, "
              f"curiosity: {agent.getCuriosityLevel():.3f}")

print("👀 Watched world for 30 steps")
print(f"📊 Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Mini Project 3: Complete Learning Agent (ENHANCED)

```python
import pynlm

# Setup with enhanced configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(15, 15, 8, 8)
world.reset()
agent.initialize(world)

# Enable ALL learning systems (ENHANCED FEATURE)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run enhanced agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("🚀 Agent simulation complete!")
print(f"🎯 Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"🧬 Development stage: {brain.getDevelopmentalStage()}")
```

---

## Quick Reference (ENHANCED)

| What you want | Old Code | **NEW Code (Enhanced)** |
|--------------|----------|--------------------------|
| Create brain | `pynlm.createBrain(config)` | `**pynlm.createSimpleBrain(config)**` |
| Start brain | `brain.initialize()` | **Automatic (with createSimpleBrain)** |
| Make brain think | `brain.step(step)` | `**pynlm.run_brain_step(brain, steps)**` |
| Create world | `pynlm.createSimpleWorld()` | `**pynlm.createSimpleWorld()**` (same, but enhanced) |
| Create agent | `pynlm.createAgentBrain(brain)` | `**pynlm.createAgentBrain(brain)**` (same, but enhanced) |
| Get brain stats | `brain.getFiringNeuronCount()` | `**brain.getFiringNeuronCount()**` (same, but more available) |
| See world | `world.getSensoryPercept()` | `**world.getSensoryPercept()**` (same, but enhanced) |
| Make action | `agent.decodeMotorCommand()` | `**agent.decodeMotorCommand()**` (same, but enhanced) |

**NEW Enhanced Features:**
- `pynlm.createDefaultConfig()` - Enhanced configuration factory
- `pynlm.config_from_dict(dict)` - New configuration from dictionary
- `pynlm.to_neuron_id(value)` - New type conversion helper
- `pynlm.from_neuron_id(id)` - New type conversion helper
- `pynlm.validate_config(config)` - New configuration validation
- `pynlm.get_version()` - New version information
- `pynlm.run_brain_step(brain, steps)` - New simulation helper

---

## Troubleshooting (ENHANCED)

### NEW: Enhanced Error Messages

**"My brain isn't doing anything"**
- Did you enable the learning systems? Try `agent.enableRewardModulation(True)` etc.
- Did you call the enhanced simulation functions? Use `pynlm.run_brain_step()`
- Check the enhanced error messages for specific issues

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Are all learning systems enabled? Use the enhanced enable methods
- Check the agent's curiosity level - it drives exploration

**"Everything is 0"**
- Brains need time to "warm up" - try the enhanced simulation with more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if the learning systems are properly enabled

**"Configuration error"**
- Use `pynlm.validate_config()` to see what specific configuration issues exist
- Try `pynlm.createDefaultConfig()` for a reliable baseline
- Check the enhanced error messages for specific missing keys

### NEW: Enhanced Error Recovery

```python
# Use the enhanced error handling for better recovery
try:
    brain = pynlm.createSimpleBrain(config)
    pynlm.run_brain_step(brain, 100)
except pynlm.ConfigError:
    # Retry with default configuration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createSimpleBrain(config)
    pynlm.run_brain_step(brain, 100)
except pynlm.BrainError:
    # Reset and retry
    brain.reset()
    pynlm.run_brain_step(brain, 100)
```

---

## Next Steps

When you're comfortable with the enhanced features:

1. **Read `HOW_TO_USE.md`** for more details on advanced usage
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Experiment with different configurations!**
4. **Use the enhanced error messages to debug issues**

You now have access to:

- 🚀 **Enhanced error handling** with custom exception classes
- 🧠 **Direct neuron access** with comprehensive control
- 🔧 **Convenience methods** that make common tasks easier
- 📊 **Enhanced monitoring** with detailed progress tracking
- 🛠️ **Configuration validation** to catch issues early
- 🎯 **Type conversion helpers** for seamless Python-C++ integration

That's it! You're now ready to use the **ENHANCED** NLM with all its new features and improvements.

**Key Improvements:** Enhanced error handling, direct neuron access, convenience methods, better monitoring, configuration validation, and type conversion helpers!


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
