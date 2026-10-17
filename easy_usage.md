# NLM - Enhanced Python Usage Guide

## What is NLM?

NLM (Neural Learning Machine) is a **brain simulator** for computers that lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

**New and Enhanced Features in Version 2.0:**
- 🧠 **Advanced Simulation Tools**: Built-in simulation runner and neural analysis utilities
- ⚡ **Enhanced Error Handling**: Custom Python exceptions with detailed error messages
- 🎯 **Convenience Functions**: High-level factory functions for common patterns
- 📊 **Network Analysis**: Built-in tools for analyzing brain performance
- 🔧 **Improved API**: Enhanced method names and additional parameters
- 📚 **Better Documentation**: Comprehensive docstrings and examples

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this, but now with much more power and flexibility.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste) - Enhanced Version

### Example 1: Simplest Brain with Enhanced Features

```python
import pynlm

# Step 1: Make a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps with error handling
try:
    for i in range(100):
        brain.step(i)  # One step of thinking
    
    print("✓ Done! Your brain thought", brain.getTotalSpikeCount(), "times")
    print("  Development stage:", brain.getDevelopmentalStage())
except pynlm.BrainError as e:
    print(f"✗ Brain error: {e}")
```

### Example 2: Advanced Simulation with Built-in Tools

```python
import pynlm

# Create a brain with enhanced configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)

brain = pynlm.createBrain(config)

# Run an enhanced simulation with analysis
print("Running advanced simulation...")
result = pynlm.createAdvancedSimulation(config, 2000, verbose=True)

print(f"Simulation completed: {result.success}")
print(f"Steps completed: {result.steps_completed}")
print(f"Total spikes: {result.total_spikes}")
print(f"Average firing rate: {result.avg_firing_rate} Hz")
print(f"Execution time: {result.elapsed_time:.2f} seconds")
```

### Example 3: Learning Experiment with Built-in Analysis

```python
import pynlm

# Create a brain optimized for learning
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Run a learning experiment with enhanced analysis
print("Starting plasticity learning experiment...")
result = pynlm.runLearningExperiment(5000)

print("=== Learning Experiment Results ===")
print(f"Learning detected: {result.learningDetected}")
print(f"Initial mean weight: {result.initialMeanWeight:.4f}")
print(f"Final mean weight: {result.finalMeanWeight:.4f}")
print(f"Weight change: {result.getWeightChange():.4f}")
print(f"Strengthened synapses: {result.getStrengthenedCount()}")
print(f"Weakened synapses: {result.getWeakenedCount()}")
print(f"Total spikes: {result.totalSpikes}")
print(f"Execution time: {result.executionTime:.2f} seconds")
```

### Example 4: Complete Agent with Error Handling

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run complete agent simulation
print("Starting complete agent simulation...")
for step in range(100):
    world.update(0.1)
    
    try:
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Tell the brain
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward-based learning
        agent.applyRewardModulation(0.1, 0.05)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
            
    except pynlm.BrainError as e:
        print(f"Error at step {step}: {e}")
        break

print("Agent simulation complete!")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

---

## Advanced Usage Guide

### 1. Configuration Management

The enhanced configuration system provides better type safety and convenience methods:

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set values with type safety
config.set("neuron_count", 1500)
config.set("random_seed", 123)
config.set("simulation_timestep", 0.001)
config.set("connection_probability", 0.2)

# Get values with default fallbacks
neuron_count = config.getInt("neuron_count", 500)
simulation_time = config.getFloat("simulation_timestep", 0.001)

# Validate configuration
if config.has("invalid_key"):
    print("Configuration has issues!")

# Save configuration
config.saveToFile("my_config.cfg")
```

### 2. Error Handling

The enhanced bindings provide specific Python exceptions:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())

try:
    brain.initialize()
    result = pynlm.createSimpleSimulation(1000)
except pynlm.BrainError as e:
    print(f"Brain initialization failed: {e}")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.NLMException as e:
    print(f"General NLM error: {e}")
```

### 3. Network Analysis

Built-in neural network analysis tools:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Analyze network statistics
stats = pynlm.analyzeNetwork(brain)
print(stats.toString())

# Compare multiple brains
brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())

pynlm.compareBrains(brain1, brain2)

# Print brain statistics
pynlm.printBrainStats(brain)
```

### 4. Simulation Utilities

High-level simulation utilities:

```python
import pynlm

# Simple simulation wrapper
def quick_test(neuron_count=500, steps=1000):
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", neuron_count)
    
    brain = pynlm.createBrain(config)
    result = pynlm.createAdvancedSimulation(config, steps, verbose=True)
    
    return {
        'success': result.success,
        'steps': result.steps_completed,
        'spikes': result.total_spikes,
        'firing_rate': result.avg_firing_rate,
        'time': result.elapsed_time
    }

# Run quick tests
results = quick_test(1000, 2000)
print(f"Quick test results: {results}")

# Run learning experiments
def learning_test(steps=5000):
    result = pynlm.runLearningExperiment(steps)
    return {
        'learning_detected': result.learningDetected,
        'weight_change': result.getWeightChange(),
        'strengthened': result.getStrengthenedCount(),
        'weakened': result.getWeakenedCount(),
        'time': result.executionTime
    }

learning_results = learning_test()
print(f"Learning test results: {learning_results}")
```

---

## Common Patterns (Enhanced)

### Pattern 1: Run a Simulation with Analysis

```python
def run_simulation_with_analysis(brain, world, agent, num_steps):
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
        
        # 7. Apply learning
        agent.applyRewardModulation(0.1, 0.05)
        
        # 8. Update development
        agent.updateDevelopment(0.1)
    
    # Analyze results
    stats = pynlm.analyzeNetwork(brain)
    return stats

# Use it like this:
# stats = run_simulation_with_analysis(brain, world, agent, 1000)
```

### Pattern 2: Get Comprehensive Brain Stats

```python
def get_brain_stats(brain, step_interval=10):
    stats = []
    
    for step in range(0, 1000, step_interval):
        brain.step(step)
        
        current_stats = {
            'step': step,
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'active_neurons': brain.getActiveNeuronCount(),
            'development_stage': brain.getDevelopmentalStage(),
            'neuromodulation_level': brain.getNeuromodulationLevel() if hasattr(brain, 'getNeuromodulationLevel') else 0.0
        }
        
        stats.append(current_stats)
    
    return stats

# Use it:
# stats = get_brain_stats(brain, 50)
```

### Pattern 3: Enable All Learning Features

```python
def enable_advanced_learning(agent):
    # Enable all available learning systems
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
    return {
        'reward_modulation': agent.isRewardModulationEnabled(),
        'structural_plasticity': agent.isStructuralPlasticityEnabled(),
        'development': agent.isDevelopmentEnabled(),
        'curiosity': agent.isCuriosityEnabled()
    }

# Use it:
# learning_enabled = enable_advanced_learning(agent)
```

---

## What Each Part Does (Enhanced)

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)
- **Enhancement**: Now includes built-in validation and error checking

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!
- **Enhancement**: Now throws `BrainError` if initialization fails

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules
- **Enhancement**: Now with better performance monitoring

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

### Enhanced Utility Functions

- `pynlm.createSimpleSimulation(steps, timestep)`: Run a simple simulation with analysis
- `pynlm.createAdvancedSimulation(config, steps, verbose)`: Run an advanced simulation
- `pynlm.analyzeNetwork(brain)`: Get comprehensive network statistics
- `pynlm.runLearningExperiment(steps)`: Run a learning experiment with analysis
- `pynlm.printBrainStats(brain)`: Print brain statistics
- `pynlm.compareBrains(brain1, brain2)`: Compare two brains

---

## Mini Projects (Enhanced)

### Project 1: Silent Brain with Analysis

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
stats = pynlm.analyzeNetwork(brain)
print(stats.toString())
```

### Project 2: Brain Watching a World with Learning

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

# Enable advanced learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
    # Apply learning
    agent.applyRewardModulation(0.1, 0.05)

print("Watched world for 30 steps")
stats = pynlm.analyzeNetwork(brain)
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(stats.toString())
```

### Project 3: Complete Agent with Error Handling and Analysis

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

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent with error handling
for step in range(100):
    world.update(0.1)
    
    try:
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(0.1, 0.05)
        agent.updateDevelopment(0.1)
        
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
            
    except pynlm.BrainError as e:
        print(f"Error at step {step}: {e}")
        break

print("Agent simulation complete!")

# Comprehensive analysis
stats = pynlm.analyzeNetwork(brain)
print(stats.toString())

# Learning analysis
learning_result = pynlm.runLearningExperiment(100)
print(f"Learning detected: {learning_result.learningDetected}")
print(f"Weight change: {learning_result.getWeightChange():.4f}")
```

---

## Quick Reference (Enhanced)

| What you want | Code | Enhanced Features |
|--------------|------|------------------|
| Create brain | `pynlm.createBrain(config)` | Built-in validation and error checking |
| Start brain | `brain.initialize()` | Throws `BrainError` on failure |
| Make brain think | `brain.step(step)` | Performance monitoring |
| Create world | `pynlm.createSimpleWorld()` | Enhanced world management |
| Create agent | `pynlm.createAgentBrain(brain)` | Advanced agent capabilities |
| Get brain stats | `pynlm.analyzeNetwork(brain)` | Comprehensive analysis |
| See world | `world.getSensoryPercept()` | Enhanced sensory processing |
| Make action | `agent.decodeMotorCommand()` | Better action selection |
| Run simulation | `pynlm.createSimpleSimulation()` | Built-in analysis and reporting |
| Learn from rewards | `agent.applyRewardModulation()` | Enhanced reward processing |

---

## Troubleshooting (Enhanced)

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`? (Now throws `BrainError` if not)
- Try increasing the number of steps
- Check for errors using try/catch blocks

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify the agent has been initialized with the world

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check for initialization errors using error handling

**"Learning not detected"**
- Enable learning features: `agent.enableRewardModulation(True)`
- Check learning parameters in configuration
- Use `pynlm.runLearningExperiment()` for built-in learning analysis

---

## Next Steps

When you're comfortable:

1. **Read the C++ documentation** in `docs/ARCHITECTURE.md` to understand how it all works
2. **Experiment with different configurations** - the enhanced configuration system makes this easy
3. **Use the built-in analysis tools** - `pynlm.analyzeNetwork()`, `pynlm.compareBrains()`
4. **Try the Phase 6 integration demo** for the most advanced features
5. **Learn from the examples** in the enhanced `easy_usage.md` and source code

### Quick Start for Advanced Users:

```python
import pynlm

# Quick setup with analysis
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run with all learning enabled
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation with analysis
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    agent.applyRewardModulation(0.1, 0.05)
    agent.updateDevelopment(0.1)

# Get comprehensive analysis
stats = pynlm.analyzeNetwork(brain)
print(stats.toString())
```

That's it! You're now ready to use the enhanced NLM with all its new features and capabilities. 🚀

### Version 2.0 Features:
- ✅ Enhanced error handling with Python exceptions
- ✅ Built-in simulation tools and analysis utilities
- ✅ Convenience functions for common patterns
- ✅ Comprehensive network analysis tools
- ✅ Better documentation and examples
- ✅ Improved type safety and validation
- ✅ Performance monitoring and reporting
- ✅ Advanced learning integration
- ✅ Enhanced configuration management
- ✅ Better error messages and debugging support