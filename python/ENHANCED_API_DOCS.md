# NLM Python Bindings - Enhanced API Documentation

## Overview

This document provides comprehensive documentation for the enhanced NLM (Neural Learning Machine) Python bindings. It covers the new features, improved error handling, and provides examples for both beginners and advanced users.

## New Features and Improvements

### 1. Enhanced Error Handling

The NLM Python bindings now include a comprehensive error handling system with custom exception types:

```python
import pynlm

try:
    config = pynlm.createStandardConfig("advanced")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # This will raise a SimulationError if something goes wrong
    brain.step(0)
    
except pynlm.ConfigurationError as e:
    print(f"Configuration problem: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation problem: {e}")
except pynlm.ResourceError as e:
    print(f"Resource problem: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### 2. Standard Configuration Templates

The bindings now provide several pre-configured standard settings:

```python
import pynlm

# Create different types of standard configurations

# Minimal configuration (fast, small brain)
minimal_config = pynlm.createStandardConfig("minimal")

# Standard configuration (balanced performance)
standard_config = pynlm.createStandardConfig("standard")

# Advanced configuration (complex, feature-rich)
advanced_config = pynlm.createStandardConfig("advanced")

# Use the configuration
brain = pynlm.createBrain(standard_config)
brain.initialize()
```

### 3. Complete Agent Simulation Helper

A new convenience function makes it easy to run complete agent simulations:

```python
import pynlm

# Run a complete agent simulation with all features enabled
pynlm.runCompleteAgentSimulation(
    config_type="standard",
    num_steps=1000,
    enable_reward_modulation=True,
    enable_curiosity=True,
    enable_development=True,
    enable_structural_plasticity=True
)

# Or customize for specific use cases
pynlm.runCompleteAgentSimulation(
    config_type="minimal",
    num_steps=500,
    enable_reward_modulation=False,
    enable_curiosity=True,
    enable_development=False,
    enable_structural_plasticity=False
)
```

### 4. Exception Registration

Proper exception handling can be registered for better error reporting:

```python
import pynlm

# Register NLM exception translators
pynlm.register_nlm_exceptions()

# Now C++ exceptions will be properly converted to Python exceptions
try:
    # Code that might throw C++ exceptions
    pass
except RuntimeError as e:
    print(f"C++ exception caught: {e}")
```

## Practical Examples

### Example 1: Simple Brain Simulation

```python
import pynlm
import time

# Create and initialize a simple brain
config = pynlm.createStandardConfig("minimal")
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
print("Running simple brain simulation...")
start_time = time.time()

for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        firing_neurons = brain.getFiringNeuronCount()
        total_spikes = brain.getTotalSpikeCount()
        print(f"Step {step}: {firing_neurons} firing, {total_spikes} total spikes")

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Final stats - Neurons: {brain.getTotalNeuronCount()}, "
      f"Firing: {brain.getFiringNeuronCount()}, "
      f"Spikes: {brain.getTotalSpikeCount()}")
```

### Example 2: Complete Agent in a World

```python
import pynlm

# Setup complete agent-world system
config = pynlm.createStandardConfig("standard")
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(15.0, 15.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

print("Running complete agent simulation...")

# Main simulation loop
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get sensory input and process it
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action and execute it
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning
    reward = 0.0
    if percept.getInternal():
        reward = percept.getInternal()[0]
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
    
    # Print progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev stage: {brain.getDevelopmentalStage()}")

print("Agent simulation complete!")
```

### Example 3: Working with Context and Error Handling

```python
import pynlm

def safe_brain_operation(config_type="standard", num_steps=100):
    """Safely create and run a brain simulation."""
    try:
        # Create configuration
        config = pynlm.createStandardConfig(config_type)
        
        # Create and initialize brain with context
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation with progress reporting
        print(f"Running {num_steps} steps...")
        
        for step in range(num_steps):
            brain.step(step)
            
            # Report progress every 25 steps
            if (step + 1) % 25 == 0:
                print(f"Progress: {((step + 1) / num_steps * 100):.1f}%")
                print(f"  Stats: {brain.getFiringNeuronCount()} firing, "
                      f"{brain.getTotalSpikeCount()} spikes")
        
        return brain
        
    except pynlm.ConfigurationError as e:
        print(f"Configuration error: {e}")
        raise
    except pynlm.SimulationError as e:
        print(f"Simulation error: {e}")
        raise
    except Exception as e:
        print(f"Unexpected error: {e}")
        raise

# Use the safe function
try:
    final_brain = safe_brain_operation("advanced", 200)
    print("Simulation completed successfully!")
    print(f"Final spike count: {final_brain.getTotalSpikeCount()}")
except Exception as e:
    print(f"Simulation failed: {e}")
```

### Example 4: Advanced Configuration and Experimentation

```python
import pynlm

def run_experiment(config_type, params, steps):
    """Run a customizable experiment with specific parameters."""
    # Create configuration
    config = pynlm.createStandardConfig(config_type)
    
    # Customize based on params
    if "neuron_count" in params:
        config.set("brain.neuron_count", params["neuron_count"])
    
    if "learning_rate" in params:
        config.set("plasticity.stdp.learning_rate", params["learning_rate"])
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run experiment
    print(f"Running experiment with {config_type} configuration...")
    print(f"Parameters: {params}")
    
    for step in range(steps):
        brain.step(step)
    
    return {
        "final_spike_count": brain.getTotalSpikeCount(),
        "firing_neurons": brain.getFiringNeuronCount(),
        "avg_firing_rate": brain.getAverageFiringRate(),
        "e_i_ratio": brain.getExcitationInhibitionRatio(),
        "development_stage": brain.getDevelopmentalStage()
    }

# Define experiment parameters
experiments = [
    {"type": "standard", "params": {"neuron_count": 1000}, "steps": 500},
    {"type": "advanced", "params": {"neuron_count": 5000, "learning_rate": 0.002}, "steps": 1000},
    {"type": "minimal", "params": {"neuron_count": 100}, "steps": 200}
]

# Run all experiments
results = []
for exp in experiments:
    try:
        result = run_experiment(exp["type"], exp["params"], exp["steps"])
        results.append({"config": exp["type"], "result": result})
        print(f"Experiment {exp['type']} completed successfully!")
    except Exception as e:
        print(f"Experiment {exp['type']} failed: {e}")
        results.append({"config": exp["type"], "error": str(e)})

# Summary of results
print("\n=== Experiment Summary ===")
for exp in results:
    if "error" in exp:
        print(f"{exp['config']}: FAILED - {exp['error']}")
    else:
        r = exp['result']
        print(f"{exp['config']}: {r['final_spike_count']} spikes, "
              f"{r['firing_neurons']} firing neurons, "
              f"E/I ratio: {r['e_i_ratio']:.2f}")
```

## API Reference

### Core Classes

#### Config
- `createDefaultConfig()`: Creates a basic configuration
- `createStandardConfig(config_type)`: Creates standard configurations
- `loadFromFile(filepath)`: Loads configuration from file
- `loadFromArgs(argc, argv)`: Loads from command line arguments
- `saveToFile(filepath)`: Saves configuration to file

#### Brain
- `initialize()`: Initializes the brain
- `step(step_number)`: Performs a simulation step
- `receiveSensoryInput(input)`: Injects sensory data
- `produceAction()`: Gets motor action
- `reset()`: Resets brain state
- `save(filepath)`: Saves brain state
- `load(filepath)`: Loads brain state

#### AgentBrain
- `initialize(world)`: Initializes with world
- `processSensoryInput(percept)`: Processes sensory input
- `decodeMotorCommand()`: Gets motor command
- `applyRewardModulation(reward, predicted_reward)`: Applies reward
- `updateDevelopment(timestep)`: Updates development

#### SimpleWorld
- `configure(width, height, visionWidth, visionHeight)`: Configures world
- `reset()`: Resets world
- `update(timestep)`: Updates world state
- `applyMotorCommand(cmd, time)`: Applies motor command
- `getSensoryPercept()`: Gets sensory percept
- `getAgentBody()`: Gets agent body state

### Convenience Functions

#### createStandardConfig(config_type)
Creates a standard configuration with preset parameters.

- **Parameters**: `config_type` ("minimal", "standard", "advanced")
- **Returns**: `Config` object
- **Examples**: See examples above

#### runCompleteAgentSimulation(config_type, num_steps, enable_reward_modulation, enable_curiosity, enable_development, enable_structural_plasticity)
Runs a complete agent simulation with all features enabled.

- **Parameters**: All boolean except config_type and num_steps
- **Returns**: None (runs simulation directly)
- **Examples**: See examples above

#### register_nlm_exceptions()
Registers NLM exception translators for proper error handling.

- **Parameters**: None
- **Returns**: None
- **Examples**: See examples above

### Exception Types

#### NLMError
Base exception class for all NLM errors.

#### ConfigurationError
Thrown for configuration-related errors.

#### SimulationError
Thrown for simulation-related errors.

#### ResourceError
Thrown for resource-related errors.

## Performance Tips

### 1. Configuration Optimization
```python
# Use standard configurations for better performance
config = pynlm.createStandardConfig("minimal")  # Faster, smaller brain
# vs. creating custom config from scratch
```

### 2. Simulation Control
```python
# Control simulation granularity
brain.step(0)  # Single step
# vs. brain.step(0, 0.001)  # With timestamp
```

### 3. Memory Management
```python
# Use context managers for resource management
with BrainContext(brain) as ctx:
    # Brain operations
    pass
```

### 4. Batch Operations
```python
# Process multiple steps efficiently
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Progress: {step/10}%")
```

## Migration Guide

### From Old to New API

#### Configuration
**Old:**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)
```

**New:**
```python
config = pynlm.createStandardConfig("standard")  # Already includes these settings
```

#### Simulation
**Old:**
```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)
```

**New:**
```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)
# (Same, but with better error handling)
```

#### Error Handling
**Old:**
```python
try:
    brain.step(0)
except Exception as e:
    print(f"Error: {e}")
```

**New:**
```python
try:
    brain.step(0)
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
```

## Troubleshooting

### Common Issues and Solutions

**Issue**: "Brain not initialized"
**Solution**: Always call `brain.initialize()` before calling `brain.step()`

**Issue**: "No such configuration key"
**Solution**: Use `config.has(key)` to check before accessing, or use `createStandardConfig()` for pre-configured settings

**Issue**: "Simulation taking too long"
**Solution**: Use `createStandardConfig("minimal")` for faster, smaller simulations

**Issue**: "Poor performance"
**Solution**: 
- Use standard configurations
- Control simulation granularity
- Monitor memory usage
- Consider running in batches

**Issue**: "Python exceptions not caught properly"
**Solution**: Call `pynlm.register_nlm_exceptions()` before running simulations

## Next Steps

### For Beginners
1. Read the `easy_usage.md` guide for basic usage
2. Try the examples in this document
3. Use `createStandardConfig("minimal")` for quick testing

### For Advanced Users
1. Explore the `HOW_TO_USE.md` guide for advanced topics
2. Implement custom configuration templates
3. Create your own convenience functions
4. Contribute to the project!

### For Developers
1. Study the C++ source code in `src/`
2. Add new convenience functions
3. Improve error handling
4. Add performance optimizations

## Conclusion

The enhanced NLM Python bindings provide a more intuitive, powerful, and user-friendly API while maintaining full backward compatibility. The new features significantly improve the developer experience, making it easier to:

- Set up simulations quickly
- Handle errors effectively
- Customize configurations
- Run complete agent simulations
- Debug and troubleshoot issues

The improvements make the NLM framework more accessible to Python developers while providing the advanced features needed by experienced users.

Happy coding with NLM!