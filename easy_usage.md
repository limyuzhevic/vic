# Advanced NLM Usage Guide

This guide covers advanced usage patterns, error handling, and complex scenarios when using NLM with Python.

## Error Handling and Best Practices

### Exception Handling

Python bindings now provide proper exception handling for error conditions:

```python
import pynlm
import traceback

# Create a brain
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Brain created successfully")
except Exception as e:
    print(f"Error creating brain: {e}")
    traceback.print_exc()

# Run simulation with error checking
try:
    for step in range(100):
        brain.step(step)
        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
except RuntimeError as e:
    print(f"Simulation error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Input Validation

```python
import pynlm

# Validate configuration values
config = pynlm.createDefaultConfig()

# Check if required keys exist
if config.has("brain.neuron_count"):
    neuron_count = config.get_int("brain.neuron_count")
    if neuron_count <= 0:
        raise ValueError("Neuron count must be positive")
    elif neuron_count > 10000:
        print("Warning: Large neuron count may impact performance")

# Validate world boundaries
world = pynlm.createSimpleWorld()
world.configure(100, 100, 20, 20)
world.setAgentStart(50, 50)

# Validate agent position is within world
if not world.isValidPosition(50, 50):
    raise ValueError("Agent start position is invalid")

# Validate sensory input
sensor_data = world.getSensoryPercept()
if not pynlm.Validator.validate_percept(sensor_data):
    print("Warning: Invalid sensory percept detected")
```

## Advanced Usage Patterns

### Context Manager Pattern

```python
import pynlm

# Use brain context manager for automatic cleanup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

with pynlm.brain_context(brain) as brain_ctx:
    brain_ctx.get().initialize()
    print(f"Brain initialized with {brain_ctx.get().getTotalNeuronCount()} neurons")
    
    # Run simulation
    for i in range(50):
        brain_ctx.get().step(i)
    
    print(f"Final spike count: {brain_ctx.get().getTotalSpikeCount()}")
    # Brain is automatically reset when exiting context

print("Context manager example completed")
```

### Simulation Runner Pattern

```python
import pynlm

# Create complete agent system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Configure world
world.configure(200, 200, 16, 16)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Use simulation runner for controlled execution
runner = pynlm.create_simulation(brain, world, agent)

if runner.start(1000, 0.05):  # Run 1000 steps
    print("Starting simulation...")
    step = 0
    while runner.is_running():
        if not runner.step():
            break
        step += 1
        
        if step % 100 == 0:
            stats = runner.get_stats()
            print(f"Step {step}: "
                  f"Neurons={stats['firing_neurons']}, "
                  f"Curiosity={stats['curiosity_level']:.3f}, "
                  f"Progress={runner.get_progress():.1%}")
    
    print(f"Simulation completed after {step} steps")
    print(f"Final stats: {runner.get_stats()}")
else:
    print("Failed to start simulation")
```

### Region-Based Processing

```python
import pynlm

# Create brain and add regions
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
brain = pynlm.createBrain(config)
brain.initialize()

# Add different brain regions
cortex_id = brain.addRegion("cortex")
hippocampus_id = brain.addRegion("hippocampus")
amygdala_id = brain.addRegion("amygdala")

# Use region iterator
print("Brain regions:")
for region in brain.getRegions():
    print(f"  {region.get_name()}: {region.get_id().value} neurons")

# Analyze region statistics
print("\nRegion analysis:")
total_neurons = 0
for region in brain.getRegions():
    neuron_count = region.get_neuron_count()
    total_neurons += neuron_count
    print(f"  {region.get_name()}: {neuron_count} neurons, "
          f"{neuron_count/total_neurons:.1%} of total")

# Filter regions by neuron count
large_regions = [r for r in brain.getRegions() if r.get_neuron_count() > 100]
print(f"\nRegions with >100 neurons: {len(large_regions)}")
```

### Advanced Agent Learning

```python
import pynlm
import random

# Create sophisticated agent with learning
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Configure a more complex world
world.configure(50, 50, 12, 12)
world.setAgentStart(25, 25)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Running advanced learning episode...")
for episode in range(5):
    print(f"\n--- Episode {episode + 1} ---")
    
    # Randomly place goals and obstacles
    for i in range(5):
        x = random.uniform(5, 45)
        y = random.uniform(5, 45)
        world.addObject(pynlm.WorldObject(x, y, 
                                          pynlm.WorldObjectType.Resource,
                                          value=1.0))
    
    for i in range(3):
        x = random.uniform(10, 40)
        y = random.uniform(10, 40)
        world.addObject(pynlm.WorldObject(x, y,
                                          pynlm.WorldObjectType.Hazard,
                                          value=-1.0))
    
    episode_steps = 0
    max_episode_steps = 200
    
    while episode_steps < max_episode_steps:
        world.update(0.05)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Record state for learning
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        prediction_error = agent.getPredictionError()
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward
        reward = 0.0
        if action.getType() == pynlm.ActionType.Eat:
            # Check if agent ate something
            reward += 1.0
        if action.getType() == pynlm.ActionType.Interact:
            # Check if agent interacted with object
            reward += 0.5
        
        # Apply neuromodulation with predictive learning
        predicted_reward = 0.0  # Could be from agent's internal model
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.05)
        
        # Track episode statistics
        episode_steps += 1
        if episode_steps % 20 == 0:
            print(f"  Step {episode_steps}: "
                  f"Curiosity={curiosity:.3f}, "
                  f"Novelty={novelty:.3f}, "
                  f"Reward={reward:.2f}")
        
        # End episode if agent develops too much
        if agent.getDevelopmentalStage() == pynlm.DevelopmentalStage.Aging:
            print("  Agent has reached aging stage, ending episode")
            break
    
    print(f"Episode ended after {episode_steps} steps")
    
    # Reset world for next episode
    world.reset()
    agent.reset()
```

### Performance Monitoring

```python
import pynlm
import time

# Create profiler for performance monitoring
profiler = pynlm.create_profiler()

# Create brain and enable learning
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation with profiling
profiler.start()

for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        profiler.stop()
        total_time = profiler.get_total_time()
        avg_time = profiler.get_average_time_per_step()
        max_time = profiler.get_max_time_per_step()
        
        print(f"Step {step}:")
        print(f"  Total time: {total_time:.3f}s")
        print(f"  Average per step: {avg_time:.6f}s")
        print(f"  Max per step: {max_time:.6f}s")
        print(f"  Steps per second: {1/avg_time:.1f}")
        
        profiler.start()

# Final profiling results
profiler.stop()
total_time = profiler.get_total_time()
avg_time = profiler.get_average_time_per_step()
steps_per_second = 1000 / total_time if total_time > 0 else 0

print(f"\nPerformance Summary:")
print(f"  Total time: {total_time:.3f}s")
print(f"  Average per step: {avg_time:.6f}s")
print(f"  Steps per second: {steps_per_second:.1f}")
print(f"  Memory efficiency: {brain.getTotalSynapseCount() / brain.getTotalNeuronCount():.3f} synapses/neuron")
```

### Configuration Management

```python
import pynlm
import json

# Advanced configuration management
config = pynlm.createDefaultConfig()

# Set multiple parameters efficiently
config.set({
    "brain.neuron_count": 5000,
    "brain.synapse_density": 0.2,
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True,
    "plasticity.structural.enable": False,
    "neuromod.dopamine.scale": 1.5,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True,
})

# Save configuration
config.saveToFile("my_config.json")

# Load configuration with error checking
try:
    loaded_config = pynlm.createDefaultConfig()
    loaded_config.loadFromFile("my_config.json")
    print("Configuration loaded successfully")
    
    # Validate loaded configuration
    if loaded_config.has("brain.neuron_count"):
        neurons = pynlm.get_int(loaded_config, "brain.neuron_count")
        if neurons < 1000:
            print("Warning: Small neuron count may affect learning quality")
        
except Exception as e:
    print(f"Failed to load configuration: {e}")

# Configuration comparison
def compare_configs(config1, config2, keys_to_check=None):
    """Compare two configurations and return differences"""
    if keys_to_check is None:
        keys_to_check = config1.getKeys()
        keys_to_check.extend(config2.getKeys())
    
    differences = {}
    for key in set(keys_to_check):
        val1 = pynlm.get_string(config1, key) if config1.has(key) else None
        val2 = pynlm.get_string(config2, key) if config2.has(key) else None
        if val1 != val2:
            differences[key] = {"old": val1, "new": val2}
    
    return differences

# Example usage
config_v1 = pynlm.createDefaultConfig()
config_v2 = pynlm.createDefaultConfig()
config_v2.set("brain.neuron_count", 2000)

diffs = compare_configs(config_v1, config_v2)
if diffs:
    print("Configuration differences:")
    for key, diff in diffs.items():
        print(f"  {key}: {diff['old']} -> {diff['new']}")
else:
    print("Configurations are identical")
```

### Event-Driven Processing

```python
import pynlm

# Event-driven simulation
class LearningEventHandler:
    def __init__(self):
        self.events_triggered = []
        self.performance_metrics = []
    
    def handle_step_event(self, event):
        step = event.get_step()
        time = event.get_time()
        
        # Record milestone events
        if step % 100 == 0:
            self.events_triggered.append(f"Step {step} reached")
        
        # Track performance
        self.performance_metrics.append({
            'step': step,
            'time': time,
            'event_type': 'step_completed'
        })
    
    def get_summary(self):
        return {
            'total_events': len(self.events_triggered),
            'milestones': self.events_triggered,
            'performance': self.performance_metrics
        }

# Create event system
event_system = pynlm.create_event_system()
handler = LearningEventHandler()

# Register event handler
event_system.add_event_handler(handler.handle_step_event)

# Run simulation with event handling
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(500):
    brain.step(step)
    
    # Create and dispatch step event
    step_event = pynlm.StepEvent(step, step * 0.1)
    event_system.dispatch_event(step_event)

# Get event summary
summary = handler.get_summary()
print(f"Event summary: {summary}")
```

### Batch Processing

```python
import pynlm
import multiprocessing as mp
from concurrent.futures import ThreadPoolExecutor

# Batch processing for multiple brains

def run_single_brain(brain_id, config_params, num_steps):
    """Run a single brain simulation"""
    config = pynlm.createDefaultConfig()
    for key, value in config_params.items():
        config.set(key, value)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(num_steps):
        brain.step(step)
    
    return {
        'brain_id': brain_id,
        'final_spikes': brain.getTotalSpikeCount(),
        'final_neurons': brain.getFiringNeuronCount(),
        'average_rate': brain.getAverageFiringRate()
    }

# Parallel execution
if __name__ == "__main__":
    # Configuration variations
    configs = [
        {"brain.neuron_count": 1000, "brain.synapse_density": 0.1},
        {"brain.neuron_count": 2000, "brain.synapse_density": 0.2},
        {"brain.neuron_count": 3000, "brain.synapse_density": 0.15},
    ]
    
    # Run simulations in parallel
    with ThreadPoolExecutor(max_workers=3) as executor:
        futures = []
        for i, config_params in enumerate(configs):
            future = executor.submit(run_single_brain, i, config_params, 200)
            futures.append(future)
        
        # Collect results
        results = []
        for future in futures:
            results.append(future.result())
    
    # Analyze results
    print("Batch processing results:")
    for result in results:
        print(f"  Brain {result['brain_id']}:")
        print(f"    Final spikes: {result['final_spikes']}")
        print(f"    Final firing neurons: {result['final_neurons']}")
        print(f"    Average firing rate: {result['average_rate']:.3f}")
```

## Performance Optimization Tips

### Memory Management

```python
import pynlm
import gc

# Efficient memory usage
config = pynlm.createDefaultConfig()

# Process in batches to manage memory
batch_size = 500
total_steps = 5000

for batch_start in range(0, total_steps, batch_size):
    batch_end = min(batch_start + batch_size, total_steps)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(batch_start, batch_end):
        brain.step(step)
    
    # Print batch statistics
    stats = {
        'batch': f"{batch_start}-{batch_end}",
        'spikes': brain.getTotalSpikeCount(),
        'neurons': brain.getTotalNeuronCount(),
        'rate': brain.getAverageFiringRate()
    }
    print(f"Batch {stats['batch']}: {stats['rate']:.3f} avg firing rate")
    
    # Clean up memory
    del brain
    gc.collect()

print("Batch processing completed")
```

### Progress Tracking

```python
import pynlm
import time

# Progress tracking for long simulations
total_steps = 10000
progress_tracker = pynlm.create_progress_tracker(total_steps)

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

start_time = time.time()
for step in range(total_steps):
    brain.step(step)
    
    progress_tracker.update(step + 1)
    
    if (step + 1) % 1000 == 0:
        elapsed = time.time() - start_time
        steps_per_second = (step + 1) / elapsed
        remaining = (total_steps - step - 1) / steps_per_second if steps_per_second > 0 else 0
        
        print(f"Progress: {progress_tracker.get_percentage():.1%}")
        print(f"  Elapsed: {elapsed:.1f}s")
        print(f"  Remaining: {remaining:.1f}s")
        print(f"  Speed: {steps_per_second:.1f} steps/s")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")

print(f"Simulation completed in {time.time() - start_time:.1f}s")
```

## Pythonic APIs Summary

### Key Improvements:

1. **Context Managers**: `brain_context()` for automatic resource cleanup
2. **Iterators**: `regions_begin()`/`regions_end()` and `neuron_ids_begin()`/`neuron_ids_end()`
3. **Simulation Runner**: `create_simulation()` for controlled execution
4. **Validation Helpers**: `Validator` class for input validation
5. **Utility Functions**: `sum()`, `average()`, `is_even()`, `is_odd()`, `format_stats()`
6. **Configuration Helpers**: `get_float()`, `get_int()`, `get_bool()`, `get_string()`
7. **Event System**: `create_event_system()` for event-driven processing
8. **Progress Tracking**: `create_progress_tracker()` for long simulations
9. **Profiling**: `create_profiler()` for performance monitoring

### Advanced Examples:

- Complete agent learning with curiosity and development
- Event-driven simulation with custom handlers
- Batch processing with parallel execution
- Performance monitoring with detailed statistics
- Configuration management with validation
- Progress tracking for long simulations

These improvements make NLM more Pythonic, easier to use, and better suited for complex research scenarios while maintaining full compatibility with existing code.
