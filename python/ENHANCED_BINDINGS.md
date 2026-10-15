# NLM Python Bindings - Enhanced Documentation

## Overview

This document provides comprehensive documentation for the enhanced NLM (Neural Learning Machine) Python bindings. The enhanced bindings include improved error handling, Pythonic APIs, configuration presets, debugging utilities, and more.

## Enhanced Features

### 1. Custom Exception System

The enhanced bindings provide a comprehensive exception system:

```python
import pynlm

# Custom exception types
error = pynlm.NLMError("General NLM error")
config_error = pynlm.ConfigError("Configuration problem")
brain_error = pynlm.BrainError("Brain initialization failed")
world_error = pynlm.WorldError("World simulation error")

# All exception types inherit from Exception
print(isinstance(config_error, Exception))  # True
print(isinstance(config_error, pynlm.NLMError))  # True
```

### 2. Pythonic APIs

The enhanced bindings provide more Pythonic APIs:

```python
import pynlm

# Configuration with [] operator
config = pynlm.createDefaultConfig()
config["brain.neuron_count"] = 2000
config["test.value"] = 3.14
config["test.enabled"] = True

# Configuration validation
if config.validate():
    print("Configuration is valid")

# Configuration with length and membership
print(len(config))  # Number of configuration entries
print("brain.neuron_count" in config)  # Check if key exists

# Configuration merge
config1.merge(config2)
```

### 3. Configuration Presets

Multiple configuration presets are available:

```python
import pynlm

# Default configuration (balanced)
default_config = pynlm.createDefaultConfig()
print(f"Neurons: {default_config['brain.neuron_count']}")

# Performance configuration (smaller, faster)
perf_config = pynlm.createPerformanceConfig()
print(f"Performance config neurons: {perf_config['brain.neuron_count']}")

# Exploration configuration (larger, more learning)
explore_config = pynlm.createExplorationConfig()
print(f"Exploration config neurons: {explore_config['brain.neuron_count']}")

# Debug configuration (small, dense, easy to debug)
debug_config = pynlm.createDebugConfig()
```

### 4. Enhanced Brain Interface

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Brain statistics
stats = brain.getBrainStatistics()
print(stats)

# Brain validation
success = brain.initializeWithValidation()
if success:
    print("Brain initialized successfully")

# Brain snapshot management
brain.createSnapshot("brain_state.bin")
brain.loadSnapshot("brain_state.bin")

# Region management
region_names = brain.getRegionNames()
for name in region_names:
    region_id = brain.getRegionByName(name)
    print(f"Region '{name}' has ID: {region_id}")
```

### 5. Enhanced Agent Brain Interface

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Subsystem control
agent.enableAllSubsystems()
agent.disableAllSubsystems()

# Get subsystem status
status = agent.getSubsystemStatus()
print(f"Reward Modulation: {status[0]}")
print(f"Structural Plasticity: {status[1]}")
print(f"Development: {status[2]}")
print(f"Curiosity: {status[3]}")

# Agent information
info = agent.getAgentInfo()
print(info)

# Development tracking
print(f"Current developmental stage: {agent.getDevelopmentalStage()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
```

### 6. Enhanced World Interface

```python
import pynlm

world = pynlm.createSimpleWorld()
world.configureSimple(20, 20)  # Simple configuration with square vision

# Object management
world.addResource(5.0, 5.0, 10.0)
world.addHazard(10.0, 10.0, 5.0)

# Position utilities
world.setAgentStart(10.0, 10.0)
print(f"Agent near origin: {world.isAgentNear(0.0, 0.0, 5.0)}")

# World bounds
width, height = world.getWorldBounds()
print(f"World dimensions: {width} x {height}")

# Reproducible simulations
world.resetAndSeed(42)
```

### 7. Episode Context Management

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Episode context manager
with pynlm.episode_context(brain, agent, world) as episode:
    print(f"Episode initialized: {episode.isInitialized()}")
    
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Episode status
    print(episode.getStatus())
```

### 8. Enhanced Action Interface

```python
import pynlm

# Create actions
action1 = pynlm.Action(pynlm.ActionType.MoveForward)
action2 = pynlm.Action(pynlm.ActionType.TurnLeft, [0.5])

# Action properties
print(f"Action type: {action1.getType()}")
print(f"Action name: {action2.getName()}")
print(f"Has parameters: {not action2.isEmpty()}")
print(f"Parameter count: {action2.getParameterCount()}")

# Clone actions
cloned_action = action2.clone()
print(f"Cloned action type: {cloned_action.getType()}")

# Action representation
print(repr(action1))
print(action2)
```

### 9. Utility Functions

```python
import pynlm

# Version information
print(pynlm.getVersion())
print(pynlm.getPlatformInfo())

# Debug configuration
debug_config = pynlm.createDebugConfig()
```

### 10. Comprehensive Example

```python
import pynlm

def run_advanced_simulation():
    """Advanced simulation using all enhanced features."""
    
    # 1. Create and configure brain with exploration preset
    config = pynlm.createExplorationConfig()
    config["neuromod.curiosity.enable"] = True
    config["neuromod.novelty.enable"] = True
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(15, 15, 6, 6)
    agent.initialize(world)
    
    # 3. Enable all subsystems
    agent.enableAllSubsystems()
    
    # 4. Run multiple episodes with context manager
    for episode in range(3):
        print(f"\n=== Episode {episode + 1} ===")
        
        with pynlm.episode_context(brain, agent, world) as ep:
            # Simulation loop
            for step in range(50):
                world.update(0.1)
                
                # Get sensory input
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                
                # Brain step
                brain.step(step)
                
                # Decode motor command
                motor_cmd = agent.decodeMotorCommand()
                world.applyMotorCommand(motor_cmd, world.getSimulationTime())
                
                # Apply neuromodulation
                reward = world.getAgentBody().energy if world.getAgentBody().energy > 0 else 0.0
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
                
                # Print progress
                if step % 10 == 0:
                    print(f"  Step {step}: Firing neurons={brain.getFiringNeuronCount()}, "
                          f"Curiosity={agent.getCuriosityLevel():.3f}, "
                          f"Novelty={agent.getNoveltyLevel():.3f}")
            
            # Episode statistics
            print(f"  Episode completed: {ep.getStatus()}")
    
    print("\n=== Final Brain Statistics ===")
    print(brain.getBrainStatistics())

# Run the example
run_advanced_simulation()
```

## Migration Guide

### From Old to New API

#### Configuration

**Old:**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
value = config.get("brain.neuron_count")
```

**New:**
```python
config = pynlm.createDefaultConfig()
config["brain.neuron_count"] = 1000
value = config["brain.neuron_count"]
```

#### Brain Initialization

**Old:**
```python
success = brain.initialize()
```

**New:**
```python
success = brain.initializeWithValidation()
```

#### Agent Brain

**Old:**
```python
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

**New:**
```python
agent.enableAllSubsystems()
```

Or for individual control:
```python
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

#### World Configuration

**Old:**
```python
world.configure(20, 20, 8, 8)
```

**New:**
```python
world.configureSimple(20, 20)
```

Or for custom configuration:
```python
world.configure(20, 20, 8, 8)
```

### Error Handling

**Old:**
```python
brain.initialize()  # Silent failure on error
```

**New:**
```python
try:
    success = brain.initializeWithValidation()
    if not success:
        raise pynlm.BrainError("Initialization failed")
except pynlm.BrainError as e:
    print(f"Brain error: {e}")
```

## Troubleshooting

### Common Issues

1. **Missing Python Dependencies**
   ```bash
   pip install pybind11 numpy pytest
   ```

2. **Build Errors**
   ```bash
   # Ensure CMake is installed
   mkdir build_python
   cd build_python
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)
   ```

3. **Import Errors**
   ```bash
   # Install the package
   cd python
   pip install -e .
   ```

4. **Test Failures**
   ```bash
   cd python
   python test_enhanced_bindings.py
   ```

### Debug Mode

For debugging, use the debug configuration:

```python
import pynlm

# Create a small, dense configuration for debugging
debug_config = pynlm.createDebugConfig()
brain = pynlm.createBrain(debug_config)

# Enable verbose logging
brain.initialize()
print(brain.getBrainStatistics())
```

## Advanced Usage

### 1. Configuration Validation

Validate your configuration before use:

```python
def validate_config(config):
    """Validate configuration with custom checks."""
    if not config.validate():
        raise pynlm.ConfigError("Invalid configuration")
    
    # Additional custom validation
    neuron_count = config["brain.neuron_count"]
    if neuron_count < 10 or neuron_count > 10000:
        raise pynlm.ConfigError(f"Invalid neuron count: {neuron_count}")
    
    return True
```

### 2. Custom Episode Management

Create custom episode managers:

```python
class CustomEpisodeManager:
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.episode_data = []
    
    def __enter__(self):
        self.world.reset()
        self.agent.reset()
        if self.brain:
            self.brain.reset()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        # Save episode data
        self.episode_data.append({
            'final_neurons': self.brain.getTotalNeuronCount() if self.brain else 0,
            'final_firing': self.brain.getFiringNeuronCount() if self.brain else 0,
            'world_time': self.world.getSimulationTime()
        })
    
    def get_episode_summary(self):
        if not self.episode_data:
            return "No episode data"
        
        latest = self.episode_data[-1]
        return f"Episode completed: {latest['final_neurons']} neurons, " \
               f"{latest['final_firing']} firing, {latest['world_time']:.1f}s world time"
```

### 3. Performance Monitoring

Monitor simulation performance:

```python
import time

class PerformanceMonitor:
    def __init__(self):
        self.step_times = []
        self.memory_usage = []
    
    def record_step_time(self, start_time, end_time):
        step_time = end_time - start_time
        self.step_times.append(step_time)
        return step_time
    
    def get_statistics(self):
        if not self.step_times:
            return "No performance data"
        
        avg_time = sum(self.step_times) / len(self.step_times)
        max_time = max(self.step_times)
        min_time = min(self.step_times)
        
        return f"Performance: avg={avg_time:.4f}s, min={min_time:.4f}s, max={max_time:.4f}s, steps={len(self.step_times)}"
```

## API Reference

### Classes

#### Config
- `createDefaultConfig()` - Create default configuration
- `createPerformanceConfig()` - Create performance-optimized configuration
- `createExplorationConfig()` - Create exploration-optimized configuration
- `createDebugConfig()` - Create debug configuration
- `validate()` - Validate configuration

#### Brain
- `initialize()` - Initialize brain
- `initializeWithValidation()` - Initialize with validation
- `step(step, time=None)` - Perform simulation step
- `getBrainStatistics()` - Get formatted brain statistics
- `getRegionNames()` - Get all region names
- `getRegionByName(name)` - Get region ID by name
- `createSnapshot(filename)` - Save brain state
- `loadSnapshot(filename)` - Load brain state

#### AgentBrain
- `initialize(world)` - Initialize with world
- `enableAllSubsystems()` - Enable all subsystems
- `disableAllSubsystems()` - Disable all subsystems
- `getSubsystemStatus()` - Get subsystem status
- `getAgentInfo()` - Get formatted agent information
- `enableRewardModulation(enabled)` - Enable/disable reward modulation
- `enableStructuralPlasticity(enabled)` - Enable/disable structural plasticity
- `enableDevelopment(enabled)` - Enable/disable development
- `enableCuriosity(enabled)` - Enable/disable curiosity

#### SimpleWorld
- `configure(width, height, vision_width, vision_height)` - Configure world
- `configureSimple(width, height)` - Configure with square vision
- `reset()` - Reset world
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world
- `applyMotorCommand(cmd, time)` - Apply motor command
- `addResource(x, y, value)` - Add resource object
- `addHazard(x, y, damage)` - Add hazard object
- `isAgentNear(x, y, distance)` - Check if agent is near position
- `getWorldBounds()` - Get world dimensions
- `resetAndSeed(seed)` - Reset and set random seed

#### Action
- `Action(type, parameters=None)` - Create action
- `getType()` - Get action type
- `setType(type)` - Set action type
- `getParameters()` - Get parameters
- `setParameters(parameters)` - Set parameters
- `getName()` - Get action name
- `clone()` - Clone action
- `isEmpty()` - Check if empty
- `getParameterCount()` - Get parameter count

#### EpisodeManager
- `episode_context(brain, agent, world)` - Create episode context manager
- `isInitialized()` - Check if initialized
- `getStatus()` - Get episode status

### Functions

#### Global
- `getVersion()` - Get version information
- `getPlatformInfo()` - Get platform information
- `validateConfig(config)` - Validate configuration

### Exceptions

#### Base
- `NLMError` - Base NLM exception
- `ConfigError` - Configuration error
- `BrainError` - Brain error
- `WorldError` - World error

## Best Practices

### 1. Use Context Managers for Episodes
```python
with pynlm.episode_context(brain, agent, world) as episode:
    # Your simulation code here
```

### 2. Validate Configuration
```python
config = pynlm.createDefaultConfig()
if not config.validate():
    raise pynlm.ConfigError("Invalid configuration")
```

### 3. Use Configuration Presets
```python
# Use exploration preset for learning environments
config = pynlm.createExplorationConfig()

# Use performance preset for real-time applications
config = pynlm.createPerformanceConfig()
```

### 4. Handle Errors Gracefully
```python
try:
    brain.initializeWithValidation()
except pynlm.BrainError as e:
    print(f"Failed to initialize brain: {e}")
```

### 5. Monitor Performance
```python
import time

start_time = time.time()
brain.step(step)
end_time = time.time()
step_time = end_time - start_time

# Log or use step_time for performance analysis
```

## Conclusion

The enhanced NLM Python bindings provide a comprehensive, Pythonic interface for neural network simulation. The enhancements include:

- Better error handling with custom exceptions
- Pythonic APIs for intuitive usage
- Configuration presets for different use cases
- Context managers for episode management
- Enhanced debugging and monitoring capabilities
- Comprehensive documentation and examples

These enhancements make NLM more accessible to Python users while maintaining the power and flexibility of the underlying C++ implementation.

## Version History

### Version 0.1.0 (Current)
- Initial enhanced release
- All new features implemented
- Comprehensive test suite
- Enhanced documentation

### Future Enhancements
- Visualization utilities
- Advanced plotting and analysis
- Machine learning integration
- Web API support
- Mobile deployment

## Licensing

NLM is licensed under the MIT License. See LICENSE file for details.

## Acknowledgments

This enhanced documentation and feature set builds upon the original NLM project and contributes to the open source neuroscience simulation community.