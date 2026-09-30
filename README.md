# NLM — 熙然

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

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

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

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── python/                         # Python bindings and API
│   ├── bindings.cpp               # Enhanced Python bindings
│   └── README.md                  # Python API documentation
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
├── nlm/                          # Python module structure
│   ├── __init__.py
│   ├── pynlm.py                # Generated Python bindings
│   ├── advanced.py             # Advanced features
│   ├── profiling.py            # Performance tools
│   └── experiments.py          # Experiment management
├── docs/                         # Documentation
│   ├── ADVANCED_GUIDE.md
│   ├── PYTHON_API_REFERENCE.md
│   ├── EXAMPLES.md
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md
└── configs/
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── python/                         # Python bindings and API
│   ├── bindings.cpp               # Enhanced Python bindings
│   └── README.md                  # Python API documentation
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
├── nlm/                          # Python module structure
│   ├── __init__.py
│   ├── pynlm.py                # Generated Python bindings
│   ├── advanced.py             # Advanced features
│   ├── profiling.py            # Performance tools
│   └── experiments.py          # Experiment management
├── docs/                         # Documentation
│   ├── ADVANCED_GUIDE.md
│   ├── PYTHON_API_REFERENCE.md
│   ├── EXAMPLES.md
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md
└── configs/
```

## Python API Overview

The NLM project provides a comprehensive Python API for neural simulation research and experimentation. The Python bindings are built using pybind11 and provide access to the full NLM neural simulation framework.

### Key Features

#### Enhanced Pythonic Interface

- **Pythonic wrapper classes** (`EnhancedBrain`, `World`, `Agent`, `MemoryManager`)
- **Configuration management** with JSON/YAML support
- **Context manager support** for automatic resource cleanup
- **Property-based access** for intuitive API design

#### Advanced Features

- **Parameter sweep experiments** for systematic parameter testing
- **Multi-environment testing** for robust evaluation across conditions
- **Agent research** for custom agent development
- **Performance monitoring** with real-time metrics
- **Experiment management** for reproducible research
- **Memory profiling** for resource optimization
- **Batch processing** for large-scale simulations

#### Performance Tools

- **Memory monitoring** for resource tracking
- **CPU monitoring** for system performance analysis

## Python Module Structure

The Python module is organized into several key components:

### Core Module (`nlm/`)

- **`pynlm`**: Generated Python bindings for the core NLM functionality
- **`advanced`**: Advanced features for experimentation and research
- **`profiling`**: Performance monitoring and optimization tools
- **`experiments`**: Experiment management and analysis framework

### Documentation

The Python API is fully documented with:

- **`docs/ADVANCED_GUIDE.md`**: Comprehensive guide for advanced users
- **`docs/PYTHON_API_REFERENCE.md`**: Complete API reference documentation
- **`docs/EXAMPLES.md`**: Practical examples and tutorials

## Quick Start

### Basic Usage

```python
from nlm import EnhancedBrain, World, Action

# Create enhanced brain
brain = EnhancedBrain()

# Create world
world = World()
world.configure(100, 100, 10, 10)
world.setAgentStart(50, 50)

# Initialize brain
brain.initialize()

# Run simulation
for step in range(100):
    world.step(0.01)
```

### Advanced Features

```python
from nlm import AdvancedExperiment, PerformanceProfiler

# Create advanced experiment
experiment = AdvancedExperiment("my_experiment")

# Run parameter sweep
param_grid = {'width': [50, 100, 200], 'height': [50, 100, 200]}
results = experiment.run_parameter_sweep(param_grid, brain_factory)

# Create performance profiler
profiler = PerformanceProfiler()
profiler.start_profiling()

# Run simulation with profiling
result = profiler.benchmark_simulation(brain, world, num_steps=100)

profiler.stop_profiling()
```

## Building Python Bindings

The Python bindings are built using the existing C++ project. To use the Python API:

1. Build the C++ project with Python bindings enabled
2. The Python package will be automatically generated
3. Use `pip install .` to install the package

For detailed build instructions, see `HOW_TO_USE.md`.

## Examples and Tutorials

### Basic Examples

See `docs/EXAMPLES.md` for:
- Hello World example
- Creating and initializing brains
- Basic simulation loops
- Configuration management

### Advanced Examples

See `docs/EXAMPLES.md` for:
- Parameter sweep experiments
- Multi-environment testing
- Agent research
- Performance benchmarking
- Custom neuron populations

### Research Examples

See `docs/EXAMPLES.md` for:
- Memory system integration
- Neuromodulation research
- Developmental studies
- Social learning

## API Reference

See `docs/PYTHON_API_REFERENCE.md` for:
- Complete class and function documentation
- Type definitions and parameters
- Return value descriptions
- Usage examples for each component

## Advanced Guide

See `docs/ADVANCED_GUIDE.md` for:
- Pythonic wrapper class design
- Configuration system with JSON/YAML support
- Performance monitoring tools
- Experiment management framework
- Custom neuron population creation
- Batch processing utilities

## Migration Guide

The enhanced Python API maintains backward compatibility while providing new features. See `docs/PYTHON_API_REFERENCE.md` for migration information from the original API to the enhanced API.

## License

MIT

## Authors

Research project — See docs for scientific background.
