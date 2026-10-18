# NLM Python API Reference Documentation
# Comprehensive Guide to Advanced Python API Features

## Overview

The NLM Python API provides comprehensive access to the Neural Learning Machine framework, enabling users to create, simulate, and analyze artificial neural systems through Python. This documentation covers both basic and advanced API features.

## Quick Start

### Basic Usage

```python
import pynlm

# Create and initialize a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### Advanced Usage

```python
import pynlm

# Create a complete agent simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Setup world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run episode
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Core Classes

### Brain

The central neural simulation class representing an artificial brain.

#### Creation and Initialization

```python
# Create with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Create with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15f)
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()
```

#### Core Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `initialize()` | Initialize the brain with configuration | None | None |
| `step(step)` | Perform one simulation step | step (int) | None |
| `step(step, time)` | Perform step with timestamp | step (int), time (float) | None |
| `reset()` | Reset brain to initial state | None | None |
| `save(filepath)` | Save brain state to file | filepath (str) | bool |
| `load(filepath)` | Load brain state from file | filepath (str) | bool |

#### Properties and Statistics

```python
# Neural statistics
brain.getTotalNeuronCount()        # Total neurons in brain
brain.getTotalSynapseCount()        # Total synapses in brain
brain.getActiveNeuronCount()        # Currently active neurons
brain.getFiringNeuronCount()        # Currently firing neurons
brain.getAverageFiringRate()        # Average firing rate (Hz)
brain.getTotalSpikeCount()          # Total spikes generated
brain.getExcitationInhibitionRatio() # E/I balance ratio

# State information
brain.getDevelopmentalStage()       # Current developmental stage
brain.addRegion("cortex")           # Add neural region
brain.getRegion(region_id)          # Get region by ID
brain.getRegionIds()                # Get all region IDs

# Configuration access
config = brain.getConfig()
```

#### Memory Systems Access

```python
# Access integrated memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()
prediction_system = brain.getPredictionSystem()

# Working memory operations
wm_traces = working_memory.getActiveTraces()
wm_capacity = working_memory.getCapacity()
wm_strength = working_memory.getMemoryActivity()

# Episodic memory operations
episodes = episodic_memory.getEpisodeCount()
episode_data = episodic_memory.getEpisodeById(episode_id)
```

### Config

Configuration management class for NLM system parameters.

#### Basic Operations

```python
# Create configuration
config = pynlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.001)

# Get configuration values
neuron_count = config.get("brain.neuron_count")
if config.has("brain.neuron_count"):
    print(f"Neuron count: {neuron_count}")

# Configuration keys
keys = config.getKeys()

# File operations
config.loadFromFile("config.json")
config.saveToFile("config.json")

# Command line arguments
config.loadFromArgs(len(sys.argv), sys.argv)
```

#### Configuration Categories

**Brain Configuration**
- `brain.neuron_count`: Total number of neurons
- `brain.synapse_density`: Synapse connectivity density
- `brain.connection_probability`: Connection probability between neurons
- `brain.initial_weight_mean`: Mean initial synaptic weight
- `brain.v_thresh`: Neuron threshold potential (mV)
- `brain.v_rest`: Resting potential (mV)
- `brain.v_reset`: Reset potential after spike (mV)
- `brain.tau_mem`: Membrane time constant (ms)
- `brain.tau_ref`: Refractory period (ms)

**Plasticity Configuration**
- `plasticity.stdp.enable`: Enable STDP
- `plasticity.stdp.learning_rate`: STDP learning rate
- `plasticity.stdp.tau_plus`: STDP time constant (ms)
- `plasticity.hebbian.enable`: Enable Hebbian learning
- `plasticity.structural.enable`: Enable structural plasticity

**Neuromodulation Configuration**
- `neuromod.dopamine.scale`: Dopamine modulation scale
- `neuromod.curiosity.enable`: Enable curiosity-driven exploration
- `neuromod.novelty.enable`: Enable novelty detection

### AgentBrain

Agent brain interface connecting NLM brain to world simulation.

#### Core Operations

```python
# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)  # Process sensory data

# Motor decoding
action = agent.decodeMotorCommand()  # Get action from neural activity

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)  # Reward learning
agent.updateDevelopment(timestep)  # Development updates
```

#### Learning Features

```python
# Enable learning subsystems
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Check learning status
agent.isRewardModulationEnabled()      # Check reward modulation
agent.isCuriosityEnabled()              # Check curiosity
agent.isDevelopmentEnabled()            # Check development
```

#### Statistics

```python
# Get system statistics
agent.getCuriosityLevel()      # Current curiosity level
agent.getNoveltyLevel()        # Current novelty level
agent.getPredictionError()     # Current prediction error
agent.getNeuromodulationLevel() # Current neuromodulation level
agent.getDevelopmentalStage()  # Current developmental stage
```

### SimpleWorld

Simple 2D world for NLM simulation.

#### World Management

```python
# Create world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# World control
world.reset()                    # Reset world to initial state
world.update(timestep)            # Update world by timestep
world.setAgentStart(x, y)         # Set agent starting position

# Agent interaction
world.applyMotorCommand(cmd, time)  # Apply motor command to world

# State access
percept = world.getSensoryPercept()  # Get sensory percept
body = world.getAgentBody()         # Get agent body state
```

#### World Objects

```python
# Create and manage world objects
world_obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
world.addObject(world_obj)
world.removeObject(x, y)
world.isValidPosition(x, y)
```

### SensoryInput

Base class for sensory input.

```python
# Vision input
vision = pynlm.Vision(width, height, channels=3)
vision.setData(data_vector)

# Audio input
audio = pynlm.Audio(sample_rate, num_samples)
audio.setSampleRate(sample_rate)

# Internal signals
internal = pynlm.InternalSignals()
internal.addSignal(value)
```

### Action

Action representation for motor output.

```python
# Create action from type
action = pynlm.Action(pynlm.ActionType.MoveForward)

# Create action with parameters
action = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.3, 0.8])

# Action properties
action.getType()                   # Get action type
action.getParameters()             # Get action parameters
action.getName()                   # Get action name
```

## Advanced Python Features

### 1. Context Managers

The API provides context managers for resource management:

```python
# Example of advanced usage with context
with pynlm.createExperiment(config) as experiment:
    experiment.initialize()
    for step in range(100):
        experiment.step(step)
    results = experiment.getResults()
```

### 2. Custom Exception Classes

```python
import pynlm

try:
    # Your NLM code here
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.NLMError as e:
    print(f"NLM runtime error: {e}")
except pynlm.NLMValueError as e:
    print(f"NLM value error: {e}")
except pynlm.NLMIndexError as e:
    print(f"NLM index error: {e}")
```

### 3. Batch Processing

```python
# Batch process multiple sensory inputs
inputs = [pynlm.Vision(8, 8) for _ in range(10)]
brain.receiveSensoryInput(inputs)

# Batch brain steps
def run_batch(brain, steps):
    for step in range(steps):
        brain.step(step)
    return brain.getTotalSpikeCount()

spikes = run_batch(brain, 1000)
```

### 4. Data Persistence

```python
# Save experiment data
pynlm.save_experiment_data("experiment.json", brain, world, metrics)

# Load experiment data
metrics = pynlm.load_experiment_data("experiment.json")

# Compare experiments
comparison = pynlm.compare_experiments({
    "exp1": results1,
    "exp2": results2
})
```

### 5. Benchmarking

```python
# Comprehensive benchmarking
results = pynlm.benchmark_brain(brain, num_steps=1000, description="My benchmark")

# Analyze results
print(f"Performance: {results['performance']['steps_per_second']:.1f} steps/second")
print(f"Neural Activity: {results['neural_activity']['final_total_spikes']} spikes")
```

## Advanced Configuration

### Complex Configuration Example

```python
import pynlm

# Create highly optimized configuration
config = pynlm.createDefaultConfig()

# Brain parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.12f)
config.set("brain.connection_probability", 0.08f)
config.set("brain.v_thresh", -50.0f)
config.set("brain.v_rest", -70.0f)
config.set("brain.v_reset", -75.0f)

# Plasticity parameters
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001f)
config.set("plasticity.stdp.tau_plus", 20.0f)
config.set("plasticity.stdp.tau_minus", 20.0f)

config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.0005f)

config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.00005f)
config.set("plasticity.structural.pruning_rate", 0.000005f)

# Neuromodulation parameters
config.set("neuromod.dopamine.scale", 1.0f)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Memory parameters
config.set("memory.working_memory.capacity", brain.getTotalNeuronCount() // 10)
config.set("memory.episodic_memory.max_episodes", 10000)

# Development parameters
config.set("development.initial_stage", "CriticalPeriod")
config.set("development.plasticity_rate", 1.0f)

# Simulation parameters
config.set("simulation.timestep", 0.001f)
config.set("simulation.max_steps", 10000)
```

## Error Handling

### Common Errors and Solutions

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()  # Always call initialize first!
except pynlm.NLMError as e:
    print(f"Brain initialization error: {e}")
    # Check configuration
    if not config.has("brain.neuron_count"):
        print("Warning: neuron_count not specified, using default")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Error Types

- **NLMError**: General runtime errors
- **NLMValueError**: Invalid parameter values
- **NLMIndexError**: Invalid array indices

## Performance Optimization

### Tips for Better Performance

```python
# 1. Use efficient configurations
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # Start small

# 2. Batch process when possible
for batch in range(num_batches):
    inputs = get_batch_of_inputs()
    brain.receiveSensoryInput(inputs)
    brain.step(batch * batch_size)

# 3. Use appropriate timestep
config.set("simulation.timestep", 0.001)  # Balance accuracy/speed

# 4. Monitor performance
results = pynlm.benchmark_brain(brain, 1000)
print(f"Performance: {results['performance']['steps_per_second']:.1f} steps/second")
```

## Examples Directory

The `examples/` directory contains comprehensive example applications:

### example1_basic.py
- **Purpose**: Basic brain simulation demonstration
- **Difficulty**: Beginner
- **Concepts**: Simple brain creation, basic simulation, neural activity monitoring

### example2_memory.py
- **Purpose**: Memory systems integration demonstration
- **Difficulty**: Intermediate
- **Concepts**: Working memory, episodic memory, memory consolidation, replay

### example3_advanced.py
- **Purpose**: Advanced API features demonstration
- **Difficulty**: Advanced
- **Concepts**: Benchmarking, experiment comparison, data persistence, professional reporting

## Testing and Validation

### Running Tests

```bash
# Run memory system integration tests
cd /path/to/nlm/src/tests
g++ -std=c++20 -I../src -I../core -I../brain -I../memory test_memory_integration.cpp -o test_memory
./test_memory

# Or use CMake build system
cmake --build build --target test
./build/nlm_test
```

### Test Coverage

The test suite includes:
- **Memory System Integration Tests**: Verify all memory systems work together
- **Stress Tests**: Test system stability under load
- **Advanced Feature Tests**: Test integrated neuromodulation, development, and cognition
- **Performance Tests**: Benchmark system performance

## Troubleshooting

### Common Issues and Solutions

**Issue 1: "Brain isn't doing anything"**
```python
# Solution: Check initialization
brain = pynlm.createBrain(config)
brain.initialize()  # Always call this!
```

**Issue 2: "The agent isn't moving"**
```python
# Solution: Check world update
world.update(0.1)  # Update world before applying action
```

**Issue 3: "Everything is 0"**
```python
# Solution: Allow brain to warm up
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

**Issue 4: ImportError: No module named 'pynlm'**
```bash
# Solution: Install properly
pip install --force-reinstall .
```

**Issue 5: Segmentation faults**
```python
# Solution: Proper initialization and error handling
config = pynlm.createDefaultConfig()
if config.has("brain.neuron_count"):
    config.set("brain.neuron_count", 100)  # Safe start
```

## Migration Guide

### From Basic to Advanced API

**Basic Usage**
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(100):
    brain.step(step)
```

**Advanced Usage**
```python
import pynlm

# Create optimized configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)

# Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# Setup agent and world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)

# Run comprehensive simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Analyze results
metrics = {
    'total_spikes': brain.getTotalSpikeCount(),
    'firing_neurons': brain.getFiringNeuronCount(),
    'curiosity': agent.getCuriosityLevel()
}
```

### Best Practices

1. **Always initialize** the brain before use
2. **Use meaningful variable names** for complex simulations
3. **Implement error handling** for robustness
4. **Batch process** when working with multiple steps
5. **Monitor performance** regularly for large simulations
6. **Use context managers** for resource management
7. **Document your experiments** with comprehensive metrics

## Future Enhancements

The API is continuously being enhanced with new features:

### Planned Features
- **Distributed Computing**: Parallel processing across multiple cores
- **GPU Acceleration**: NVIDIA CUDA support for faster simulation
- **Visualization Tools**: Real-time neural activity visualization
- **Cloud Integration**: Easy deployment to cloud platforms
- **Mobile Support**: Lightweight NLM versions for mobile devices

### Contribution Guidelines

We welcome contributions to improve the NLM Python API:

1. **Report Issues**: Use GitHub issues for bug reports and feature requests
2. **Submit Pull Requests**: Follow the project's coding standards
3. **Improve Documentation**: Enhance examples and API documentation
4. **Performance Optimizations**: Improve benchmark and performance tools
5. **New Features**: Implement new memory systems, neuromodulators, or cognitive architectures

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## References

1. **NLM Documentation**: See `docs/` for comprehensive documentation
2. **Scientific Papers**: See `docs/SCIENCE.md` for scientific background
3. **Architecture**: See `docs/ARCHITECTURE.md` for system design
4. **Roadmap**: See `docs/ROADMAP.md` for future development plans

## Acknowledgements

This API was developed as part of the NLM (Neural Learning Machine) project, an experimental artificial developmental brain research initiative.

For more information, visit the project website or documentation.
