# NLM - How to Use Guide

This guide provides comprehensive instructions for using the NLM (Neural Learning Machine) neural simulation framework.

## Quick Start

### Basic Usage

```bash
# Run the default brain simulation
./nlm

# Run with custom configuration
./nlm --config=my_config.cfg --neurons=500 --steps=1000

# Load and save brain state
./nlm --load=saved_brain.bin --output=trained_brain.bin --steps=2000
```

## Installation

### Building from Source

1. Clone the repository
2. Create a build directory
3. Configure with CMake
4. Build with make

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Package Installation

The NLM Python bindings can be installed via pip:

```bash
pip install pynlm
```

## Command Line Options

### Basic Options

- `--help, -h` - Show help message
- `--version` - Show version information
- `--config=FILE` - Load configuration from file
- `--output=FILE` - Save brain state to file
- `--load=FILE` - Load brain state from file

### Simulation Control

- `--neurons=NUMBER` - Set number of neurons (default: 1000)
- `--steps=NUMBER` - Number of simulation steps (default: 10000)
- `--regions=NUMBER` - Number of brain regions (default: 2)
- `--timestep=VALUE` - Simulation timestep in seconds (default: 0.001)
- `--connection-prob=VALUE` - Connection probability (0.0-1.0, default: 0.1)

### Feature Controls

- `--enable-plasticity` - Enable synaptic plasticity
- `--disable-plasticity` - Disable synaptic plasticity
- `--enable-development` - Enable developmental processes
- `--disable-development` - Disable developmental processes
- `--enable-curiosity` - Enable curiosity-driven exploration
- `--disable-curiosity` - Disable curiosity-driven exploration
- `--enable-reward` - Enable reward-based learning
- `--disable-reward` - Disable reward-based learning
- `--enable-visualization` - Enable visualization
- `--disable-visualization` - Disable visualization

### Advanced Options

- `--checkpoint-interval=NUMBER` - Checkpoint save interval
- `--replay-interval=NUMBER` - Memory replay interval
- `--memory-capacity=NUMBER` - Working memory capacity
- `--max-episodes=NUMBER` - Maximum episodic memory episodes
- `--stdp-ltp-weight=VALUE` - STDP LTP learning rate
- `--stdp-ltd-weight=VALUE` - STDP LTD learning rate
- `--stdp-time-constant=VALUE` - STDP time constant
- `--synaptogenesis-rate=VALUE` - Synaptogenesis rate
- `--pruning-rate=VALUE` - Pruning rate

### Experiment Types

- `--experiment=basic` - Run basic neural connectivity tests
- `--experiment=learning` - Run plasticity learning experiment
- `--experiment=plasticity` - Run STDP verification experiment
- `--experiment=full` - Run comprehensive integration test

### Logging

- `--verbose, -v` - Enable verbose logging
- `--quiet` - Suppress most output
- `--debug` - Enable debug logging

## Configuration Files

### Default Configuration

The default configuration is located at `configs/default.cfg`:

```ini
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
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1
curiosity_baseline = 0.0

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
```

### Custom Configuration

You can create custom configuration files by copying the default and modifying values:

```bash
cp configs/default.cfg my_config.cfg
# Edit my_config.cfg with your desired settings
```

## Python API

The NLM framework includes Python bindings for easier access to the C++ functionality.

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
stats = brain.getStats()
print(f"Total spikes: {stats['total_spikes']}")
print(f"Active neurons: {stats['active_neurons']}")

# Save brain state
brain.save("my_brain.bin")
```

### Advanced Python API

```python
import pynlm
from pynlm import brain, world, agent

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.neuron_count = 500
config.enable_structural_plasticity = True
config.enable_curiosity = True

brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run agent in world
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Available Modules

- `pynlm.brain` - Core neural simulation
- `pynlm.world` - Environment simulation
- `pynlm.agent` - Agent interface
- `pynlm.experiments` - Predefined scenarios
- `pynlm.config` - Configuration management
- `pynlm.utils` - Utility functions

## Advanced Features

### Running Experiments

You can run different types of experiments using command-line options:

```bash
# Basic connectivity test
./nlm --experiment=basic --neurons=200

# Plasticity learning experiment
./nlm --experiment=learning --neurons=300 --steps=1000

# STDP verification experiment
./nlm --experiment=plasticity --neurons=100

# Full integration test
./nlm --experiment=full --neurons=500 --steps=2000
```

### Memory Management

The NLM framework includes advanced memory systems:

- **Working Memory**: Short-term storage of active information
- **Episodic Memory**: Storage of experiences and episodes
- **Associative Memory**: Pattern associations and recall

### Neuromodulation

The brain includes several neuromodulation systems:

- **Dopamine**: Reward and reinforcement learning
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **Prediction Error**: Learning from prediction errors

### Development

The brain undergoes developmental processes:

- **Structural Plasticity**: Synaptogenesis and pruning
- **Functional Maturation**: Changes in neural properties
- **Critical Periods**: Sensitive periods for learning

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that neurons are receiving input

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Ensure the agent has motor neurons

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check neuron parameters and connectivity

### Debug Mode

Enable debug mode for detailed information:

```bash
./nlm --debug --verbose
```

### Performance Issues

For better performance:
- Increase simulation timestep (but may affect stability)
- Reduce number of neurons
- Disable unnecessary features
- Use more cores (if available)

## Examples and Tutorials

### Example 1: Simple Brain Simulation

```bash
# Create a simple configuration
cat > simple_config.cfg << EOF
random_seed = 42
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
connection_probability = 0.15
EOF

# Run with custom configuration
./nlm --config=simple_config.cfg --steps=500
```

### Example 2: Learning Scenario

```bash
# Run a learning scenario
./nlm --experiment=learning --neurons=200 --steps=1000 \
     --enable-plasticity --enable-curiosity --verbose
```

### Example 3: Brain Training and Saving

```bash
# Train a brain and save the state
./nlm --neurons=300 --steps=2000 --output=trained_brain.bin \
     --enable-development --enable-reward --verbose

# Later, load and continue training
./nlm --load=trained_brain.bin --steps=1000 --output=more_trained_brain.bin
```

## Research and Development

### Modifying the Framework

For research purposes, you can modify the source code:

1. **Core Neural Components**: Modify `src/brain/` files
2. **Memory Systems**: Modify `src/memory/` files
3. **Plasticity Rules**: Modify `src/plasticity/` files
4. **Neuromodulation**: Modify `src/neuromodulation/` files

### Extending the Python API

Add new Python functionality by modifying `python/bindings.cpp`:

```cpp
// Example: Add a new Python class
py::class_<MyNewClass>(m, "MyNewClass")
    .def(py::init<>())
    .def("method", &MyNewClass::method);
```

### Running Phase 6 Integration Test

To verify that all systems are properly integrated:

```bash
./nlm_phase6_demo
```

This runs the Phase 6 integration test, which verifies:
- Memory systems integration
- Neuromodulation integration
- Development integration
- Prediction system integration
- Checkpoint functionality
- Replay functionality

## Further Reading

### Documentation

- `docs/ARCHITECTURE.md` - System architecture overview
- `docs/SCIENCE.md` - Scientific background
- `docs/ROADMAP.md` - Development roadmap
- `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 audit report

### Research Papers

The NLM framework is based on several research principles:
- Real spiking neural computation (LIF neurons)
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)
- Memory systems integration

## Conclusion

NLM is a comprehensive neural simulation framework that provides:

- **Real neural computation**: LIF neurons with event-driven dynamics
- **Integrated memory systems**: Working, episodic, and associative memory
- **Advanced learning**: STDP, Hebbian, and reward-based learning
- **Neuromodulation**: Curiosity, dopamine, and novelty systems
- **Development**: Structural and functional maturation
- **Flexible configuration**: Extensive command-line and file-based options
- **Python accessibility**: High-level Python API for easier use

This guide covers the main features and usage patterns. For more advanced topics, refer to the technical documentation and source code comments.

## Version Information

**NLM Phase 6.0**
- Version: 0.1.0
- Phase: Final Integration
- Build: C++20 with pybind11 Python bindings
- License: MIT

For issues and contributions, please refer to the project repository.
