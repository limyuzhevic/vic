# NLM Advanced Usage Guide

This document provides comprehensive guidance for advanced users of the NLM (Neural Learning Machine) system, covering the new features added in Phase 6.

## Overview

NLM Phase 6 represents the final integration of all artificial brain components into a coherent, self-organizing system. This guide covers:

- Advanced CLI usage
- Python API extensions
- Configuration management
- Performance optimization
- Experiment design and analysis
- Debugging and troubleshooting

## 1. Command-Line Interface (CLI) Advanced Usage

### Available Modes

The NLM CLI now supports multiple operation modes:

#### `--mode experiment` (Default)
Runs the complete Phase 6 integration experiment with all systems connected.

```bash
# Basic experiment
nlm --mode experiment

# With custom parameters
nlm --mode experiment --neuron-count 2000 --steps 10000 --enable-checkpointing

# Fast experiment
nlm --mode experiment --fast

# Research-grade experiment
nlm --mode experiment --comprehensive
```

#### `--mode demo`
Runs a simplified demonstration for learning and testing.

```bash
# Basic demo
nlm --mode demo

# With custom settings
nlm --mode demo --neuron-count 200 --steps 500
```

#### `--mode test`
Runs comprehensive verification tests for all systems.

```bash
# Run all tests
nlm --mode test

# Test specific systems
nlm --mode test --neuron-count 500
```

#### `--mode benchmark`
Runs performance benchmarking tests.

```bash
# Fast benchmark
nlm --mode benchmark --fast

# Comprehensive benchmark
nlm --mode benchmark --comprehensive
```

#### `--mode config`
Manages configuration files.

```bash
# Load and save configuration
nlm --mode config --config my_config.cfg --output updated_config.cfg

# Show current configuration
nlm --mode config
```

### Configuration Presets

#### `--fast`
Optimizes for speed with smaller networks:
- Neurons: 200
- Steps: 2,000
- Memory: Minimal

#### `--comprehensive`
Optimizes for completeness with larger networks:
- Neurons: 2,000
- Steps: 50,000
- Memory: Full

#### `--research`
Optimizes for research-grade experiments:
- Neurons: 3,000
- Steps: 100,000
- Checkpointing: Enabled
- Development: Advanced

#### `--demo-mode`
Beginner-friendly settings:
- Neurons: 100
- Steps: 200
- All features enabled but small scale

### Configuration File Format

NLM uses JSON configuration files:

```json
{
  "neuron_count": 1000,
  "region_count": 1,
  "connection_probability": 0.1,
  "simulation_timestep": 0.001,
  "max_steps": 10000,
  "random_seed": 42,
  "enable_checkpointing": true,
  "enable_replay": true,
  "enable_development": true,
  "checkpoint_path": "./checkpoints/default.bin"
}
```

### Command-Line Argument Reference

| Option | Description | Default |
|--------|-------------|---------|
| `--config <file>` | Load configuration from file | `configs/default.cfg` |
| `--neuron-count <n>` | Number of neurons | 1000 |
| `--region-count <n>` | Number of brain regions | 1 |
| `--connection-prob <p>` | Connection probability (0.0-1.0) | 0.1 |
| `--timestep <t>` | Simulation timestep | 0.001 |
| `--steps <n>` | Number of simulation steps | 10000 |
| `--seed <n>` | Random seed for reproducibility | 42 |
| `--checkpoint <file>` | Checkpoint file path | `./checkpoint.bin` |
| `--enable-checkpointing` | Enable checkpoint saving/loading | false |
| `--enable-replay` | Enable memory replay system | false |
| `--enable-development` | Enable developmental processes | false |

## 2. Python API Advanced Features

### Enhanced Python Bindings

The Python bindings now include advanced features for research and development:

#### 1. Advanced Brain Configuration

```python
import pynlm as nlm

# Create custom configuration
config = nlm.createDefaultConfig()
config.set("neuron_count", 2000, nlm.ConfigSource.User)
config.set("region_count", 3, nlm.ConfigSource.User)
config.set("connection_probability", 0.15, nlm.ConfigSource.User)

# Create brain with custom configuration
brain = nlm.createBrain(config)
brain.initialize()
```

#### 2. System Information and Status

```python
# Get system information
system_info = nlm.getSystemInfo()
print(f"Version: {system_info['version']}")
print(f"Memory Systems: {system_info['memorySystems']}")
print(f"Neuromodulation: {system_info['neuromodulation']}")

# Check individual system status
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

print(f"Working Memory: {brain.getWorkingMemory() is not None}")
print(f"Episodic Memory: {brain.getEpisodicMemory() is not None}")
print(f"Neuromodulation: {brain.getDopamine() is not None}")
```

#### 3. Advanced Agent Brain

```python
# Create agent brain with custom features
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Get system status
print(f"Reward Modulation: {agent.isRewardModulationEnabled()}")
print(f"Development Enabled: {agent.isDevelopmentEnabled()}")
print(f"Curiosity Level: {agent.getCuriosityLevel()}")
```

#### 4. Phase 6 Integration

```python
# Run Phase 6 integration experiment with custom parameters
result = nlm.runPhase6Demo(
    neuronCount=1500,
    maxSteps=5000,
    seed=123
)

print(f"Integration Health: {result.memoryWorkingMemoryIntegrated and result.memoryEpisodicMemoryIntegrated}")
print(f"Performance: {result.totalReward:.2f} reward, {result.avgFiringRate:.2f} Hz firing rate")
```

#### 5. Configuration Management

```python
# Export configuration to file
nlm.exportConfig("my_config.json", config)

# Import configuration from file
imported_config = nlm.importConfig("my_config.json")

# Benchmark configuration
benchmark_result = nlm.benchmarkConfiguration(config)
print(f"Benchmark: {benchmark_result['stepsPerSecond']:.2f} steps/second")
```

#### 6. Advanced World Simulation

```python
# Create and configure world
world = nlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.setAgentStart(15, 15)
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.01)

# Add objects to the world
obj = nlm.WorldObject(x=5, y=5, type=nlm.WorldObjectType.Resource, value=10.0)
world.addObject(obj)

# Agent simulation loop
for step in range(1000):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process through agent
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward
    agent.applyRewardModulation(result.reward, 0.0)
```

### Python Module Structure

```python
# Available modules and classes
from pynlm import (
    Config,                    # Configuration management
    Brain,                     # Neural brain system
    AgentBrain,                # Agent interface
    SimpleWorld,               # Environment simulation
    Vision,                    # Visual sensory input
    Audio,                     # Audio sensory input
    InternalSignals,           # Internal neural signals
    Action,                    # Motor output actions
    ActionResult,              # Action execution results
    SensoryPercept,            # Combined sensory data
    WorldObject,               # World objects
    AgentBody,                 # Agent physical state
    Phase6Config,              # Phase 6 experiment configuration
    Phase6IntegrationResult,   # Phase 6 experiment results
    Phase6IntegratedExperiment, # Phase 6 experiment runner
)

# Constants and utilities
from pynlm import (
    INVALID_NEURON_ID,
    INVALID_SYNAPSE_ID,
    INVALID_REGION_ID,
    INVALID_POPULATION_ID,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
)

# Utility functions
from pynlm import (
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
    quickTest,
    getSystemInfo,
    saveBrain,
    loadBrain,
    exportConfig,
    importConfig,
    benchmarkConfiguration,
)
```

## 3. Configuration Management

### Configuration Management Tool

The NLM configuration management tool provides comprehensive control over brain configuration:

```bash
# Create default configuration
nlm --mode config

# Load from file and save with modifications
nlm --mode config --config my_config.cfg --output updated_config.cfg

# Show configuration summary
nlm --mode config
```

### Configuration Keys

| Key | Type | Description | Default |
|-----|------|-------------|---------|
| `neuron_count` | int64 | Number of neurons | 1000 |
| `region_count` | int64 | Number of brain regions | 1 |
| `connection_probability` | float | Synaptic connection probability | 0.1 |
| `simulation_timestep` | double | Simulation timestep in seconds | 0.001 |
| `random_seed` | int64 | Random seed for reproducibility | 42 |
| `enable_checkpointing` | bool | Enable checkpoint saving | false |
| `enable_replay` | bool | Enable memory replay | false |
| `enable_development` | bool | Enable developmental processes | false |
| `checkpoint_path` | string | Path for checkpoint files | `./checkpoint.bin` |
| `max_steps` | int64 | Maximum simulation steps | 10000 |

### Configuration Templates

Create custom configuration templates:

#### Research Template (`configs/research.cfg`)
```json
{
  "neuron_count": 3000,
  "region_count": 3,
  "connection_probability": 0.15,
  "simulation_timestep": 0.0005,
  "random_seed": 123,
  "enable_checkpointing": true,
  "enable_replay": true,
  "enable_development": true,
  "checkpoint_path": "./checkpoints/research_run.bin"
}
```

#### Demo Template (`configs/demo.cfg`)
```json
{
  "neuron_count": 100,
  "region_count": 1,
  "connection_probability": 0.2,
  "simulation_timestep": 0.001,
  "random_seed": 42,
  "enable_checkpointing": false,
  "enable_replay": true,
  "enable_development": true,
  "checkpoint_path": "./checkpoints/demo.bin"
}
```

## 4. Performance Optimization

### Benchmarking

Run performance benchmarks:

```bash
# Fast benchmark
nlm --mode benchmark --fast

# Comprehensive benchmark
nlm --mode benchmark --comprehensive

# Custom benchmark
nlm --mode benchmark --neuron-count 2000 --steps 5000
```

### Performance Metrics

Benchmark results include:

- **Speed**: Steps per second
- **Memory Usage**: Neural system memory consumption
- **Integration Health**: System connectivity percentage
- **Learning Efficiency**: Reward per step ratio
- **Scalability**: Performance with increasing neuron count

### Profiling

Enable detailed profiling:

```bash
nlm --mode profile --comprehensive --verbose
```

### Optimization Tips

1. **Choose appropriate neuron count**: Start with 200 neurons for testing, scale up for research.
2. **Balance timestep size**: Smaller timesteps for stability, larger for speed.
3. **Enable efficient systems**: Use checkpointing and replay for long experiments.
4. **Monitor resource usage**: Watch memory and CPU usage during long runs.

## 5. Experiment Design and Analysis

### Experiment Configuration

Design experiments with custom configurations:

```bash
# Create experiment with specific parameters
nlm --mode experiment \
    --neuron-count 1500 \
    --region-count 2 \
    --connection-prob 0.12 \
    --steps 8000 \
    --enable-checkpointing \
    --enable-replay \
    --enable-development
```

### Experiment Results Analysis

Analyze experiment results:

```python
import pynlm as nlm

# Load experiment results
result = nlm.getSystemInfo()

# Calculate performance metrics
health_score = 0.0
health_score += result['memorySystems'] * 0.25
health_score += result['neuromodulation'] * 0.25
health_score += result['prediction'] * 0.25
health_score += result['cognition'] * 0.25

print(f"Experiment Health Score: {health_score:.2f}/10")
print(f"System Status: {'Excellent' if health_score > 8 else 'Good' if health_score > 6 else 'Needs Attention'}")
```

### Experiment Comparison

Compare different experimental conditions:

```python
# Run multiple experiments with different parameters
experiments = []

# Experiment 1: Fast, small network
config1 = nlm.createDefaultConfig()
config1.set("neuron_count", 500)
brain1 = nlm.createBrain(config1)
# Run experiment...

# Experiment 2: Comprehensive, larger network  
config2 = nlm.createDefaultConfig()
config2.set("neuron_count", 2000)
brain2 = nlm.createBrain(config2)
# Run experiment...

# Compare results
result1 = experiment1.run()
result2 = experiment2.run()

print(f"Experiment 1 (Small): {result1.avgFiringRate:.2f} Hz")
print(f"Experiment 2 (Large): {result2.avgFiringRate:.2f} Hz")
print(f"Performance ratio: {result2.avgFiringRate / result1.avgFiringRate:.2f}x")
```

## 6. Debugging and Troubleshooting

### Common Issues and Solutions

#### Issue: Brain Initialization Fails

**Problem**: Brain fails to initialize with configuration.

**Solutions**:
1. Check configuration values for validity
2. Ensure required files exist (if loading from disk)
3. Increase neuron count if too small
4. Check system resources (memory, CPU)

#### Issue: Simulation Runs Slowly

**Problem**: Simulation performance is slower than expected.

**Solutions**:
1. Reduce neuron count for testing
2. Increase timestep size
3. Disable non-essential systems
4. Use `--fast` preset

#### Issue: Memory Usage High

**Problem**: Simulation consumes excessive memory.

**Solutions**:
1. Reduce neuron count
2. Disable checkpointing for short experiments
3. Use `--fast` preset
4. Monitor memory usage with system tools

### Debugging Commands

Enable verbose logging:

```bash
nlm --mode experiment --verbose
```

Run specific system tests:

```bash
nlm --mode test
```

Check system status:

```python
import pynlm as nlm
system_info = nlm.getSystemInfo()
print("System Status:", system_info)
```

### Error Handling

The Python API includes comprehensive error handling:

```python
try:
    brain = nlm.createBrain(nlm.createDefaultConfig())
    brain.initialize()
    # Run simulation...
except nlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## 7. Advanced Topics

### Custom Neural Architectures

Create custom brain architectures:

```python
import pynlm as nlm

# Create brain with specific regions
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)

# Add custom regions
region1_id = brain.addRegion("Cortex")
region2_id = brain.addRegion("Thalamus")

# Configure regions with different properties
# (Implementation depends on brain API)
```

### Custom Plasticity Rules

Implement custom learning rules:

```python
# Access plasticity systems
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Enable custom plasticity combinations
brain.enablePlasticity(hebbian=True, stdp=False, structural=True)
```

### Custom World Environments

Create custom world environments:

```python
# Extend SimpleWorld for custom environments
class CustomWorld(nlm.SimpleWorld):
    def __init__(self):
        super().__init__()
        self.custom_objects = []
    
    def addCustomObject(self, obj):
        self.custom_objects.append(obj)
        self.addObject(obj)
    
    def update(self, timestep):
        # Custom world dynamics
        super().update(timestep)
        # Add custom physics or rules
```

## 8. API Reference

### Core Classes

#### Config
Configuration management for NLM system.

#### Brain
Main neural brain class with all integrated systems.

#### AgentBrain
Interface connecting brain to world environment.

#### SimpleWorld
2D world simulation for NLM experiments.

#### Vision, Audio, InternalSignals
Sensory input modalities.

#### Action, ActionResult
Motor output and execution results.

#### SensoryPercept
Combined sensory input from world.

#### WorldObject, AgentBody
World and agent physical state.

### Enumerations

#### NeuronType
Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal

#### SynapseType
Excitatory, Inhibitory, Modulatory, Electrical, GapJunction

#### DevelopmentalStage
Initial, CriticalPeriod, Maturation, Adult, Aging

#### FiringState
Resting, Active, Refractory, Inhibited

#### ActionType
MoveForward, MoveBackward, MoveLeft, MoveRight, TurnLeft, TurnRight, etc.

#### MotorCommand
MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait

#### WorldObjectType
Empty, Resource, Hazard, Wall, Marker

### Functions

#### Factory Functions
- `createDefaultConfig()`: Create default configuration
- `createBrain(config)`: Create brain with configuration
- `createSimpleWorld()`: Create simple world
- `createAgentBrain(brain)`: Create agent interface

#### Utility Functions
- `quickTest()`: Quick integration test
- `getSystemInfo()`: Get system information
- `saveBrain(filepath, brain)`: Save brain to file
- `loadBrain(filepath, config)`: Load brain from file
- `exportConfig(filepath, config)`: Export configuration
- `importConfig(filepath)`: Import configuration
- `benchmarkConfiguration(config)`: Benchmark configuration

## 9. Examples

### Example 1: Complete Simulation

```python
import pynlm as nlm

# Setup
config = nlm.createDefaultConfig()
config.set("neuron_count", 1000, nlm.ConfigSource.User)
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    agent.applyRewardModulation(result.reward, 0.0)

# Get results
print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
print(f"Final reward: {agent.getBrain().getAverageFiringRate():.2f} Hz")
```

### Example 2: Phase 6 Integration

```python
import pynlm as nlm

# Run Phase 6 integration
result = nlm.runPhase6Demo(neuronCount=1500, maxSteps=3000)

# Analyze results
health = (result.memoryWorkingMemoryIntegrated and 
          result.memoryEpisodicMemoryIntegrated and
          result.neuromodulationIntegrated)

print(f"Integration Health: {'Excellent' if health else 'Failed'}")
print(f"Performance: {result.totalReward:.2f} reward, {result.avgFiringRate:.2f} Hz")
print(f"Memory: {result.memoryEpisodesStored} episodes stored")
```

### Example 3: Configuration Management

```python
import pynlm as nlm
import json

# Create and save configuration
config = nlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("region_count", 2)

# Export to file
nlm.exportConfig("my_config.json", config)

# Load and modify
loaded_config = nlm.importConfig("my_config.json")
loaded_config.set("enable_checkpointing", True)

# Create brain with loaded config
brain = nlm.createBrain(loaded_config)
brain.initialize()
```

## 10. Further Reading

### Documentation

- **NLM Architecture**: `docs/ARCHITECTURE.md`
- **Phase 6 Audit**: `docs/PHASE6_FINAL_AUDIT.md`
- **Science**: `docs/SCIENCE.md`
- **Roadmap**: `docs/ROADMAP.md`
- **Research Methods**: `docs/RESEARCH_METHODS.md`

### References

1. **NLM Research Papers**: Available in the NLM research repository
2. **API Documentation**: Generated documentation in `docs/api/`
3. **Tutorials**: `docs/tutorials/` - Step-by-step guides
4. **Examples**: `examples/` - Complete working examples

### Support

For advanced usage questions:
- **GitHub Issues**: Report bugs and request features
- **Research Collaboration**: Contact the NLM research team
- **Community Forum**: Discuss usage and best practices
- **Documentation**: Check the latest documentation updates

## Conclusion

NLM Phase 6 provides a comprehensive artificial brain system with extensive customization options for advanced users. This guide covers the main features:

- **CLI Advanced Usage**: Multiple modes and configuration presets
- **Python API Extensions**: Enhanced bindings with advanced utilities
- **Configuration Management**: Comprehensive configuration tools
- **Performance Optimization**: Benchmarking and profiling capabilities
- **Experiment Design**: Tools for designing and analyzing experiments
- **Debugging**: Troubleshooting and error handling
- **Advanced Topics**: Custom architectures and plasticity rules

The NLM system is ready for research, development, and production use with complete integration of all brain components and extensive customization options for advanced users.