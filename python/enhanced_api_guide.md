# Enhanced NLM Python API

## Overview

The Enhanced NLM Python API provides significant improvements over the basic Python bindings, including better error handling, type safety, Pythonic interfaces, and advanced convenience functions. This API is designed to provide both beginners and advanced users with a more productive and intuitive interface to the NLM (Neural Learning Machine) brain simulation framework.

## Key Improvements

### 1. Better Error Handling

The enhanced API introduces custom exception classes for different error types:

- `NLMError`: Base exception for all NLM errors
- `ConfigurationError`: Errors related to configuration
- `SimulationError`: Errors during brain simulation
- `AgentError`: Errors related to agent operations

### 2. Type Safety and Type Hints

Enhanced classes provide type-safe getters and setters:

```python
# Type-safe configuration access
config.set("brain.neuron_count", 1000)    # int
config.set("brain.synapse_density", 0.1)  # float
config.set("test.enabled", True)         # bool
config.set("test.name", "hello")         # str

# Type-safe retrieval
neuron_count = config.getInt("brain.neuron_count")
synapse_density = config.getFloat("brain.synapse_density")
enabled = config.getBool("test.enabled")
name = config.getString("test.name")
```

### 3. Pythonic Configuration Helpers

Enhanced configuration provides convenient helper methods:

```python
# Configure brain with single method
config.configureBrain(2000, 0.2)

# Configure memory system
config.configureMemory(50000)

# Configure neuromodulation
config.configureNeuromodulation(2.0, False)
```

### 4. Enhanced Brain Interface

The enhanced brain class provides:

- **Progress Callbacks**: Monitor simulation progress with callbacks
- **Statistics Dictionary**: Get comprehensive statistics as Python dict
- **Region Information**: Access region details as Python dicts
- **Context Manager Support**: Safe brain operations with automatic cleanup
- **Advanced Step Methods**: Step with optional timestamp

```python
# Create enhanced brain
enhanced_brain = EnhancedBrain(config)
enhanced_brain.initialize()

# Get comprehensive statistics
stats = enhanced_brain.getStatistics()
print(f"Neurons: {stats['total_neurons']}")
print(f"Firing rate: {stats['average_firing_rate']}")

# Run simulation with progress callback
def progress_callback(step, stats):
    print(f"Step {step}: {stats['firing_neurons']} neurons firing")

enhanced_brain.runSimulation(1000, progress_callback)
```

### 5. Enhanced Agent Interface

The enhanced agent provides:

- **Subsystem Control**: Easy enable/disable of neuromodulation, plasticity, etc.
- **State Monitoring**: Get agent state as Python dictionary
- **Advanced Neuromodulation**: Apply reward modulation with prediction error
- **Development Control**: Update development with timestep

```python
# Create enhanced agent
enhanced_agent = EnhancedAgentBrain(brain)
enhanced_agent.initialize(world)

# Enable all learning subsystems
enhanced_agent.enableRewardModulation(True)
enhanced_agent.enableCuriosity(True)
enhanced_agent.enableDevelopment(True)
enhanced_agent.enableStructuralPlasticity(True)

# Get agent state
state = enhanced_agent.getAgentState()
print(f"Curiosity: {state['curiosity_level']}")
print(f"Novelty: {state['novelty_level']}")
print(f"Development stage: {state['developmental_stage']}")
```

### 6. Enhanced World Interface

The enhanced world provides:

- **Pythonic Object Management**: Add/remove world objects with Pythonic interface
- **Agent Body Information**: Get agent state as Python dictionary
- **Convenience Methods**: Simplified configuration and access
- **State Access**: Get comprehensive world state

```python
# Create enhanced world
enhanced_world = EnhancedSimpleWorld()
enhanced_world.configure(20, 20, 8, 8)
enhanced_world.setAgentStart(10.0, 10.0)

# Get agent body as dictionary
body = enhanced_world.getAgentBody()
print(f"Agent position: ({body['x']}, {body['y']})")
print(f"Agent energy: {body['energy']}")

# Add world objects
enhanced_world.addObject(5.0, 5.0, WorldObjectType.Resource, 10.0, 1.0)
```

### 7. Convenience Functions

The enhanced API includes convenience functions for common workflows:

```python
# Complete agent simulation
results = runCompleteAgentSimulation(1000)
print(f"Simulation completed: {results['success']}")
print(f"Final neurons: {results['final_stats']['total_neurons']}")

# Get default configuration
config = getDefaultConfiguration()
print(config)

# Generate configuration code
code = generateConfigurationCode(config)
print(code)
```

## Advanced Features

### Context Manager Support

Enhanced brain supports context manager for safe operations:

```python
# Safe brain operations with automatic cleanup
with enhanced_brain.createContext() as brain_ctx:
    brain_ctx.step(1)
    brain_ctx.step(2)
    # Brain automatically cleaned up on exit
```

### Progress Monitoring

Track simulation progress with customizable callbacks:

```python
def progress_callback(step, stats):
    if step % 100 == 0:
        print(f"Step {step}: {stats['firing_neurons']} neurons active")

enhanced_brain.runSimulation(1000, progress_callback)
```

### Factory Functions

Create enhanced objects with factory functions:

```python
# All factory functions available
enhanced_config = EnhancedConfig.createEnhancedConfig()
enhanced_brain = EnhancedBrain.createEnhancedBrain(config)
enhanced_agent = EnhancedAgentBrain.createEnhancedAgentBrain(brain)
enhanced_world = EnhancedSimpleWorld.createEnhancedSimpleWorld()
```

## Error Handling Best Practices

### Basic Error Handling

```python
try:
    enhanced_brain = EnhancedBrain(config)
    enhanced_brain.initialize()
    enhanced_brain.step(1)
except (nlm.ConfigurationError, nlm.SimulationError) as e:
    print(f"NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Configuration Validation

```python
def safe_config_operation():
    enhanced_config = EnhancedConfig()
    
    try:
        # Validate configuration before use
        if enhanced_config.has("brain.neuron_count"):
            neurons = enhanced_config.getInt("brain.neuron_count")
            if neurons > 0:
                enhanced_config.configureBrain(neurons, 0.1)
            else:
                raise ConfigurationError("Neuron count must be positive")
    except ConfigurationError as e:
        print(f"Configuration error: {e}")
        # Set safe defaults
        enhanced_config.configureBrain(1000, 0.1)
```

## Migration Guide

### Basic to Enhanced Migration

1. **Import Enhanced Classes**:

```python
# Basic API
from pynlm import createDefaultConfig, createBrain

# Enhanced API (recommended)
from pynlm import EnhancedConfig, EnhancedBrain
```

2. **Configuration**

```python
# Basic
config = createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Enhanced (recommended)
config = EnhancedConfig()
config.configureBrain(1000)  # Single method call
```

3. **Brain Operations**

```python
# Basic
brain = createBrain(config)
brain.initialize()
brain.step(1)

# Enhanced (recommended)
brain = EnhancedBrain(config)
if brain.initialize():
    brain.step(1)
    stats = brain.getStatistics()
```

4. **Agent Operations**

```python
# Basic
agent = createAgentBrain(brain)
agent.initialize(world)

# Enhanced (recommended)
agent = EnhancedAgentBrain(brain)
agent.initialize(world)
state = agent.getAgentState()
```

## Performance Considerations

### Memory Usage

- Enhanced objects use slightly more memory due to additional metadata
- However, the memory overhead is minimal compared to simulation data
- Consider using `enhanced_brain.reset()` to free memory between simulations

### Simulation Performance

- Enhanced API adds minimal overhead to core simulation operations
- Progress callbacks have minimal performance impact when disabled
- Type-safe methods may be slightly slower than direct C++ API calls

## Best Practices

### 1. Use Type-Safe Access

Always use type-safe getters and setters to avoid runtime errors:

```python
# Good practice
if config.has("brain.neuron_count"):
    neurons = config.getInt("brain.neuron_count")
    config.set("brain.neuron_count", neurons)
```

### 2. Handle Errors Gracefully

Always handle NLM exceptions:

```python
try:
    enhanced_brain.step(1)
except SimulationError as e:
    print(f"Simulation failed: {e}")
    # Attempt recovery or fallback
```

### 3. Monitor Progress in Long Simulations

Use progress callbacks for simulations longer than 100 steps:

```python
def monitor_progress(step, stats):
    if step % 500 == 0:
        print(f"Progress: {step} steps, {stats['firing_neurons']} neurons firing")

enhanced_brain.runSimulation(10000, monitor_progress)
```

### 4. Use Context Managers

For complex operations, use context managers:

```python
# Safe resource management
with enhanced_brain.createContext() as ctx:
    for i in range(100):
        ctx.step(i)
    # Automatic cleanup on exit
```

### 5. Validate Configuration

Always validate configuration before simulation:

```python
def validate_config(config):
    required_keys = ["brain.neuron_count", "brain.synapse_density"]
    for key in required_keys:
        if not config.has(key):
            raise ConfigurationError(f"Missing required key: {key}")
    
    neurons = config.getInt("brain.neuron_count")
    if neurons <= 0:
        raise ConfigurationError(f"Invalid neuron count: {neurons}")

validate_config(config)
```

## Examples

### Example 1: Simple Brain Simulation

```python
import pynlm

# Create enhanced brain with simple configuration
config = pynlm.EnhancedConfig()
config.configureBrain(500, 0.05)

brain = pynlm.EnhancedBrain(config)
if brain.initialize():
    # Run simulation
    brain.runSimulation(100)
    
    # Get results
    stats = brain.getStatistics()
    print(f"Simulation completed: {stats['total_spikes']} spikes")
```

### Example 2: Agent with Learning

```python
import pynlm

# Setup environment
config = pynlm.EnhancedConfig()
config.configureBrain(1000)

brain = pynlm.EnhancedBrain(config)
brain.initialize()

world = pynlm.EnhancedSimpleWorld()
world.configure(15, 15, 8, 8)
world.reset()
world.setAgentStart(7.5, 7.5)

agent = pynlm.EnhancedAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(200):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward and apply learning
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)

# Get final statistics
state = agent.getAgentState()
print(f"Final curiosity: {state['curiosity_level']}")
print(f"Final novelty: {state['novelty_level']}")
print(f"Development stage: {state['developmental_stage']}")
```

### Example 3: Complete Simulation with Progress Monitoring

```python
import pynlm

def progress_callback(step, stats):
    if step % 50 == 0:
        print(f"Step {step}: {stats['firing_neurons']} neurons firing, "
              f"curiosity: {stats.get('curiosity_level', 0):.2f}")

# Run complete simulation
results = pynlm.runCompleteAgentSimulation(500)

if results['success']:
    print("✓ Simulation completed successfully!")
    final_stats = results['final_stats']
    print(f"  Total neurons: {final_stats['total_neurons']}")
    print(f"  Total synapses: {final_stats['total_synapses']}")
    print(f"  Final firing rate: {final_stats['average_firing_rate']:.2f}")
    print(f"  Final curiosity: {final_stats['final_curiosity']:.2f}")
else:
    print(f"✗ Simulation failed: {results['error']}")
```

### Example 4: Configuration Builder

```python
import pynlm

# Create configuration using helper methods
config = pynlm.EnhancedConfig()
config.configureBrain(2000, 0.1)
config.configureMemory(10000)
config.configureNeuromodulation(1.5, True)

# Add custom settings
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("development.initial_plasticity", 1.0)

# Save configuration
code = pynlm.generateConfigurationCode(config)
print("Configuration code:")
print(code)

# Run simulation with custom config
brain = pynlm.EnhancedBrain(config)
if brain.initialize():
    brain.runSimulation(100)
```

## Testing and Validation

The enhanced API includes comprehensive tests covering:

- **Basic Operations**: Configuration, brain operations, agent operations
- **Error Handling**: Invalid inputs, missing configuration, simulation errors
- **Progress Monitoring**: Callback functionality and statistics
- **Backward Compatibility**: Basic API still works alongside enhanced API
- **Integration Tests**: Complete agent simulation workflows

Run tests with:

```bash
python -m pytest test_enhanced_api.py -v
```

## Version Information

- **Enhanced API Version**: 2.0.0
- **Minimum Python Version**: 3.8
- **C++ Standard**: C++20
- **Build System**: scikit-build-core with CMake

## Dependencies

### Python Dependencies

```bash
pip install numpy pytest scikit-build-core pybind11
```

### Build Dependencies

```bash
# Ubuntu/Debian
cmake g++ make

# macOS
xcode-select --install

# Windows
Visual Studio 2019+ with C++ support
```

## Build Instructions

### Build Enhanced API

```bash
# Clone repository
cd /path/to/nlm

# Build with enhanced Python bindings
pip install -e .  # This builds both basic and enhanced bindings

# Or build specific Python package
pip install -e .[enhanced]
```

### Build Examples

After installation, run examples:

```bash
python examples/example1_basic_simulation.py
python examples/example2_agent_learning.py
python examples/example3_complete_simulation.py
python examples/example4_configuration_builder.py
```

## Troubleshooting

### Common Issues

#### Issue: "Enhanced API not available"

**Cause**: Enhanced Python bindings not built.

**Solution**:

```bash
# Rebuild with enhanced bindings
pip install --force-reinstall .
```

#### Issue: Import Error

**Cause**: Python environment or dependencies issue.

**Solution**:

```bash
# Create virtual environment and install dependencies
python -m venv nlm_env
source nlm_env/bin/activate  # On Windows: nlm_env\Scripts\activate
pip install numpy pytest scikit-build-core pybind11
```

#### Issue: Memory Errors

**Cause**: Large simulation configuration.

**Solution**:

```python
# Reduce configuration size
config = EnhancedConfig()
config.configureBrain(500, 0.05)  # Reduced from 1000, 0.1
```

#### Issue: Simulation Timeout

**Cause**: Very long simulation steps.

**Solution**:

```python
# Add progress monitoring
enhanced_brain.runSimulation(1000, progress_callback)
```

### Error Messages

#### "Configuration error: Key not found: XXX"

**Solution**: Check configuration keys or use `has()` method:

```python
if config.has("brain.neuron_count"):
    value = config.getInt("brain.neuron_count")
```

#### "Simulation error: Brain not initialized"

**Solution**: Always initialize brain before use:

```python
brain = EnhancedBrain(config)
if brain.initialize():
    brain.step(1)  # Safe to step after initialization
```

#### "Agent error: Invalid world configuration"

**Solution**: Validate world configuration before use:

```python
world = EnhancedSimpleWorld()
world.configure(10, 10, 5, 5)  # Validate dimensions
```

## Support and Community

### Documentation

- [Enhanced API Documentation](docs/enhanced_api.md)
- [Python API Reference](docs/python_api.md)
- [Examples and Tutorials](examples/)

### Support

- **Issue Tracker**: [GitHub Issues](https://github.com/nlm-project/nlm/issues)
- **Community Forum**: [Discourse](https://community.nlm.ai)
- **Email Support**: support@nlm.ai

### Contributing

We welcome contributions to the enhanced Python API! Please follow our contribution guidelines.

### Feedback

Please provide feedback on the enhanced API through GitHub issues or the community forum. Your feedback helps us improve the user experience.

## License

This enhanced Python API is part of the NLM (Neural Learning Machine) project.

- **License**: MIT
- **Copyright**: 2024 NLM Authors
- **Version**: 2.0.0-enhanced

## Acknowledgments

The enhanced Python API builds upon the excellent work of:

- The NLM core development team
- pybind11 contributors
- scikit-build-core developers
- Scientific Python community

Special thanks to all contributors who have helped improve the NLM ecosystem.

---

*This document is generated automatically. For the most up-to-date information, visit the official NLM documentation.*
