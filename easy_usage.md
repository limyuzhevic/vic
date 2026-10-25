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

## New Features for Advanced Users (NEW!)

### Time Integration (Python 3.8+)

The NLM Python bindings now include full integration with Python's time module:

```python
import pynlm

# Get current time
start_time = pynlm.getTimestamp()
print(f"Start time: {start_time}")

# Run simulation and measure time
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)

elapsed = pynlm.elapsedTime(start_time)
print(f"Simulation took {elapsed:.3f} seconds")

# Format timestamps
formatted = pynlm.formatTime(start_time)
print(f"Formatted: {formatted}")
```

### Batch Processing

Process multiple neurons or operations at once:

```python
# Process multiple neurons with a callback
def process_neuron(neuron_id, potential):
    print(f"Neuron {neuron_id.value}: {potential} mV")

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Process first 10 neurons
neuron_ids = [pynlm.NeuronId(i) for i in range(10)]
pynlm.batchProcessNeurons(brain, neuron_ids, process_neuron)
```

### Comprehensive Statistics (NEW!)

Get detailed statistics in one call:

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

all_stats = pynlm.getComprehensiveStats(brain)

print("=== Basic Statistics ===")
print(f"Total neurons: {all_stats['basic']['total_neurons']}")
print(f"Total synapses: {all_stats['basic']['total_synapses']}")
print(f"Active neurons: {all_stats['basic']['active_neurons']}")
print(f"Firing neurons: {all_stats['basic']['firing_neurons']}")
print(f"Firing rate: {all_stats['basic']['average_firing_rate']:.2f} Hz")

print("\n=== Region Information ===")
for region in all_stats['regions']:
    print(f"Region {region['id']}: {region['neuron_count']} neurons")
```

### Region-Specific Statistics (NEW!)

Get detailed information about specific brain regions:

```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Add a custom region
region_id = brain.addRegion("cortex")
region_stats = pynlm.getRegionStatistics(brain, region_id)

print(f"Region {region_id.value} statistics:")
for key, value in region_stats.items():
    print(f"  {key}: {value}")
```

### Configuration and Benchmarking (NEW!)

Benchmark your brain performance:

```python
import time

brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Run benchmark
results = pynlm.benchmarkBrain(brain, 1000, 0.1)

print("=== Benchmark Results ===")
print(f"Steps performed: {results['steps_performed']}")
print(f"Time elapsed: {results['elapsed_time_ms']:.2f} ms")
print(f"Performance: {results['steps_per_second']:.0f} steps/second")
print(f"Neurons: {results['neurons']}")
print(f"Total spikes: {results['spikes']}")
```

### Agent Configuration (NEW!)

Configure all agent features at once:

```python
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent.initialize(world)

# Old way - individual calls:
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# New way - all at once:
pynlm.setAgentConfig(agent, 
                   rewardModulation=True,
                   structuralPlasticity=True,
                   development=True,
                   curiosity=True)
```

### Advanced Agent Features (NEW!)

Create sophisticated agents with all features enabled:

```python
import pynlm

def run_advanced_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with all features."""
    
    # 1. Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # 2. Enable ALL features at once (NEW!)
    pynlm.setAgentConfig(agent, 
                        rewardModulation=True,
                        curiosity=True,
                        development=True,
                        structuralPlasticity=True)
    
    # 3. Run simulation
    start_time = pynlm.getTimestamp()
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step, pynlm.getTimestamp())
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Optional: Get stats every 100 steps
        if step % 100 == 0:
            stats = pynlm.getComprehensiveStats(brain)
            print(f"Step {step}: {stats['basic']['firing_neurons']} firing neurons")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {agent.getDevelopmentalStage()}")
    
    # 4. Get final benchmark
    end_time = pynlm.getTimestamp()
    elapsed = end_time - start_time
    
    final_stats = pynlm.getComprehensiveStats(brain)
    
    print(f"\n=== Advanced Agent Simulation Complete ===")
    print(f"Duration: {elapsed:.2f} seconds")
    print(f"Total neurons: {final_stats['basic']['total_neurons']}")
    print(f"Total spikes: {final_stats['basic']['total_spikes']}")
    print(f"Final firing rate: {final_stats['basic']['average_firing_rate']:.2f} Hz")

# Run the simulation
run_advanced_agent_simulation(500)
```

---

## New Time Integration Features

### Basic Time Operations

```python
import pynlm

# Get current time
current_time = pynlm.getTimestamp()
print(f"Current time: {current_time} seconds")

# Get time in milliseconds
milliseconds = pynlm.getCurrentTime()
print(f"Current time: {milliseconds} ms")
```

### Time Formatting and Calculations

```python
# Format timestamp as readable string
formatted = pynlm.formatTime(current_time)
print(f"Formatted: {formatted}")

# Calculate elapsed time
start = pynlm.getTimestamp()
# ... do work ...
elapsed = pynlm.elapsedTime(start)
print(f"Work took {elapsed:.3f} seconds")

# Add time to timestamp
future_time = pynlm.addTime(current_time, 10.0)  # Add 10 seconds
print(f"Future time: {future_time}")

# Time difference
time_diff = pynlm.timeDiff(current_time, future_time)
print(f"Time difference: {time_diff} seconds")
```

### Simulation Timing

```python
# Simulate with precise timing
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate for 1 second of simulated time at 100x speed
for step in range(100):
    simulated_time = step * 0.1  # 0.1 seconds per step
    current_real_time = pynlm.getTimestamp()
    brain.step(step, current_real_time)

print("Timed simulation complete!")
```

---

## Quick Reference

### New Functions Added

| Function | Description | When to Use |
|----------|-------------|-------------|
| `pynlm.getTimestamp()` | Get current time in seconds | For timing simulations |
| `pynlm.getCurrentTime()` | Get current time in milliseconds | For high-precision timing |
| `pynlm.formatTime(timestamp)` | Format timestamp as string | For logging/output |
| `pynlm.elapsedTime(start_time)` | Calculate elapsed time | Performance measurement |
| `pynlm.addTime(timestamp, seconds)` | Add seconds to timestamp | Time arithmetic |
| `pynlm.timeDiff(time1, time2)` | Calculate time difference | Comparing timestamps |
| `pynlm.batchProcessNeurons(brain, neuron_ids, callback)` | Process multiple neurons | Batch operations |
| `pynlm.getRegionStatistics(brain, region_id)` | Get region-specific stats | Region analysis |
| `pynlm.getComprehensiveStats(brain)` | Get all statistics | Complete analysis |
| `pynlm.benchmarkBrain(brain, steps, timestep)` | Benchmark brain performance | Performance testing |
| `pynlm.setAgentConfig(agent, ...)` | Configure all agent features | Agent setup |
| `pynlm.runSimulation(...)` | Run complete agent simulation | Full workflow |
| `pynlm.configureStandardWorld(world, ...)` | Configure world with defaults | World setup |

### Enum Improvements

All enumeration types now have `__str__()` methods:

```python
import pynlm

# All enums now have string representations
print(pynlm.NeuronType.Excitatory)  # "Excitatory"
print(pynlm.DevelopmentalStage.CriticalPeriod)  # "CriticalPeriod"
print(pynlm.ActionType.MoveForward)  # "MoveForward"
```

### Class Improvements

All major classes now have `__repr__()` methods:

```python
import pynlm

# All classes have better string representations
brain = pynlm.createBrain(pynlm.createDefaultConfig())
print(repr(brain))  # Shows comprehensive info

world = pynlm.createSimpleWorld()
print(repr(world))  # Shows world dimensions and time

agent = pynlm.createAgentBrain(brain)
print(repr(agent))  # Shows agent state
```

---

## Troubleshooting New Features

**"Time functions not working"**
- Ensure you're using Python 3.8+
- All time functions are available with the updated bindings

**"New statistics functions throwing errors"**
- Check that brain objects are properly initialized
- Region IDs must be valid (from `brain.addRegion()` or `brain.getRegionIds()`)

**"Batch processing not processing all neurons"**
- Ensure neuron IDs are in the valid range (0 to total neurons - 1)
- Use `brain.getTotalNeuronCount()` to get the correct range

**"Benchmark results showing poor performance"**
- Ensure brain is initialized before benchmarking
- Check that timestep is appropriate for your use case

**"Agent configuration not applying settings"**
- Call `pynlm.setAgentConfig()` AFTER creating the agent
- The function modifies the agent in-place

**"Comprehensive stats missing regions"**
- Ensure brain has regions (regions are created during initialization)
- Use `brain.getRegionCount()` to check region count

---

## Best Practices

### For Time-Sensitive Simulations

```python
import pynlm
import time

def run_timed_simulation(brain, world, agent, target_duration):
    """Run simulation for a specific duration."""
    start_time = pynlm.getTimestamp()
    steps_completed = 0
    
    while True:
        current_time = pynlm.getTimestamp()
        elapsed = current_time - start_time
        
        if elapsed >= target_duration:
            break
            
        # Run one simulation step
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(steps_completed, current_time)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, current_time)
        
        steps_completed += 1
    
    return steps_completed, elapsed

# Use it
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

steps, duration = run_timed_simulation(brain, world, agent, 10.0)  # 10 seconds
print(f"Completed {steps} steps in {duration:.2f} seconds")
```

### For Large-Scale Analysis

```python
import pynlm

def analyze_brain_comprehensive(brain):
    """Perform comprehensive analysis of a brain."""
    stats = pynlm.getComprehensiveStats(brain)
    
    # Basic statistics
    print("=== Basic Brain Statistics ===")
    for key, value in stats['basic'].items():
        print(f"{key}: {value}")
    
    # Region analysis
    print(f"\n=== Region Analysis ({len(stats['regions'])} regions) ===")
    for region in stats['regions']:
        print(f"Region {region['id']}: {region['neuron_count']} neurons, "
              f"{region['firing_rate']:.2f} Hz firing rate")
    
    # Recommendations based on analysis
    if stats['basic']['average_firing_rate'] < 0.5:
        print("\n⚠️  Warning: Low average firing rate. Consider increasing brain activity.")
    
    if stats['basic']['excitation_inhibition_ratio'] < 0.5 or > 2.0:
        print("⚠️  Warning: E/I ratio may be unbalanced. Consider adjusting plasticity.")
    
    return stats

# Perform analysis
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
analysis = analyze_brain_comprehensive(brain)
```

### For Performance Testing

```python
import pynlm

def benchmark_multiple_configs():
    """Benchmark different brain configurations."""
    configs = [
        {"neurons": 100, "synapses": 0.1},
        {"neurons": 500, "synapses": 0.05},
        {"neurons": 1000, "synapses": 0.01},
    ]
    
    results = []
    
    for config in configs:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        # Configure brain
        brain_config = brain.getConfig()
        brain_config.set("brain.neuron_count", config["neurons"])
        brain_config.set("brain.synapse_density", config["synapses"])
        
        # Re-initialize with new config
        brain.reset()
        brain.initialize()
        
        # Benchmark
        benchmark_results = pynlm.benchmarkBrain(brain, 1000, 0.1)
        results.append({
            "config": config,
            "results": benchmark_results
        })
        
        print(f"Tested {config['neurons']} neurons: "
              f"{benchmark_results['steps_per_second']:.0f} steps/sec")
    
    return results

# Run benchmark
benchmark_results = benchmark_multiple_configs()
```

---

## Next Steps

### For Beginners

1. **Start with the basic examples** in this guide
2. **Experiment with the mini projects** to understand how everything works
3. **Read `HOW_TO_USE.md`** for more detailed information about individual features
4. **Try the troubleshooting section** if you encounter issues

### For Intermediate Users

1. **Explore the time integration features** for precise control over simulations
2. **Use batch processing** for efficient multiple operations
3. **Analyze brain statistics** to understand system behavior
4. **Create your own helper functions** using the new convenience methods

### For Advanced Users

1. **Implement custom callbacks** for batch processing
2. **Create benchmark suites** to compare different configurations
3. **Design complex agents** with all features enabled
4. **Extend the system** by creating new utility functions
5. **Contribute to the documentation** by adding your own examples

### For Researchers

1. **Use time features** for precise experiment timing
2. **Analyze brain statistics** for system monitoring
3. **Benchmark different architectures** for performance comparison
4. **Create custom analysis tools** using the comprehensive statistics
5. **Document your experimental protocols** using the new examples

---

## Summary

The NLM Python bindings have been significantly enhanced with:

1. **Improved class representations** with `__repr__()` methods
2. **Better enum handling** with `__str__()` methods
3. **Type validation and error handling** for more robust code
4. **Convenience methods** for common operations
5. **Time integration** for precise simulation control
6. **Advanced features** for power users
7. **Comprehensive statistics** for brain analysis
8. **Better documentation** with extensive examples

These improvements make NLM more accessible to beginners while providing powerful tools for advanced users and researchers.

That's it! You're now ready to use NLM with all the new features and improvements.
