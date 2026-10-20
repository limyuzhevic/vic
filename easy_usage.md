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

## New Enhanced Features (v6.0.0)

NLM v6.0.0 includes major enhancements with **100% backward compatibility**:

### 🚀 What's New?

**Enhanced Logging System:**
- New log levels: **Trace** (-1) and **Fatal** (5) for finer control
- Category-based filtering (Brain, Memory, Neuromodulation, Cognition, Development, Performance, System, Debug)
- Asynchronous logging with lock-free queues for high performance
- Structured logging with JSON output for easy analysis
- Performance metrics and system monitoring

**Advanced Configuration System:**
- Schema-based configuration with validation
- Multiple file format support (JSON, YAML, INI, TOML)
- Runtime configuration updates
- Configuration history and change tracking
- Template-based configuration

**Enhanced CLI Tool:**
- Comprehensive command-line interface
- Advanced configuration management
- Real-time logging control
- Performance monitoring

**New Python Features:**
- Enhanced configuration validation
- Advanced logging integration
- Template-based configuration
- Configuration history and snapshots

### How to Get Started with New Features

#### 1. Basic Enhanced Logging
```python
import pynlm

# Create brain with enhanced logging
config = pynlm.createDefaultConfig()
config.set("log_level", "INFO")
config.set("log_async", True)  # Enable async logging
config.set("log_system_metrics", True)  # Monitor system resources

brain = pynlm.createBrain(config)
brain.initialize()

# Log simulation progress
for step in range(100):
    brain.step(step)
    
    if step % 10 == 0:
        pynlm.Logger.logInfo(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

#### 2. Configuration Validation
```python
import pynlm

# Create configuration with schema validation
config = pynlm.createDefaultConfig()

# Validate configuration
validation = config.validateSchema()
if validation.valid:
    print("✓ Configuration is valid!")
    
    # Use configuration
    config.set("brain.neuron_count", 2000)
    print(f"Brain configured: {config.get('brain.neuron_count')} neurons")
else:
    print("✗ Configuration errors:")
    for error in validation.errors:
        print(f"  - {error}")
```

#### 3. Advanced Configuration with Templates
```python
import pynlm

# Create configuration with templates
config = pynlm.createDefaultConfig()

# Apply research template
config.applyTemplate("research")

# Customize template
config.set("brain.neuron_count", 5000, reason="Research with more neurons")
config.set("plasticity.stdp.learning_rate", 0.003, reason="Faster learning")

# Validate customized configuration
validation = config.validateSchema()
if validation.valid:
    print("✓ Advanced configuration applied successfully")
else:
    print("✗ Configuration validation failed:", validation.errors)
```

#### 4. Enhanced Logging with Structured Output
```python
import pynlm

# Configure for structured logging
config = pynlm.createDefaultConfig()
config.set("log_file", "simulation.log")
config.set("log_async", True)
config.set("log_system_metrics", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Log structured data
for step in range(100):
    brain.step(step)
    
    # Create structured log entry
    log_entry = {
        "timestamp": time.time(),
        "step": step,
        "firing_neurons": brain.getFiringNeuronCount(),
        "total_neurons": brain.getTotalNeuronCount(),
        "firing_rate": brain.getAverageFiringRate(),
        "development_stage": brain.getDevelopmentalStage(),
        "simulation_type": "research_experiment"
    }
    
    # Log structured data
    pynlm.Logger.logStructured("Brain Statistics", log_entry)
    
    if step % 20 == 0:
        print(f"Step {step}: Logged structured data")
```

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

And now with **enhanced logging, configuration validation, and advanced features!**

---

## Quick Start (Copy & Paste) with Enhanced Features

### Example 1: Enhanced Brain with Logging
```python
import pynlm

# Step 1: Make a brain with enhanced logging
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think with enhanced logging
for i in range(100):
    brain.step(i)  # One step of thinking
    
    # Enhanced logging with progress tracking
    if i % 10 == 0:
        pynlm.Logger.logInfo(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
        
        # Structured logging for analysis
        pynlm.Logger.logStructured("Simulation Progress", {
            "step": i,
            "firing_neurons": brain.getFiringNeuronCount(),
            "development_stage": brain.getDevelopmentalStage()
        })

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
print("Enhanced logging captured all simulation details!")
```

### Example 2: Brain in a Simple World (Enhanced)
```python
import pynlm

# Create everything with enhanced features
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable enhanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run for 50 steps with enhanced logging
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action
    
    # Enhanced logging
    if step % 10 == 0:
        print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development: {brain.getDevelopmentalStage()}")

print("Simulation finished!")
print("Enhanced logging captured all agent behavior!")
```

---

## Simple Analogy (Enhanced)

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |
| **Your brain logs everything** | `pynlm.Logger.logInfo("Step complete")` |
| **Brain configuration validated** | `config.validateSchema()` |
| **Structured logging for analysis** | `pynlm.Logger.logStructured("data", {...})` |

---

## The Simplest Possible Example (Enhanced)

```python
import pynlm

# One line to create a virtual brain with enhanced logging
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
print("✓ Enhanced logging automatically captures all activity")
print("✓ Configuration validation ensures proper setup")
```

---

## Common Patterns (Enhanced)

### Pattern 1: Run a Simulation with Enhanced Logging
```python
def run_simulation_with_logging(brain, world, agent, num_steps):
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
        
        # Enhanced logging
        if step % 100 == 0:
            pynlm.Logger.logInfo(f"Progress: Step {step}/{num_steps}")
            
            # Structured logging for analysis
            pynlm.Logger.logStructured("Simulation Progress", {
                "step": step,
                "total_steps": num_steps,
                "firing_neurons": brain.getFiringNeuronCount(),
                "progress_percent": (step / num_steps) * 100
            })

# Use it like this:
run_simulation_with_logging(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats with Enhanced Metrics
```python
# Just add these inside your loop with enhanced features:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())

# Enhanced metrics with structured logging
pynlm.Logger.logStructured("Performance Metrics", {
    "firing_neurons": brain.getFiringNeuronCount(),
    "total_spikes": brain.getTotalSpikeCount(),
    "firing_rate": brain.getAverageFiringRate(),
    "development_stage": str(brain.getDevelopmentalStage()),
    "curiosity": agent.getCuriosityLevel(),
    "novelty": agent.getNoveltyLevel()
})
```

### Pattern 3: Enable Brain Features with Enhanced Logging
```python
# These are all optional with enhanced logging:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Enhanced logging for feature monitoring
pynlm.Logger.logInfo("Enhanced brain features enabled:")
pynlm.Logger.logInfo("  - Reward modulation enabled")
pynlm.Logger.logInfo("  - Structural plasticity enabled") 
pynlm.Logger.logInfo("  - Development enabled")
pynlm.Logger.logInfo("  - Curiosity enabled")
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

And now with **enhanced logging, configuration validation, and advanced features!**

---

## Mini Projects (Enhanced)

### Project 1: Silent Brain with Enhanced Logging
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)
    
    # Enhanced logging
    pynlm.Logger.logInfo(f"Step {i}: Brain thinking...")

print("Silent brain test done!")
print("✓ Enhanced logging captured all brain activity")
```

### Project 2: Brain Watching a World with Enhanced Features
```python
import pynlm

# Setup with enhanced features
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable enhanced learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Watch the world for 30 steps with enhanced logging
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
    # Enhanced logging with detailed metrics
    if i % 10 == 0:
        pynlm.Logger.logStructured("World Observation", {
            "step": i,
            "firing_rate": brain.getAverageFiringRate(),
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel()
        })

print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
print("✓ Enhanced logging captured world interaction")
```

### Project 3: Complete Agent with Enhanced Features
```python
import pynlm

# Setup with enhanced features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all enhanced learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent with enhanced logging
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        # Enhanced logging with agent behavior metrics
        pynlm.Logger.logStructured("Agent Performance", {
            "step": step,
            "firing_neurons": brain.getFiringNeuronCount(),
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "reward": world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        })
        
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
print("✓ Enhanced logging captured all agent behavior")
```

---

## Quick Reference (Enhanced)

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
| **Log progress** | `pynlm.Logger.logInfo("message")` |
| **Validate config** | `config.validateSchema()` |
| **Structured logging** | `pynlm.Logger.logStructured("data", {...})` |
| **Apply template** | `config.applyTemplate("name")` |
| **Enable features** | `agent.enableCuriosity(True)` |

---

## Troubleshooting (Enhanced)

### Enhanced Troubleshooting with New Error Messages

**"Configuration validation failed"**
- Did you call `config.validateSchema()`?
- Check for missing or invalid parameters
- Ensure configuration matches schema requirements

**"Log file not created"**
- Did you configure `log_file` in your config?
- Check file permissions
- Enable `log_async` for better performance

**"Performance degradation"**
- Did you enable excessive logging?
- Consider using `log_sampling` for large simulations
- Use `log_level=WARNING` for production

**"Brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Enable enhanced learning features with `agent.enableCuriosity(True)`

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Enable motor learning with `agent.enableStructuralPlasticity(True)`

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Configure enhanced learning with `agent.enableRewardModulation(True)`

---

## Next Steps

When you're comfortable:

1. Read `HOW_TO_USE.md` for **advanced CLI documentation, enhanced logging, configuration validation, and Python API details**
2. Read `docs/ARCHITECTURE.md` to understand **how all the enhanced features work together**
3. **Experiment with different configurations** using templates and schemas
4. **Explore advanced features** like async logging, structured logging, and configuration history
5. **Run the enhanced CLI tool** with `./nlm_cli simulate --help` for more options

That's it! You're now ready to use NLM with **all the enhanced features and capabilities**! 🚀

**New v6.0.0 Features Added:**
- ✅ Enhanced logging with Trace/Fatal levels and category filtering
- ✅ Asynchronous logging with lock-free queues
- ✅ Structured logging with JSON output
- ✅ Schema-based configuration validation
- ✅ Advanced CLI tool with comprehensive options
- ✅ Configuration templates and history tracking
- ✅ Performance monitoring and system metrics
- ✅ 100% backward compatibility maintained
- ✅ Comprehensive documentation and examples

You're now equipped to use NLM as a modern, production-ready neural simulation platform! 🎯
