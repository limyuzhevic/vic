# NLM (Neural Learning Machine) Documentation

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. It implements a brain-inspired spiking neural network simulator with multiple cognitive systems including memory, neuromodulation, prediction, and development.

**Phase:** 6 - Final Integration
**Status:** Mostly implemented but requires integration of disconnected systems

---

## Documentation Structure

This documentation covers:

- **Core Documentation** (Phase 1): API references, configuration guides, and basic usage
- **Advanced Documentation** (Phase 2): Developer guides, architecture, and extension patterns
- **Interactive Examples** (Phase 3): Tutorials and code samples
- **Automation** (Phase 4): Documentation generation and maintenance

---

## Quick Navigation

### 📚 Core Documentation

#### Getting Started
- [Installation Guide](guides/installation.md) - Build and setup instructions
- [Getting Started Guide](guides/getting_started.md) - Beginner tutorial
- [User Guide](guides/user_guide.md) - Basic usage patterns

#### API References
- [C++ API](api/cpp/brain.md) - Core brain class reference
- [Python API](api/python/core.md) - Python bindings documentation

#### Configuration
- [Configuration Overview](configuration/config_overview.md) - Configuration system
- [Configuration Reference](configuration/config_reference.md) - All options with descriptions
- [Configuration Examples](configuration/config_examples.md) - Practical examples

### 🔧 Developer Documentation

#### System Architecture
- [System Architecture](architecture/system_architecture.md) - Overview of brain components
- [Component Diagrams](architecture/component_diagrams.md) - Detailed component interactions
- [Data Flow Documentation](architecture/data_flow.md) - Information flow between components

#### Extension and Development
- [Developer Guide](guides/developer_guide.md) - Development standards and practices
- [Extension Guide](guides/extension_guide.md) - How to extend the system
- [Performance Guide](guides/performance_guide.md) - Optimization techniques

### 🧠 Scientific Documentation

#### Cognitive Systems
- [Scientific Background](research/background.md) - Biological inspiration and theory
- [Research Papers](research/papers.md) - Related literature
- [Current Limitations](research/limitations.md) - Known limitations and open questions

#### System Components
- [Memory Systems](architecture/memory_overview.md) - Working memory, episodic memory, etc.
- [Neuromodulation](architecture/neuromodulation_overview.md) - Dopamine, curiosity, novelty
- [Prediction System](architecture/prediction_overview.md) - Temporal sequence learning
- [Cognitive Layer](architecture/cognition_overview.md) - Attention, planning, self-model

### 📝 Examples and Tutorials

#### Beginner Tutorials
- [Basic Brain Tutorial](examples/tutorials/basic_brain.md) - Your first NLM brain
- [Working Memory Tutorial](examples/tutorials/working_memory.md) - Temporary storage
- [Neuromodulation Tutorial](examples/tutorials/neuromodulation.md) - Reward and curiosity

#### Intermediate Examples
- [Hello World](examples/hello_world.md) - Minimal working example
- [Agent Simulation](examples/agent_simulation.md) - Complete brain-world interaction
- [Learning Example](examples/learning_example.md) - How to enable learning

#### Advanced Examples
- [Visualization Example](examples/visualization_example.md) - Brain visualization
- [Experiment Examples](examples/experiments/) - Research experiment templates

---

## API Documentation

### C++ API Reference

The C++ API is implemented in the `src/` directory with the following main components:

- **Brain Class**: Core neural processing engine (src/brain/Brain.hpp)
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
- **Plasticity**: STDP, Hebbian learning, structural plasticity
- **Development**: Synaptogenesis, pruning, maturation

### Python API Reference

The Python bindings are accessible via the `pynlm` module:

```python
import pynlm

# Create a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

---

## Configuration System

NLM uses a hierarchical configuration system for:

- **Brain parameters**: Neuron count, synaptic density, plasticity rates
- **Memory settings**: Storage capacity, decay rates, consolidation
- **Neuromodulation**: Reward scaling, novelty detection, curiosity thresholds
- **Development**: Age-based plasticity modifiers, stage transitions
- **Performance**: Time stepping, memory pools, parallel processing

Configuration can be loaded from JSON files or created programmatically:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Modify specific parameters
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 0.5)

# Save configuration
config.saveToFile("my_config.json")
```

---

## Installation and Setup

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **Build Tools**: Make or Ninja

### Build Instructions

```bash
# Clone the repository
cd /path/to/nlm

# Create build directory and configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Install (optional)
make install
```

### Python Installation

```bash
# Install from source
pip install -e .

# Or install with scikit-build-core
pip install scikit-build-core pybind11
pip install .
```

---

## Project Status and Roadmap

### Current Phase (Phase 6)

**Status:** Final Integration
**Focus:** Connect existing systems into coherent brain

Key achievements:
- All cognitive systems implemented
- Basic sensory-motor loop working
- Memory systems designed but disconnected
- Neuromodulation partially integrated

### Phase 6 Goals (Final Integration)

1. **Connect Memory Systems**:
   - Integrate WorkingMemory with sensory processing
   - Connect EpisodicMemory for experience storage
   - Implement replay and consolidation mechanisms

2. **Connect Cognitive Systems**:
   - Integrate NeuralPlanner with action selection
   - Connect ConceptFormation for pattern discovery
   - Implement SelfModel for sensorimotor learning

3. **Enhance Neuromodulation**:
   - Implement full dopamine effects
   - Connect ACh, NE, 5-HT to attention
   - Integrate neuromodulation with memory

4. **Implement Persistence**:
   - Connect CheckpointSystem with Brain save/load
   - Enable life history tracking
   - Implement sleep/rest cycles

### Future Research Directions

- Enhanced biological fidelity
- Multi-agent social learning
- Continual learning without catastrophic forgetting
- Generalization across tasks
- Understanding memory consolidation mechanisms

---

## Contact and Community

### Getting Help

- **Documentation**: This documentation site
- **Code Issues**: GitHub repository issues
- **Technical Questions**: Project mailing list
- **Bug Reports**: GitHub issue tracker

### Contributing

See [Contribute](contribute.md) for detailed contribution guidelines.

### License

MIT License - See LICENSE file for details.

---

## Version Information

- **Documentation Version**: 1.0 (Phase 6)
- **Code Version**: Main branch
- **Last Updated**: 2026-09-05
- **Target Release**: Phase 6 complete
