# NLM Python Bindings - Installation and Usage Guide

This document provides comprehensive instructions for installing, building, and using the improved NLM Python bindings.

## Overview

The NLM Python bindings provide complete access to all NLM (Neural Learning Machine) functionality through a user-friendly Python API. This improved version addresses all issues from the original implementation:

- ✅ **Complete functionality**: All major NLM systems exposed
- ✅ **User-friendly API**: Easy for beginners, powerful for advanced users
- ✅ **Robust error handling**: Comprehensive validation and exceptions
- ✅ **Advanced features**: Learning systems, neuromodulation, development
- ✅ **Comprehensive documentation**: Type hints, examples, and detailed docstrings

## Prerequisites

### System Requirements
- Linux, macOS, or Windows
- Python 3.8+
- CMake 3.16+
- C++ compiler supporting C++20

### Python Dependencies
```bash
# Core requirements (automatically handled by build system)
- pybind11 >= 2.11.0

# Optional development dependencies
pip install pytest numpy scipy matplotlib
```

## Building the Bindings

### Standard Build

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4
```

### Using Python Native Build (pyproject.toml)

The project uses scikit-build-core for Python package building:

```bash
# Install in development mode
pip install -e .

# Or build and install
pip install .
```

### Build Options

```bash
cmake ..
  -DCMAKE_BUILD_TYPE=Release          # Build type (Debug, Release, RelWithDebInfo)
  -DPYTHON_EXECUTABLE=/path/to/python # Python interpreter
  -DCMAKE_CXX_FLAGS="-O3 -march=native" # Additional compiler flags
```

## Installation

### Option 1: System-wide Installation

```bash
pip install pynlm
```

### Option 2: Development Installation

```bash
pip install -e .
```

### Option 3: From Source

```bash
mkdir build
cd build
cmake ..
make
pip install -e .
```

## Quick Start Examples

### Basic Usage

```python
import pynlm as nlm

# Create default configuration
config = nlm.createDefaultConfig()

# Create brain
brain = nlm.createBrain(config)

# Create world
world = nlm.createSimpleWorld(100, 100, 20, 20)

# Create agent brain
agent_brain = nlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Run simulation
reward = nlm.runEpisode(agent_brain, world, 1000)
print(f"Final reward: {reward}")
```

### Advanced Features

```python
import pynlm as nlm

# Create advanced configuration
config = nlm.getDefaultConfigTemplate()

# Get comprehensive brain statistics
brain = nlm.createBrain(config)
stats = nlm.getBrainStats(brain)

print(f"Brain statistics: {stats}")

# Enable all advanced features
agent_brain = nlm.createAgentBrain(brain)
agent_brain.enableRewardModulation(True)
agent_brain.enableStructuralPlasticity(True)
agent_brain.enableDevelopment(True)
agent_brain.enableCuriosity(True)

# Demonstrate neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()

# Configure development system
dev_system = brain.getDevelopmentSystem()
dev_system.setStage(nlm.DevelopmentalStage.CriticalPeriod)
```

### Error Handling

```python
import pynlm as nlm

try:
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    
    # This will raise NLMError if brain is None
    nlm.exportBrainState(brain, "/invalid/path.brain")
    
except nlm.NLMError as e:
    print(f"NLM Error: {e}")
except nlm.InvalidParameterError as e:
    print(f"Invalid parameter: {e}")
except nlm.InitializationError as e:
    print(f"Initialization failed: {e}")
except nlm.OperationError as e:
    print(f"Operation failed: {e}")
```

## API Reference

### Core Classes

#### Config
- `Config()` - Create empty configuration
- `config.loadFromFile(filepath)` - Load from JSON file
- `config.loadFromArgs(argc, argv)` - Load from command line
- `config.saveToFile(filepath)` - Save to JSON file
- `config.has(key)` - Check if key exists
- `config.get(key)` - Get value
- `config.getOr(key, default)` - Get with default
- `config.set(key, value)` - Set value
- `config.clear()` - Clear all
- `config.summary()` - Get summary string

#### Brain
- `Brain(config)` - Create brain with configuration
- `brain.initialize()` - Initialize brain
- `brain.step(step)` - Perform simulation step
- `brain.receiveSensoryInput(input)` - Inject sensory input
- `brain.produceAction()` - Generate motor action
- `brain.save(filepath)` - Save brain state
- `brain.load(filepath)` - Load brain state
- `brain.reset()` - Reset state

#### AgentBrain
- `AgentBrain(brain)` - Create agent brain interface
- `agentBrain.initialize(world)` - Initialize with world
- `agentBrain.processSensoryInput(percept)` - Process sensory input
- `agentBrain.decodeMotorCommand()` - Decode motor command
- `agentBrain.applyRewardModulation(reward, predicted)` - Apply reward modulation
- `agentBrain.updateDevelopment(timestep)` - Update development

#### SimpleWorld
- `SimpleWorld()` - Create empty world
- `world.configure(width, height, visionWidth, visionHeight)` - Configure world
- `world.update(timestep)` - Update world
- `world.applyMotorCommand(cmd, time)` - Apply motor command
- `world.getSensoryPercept()` - Get sensory percept
- `world.getAgentBody()` - Get agent body

### Helper Functions

#### Factory Functions
- `createDefaultConfig()` - Create default configuration
- `createBrain(config)` - Create and initialize brain
- `createSimpleWorld(width, height, visionWidth, visionHeight)` - Create world
- `createAgentBrain(brain)` - Create agent brain
- `buildNeuralNetwork(config, name)` - Build neural network

#### Simulation Utilities
- `runEpisode(agentBrain, world, maxSteps)` - Run complete episode
- `resetEnvironment(world, agentBrain)` - Reset environment
- `benchmarkStep(brain, steps)` - Benchmark brain performance

#### Monitoring and Debugging
- `exportBrainState(brain, filepath)` - Save brain state
- `importBrainState(brain, filepath)` - Load brain state
- `getBrainStats(brain)` - Get brain statistics
- `validateConfig(config)` - Validate configuration
- `getConfigValidationError(config)` - Get validation errors

### Neuromodulation System

#### Reward Class
- `Reward()` - Create reward system
- `reward.getValue()` - Get current reward value
- `reward.setValue(value)` - Set reward value
- `reward.add(delta)` - Add to reward
- `reward.reset()` - Reset reward
- `reward.computeReward(observation)` - Compute reward from observation

#### Neuromodulator Base Class
- `Neuromodulator` - Abstract base class
- `Dopamine()` - Dopamine neuromodulator
- `Acetylcholine()` - Acetylcholine neuromodulator
- `Norepinephrine()` - Norepinephrine neuromodulator
- `Serotonin()` - Serotonin neuromodulator

#### Curiosity System
- `Curiosity()` - Create curiosity system
- `curiosity.initialize(brain)` - Initialize with brain reference
- `curiosity.getLevel()` - Get curiosity level
- `curiosity.update(novelty, predictionError, dt)` - Update curiosity
- `curiosity.getExplorationDrive()` - Get exploration drive

#### Novelty Detection
- `Novelty()` - Create novelty detector
- `novelty.initialize(brain)` - Initialize with brain reference
- `novelty.getLevel()` - Get novelty level
- `novelty.setLevel(level)` - Set novelty level
- `novelty.detectNovelty(current, previous)` - Detect novelty

#### Prediction Error
- `PredictionError()` - Create prediction error system
- `predictionError.initialize(brain)` - Initialize with brain reference
- `predictionError.getError()` - Get error value
- `predictionError.computeError(predicted, actual)` - Compute error
- `predictionError.getMagnitude()` - Get error magnitude

### Development System

#### DevelopmentSystem
- `DevelopmentSystem()` - Create development system
- `devSystem.getStage()` - Get current developmental stage
- `devSystem.setStage(stage)` - Set developmental stage
- `devSystem.advanceStage()` - Advance to next stage
- `devSystem.getStageName()` - Get stage name
- `devSystem.update(brain, timestep)` - Update development
- `devSystem.getPlasticityModifier()` - Get plasticity modifier
- `devSystem.isCriticalPeriod()` - Check if in critical period

### Exceptions

The bindings provide comprehensive exception handling:

- `NLMError` - Base NLM exception
- `InvalidParameterError` - Invalid parameter error
- `InitializationError` - Initialization failed error
- `OperationError` - Operation failed error

## Advanced Features

### Memory Management

```python
# Access memory systems
brain = nlm.createBrain(config)

working_memory = brain.getWorkingMemory()
if working_memory:
    # Access working memory
    pass

episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    # Access episodic memory
    pass

associative_memory = brain.getAssociativeMemory()
if associative_memory:
    # Access associative memory
    pass
```

### Plasticity Systems

```python
brain = nlm.createBrain(config)

# Access plasticity systems
stdp = brain.getSTDP()
if stdp:
    # Access STDP plasticity
    pass

hebbian = brain.getHebbian()
if hebbian:
    # Access Hebbian plasticity
    pass

structural_plasticity = brain.getStructuralPlasticity()
if structural_plasticity:
    # Access structural plasticity
    pass
```

### Prediction System

```python
brain = nlm.createBrain(config)

# Access prediction system
prediction_system = brain.getPredictionSystem()
if prediction_system:
    # Access prediction capabilities
    pass

neural_prediction = brain.getPredictionSystem()
if neural_prediction:
    # Access neural prediction
    pass
```

### Cognition Systems

```python
brain = nlm.createBrain(config)

# Access cognition systems
planner = brain.getPlanner()
if planner:
    # Access neural planning
    pass

concept_formation = brain.getConceptFormation()
if concept_formation:
    # Access concept formation
    pass

attention = brain.getAttention()
if attention:
    # Access attentional selection
    pass
```

## Performance Monitoring

```python
import pynlm as nlm

config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)

# Get comprehensive brain statistics
stats = nlm.getBrainStats(brain)

# Print statistics
for key, value in stats.items():
    if isinstance(value, (int, float)):
        print(f"{key}: {value:.3f}")
    else:
        print(f"{key}: {value}")

# Benchmark performance
results = nlm.benchmarkStep(brain, 1000)
print(f"Average time per step: {results['avgTimePerStepMs']:.3f} ms")
```

## Configuration Management

```python
import pynlm as nlm

# Create configuration with defaults
config = nlm.createDefaultConfig()

# Add custom configuration
config.set("brain.neuronCount", 2000)
config.set("brain.learningRate", 0.001)
config.set("neuromodulation.curiosityWeight", 0.8)
config.set("development.initialStage", "CriticalPeriod")

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
new_config = nlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")

# Validate configuration
is_valid = nlm.validateConfig(config)
if is_valid:
    print("Configuration is valid")
else:
    print("Configuration errors:", nlm.getConfigValidationError(config))
```

## Simulation Examples

### Basic Agent Simulation

```python
import pynlm as nlm

# Setup
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld(200, 200, 30, 30)
agent_brain = nlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Run multiple episodes
total_reward = 0
for episode in range(10):
    reward = nlm.runEpisode(agent_brain, world, 500)
    total_reward += reward
    print(f"Episode {episode + 1}: Reward = {reward:.3f}")

print(f"Average reward: {total_reward / 10:.3f}")
```

### Development Simulation

```python
import pynlm as nlm

# Create brain with development
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)

# Track development over time
for day in range(100):
    brain.setDevelopmentalStage(
        nlm.DevelopmentalStage.Maturation if day < 50 else nlm.DevelopmentalStage.Adult
    )
    
    # Run simulation step
    brain.step(day)
    
    # Print development status
    dev_system = brain.getDevelopmentSystem()
    if dev_system:
        print(f"Day {day}: Stage={dev_system.getStageName()}, "
              f"Plasticity={dev_system.getPlasticityModifier():.3f}")
```

### Neuromodulation Simulation

```python
import pynlm as nlm

# Create brain with neuromodulation
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)

# Get neuromodulators
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()

# Simulate learning episode
for step in range(100):
    # Simulate external reward
    reward = 1.0 if step % 20 == 0 else 0.1
    
    # Apply neuromodulation
    if dopamine:
        dopamine.signalReward(reward)
    
    # Simulate novelty (random)
    import random
    detected_novelty = random.random() * 0.5
    if novelty:
        novelty.setLevel(detected_novelty)
    
    # Update curiosity
    prediction_error = random.random() * 0.3
    if curiosity:
        curiosity.update(detected_novelty, prediction_error, 1.0)
    
    # Run brain step
    brain.step(step)
    
    # Print neuromodulation levels
    print(f"Step {step}: Dopamine={dopamine.getLevel() if dopamine else 0:.3f}, "
          f"Curiosity={curiosity.getLevel() if curiosity else 0:.3f}, "
          f"Novelty={novelty.getLevel() if novelty else 0:.3f}")
```

## Error Handling Best Practices

### Checking for Null Pointers

```python
import pynlm as nlm

def safe_brain_operation(brain):
    """Safe brain operation with null checks."""
    if brain is None:
        raise nlm.InvalidParameterError("brain", "cannot be null")
    
    # Check optional systems
    dev_system = brain.getDevelopmentSystem()
    if dev_system is None:
        print("Warning: Development system not available")
    
    dopamine = brain.getDopamine()
    if dopamine is None:
        print("Warning: Dopamine system not available")
    
    # Perform operations
    return brain.getTotalNeuronCount()
```

### Configuration Validation

```python
import pynlm as nlm

def create_safe_brain(config):
    """Create brain with comprehensive validation."""
    # Validate configuration
    if not nlm.validateConfig(config):
        error_msg = nlm.getConfigValidationError(config)
        raise nlm.InvalidParameterError("config", f"invalid: {error_msg}")
    
    # Create brain with error handling
    try:
        brain = nlm.createBrain(config)
    except nlm.InitializationError as e:
        raise nlm.InitializationError("brain", f"failed to initialize: {e}")
    
    return brain
```

## Performance Tips

### Memory Management

```python
import pynlm as nlm

# Use configuration to control memory usage
config = nlm.getDefaultConfigTemplate()
config.set("simulation.maxMemory", 500.0)  # Limit memory

brain = nlm.createBrain(config)

# Monitor memory usage
stats = nlm.getBrainStats(brain)
if "memoryUsage" in stats:
    print(f"Memory usage: {stats['memoryUsage']:.1f}%")
```

### Performance Optimization

```python
import pynlm as nlm

# Create optimized configuration
config = nlm.createDefaultConfig()
config.set("brain.learningRate", 0.01)
config.set("brain.connectionProbability", 0.05)

brain = nlm.createBrain(config)

# Run performance benchmark
results = nlm.benchmarkStep(brain, 500)
print(f"Performance: {results['stepsPerSecond']:.1f} steps/second")
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Module Not Found
**Solution:** Ensure the module is installed correctly:
```bash
pip install pynlm
```

#### Issue 2: Build Failures
**Solution:** Check CMake and compiler requirements:
```bash
# Check compiler
cpp --version

# Check CMake version
cmake --version

# Build with verbose output
cmake .. -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
make VERBOSE=1
```

#### Issue 3: Missing Dependencies
**Solution:** Install required dependencies:
```bash
pip install pybind11 numpy pytest
```

#### Issue 4: Runtime Errors
**Solution:** Check error messages and use try-catch:
```python
import pynlm as nlm

try:
    # Your code here
    pass
except nlm.NLMError as e:
    print(f"NLM Error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Debugging Tips

#### 1. Enable Debug Build
```bash
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
```

#### 2. Add Debug Prints
```python
import pynlm as nlm

# Add debug prints to your code
brain = nlm.createBrain(config)
stats = nlm.getBrainStats(brain)

# Print detailed statistics
for key, value in stats.items():
    print(f"[DEBUG] {key}: {value}")
```

#### 3. Check System Resources
```python
import psutil
import os

process = psutil.Process(os.getpid())
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")
print(f"CPU usage: {process.cpu_percent()}%")
```

## Migration Guide

### From Old API to New API

#### Configuration
```python
# Old way
config = Config()
config.set("neuronCount", 100)

# New way
config = nlm.createDefaultConfig()
config.set("brain.neuronCount", 100)
```

#### Brain Creation
```python
# Old way
brain = Brain(config)

# New way
brain = nlm.createBrain(config)
```

#### Error Handling
```python
# Old way
try:
    brain.initialize()
except Exception as e:
    print(f"Error: {e}")

# New way
try:
    brain = nlm.createBrain(config)
except nlm.InitializationError as e:
    print(f"Initialization failed: {e}")
```

## Supported Platforms

### Operating Systems
- Linux (all distributions)
- macOS (Intel and Apple Silicon)
- Windows (with Visual Studio 2019+)

### Python Versions
- Python 3.8
- Python 3.9
- Python 3.10
- Python 3.11
- Python 3.12

### Architecture Support
- x86_64 (64-bit)
- ARM64 (Apple Silicon, some Raspberry Pi)

## Future Development

### Planned Features
1. **Advanced Memory Systems**
   - More sophisticated working memory
   - Temporal memory for pattern completion
   - Semantic memory integration

2. **Enhanced Neuromodulation**
   - Serotonin system for mood and impulsivity
   - Novel neuromodulator types
   - Cross-system neuromodulation interactions

3. **Advanced Learning**
   - Meta-learning capabilities
   - Transfer learning
   - Continual learning algorithms

4. **Development Extensions**
   - More developmental stages
   - Stage-specific learning rates
   - Neural pruning and rewiring

5. **Simulation Enhancements**
   - Multi-agent environments
   - Complex world models
   - Real-time visualization

6. **Performance Improvements**
   - GPU acceleration support
   - Better memory management
   - Parallel processing

### Research Areas
- Developmental trajectories
- Neuromodulation synergies
- Prediction-error driven learning
- Memory consolidation
- Social learning mechanisms
- Cognitive architecture

## Conclusion

The NLM Python bindings provide a comprehensive, user-friendly interface to the NLM neural simulation framework. This improved version addresses all issues from the original implementation:

- ✅ **Complete functionality**: All major systems exposed
- ✅ **Robust error handling**: Comprehensive exception management
- ✅ **Advanced features**: Neuromodulation, development, learning
- ✅ **Performance tools**: Benchmarking, validation, monitoring
- ✅ **Extensive documentation**: Examples, type hints, API reference
- ✅ **User-friendly**: Easy for beginners, powerful for experts

The bindings enable researchers and developers to:
- Quickly prototype neural systems
- Integrate NLM with existing Python workflows
- Perform comprehensive simulations
- Debug and optimize neural networks
- Explore advanced AI concepts

Get started today and discover the power of neural learning machine simulation!
