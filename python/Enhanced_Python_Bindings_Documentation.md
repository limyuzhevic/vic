# NLM Python Bindings - Enhanced Documentation

This document provides comprehensive documentation for the enhanced NLM Python bindings, including new features, improvements, and usage examples.

## Overview

The NLM (Neural Learning Machine) Python bindings provide a high-level interface to the C++ neural simulation framework. The enhanced bindings include:

- **Pythonic API design**: Property accessors, convenience methods, and intuitive interfaces
- **Error handling**: Comprehensive exception handling and input validation
- **Visualization support**: Plotting, data export, and analysis tools
- **Performance features**: Batch operations, compression, and optimization
- **Integration features**: NumPy support, callbacks, and event handling
- **Enhanced documentation**: Detailed docstrings and usage examples

## Quick Start

```python
import pynlm

# Create and initialize a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for i in range(100):
    brain.step(i)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

## Core Classes

### Brain Class

The Brain class represents the central neural system. The enhanced bindings include:

#### Basic Operations
- `brain.initialize()` - Initialize the brain
- `brain.step(step_number)` - Run one simulation step
- `brain.reset()` - Reset brain state
- `brain.save(filepath)` - Save brain state
- `brain.load(filepath)` - Load brain state

#### Statistics and Monitoring
- `brain.getStats()` - Get comprehensive statistics as dictionary
- `brain.getHealth()` - Get brain health score (0.0 to 1.0)
- `brain.isHealthy()` - Check if brain is healthy
- `brain.logStatus()` - Log brain status

#### Development Features
- `brain.getDevelopmentalStage()` - Get current developmental stage
- `brain.setDevelopmentalStage(stage)` - Set developmental stage
- `brain.getDevelopmentInfo()` - Get developmental information

#### Enhanced Simulation
- `brain.simulate(sensory_data, steps)` - Simulate with sensory input
- `brain.simulateEpisode(steps, dt)` - Run simulation episode
- `brain.createTestData()` - Create test sensory data

#### Callbacks and Events
- `brain.setStepCallback(callback)` - Set callback for each simulation step
- `brain.setSpikeCallback(callback)` - Set callback for spike events
- `brain.setRewardCallback(callback)` - Set callback for reward events
- `brain.setDevelopmentCallback(callback)` - Set callback for development stage changes

#### Visualization and Analysis
- `brain.plotActivity()` - Plot brain activity
- `brain.getNeuralHeatmap()` - Get neural activity heatmap
- `brain.getSpikingPattern()` - Get spiking pattern
- `brain.exportForViz()` - Export data for visualization
- `brain.getConnectivityMatrix()` - Get connectivity matrix
- `brain.computeNetworkMetrics()` - Compute network topology metrics

#### Performance and Optimization
- `brain.optimizeSynapses()` - Optimize synaptic weights
- `brain.compressState()` - Compress brain state
- `brain.expandState()` - Expand compressed brain state
- `brain.getCompressionRatio()` - Get state compression ratio
- `brain.getMemoryUsage()` - Get memory usage statistics
- `brain.getPerformanceStats()` - Get performance statistics
- `brain.setPerformanceMode(mode)` - Set performance optimization mode

#### Advanced Control
- `brain.runFor(steps, dt)` - Run brain simulation for specified duration
- `brain.runUntil(condition, dt)` - Run brain until condition is met
- `brain.runWithControl(controller, dt)` - Run brain with external controller

#### Network Analysis
- `brain.analyzeConnectivity()` - Analyze brain connectivity patterns
- `brain.getActivityProfile()` - Get neural activity profile
- `brain.detectDynamicsRegime()` - Detect current brain dynamics regime
- `brain.getRegionActivity(region_id)` - Get activity for specific region
- `brain.findActiveRegions(threshold)` - Find regions with activity above threshold

### AgentBrain Class

The AgentBrain class provides the interface between the brain and the world:

#### Basic Operations
- `agent.initialize(world)` - Initialize with world
- `agent.processSensoryInput(percept)` - Process sensory percept
- `agent.decodeMotorCommand()` - Decode motor command
- `agent.reset()` - Reset agent for new episode

#### Neuromodulation
- `agent.applyRewardModulation(reward, predicted_reward)` - Apply reward-based neuromodulation
- `agent.updateDevelopment(timestep)` - Update development system

#### Subsystem Control
- `agent.enableRewardModulation(enable)` - Enable/disable reward modulation
- `agent.enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `agent.enableDevelopment(enable)` - Enable/disable development
- `agent.enableCuriosity(enable)` - Enable/disable curiosity

#### Statistics and State
- `agent.getSystemStats()` - Get agent brain system statistics
- `agent.getLearningState()` - Get current learning state
- `agent.getNeuromodulationLevel()` - Get current neuromodulation level
- `agent.getCuriosityLevel()` - Get curiosity level
- `agent.getNoveltyLevel()` - Get novelty level
- `agent.getPredictionError()` - Get prediction error

#### Convenience Methods
- `agent.getSubsystems()` - Get current subsystem status
- `agent.enableAll()` - Enable all subsystems
- `agent.disableAll()` - Disable all subsystems
- `agent.updateAll(timestep)` - Update all agent systems
- `agent.processInputAndUpdate(percept, timestep)` - Process sensory input and update development
- `agent.getAction(percept)` - Get action from percept
- `agent.runEpisode(world, steps, dt)` - Run complete episode simulation
- `agent.createEpisodeData()` - Create episode data summary

### SimpleWorld Class

The SimpleWorld class represents the simulation environment:

#### Basic Operations
- `world.configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `world.reset()` - Reset world to initial state
- `world.update(timestep)` - Update world simulation
- `world.applyMotorCommand(cmd, currentTime)` - Apply motor command to agent

#### State Access
- `world.getSensoryPercept()` - Get current sensory percept
- `world.getAgentBody()` - Get agent body state

#### World Objects
- `world.addObject(obj)` - Add world object
- `world.removeObject(x, y)` - Remove object at position
- `world.createDefaultObject(x, y, type)` - Create and add default world object
- `world.isValidPosition(x, y)` - Check if position is valid

#### Properties
- `world.getWidth()` - Get world width
- `world.getHeight()` - Get world height
- `world.getMaxEnergy()` - Get maximum energy
- `world.setMaxEnergy(e)` - Set maximum energy
- `world.getEnergyDecayRate()` - Get energy decay rate
- `world.setEnergyDecayRate(r)` - Set energy decay rate
- `world.getSimulationTime()` - Get current simulation time
- `world.getRandomSeed()` - Get random seed
- `world.setRandomSeed(seed)` - Set random seed

#### Utility Methods
- `world.getDistance(x1, y1, x2, y2)` - Calculate Euclidean distance
- `world.teleportAgent(x, y)` - Teleport agent to position

### Vision Class

The Vision class represents visual sensory input:

#### Basic Operations
- `Vision(width, height, channels)` - Create vision with specified dimensions
- `vision.setData(data)` - Set image data from vector or numpy array
- `vision.getData()` - Get image data as vector
- `vision.getWidth()` - Get image width
- `vision.getHeight()` - Get image height
- `vision.getChannels()` - Get number of channels

#### Data Processing
- `vision.validateData()` - Validate vision data dimensions
- `vision.normalize(min=0.0, max=1.0)` - Normalize vision data to range
- `vision.getDataAsArray()` - Get vision data as numpy array (read-only)
- `vision.copyData()` - Copy vision data as Python list

#### Pixel Access
- `vision.getPixel(x, y, c=0)` - Get pixel value at position (x, y) channel c
- `vision.setPixel(x, y, c, value)` - Set pixel value at position (x, y) channel c

#### Transformations
- `vision.reshape(width, height, channels)` - Create new Vision with different shape
- `vision.resize(width, height)` - Resize image

### Audio Class

The Audio class represents audio sensory input:

#### Basic Operations
- `Audio(sampleRate, numSamples)` - Create audio with specified parameters
- `audio.setData(data)` - Set audio data from vector or numpy array
- `audio.getData()` - Get audio data as vector
- `audio.getSampleRate()` - Get sample rate
- `audio.getNumSamples()` - Get number of samples
- `audio.setSampleRate(sampleRate)` - Set sample rate

#### Analysis
- `audio.validateData()` - Validate audio data dimensions
- `audio.normalize(min=0.0, max=1.0)` - Normalize audio data to range
- `audio.getDataAsArray()` - Get audio data as numpy array (read-only)
- `audio.copyData()` - Copy audio data as Python list

#### Signal Processing
- `audio.getRMS()` - Get root mean square (RMS) of audio signal
- `audio.getPeak()` - Get peak amplitude of audio signal
- `audio.getEnvelope()` - Get audio envelope (normalized amplitude)

#### Utility Methods
- `audio.getDuration()` - Get audio duration (alias for numSamples)
- `audio.getFrequency()` - Get frequency (placeholder)
- `audio.getDataSlice(start, end)` - Get audio data slice as vector
- `audio.getSampleAt(index)` - Get audio sample at index
- `audio.setSampleAt(index, value)` - Set audio sample at index

### SensoryPercept Class

The SensoryPercept class represents combined sensory input:

#### Signal Access
- `percept.getVision()` - Get vision signal
- `percept.setVision(vision)` - Set vision signal
- `percept.getTouch()` - Get touch signal
- `percept.setTouch(touch)` - Set touch signal
- `percept.getInternal()` - Get internal signals
- `percept.setInternal(internal)` - Set internal signals
- `percept.getProprioception()` - Get proprioception
- `percept.setProprioception(proprioception)` - Set proprioception
- `percept.getAudio()` - Get audio signal
- `percept.setAudio(audio)` - Set audio signal

#### Signal Processing
- `percept.getAllSignals()` - Get all signals as dictionary
- `percept.getSignal(type)` - Get signal data by type
- `percept.setSignal(type, data)` - Set signal data by type

#### Data Conversion
- `percept.to_dict()` - Convert percept to Python dictionary
- `percept.from_dict(data)` - Update percept from Python dictionary

### Config Class

The Config class manages brain configuration:

#### Basic Operations
- `config.loadFromFile(filepath)` - Load configuration from JSON file
- `config.loadFromArgs(argc, argv)` - Load configuration from command line arguments
- `config.saveToFile(filepath)` - Save configuration to JSON file
- `config.clear()` - Clear all configuration entries

#### Query Methods
- `config.has(key)` - Check if a configuration key exists
- `config.getKeys()` - Get all configuration keys
- `config.summary()` - Get a summary string of the configuration

### Action Class

The Action class represents motor output:

#### Basic Operations
- `Action(type)` - Create action with specified type
- `Action(type, parameters)` - Create action with type and parameters
- `action.getType()` - Get action type
- `action.setType(type)` - Set action type
- `action.getParameters()` - Get action parameters
- `action.setParameters(params)` - Set action parameters
- `action.getName()` - Get action name
- `action.clone()` - Create a copy of the action

### WorldObject Class

The WorldObject class represents objects in the world:

#### Basic Operations
- `WorldObject(x, y, type, value, radius)` - Create world object with position and properties
- `obj.x`, `obj.y` - Object position
- `obj.type` - Object type
- `obj.value` - Object value
- `obj.radius` - Object radius
- `obj.active` - Object active state

### AgentBody Class

The AgentBody class represents agent state:

#### Properties
- `body.x`, `body.y` - Agent position
- `body.orientation` - Agent orientation
- `body.velocityX`, `body.velocityY` - Agent velocity
- `body.angularVelocity` - Agent angular velocity
- `body.energy` - Agent energy
- `body.health` - Agent health
- `body.age` - Agent age
- `body.isMoving` - Agent moving state
- `body.isTurning` - Agent turning state
- `body.lastActionTime` - Agent last action time

### ActionResult Class

The ActionResult class represents result of actions:

#### Properties
- `result.reward` - Reward value
- `result.success` - Success flag
- `result.message` - Result message

### Enumerations

The following enumerations are available:

#### NeuronType
- `pynlm.NeuronType.Excitatory` - Excitatory neuron
- `pynlm.NeuronType.Inhibitory` - Inhibitory neuron
- `pynlm.NeuronType.Modulatory` - Modulatory neuron
- `pynlm.NeuronType.Sensory` - Sensory neuron
- `pynlm.NeuronType.Motor` - Motor neuron
- `pynlm.NeuronType.Internal` - Internal neuron

#### SynapseType
- `pynlm.SynapseType.Excitatory` - Excitatory synapse
- `pynlm.SynapseType.Inhibitory` - Inhibitory synapse
- `pynlm.SynapseType.Modulatory` - Modulatory synapse
- `pynlm.SynapseType.Electrical` - Electrical synapse
- `pynlm.SynapseType.GapJunction` - Gap junction

#### DevelopmentalStage
- `pynlm.DevelopmentalStage.Initial` - Initial stage
- `pynlm.DevelopmentalStage.CriticalPeriod` - Critical period
- `pynlm.DevelopmentalStage.Maturation` - Maturation stage
- `pynlm.DevelopmentalStage.Adult` - Adult stage
- `pynlm.DevelopmentalStage.Aging` - Aging stage

#### FiringState
- `pynlm.FiringState.Resting` - Resting state
- `pynlm.FiringState.Active` - Active state
- `pynlm.FiringState.Refractory` - Refractory state
- `pynlm.FiringState.Inhibited` - Inhibited state

#### ActionType
- `pynlm.ActionType.MoveForward` - Move forward
- `pynlm.ActionType.MoveBackward` - Move backward
- `pynlm.ActionType.MoveLeft` - Move left
- `pynlm.ActionType.MoveRight` - Move right
- `pynlm.ActionType.TurnLeft` - Turn left
- `pynlm.ActionType.TurnRight` - Turn right
- `pynlm.ActionType.Look` - Look
- `pynlm.ActionType.LookUp` - Look up
- `pynlm.ActionType.LookDown` - Look down
- `pynlm.ActionType.Interact` - Interact
- `pynlm.ActionType.Eat` - Eat
- `pynlm.ActionType.Drink` - Drink
- `pynlm.ActionType.Rest` - Rest
- `pynlm.ActionType.Wait` - Wait
- `pynlm.ActionType.Custom` - Custom action

#### MotorCommand
- `pynlm.MotorCommand.MoveForward` - Move forward
- `pynlm.MotorCommand.MoveBackward` - Move backward
- `pynlm.MotorCommand.TurnLeft` - Turn left
- `pynlm.MotorCommand.TurnRight` - Turn right
- `pynlm.MotorCommand.LookLeft` - Look left
- `pynlm.MotorCommand.LookRight` - Look right
- `pynlm.MotorCommand.Interact` - Interact
- `pynlm.MotorCommand.Wait` - Wait

#### WorldObjectType
- `pynlm.WorldObjectType.Empty` - Empty object
- `pynlm.WorldObjectType.Resource` - Resource object
- `pynlm.WorldObjectType.Hazard` - Hazard object
- `pynlm.WorldObjectType.Wall` - Wall object
- `pynlm.WorldObjectType.Marker` - Marker object

## Usage Examples

### Example 1: Basic Brain Simulation

```python
import pynlm

# Create and initialize a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
    if i % 20 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
```

### Example 2: Complete Agent Simulation

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        if percept.getInternal().getData().size() > 0:
            reward = percept.getInternal().getData()[0]
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    # 8. Get final statistics
    stats = brain.getStats()
    print(f"\nFinal statistics:")
    print(f"Neurons: {stats['neurons']}")
    print(f"Firing neurons: {stats['firing_neurons']}")
    print(f"Total spikes: {stats['total_spikes']}")
    print(f"Firing rate: {stats['firing_rate']:.2f}")
    print(f"Health: {brain.getHealth():.3f}")

# Run the simulation
run_agent_simulation(1000)
```

### Example 3: Enhanced Features

```python
import pynlm
import time

# Create brain with enhanced features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable all subsystems for an agent
agent = pynlm.createAgentBrain(brain)
agent.enableAll()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()
world.setAgentStart(15.0, 15.0)

# Initialize agent
agent.initialize(world)

# Set up callbacks for monitoring

def step_callback(step_num, current_time, reward):
    print(f"Step {step_num}: Time {current_time:.2f}, Reward {reward:.2f}")

brain.setStepCallback(step_callback)

# Run enhanced simulation with performance monitoring
start_time = time.time()

for step in range(500):
    # Update world
    world.update(0.05)
    
    # Get percept and process
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action and apply
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get reward and apply modulation
    if percept.getInternal().getData().size() > 0:
        reward = percept.getInternal().getData()[0]
        agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.05)

end_time = time.time()

# Get performance statistics
perf_stats = brain.getPerformanceStats()
memory_info = brain.getMemoryUsage()
network_metrics = brain.computeNetworkMetrics()
activity_profile = brain.getActivityProfile()
health = brain.getHealth()

print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
print(f"Steps per second: {perf_stats['steps_per_second']:.0f}")
print(f"Memory usage: {memory_info['total_bytes']} bytes")
print(f"Network metrics: {network_metrics}")
print(f"Activity profile: {activity_profile}")
print(f"Brain health: {health:.3f}")

# Get episode data
episode_data = agent.createEpisodeData()
print(f"Episode data keys: {list(episode_data.keys())}")
```

### Example 4: Vision Processing

```python
import pynlm
import numpy as np

# Create vision object
vision = pynlm.Vision(64, 64, 3)

# Create test image data
test_image = np.random.rand(64 * 64 * 3).astype(np.float32) * 255

# Set vision data using numpy array
vision.setData(test_image)

# Process the image
vision.normalize(0.0, 1.0)  # Normalize to [0, 1]

# Access individual pixels
for x in range(0, 64, 10):
    for y in range(0, 64, 10):
        red = vision.getPixel(x, y, 0)
        green = vision.getPixel(x, y, 1)
        blue = vision.getPixel(x, y, 2)
        print(f"Pixel ({x}, {y}): RGB({red:.2f}, {green:.2f}, {blue:.2f})")

# Get data as numpy array (read-only)
image_array = vision.getDataAsArray()
print(f"Image shape: {image_array.shape}")
print(f"Image min/max: {image_array.min():.2f} / {image_array.max():.2f}")

# Get image statistics
width, height, channels = vision.getWidth(), vision.getHeight(), vision.getChannels()
print(f"Image dimensions: {width}x{height}x{channels}")
print(f"Total pixels: {width * height}")
```

### Example 5: Audio Processing

```python
import pynlm
import numpy as np

# Create audio object with 44100 Hz sample rate and 1 second of audio
audio = pynlm.Audio(44100, 44100)

# Generate test audio (sine wave)
t = np.linspace(0, 1, 44100, endpoint=False)
sine_wave = 0.5 * np.sin(2 * np.pi * 440 * t)  # 440 Hz sine wave

# Set audio data
audio.setData(sine_wave.astype(np.float32))

# Process the audio
audio.normalize(-1.0, 1.0)  # Normalize to [-1, 1]

# Get audio statistics
rms = audio.getRMS()
peak = audio.getPeak()
duration = audio.getDuration()
sample_rate = audio.getSampleRate()

print(f"Audio statistics:")
print(f"  Sample rate: {sample_rate} Hz")
print(f"  Duration: {duration/1000:.2f} seconds")
print(f"  RMS: {rms:.3f}")
print(f"  Peak: {peak:.3f}")

# Get envelope
envelope = audio.getEnvelope()
print(f"Envelope min/max: {min(envelope):.3f} / {max(envelope):.3f}")

# Get audio data as numpy array
audio_array = audio.getDataAsArray()
print(f"Audio shape: {audio_array.shape}")
```

### Example 6: World Interaction

```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
world.reset()
world.setAgentStart(25.0, 25.0)

# Add objects to the world
# Resource at position (10, 10)
world.createDefaultObject(10.0, 10.0, "resource")

# Hazard at position (40, 40)
world.createDefaultObject(40.0, 40.0, "hazard")

# Wall at position (25, 10)
world.createDefaultObject(25.0, 10.0, "wall")

# Marker at position (10, 40)
world.createDefaultObject(10.0, 40.0, "marker")

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.enableAll()

agent.initialize(world)

# Run simulation and track agent movement
for step in range(200):
    # Update world
    world.update(0.1)
    
    # Get percept and process
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action and apply
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get agent position
    body = world.getAgentBody()
    print(f"Step {step}: Agent at ({body.x:.1f}, {body.y:.1f}), "
          f"Energy: {body.energy:.1f}, Health: {body.health:.1f}")
    
    # Check for nearby objects
    if body.energy < 10.0:
        print(f"  Low energy! Looking for resources...")
```

### Example 7: Configuration Management

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Get configuration keys
keys = config.getKeys()
print(f"Configuration keys: {keys}")

# Check if specific keys exist
if config.has("brain.neuron_count"):
    print("Brain neuron count is configured")

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.learning_rate", 0.01)

# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")

# Create brain with new configuration
brain = pynlm.createBrain(new_config)
brain.initialize()

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
```

## Installation and Building

### From Source

```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install the package
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml

```bash
# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Building C++ Components

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

## Performance Tips

1. **Enable Performance Mode**: Use `brain.setPerformanceMode("optimized")` for faster simulation
2. **Batch Operations**: Use batch processing methods for large-scale simulations
3. **Memory Management**: Use `brain.compressState()` to reduce memory footprint
4. **Parallel Processing**: Enable SIMD and parallel processing in CMake configuration
5. **Hardware Acceleration**: Configure CMake to use GPU acceleration if available

## Common Issues and Troubleshooting

### Build Issues

**"CMake cannot find Python:"**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**"Compilation errors:"**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**"ImportError: No module named 'pynlm':"**
```bash
pip install --force-reinstall .
```

**"Segmentation faults:"**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify that all objects are properly initialized

**"Vision data validation errors:"**
- Ensure vision data size matches dimensions
- Check that numpy arrays are properly formatted
- Validate that data is not empty

**"Audio processing errors:"**
- Ensure audio data size matches sample count
- Check that numpy arrays are properly formatted
- Verify that sample rate is reasonable

## Advanced Usage

### Using Callbacks for Real-time Monitoring

```python
def create_monitoring_callback():
    """Create a callback for monitoring simulation progress."""
    
    def step_callback(step_num, current_time, reward):
        # Log step information
        print(f"Step {step_num}: Time {current_time:.2f}, Reward {reward:.2f}")
        
        # Save checkpoint periodically
        if step_num % 100 == 0:
            filename = f"checkpoint_step_{step_num}.bin"
            brain.save(filename)
            print(f"Checkpoint saved: {filename}")
        
        # Monitor for unusual behavior
        if abs(reward) > 10.0:
            print(f"Unusual reward detected: {reward}")
    
    return step_callback

# Usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

brain.setStepCallback(create_monitoring_callback())
```

### Data Export for Analysis

```python
def export_simulation_data(brain, world, agent, filename_prefix="simulation"):
    """Export simulation data for external analysis."""
    
    # Export brain state
    brain.exportForViz()
    
    # Get connectivity matrix
    connectivity = brain.getConnectivityMatrix()
    
    # Get network metrics
    metrics = brain.computeNetworkMetrics()
    
    # Get activity profile
    activity = brain.getActivityProfile()
    
    # Get episode data
    episode_data = agent.createEpisodeData()
    
    # Save to files
    import json
    import numpy as np
    
    # Save connectivity matrix
    np.save(f"{filename_prefix}_connectivity.npy", connectivity)
    
    # Save metrics
    with open(f"{filename_prefix}_metrics.json", 'w') as f:
        json.dump(metrics, f, indent=2)
    
    # Save activity profile
    with open(f"{filename_prefix}_activity.json", 'w') as f:
        json.dump(activity, f, indent=2)
    
    # Save episode data
    with open(f"{filename_prefix}_episode.json", 'w') as f:
        json.dump(episode_data, f, indent=2)
    
    print(f"Data exported with prefix: {filename_prefix}")
```

### Custom Visualization

```python
def create_custom_visualization(brain, world, agent):
    """Create custom visualization of simulation data."""
    
    # Get neural activity heatmap
    heatmap = brain.getNeuralHeatmap()
    
    # Get spiking pattern
    spiking_pattern = brain.getSpikingPattern()
    
    # Get connectivity matrix
    connectivity = brain.getConnectivityMatrix()
    
    # Create visualization plots (using matplotlib)
    import matplotlib.pyplot as plt
    import numpy as np
    
    # Plot 1: Neural activity heatmap
    plt.figure(figsize=(15, 5))
    
    plt.subplot(1, 3, 1)
    if not heatmap.empty():
        plt.imshow(heatmap, cmap='hot', interpolation='nearest')
        plt.colorbar(label='Activity')
    plt.title('Neural Activity Heatmap')
    
    # Plot 2: Spiking pattern
    plt.subplot(1, 3, 2)
    if not spiking_pattern.empty():
        plt.plot(spiking_pattern)
    plt.title('Spiking Pattern')
    plt.xlabel('Time step')
    plt.ylabel('Spike count')
    
    # Plot 3: Network connectivity
    plt.subplot(1, 3, 3)
    if not connectivity.empty():
        plt.imshow(connectivity, cmap='viridis', interpolation='nearest')
        plt.colorbar(label='Connection strength')
    plt.title('Network Connectivity')
    
    plt.tight_layout()
    plt.savefig('brain_simulation.png')
    plt.show()
    
    print("Custom visualization created: brain_simulation.png")
```

## License

This project is licensed under the MIT License. See the LICENSE file for more details.

## Authors

NLM Research Team - An Experimental Artificial Developmental Brain Project

For more information, visit:
- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Documentation](https://nlm.readthedocs.io)
- [Research Papers](https://nlm.readthedocs.io/science)
