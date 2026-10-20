# NLM Python Bindings - Comprehensive Documentation

## Overview

NLM (Neural Learning Machine) Python bindings provide a comprehensive interface to the NLM C++ neural simulation framework. This documentation covers all features from basic usage to advanced Phase 6 integration capabilities.

## Installation

The NLM Python bindings are built as part of the NLM framework. After building the C++ project with scikit-build, the `pynlm` module will be available in your Python environment.

```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install -e .
```

## Version Information

- **Python Bindings Version**: 0.1.0 - Phase 6 Integration
- **C++ Framework Version**: Phase 6
- **License**: MIT
- **Documentation**: Generated from source code and examples

## Quick Start - Beginner Examples

### Example 1: Silent Brain (Just Neurons)

```python
import pynlm

# Create a virtual brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize the brain
brain.initialize()

# Make the brain think for 10 steps
for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print("Neurons:", brain.getTotalNeuronCount())
print("Spikes:", brain.getTotalSpikeCount())
```

### Example 2: Complete Agent with World Interaction

```python
import pynlm

# Setup configuration and brain
config = pynlm.createExperimentConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run complete agent simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

### Example 3: Integration Test with System Monitoring

```python
import pynlm

# Run Phase 6 integration test with advanced monitoring
results = pynlm.run_integration_test("./nlm_checkpoint.bin")

print("=== INTEGRATION TEST RESULTS ===")
print(f"Total Reward: {results['total_reward']:.2f}")
print(f"Average Firing Rate: {results['avg_firing_rate']:.2f} Hz")
print(f"Average Firing Count: {results['avg_firing_count']:.0f}")
print(f"Neuromodulation Level: {results['neuromodulation_level']:.3f}")
print(f"Curiosity Level: {results['curiosity_level']:.3f}")
print(f"Novelty Level: {results['novelty_level']:.3f}")
print(f"Prediction Error: {results['prediction_error']:.3f}")
print(f"Total Steps: {results['total_steps']:.0f}")
```

## Advanced Features and Commands

### 1. System Capabilities and Status

```python
import pynlm

# Get comprehensive system capabilities
capabilities = pynlm.get_system_capabilities()
print("=== SYSTEM CAPABILITIES ===")
for category, features in capabilities.items():
    print(f"\n{category.upper()}:")
    for feature in features:
        print(f"  • {feature}")

# Get current system status
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
agent.initialize(pynlm.createSimpleWorld())

status = pynlm.get_system_status(agent, brain)
print("\n=== SYSTEM STATUS ===")
for key, value in status.items():
    print(f"{key}: {value}")
```

### 2. Memory System Monitoring

```python
import pynlm

brain = pynlm.createBrain(pynlm.createExperimentConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Get memory system statistics
memory_stats = pynlm.get_memory_stats(brain)
print("=== MEMORY SYSTEM STATISTICS ===")
for key, value in memory_stats.items():
    print(f"{key}: {value}")

# Simulate some activity and check changes
for i in range(100):
    brain.step(i)

memory_stats_after = pynlm.get_memory_stats(brain)
print("\n=== AFTER SIMULATION ===")
for key, value in memory_stats_after.items():
    print(f"{key}: {value}")
```

### 3. Development and Maturation Monitoring

```python
import pynlm

# Create agent with development enabled
agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Monitor development over time
print("=== DEVELOPMENT MONITORING ===")
for day in range(0, 30, 5):
    stats = pynlm.get_development_stats(agent)
    print(f"Day {day}:")
    print(f"  Developmental Stage: {stats['developmental_stage']}")
    print(f"  Neuromodulation: {stats['neuromodulation_level']:.3f}")
    print(f"  Curiosity: {stats['curiosity_level']:.3f}")
    print(f"  Novelty: {stats['novelty_level']:.3f}")
    
    # Simulate development progress
    for step in range(10):
        agent.updateDevelopment(0.001)
```

### 4. Prediction System Access

```python
import pynlm

brain = pynlm.createBrain(pynlm.createExperimentConfig())
brain.initialize()

# Get prediction system statistics
pred_stats = pynlm.get_prediction_stats(brain)
print("=== PREDICTION SYSTEM STATISTICS ===")
for key, value in pred_stats.items():
    print(f"{key}: {value}")

# Simulate cognitive tasks that generate prediction errors
for i in range(50):
    # Inject some variability to generate prediction errors
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 0.1f)
    brain.step(i, i * 0.001)

pred_stats_after = pynlm.get_prediction_stats(brain)
print("\n=== AFTER PREDICTION TASKS ===")
for key, value in pred_stats_after.items():
    print(f"{key}: {value}")
```

### 5. Advanced Configuration Management

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Configure for specific use case
config.set_int("random_seed", 12345)
config.set_float("simulation_timestep", 0.0005)
config.set_int("neuron_count", 2000)
config.set_int("region_count", 3)
config.set_float("connection_probability", 0.08)

# Validate configuration
errors = pynlm.validate_config(config)
if errors:
    print("Configuration errors:", errors)
else:
    print("Configuration is valid!")

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()

# Reset to defaults if needed
reset_config = pynlm.reset_to_defaults(pynlm.createDefaultConfig())
print("Reset configuration:", reset_config.summary())
```

### 6. Phase 6 Experiment Configuration

```python
import pynlm

# Create Phase 6 integration configuration
phase6_config = pynlm.create_phase6_config(
    neuronCount=1500,
    maxSteps=8000,
    enableCheckpointing=True,
    enableReplay=True,
    enableDevelopment=True
)

print("=== PHASE 6 CONFIGURATION ===")
print(f"Neuron Count: {phase6_config.neuronCount}")
print(f"Max Steps: {phase6_config.maxSteps}")
print(f"Enable Checkpointing: {phase6_config.enableCheckpointing}")
print(f"Enable Replay: {phase6_config.enableReplay}")
print(f"Enable Development: {phase6_config.enableDevelopment}")

# Run Phase 4-6 experiment
print("\n=== RUNNING PHASE 4-6 EXPERIMENT ===")
experiment_results = pynlm.run_phase456_experiment(phase6_config)

print("=== EXPERIMENT RESULTS ===")
for key, value in experiment_results.items():
    print(f"{key}: {value:.3f}" if isinstance(value, float) else f"{key}: {value}")
```

## Error Handling and Validation

### Exception Types

The NLM Python bindings provide specific exception types for different error conditions:

```python
import pynlm

# These exception types are available in the pynlm module
try:
    # Code that might raise exceptions
    brain = pynlm.createBrain(None)  # This would cause an error
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except pynlm.InvalidArgumentError as e:
    print(f"Invalid argument: {e}")
except pynlm.OutOfRangeError as e:
    print(f"Out of range: {e}")
```

### Configuration Validation

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set_int("random_seed", 42)

# Validate configuration
errors = pynlm.validate_config(config)
if errors:
    print("Please fix these configuration issues:")
    for error in errors:
        print(f"  • {error}")
else:
    print("Configuration is valid!")
```

## Debugging and Monitoring

### Getting System Statistics

```python
import pynlm

# Create agent with all learning features enabled
agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createExperimentConfig()))
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Get comprehensive statistics
brain = agent.getBrain()
agent.initialize(pynlm.createSimpleWorld())

print("=== COMPREHENSIVE SYSTEM MONITORING ===")
print(f"Brain Statistics:")
print(f"  Neurons: {brain.getTotalNeuronCount()}")
print(f"  Synapses: {brain.getTotalSynapseCount()}")
print(f"  Spikes: {brain.getTotalSpikeCount()}")
print(f"  Firing Rate: {brain.getAverageFiringRate():.2f} Hz")

print(f"\nAgent Statistics:")
agent_stats = pynlm.get_development_stats(agent)
for key, value in agent_stats.items():
    print(f"  {key}: {value:.3f}")

print(f"\nMemory Statistics:")
memory_stats = pynlm.get_memory_stats(brain)
for key, value in memory_stats.items():
    print(f"  {key}: {value:.1f}")
```

## Best Practices and Tips

### 1. Configuration Management

```python
# Always validate configuration before use
def safe_create_brain(config=None):
    if config is None:
        config = pynlm.createExperimentConfig()
    
    errors = pynlm.validate_config(config)
    if errors:
        raise ValueError(f"Invalid configuration: {errors}")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain
```

### 2. Error Recovery

```python
import pynlm

brain = safe_create_brain()

try:
    for step in range(100):
        brain.step(step)
        
        # Check for errors periodically
        if brain.getTotalSpikeCount() > 10000:
            print(f"Warning: High spike count at step {step}")
            
except pynlm.RuntimeError as e:
    print(f"Simulation failed: {e}")
    # Attempt recovery
    brain.reset()
    brain.initialize()
except Exception as e:
    print(f"Unexpected error: {e}")
    # Log error and continue or exit gracefully
```

### 3. Performance Monitoring

```python
import time
import pynlm

brain = pynlm.createBrain(pynlm.createExperimentConfig())
brain.initialize()

print("=== PERFORMANCE MONITORING ===")
start_time = time.time()
step_times = []

for step in range(100):
    step_start = time.time()
    brain.step(step)
    step_end = time.time()
    step_times.append(step_end - step_start)
    
    if step % 20 == 0:
        avg_step_time = sum(step_times) / len(step_times)
        print(f"Step {step}: {step_times[-1]*1000:.1f}ms avg: {avg_step_time*1000:.1f}ms")

total_time = time.time() - start_time
print(f"Total simulation time: {total_time:.2f}s")
print(f"Average step time: {sum(step_times)/len(step_times)*1000:.1f}ms")
```

## Common Issues and Solutions

### Issue 1: Brain Initialization Fails

**Problem**: `RuntimeError: Failed to initialize brain`

**Solution**:
```python
import pynlm

# Check configuration first
config = pynlm.createDefaultConfig()
errors = pynlm.validate_config(config)
if errors:
    print("Fix configuration issues:", errors)
    # Or use experiment config which is more robust
    config = pynlm.createExperimentConfig()

brain = pynlm.createBrain(config)
brain.initialize()  # Should now work
```

### Issue 2: Memory Issues

**Problem**: Memory usage grows too high during long simulations

**Solution**:
```python
import pynlm

# Use smaller configurations for long runs
config = pynlm.createExperimentConfig()
config.set_int("neuron_count", 500)  # Smaller brain
config.set_int("max_simulation_steps", 5000)  # Fewer steps

brain = pynlm.createBrain(config)
brain.initialize()

# Add periodic checkpoints for long simulations
for step in range(0, 5000, 500):
    brain.step(step)
    if step % 1000 == 0:
        # Save checkpoint periodically
        brain.save(f"checkpoint_step_{step}.bin")
```

### Issue 3: Learning Not Working

**Problem**: Agent is not learning from experience

**Solution**:
```python
import pynlm

agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createExperimentConfig()))

# Ensure all learning features are enabled
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Verify features are enabled
print(f"Reward Modulation: {agent.isRewardModulationEnabled()}")
print(f"Curiosity: {agent.isCuriosityEnabled()}")
print(f"Development: {agent.isDevelopmentEnabled()}")
print(f"Structural Plasticity: {agent.isStructuralPlasticityEnabled()}")

# Test with reward
world = pynlm.createSimpleWorld()
world.configure(10, 10, 4, 4)
agent.initialize(world)

for step in range(20):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    agent.applyRewardModulation(1.0, 0.0)  # Give reward
```

## API Reference

### Core Classes

#### Config
- `createDefaultConfig()`: Create basic configuration
- `createExperimentConfig()`: Create configuration optimized for experiments
- `validate_config(config)`: Validate configuration and return errors
- `reset_to_defaults(config)`: Reset configuration to sensible defaults

#### Brain
- `createBrain(config)`: Create brain with configuration
- Various step methods for simulation
- `injectCurrent()`, `injectCurrentToNeurons()`: Neural stimulation
- `getTotalNeuronCount()`, `getTotalSynapseCount()`: Statistics
- `save()`, `load()`: Checkpoint persistence

#### AgentBrain
- `createAgentBrain(brain)`: Create agent interface
- `enableRewardModulation()`, `enableCuriosity()`, etc.: Feature control
- `getNeuromodulationLevel()`, `getCuriosityLevel()`: System monitoring
- `decodeMotorCommand()`: Action selection
- `applyRewardModulation()`: Learning

#### SimpleWorld
- `createSimpleWorld()`: Create simulation world
- `configure(width, height, visionWidth, visionHeight)`: World setup
- `update(timestep)`: World advancement
- `applyMotorCommand(cmd, time)`: Action execution

### Module-Level Functions

#### Advanced Features
- `get_advanced_features()`: List of available features
- `enable_advanced_learning(agent)`: Enable all advanced learning
- `get_system_capabilities()`: Comprehensive system capabilities
- `get_system_status(agent, brain)`: Current system status

#### Integration Testing
- `run_integration_test(checkpointFile)`: Phase 6 integration test
- `create_phase6_config(...)`: Create Phase 6 experiment configuration
- `run_phase456_experiment(config)`: Run Phase 4-6 simulation

#### Monitoring
- `get_memory_stats(brain)`: Memory system statistics
- `get_prediction_stats(brain)`: Prediction system statistics
- `get_development_stats(agent)`: Development and maturation statistics

## Version and Compatibility

### Supported Python Versions
- Python 3.8+
- Python 3.9
- Python 3.10
- Python 3.11
- Python 3.12

### Dependencies
- pybind11 >= 2.11.0
- NumPy (optional, for advanced features)
- Standard library only for core functionality

### Compatibility Notes

1. **Version 0.1.0**: First release with full Phase 6 integration
2. **Binary Compatibility**: Python bindings are tied to C++ framework version
3. **API Stability**: Core classes and methods are stable, advanced features may evolve

## Getting Help

### Documentation
- This file: Comprehensive user documentation
- `easy_usage.md`: Quick start guide for beginners
- `HOW_TO_USE.md`: Detailed usage instructions
- `docs/ARCHITECTURE.md`: Technical architecture documentation

### Examples
- All examples in this documentation are runnable
- Test scripts in `tests/` directory demonstrate various use cases
- Experiment scripts in `experiments/` show advanced usage

### Community
- GitHub repository: Available in the NLM project
- Issue tracking: Available through project channels
- Discussion forums: For community support

## Acknowledgments

This documentation and API represents the culmination of Phase 6 integration in the NLM framework. It provides:

- **Comprehensive Integration**: All major brain systems work together seamlessly
- **Advanced Learning**: Reward modulation, curiosity, development, and structural plasticity
- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Cognitive Capabilities**: Prediction, planning, and attention systems
- **Persistence**: Checkpoint save/load and replay mechanisms
- **Developmental Progression**: Maturation from initial to adult stages
- **Embodied Intelligence**: Sensory-motor loops with real-world interaction

The NLM Python bindings provide a powerful interface for research and development in artificial neural systems, enabling both beginners and advanced users to explore complex brain-like intelligence.

---

*Last updated: October 2026*
*NLM Python Bindings v0.1.0 - Phase 6 Integration*