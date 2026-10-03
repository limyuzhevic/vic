# NLM (Neural Learning Machine)

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://travis-ci.org/nlm-project/nlm)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

NLM (熙然, meaning "serene flow") is an experimental computational brain project that creates virtual brains with neurons that can learn, remember, and make decisions. The system implements a brain-inspired architecture with real spiking neural computation.

## Overview

NLM is designed to simulate brain-like systems with:

- **Real spiking neurons**: Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
- **Multiple plasticity mechanisms**: STDP, Hebbian learning, and structural plasticity
- **Integrated memory systems**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error
- **Cognition systems**: Planning, attention, concept formation
- **Developmental stages**: From high plasticity to stable adult brain

## Phase 6: Final Integration

The current phase (Phase 6) focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Quick Start

### Prerequisites

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .
```

### Basic Usage

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Building

### C++ Components

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Package

```bash
pip install .
# or for development
pip install -e .
```

## Project Structure

```
NLM/
├── src/
│   ├── brain/              # Neural components
│   ├── dynamics/           # Neural dynamics
│   ├── plasticity/         # Plasticity rules
│   ├── development/        # Developmental system
│   ├── neuromodulation/    # Neuromodulators
│   ├── memory/            # Memory systems
│   ├── prediction/         # Prediction systems
│   ├── cognition/          # Cognitive mechanisms
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Motor system
│   ├── environment/       # Environment interface
│   ├── experiments/       # Experiment framework
│   └── visualization/     # Visualization
├── python/                 # Python bindings
│   ├── bindings.cpp       # PyBind11 bindings
│   └── CMakeLists.txt     # Python bindings build
├── tests/                 # C++ tests
├── docs/                  # Documentation
│   ├── ARCHITECTURE.md    # System architecture
│   ├── SCIENCE.md        # Scientific background
│   ├── ROADMAP.md        # Development roadmap
│   └── ...                # More docs
├── easy_usage.md          # Beginner guide
├── HOW_TO_USE.md          # Detailed usage guide
├── nlm_cli.py             # Command-line interface
├── nlm_benchmark.py       # Performance benchmarking
├── NLM_Documentation.md    # Comprehensive documentation
├── README.md              # This file
├── pyproject.toml         # Python package configuration
├── CMakeLists.txt         # C++ build configuration
└── LICENSE                # MIT License
```

## Python API

### Core Classes

- **Brain**: Central neural simulation brain class
- **AgentBrain**: Interface connecting NLM brain to world
- **SimpleWorld**: Simple 2D world for NLM simulation
- **Config**: Configuration class for NLM system
- **Action**: Action representation for motor output
- **WorldObject**: World object representation
- **AgentBody**: Agent body state
- **SensoryPercept**: Sensory percept data

### Key Features

#### 1. Configuration Management

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.saveToFile("config.json")
```

#### 2. Memory System Access

```python
brain = pynlm.createBrain(config)
brain.initialize()

working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory capacity: {working_memory.getCapacity()}")

episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"Episodic memory episodes: {episodic_memory.getEpisodeCount()}")
```

#### 3. Neuromodulation and Learning

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Apply reward-based learning
reward = 0.1 if task_completed else -0.1
agent.applyRewardModulation(reward, 0.0)
```

#### 4. Development Stages

```python
# Development affects learning and behavior
development_stage = brain.getDevelopmentalStage()
print(f"Developmental stage: {development_stage}")

# Different stages have different plasticity rates
agent.updateDevelopment(0.1)
```

### Error Handling

```python
import pynlm
from pynlm import NLMError

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except NLMError as e:
    print(f"NLM error: {e}")
```

## Command Line Interface

The NLM project includes a command-line interface for basic operations:

### Installation

```bash
chmod +x nlm_cli.py
python nlm_cli.py --help
```

### Basic Commands

```bash
# Create and run a brain
nlm create --neurons 1000 --steps 100

# Save brain state
nlm save brain_state.bin

# Load brain state
nlm load brain_state.bin

# Inspect brain
nlm inspect
```

### Advanced Features

```bash
# Run simulation with verbose output
nlm run --steps 1000 --verbose 100

# Create brain with custom parameters
nlm create --neurons 5000 --regions 4 --variance 0.15
```

## Benchmarking and Performance

The NLM project includes benchmarking tools for performance analysis:

### Quick Benchmark

```bash
python nlm_benchmark.py
```

### Advanced Benchmarking

```python
import pynlm
from nlm_benchmark import NLMBenchmark

benchmark = NLMBenchmark()
comparison = benchmark.compare_configurations([
    {'neurons': 1000, 'steps': 1000, 'description': 'Standard'},
    {'neurons': 5000, 'steps': 1000, 'description': 'Large'},
])
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.region_count` | int | 1 | Number of brain regions |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Connection probability |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |

## Development

### Testing

```bash
# Run C++ tests
cd build
ctest --output-on-failure

# Run Python tests
pytest tests/
```

### Code Style

- C++: Use C++20 standards with clang-format
- Python: PEP 8 compliant
- Documentation: Markdown format

### Adding Features

1. Add new C++ components to `src/` directory
2. Implement Python bindings in `python/bindings.cpp`
3. Update documentation in `docs/`
4. Add tests to `tests/` directory
5. Update `easy_usage.md` and `HOW_TO_USE.md`

## Contributing

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests for new functionality
5. Update documentation
6. Submit a pull request

## License

MIT License. See LICENSE file for details.

## Authors

Research project — See docs for scientific background.
