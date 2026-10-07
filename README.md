# NLM Python Bindings - Improved API Documentation

## Overview
This document provides comprehensive documentation for the improved NLM Python bindings, including new Pythonic features, advanced user capabilities, and best practices.

## Quick Start (Beginner)

### Basic Usage
```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

## New Pythonic Features

### 1. Property Access

All classes now provide Pythonic property access for better encapsulation:

```python
# Instead of direct attribute access
world_object = pynlm.WorldObject()
world_object.setX(10.0)
world_object.setY(20.0)

# Use properties
world_object.x = 10.0
world_object.y = 20.0

# Read-only properties
print(world_object.radius)  # Instead of world_object.getRadius()
print(world_object.type)   # Instead of world_object.getType()
```

### 2. Context Managers

All resource-managing classes now support context managers for automatic cleanup:

```python
# Automatic cleanup
with pynlm.createBrain(config) as brain:
    brain.initialize()
    # Run simulation...
    print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")

# Context manager usage
with pynlm.createSimpleWorld() as world:
    world.configure(20, 20, 8, 8)
    world.reset()
    # Simulate...
    print(f"World time: {world.getSimulationTime()}")
```

### 3. Custom Exceptions

The bindings now provide a comprehensive exception hierarchy:

```python
import pynlm

try:
    # Configuration validation
    config = pynlm.createDefaultConfig()
    config.set("invalid.key", "value")  # May raise ConfigError
    config.validate()  # Validates configuration
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
except pynlm.ResourceError as e:
    print(f"Resource error: {e}")
```

### 4. Type Conversion Utilities

```python
# Convert between different representations
neuron_type = pynlm.convertNeuronType("excitatory")  # "excitatory" -> NeuronType.Excitatory
brain_config = pynlm.normalizeConfig({"neuron_count": 1000, "connection_probability": 0.1})
formatted_id = pynlm.formatId(pynlm.NeuronId(42))  # Format ID for display
```

## Advanced User Features

### 1. Batch Operations

Perform multiple operations efficiently:

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Batch current injection
neurons_to_stimulate = [neuron_id1, neuron_id2, neuron_id3]
brain.batchInjectCurrent(neurons_to_stimulate, 50.0)

# Batch parameter setting
brain.batchSetParameters({
    "brain.region_count": 3,
    "brain.connection_probability": 0.15,
    "plasticity.stdp.enable": True,
    "neuromod.dopamine.scale": 1.5
})

# Batch region operations
region_ids = brain.getRegionIds()
for region_id in region_ids:
    region = brain.getRegion(region_id)
    print(f"Region {region_id}: {region.getTotalNeuronCount()} neurons")
```

### 2. Performance Optimization

```python
brain = pynlm.createBrain(config)
brain.initialize()

# Performance benchmarking
stats = brain.benchmarkSimulation(1000)
print(f"Simulation performance:")
print(f"  Average timestep: {stats['avg_timestep_ms']:.2f} ms")
print(f"  Total spikes/sec: {stats['spikes_per_sec']:.0f}")
print(f"  Memory usage: {stats['memory_mb']:.1f} MB")

# Activity profiling
profile = brain.profileActivity(500)
print(f"Neural activity profile:")
print(f"  Average firing rate: {profile['avg_firing_rate']:.2f} Hz")
print(f"  Peak activity: {profile['peak_activity']}")

# Connection optimization
optimized = brain.optimizeConnections()
print(f"Connection optimization improved efficiency by {optimized['efficiency_gain']:.1%}")
```

### 3. Parallel Processing Utilities

```python
import concurrent.futures

# Parallel simulation steps
def run_simulation_step(step):
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(step)
    return brain.getTotalSpikeCount()

with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
    steps = range(100, 200)
    results = list(executor.map(run_simulation_step, steps))

print(f"Parallel simulation completed. Total spikes: {sum(results)}")

# Parallel sensory processing
def process_sensory_batch(percepts):
    for percept in percepts:
        agent.processSensoryInput(percept)

# Process multiple perceptual inputs in parallel
percept_batches = [batch1, batch2, batch3]
with concurrent.futures.ThreadPoolExecutor(max_workers=8) as executor:
    executor.map(process_sensory_batch, percept_batches)
```

### 4. Advanced Configuration Validation

```python
# Configuration validation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.2)

# Validate configuration
validation_result = config.validate()
if not validation_result.is_valid:
    print("Configuration validation errors:")
    for error in validation_result.errors:
        print(f"  - {error}")
    
    print("\nSuggested fixes:")
    for fix in validation_result.suggestions:
        print(f"  - {fix}")

# Compare with recommended template
template = config.getRecommendedTemplate()
differences = config.compareWithTemplate(template)
print(f"Configuration differences from template: {differences}")
```

## Complete Agent Example with Advanced Features

```python
import pynlm
import time

def advanced_agent_simulation(num_steps=1000):
    """Advanced agent simulation with all new features."""
    
    # 1. Create and configure brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.region_count", 2)
    config.set("brain.connection_probability", 0.1)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent with context manager support
    with pynlm.createAgentBrain(brain) as agent:
        # Enable advanced features
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # 3. Create and configure world
        with pynlm.createSimpleWorld() as world:
            world.configure(20, 20, 8, 8)
            world.reset()
            world.setAgentStart(10.0, 10.0)
            agent.initialize(world)
            
            # 4. Simulation statistics
            simulation_stats = {
                'start_time': time.time(),
                'steps_completed': 0,
                'total_spikes': 0,
                'episodes_completed': 0,
                'behavior_metrics': {}
            }
            
            # 5. Run simulation with advanced features
            for step in range(num_steps):
                # Update world
                world.update(0.1)
                
                # Get and process sensory input
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Run brain step with performance monitoring
                step_start = time.time()
                brain.step(step)
                step_time = time.time() - step_start
                
                # Decode motor command
                action = agent.decodeMotorCommand()
                
                # Apply action to world
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward modulation with prediction error
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, agent.getPredictionError())
                
                # Update development
                agent.updateDevelopment(0.1)
                
                # Collect statistics every 100 steps
                if step % 100 == 0:
                    simulation_stats['steps_completed'] += 100
                    simulation_stats['total_spikes'] += brain.getTotalSpikeCount()
                    
                    # Performance metrics
                    simulation_stats['behavior_metrics'][step] = {
                        'step_time_ms': step_time * 1000,
                        'firing_rate': brain.getAverageFiringRate(),
                        'curiosity': agent.getCuriosityLevel(),
                        'novelty': agent.getNoveltyLevel(),
                        'development_stage': agent.getDevelopmentalStage().name
                    }
                    
                    # Print progress with enhanced metrics
                    print(f"Step {step}:")
                    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
                    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
                    print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
                    print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
                    print(f"  Development stage: {brain.getDevelopmentalStage().name}")
                    print(f"  Step time: {step_time*1000:.1f} ms")
                    print()
            
            # 6. Post-simulation analysis and optimization
            final_analysis = {
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                'development_stage': agent.getDevelopmentalStage().name,
                'neuromodulation_level': agent.getNeuromodulationLevel(),
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel(),
                'simulation_efficiency': num_steps / (time.time() - simulation_stats['start_time'])
            }
            
            print("\n=== Advanced Agent Simulation Results ===")
            for key, value in final_analysis.items():
                print(f"{key.replace('_', ' ').title()}: {value}")
            
            # 7. Save optimized configuration
            optimized_config = brain.getConfig()
            optimized_config.saveToFile("optimized_config.json")
            
            return final_analysis

# Run the advanced simulation
results = advanced_agent_simulation(1000)
print("\nSimulation completed successfully!")
```

## Best Practices

### 1. Resource Management
```python
# Always use context managers for resource management
with pynlm.createBrain(config) as brain:
    brain.initialize()
    # Use brain...

# Or manually manage resources
brain = pynlm.createBrain(config)
try:
    brain.initialize()
    # Use brain...
finally:
    brain.reset()  # or explicit cleanup if available
```

### 2. Configuration Management
```python
# Use validated configurations
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 2)

# Always validate important configurations
if not config.validate().is_valid:
    raise ValueError("Invalid configuration")

# Save configurations for reproducibility
config.saveToFile("production_config.json")
```

### 3. Error Handling
```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(0)
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
except pynlm.ResourceError as e:
    print(f"Resource error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
    # Consider logging the error for debugging
```

### 4. Performance Optimization
```python
# Use batch operations for efficiency
brain.batchInjectCurrent([1, 2, 3, 4, 5], 50.0)

# Use parallel processing for independent operations
import concurrent.futures

def process_batch(batch):
    # Process batch of operations
    pass

with concurrent.futures.ThreadPoolExecutor() as executor:
    batches = [batch1, batch2, batch3]
    executor.map(process_batch, batches)

# Profile and optimize performance
brain.benchmarkSimulation(1000)
brain.profileActivity(500)
```

## API Reference

### Core Classes

#### Config
- `createDefaultConfig()` - Create default configuration
- `Config.loadFromFile(filepath)` - Load from JSON
- `Config.validate()` - Validate configuration
- `Config.getRecommendedTemplate()` - Get recommended template

#### Brain
- `Brain.initialize()` - Initialize brain
- `Brain.step(step, time=None)` - Run simulation step
- `Brain.batchInjectCurrent(neuron_ids, current)` - Batch current injection
- `Brain.benchmarkSimulation(steps)` - Performance benchmarking
- `Brain.profileActivity(steps)` - Activity profiling
- `Brain.optimizeConnections()` - Optimize synaptic connections

#### AgentBrain
- `AgentBrain.initialize(world)` - Initialize agent
- `AgentBrain.processSensoryInput(percept)` - Process sensory input
- `AgentBrain.decodeMotorCommand()` - Decode motor action
- `AgentBrain.applyRewardModulation(reward, predicted)` - Reward modulation

#### SimpleWorld
- `SimpleWorld.configure(width, height, vision_width, vision_height)` - Configure world
- `SimpleWorld.update(timestep)` - Update world
nlm.end()
```

## Comparison with Original API

| Feature | Original API | Improved API |
|---------|-------------|--------------|
| Property Access | `world_object.getX()` | `world_object.x` |
| Context Management | Manual cleanup | `with brain:` |
| Error Handling | Basic `RuntimeError` | Custom exception hierarchy |
| Batch Operations | Individual calls | `batchInjectCurrent()`, `batchSetParameters()` |
| Performance | No tools | `benchmarkSimulation()`, `profileActivity()` |
| Configuration Validation | No validation | `Config.validate()`, `Config.compareWithTemplate()` |
| Parallel Processing | Sequential only | Thread pool utilities |

## Migration Guide

### From Original to Improved API

1. **Property Access**: Replace getter/setter methods with direct attribute access
2. **Error Handling**: Replace try-except blocks with specific exception types
3. **Resource Management**: Add context managers for automatic cleanup
4. **Performance**: Use batch operations and benchmarking tools
5. **Configuration**: Add validation and template comparison

### Example Migration

```python
# Before (Original API)
world_object = pynlm.WorldObject()
world_object.setX(10.0)
world_object.setY(20.0)
distance = world_object.getDistance()

# After (Improved API)
with pynlm.createSimpleWorld() as world:
    world.configure(20, 20, 8, 8)
    world.setAgentStart(10.0, 20.0)
    
    agent_body = world.getAgentBody()
    agent_body.x = 10.0  # Property access
    agent_body.y = 20.0  # Property access
    
    distance = ((agent_body.x) ** 2 + (agent_body.y) ** 2) ** 0.5  # Direct calculation
```

## Frequently Asked Questions

### Q: How do I migrate my existing code?

A: The improved API maintains backward compatibility. You can gradually migrate:
1. Replace getter/setter calls with property access
2. Add context managers for resource management
3. Use batch operations for performance
4. Implement proper error handling with custom exceptions

### Q: What are the performance implications?

A: The improvements provide:
- Property access: Minimal overhead (direct field access)
- Context managers: Automatic resource cleanup (prevents resource leaks)
- Batch operations: Significant performance improvements for bulk operations
- Benchmarking: Tools to identify and optimize bottlenecks

### Q: How do I get started with advanced features?

A: Begin with the quick start examples, then gradually add:
1. Context managers for resource management
2. Configuration validation
3. Batch operations
4. Performance optimization
5. Parallel processing

## License

MIT License

Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.