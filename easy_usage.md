# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

## Context Manager Usage (Recommended)

NLM's improved Python API now supports context managers for automatic resource cleanup. This is the recommended approach:

```python
import pynlm

# Context manager usage with automatic cleanup
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    # Your code here - automatic cleanup on exit
    for i in range(100):
        brain.step(i)
    print(f"Total spikes: {brain.getTotalSpikeCount()}")

# The brain is automatically reset and resources cleaned up here
```

### Context Manager with World and Agent

```python
import pynlm

# Full simulation with context managers
with pynlm.createSimpleWorld() as world:
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
        brain.initialize()
        
        with pynlm.createAgentBrain(brain) as agent:
            agent.initialize(world)
            
            # Run simulation
            for step in range(50):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
```

---

### Context Manager Usage (Recommended)

NLM's improved Python API now supports context managers for automatic resource cleanup. This is the recommended approach:

```python
import pynlm

# Context manager usage with automatic cleanup
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    # Your code here - automatic cleanup on exit
    for i in range(100):
        brain.step(i)
    print(f"Total spikes: {brain.getTotalSpikeCount()}")

# The brain is automatically reset and resources cleaned up here
```

### Context Manager with World and Agent

```python
import pynlm

# Full simulation with context managers
with pynlm.createSimpleWorld() as world:
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
        brain.initialize()
        
        with pynlm.createAgentBrain(brain) as agent:
            agent.initialize(world)
            
            # Run simulation
            for step in range(50):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
```

---

## Error Handling with Context Managers

```python
import pynlm

# Error handling examples
try:
    with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
        brain.initialize()
        brain.step(0)
        print("Simulation completed successfully")
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")

# Using context manager for resource management
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

try:
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        for i in range(100):
            brain.step(i)
finally:
    # Resources automatically cleaned up
    print("Resources cleaned up")
```

---

## Performance Optimization Examples

```python
import pynlm
import time
import psutil
import os

# Performance benchmarking
process = psutil.Process(os.getpid())

# Example 1: Large-scale simulation
print("Running large-scale simulation...")
start_time = time.time()
start_memory = process.memory_info().rss / 1024 / 1024

with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    for i in range(10000):
        brain.step(i)

end_time = time.time()
end_memory = process.memory_info().rss / 1024 / 1024

print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Memory usage: {end_memory - start_memory:.2f} MB")
print(f"Average spikes per second: {brain.getTotalSpikeCount() / (end_time - start_time):.2f}")

# Example 2: Parallel processing simulation
print("\nRunning parallel processing simulation...")
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    # Enable brain features for faster processing
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=5, height=5, visionWidth=3, visionHeight=3)
    world.reset()
    agent.initialize(world)
    
    # Process multiple steps efficiently
    import concurrent.futures
    
    def process_step(step):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        return agent.decodeMotorCommand()
    
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
        futures = [executor.submit(process_step, i) for i in range(20)]
        for future in concurrent.futures.as_completed(futures):
            action = future.result()
            world.applyMotorCommand(action, world.getSimulationTime())
```

---

## Convenience Methods Usage

```python
import pynlm

# Using convenience methods from the improved API
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    
    # Convenience methods
    brain.logStatus()  # Log brain status
    brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 0.5)  # Inject current to specific neurons
    brain.injectCurrentToNeurons(pynlm.NeuronType.Inhibitory, 0.3)   # Inject current to inhibitory neurons
    
    # Get statistics
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
```

---

## Advanced Usage with New Python Features

```python
import pynlm

# Using type hints for better code documentation
def run_simulation_with_brain(config):
    """Run a simulation with proper type hints."""
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        
        # Get brain statistics with return type hints
        total_neurons = brain.getTotalNeuronCount()
        total_synapses = brain.getTotalSynapseCount()
        firing_rate = brain.getAverageFiringRate()
        
        for step in range(100):
            brain.step(step)
            
        return {
            'total_neurons': total_neurons,
            'total_synapses': total_synapses,
            'firing_rate': firing_rate,
            'total_spikes': brain.getTotalSpikeCount()
        }

# Using region-based brain organization
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    
    # Create regions for organizing neurons
    cortex_region = brain.addRegion("cortex")
    hippocampus_region = brain.addRegion("hippocampus")
    
    # Get regions
    region = brain.getRegion(cortex_region)
    print(f"Created {brain.getRegionCount()} regions")
    
    # Simulation
    for step in range(100):
        brain.step(step)
```

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

## Advanced Usage with New Python Features

```python
import pynlm

# Using type hints for better code documentation
def run_simulation_with_brain(config):
    """Run a simulation with proper type hints."""
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        
        # Get brain statistics with return type hints
        total_neurons = brain.getTotalNeuronCount()
        total_synapses = brain.getTotalSynapseCount()
        firing_rate = brain.getAverageFiringRate()
        
        for step in range(100):
            brain.step(step)
            
        return {
            'total_neurons': total_neurons,
            'total_synapses': total_synapses,
            'firing_rate': firing_rate,
            'total_spikes': brain.getTotalSpikeCount()
        }

# Using region-based brain organization
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    
    # Create regions for organizing neurons
    cortex_region = brain.addRegion("cortex")
    hippocampus_region = brain.addRegion("hippocampus")
    
    # Get regions
    region = brain.getRegion(cortex_region)
    print(f"Created {brain.getRegionCount()} regions")
    
    # Simulation
    for step in range(100):
        brain.step(step)
```

---

## Performance Optimization Examples

```python
import pynlm
import time
import psutil
import os

# Performance benchmarking
process = psutil.Process(os.getpid())

# Example 1: Large-scale simulation
print("Running large-scale simulation...")
start_time = time.time()
start_memory = process.memory_info().rss / 1024 / 1024

with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    for i in range(10000):
        brain.step(i)

end_time = time.time()
end_memory = process.memory_info().rss / 1024 / 1024

print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Memory usage: {end_memory - start_memory:.2f} MB")
print(f"Average spikes per second: {brain.getTotalSpikeCount() / (end_time - start_time):.2f}")

# Example 2: Parallel processing simulation
print("\nRunning parallel processing simulation...")
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    # Enable brain features for faster processing
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=5, height=5, visionWidth=3, visionHeight=3)
    world.reset()
    agent.initialize(world)
    
    # Process multiple steps efficiently
    import concurrent.futures
    
    def process_step(step):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        return agent.decodeMotorCommand()
    
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
        futures = [executor.submit(process_step, i) for i in range(20)]
        for future in concurrent.futures.as_completed(futures):
            action = future.result()
            world.applyMotorCommand(action, world.getSimulationTime())
```

---

## Convenience Methods Usage

```python
import pynlm

# Using convenience methods from the improved API
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    
    # Convenience methods
    brain.logStatus()  # Log brain status
    brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 0.5)  # Inject current to specific neurons
    brain.injectCurrentToNeurons(pynlm.NeuronType.Inhibitory, 0.3)   # Inject current to inhibitory neurons
    
    # Get statistics
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
```

## Advanced Usage with New Python Features

```python
import pynlm

# Using type hints for better code documentation
def run_simulation_with_brain(config):
    """Run a simulation with proper type hints."""
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        
        # Get brain statistics with return type hints
        total_neurons = brain.getTotalNeuronCount()
        total_synapses = brain.getTotalSynapseCount()
        firing_rate = brain.getAverageFiringRate()
        
        for step in range(100):
            brain.step(step)
            
        return {
            'total_neurons': total_neurons,
            'total_synapses': total_synapses,
            'firing_rate': firing_rate,
            'total_spikes': brain.getTotalSpikeCount()
        }

# Using region-based brain organization
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    
    # Create regions for organizing neurons
    cortex_region = brain.addRegion("cortex")
    hippocampus_region = brain.addRegion("hippocampus")
    
    # Get regions
    region = brain.getRegion(cortex_region)
    print(f"Created {brain.getRegionCount()} regions")
    
    # Simulation
    for step in range(100):
        brain.step(step)
```

---

## Performance Optimization Examples

```python
import pynlm
import time
import psutil
import os

# Performance benchmarking
process = psutil.Process(os.getpid())

# Example 1: Large-scale simulation
print("Running large-scale simulation...")
start_time = time.time()
start_memory = process.memory_info().rss / 1024 / 1024

with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    for i in range(10000):
        brain.step(i)

end_time = time.time()
end_memory = process.memory_info().rss / 1024 / 1024

print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Memory usage: {end_memory - start_memory:.2f} MB")
print(f"Average spikes per second: {brain.getTotalSpikeCount() / (end_time - start_time):.2f}")

# Example 2: Parallel processing simulation
print("\nRunning parallel processing simulation...")
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    # Enable brain features for faster processing
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=5, height=5, visionWidth=3, visionHeight=3)
    world.reset()
    agent.initialize(world)
    
    # Process multiple steps efficiently
    import concurrent.futures
    
    def process_step(step):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        return agent.decodeMotorCommand()
    
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
        futures = [executor.submit(process_step, i) for i in range(20)]
        for future in concurrent.futures.as_completed(futures):
            action = future.result()
            world.applyMotorCommand(action, world.getSimulationTime())
```

---

## Convenience Methods Usage

```python
import pynlm

# Using convenience methods from the improved API
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    
    # Convenience methods
    brain.logStatus()  # Log brain status
    brain.injectCurrentToNeurons(pynlm.NeuronType.Excitatory, 0.5)  # Inject current to specific neurons
    brain.injectCurrentToNeurons(pynlm.NeuronType.Inhibitory, 0.3)   # Inject current to inhibitory neurons
    
    # Get statistics
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
```

---

## Error Handling

```python
import pynlm

# Exception handling examples
try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brain.step(0)
    print("Simulation completed successfully")
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")

# Best practices for error handling
config = pynlm.createDefaultConfig()

try:
    with pynlm.createBrain(config) as brain:
        brain.initialize()
        for i in range(100):
            brain.step(i)
except pynlm.RuntimeError as runtime_error:
    print(f"Brain runtime error: {runtime_error}")
except pynlm.ValueError as value_error:
    print(f"Invalid parameter: {value_error}")
except pynlm.KeyError as key_error:
    print(f"Missing configuration key: {key_error}")
except Exception as general_error:
    print(f"General error: {general_error}")
finally:
    # Resources automatically cleaned up
    print("Resources cleaned up")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `with pynlm.createBrain(config) as brain:` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `with pynlm.createSimpleWorld() as world:` |
| Create agent | `with pynlm.createAgentBrain(brain) as agent:` |
| Get brain stats | `brain.getTotalNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Inject current | `brain.injectCurrentToNeurons(type, current)` |
| Log status | `brain.logStatus()` |
| Get development | `brain.getDevelopmentalStage()` |
| Add region | `brain.addRegion(name)` |

---

## Next Steps

When you're comfortable:

1. Read `HOW_TO_USE.md` for more detailed information about all methods
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Check `advanced_examples.md` for advanced usage patterns
4. Experiment with different configurations and performance optimizations!

That's it! You're now ready to use the improved NLM Python API with context managers and modern Python features.
