# Enhanced NLM Python API - User Documentation

This document provides comprehensive information about the enhanced Python API for NLM (Neural Learning Machine). The API has been significantly enhanced to provide a more user-friendly experience while maintaining full backward compatibility.

## Quick Start

### Installation

The enhanced Python API requires building from source. Follow these steps:

```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Basic Usage

```python
import pynlm

# Create a research configuration
config = pynlm.createResearchConfig()

# Create and initialize brain with context manager
with pynlm.createBrain(config) as brain:
    # Brain is automatically initialized
    for step in range(100):
        brain.step(step)
    
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

## Enhanced Features

### 1. Context Managers for Resource Management

The enhanced API provides automatic resource management using Python context managers:

```python
# Brain context manager
with pynlm.createBrain(config) as brain:
    brain.initialize()  # Auto-initialized
    # Use brain...
    # Automatically cleaned up

# AgentBrain context manager
with pynlm.createAgentBrain(brain) as agent:
    agent.initialize(world)  # Auto-initialized
    # Use agent...
    # Automatically cleaned up

# SimpleWorld context manager
with pynlm.createSimpleWorld() as world:
    world.reset()  # Auto-reset
    # Use world...
    # Automatically cleaned up
```

**Benefits:**
- Automatic initialization and cleanup
- Prevents resource leaks
- Exception-safe
- More Pythonic

### 2. Convenience Methods

#### Brain Convenience Methods
```python
brain = pynlm.createBrain(config)
brain.initialize()

# Run multiple steps efficiently
brain.runSteps(50)  # Run 50 steps
brain.runStepsWithTime(50, 0.1)  # Run with time tracking

# Get brain summary
summary = brain.getBrainSummary()
print(summary)

# Get comprehensive statistics
stats = brain.getFiringStatistics()
print(f"Activity level: {stats['firing_percentage']:.1f}%")
```

#### AgentBrain Convenience Methods
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run complete episode
agent.runEpisode(world, 100)

# Get agent status
status = agent.getAgentStatus(world)
print(status)
```

#### SimpleWorld Convenience Methods
```python
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

# Create objects easily
resource = world.createResource(5, 5, 10.0)
hazard = world.createHazard(10, 10, 15.0)
wall = world.createWall(15, 15, 1.0)
marker = world.createMarker(25, 25)

# Get object statistics
print(f"Resources: {world.getObjectCount(pynlm.WorldObjectType.Resource)}")
print(f"Hazards: {world.getObjectCount(pynlm.WorldObjectType.Hazard)}")
print(f"Walls: {world.getObjectCount(pynlm.WorldObjectType.Wall)}")
```

### 3. Configuration Presets

Four specialized configuration presets for different use cases:

```python
# Research Configuration (enhanced learning)
config = pynlm.createResearchConfig()

# Development Configuration (training focus)
config = pynlm.createDevelopmentConfig()

# Testing Configuration (stable, minimal)
config = pynlm.createTestingConfig()

# Production Configuration (stable, predictable)
config = pynlm.createProductionConfig()

# Default Configuration (backward compatibility)
config = pynlm.createDefaultConfig()
```

**Configuration Summary:**

| Preset | Neurons | Synapse Density | Plasticity | Neuromodulation | Use Case |
|--------|---------|----------------|-----------|----------------|----------|
| Research | 2000 | 0.15 | All enabled | Enhanced | Advanced research |
| Development | 1500 | 0.12 | All enabled | Moderate | Training and learning |
| Testing | 100 | 0.1 | All disabled | Disabled | Unit testing |
| Production | 1000 | 0.1 | STDP enabled | Curiosity disabled | Long-running sims |

### 4. High-Level Simulation Helpers

#### Simple Brain Example
```python
# Run a simple brain simulation and get results
total_spikes = pynlm.run_simple_brain_example(100)
print(f"Total spikes: {total_spikes}")
```

#### Complete Agent Simulation
```python
# Run a complete agent simulation
results = pynlm.run_agent_simulation(500, "research")
total_spikes, avg_firing_rate, curiosity = results

print(f"Simulation results:")
print(f"  - Total spikes: {total_spikes}")
print(f"  - Average firing rate: {avg_firing_rate:.2f} Hz")
print(f"  - Final curiosity level: {curiosity:.3f}")
```

#### Performance Benchmarking
```python
# Benchmark brain performance
performance = pynlm.benchmark_brain_performance(brain, 1000)

print(f"Performance:")
print(f"  - Steps per second: {performance['steps_per_second']:.0f}")
print(f"  - Average spikes per step: {performance['spikes_per_step']:.2f}")
```

### 5. Advanced Research Tools

#### Brain Analysis and Visualization
```python
# Create neural network visualization
viz = pynlm.create_neural_network_visualizer(brain)
print(viz)

# Analyze brain activity patterns
analysis = pynlm.analyze_brain_activity(brain)
print(f"Activity level: {analysis['activity_level']}")
print(f"Stability: {analysis['stability']}")
```

#### State Management and Comparison
```python
# Create brain snapshots for comparison
snapshot1 = pynlm.create_brain_snapshot(brain, "before_simulation")
snapshot2 = pynlm.create_brain_snapshot(brain, "after_simulation")

comparison = pynlm.compare_brain_snapshots(snapshot1, snapshot2)
print(f"Neurons changed: {comparison.get('total_neurons_difference', 0)}")
print(f"Firing rate changed: {comparison.get('average_firing_rate_difference', 0):.3f} Hz")
```

### 6. Enhanced Error Handling and Validation

```python
# Validate configuration
config = pynlm.createResearchConfig()
errors, warnings = pynlm.validate_config(config)

if errors:
    print("Configuration errors:")
    for error in errors:
        print(f"  - {error}")

if warnings:
    print("Configuration warnings:")
    for warning in warnings:
        print(f"  - {warning}")
```

### 7. Ready-to-Run Environments

```python
# Create a complete research environment
brain, world, agent = pynlm.createReadyToRunEnvironment()

# Environment is pre-configured with optimal settings
print(f"Environment created:")
print(f"  - Brain: {brain.getTotalNeuronCount()} neurons")
print(f"  - World: {world.getWidth()}x{world.getHeight()} cells")
print(f"  - Agent: Reward={agent.isRewardModulationEnabled()}, "
      f"Curiosity={agent.isCuriosityEnabled()}, "
      f"Development={agent.isDevelopmentEnabled()}")
```

## Statistical Analysis Methods

### Comprehensive Firing Statistics
```python
stats = brain.getFiringStatistics()

# Key statistics
print(f"Total neurons: {stats['total_neurons']}")
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Active neurons: {stats['active_neurons']}")
print(f"Total spikes: {stats['total_spikes']}")

# Derived metrics
print(f"Firing percentage: {stats['firing_percentage']:.1f}%")
print(f"Activity percentage: {stats['activity_percentage']:.1f}%")
print(f"Spikes per neuron: {stats['spikes_per_neuron']:.2f}")
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"E/I ratio: {stats['e_i_ratio']:.3f}")
```

### Spike Rate Distribution
```python
# Get spike rate distribution histogram
distribution = brain.getSpikeRateDistribution(20)
print(f"Spike rate distribution (20 bins): {distribution}")
```

### Brain Comparison
```python
# Compare two brains on different metrics
comparison = brain1.compareBrains(brain2, "firing_rate")
print(f"Firing rate difference: {comparison['difference']:.3f}")
print(f"Percent change: {comparison['percent_change']:.1f}%")

# Other comparison metrics
brain1.compareBrains(brain2, "total_spikes")
brain1.compareBrains(brain2, "neuron_count")
```

## File I/O Operations

### Configuration File Management
```python
import json

# Save configuration to JSON file
config = pynlm.createResearchConfig()
pynlm.save_config_to_file(config, "my_research_config.json")

# Load configuration from JSON file
loaded_config = pynlm.load_config_from_file("my_research_config.json")

# Example: Modify loaded configuration
loaded_config.set("brain.neuron_count", 1500)
pynlm.save_config_to_file(loaded_config, "custom_config.json")
```

## World Object Creation and Management

### Creating World Objects
```python
world = pynlm.createSimpleWorld()
world.configure(30, 30, 8, 8)

# Create resource objects
resource = world.createResource(5, 5, 10.0)
print(f"Created resource: {resource.value} energy at ({resource.x}, {resource.y})")

# Create hazard objects
hazard = world.createHazard(10, 10, 15.0)
print(f"Created hazard: {hazard.value} damage at ({hazard.x}, {hazard.y})")

# Create wall objects
wall = world.createWall(20, 20, 1.0)
print(f"Created wall: radius {wall.radius} at ({wall.x}, {world.y})")

# Create marker objects
marker = world.createMarker(25, 25)
print(f"Created marker at ({marker.x}, {marker.y})")
```

### Object Management
```python
# Get objects at specific positions
resource_at_pos = world.getResourceAt(5, 5)
hazard_at_pos = world.getHazardAt(10, 10)
wall_at_pos = world.getWallAt(20, 20)

# Get object counts
print(f"Total objects: {world.getTotalObjectCount()}")
print(f"Resources: {world.getObjectCount(pynlm.WorldObjectType.Resource)}")
print(f"Hazards: {world.getObjectCount(pynlm.WorldObjectType.Hazard)}")
print(f"Walls: {world.getObjectCount(pynlm.WorldObjectType.Wall)}")
print(f"Markers: {world.getObjectCount(pynlm.WorldObjectType.Marker)}")
```

## Complete Agent Example

### Full Agent Simulation
```python
import pynlm

def run_complete_agent_example(num_steps=300):
    """Run a complete agent simulation with all features"""
    
    # 1. Create configuration and brain
    config = pynlm.createResearchConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.setAgentStart(10, 10)
    
    # 3. Create and initialize agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 5. Run simulation with progress tracking
    print(f"Running agent simulation for {num_steps} steps...")
    for step in range(num_steps):
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 50 steps
        if step % 50 == 0:
            print(f"Step {step}: Neurons={brain.getFiringNeuronCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}")
    
    # 6. Get final results and status
    total_spikes = brain.getTotalSpikeCount()
    avg_firing_rate = brain.getAverageFiringRate()
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    
    status = agent.getAgentStatus(world)
    
    print(f"\nSimulation completed!")
    print(f"Results: {total_spikes} spikes, {avg_firing_rate:.2f} Hz, "
          f"Curiosity={curiosity:.3f}, Novelty={novelty:.3f}")
    
    return {
        "total_spikes": total_spikes,
        "average_firing_rate": avg_firing_rate,
        "curiosity": curiosity,
        "novelty": novelty,
        "status": status,
        "brain_stats": brain.getFiringStatistics(),
        "agent_stats": {
            "reward_mod_enabled": agent.isRewardModulationEnabled(),
            "curiosity_enabled": agent.isCuriosityEnabled(),
            "development_enabled": agent.isDevelopmentEnabled()
        }
    }

# Run the example
results = run_complete_agent_example(200)
print(f"\nFinal brain state: {results['brain_stats']['activity_level']}")
```

## Backward Compatibility

All existing Python API functions remain unchanged:

### Original Functions (Still Available)
```python
# Configuration
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
config.saveToFile("config.json")
config.set("brain.neuron_count", 1000)
config.get("brain.neuron_count")
config.has("brain.neuron_count")

# Brain
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)
brain.getTotalNeuronCount()
brain.getFiringNeuronCount()
brain.getTotalSpikeCount()

# World
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.update(0.1)
world.getSensoryPercept()

# Agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.processSensoryInput(percept)
agent.decodeMotorCommand()
```

### Original Methods (Still Available)
- `Brain.initialize()`
- `Brain.step(step)`
- `Brain.receiveSensoryInput(input)`
- `Brain.produceAction()`
- `Brain.save(filepath)` / `Brain.load(filepath)`
- `Brain.getTotalNeuronCount()`
- `Brain.getTotalSynapseCount()`
- `Brain.getFiringNeuronCount()`
- `Brain.getAverageFiringRate()`
- And many more...

**All original functionality is preserved with enhanced functionality added alongside.**

## Performance Considerations

### Memory Usage
- Context managers help prevent memory leaks
- Large simulations should use `benchmark_brain_performance()` to monitor memory
- Consider using smaller configurations for testing

### Simulation Speed
- Use `runSteps()` for efficient batch processing
- Consider `createTestingConfig()` for performance testing
- Monitor with `benchmark_brain_performance()` for optimization

## Troubleshooting

### Common Issues

**"Brain isn't doing anything"**
- Did you call `brain.initialize()`? (Now auto-initialized with context managers)
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - ensure sensory input is connected

### Error Messages

The enhanced API provides better error messages:
- Detailed validation errors from `validate_config()`
- Clear exception messages from Python exceptions
- Informative warnings for potential issues

## Future Enhancements

The enhanced API provides a foundation for future improvements:

1. **Machine Learning Integration**
   - Integration with scikit-learn and TensorFlow
   - Neural network analysis tools
   - Pattern recognition capabilities

2. **Advanced Visualization**
   - 3D neural network visualization
   - Interactive web-based visualization
   - Real-time monitoring tools

3. **Distributed Computing**
   - Multi-process simulation support
   - Distributed computing capabilities
   - Cloud integration

4. **Enhanced Learning Algorithms**
   - Advanced plasticity rules
   - Novel neuromodulation mechanisms
   - Cognitive development algorithms

## Version Information

```python
import pynlm
print(pynlm.get_version())
```

Current version: `pynlm 0.1.0 - Enhanced Python Bindings for NLM`

## Build Instructions

### From Source
```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install
pip install .

# Development install
pip install -e .
```

### Build Outputs
After successful build, you'll have:
- `pynlm` Python module
- Documentation files
- Example scripts
- Test files

### Testing
```bash
# Run the enhanced examples
python3 enhanced_examples.py

# Run quick test
python3 quick_test.py

# Run unit tests (if available)
python3 -m pytest tests/
```

## Conclusion

The enhanced Python API for NLM provides:

1. **Better Developer Experience**: Context managers, convenience methods, and Pythonic API
2. **Research Optimization**: Specialized configuration presets and analysis tools
3. **Enhanced Functionality**: Advanced statistical methods, simulation helpers, and research tools
4. **Better Error Handling**: Comprehensive validation and informative error messages
5. **Performance Monitoring**: Built-in benchmarking and analysis capabilities
6. **Backward Compatibility**: All existing code continues to work unchanged

The enhancements maintain the original C++ functionality while providing a much more user-friendly Python interface that follows Python conventions and best practices.