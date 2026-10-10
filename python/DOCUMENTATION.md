# NLM Python Documentation

This document provides comprehensive guidance on using the NLM Python bindings.

## Overview

The NLM (Neural Learning Machine) project now includes a fully-functional Python API that provides access to the powerful C++ brain simulation framework. The Python bindings are built using pybind11 and include enhanced interfaces for better usability.

## Installation

### Prerequisites

- Python 3.8 or higher
- CMake 3.16 or higher
- A C++ compiler (gcc 10+, clang 12+, or MSVC 2019+)

### Quick Installation

For the easiest setup, use the build script:

```bash
# Clone the repository
cd /path/to/nlm

# Build and install Python bindings
python python/build.py --install
```

Or for advanced users who already have the dependencies:

```bash
# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build Python bindings
mkdir -p python/build
cd python/build
cmake ..
make -j4
pip install -e .
```

### Build Options

The build script supports several options:

- `--install`: Install the package after building
- `--tests`: Run tests after building
- `--verbose`: Enable verbose output

### Platform-Specific Notes

**Linux/macOS**:
- Ensure you have a C++ compiler installed
- Install dependencies with:
  ```bash
  sudo apt install build-essential cmake  # Ubuntu/Debian
  brew install cmake gcc  # macOS
  ```

**Windows**:
- Install Visual Studio with C++ support
- Use the Developer Command Prompt
- Install dependencies via pip (as above)

## Basic Usage

### Creating a Simple Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize a brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Agent in a World

```python
import pynlm

# Setup the complete system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run agent simulation
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action and execute it
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)

print("Agent simulation complete!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Novelty: {agent.getNoveltyLevel():.3f}")
print(f"Dev Stage: {brain.getDevelopmentalStage()}")
```

### Using Enhanced Configuration

```python
import pynlm

# Get recommended configuration for testing
config = pynlm.get_default_configuration()

# Modify configuration as needed
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### Benchmark Performance

```python
import pynlm

# Get recommended configuration
config = pynlm.get_default_configuration()

# Run benchmark
results = pynlm.benchmark_brain(config, steps=1000)
print(f"Performance: {results['steps_per_second']:.1f} steps/second")
print(f"Memory efficiency: {results['average_firing_rate']:.3f} Hz average firing rate")
```

### Progress Tracking

```python
import pynlm

# Setup with progress tracking
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create progress tracker
tracker = pynlm.NLMProgressTracker(1000)

for step in range(1000):
    brain.step(step)
    tracker.step(step)
    
    # Do something with brain output
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

## Python API Reference

### Core Classes

#### Config
Represents NLM configuration parameters.

**Methods:**
- `has(key)` - Check if configuration has a key
- `getOr(key, default)` - Get value with default
- `set(key, value, source=None)` - Set configuration value
- `loadFromFile(filename)` - Load from file
- `saveToFile(filename)` - Save to file

#### Brain
Main neural simulation class.

**Methods:**
- `initialize()` - Initialize the brain
- `step(step_number, time=0.0)` - Run one simulation step
- `reset()` - Reset brain state
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getFiringNeuronCount()` - Get currently firing neurons
- `getTotalSpikeCount()` - Get total spike count
- `getAverageFiringRate()` - Get average firing rate
- `getExcitationInhibitionRatio()` - Get E/I balance
- `getDevelopmentalStage()` - Get developmental stage
- `save(filename)` - Save brain state
- `load(filename)` - Load brain state
- `injectCurrent(amount)` - Inject current into sensory neurons
- `injectCurrentToNeurons(neuron_type, amount)` - Inject to specific neuron type
- `logStatus()` - Log brain status

#### AgentBrain
Interface between brain and environment.

**Methods:**
- `initialize(world)` - Initialize with world interface
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Get motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward
- `updateDevelopment(timestep)` - Update development
- `enableRewardModulation(enable)` - Enable/disable reward modulation
- `enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `enableDevelopment(enable)` - Enable/disable development
- `enableCuriosity(enable)` - Enable/disable curiosity
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error
- `getNeuromodulationLevel()` - Get neuromodulation level

#### SimpleWorld
Environment simulation class.

**Methods:**
- `configure(**kwargs)` - Configure world parameters
- `reset()` - Reset world
- `update(timestep)` - Update world
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent body
- `getSimulationTime()` - Get simulation time
- `setAgentStart(x, y)` - Set agent start position
- `applyMotorCommand(command, time)` - Apply motor command
- `addObject(obj)` - Add object to world
- `removeObject(x, y)` - Remove object
- `isValidPosition(x, y)` - Check if position is valid
- `getWidth()` - Get world width
- `getHeight()` - Get world height
- `getMaxEnergy()` - Get max energy
- `setMaxEnergy(energy)` - Set max energy
- `setRandomSeed(seed)` - Set random seed

### Factory Functions

#### pynlm.createDefaultConfig()
Create a default NLM configuration.

#### pynlm.createBrain(config=None)
Create and initialize an NLM brain.

#### pynlm.createAgentBrain(brain)
Create an agent brain interface.

#### pynlm.createSimpleWorld()
Create a simple NLM world.

### Utility Functions

#### pynlm.get_default_configuration()
Get recommended default configuration for common use cases.

#### pynlm.validate_configuration(config)
Validate configuration and return any issues.

#### pynlm.get_system_info()
Get system information about the NLM installation.

### Convenience Functions

#### pynlm.run_minimal_example()
Run a minimal NLM simulation example.

#### pynlm.run_agent_example()
Run a complete agent simulation example.

#### pynlm.benchmark_brain(config, steps)
Benchmark brain performance.

#### pynlm.NLMProgressTracker(total_steps)
Track progress of long-running operations.

### Example Scripts

#### Minimal Example
```python
import pynlm

# Create and initialize
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Done!")
```

#### Agent Example
```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

## Advanced Features

### Learning Configuration

The NLM supports multiple learning mechanisms:

1. **STDP (Spike-Timing-Dependent Plasticity)**: Learning based on spike timing
2. **Hebbian Learning**: Co-activation strengthens synapses
3. **Structural Plasticity**: Creation and elimination of synapses
4. **Neuromodulation**: Dopamine, curiosity, and novelty signals
5. **Development**: Age-dependent plasticity and maturation

### Development Stages

The brain progresses through developmental stages:

- **Initial**: High plasticity, rapid learning
- **CriticalPeriod**: Sensitive learning phase
- **Maturation**: Stable learning rate
- **Adult**: Mature, stable performance
- **Aging**: Decreased plasticity, performance changes

### Action Types

The agent can perform various actions:

- `MoveForward` - Move forward
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Interact` - Interact with environment
- `Eat` - Consume resources
- `Wait` - Do nothing

### World Object Types

Objects in the world have these types:

- `Empty` - Empty space
- `Resource` - Valuable resource
- `Hazard` - Dangerous object
- `Wall` - Physical barrier
- `Marker` - Special marker object

## Performance Tips

### For Large Simulations

```python
import pynlm

# Optimize for large simulations
config = pynlm.get_default_configuration()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.05)

brain = pynlm.createBrain(config)
brain.initialize()

# Run in batches for better memory management
for batch_start in range(0, 10000, 1000):
    batch_end = min(batch_start + 1000, 10000)
    for step in range(batch_start, batch_end):
        brain.step(step)
```

### For Real-Time Applications

```python
import pynlm
import time

# Create a smaller brain for real-time response
config = pynlm.get_default_configuration()
config.set("brain.neuron_count", 500)

brain = pynlm.createBrain(config)
brain.initialize()

# Track performance
start_time = time.time()
steps = 0

for step in range(1000):
    step_start = time.time()
    brain.step(step)
    steps += 1
    
    # Sleep to maintain real-time rate
    elapsed = time.time() - step_start
    if elapsed < 0.1:  # Aim for 10 Hz
        time.sleep(0.1 - elapsed)

end_time = time.time()
print(f"Real-time performance: {steps/(end_time-start_time):.1f} steps/second")
```

### Memory Optimization

```python
import pynlm

# Save intermediate states
brain = pynlm.createBrain()
brain.initialize()

for step in range(0, 10000, 1000):
    # Do some work
    for i in range(step, min(step + 1000, 10000)):
        brain.step(i)
    
    # Save state for later recovery
    brain.save(f"checkpoint_{step//1000}.bin")
    
    # Print progress
    print(f"Completed {min(step + 1000, 10000)} steps")
```

## Troubleshooting

### Common Issues

#### "ImportError: No module named 'pynlm'"

**Solution:**
```bash
pip install -e python/
```

Or build using the build script:
```bash
python python/build.py --install
```

#### "ImportError: libpynlm.so: cannot open shared object file"

**Solution:**
- On Linux: `LD_LIBRARY_PATH=/path/to/nlm/build:$LD_LIBRARY_PATH python -c "import pynlm"`
- On macOS: `DYLD_LIBRARY_PATH=/path/to/nlm/build:$DYLD_LIBRARY_PATH python -c "import pynlm"`
- Build and install the C++ components first

#### "Brain initialization failed"

**Solution:**
1. Check configuration parameters
2. Ensure all required libraries are available
3. Try with a minimal configuration:

```python
import pynlm
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 100)
brain = pynlm.createBrain(config)
brain.initialize()
```

#### "Performance is too slow"

**Solutions:**
1. Reduce neuron count for testing
2. Use fewer plasticity rules
3. Disable development features
4. Increase timestep size
5. Use optimized configuration

#### "Memory usage is too high"

**Solutions:**
1. Reduce brain size
2. Use checkpointing to save intermediate states
3. Disable memory-intensive features
4. Use batch processing

### Getting Help

If you encounter issues:

1. **Check the documentation:** Visit the NLM documentation website
2. **Check examples:** Run the example scripts in `examples/`
3. **Check logs:** Enable verbose logging in your code
4. **Check tests:** Run the test suite
5. **Create issues:** Report bugs on GitHub
6. **Join discussions:** Participate in NLM community forums

### Support Resources

- **Documentation:** https://nlm.readthedocs.io
- **GitHub Repository:** https://github.com/nlm-project/nlm
- **Issue Tracker:** https://github.com/nlm-project/nlm/issues
- **Examples:** `examples/` directory in the project
- **Build Script:** `python/build.py`

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Copyright

Copyright (c) 2025 NLM Project. All rights reserved.