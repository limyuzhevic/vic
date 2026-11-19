# NLM User Guide

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain project that implements real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

## Quick Start

### Installation

NLM is a C++ framework that can be built using CMake. The project includes Python bindings for easier integration with Python-based tools.

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build with Python bindings (default)
mkdir build
cd build
cmake -D NLM_BUILD_PYTHON_BINDINGS=ON ..
make -j$(nproc)

# Or build without Python bindings
mkdir build
cd build
cmake -D NLM_BUILD_PYTHON_BINDINGS=OFF ..
make -j$(nproc)
```

### Running Demos

The project includes several demonstration executables:

```bash
# Phase 2: Real neural computation (basic)
./nlm

# Phase 3: World interaction
./nlm_phase3_demo

# Phase 4: Prediction and memory
./nlm_phase4_demo

# Phase 5: Pattern probe and sandbox
./nlm_phase5_demo

# Phase 6: Full integration demo
./nlm_phase6_demo

# Phase 6: Text/Thought demo
./nlm_phase6_text_demo

# Benchmark performance
./nlm_benchmark
```

### Python Bindings

```python
import pynlm

# Create a brain
brain = pynlm.Brain()
brain.initialize()

# Run simulation
brain.step(0)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
```

## Configuration

NLM uses a flexible configuration system that supports both file-based and command-line configuration.

### Configuration File Format

The default configuration file is `configs/default.cfg`:

```ini
# NLM Configuration File
# This is the default configuration for Phase 1 skeleton

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.0

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

### Command-Line Configuration

NLM supports command-line arguments for overriding configuration values:

```bash
./nlm --random_seed=123 --neuron_count=500 --connection_probability=0.05 --viz-mode=all
./nlm_phase3_demo --step-count=100 --random-seed=42
./nlm_phase6_demo --experiment=sandbox --viz-export=svg
```

### Advanced Configuration Options

```bash
# Show all available options
./nlm --help

# Show system information
./nlm --info

# Show version information
./nlm --version

# Load custom configuration file
./nlm --config=my_config.cfg

# Set configuration value
./nlm --config-set=neuron_count=2000

# Dump current configuration
./nlm --config-dump
```

## Using Demos

### Phase 2: Basic Neural Computation

The Phase 2 demo demonstrates the core neural computation capabilities:

```bash
./nlm
```

**What you'll see:**
- Real LIF neuron dynamics
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)
- Learning experiment showing measurable synaptic changes

**Features:**
- Three test suites: Basic connectivity, plasticity learning, and STDP verification
- Detailed logging of neural activity and synaptic changes
- Configurable through command-line arguments

### Phase 3: World Interaction

The Phase 3 demo shows how the brain interacts with an environment:

```bash
./nlm_phase3_demo [num_episodes] [steps_per_episode]
```

**Features:**
- Sensory perception and motor output
- Closed-loop interaction with SimpleWorld
- Reward-based learning
- Memory integration (working memory, episodic memory)
- Curiosity and novelty-driven exploration
- Checkpoint and replay capabilities

### Phase 6: Integration Demo

The Phase 6 demo demonstrates the fully integrated brain system:

```bash
./nlm_phase6_demo
```

**Features:**:
- All brain systems integrated: memory, neuromodulation, prediction, cognition, development
- Complete lifecycle from perception to action
- Detailed metrics and performance tracking
- Visualization options
- Checkpoint and replay capabilities

## Debugging and Monitoring

### Debug Mode

Enable debug mode for detailed output:

```bash
./nlm --debug=2 --verbose
./nlm_phase3_demo --debug --viz-network --viz-activity
```

### Visualization Options

```bash
# Set visualization mode
./nlm --viz-mode=network    # Display neural network structure
./nlm --viz-mode=activity   # Show neural activity patterns
./nlm --viz-mode=spikes     # Display spike raster plots
./nlm --viz-mode=weights    # Show weight matrix visualization
./n
lm --viz-mode=all         # All visualization modes

# Set visualization update rate
./nlm --viz-update-rate=15.0

# Enable file export
./nlm --viz-export=svg      # Export to SVG format
./nlm --viz-export=png      # Export to PNG format
```

### Metrics and Performance

```bash
# Save metrics to file
./nlm --metrics-file=brain_metrics.json
./nlm_phase3_demo --metrics-file=phase3_metrics.csv

# Performance profiling
./nlm --perf-profile=true
```

## Python Bindings

### Installation

```bash
# Install Python bindings
pip install .

# Or build and install from source
mkdir build
cd build
cmake -D NLM_BUILD_PYTHON_BINDINGS=ON ..
make -j$(nproc)
pip install .
```

### Usage Examples

```python
import pynlm

# Create and initialize brain
brain = pynlm.Brain()
brain.initialize()

# Configure brain
config = {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "random_seed": 42
}
brain.configure(config)

# Main simulation loop
for step in range(1000):
    brain.step(step)
    
    # Get current state
    neurons = brain.get_neurons()
    synapses = brain.get_synapses()
    
    # Log metrics
    print(f"Step {step}: {len(neurons)} neurons, "
          f"{len(synapses)} synapses")

# Save brain state
brain.save("brain_checkpoint.bin")

# Load brain state
new_brain = pynlm.Brain()
new_brain.load("brain_checkpoint.bin")
```

### Python API Reference

#### Core Classes

**Brain**
- `__init__(config=None)` - Create brain with optional configuration
- `initialize()` - Initialize brain with current configuration
- `step(step_num)` - Run simulation step
- `reset()` - Reset brain to initial state
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getAverageFiringRate()` - Get average firing rate
- `getTotalSpikeCount()` - Get total spike count
- `getWorkingMemory()` - Get working memory instance
- `getEpisodicMemory()` - Get episodic memory instance
- `getPredictionSystem()` - Get prediction system instance
- `getPlanner()` - Get neural planner instance
- `getConceptFormation()` - Get concept formation instance
- `getAttention()` - Get attentional selection instance
- `getDevelopmentSystem()` - Get development system instance
- `getDopamine()` - Get dopamine neuromodulator instance
- `getCuriosity()` - Get curiosity neuromodulator instance
- `getNovelty()` - Get novelty detection instance
- `getPredictionError()` - Get prediction error instance
- `configure(config)` - Configure brain with dictionary
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file

**Config**
- `__init__()` - Create empty configuration
- `set(key, value, source=ConfigSource.Default)` - Set configuration value
- `get(key, default_value=None)` - Get configuration value
- `getOr(key, default_value)` - Get configuration with default
- `loadFromFile(filepath)` - Load configuration from file
- `saveToFile(filepath)` - Save configuration to file

**Neuron**
- `getId()` - Get neuron ID
- `getType()` - Get neuron type
- `getMembranePotential()` - Get membrane potential
- `getThreshold()` - Get threshold voltage
- `injectCurrent(current)` - Inject current into neuron
- `getState()` - Get neuron state

**Synapse**
- `getWeight()` - Get synaptic weight
- `enablePlasticity(hebbian, stdp, structural)` - Enable plasticity types
- `getSourceNeuron()` - Get source neuron
- `getDestinationNeuron()` - Get destination neuron

**Action**
- `getCommand()` - Get motor command
- `getActivity()` - Get activity level
- `getFiringRate()` - Get firing rate

#### Integration Classes

**AgentBrain**
- `__init__(brain)` - Create agent brain interface
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, prediction_error)` - Apply reward modulation
- `updateDevelopment(delta_time)` - Update development

**SimpleWorld**
- `__init__()` - Create world
- `configure(width, height, vision_width, vision_height)` - Configure world dimensions
- `getSensoryPercept()` - Get sensory percept
- `applyMotorCommand(command, time)` - Apply motor command
- `update(delta_time)` - Update world simulation
- `reset()` - Reset world

**SensoryPercept**
- `setVision(vision_array)` - Set visual input
- `setTouch(touch_array)` - Set tactile input
- `setInternal(internal_array)` - Set internal signals
- `setProprioception(proprioception_array)` - Set proprioception
- `setAudio(audio_array)` - Set audio input

## Tutorials

### Tutorial 1: Creating Your First Brain

This tutorial will guide you through creating and running a simple brain simulation.

```python
import pynlm

# Create brain configuration
config = {
    "neuron_count": 100,
    "region_count": 1,
    "connection_probability": 0.1,
    "random_seed": 42,
    "simulation_timestep": 0.001
}

# Create and initialize brain
brain = pynlm.Brain(config)
brain.initialize()

print("Brain initialized successfully!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)
    
    # Print progress every 20 steps
    if step % 20 == 0:
        print(f"Step {step}: Average firing rate = {brain.getAverageFiringRate():.2f} Hz")

print("Simulation complete!")
```

### Tutorial 2: World Interaction

This tutorial demonstrates how the brain interacts with an environment.

```python
import pynlm

# Create brain and world
brain = pynlm.Brain()
brain.initialize()

world = pynlm.SimpleWorld()
world.configure(20.0, 20.0, 16, 16)  # width, height, vision_width, vision_height

agent = pynlm.AgentBrain(brain)
agent.initialize(world)

# Run interaction loop
for episode in range(5):
    print(f"\n=== Episode {episode} ===")
    
    # Reset world and brain
    world.reset()
    agent.reset()
    
    # Run episode
    total_reward = 0.0
    for step in range(50):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain simulation (10 steps per action)
        for brain_step in range(10):
            brain.step(step * 10 + brain_step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward
        agent.applyRewardModulation(result.reward, 0.0)
        
        # Update world
        world.update(0.1)  # 0.1 second action interval
        
        total_reward += result.reward
        
        print(f"  Step {step}: Action={action.getCommand()}, "
              f"Reward={result.reward:.3f}, "
              f"Total={total_reward:.3f}")

print(f"\nEpisode complete. Total reward: {total_reward:.3f}")
```

### Tutorial 3: Configuration Management

This tutorial shows how to manage brain configuration.

```python
import pynlm

# Create brain
brain = pynlm.Brain()

# Set configuration values
brain.set("neuron_count", 500)
brain.set("region_count", 2)
brain.set("connection_probability", 0.05)
brain.set("random_seed", 123)
brain.set("simulation_timestep", 0.001)

# Get configuration values
print(f"Neuron count: {brain.get('neuron_count', 0)}")
print(f"Region count: {brain.get('region_count', 0)}")
print(f"Connection probability: {brain.get('connection_probability', 0.0)}")

# Update configuration
brain.set("neuron_count", 1000)
print(f"Updated neuron count: {brain.get('neuron_count', 0)}")

# Save configuration
brain.save("my_config.json")

# Load configuration
new_brain = pynlm.Brain()
new_brain.load("my_config.json")

print(f"Loaded neuron count: {new_brain.get('neuron_count', 0)}")
```

## Common Problems and Solutions

### Problem 1: Brain initialization fails

**Symptoms:** Brain initialization returns false or crashes

**Solutions:**
1. Check configuration values:
   ```python
   config = {
       "neuron_count": 100,
       "region_count": 1,
       "connection_probability": 0.1
   }
   ```

2. Verify all required configuration keys are present

3. Check for memory constraints (large networks may need more memory)

### Problem 2: Simulation runs too slowly

**Symptoms:** Simulation step rate is much slower than expected

**Solutions:**
1. Reduce network size:
   ```python
   config["neuron_count"] = 100  # Instead of 1000
   ```

2. Reduce simulation steps

3. Disable visualization during performance testing

4. Use command-line options:
   ```bash
   ./nlm --neuron_count=100 --connection_probability=0.05
   ```

### Problem 3: Configuration file parsing errors

**Symptoms:** Configuration file cannot be loaded or contains invalid values

**Solutions:**
1. Check file format (key = value pairs)

2. Remove comments (lines starting with #)

3. Ensure all values are properly formatted

4. Use command-line options as alternative:
   ```bash
   ./nlm --config-set=neuron_count=100
   ```

### Problem  önemliş: Python bindings not available

**Solutions:**
1. Build with Python bindings:
   ```bash
   cmake -D NLM_BUILD_PYTHON_BINDINGS=ON ..
   ```

2. Install Python bindings separately:
   ```bash
   pip install .
   ```

## Advanced Usage

### Performance Optimization

```bash
# Use performance benchmarking
./nlm_benchmark

# Profile specific operations
./nlm --perf-profile=true --perf-stats=summary

# Run with minimal output for performance
./nlm --quiet
```

### Network Analysis

```python
# Analyze network connectivity
brain = pynlm.Brain()
brain.initialize()

# Get network statistics
stats = brain.getNetworkStatistics()
print(f"Average degree: {stats.averageDegree}")
print(f"Clustering coefficient: {stats.clusteringCoefficient}")
print(f"Path length: {stats.pathLength}")
```

### Learning Analysis

```python
# Track learning progress
brain = pynlm.Brain()
brain.initialize()

learning_metrics = []
for step in range(1000):
    brain.step(step)
    
    metrics = {
        'step': step,
        'firing_rate': brain.getAverageFiringRate(),
        trover_count': brain.getTotalSpikeCount(),
        'weight_change': brain.getTotalWeightChange(),
        'novelty': brain.getNoveltyLevel(),
        'curiosity': brain.getCuriosityLevel()
    }
    learning_metrics.append(metrics)

# Analyze learning progress
import matplotlib.pyplot as plt
steps = [m['step'] for m in learning_metrics]
learning_curves = [m['firing_rate'] for m in learning_metrics]

plt.plot(steps, learning_curves)
plt.xlabel('Step')
plt.ylabel('Firing Rate (Hz)')
plt.title('Learning Progress')
plt.show()
```

## Troubleshooting

### Error Messages

**"Failed to initialize brain!"**
- Solution: Check configuration values, available memory, and system resources

**"Configuration file not found!"**
- Solution: Ensure configuration file exists or use command-line options

**"Visualization not available!"**
- Solution: Build with visualization support or use command-line arguments

**"Python bindings not found!"**
- Solution: Build with Python bindings or install via pip

### System Requirements

**Minimum Requirements:**
- CPU: 2 cores (4+ recommended)
- Memory: 4 GB RAM (8+ GB recommended)
- Disk Space: 1 GB for source, 100 MB for runtime
- C++ Compiler: C++20 compatible (GCC 9+, Clang 10+, MSVC 2019+)

**Recommended Requirements:**
- CPU: 8+ cores
- Memory: 16+ GB RAM
- Disk Space: 500 MB for full installation
- GPU: CUDA-capable GPU for accelerated computation

### Support

For additional help and resources:

1. **Documentation**: https://nlm.readthedocs.io
2. **GitHub Repository**: https://github.com/nlm-project/nlm
3. **Issue Tracker**: https://github.com/nlm-project/nlm/issues
4. **Discussions**: https://github.com/nlm-project/nlm/discussions
5. **Discord Server**: https://discord.gg/nlm
6. **Email Support**: support@nlm-project.org

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgments

Special thanks to:
- All contributors and maintainers
- Research participants who tested these systems
- Open source libraries and frameworks used in development
- Community members who provided feedback and support

## Version Information

Current version: 0.1.0 (Phase 2)
Development phase: Phase 2 (Real Neural Computation)
API version: 0.1.0

---

*This guide is continuously updated with each release. For the latest information, visit the official documentation website.*
