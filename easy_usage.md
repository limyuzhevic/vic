# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

## New Improved Features (Version 0.1.0+)

### Enhanced Error Handling
- Custom exceptions: `NLMError`, `ConfigurationError`, `SimulationError`
- Better error messages with detailed context
- Graceful failure handling in common scenarios

### Convenience Functions
- `createConfigPreset(name)`: Create configurations with predefined settings
- `runAgentEpisode(agent, world, numSteps, verbose)`: Run complete agent episodes
- `computeBrainStats(brain)`: Get comprehensive brain statistics
- `computeAgentStats(agent)`: Get agent performance metrics

### Advanced Configuration Options
- Preset configurations: "minimal", "standard", "advanced"
- Enhanced configuration validation
- Configuration normalization

### Enhanced Documentation
- Detailed docstrings for all classes and methods
- Type hints and parameter descriptions
- Usage examples and tutorials
- Error code references

## Quick Start (Copy & Paste) - Enhanced Version

### Example 1: Simplest Brain (Minimal Example)

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

### Example 2: Using Configuration Presets

```python
import pynlm

# Create different brain configurations
minimal_config = pynlm.createConfigPreset("minimal")
standard_config = pynlm.createConfigPreset("standard") 
advanced_config = pynlm.createConfigPreset("advanced")

# Create brains with different configurations
minimal_brain = pynlm.createBrain(minimal_config)
standard_brain = pynlm.createBrain(standard_config)
advanced_brain = pynlm.createBrain(advanced_config)
```

### Example 3: Enhanced Agent Simulation with Statistics

```python
import pynlm

# Create advanced setup with all subsystems
config = pynlm.createConfigPreset("advanced")
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation with progress tracking
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        # Get detailed statistics
        brain_stats = pynlm.computeBrainStats(brain)
        agent_stats = pynlm.computeAgentStats(agent)
        
        print(f"Step {step}:")
        print(f"  Neurons: {brain_stats['neuron_count']}, Firing: {brain_stats['firing_neurons']}")
        print(f"  Neuromodulation: {agent_stats['neuromodulation']:.3f}")
        print(f"  Curiosity: {agent_stats['curiosity']:.3f}")
        print(f"  Novelty: {agent_stats['novelty']:.3f}")
        print(f"  Development Stage: {agent_stats['development_stage']}")

# Get final statistics
final_brain_stats = pynlm.computeBrainStats(brain)
final_agent_stats = pynlm.computeAgentStats(agent)
print("\nFinal Statistics:")
print(f"Total Spikes: {final_brain_stats['total_spikes']}")
print(f"Final Prediction Error: {final_agent_stats['prediction_error']:.3f}")
```

### Example 4: Error Handling and Validation

```python
import pynlm

# Create and validate configuration
config = pynlm.createConfigPreset("standard")

# Validate configuration
issues = pynlm.validateConfig(config)
if issues:
    print("Configuration issues:", issues)
else:
    print("Configuration is valid!")

# Normalize configuration
pynlm.normalizeConfig(config)

# Safe brain creation with error handling
try:
    brain = pynlm.createBrain(config)
    print("Brain created successfully!")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
```

## Enhanced Quick Reference

### Configuration Management

| What you want | Code |
|--------------|------|
| Create brain | `brain = pynlm.createBrain(config)` |
| Create default config | `config = pynlm.createDefaultConfig()` |
| Create preset config | `config = pynlm.createConfigPreset("standard")` |
| Validate config | `issues = pynlm.validateConfig(config)` |
| Normalize config | `pynlm.normalizeConfig(config)` |

### Enhanced Statistics

| What you want | Code |
|--------------|------|
| Get brain stats | `stats = pynlm.computeBrainStats(brain)` |
| Get agent stats | `stats = pynlm.computeAgentStats(agent)` |
| Get sensory stats | `stats = pynlm.computeSensoryStats(world)` |

### Error Handling

| What you want | Code |
|--------------|------|
| Safe execution | `pynlm.tryCatch(lambda: func(), "Error message")` |
| Custom exceptions | `except pynlm.ConfigurationError as e:` |
| Get status summary | `brain.logStatus()` |

### Advanced Features

| What you want | Code |
|--------------|------|
| Run episode | `pynlm.runAgentEpisode(agent, world, 1000, verbose=True)` |
| Get build info | `info = pynlm.getBuildInfo()` |
| Get example | `example = pynlm.createMinimalExample()` |

---

## Enhanced Common Patterns

### Pattern 1: Run a Simulation with Statistics

```python
def run_enhanced_simulation(brain, world, agent, num_steps, verbose=False):
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
        
        # 7. Update development
        agent.updateDevelopment(0.1)
        
        # 8. Log statistics periodically
        if verbose and step % 100 == 0:
            brain_stats = pynlm.computeBrainStats(brain)
            agent_stats = pynlm.computeAgentStats(agent)
            print(f"Step {step}: {brain_stats['firing_neurons']} neurons firing, "
                  f"curiosity: {agent_stats['curiosity']:.3f}")

# Use it like this:
run_enhanced_simulation(brain, world, agent, 1000, verbose=True)
```

### Pattern 2: Get Enhanced Brain Stats

```python
def print_detailed_brain_stats(brain):
    """Print comprehensive brain statistics in a readable format."""
    stats = pynlm.computeBrainStats(brain)
    
    print("=== Brain Statistics ===")
    print(f"Total Neurons: {stats['neuron_count']}")
    print(f"Total Synapses: {stats['synapse_count']}")
    print(f"Active Neurons: {stats['active_neurons']}")
    print(f"Firing Neurons: {stats['firing_neurons']}")
    print(f"Total Spikes: {stats['total_spikes']}")
    print(f"Average Firing Rate: {stats['avg_firing_rate']:.3f} Hz")
    print(f"Excitation/Inhibition Ratio: {stats['e_i_ratio']:.3f}")
    
    # Calculate percentages
    if stats['neuron_count'] > 0:
        firing_percentage = (stats['firing_neurons'] / stats['neuron_count']) * 100
        print(f"Firing Rate Percentage: {firing_percentage:.1f}%")
```

### Pattern 3: Enhanced Agent Setup

```python
def setup_advanced_agent(config_preset="standard"):
    """Setup an agent with all learning subsystems enabled."""
    # Create configuration
    config = pynlm.createConfigPreset(config_preset)
    
    # Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    return brain, agent, world

def run_learning_episode(brain, agent, world, num_steps=500):
    """Run a learning episode with curiosity and reward modulation."""
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply motor command
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            agent_stats = pynlm.computeAgentStats(agent)
            print(f"Step {step}: Curiosity {agent_stats['curiosity']:.3f}, "
                  f"Novelty {agent_stats['novelty']:.3f}, "
                  f"Dev Stage {agent_stats['development_stage']}")
    
    # Get final statistics
    final_stats = pynlm.computeAgentStats(agent)
    print(f"\nEpisode Complete!")
    print(f"Final Curiosity: {final_stats['curiosity']:.3f}")
    print(f"Final Novelty: {final_stats['novelty']:.3f}")
    print(f"Final Prediction Error: {final_stats['prediction_error']:.3f}")
```

## Enhanced Troubleshooting

### "My brain isn't doing anything"

**Check:**
- Did you call `brain.initialize()`?
- Are all learning subsystems enabled?
- Is the configuration valid? Use `pynlm.validateConfig(config)`
- Try running for more steps

**Debug:**
```python
# Debug brain initialization
brain = pynlm.createBrain(config)
print("Brain created:", brain.getTotalNeuronCount(), "neurons")
print("Firing neurons:", brain.getFiringNeuronCount())
print("Brain status:", brain.getStatusSummary())
```

### "The agent isn't moving"

**Check:**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify motor neuron groups are active
- Check agent statistics: `pynlm.computeAgentStats(agent)`

**Debug:**
```python
# Debug agent behavior
agent_stats = pynlm.computeAgentStats(agent)
print("Agent statistics:", agent_stats)
print("Neuromodulation level:", agent_stats['neuromodulation'])
print("Curiosity level:", agent_stats['curiosity'])
print("Motor neuron groups:", agent.getMotorOutputSize(), "output size")
```

### "Everything is 0"

**Check:**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration: ensure neurons and connections are enabled
- Verify world configuration: check vision dimensions

**Debug:**
```python
# Debug simulation state
brain = pynlm.createBrain(config)
brain.initialize()

# Check initial state
print("Initial state:")
print("  Neurons:", brain.getTotalNeuronCount())
print("  Firing:", brain.getFiringNeuronCount())
print("  Active:", brain.getActiveNeuronCount())

# Run a few steps to warm up
for i in range(10):
    brain.step(i)

print("After warm-up:")
print("  Firing:", brain.getFiringNeuronCount())
print("  Active:", brain.getActiveNeuronCount())
print("  Spikes:", brain.getTotalSpikeCount())
```

## New Examples

### Example 5: Configuration Management

```python
import pynlm

# Create and save a custom configuration
config = pynlm.createConfigPreset("advanced")
config.set("brain.neuron_count", 3000)
config.saveToFile("my_brain_config.json")

# Load configuration from file
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile("my_brain_config.json")

# Validate and normalize
issues = pynlm.validateConfig(loaded_config)
if not issues:
    pynlm.normalizeConfig(loaded_config)
    print("Configuration is ready to use!")
```

### Example 6: Performance Monitoring

```python
import time
import pynlm

# Create simulation components
config = pynlm.createConfigPreset("standard")
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()

# Time the simulation
start_time = time.time()
steps_per_sec = []

for episode in range(5):
    world.reset()
    agent.reset()
    brain.reset()
    
    episode_start = time.time()
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.updateDevelopment(0.1)
    
    episode_time = time.time() - episode_start
    steps_per_sec.append(200 / episode_time)

# Get performance statistics
perf_stats = pynlm.getPerformanceStats()
print(f"Average steps per second: {sum(steps_per_sec) / len(steps_per_sec):.1f}")
print(f"Build info: {pynlm.getBuildInfo()}")
```

### Example 7: Error Handling and Recovery

```python
import pynlm

def safe_simulation(brain, world, agent, steps):
    """Run simulation with error handling and recovery."""
    try:
        for step in range(steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
            
    except pynlm.ConfigurationError as e:
        print(f"Configuration error: {e}")
        # Try to recover with default config
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
    except pynlm.SimulationError as e:
        print(f"Simulation error: {e}")
        # Try to reset and continue
        brain.reset()
        agent.reset()
        world.reset()
    except Exception as e:
        print(f"Unexpected error: {e}")
        raise

# Use the safe simulation
try:
    safe_simulation(brain, world, agent, 1000)
    print("Simulation completed successfully!")
except Exception as e:
    print(f"Simulation failed: {e}")
```

## New Recommendations

### For Beginners:
1. Start with the minimal example using `createBrain()` and `createDefaultConfig()`
2. Use `createConfigPreset("minimal")` for basic testing
3. Enable learning subsystems gradually: start with reward modulation, then add curiosity and development
4. Use `verbose=True` in `runAgentEpisode()` to track progress

### For Advanced Users:
1. Use `createConfigPreset("advanced")` for complex simulations
2. Implement custom error handling with `tryCatch()`
3. Use `computeBrainStats()` and `computeAgentStats()` for detailed monitoring
4. Create custom configurations and save/load them
5. Implement performance monitoring with `getPerformanceStats()`

### For Developers:
1. Extend the Python bindings with custom classes
2. Create custom configuration presets
3. Implement custom validation and normalization functions
4. Add visualization and debugging tools
5. Contribute to the enhanced documentation

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md`** for more detailed information about building and installing
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works internally
3. **Experiment with different configuration presets**
4. **Create custom examples and share them**
5. **Explore the C++ source code** to understand the underlying implementation

You now have access to an enhanced NLM with better error handling, convenience functions, advanced configuration options, and comprehensive statistics! That's it! You're now ready to use the enhanced NLM with all its new features and improvements.

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
