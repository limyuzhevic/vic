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

## Advanced Configuration and Helper Functions

### Configuration Presets

```python
import pynlm

# Create brain with configuration preset
config = pynlm.createDefaultConfig()
config.apply_preset("debug")  # Smaller brain for debugging
config.apply_preset("high_performance")  # Larger brain for performance testing
config.apply_preset("default")  # Standard configuration

# Get all configuration as dictionary
config_dict = config.get_all_as_dict()
print("Configuration:", config_dict)
```

### Brain Configuration Helpers

```python
# Create specialized configurations
brain_config = pynlm.create_brain_config()
agent_config = pynlm.create_agent_config()
world_config = pynlm.create_world_config()

# Merge configurations
combined_config = pynlm.merge_configs([brain_config, agent_config, world_config])
```

## Performance Monitoring and Debugging

### Performance Monitoring

```python
import pynlm

# Create performance monitor
monitor = pynlm.createPerformanceMonitor()
monitor.start()

# Run simulation with monitoring
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(1000):
    brain.step(step)
    # Monitor would record step timing, spikes, etc.

monitor.stop()

# Get performance metrics
metrics = monitor.getCurrentMetrics()
print(f"Performance: {metrics.avgStepTimeMs:.2f} ms/step, "
      f"{metrics.totalMemoryMB} MB memory")
```

### Memory Profiling

```python
# Create memory profiler
profiler = pynlm.createMemoryProfiler()

# Run simulation while profiling
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Take memory snapshots
for step in range(100):
    brain.step(step)
    profiler.record()  # Record memory usage

print(f"Peak memory usage: {profiler.getPeakMemoryMB()} MB")
print(f"Current memory usage: {profiler.getCurrentMemoryMB()} MB")

# Get detailed breakdown
breakdown = profiler.getBreakdown()
print(f"Neuron memory: {breakdown.neuronsMB} MB")
print(f"Synapse memory: {breakdown.synapsesMB} MB")
```

### Metrics Collection

```python
# Create metrics collector
metrics_collector = pynlm.createMetricsCollector()

# Record performance metrics
metrics_collector.record("neuronsPerSecond", 1000.0)
metrics_collector.record("spikesPerSecond", 50000.0)
metrics_collector.record("stepTimeMs", 0.5)

# Get statistics
print(f"Average neurons/sec: {metrics_collector.getMean('neuronsPerSecond'):.2f}")
print(f"Min step time: {metrics_collector.getMin('stepTimeMs'):.3f} ms")
print(f"Max step time: {metrics_collector.getMax('stepTimeMs'):.3f} ms")

# Get all recorded metrics
metric_names = metrics_collector.getMetricNames()
for name in metric_names:
    values = metrics_collector.getValues(name)
    print(f"{name}: {len(values)} samples")
```

## Enhanced Brain API

### Pythonic Brain Operations

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make brain think
for i in range(100):
    brain.step(i)

# Get brain statistics as a dictionary (NEW!)
stats = brain.get_stats()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"Memory usage: {stats['development_stage']}")

# Check if brain is initialized
if brain.is_initialized():
    print("Brain is ready for simulation!")

# Brain status (convenience method)
brain.status()  # Prints brain status

# Save/load with convenience methods
brain.save_as_json("brain_state.json")
brain.load_from_json("brain_state.json")

# Reset with new configuration
brain = brain.reset_with_config(new_config)
```

### Advanced Visualization

```python
import pynlm

# Create visualization interface
viz = pynlm.createVisualization()
viz.initialize()
viz.setUpdateRate(30.0)  # 30 Hz update rate

# Run simulation with visualization
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(500):
    brain.step(step)
    viz.update()  # Update visualization data
    viz.render()  # Render current state

# Save visualization data
# (Note: Real visualization would save network, activity, and weight data)
print("Visualization completed")

viz.close()
```

## Complete Example: Simulation with Performance Monitoring

```python
import pynlm
import time

def run_monitored_simulation(steps=1000, preset="default"):
    """Run a complete simulation with performance monitoring and metrics collection"""
    
    # Create performance monitoring tools
    monitor = pynlm.createPerformanceMonitor()
    profiler = pynlm.createMemoryProfiler()
    metrics = pynlm.createMetricsCollector()
    
    # Configure simulation
    config = pynlm.createDefaultConfig()
    config.apply_preset(preset)
    
    # Create brain and visualization
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world for interactive simulation
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Start monitoring
    monitor.start()
    
    # Run simulation
    start_time = time.time()
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get perception and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain processes
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record metrics
        metrics.record("spikes", brain.getFiringNeuronCount())
        metrics.record("step_time_ms", 100.0)  # Approximate
        
        # Monitor records (in real implementation)
        monitor.recordStep(0.1, brain.getFiringNeuronCount(), brain.getActiveNeuronCount())
        
        # Memory profiling
        profiler.record()
        
        # Print progress
        if step % 100 == 0:
            stats = brain.get_stats()
            print(f"Step {step}: {stats['firing_neurons']} neurons firing, "
                  f"{stats['total_spikes']} total spikes")
    
    # Stop monitoring
    monitor.stop()
    end_time = time.time()
    
    # Get results
    final_metrics = monitor.getCurrentMetrics()
    peak_memory = profiler.getPeakMemoryMB()
    
    print(f"\n=== Simulation Complete ===")
    print(f"Duration: {end_time - start_time:.2f} seconds")
    print(f"Performance: {final_metrics.neuronsPerSecond:.0f} neurons/sec")
    print(f"Memory usage: {peak_memory} MB peak")
    print(f"Average step time: {final_metrics.avgStepTimeMs:.3f} ms")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    # Save results
    brain.save(f"simulation_results_step{steps}.json")
    
    # Generate report
    report = {
        "duration": end_time - start_time,
        "steps": steps,
        "performance_metrics": {
            "neurons_per_second": final_metrics.neuronsPerSecond,
            "memory_mb": peak_memory,
            "avg_step_time_ms": final_metrics.avgStepTimeMs,
            "total_spikes": brain.getTotalSpikeCount()
        }
    }
    
    return report

# Run the simulation
if __name__ == "__main__":
    results = run_monitored_simulation(steps=500, preset="debug")
    print("\nSimulation results:", results)
```

## Integration with Visualization

```python
import pynlm

# Complete integration example with visualization
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

viz = pynlm.createVisualization()
viz.initialize()
viz.setUpdateRate(10.0)

# Run interactive simulation
for step in range(200):
    # World update
    world.update(0.1)
    
    # Agent perception and action
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Visualization
    viz.update()
    viz.visualizeNetwork(brain)
    viz.visualizeActivity(brain)
    viz.render()
    
    # Control visualization
    if step % 50 == 0:
        print(f"Step {step}: Active neurons: {brain.getActiveNeuronCount()}")

viz.close()
print("Interactive visualization completed")
```

That's it! You're now ready to use NLM.
