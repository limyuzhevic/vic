# NLM (熙然) — Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Quick Start

### Installation

NLM comes with two main components:
1. **C++ Core**: The main neural simulation framework
2. **Python Bindings**: Python interface for easier use

To build and install:

```bash
# Build C++ core
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build Python bindings (if using Python)
cd python
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install .
```

### Basic Usage

#### Using Python API (Recommended for most users)

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.create_brain()
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step, step * 0.001)

# Get statistics
stats = brain.get_stats()
print(f"Neurons: {stats['neuron_count']}")
print(f"Total spikes: {stats['total_spikes']}")
```

#### Using C++ API (For advanced users)

```bash
# Run demo executable
./nlm --mode demo --steps 2000

# Run experiment mode
./nlm --mode experiment --set neuron_count=1000 --set connection_probability=0.2

# Run with custom configuration
./nlm --config my_config.cfg --save-brain brain_state.bin
```

### Development Environment Setup

```bash
# Install development dependencies
pip install -e .[dev]

# Run tests
pytest tests/

# Build with all optimizations
cmake -DCMAKE_BUILD_TYPE=Release .. && make -j$(nproc)
```

## Features

### Core Brain Systems

1. **Neuromodulation System**
   - Dopamine for reward prediction
   - Curiosity for exploration
   - Novelty detection for surprise
   - Prediction error for learning

2. **Memory Systems**
   - Working memory with persistent activity
   - Episodic memory with experience encoding and replay
   - Associative memory with Hebbian pattern associations
   - All memory systems connected to neural processing

3. **Prediction System**
   - Integrated into brain loop
   - Prediction error signals affect learning
   - Confidence tracking implemented

4. **Cognition Systems**
   - Neural planner with action sequence evaluation
   - Concept formation from experience patterns
   - Attention with competitive selection dynamics
   - All cognition systems connected to perception and action

5. **Development Integration**
   - Developmental stages affect plasticity rates
   - Structural plasticity modulated by age
   - Neural excitability changes with development

### Advanced Capabilities

- **Checkpoint Save/Load**: Can resume from saved checkpoints
- **Memory Replay**: Episodic memory replay during simulation
- **Memory Consolidation**: Integration with sleep/rest cycle
- **World Interaction**: Simple world simulation with agent control
- **Sensory Processing**: Vision, audio, and internal signal processing
- **Action Selection**: Motor system with multiple action types

## Architecture

NLM operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## API Reference

### Python API

#### Core Classes

- **`pynlm.Config()`**: Configuration wrapper for brain parameters
- **`pynlm.create_brain(config=None)`**: Create a brain instance
- **`pynlm.create_simple_world()`**: Create simulation world
- **`pynlm.create_agent_brain(brain)`**: Create agent interface

#### Configuration Options

Common configuration keys:
- `neuron_count`: Number of neurons (default: 1000)
- `region_count`: Number of brain regions (default: 2)
- `connection_probability`: Random connection probability (default: 0.1)
- `simulation_timestep`: Time step duration (default: 0.001)
- `random_seed`: Random number generator seed (default: 42)

#### Usage Examples

**Basic Simulation**:
```python
import pynlm

# Create brain with custom config
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)

# Initialize and run
brain.initialize()
for i in range(100):
    brain.step(i, i * 0.001)

# Get statistics
stats = brain.get_stats()
print(f"Simulation complete: {stats['total_spikes']} spikes")
```

**World Interaction**:
```python
import pynlm

# Create all components
brain = pynlm.create_brain()
world = pynlm.create_simple_world()
agent_brain = pynlm.create_agent_brain(brain)

# Initialize interaction
agent_brain.initialize(world)
world.set_agent_start(10.0, 10.0)

# Run episode
for step in range(500):
    # Get sensory input
    percept = world.get_sensory_percept()
    
    # Process in agent brain
    agent_brain.process_sensory_input(percept)
    
    # Produce action
    action = agent_brain.decode_motor_command()
    
    # Apply to world
    result = world.apply_motor_command(action, world.get_simulation_time())
    
    # Apply learning
    agent_brain.apply_reward_modulation(result.reward)
    
    # Update world
    world.update(0.01)
    
    # Update development
    agent_brain.update_development(0.01)
```

**Memory and Learning**:
```python
import pynlm

# Create brain with learning enabled
config = pynlm.get_example_config()
brain = pynlm.create_brain(config)
agent_brain = pynlm.create_agent_brain(brain)

# Enable learning systems
agent_brain.enable_reward_modulation(True)
agent_brain.enable_structural_plasticity(True)
agent_brain.enable_development(True)
agent_brain.enable_curiosity(True)

# Run learning episode
for step in range(1000):
    brain.step(step, step * 0.001)
    
    # Check for exploration
    curiosity = agent_brain.get_curiosity_level()
    if curiosity > 0.5:
        print(f"Step {step}: High curiosity ({curiosity:.2f})")
```

### C++ API

#### Key Classes

- **`Config`**: Configuration management
- **`Brain`**: Main neural simulation engine
- **`AgentBrain`**: Agent-world interface
- **`SimpleWorld`**: Simulation environment
- **`SensoryInput`**: Sensory modality base class
- **`Vision`**: Visual input (inherits from SensoryInput)
- **`Audio`**: Audio input (inherits from SensoryInput)
- **`InternalSignals`**: Internal state signals
- **`Action`**: Motor action representation

#### Command Line Interface

The C++ executable supports advanced configuration:

```bash
# Basic usage
./nlm --mode demo --steps 2000

# Custom configuration
./nlm --mode experiment \
     --set neuron_count=1000 \
     --set connection_probability=0.2 \
     --set simulation_timestep=0.0005

# Save/load brain state
./nlm --mode demo --save-brain brain1.bin
./nlm --mode demo --load-brain brain1.bin

# Memory replay
./nlm --mode demo --enable-replay --replay-steps 500

# Development
./nlm --mode demo --enable-development --enable-plasticity
```

## Performance

### Benchmark Results

NLM is optimized for performance:

- **Neuron Count**: Up to 10,000 neurons on modern hardware
- **Memory Efficiency**: <100MB for basic configurations
- **Simulation Speed**: ~1M steps per second per CPU core
- **Parallel Processing**: SIMD vectorization and multi-threading support

### Hardware Requirements

Minimum requirements:
- 4 CPU cores
- 8GB RAM
- 64GB disk space

Recommended for research:
- 8+ CPU cores
- 16GB+ RAM
- Additional storage for simulations

## Research Applications

### Use Cases

1. **Neural Development Research**: Study how brains develop from primitive states
2. **Learning Mechanisms**: Investigate different plasticity rules and learning algorithms
3. **Memory Systems**: Research memory encoding, storage, and retrieval
4. **Neuromodulation**: Study reward, curiosity, and motivation systems
5. **Cognitive Architecture**: Test hypothesis about cognitive processes
6. **Embodied Intelligence**: Explore brain-world interaction
7. **Decision Making**: Analyze action selection and planning

### Scientific Validation

NLM provides measurable scientific outputs:

- **Synaptic plasticity**: Weight changes through Hebbian and STDP
- **Learning curves**: Reward prediction and adaptation
- **Memory metrics**: Recall accuracy and consolidation
- **Behavioral patterns**: Action selection statistics
- **Developmental trajectories**: Age-dependent changes
- **Network dynamics**: Spike statistics and connectivity

## Development

### Project Structure

```
NLM/
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components
│   ├── memory/        # Memory systems
│   ├── neuromodulation/ # Neuromodulators
│   ├── prediction/     # Prediction systems
│   ├── cognition/     # Cognitive mechanisms
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── agent/         # Agent system
│   ├── world/         # World system
│   ├── experiments/   # Experiment framework
│   └── visualization/ # Visualization
├── python/             # Python bindings
├── configs/           # Configuration files
├── docs/              # Documentation
├── tests/             # Test suite
├── CMakeLists.txt     # Main build system
└── pyproject.toml    # Python package configuration
```

### Contributing

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests for new functionality
5. Submit a pull request

### Testing

Run the test suite:

```bash
# Run all tests
pytest tests/

# Run C++ tests
mkdir build && cd build
cmake ..
make test

# Run specific test module
pytest tests/test_brain.py
```

### Building from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python bindings
pip install python/
```

## Troubleshooting

### Common Issues

**Issue: Build fails due to missing dependencies**

```bash
# Install build dependencies
apt-get update
apt-get install -y cmake build-essential libpython3-dev python3-dev
pip install pybind11 numpy
```

**Issue: Python bindings don't import**

```bash
# Check installation
pip list | grep pynlm

# Reinstall if needed
pip install -e python/
```

**Issue: Memory usage too high**

```bash
# Reduce neuron count in configuration
./nlm --set neuron_count=100
```

**Issue: Simulation too slow**

```bash
# Optimize compilation
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native"
```

### Getting Help

- **Documentation**: https://nlm.readthedocs.io
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions

## License

MIT License. See LICENSE file for details.

## Copyright

Copyright (c) 2024 NLM Authors

All rights reserved.

---

*For more detailed documentation, API references, and scientific background, please visit the official NLM documentation.*
