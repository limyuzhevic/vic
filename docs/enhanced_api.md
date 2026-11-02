# Enhanced NLM API Documentation
# Complete API Reference for NLM v2.6.0

This document provides comprehensive API documentation for the enhanced NLM (Neural Learning Machine) framework, including both the original pynlm bindings and the new enhanced functionality.

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain system that implements:

- Real spiking neural computation with Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- Multiple plasticity mechanisms (STDP, Hebbian, structural)
- Integrated memory systems (working memory, episodic memory, associative memory)
- Neuromodulation (dopamine, curiosity, novelty)
- Cognitive systems (planning, attention, concept formation)
- Predictive systems (prediction and error computation)
- Development stages and structural plasticity
- Advanced learning algorithms and optimization

## API Structure

### Core Classes

#### 1. `nlm.Brain` - The Main Neural System

**Main Components:**
- **Neurons**: LIF neurons with membrane dynamics, firing states, and refractory periods
- **Synapses**: Event-driven connections with plastic weights and delays
- **Regions**: Organized neuron populations for different functions
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
- **Cognition**: Planning, attention, concept formation
- **Development**: Age-dependent plasticity and maturation

**Key Methods:**

```python
# Brain Creation and Initialization
brain = nlm.Brain.create(config)  # Legacy (use EnhancedConfig.createBrain)
brain = nlm.EnhancedConfig().createBrain()  # Enhanced API
brain.initialize()

# Configuration Management
config = nlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

# Simulation
brain.step(step_number)                    # Run one simulation step
brain.step(step, time)                     # Run with explicit time
brain.reset()                             # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()            # Get motor action

# Statistics
brain.getTotalNeuronCount()               # Total neurons
brain.getTotalSynapseCount()              # Total synapses
brain.getFiringNeuronCount()             # Currently firing neurons
brain.getAverageFiringRate()              # Average firing rate (Hz)
brain.getExcitationInhibitionRatio()     # E/I balance
brain.getDevelopmentalStage()            # Current developmental stage

# Memory Systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Neuromodulation
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()

# Cognition
planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attention = brain.getAttention()

# Development
brain.getDevelopmentSystem().getStage()
brain.develop()                          # Apply development effects

# Plasticity
brain.getSTDP().configure(ltp_weight, ltd_weight, tau)
brain.getHebbian().configure(learning_rate)
brain.getStructuralPlasticity().update(brain, rng)

# Checkpointing
brain.save("checkpoint.bin")             # Save brain state
brain.load("checkpoint.bin")             # Load brain state
```

#### 2. `nlm.EnhancedConfig` - Advanced Configuration

**Configuration Profiles:**

```python
# Get configuration manager
config = nlm.EnhancedConfig()

# Apply predefined profiles
config.createDefault().setLearningConfig().setMemoryConfig()  # Learning-focused
config.createDefault().setDevelopmentConfig()                   # Development-focused
config.createDefault().setMemoryConfig()                          # Memory-focused

# Configuration Keys:
# Brain Parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.connection_probability", 0.05)

# Plasticity Configuration
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.01)
config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.0001)

# Neuromodulation Configuration
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Memory Configuration
config.set("memory.working_memory.capacity", 100)
config.set("memory.episodic_memory.max_episodes", 1000)

# Development Configuration
config.set("development.enabled", True)
config.set("development.initial_plasticity", 1.0)
config.set("development.critical_period_length", 500)

# Advanced Configuration
config.set("replay_interval", 100)
config.set("consolidation_interval", 1000)
config.set("checkpoint_dir", "./checkpoints")
```

#### 3. `nlm.AgentBrain` - Agent Interface

**Agent Capabilities:**

```python
# Create agent brain interface
agent = nlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Processing Loop
def run_agent_simulation(agent, world, num_steps):
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        reward = extract_reward_from_percept(percept)
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)

# Agent Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### 4. `nlm.SimpleWorld` - Environment Simulation

**World Capabilities:**

```python
# Create simple world
world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# World Operations
world.reset()                              # Reset to initial state
world.update(0.1)                         # Update world state
action = world.applyMotorCommand(cmd, time)  # Execute action

# State Access
percept = world.getSensoryPercept()        # Get current perception
body = world.getAgentBody()               # Get agent state

# World Objects
world.addObject(pynlm.WorldObject(x, y, type, value))
world.removeObject(x, y)
world.isValidPosition(x, y)

# World Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
```

## Convenience Functions

### Brain Creation Presets

```python
import nlm

# Create different types of brains
minimal_brain = nlm.convenience.create_minimal_brain(100)
learning_brain = nlm.convenience.create_learning_brain(500)
developmental_brain = nlm.convenience.create_developmental_brain(1000)

# Run simulation with helper
nlm.convenience.run_for_steps(brain, 1000, callback=lambda step, b: print(f"Step {step}"))
```

### Analysis Tools

```python
# Get comprehensive brain statistics
stats = nlm.convenience.get_brain_statistics(brain)
# Returns dict with: neuron_count, synapse_count, total_spikes, firing_neurons, avg_firing_rate, e_i_ratio, developmental_stage, learning_score

# Calculate learning score
learning_score = nlm.convenience.calculate_learning_score(brain)

# Get weight distribution
weights = nlm.convenience.get_weight_distribution(brain)
```

### High-Level Simulation Helpers

```python
# Complete sensory-motor loop
nlm.convenience.run_sensory_motor_loop(brain, world, agent, 1000)

# Pattern learning
pattern = [0.5, 0.7, 0.9, 0.8, 0.6]
nlm.convenience.learn_pattern(brain, pattern, 100)

# Memory system training
nlm.convenience.train_memory_system(brain, 100)
```

## Enhanced API Features

### 1. Convenience Module

```python
import nlm.convenience as conv

# All convenience functions are available here
brain = conv.createLearningBrain()
stats = conv.get_brain_statistics(brain)
conv.run_sensory_motor_loop(brain, world, agent, 500)
```

### 2. Error Handling and Validation

```python
import nlm.enhanced as enhanced

# Validate configuration
errors = enhanced.validateBrainConfig(config)
if errors:
    print("Configuration errors:", errors)

# Safe operations
if enhanced.safe_operation(brain, some_function, "operation_name"):
    print("Operation succeeded")

# Exception handling
try:
    result = enhanced.risky_operation(brain)
except enhanced.NLMRuntimeError as e:
    print(f"Error: {e}")
```

### 3. Advanced Configuration Management

```python
# Configuration factory
config = nlm.EnhancedConfig()
config.setLearningConfig().setMemoryConfig()  # Method chaining

# Profile-based configuration
nlm.EnhancedConfig().createLearningBrain(500)
nlm.EnhancedConfig().createDevelopmentalBrain(1000)
nlm.EnhancedConfig().createMinimalBrain(100)

# Configuration validation
validator = nlm.EnhancedConfig().createValidator()
if not validator.isValid(config):
    print("Configuration issues:", validator.getErrors())
```

### 4. Performance Monitoring

```python
import nlm.performance

# Get system performance
metrics = nlm.performance.getPerformanceMetrics(brain)
print(f"CPU Usage: {metrics.cpu_usage}%")
print(f"Memory Usage: {metrics.memory_usage}%")
print(f"Spike Throughput: {metrics.spike_throughput} spikes/sec")

# Benchmark simulation
benchmark = nlm.benchmark.simulation(brain, 1000)
print(f"Simulation speed: {benchmark.steps_per_second} steps/sec")
```

### 5. Experiment Management

```python
import nlm.experiments

# Create experiment
experiment = nlm.experiments.createExperiment("pattern_learning", brain,
                                             {"epochs": 100, "pattern_size": 5})

# Run experiment
results = nlm.experiments.run(experiment)
print(f"Experiment completed: {results.success}")
print(f"Results: {results.metrics}")

# Log experiment data
nlm.experiments.logData(experiment, "experiment_log.csv")
```

### 6. Debugging and Analysis Tools

```python
import nlm.debug

# Analyze activation patterns
activation = nlm.debug.analyzeActivationPattern(brain, 0, 100)
nlm.debug.plotActivationPattern(activation)

# Export data for external analysis
export_data = nlm.debug.exportForAnalysis(brain)
with open("brain_data.json", "w") as f:
    json.dump(export_data, f)

# Memory analysis
memory_stats = nlm.debug.analyzeMemoryUsage(brain)
print(f"Memory usage: {memory_stats.used_memory}/{memory_stats.total_memory} bytes")
```

## Python Language Features

### 1. Enhanced Error Types

```python
# Specific error types for different failure modes
nlm.NLMRuntimeError      # General runtime errors
nlm.NLMInvalidArgumentError  # Invalid argument errors
nlm.NLMOutOfRangeError   # Out of range errors
nlm.NLMConfigurationError   # Configuration errors
```

### 2. Python Decorators

```python
# Decorators for function wrapping
@nlm.decorator
async def async_simulation(brain, steps):
    await brain.step(steps)

# Context managers for resource management
with nlm.scoped_guard(brain) as guarded_brain:
    # Brain operations here
    pass
```

### 3. Type Hints and Intellisense

```python
# Enhanced type hints for better IDE support
from typing import Optional, List, Dict, Tuple, Any

brain: nlm.Brain
config: nlm.EnhancedConfig
world: nlm.SimpleWorld
agent: nlm.AgentBrain

# Property access with type hints
neuron_count: int = brain.getTotalNeuronCount()
firing_rate: float = brain.getAverageFiringRate()
development_stage: nlm.DevelopmentalStage = brain.getDevelopmentalStage()
```

### 4. Context Managers

```python
# Context manager for brain lifecycle
with nlm.BrainContext(config) as brain:
    brain.initialize()
    # Brain operations here
    # Automatic cleanup on exit

# Context manager for experiment runs
with nlm.ExperimentContext(brain, experiment_config) as experiment:
    experiment.run()
    # Automatic logging and cleanup
```

### 5. Iterator Support

```python
# Iteration over simulation steps
brain = nlm.convenience.createLearningBrain()
for step in nlm.stepGenerator(brain, 1000):
    print(f"Processed step {step}")

# Generator-based simulation
steps = nlm.generateSteps(brain, 1000)
for step in steps:
    # Process each step
    pass
```

### 6. Pattern Matching

```python
# Pattern matching for different action types
match action = brain.produceAction():
    nlm.ActionType.MoveForward:
        print("Moving forward")
    nlm.ActionType.TurnLeft:
        print("Turning left")
    nlm.ActionType.Interact:
        print("Interacting")
    _:
        print("Unknown action")
```

### 7. Serialization

```python
# JSON serialization
brain_data = nlm.toJSON(brain)
with open("brain_state.json", "w") as f:
    json.dump(brain_data, f)

# Deserialization
loaded_brain = nlm.fromJSON(brain_data)
```

### 8. Benchmarking

```python
# Performance benchmarking
import time

start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

benchmark = nlm.benchmark(simulation=brain, steps=1000)
print(f"Performance: {benchmark.steps_per_second:.2f} steps/sec")
```

### 9. Progress Tracking

```python
# Progress bar for long operations
nlm.progressBar(total_steps, brain.step)

# Custom progress callback
nlm.progressBar(1000, lambda step: print(f"Progress: {step}/1000"))
```

### 10. Logging

```python
# Enhanced logging
nlm.setupLogging("INFO", "nlm.log")
nlm.logMessage("Brain initialized", "startup")
nlm.logError("Failed to initialize", "initialization", exception)
```

## Migration Guide

### From v2.5 to v2.6

1. **New Enhanced API**: Replace `nlm.Brain` with `nlm.EnhancedBrain` for enhanced functionality
2. **Configuration**: Use `nlm.EnhancedConfig()` instead of `nlm.Config()`
3. **Convenience Functions**: Import `nlm.convenience` for high-level helpers
4. **Error Handling**: Use enhanced error types for better error messages
5. **Performance**: Use `nlm.performance` module for system monitoring

```python
# v2.5 style
brain = nlm.Brain(config)
brain.initialize()

# v2.6 style (recommended)
from nlm.enhanced import EnhancedBrain, EnhancedConfig
config = EnhancedConfig().setLearningConfig()
brain = EnhancedBrain.create(config)
brain.initialize()
```

## Installation and Setup

### Python Package Installation

```bash
# Install from source
pip install .

# Install with enhanced features
pip install .[enhanced]

# Install with all optional dependencies
pip install .[all]
```

### Building from Source

```bash
# Clone repository
mkdir nlm
cd nlm
git clone https://github.com/nlm-research/nlm.git

cd nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install .
```

### Environment Setup

```python
import nlm
import nlm.convenience
import nlm.enhanced
import nlm.performance
import nlm.experiments
import nlm.debug

# Verify installation
print(f"NLM Version: {nlm.__version__}")
print(f"Enhanced API Available: {hasattr(nlm, 'EnhancedConfig')}")
print(f"Convenience Module: {hasattr(nlm, 'convenience')}")
```

## Examples

### 1. Basic Learning System

```python
import nlm
import nlm.convenience

# Create learning brain
brain = nlm.convenience.createLearningBrain(500)

# Simple learning loop
for step in range(1000):
    # Simple pattern learning
    if step % 10 == 0:
        pattern = [0.5, 0.7, 0.9, 0.8, 0.6]
        nlm.convenience.learnPattern(brain, pattern, 1)
    
    brain.step(step)
    
    if step % 100 == 0:
        stats = nlm.convenience.get_brain_statistics(brain)
        print(f"Step {step}: {stats['learning_score']:.3f} learning score")
```

### 2. Complete Agent Simulation

```python
import nlm
import nlm.convenience

# Setup complete agent system
brain = nlm.convenience.createLearningBrain(1000)
world = nlm.createSimpleWorld()
world.configure(50, 50, 10, 10)
agent = nlm.createAgentBrain(brain)

# Initialize with enhanced configuration
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run complete simulation
nlm.convenience.run_sensory_motor_loop(brain, world, agent, 500)
```

### 3. Advanced Experiment

```python
import nlm
import nlm.experiments
import nlm.performance

# Create research setup
brain = nlm.enhanced.EnhancedConfig().createLearningBrain(2000)

# Create experiment
experiment = nlm.experiments.createExperiment("memory_training", brain,
                                             {"episodes": 100, "pattern_size": 10})

# Run with monitoring
metrics = nlm.performance.monitorSimulation(
    lambda: nlm.experiments.run(experiment),
    "memory_training_experiment"
)

# Analyze results
results = nlm.analyzeResults(experiment, metrics)
nlm.experiments.logResults(results, "experiment_results.json")
```

### 4. Development and Testing

```python
import nlm
import nlm.debug

# Create brain for testing
test_brain = nlm.convenience.createMinimalBrain(100)

# Run unit tests
test_results = nlm.debug.runTests(test_brain)
print(f"Test Results: {test_results}")

# Analyze brain state
activation = nlm.debug.analyzeActivationPattern(test_brain, 0, 50)
nlm.debug.visualizeActivation(activation)

# Performance testing
performance = nlm.debug.benchmark(brain, 1000)
print(f"Performance: {performance.steps_per_second:.2f} steps/sec")
```

## Troubleshooting

### Common Issues and Solutions

1. **Import Errors**
   ```python
   # Error: No module named 'nlm.enhanced'
   # Solution: Install enhanced features
   pip install .[enhanced]
   
   # Error: No module named 'nlm.performance'
   # Solution: Install performance features
   pip install .[performance]
   ```

2. **Brain Initialization Failures**
   ```python
   # Error: Failed to initialize brain
   # Solution: Check configuration
   config = nlm.EnhancedConfig()
   if not nlm.validateBrainConfig(config):
       print("Fix configuration errors:", nlm.getValidationErrors(config))
   ```

3. **Memory Issues**
   ```python
   # Error: Out of memory
   # Solution: Use smaller brain configuration
   brain = nlm.convenience.createMinimalBrain(100)
   ```

4. **Simulation Performance**
   ```python
   # Error: Simulation too slow
   # Solution: Use performance-optimized configuration
   brain = nlm.EnhancedConfig().createMinimalBrain(100)
   ```

### Error Messages and Meanings

- **NLMRuntimeError**: General runtime error in NLM
- **NLMInvalidArgumentError**: Invalid argument provided
- **NLMOutOfRangeError**: Value outside valid range
- **NLMConfigurationError**: Configuration validation failed
- **NLMBrainError**: Brain-specific error (initialization, memory, etc.)
- **NLMNeuromodulationError**: Neuromodulation system error
- **NLMPlasiticityError**: Plasticity system error
- **NLMMemoryError**: Memory system error

## Best Practices

### 1. Configuration Management

```python
# Use configuration profiles for consistent setups
def getLearningConfig():
    return (nlm.EnhancedConfig()
            .createLearningBrain(500)
            .set("plasticity.stdp.enable", True)
            .set("neuromod.curiosity.enable", True))

# Validate configuration before use
config = getLearningConfig()
if not nlm.validateBrainConfig(config):
    raise ValueError("Invalid configuration")
```

### 2. Error Handling

```python
# Use try-catch blocks for robust error handling
try:
    result = nlm.complex_operation(brain, config)
except nlm.NLMRuntimeError as e:
    nlm.logError("Operation failed", "complex_operation", e)
    # Attempt recovery or fallback
except nlm.NLMConfigurationError as e:
    nlm.logError("Configuration error", "setup", e)
    # Fix configuration and retry
```

### 3. Performance Optimization

```python
# Use appropriate brain sizes for different use cases
# For development/testing: use minimal brain
brain = nlm.convenience.createMinimalBrain(100)

# For production: use optimized brain
brain = nlm.EnhancedConfig().createLearningBrain(500)

# For research: use full-scale brain
brain = nlm.EnhancedConfig().createDevelopmentalBrain(2000)
```

### 4. Memory Management

```python
# Monitor memory usage during long simulations
import nlm.performance

# Get memory statistics before and after
before_stats = nlm.performance.getMemoryStats()
# Run simulation
nlm.convenience.run_sensory_motor_loop(brain, world, agent, 1000)
after_stats = nlm.performance.getMemoryStats()

# Check for memory issues
if after_stats.used_memory > before_stats.used_memory * 1.5:
    print("Warning: High memory usage detected")
```

### 5. Experiment Tracking

```python
# Use experiment tracking for research
import nlm.experiments

# Create experiment with metadata
experiment = nlm.experiments.createExperiment(
    "pattern_learning",
    brain,
    {
        "epochs": 100,
        "pattern_size": 5,
        "learning_rate": 0.01,
        "model_version": "2.6.0"
    }
)

# Run experiment with automatic logging
results = nlm.experiments.run(experiment)

# Log results for analysis
nlm.experiments.logResults(results, "results.json")
nlm.experiments.logData(experiment, "experiment_data.csv")
```

## Future Enhancements

### Planned Features (v2.7+)

1. **Advanced Neural Architecture**: Support for custom neural architectures
2. **Distributed Computing**: Multi-GPU and multi-machine support
3. **Real-time Simulation**: Live visualization and interaction
4. **Model Compression**: Support for exporting and loading models
5. **Hardware Acceleration**: Direct GPU support for neural computation
6. **Cloud Integration**: Support for cloud-based simulation
7. **Advanced Learning**: Reinforcement learning and meta-learning support
8. **Cross-language Support**: JavaScript, Rust, and other language bindings
9. **Plugin System**: Extensible plugin architecture for custom components
10. **Advanced Visualization**: 3D neural network visualization

## Support and Community

### Getting Help

1. **Documentation**: Visit [NLM Documentation](http://nlm.readthedocs.io)
2. **GitHub Issues**: Report bugs and request features at [GitHub Issues](https://github.com/nlm-research/nlm/issues)
3. **Discord**: Join our community at [NLM Discord](https://discord.gg/nlm)
4. **Slack**: Get help in our Slack workspace
5. **Email**: Contact support at [support@nlm.ai](mailto:support@nlm.ai)

### Contributing

1. **Fork Repository**: Create your own fork of the NLM repository
2. **Report Issues**: Use GitHub Issues to report bugs and request features
3. **Submit Pull Requests**: Submit pull requests for enhancements and bug fixes
4. **Code Reviews**: Participate in code reviews and discussions
5. **Documentation**: Help improve documentation and examples

### Academic Use

NLM is available for academic research and educational purposes under the MIT License. For commercial use, please contact us at [licensing@nlm.ai](mailto:licensing@nlm.ai).

## License

NLM is licensed under the MIT License. See the LICENSE file for more information.

## Copyright

Copyright 2026 NLM Research Team. All rights reserved.

---
*This documentation is continuously updated with new features and enhancements. For the latest information, visit the NLM documentation website.*