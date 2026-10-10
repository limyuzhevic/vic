# NLM Python Bindings - Enhanced Features

## Overview

The NLM (Neural Learning Machine) Python bindings have been significantly enhanced to provide a more powerful, user-friendly, and feature-complete interface for neural simulation research and development.

## Key Improvements

### 1. Enhanced Pythonic API Design

#### Identifier Classes (Improved Type Safety)

**NeuronId, SynapseId, RegionId, PopulationId** now include:
- **Hex String Constructors**: `NeuronId("0x123")` for easy ID parsing
- **to_hex() Methods**: Convert IDs to human-readable hex strings
- **is_valid() Methods**: Validate ID correctness
- **Better String Representations**: Python-friendly `__str__()` methods
- **Type Safety**: Enhanced validation and error checking

**Example Usage:**
```python
import pynlm

# Hex string constructor
neuron_id = pynlm.NeuronId("0x123")
synapse_id = pynlm.SynapseId("0x456")

# Convert to hex
print(neuron_id.to_hex())  # "0x123"

# Validate IDs
print(neuron_id.is_valid())  # True/False

# Pythonic string representation
print(str(neuron_id))  # "NeuronId(291)"
```

#### Config Class (Advanced Configuration)

**Enhanced Config class** now includes:
- **Direct File Loading**: `config = pynlm.Config("config.json")`
- **Argument Loading**: `config.load_args(["--neurons", "1000", "--seed", "42"])`
- **Type-Safe Accessors**: `config.get_float("learning_rate", 0.001)`
- **Advanced Configuration Methods**: `set_plasticity()`, `set_development()`, `set_simulation()`
- **Built-in Validation**: `config.validate()`

**Example Usage:**
```python
import pynlm

# Direct file loading
config = pynlm.Config("my_config.json")

# Advanced configuration
config.set_plasticity(stdp_enabled=True, hebbian_enabled=True, reward_modulated=True)
config.set_development(initial_age=0.0, critical_period=60.0, maturation_age=120.0)
config.set_simulation(timestep=0.001, max_neurons=10000, max_synapses=50000)

# Type-safe access
learning_rate = config.get_float("plasticity.stdp.learning_rate", 0.001)
neuron_count = config.get_int("brain.neuron_count", 1000)
synapse_density = config.get_float("brain.synapse_density", 0.1)

# Validation
if config.validate():
    print("Configuration is valid!")
else:
    print("Configuration has errors!")
```

### 2. Performance Monitoring and Optimization

#### Brain Class Enhancements

The Brain class now includes:
- **Advanced Simulation Control**:
  - `step_until(predicate, max_steps=1000)`: Step until condition met
  - `step_adaptive(max_duration, target_fps=60)`: CPU-adaptive stepping
  - `pause()` / `resume()`: Manual simulation control
  - `is_paused()`: Check simulation state

- **Performance Monitoring**:
  - `get_metrics()`: Returns comprehensive performance dictionary
  - `profile_step(duration_ms="auto")`: Performance profiling
  - `get_memory_usage()`: Current memory usage
  - `optimize_spike_delivery()`: Spike delivery optimization
  - `compress_memory(ratio=0.5)`: Memory compression

- **Enhanced Input/Output**:
  - `inject_current_to_neurons_by_type(type, current)`: Type-specific injection
  - `get_neurons_by_type(type)`: Query neurons by type
  - `get_synapses_by_type(type)`: Query synapses by type

- **Development & Learning Features**:
  - `enable_development(boolean)`: Enable/disable development
  - `is_development_enabled()`: Check development status
  - `update_development_stage(timestep)`: Update developmental stage
  - `set_developmental_target(target_stage)`: Set development target

- **Statistics & Analytics**:
  - `get_statistics()`: Comprehensive statistics dictionary
  - `get_activity_summary()`: Neural activity summary
  - `get_plasticity_summary()`: Plasticity changes summary

**Example Usage:**
```python
import pynlm
import time

# Create and configure brain
config = pynlm.Config()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable development
brain.enable_development(True)

# Performance monitoring
brain.pause()  # Pause for setup
metrics = brain.get_metrics()
print(f"Memory usage: {brain.get_memory_usage()} bytes")

# Adaptive stepping
brain.resume()
# Simulate with adaptive stepping
brain.step_adaptive(max_duration=10000, target_fps=30)

# Get statistics
stats = brain.get_statistics()
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
```

### 3. NumPy Integration

#### Enhanced SensoryInput Classes

All SensoryInput classes now include:
- **NumPy Array Support**: Direct conversion to/from NumPy arrays
- **Pythonic Interface**: `len()`, `[]` indexing, slice operations
- **Type Safety**: Automatic validation and conversion

**Vision Class Enhancements**:
- **Image Conversion**: `to_image()` returns PIL Image objects
- **File Export**: `save_image(filepath)` for image files
- **Image Processing**: `normalize()`, `apply_filter()`, `resize()`
- **Analysis Tools**: `get_histogram()`, `get_statistics()`
- **Edge Detection**: `get_edges()` using Sobel filter
- **Color Conversion**: `to_gray()` for grayscale conversion

**Example Usage:**
```python
import pynlm
import numpy as np
from PIL import Image

# Create vision input
vision = pynlm.Vision(320, 240, 3)  # 320x240 RGB image

# NumPy array interface
numpy_array = np.random.rand(320, 240, 3).astype(np.float32)
vision.set_data_numpy(numpy_array)  # Direct NumPy assignment

# Pythonic interface
print(len(vision))  # 960 (width * height * channels)
vision[0] = 0.5  # Access individual pixel

# Convert to NumPy for processing
data_array = vision.get_data_numpy()  # Returns NumPy array

# Image processing
vision.normalize()  # Normalize to [0, 1]
vision.resize(160, 120)  # Resize image
vision.apply_filter(np.ones((3, 3)) / 9)  # Blur filter

# Export and analysis
vision.save_image("processed.png")  # Save as PNG
vision.to_image()  # Returns PIL Image object
histogram = vision.get_histogram()  # Get intensity histogram
statistics = vision.get_statistics()  # Get mean, std, min, max
```

### 4. Comprehensive Error Handling

#### Custom Exception Classes

Enhanced error handling includes:
- **ValidationError**: Configuration and input validation errors
- **ConfigurationError**: Configuration loading and parsing errors
- **MemoryError**: Memory allocation and management errors
- **IndexError**: Array and index bounds errors
- **TypeError**: Type conversion and validation errors

**Example Usage:**
```python
import pynlm

try:
    # Configuration validation
    config = pynlm.Config("invalid_config.json")
    if not config.validate():
        raise pynlm.ConfigurationError("Invalid configuration")
    
    # ID validation
    neuron_id = pynlm.NeuronId("invalid")
    if not neuron_id.is_valid():
        raise pynlm.ValidationError("Invalid neuron ID")
    
    # Input validation
    data = vision.get_data_numpy()
    if data.shape != (320, 240, 3):
        raise pynlm.ValidationError("Invalid data dimensions")
    
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
except pynlm.MemoryError as e:
    print(f"Memory error: {e}")
```

### 5. Advanced Command-Line Interface

#### Enhanced CLI Options

The enhanced CLI includes:
- **Configuration File Options**: `--config`, `--config-file`
- **Simulation Control**: `--pause`, `--resume`, `--step-until`
- **Performance Options**: `--profile`, `--metrics`, `--verbose`
- **Development Options**: `--development`, `--target-stage`
- **Debugging Options**: `--debug`, `--trace`, `--checkpoint`

**Example CLI Usage:**
```bash
# Basic simulation with configuration
nlm --config simulation.cfg --neurons 1000 --timestep 0.001

# Development simulation
nlm --config dev.cfg --development --target-stage "maturation"

# Performance profiling
nlm --config perf.cfg --profile --metrics output.json

# Step until condition
nlm --config pause_resume.cfg --step-until "firing_rate > 10" --max-steps 5000
```

## Advanced Usage Examples

### Example 1: Complex Brain Development Simulation

```python
import pynlm
import time
import json

# Configure for complex development
config = pynlm.Config()
config.set_development(
    initial_age=0.0,
    critical_period=60.0,
    maturation_age=120.0
)
config.set_plasticity(
    stdp_enabled=True,
    hebbian_enabled=True,
    reward_modulated=True
)
config.set_simulation(
    timestep=0.001,
    max_neurons=5000,
    max_synapses=20000
)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()
brain.enable_development(True)

# Performance monitoring
brain.pause()
print("Initial metrics:", brain.get_metrics())
brain.resume()

# Run development simulation
start_time = time.time()
steps_without_firing = 0

while steps_without_firing < 100:  # Stop after 100 steps without firing
    brain.step_adaptive(max_duration=1000, target_fps=30)
    
    # Check for activity
    firing_rate = brain.get_average_firing_rate()
    if firing_rate > 0:
        steps_without_firing = 0
        print(f"Step {brain.getTotalSpikeCount()}: Firing rate = {firing_rate:.2f} Hz")
    else:
        steps_without_firing += 1

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")

# Save results
results = {
    'total_steps': brain.getTotalSpikeCount(),
    'final_firing_rate': brain.get_average_firing_rate(),
    'development_stage': brain.get_developmental_stage().value,
    'memory_usage': brain.get_memory_usage(),
    'performance_metrics': brain.get_metrics()
}

with open("simulation_results.json", "w") as f:
    json.dump(results, f, indent=2)

print("Results saved to simulation_results.json")
```

### Example 2: Real-Time Sensor Processing

```python
import pynlm
import numpy as np
from threading import Thread

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=640, height=480, visionWidth=320, visionHeight=240)
world.reset()

config = pynlm.Config()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# Real-time processing loop
def process_frame():
    while True:
        # Update world
        world.update(0.1)
        
        # Get sensor data
        percept = world.getSensoryPercept()
        
        # Process with NumPy for speed
        vision_data = percept.getVision()
        numpy_vision = vision_data.get_data_numpy()
        
        # Apply simple processing
        gray_vision = np.mean(numpy_vision, axis=2)
        normalized_vision = gray_vision / 255.0
        
        # Create new percept with processed data
        processed_percept = pynlm.SensoryPercept()
        processed_vision = pynlm.Vision(320, 240)
        processed_vision.set_data_numpy(normalized_vision.flatten())
        processed_percept.setVision(processed_vision)
        
        # Process in brain
        agent.processSensoryInput(processed_percept)
        brain.step(0)  # Step with timestamp 0
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Performance monitoring
        if world.getSimulationTime() % 100 == 0:
            metrics = brain.get_metrics()
            print(f"Time: {world.getSimulationTime():.1f}s, "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Memory: {brain.get_memory_usage()} bytes")

# Start real-time processing
processing_thread = Thread(target=process_frame)
processing_thread.daemon = True
processing_thread.start()

print("Real-time processing started. Press Ctrl+C to stop.")
try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    print("Processing stopped.")
```

### Example 3: Machine Learning with NLM

```python
import pynlm
import numpy as np

# Create environment
env = pynlm.createSimpleWorld()
env.configure(width=100, height=100, visionWidth=20, visionHeight=20)
env.reset()

# Create learning brain
config = pynlm.Config()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.learning_rate", 0.0005)

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(env)
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# Training loop
episodes = 1000
total_reward = 0

for episode in range(episodes):
    # Reset environment
    env.reset()
    agent.reset()
    
    episode_reward = 0
    steps = 0
    
    for step in range(200):  # Max steps per episode
        # Get percept
        percept = env.getSensoryPercept()
        
        # Process percept
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        env.applyMotorCommand(action, env.getSimulationTime())
        
        # Get reward and learn
        reward = env.get_sensory_percept().get_internal()[0] if env.get_sensory_percept().get_internal() else 0.0
        agent.apply_reward_modulation(reward, 0.0)
        
        episode_reward += reward
        steps += 1
        
        # End episode if done
        if env.get_sensory_percept().get_internal() and env.get_sensory_percept().get_internal()[0] < -0.9:
            break
    
    total_reward += episode_reward
    
    # Log progress
    if episode % 100 == 0:
        avg_reward = total_reward / (episode + 1)
        print(f"Episode {episode}: Steps {steps}, "
              f"Avg Reward {avg_reward:.3f}, "
              f"Firing Rate {brain.get_average_firing_rate():.2f} Hz")
        
        # Save model checkpoint
        brain.save(f"model_checkpoint_{episode}.bin")

print(f"Training complete. Average reward: {total_reward/episodes:.3f}")
```

## Installation and Setup

### Basic Installation

```bash
# Clone the repository
cd /path/to/nlm

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install scikit-build-core pybind11
pip install .
```

### Development Installation

```bash
# Install in development mode
pip install -e .

# Install with additional dependencies
pip install -e .[dev]
```

### Quick Test

```python
import pynlm

# Test basic functionality
config = pynlm.Config()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.get_total_neuron_count()} neurons")
print(f"Default configuration: {config.summary()}")

# Test enhanced features
brain.enable_development(True)
print(f"Development enabled: {brain.is_development_enabled()}")

print("All tests passed!")
```

## API Reference

### Core Classes

#### Config
- `Config()`: Default constructor
- `Config(filepath)`: Load from file
- `load(filepath)`: Load from file
- `load_args(args)`: Load from argument list
- `save(filepath)`: Save to file
- `validate()`: Validate configuration
- `get_float(key, default)`: Type-safe float access
- `get_int(key, default)`: Type-safe integer access
- `get_string(key, default)`: Type-safe string access
- `set_plasticity(stdp, hebbian, reward)`: Configure plasticity
- `set_development(initial, critical, maturation)`: Configure development
- `set_simulation(timestep, neurons, synapses)`: Configure simulation

#### Brain
- `Brain(config)`: Create brain
- `initialize()`: Initialize brain
- `step(step)`: Basic simulation step
- `step(step, time)`: Simulation step with timestamp
- `receiveSensoryInput(percept)`: Process sensory input
- `inject_current(neuron, current)`: Inject current
- `inject_current_to_neurons(type, current)`: Inject to neuron type
- `produceAction()`: Generate motor action
- `reset()`: Reset brain state
- `save(filepath)`: Save state
- `load(filepath)`: Load state
- `step_until(predicate, max_steps)`: Step until condition
- `step_adaptive(max_duration, target_fps)`: Adaptive stepping
- `pause()` / `resume()`: Control simulation
- `is_paused()`: Check simulation state
- `get_metrics()`: Performance metrics
- `profile_step(duration_ms)`: Performance profiling
- `get_memory_usage()`: Memory usage
- `optimize_spike_delivery()`: Optimize spike delivery
- `compress_memory(ratio)`: Memory compression
- `get_neurons_by_type(type)`: Query neurons
- `get_synapses_by_type(type)`: Query synapses
- `enable_development(enable)`: Enable development
- `is_development_enabled()`: Check development status
- `update_development_stage(timestep)`: Update development
- `set_developmental_target(stage)`: Set development target
- `get_statistics()`: Comprehensive statistics
- `get_activity_summary()`: Activity summary
- `get_plasticity_summary()`: Plasticity summary

#### AgentBrain
- `AgentBrain(brain)`: Create agent interface
- `initialize(world)`: Initialize with world
- `getSensoryInputSize()`: Get sensory input size
- `getMotorOutputSize()`: Get motor output size
- `processSensoryInput(percept)`: Process percept
- `decodeMotorCommand()`: Decode motor command
- `apply_reward_modulation(reward, predicted)`: Apply reward
- `updateDevelopment(timestep)`: Update development
- `getDevelopmentalStage()`: Get developmental stage
- `getNeuromodulationLevel()`: Get neuromodulation level
- `getCuriosityLevel()`: Get curiosity level
- `getNoveltyLevel()`: Get novelty level
- `getPredictionError()`: Get prediction error
- `reset()`: Reset agent
- `getBrain()`: Get underlying brain
- `enableRewardModulation(enable)`: Enable reward modulation
- `enableStructuralPlasticity(enable)`: Enable structural plasticity
- `enableDevelopment(enable)`: Enable development
- `enableCuriosity(enable)`: Enable curiosity
- `isRewardModulationEnabled()`: Check reward modulation
- `isStructuralPlasticityEnabled()`: Check structural plasticity
- `isDevelopmentEnabled()`: Check development
- `isCuriosityEnabled()`: Check curiosity

#### Vision (SensoryInput)
- `Vision()`: Default constructor
- `Vision(width, height, channels)`: Create vision input
- `setData(data)`: Set data
- `set_data_numpy(array)`: Set from NumPy array
- `getData()`: Get data
- `get_data_numpy()`: Get as NumPy array
- `getWidth()`: Get width
- `getHeight()`: Get height
- `getChannels()`: Get channels
- `toImage()`: Convert to PIL Image
- `saveImage(filepath)`: Save as image
- `getHistogram()`: Get intensity histogram
- `getStatistics()`: Get statistical summary
- `normalize()`: Normalize to [0,1]
- `applyFilter(kernel)`: Apply convolution filter
- `resize(width, height)`: Resize image
- `toGray()`: Convert to grayscale
- `getEdges()`: Detect edges

#### SensoryPercept
- `SensoryPercept()`: Default constructor
- `getVision()`: Get vision input
- `setVision(vision)`: Set vision input
- `getVisionWidth()`: Get vision width
- `getVisionHeight()`: Get vision height
- `getTouch()`: Get touch input
- `setTouch(touch)`: Set touch input
- `getInternal()`: Get internal signals
- `setInternal(internal)`: Set internal signals
- `getProprioception()`: Get proprioception
- `setProprioception(proprioception)`: Set proprioception
- `getAudio()`: Get audio input
- `setAudio(audio)`: Set audio input
- `getAllSignals()`: Get all signals
- `getTimestamp()`: Get timestamp
- `setTimestamp(timestamp)`: Set timestamp

#### SimpleWorld
- `SimpleWorld()`: Default constructor
- `configure(width, height, visionWidth, visionHeight)`: Configure world
- `reset()`: Reset world
- `setAgentStart(x, y)`: Set agent start position
- `update(timestep)`: Update world
- `applyMotorCommand(cmd, time)`: Apply motor command
- `getSensoryPercept()`: Get sensory percept
- `getAgentBody()`: Get agent body
- `addObject(obj)`: Add world object
- `removeObject(x, y)`: Remove object
- `isValidPosition(x, y)`: Check position validity
- `getWidth()`: Get width
- `getHeight()`: Get height
- `getMaxEnergy()`: Get max energy
- `setMaxEnergy(e)`: Set max energy
- `getEnergyDecayRate()`: Get energy decay rate
- `setEnergyDecayRate(r)`: Set energy decay rate
- `getSimulationTime()`: Get simulation time
- `setRandomSeed(seed)`: Set random seed
- `getRandomSeed()`: Get random seed

### Enumerations

#### NeuronType
- `Excitatory`: Excitatory neuron
- `Inhibitory`: Inhibitory neuron
- `Modulatory`: Modulatory neuron
- `Sensory`: Sensory neuron
- `Motor`: Motor neuron
- `Internal`: Internal neuron

#### SynapseType
- `Excitatory`: Excitatory synapse
- `Inhibitory`: Inhibitory synapse
- `Modulatory`: Modulatory synapse
- `Electrical`: Electrical synapse
- `GapJunction`: Gap junction

#### DevelopmentalStage
- `Initial`: Initial stage
- `CriticalPeriod`: Critical period
- `Maturation`: Maturation stage
- `Adult`: Adult stage
- `Aging`: Aging stage

#### FiringState
- `Resting`: Resting state
- `Active`: Active state
- `Refractory`: Refractory state
- `Inhibited`: Inhibited state

#### ActionType
- `MoveForward`: Move forward
- `MoveBackward`: Move backward
- `MoveLeft`: Move left
- `MoveRight`: Move right
- `TurnLeft`: Turn left
- `TurnRight`: Turn right
- `Look`: Look
- `LookUp`: Look up
- `LookDown`: Look down
- `Interact`: Interact
- `Eat`: Eat
- `Drink`: Drink
- `Rest`: Rest
- `Wait`: Wait
- `Custom`: Custom action

#### MotorCommand
- `MoveForward`: Move forward
- `MoveBackward`: Move backward
- `TurnLeft`: Turn left
- `TurnRight`: Turn right
- `LookLeft`: Look left
- `LookRight`: Look right
- `Interact`: Interact
- `Wait`: Wait

#### WorldObjectType
- `Empty`: Empty object
- `Resource`: Resource object
- `Hazard`: Hazard object
- `Wall`: Wall object
- `Marker`: Marker object

### Constants

#### Invalid IDs
- `INVALID_NEURON_ID`: Invalid neuron ID
- `INVALID_SYNAPSE_ID`: Invalid synapse ID
- `INVALID_REGION_ID`: Invalid region ID
- `INVALID_POPULATION_ID`: Invalid population ID

## Performance Considerations

### Memory Management
- Use `compress_memory()` to reduce memory footprint
- Monitor memory usage with `get_memory_usage()`
- Enable development only when needed

### Simulation Control
- Use `step_adaptive()` for variable performance requirements
- Pause simulation for debugging and setup
- Use `step_until()` for condition-based simulation

### NumPy Integration
- Use NumPy arrays for fast numerical operations
- Leverage vectorized operations for performance
- Use `get_data_numpy()` and `set_data_numpy()` for efficient data transfer

### Development Features
- Enable development only during learning phases
- Use `set_developmental_target()` to control development pace
- Monitor developmental stage with `get_developmental_stage()`

## Troubleshooting

### Common Issues

**Issue**: Brain doesn't fire neurons
**Solution**: Check that `initialize()` is called before `step()`. Ensure sufficient input current is injected.

**Issue**: Configuration validation fails
**Solution**: Check `config.validate()` and review configuration file. Use `config.get_string()` to debug configuration values.

**Issue**: NumPy array operations are slow
**Solution**: Use `get_data_numpy()` and `set_data_numpy()` for efficient NumPy integration. Ensure arrays are properly sized.

**Issue**: Memory usage is high
**Solution**: Call `compress_memory()` periodically. Monitor usage with `get_memory_usage()`. Disable unnecessary features.

**Issue**: Simulation is too slow
**Solution**: Use `step_adaptive()` with appropriate target FPS. Reduce simulation complexity. Use `profile_step()` to identify bottlenecks.

### Error Messages

**ConfigurationError**: Invalid configuration file or parameters
**MemoryError**: Out of memory or memory allocation failure
**ValidationError**: Input validation failed
**IndexError**: Array index out of bounds
**TypeError**: Type conversion or validation failed

## Future Enhancements

The NLM Python bindings will continue to evolve with:

1. **Advanced Machine Learning Integration**: TensorFlow/PyTorch interoperability
2. **GPU Acceleration**: CUDA support for faster computations
3. **Distributed Computing**: Multi-node simulation capabilities
4. **Advanced Visualization**: Real-time plotting and animation
5. **Cloud Integration**: AWS/GCP integration for scalable simulations
6. **Mobile Support**: iOS/Android mobile app bindings
7. **Web Interface**: JavaScript bindings for web-based simulations
8. **Advanced Learning Algorithms**: Deep reinforcement learning integration

## Conclusion

The enhanced NLM Python bindings provide a powerful, user-friendly interface for neural simulation research and development. With advanced configuration management, performance monitoring, NumPy integration, and comprehensive error handling, users can build sophisticated neural systems for a wide range of applications.

The improved API design follows Python conventions while maintaining access to the full power of the underlying C++ neural simulation engine. Whether you're a beginner looking for simple examples or an advanced user requiring maximum performance, the enhanced NLM Python bindings provide the tools you need.

Get started today and discover the next generation of neural simulation! 🚀
