# NLM Python Bindings - User Guide

This document provides comprehensive guidance for using the NLM (Neural Learning Machine) Python bindings.

## Quick Start

### Installation

First, ensure you have the NLM C++ library built and installed. You can build it using CMake:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Installation

Install the Python package in development mode:

```bash
pip install -e .
```

## Basic Usage

### Creating a Brain

```python
import pynlm

# Create a default configuration with sensible parameters
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)

# Alternative: create config manually
config = pynlm.Config()
config.set("neuron_count", 1000)
config.set("region_count", 1)
config.set("random_seed", 42)
brain = pynlm.create_brain(config)
```

### Initializing and Running Simulation

```python
# Initialize the brain with the configuration
brain.initialize()

# Run simulation for a specific number of steps
brain.runSteps(1000)

# Or run with custom time steps
brain.runStepsWithTime(500, 0.001)  # 0.5 seconds of simulation time
```

### Getting Status Information

```python
# Get comprehensive brain status
status = brain.get_status()
print(status)

# Get specific system status
print("Working Memory:", brain.getWorkingMemoryStatus())
print("Episodic Memory:", brain.getEpisodicMemoryStatus())
print("Prediction System:", brain.getPredictionSystemStatus())
print("Cognition Status:", brain.getCognitionStatus())
print("Neuromodulation Status:", brain.getNeuromodulationStatus())
print("Development Status:", brain.getDevelopmentStatus())
```

## Creating an Agent

### Basic Agent Setup

```python
import pynlm

# Create brain and agent
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
agent = pynlm.create_agent_brain(brain)

# Create world and initialize agent
world = pynlm.createSimpleWorld()
world.configure(16, 16, 32, 32)  # Set world dimensions
agent.initialize(world)

# Enable all neuromodulatory systems
agent.enable_all_systems()
```

### Running Agent Simulation

```python
# Set agent starting position
world.setAgentStart(8.0, 8.0)

# Run simulation loop
for step in range(1000):
    # Get sensory percept from world
    percept = world.getSensoryPercept(world.getAgentBody())
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain simulation step
    brain.step(step)
    
    # Get motor command
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, float(step) * 0.001)
    
    # Apply reward modulation
    reward = world.computeReward(world.getAgentBody())
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.001)
    
    # Check agent status periodically
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Reward: {reward}")
        print(f"  Firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  Action: {action.getName()}")
```

## Error Handling

### NLM Custom Exceptions

The NLM bindings provide specific exception types for better error handling:

```python
import pynlm

try:
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.BrainInitializationError as e:
    print(f"Brain initialization error: {e}")
except pynlm.MemoryError as e:
    print(f"Memory error: {e}")
except pynlm.NLMError as e:
    print(f"General NLM error: {e}")
```

### Configuration Validation

```python
def safe_config_creation():
    try:
        config = pynlm.Config()
        
        # Validate configuration parameters
        config.set("neuron_count", 1000)
        config.set("region_count", 1)
        config.set("random_seed", 42)
        
        if not config.has("neuron_count"):
            raise pynlm.ConfigurationError("Missing required parameter: neuron_count")
        
        brain = pynlm.create_brain(config)
        
        if not brain.initialize():
            raise pynlm.BrainInitializationError("Brain failed to initialize")
            
        return brain
        
    except Exception as e:
        print(f"Failed to setup NLM: {e}")
        raise
```

## Advanced Features

### Working with Neurons and Regions

```python
# Access brain regions
regions = brain.getRegions()
for i, region in enumerate(regions):
    print(f"Region {i}: {region.get_name() if hasattr(region, 'get_name') else 'Unnamed'}")
    print(f"  Neurons: {region.getTotalNeuronCount()}")
    print(f"  Synapses: {region.getSynapseCount()}")
```

### Monitoring and Debugging

```python
# Comprehensive status monitoring
print("=== NLM Brain Status ===")
print(brain.get_status())

print("\n=== Agent Motivational State ===")
print(agent.getMotivationalState())

# Check memory system health
print("\n=== Memory System Health ===")
print(f"Working Memory: {brain.getWorkingMemoryStatus()}")
print(f"Episodic Memory: {brain.getEpisodicMemoryStatus()}")

# Get simulation statistics (basic version)
stats = brain.getSimulationStats()
print(f"\n=== Simulation Stats ===")
print(stats.to_string())
```

### Checkpointing and Persistence

```python
# Save brain state
brain.save("/tmp/nlm_checkpoint.bin")

# Load brain state (creates new brain)
new_brain = pynlm.create_brain(config)
new_brain.load("/tmp/nlm_checkpoint.bin")
```

## Integration Examples

### Complete Integration Test

```python
import pynlm
import time

def run_integration_test():
    """Run a complete integration test of the NLM system."""
    start_time = time.time()
    
    try:
        # Setup
        config = pynlm.create_default_config()
        brain = pynlm.create_brain(config)
        agent = pynlm.create_agent_brain(brain)
        world = pynlm.createSimpleWorld()
        
        # Initialize world
        world.configure(20, 20, 40, 40)
        world.setAgentStart(10.0, 10.0)
        
        # Initialize agent brain
        agent.initialize(world)
        agent.enable_all_systems()
        
        # Run simulation
        brain.initialize()
        print("NLM brain initialized successfully")
        
        for step in range(500):
            # Get perception
            percept = world.getSensoryPercept(world.getAgentBody())
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step, float(step) * 0.001)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, float(step) * 0.001)
            
            # Get reward
            reward = world.computeReward(world.getAgentBody())
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.001)
            
            # Periodic monitoring
            if step % 100 == 0:
                print(f"Step {step}: Reward={reward:.2f}, "
                      f"Firing={brain.getAverageFiringRate():.2f}Hz")
        
        # Final status
        print("\n=== Final Integration Status ===")
        print(brain.get_status())
        print(agent.getMotivationalState())
        
        # Test checkpointing
        brain.save("/tmp/integration_test.bin")
        print("\nCheckpoint saved successfully")
        
        elapsed = time.time() - start_time
        print(f"\nIntegration test completed in {elapsed:.2f} seconds")
        
        return True
        
    except Exception as e:
        print(f"Integration test failed: {e}")
        return False

# Run the integration test
if __name__ == "__main__":
    success = run_integration_test()
    exit(0 if success else 1)
```

## API Reference

### Core Classes

#### Brain
- `initialize()` - Initialize brain with configuration
- `step(step)` - Perform simulation step
- `step_with_time(step, time)` - Step with explicit time
- `getStatus()` - Get comprehensive status
- `getWorkingMemoryStatus()` - Get working memory status
- `getEpisodicMemoryStatus()` - Get episodic memory status
- `getPredictionSystemStatus()` - Get prediction system status
- `getCognitionStatus()` - Get cognitive systems status
- `getNeuromodulationStatus()` - Get neuromodulation systems status
- `getDevelopmentStatus()` - Get development system status
- `runSteps(numSteps)` - Run multiple steps
- `runStepsWithTime(numSteps, timeStep)` - Run multiple steps with time control
- `getNeuronActivations()` - Get neuron activation vector
- `getActiveNeuronCount()` - Get count of active neurons
- `getAverageFiringRate()` - Get average firing rate
- `getTotalSpikeCount()` - Get total spike count
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file

#### AgentBrain
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward modulation
- `updateDevelopment(timestep)` - Update development system
- `getDevelopmentalStage()` - Get developmental stage
- `getNeuromodulationLevel()` - Get neuromodulation level
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error
- `reset()` - Reset agent for new episode
- `getMotivationalState()` - Get motivational state string
- `enableAllSystems()` - Enable all agent brain systems

### Configuration Class

#### Config
- `loadFromFile(filepath)` - Load configuration from JSON file
- `saveToFile(filepath)` - Save configuration to JSON file
- `has(key)` - Check if configuration key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration entries
- `summary()` - Get configuration summary
- `set(key, value)` - Set configuration value
- `get(key, default)` - Get configuration value

### Utility Functions

#### Module-level Functions
- `create_default_config()` - Create default configuration
- `create_brain(config)` - Create brain with configuration (Python wrapper)
- `create_agent_brain(brain)` - Create agent brain interface (Python wrapper)
- `create_brain(config)` - Create raw C++ brain (legacy API)
- `create_agent_brain(brain)` - Create raw C++ agent brain (legacy API)
- `createSimpleWorld()` - Create simple world

### Exception Types

#### NLM Error Hierarchy
- `NLMError` - Base NLM error
- `ConfigurationError` - Configuration-related errors
- `BrainInitializationError` - Brain initialization errors
- `MemoryError` - Memory-related errors

## Performance Tips

1. **Batch Operations**: Use `runSteps()` for multiple simulation steps
2. **Monitor Progress**: Check status periodically in long-running simulations
3. **Memory Management**: The Python bindings use reference counting for automatic memory management
4. **Checkpointing**: Use checkpointing for long-running simulations
5. **Configuration**: Use `create_default_config()` for sensible default parameters

## Migration Guide

### From Legacy API to Enhanced API

The enhanced Python bindings maintain backward compatibility while adding new features:

**Legacy API (still works)**:
```python
import pynlm

# Old way
brain = pynlm.create_brain(config)
brain.step(step)
```

**Enhanced API (recommended)**:
```python
import pynlm

# New way with better error handling and convenience methods
brain = pynlm.create_brain(config)  # Returns Python wrapper
brain.initialize()
brain.runSteps(100)  # Convenience method

# Access enhanced features
print(brain.get_status())
```

### Error Handling Migration

**Legacy**: Catch generic exceptions
```python
try:
    brain.step(step)
except Exception as e:
    print(f"Error: {e}")
```

**Enhanced**: Catch specific NLM errors
```python
try:
    brain.step(step)
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.BrainInitializationError as e:
    print(f"Brain initialization error: {e}")
except pynlm.MemoryError as e:
    print(f"Memory error: {e}")
except pynlm.NLMError as e:
    print(f"General NLM error: {e}")
```

## Troubleshooting

### Common Issues and Solutions

**Issue: Brain fails to initialize**
- Solution: Check configuration parameters and ensure all required settings are valid
- Check: `brain.initialize()` returns False

**Issue: Memory errors**
- Solution: Ensure proper cleanup of references
- Use `brain.reset()` to clear memory between episodes

**Issue: Slow simulation performance**
- Solution: Optimize configuration parameters
- Use `create_default_config()` for sensible defaults
- Consider checkpointing for very long simulations

**Issue: Incorrect sensory processing**
- Solution: Verify world configuration and agent setup
- Check that `agent.initialize(world)` is called before processing

### Debugging Tips

1. **Use status information**: `brain.getStatus()` provides comprehensive status
2. **Monitor memory systems**: Check `brain.getWorkingMemoryStatus()` and `brain.getEpisodicMemoryStatus()`
3. **Check agent motivation**: `agent.getMotivationalState()` shows internal state
4. **Validate configuration**: Use `config.has(key)` to check required parameters
5. **Test with small simulations**: Start with fewer neurons and steps for debugging

## Future Enhancements

The NLM Python bindings are continuously being improved. Future features may include:

- **Visualization integration**: Plot neural activity and behavior
- **Advanced agent behaviors**: More sophisticated action selection
- **Multi-agent simulations**: Support for multiple interacting agents
- **Real-time interfaces**: Live visualization and control
- **Machine learning integration**: Better interfaces with ML libraries
- **Distributed computing**: Support for parallel simulations
- **Hardware acceleration**: GPU support for faster simulations

## Acknowledgments

This enhanced Python interface builds upon the original NLM (熙然) project, which is an experimental artificial developmental brain system. The original C++ implementation was developed as part of Phase 6 (Final Integration) of the NLM project.

Special thanks to the NLM research team for their contributions to the underlying neural simulation framework.