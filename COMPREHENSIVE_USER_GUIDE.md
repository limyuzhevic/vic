# NLM (Neural Learning Machine) - Comprehensive User Guide

This is the comprehensive user guide for NLM, including both basic and advanced usage. For the original beginner guide, see `easy_usage.md`.

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API
4. Advanced features and usage patterns

## Quick Start

### Minimal Example

```python
import pynlm

# Create and initialize a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for i in range(100):
    brain.step(i)

print("Simulation complete!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
```

### Complete Agent Simulation

```python
import pynlm

# Setup configuration and components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent simulation complete!")
```

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

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

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(f"NLM version: {pynlm.__version__}")

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

## Part 3: Using the NLM Python API

### Basic Usage

#### Creating and Initializing a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with statistics collection
for step in range(1000):
    brain.step(step)
    
    # Collect statistics every 100 steps
    if step % 100 == 0:
        stats = {
            'neurons': brain.getTotalNeuronCount(),
            'firing': brain.getFiringNeuronCount(),
            'spikes': brain.getTotalSpikeCount(),
            'avg_rate': brain.getAverageFiringRate()
        }
        print(f"Step {step}: {stats['firing']} neurons firing, "
              f"{stats['avg_rate']:.2f} avg firing rate")
```

#### Creating and Configuring a World

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=50,           # World width
    height=50,          # World height
    visionWidth=16,     # Vision width
    visionHeight=16     # Vision height
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(25.0, 25.0)

# Run world update
world.update(timestep=0.05)

print(f"World created: {world.getWidth()}x{world.getHeight()}")
print(f"Agent starting position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
```

### Complete Agent Example

```python
import pynlm

def create_advanced_simulation():
    """Create an advanced simulation setup with all features."""
    
    # 1. Create enhanced configuration
    config = pynlm.create_custom_config(
        neuron_count=3000,
        synapse_density=0.18,
        enable_stdp=True,
        enable_hebbian=True,
        enable_curiosity=True
    )
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    
    # 4. Configure world
    world.configure(width=60, height=60, visionWidth=20, visionHeight=20)
    world.reset()
    world.setAgentStart(30.0, 30.0)
    
    # 5. Initialize agent
    agent.initialize(world)
    
    # 6. Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    return brain, agent, world

def run_simulation(brain, agent, world, num_steps):
    """Run a complete simulation episode."""
    
    print(f"Starting {num_steps}-step simulation...")
    print("Features enabled:")
    print(f"  Reward modulation: {agent.isRewardModulationEnabled()}")
    print(f"  Structural plasticity: {agent.isStructuralPlasticityEnabled()}")
    print(f"  Development: {agent.isDevelopmentEnabled()}")
    print(f"  Curiosity: {agent.isCuriosityEnabled()}")
    
    for step in range(num_steps):
        # Update world
        world.update(0.05)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get and apply action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress
        if step % 100 == 0:
            stats = pynlm.getBrainStats(brain)
            print(f"Step {step}: {stats['firing_neurons']} neurons firing, "
                  f"{stats['average_firing_rate']:.2f} Hz, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}")
    
    return brain, agent, world

# Create and run advanced simulation
brain, agent, world = create_advanced_simulation()
brain, agent, world = run_simulation(brain, agent, world, 1000)

# Get final statistics
final_stats = pynlm.getBrainStats(brain)
print(f"\nFinal statistics:")
for key, value in final_stats.items():
    if isinstance(value, float):
        print(f"  {key}: {value:.2f}")
    else:
        print(f"  {key}: {value}")
```

### Enhanced Features Examples

#### 1. Configuration Validation

```python
import pynlm

# Create configuration
config = pynlm.create_custom_config(
    neuron_count=5000,
    synapse_density=0.25,
    enable_stdp=True
)

# Validate configuration
is_valid, message = pynlm.validateConfig(config)
if is_valid:
    print("✓ Configuration is valid!")
else:
    print(f"✗ Configuration errors: {message}")
```

#### 2. Data Persistence

```python
import pynlm

# Create and run a simulation
config = pynlm.create_default_config()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(500):
    brain.step(i)

# Save brain state
print("Saving brain state...")
success = pynlm.export_brain_data(brain, "simulation_checkpoint.bin")
if success:
    print("✓ Brain state saved successfully!")
else:
    print("✗ Failed to save brain state")

# Later, import the saved brain
print("Loading brain state...")
loaded_brain = pynlm.import_brain_data("simulation_checkpoint.bin")
if loaded_brain:
    print(f"✓ Brain state loaded with {loaded_brain.getTotalNeuronCount()} neurons")
else:
    print("✗ Failed to load brain state")
```

#### 3. Batch Simulation

```python
import pynlm

# Create configuration for batch simulation
config = pynlm.create_default_config()

# Run batch simulation
print("Running batch simulation...")
batch_results = pynlm.run_batch_simulation(
    config=config,
    num_episodes=20,
    steps_per_episode=200
)

print(f"\nBatch results:")
for i, result in enumerate(batch_results):
    print(f"Episode {i}: {result['neurons']} neurons, "
          f"{result['spikes']} spikes, {result['avg_rate']:.2f} Hz")
```

#### 4. Machine Learning Experiment

```python
import pynlm

# Setup machine learning experiment configuration
experiment_config = {
    'neuron_count': 10000,
    'synapse_density': 0.3,
    'learning_rate': 0.01,
    'world_size': 100,
    'num_episodes': 50,
    'steps_per_episode': 500
}

# Create configuration
config = pynlm.create_custom_config(
    neuron_count=experiment_config['neuron_count'],
    synapse_density=experiment_config['synapse_density'],
    enable_stdp=True,
    enable_hebbian=True,
    enable_curiosity=True
)

# Create and run multiple experiments
print(f"Running {experiment_config['num_episodes']} machine learning experiments...")
experiment_results = []

for episode in range(experiment_config['num_episodes']):
    # Create new brain for each episode
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=experiment_config['world_size'],
                   height=experiment_config['world_size'],
                   visionWidth=experiment_config['world_size']//5,
                   visionHeight=experiment_config['world_size']//5)
    agent.initialize(world)
    
    # Run episode
    for step in range(experiment_config['steps_per_episode']):
        world.update(0.05)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Collect results
    episode_result = {
        'episode': episode,
        'final_neurons': brain.getTotalNeuronCount(),
        'final_spikes': brain.getTotalSpikeCount(),
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel()
    }
    experiment_results.append(episode_result)
    
    if (episode + 1) % 10 == 0:
        print(f"  Completed {episode + 1}/{experiment_config['num_episodes']} episodes")

# Analyze results
avg_final_spikes = sum(r['final_spikes'] for r in experiment_results) / len(experiment_results)
avg_curiosity = sum(r['final_curiosity'] for r in experiment_results) / len(experiment_results)

print(f"\nExperiment Results:")
print(f"Average final spikes: {avg_final_spikes:.0f}")
print(f"Average final curiosity: {avg_curiosity:.3f}")

# Export results for analysis
with open('experiment_results.json', 'w') as f:
    import json
    json.dump(experiment_results, f, indent=2)
print("Results saved to 'experiment_results.json'")
```

#### 5. Quick Testing and Debugging

```python
import pynlm

# Quick test function
print("Running quick brain test...")
test_results = pynlm.quickTest()
print(f"Test results - Neurons: {test_results[0]}, "
      f"Firing: {test_results[1]}, Spikes: {test_results[2]}")

# Example function to demonstrate advanced features
print("\nExample code:")
print(pynlm.get_example_code())
```

### Advanced Usage Patterns

#### Pattern 1: Research Experiment Setup

```python
import pynlm

def create_research_experiment(config_file=None):
    """Create a research experiment setup."""
    
    # Load configuration from file or create default
    if config_file:
        config = pynlm.createDefaultConfig()
        config.loadFromFile(config_file)
    else:
        config = pynlm.create_custom_config(
            neuron_count=5000,
            synapse_density=0.2,
            enable_stdp=True,
            enable_hebbian=True
        )
    
    # Create all components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=100, height=100, visionWidth=20, visionHeight=20)
    agent.initialize(world)
    
    return brain, agent, world, config

# Use the research setup
brain, agent, world, config = create_research_experiment()
```

#### Pattern 2: Performance Testing

```python
import pynlm
import time

# Performance testing
config = pynlm.create_custom_config(neuron_count=10000)

brain = pynlm.createBrain(config)
brain.initialize()

# Time simulation performance
start_time = time.time()
for i in range(2000):
    brain.step(i)
end_time = time.time()

elapsed = end_time - start_time
steps_per_second = 2000 / elapsed
print(f"Performance: {steps_per_second:.0f} steps/second")
print(f"Time per step: {elapsed/2000*1000:.2f} ms")
```

#### Pattern 3: Visualization and Monitoring

```python
import pynlm

# Create simulation for visualization
config = pynlm.create_custom_config(neuron_count=3000)
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=80, height=80, visionWidth=16, visionHeight=16)
agent.initialize(world)

# Run simulation and collect data for visualization
visualization_data = []
for step in range(1000):
    world.update(0.05)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Collect data for visualization
    if step % 50 == 0:
        data_point = {
            'step': step,
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'agent_x': world.getAgentBody().x,
            'agent_y': world.getAgentBody().y
        }
        visualization_data.append(data_point)

print(f"Collected {len(visualization_data)} data points for visualization")
```

## Advanced Feature Summary

### Available Functions

The enhanced NLM Python bindings provide the following functions:

| Function | Description | Example |
|----------|-------------|---------|
| `pynlm.getBrainStats(brain)` | Get comprehensive brain statistics | `stats = pynlm.getBrainStats(brain)` |
| `pynlm.runSimulationEpisode(brain, agent, world, steps)` | Run complete episode | `pynlm.runSimulationEpisode(brain, agent, world, 500)` |
| `pynlm.create_custom_config(...)` | Create custom configuration | `pynlm.create_custom_config(1000, 0.1, True)` |
| `pynlm.validateConfig(config)` | Validate configuration | `is_valid, msg = pynlm.validateConfig(config)` |
| `pynlm.export_brain_data(brain, path)` | Export brain state | `pynlm.export_brain_data(brain, "save.bin")` |
| `pynlm.import_brain_data(path)` | Import brain state | `brain = pynlm.import_brain_data("save.bin")` |
| `pynlm.run_batch_simulation(config, episodes, steps)` | Run batch experiments | `pynlm.run_batch_simulation(config, 10, 200)` |
| `pynlm.quickTest()` | Run quick test | `results = pynlm.quickTest()` |
| `pynlm.get_example_code()` | Get example code | `examples = pynlm.get_example_code()` |

### Property Access

The enhanced bindings provide convenient property-style access:

```python
# Brain statistics access
brain_stats = brain.getTotalNeuronCount()  # Total neuron count
brain_stats = brain.getTotalSynapseCount()  # Total synapse count
brain_stats = brain.getFiringNeuronCount()  # Currently firing
brain_stats = brain.getAverageFiringRate()  # Average firing rate

# Agent statistics access
agent_curiosity = agent.getCuriosityLevel()  # Curiosity level
agent_novelty = agent.getNoveltyLevel()  # Novelty level
agent_prediction = agent.getPredictionError()  # Prediction error
agent_development = agent.getDevelopmentalStage()  # Developmental stage
```

### Configuration Options

#### Custom Configuration Parameters

The `create_custom_config()` function accepts these parameters:

- `neuron_count`: Total number of neurons (default: 500)
- `synapse_density`: Synapse connectivity density (default: 0.1)
- `enable_stdp`: Enable Spike-Timing Dependent Plasticity (default: True)
- `enable_hebbian`: Enable Hebbian learning (default: True)
- `enable_curiosity`: Enable curiosity-driven exploration (default: True)

#### Validation Rules

Configuration validation checks:
- Minimum neuron count (100 neurons)
- Valid synapse density (0.01 to 1.0)
- Proper boolean values for learning flags

## Quick Reference

### Installation

```bash
# Install from source
pip install scikit-build-core pybind11
pip install -e .

# Or use pip directly
pip install .
```

### Basic Usage

```python
import pynlm

# Create and run simple simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(100):
    brain.step(i)
```

### Advanced Usage

```python
import pynlm

# Advanced simulation with all features
config = pynlm.create_custom_config(1000, 0.15, True, True, True)
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(50, 50, 10, 10)
agent.initialize(world)

# Run with learning
for step in range(500):
    world.update(0.05)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

## Troubleshooting

### Common Issues

**1. Installation Issues**
```bash
# Force reinstall if module not found
pip install --force-reinstall .
```

**2. Performance Issues**
- For large simulations, use smaller world sizes
- Monitor memory usage with `pynlm.getBrainStats(brain)`
- Consider running fewer steps in batch simulations

**3. Configuration Issues**
```python
# Validate configuration before use
is_valid, message = pynlm.validateConfig(config)
if not is_valid:
    print(f"Configuration error: {message}")
```

**4. Data Persistence Issues**
```python
# Ensure file paths are writable
import os
if os.access("data_directory", os.W_OK):
    pynlm.export_brain_data(brain, "data_directory/save.bin")
else:
    print("No write permission in data directory")
```

### Getting Help

- **Documentation**: `easy_usage.md` (beginner), `ADVANCED_PYTHON_GUIDE.md` (advanced)
- **Examples**: Available in the `examples/` directory
- **API Reference**: See `docs/` directory for detailed API documentation
- **Community**: Join the NLM community for support and discussion

## Additional Resources

### Related Documentation

- **Architecture**: `docs/ARCHITECTURE.md` - Detailed system architecture
- **Science**: `docs/SCIENCE.md` - Scientific background and theory
- **Experiments**: `docs/EXPERIMENTS.md` - Experiment descriptions and protocols
- **Phase 6 Audit**: `docs/PHASE6_FINAL_AUDIT.md` - Final integration audit report

### Example Projects

The repository includes several example projects:

1. **Phase 3 Demo**: Basic neural learning demonstration
2. **Phase 4 Demo**: Advanced learning with curiosity
3. **Phase 5 Experiment**: Performance optimization experiments
4. **Phase 6 Integration**: Complete system integration

### Research Resources

NLM is a research project investigating computational brain-like systems. For research use:

1. **Citation**: Reference NLM in publications
2. **Acknowledgements**: Acknowledge NLM project funding
3. **Bug Reports**: Report issues on GitHub
4. **Contributions**: Contribute improvements and extensions

## License

MIT License. See `LICENSE` file for details.

---

This comprehensive guide covers all aspects of using NLM, from basic setup to advanced research applications. For the simplest usage, start with `easy_usage.md`. For advanced features and research applications, use this guide and `ADVANCED_PYTHON_GUIDE.md`.
