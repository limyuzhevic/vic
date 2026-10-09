# NLM — 熙然

**An Experimental Artificial Developmental Brain**

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## What is NLM?

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. All major components are now connected and working together:

- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Prediction System**: Temporal sequence learning
- **Cognition Systems**: Attention, planning, concept formation
- **Development Integration**: Stage-dependent plasticity
- **Persistence**: Checkpoint save/load capability
- **Replay and Consolidation**: Memory reinforcement

## Quick Start

### Building
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Python Usage
```python
import pynlm

# Create a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
```

### Documentation
For comprehensive documentation, including API references, tutorials, and examples, visit our documentation site:
- **[Online Documentation](https://nlm.readthedocs.io)** - Complete API reference and guides
- **[GitHub Repository](https://github.com/nlm-project/nlm)** - Source code and issues

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── NLMDocumentation/          # Documentation (new)
│   ├── docs/                 # Markdown documentation
│   ├── examples/             # Code examples
│   └── api-reference/        # Auto-generated API docs
├── docs/                     # Existing documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   └── PHASE6_FINAL_AUDIT.md
├── python/                   # Python bindings
│   ├── bindings.cpp
│   └── pyproject.toml
├── src/                      # Source code
│   ├── core/                # Core utilities
│   ├── brain/               # Neural components
│   ├── memory/              # Memory systems
│   ├── plasticity/          # Plasticity rules
│   └── ... (many more)      # Additional modules
├── tests/                   # Unit tests
├── configs/                 # Configuration files
└── examples/                # Example scripts
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton and core infrastructure

### Phase 2 (Complete)
- LIF neuron dynamics and event-driven processing
- STDP and Hebbian plasticity

### Phase 3 (Complete)
- World interaction loop with reward prediction
- Developmental stages and curiosity

### Phase 4 (Complete)
- Prediction systems and cognitive capabilities
- Working memory, episodic memory, concepts

### Phase 5 (Complete)
- Performance optimizations and scaling
- Memory pools, SIMD, parallel processing

### Phase 6 (Complete - Final Integration)
- **All systems integrated and functional**
- **Memory, neuromodulation, prediction working together**

## Building the Project

```bash
# Standard build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Build with tests
mkdir build
cd build
cmake .. -DBUILD_TESTS=ON
make -j$(nproc)

# Run tests (after building)
ctest --output-on-failure
```

## Running Demonstrations

### Phase 6 Integration Test
```bash
./build/nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Phase 4 Cognitive Demo
```bash
./build/nlm_phase4_demo
```

### Performance Benchmark
```bash
./build/nlm --benchmark --neurons=10000 --steps=10000
```

## Python Package

The project includes Python bindings via `pybind11`:

### Installation
```bash
# Install from source (recommended)
pip install -e .

# Or install from PyPI (if available)
pip install pynlm
```

### Quick Usage
```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

## Documentation

A comprehensive documentation system has been created to replace and extend the existing documentation:

- **API References**: Complete C++ and Python API documentation
- **Tutorials**: Step-by-step guides for beginners
- **Configuration Guide**: Detailed parameter descriptions
- **Architecture Documentation**: System design and components
- **Performance Guide**: Optimization and scaling
- **Troubleshooting**: Common issues and solutions

### Key Documentation Categories

1. **Getting Started** - Installation and basic usage
2. **Python API Reference** - Complete Python bindings documentation
3. **Configuration Guide** - All configuration parameters
4. **Tutorial Series** - Progressive learning path
5. **Architecture Documentation** - System design overview
6. **Performance Guide** - Optimization and tuning
7. **Examples** - Working code examples
8. **Troubleshooting** - Common problems and solutions

## Scientific Background

NLM is inspired by biological brain principles but makes no claims of accurately reproducing biological brains. The project investigates whether cognition can emerge from spiking dynamics and neural computation without predefined knowledge.

See `docs/SCIENCE.md` for detailed scientific background and limitations.

## License

MIT

## Authors

Research project — See docs for scientific background.

## Documentation Navigation

- **New Users**: Start with `NLMDocumentation/docs/getting-started/quick-start.md`
- **Python Developers**: Use `NLMDocumentation/docs/python-api/index.md`
- **Advanced Users**: Explore `NLMDocumentation/docs/architecture/` and `NLMDocumentation/docs/performance/`
- **Configuration**: See `NLMDocumentation/docs/configuration/index.md`
- **Examples**: Check `NLMDocumentation/docs/examples/` directory
