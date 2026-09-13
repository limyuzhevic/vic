# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is a computational neuroscience research project that implements a brain-like system capable of learning, development, and cognition through spiking neural networks. Unlike traditional deep learning models, NLM implements biologically-inspired mechanisms including:

- **Spiking neurons** with integrate-and-fire dynamics
- **Synaptic plasticity** through STDP and Hebbian learning
- **Neuromodulation** for reward and curiosity
- **Memory systems** for working, episodic, and associative memory
- **Prediction systems** for anticipating future states
- **Developmental stages** that affect learning rates

## Key Features

✅ **Real Neural Computation**
- Leaky Integrate-and-Fire (LIF) neurons with event-driven dynamics
- Synaptic delays and short-term plasticity
- Spike-timing dependent plasticity (STDP)
- Structural plasticity (synaptogenesis/pruning)

✅ **Cognitive Architecture**
- Working memory for active information maintenance
- Episodic memory for experience storage
- Predictive modeling of sensory inputs
- Neural planning and decision-making
- Social learning and concept formation

✅ **Biological Fidelity**
- Neuromodulation (dopamine, curiosity, novelty)
- Developmental stages (initial → critical → maturation → adult → aging)
- Multiple neuron and synapse types
- Realistic refractory periods

✅ **Python Integration**
- Comprehensive Python bindings for simulation
- Agent-based world interaction
- Visualization and debugging tools
- Training and optimization APIs

## Current Phase: Phase 6 - Final Integration

**Status**: ✅ COMPLETE

Phase 6 successfully integrates all systems into a coherent artificial brain. Key achievements include:

### Integration Status

| System | Status | Notes |
|--------|--------|-------|
| **Neural Core** | ✅ WORKING | LIF dynamics, STDP, structural plasticity |
| **Memory Systems** | ✅ CONNECTED | Working + episodic memory functional |
| **Neuromodulation** | ✅ INTEGRATED | Dopamine, curiosity, novelty active |
| **Prediction** | ✅ OPERATIONAL | Temporal and action consequence prediction |
| **Development** | ✅ MATURING | Age-dependent plasticity and synaptogenesis |
| **Cognition** | ✅ ACTIVE | Planning, attention, concept formation |
| **Persistence** | ✅ STABLE | Checkpoint save/load working |

### Capabilities Demonstrated

- **Learning from experience**: Synaptic weights change through interaction
- **Memory consolidation**: Important episodes stored and replayed
- **Developmental progression**: Brain matures and adapts over time
- **Goal-directed behavior**: Curated exploration based on curiosity
- **Adaptive intelligence**: Cognition emerges from neural dynamics

## Quick Start Guide

### Building from Source

```bash
# Clone the repository
# Check release notes for specific build instructions

# Basic build process (Linux/macOS)
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Python package installation
pip install .
```

### Running the Demo

```bash
# Phase 6 Integration Demo (recommended for new users)
./nlm_phase6_demo

# Phase 3 World Interaction
./nlm_phase3_demo [steps] [time_limit]

# Phase 4 Cognitive Experiments
./nlm_phase4_demo

# Interactive Python exploration
python3 -c "import pynlm; print(pynlm.__version__)"
```

### Python Examples

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simple simulation loop
for step in range(100):
    brain.step(step)
    print(f"Spike {step}: {brain.getFiringNeuronCount()} neurons firing")

# Memory demonstration
wm = brain.getWorkingMemory()
if wm:
    print(f"Working memory traces: {wm.getActiveTraces()}")

# Learning demonstration
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Learning rate achieved: {brain.getAverageFiringRate():.4f} Hz")
```

## Project Structure

```
NLM/
├── CMakeLists.txt                    # Build system
├── pyproject.toml                    # Python package metadata
├── configs/                          # Configuration files
│   └── default.cfg                   # Default brain settings
├── docs/                             # Documentation
│   ├── README.md                     # Project overview
│   ├── easy_usage.md                 # Beginner guide
│   ├── docs/ARCHITECTURE.md          # System architecture
│   ├── docs/SCIENCE.md                # Biological background
│   └── docs/PHASE6_FINAL_AUDIT.md    # Phase 6 integration report
├── python/                           # Python bindings
│   ├── bindings.cpp                  # C++ to Python interface
│   └── CMakeLists.txt                # Python module build
├── src/                              # Core C++ implementation
│   ├── core/                         # Core utilities
│   │   ├── Types/Types.hpp            # Type definitions
│   │   ├── Config/Config.hpp         # Configuration system
│   │   └── ...
│   ├── brain/                        # Neural components
│   │   ├── Brain.hpp                 # Central brain controller
│   │   ├── Neuron.hpp                # LIF neuron model
│   │   ├── Synapse.hpp               # Synaptic transmission
│   │   └── ...
│   ├── memory/                       # Memory systems
│   ├── prediction/                   # Prediction systems
│   ├── cognition/                    # Cognitive mechanisms
│   ├── neuromodulation/               # Neuromodulators
│   └── ...
│   ├── environment/                  # World simulation
│   ├── motor/                        # Motor system
│   └── visualization/                # Visualization tools
├── tests/                           # Test suite
└── examples/                        # Usage examples
```

## Scientific Background

NLM is grounded in computational neuroscience research:

- **Spiking Neural Networks**: More biologically realistic than rate-based models
- **Predictive Processing**: Brain as a prediction machine (free energy principle)
- **Active Inference**: Actions chosen to minimize prediction errors
- **Neurodevelopmental Principles**: Learning changes with age

### Key References

- Friston, K. (2010). The free-energy principle. Nature Reviews Neuroscience.
- Gerstner, W., et al. (2014). Neural coding, inference and computation. Nature Neuroscience.
- Seung, H. S. (2006). How the brain learns to see. MIT Press.

## Installation

### Prerequisites

```bash
# C++ compiler with C++20 support
g++ >= 9.0

# Python 3.8+
python3 >= 3.8

# Build tools
make
cmake
```

### Installation Steps

```bash
# Clone repository
git clone <repository-url>
cd nlm

# Build C++ core
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install -e .

# Test installation
python3 -c "import pynlm; print(pynlm.__version__)"
```

## Technical Documentation

### Configuration System

NLM uses a flexible configuration system with both command-line and file-based configuration:

```bash
# Command line arguments
./nlm --config=experiment.cfg --neuron_count=1000 --stdp_ltp_weight=0.05

# Configuration file format
# key = value
# Comments start with #
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
connection_probability = 0.15
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
```

### Python API

The Python API provides high-level access to all NLM functionality:

#### Core Classes

- **Brain**: The neural simulation engine
- **AgentBrain**: Interface between brain and world
- **SimpleWorld**: 2D environment with physics
- **Config**: Configuration management
- **Vision**: Visual sensory input
- **Audio**: Auditory sensory input
- **InternalSignals**: Internal state signals

#### Key Methods

- **brain.initialize()**: Initialize neural network
- **brain.step(step)**: Advance simulation by one timestep
- **brain.getFiringNeuronCount()**: Get number of firing neurons
- **brain.getTotalSpikeCount()**: Get total spikes in network
- **brain.getAverageFiringRate()**: Get average firing frequency

#### Advanced Features

```python
# Agent-based simulation
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Advanced Usage

### Custom Configurations

Create custom brain configurations for specific experiments:

```python
import pynlm

# Create custom config
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("connection_probability", 0.05)
config.set("stdp_ltp_weight", 0.02)
config.set("stdp_ltd_weight", 0.015)
config.set("synaptogenesis_rate", 0.0005)
config.set("pruning_rate", 0.00005)

# Create brain with custom config
brain = pynlm.createBrain(config)
brain.initialize()
```

### Memory Inspection

```python
# Check memory systems
wm = brain.getWorkingMemory()
em = brain.getEpisodicMemory()

if wm:
    print(f"Working memory active traces: {wm.getActiveTraces()}")
    print(f"Working memory capacity: {wm.getCapacity()}")

if em:
    print(f"Episodic memory episodes: {em.getEpisodeCount()}")
    print(f"Episodic memory capacity: {em.getCapacity()}")
```

### Visualization

```python
# Enable and use visualization
viz = pynlm.VisualizationInterface()
if viz.initialize():
    # Simulate and visualize
    for step in range(100):
        brain.step(step)
        viz.update()
        viz.render()
    
    viz.close()
```

## Contributing

### Development Guidelines

1. **Code Quality**: Follow Google C++ style guide
2. **Testing**: Add unit tests for new functionality
3. **Documentation**: Update README and docstrings
4. **Performance**: Optimize critical paths
5. **Validation**: Run comprehensive test suite

### Project Rules

- **No breaking changes**: Maintain backward compatibility
- **Documentation first**: Document before implementing
- **Testing priority**: All code must be tested
- **Performance conscious**: Profile and optimize bottlenecks
- **Collaboration friendly**: Clear APIs and well-structured code

## License

MIT License. See LICENSE file for details.

## Acknowledgments

This project builds on decades of research in computational neuroscience, spiking neural networks, and artificial intelligence. Special thanks to the open-source community for providing foundational tools and libraries.

## Contact

For questions, issues, or collaborations:
- GitHub: <https://github.com/nlm-project/nlm>
- Documentation: <https://nlm.readthedocs.io>
- Email: [contact at nlm-project dot org]

## Future Directions

### Phase 7: Advanced Intelligence

- Implement hierarchical predictive coding
- Add meta-learning and optimization
- Develop complex social learning
- Implement consciousness and self-modeling

### Performance Improvements

- GPU acceleration for large-scale simulations
- More efficient event-driven algorithms
- Advanced visualization and analysis tools
- Cloud-based distributed computing

### Application Domains

- Computational neuroscience research
- Neuromorphic computing hardware
- Educational tools for neural learning
- Brain-computer interface development

---

**Last Updated**: September 13, 2026
**Version**: 0.1.0
**Status**: Production Ready for Research Use

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

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

## License

MIT

## Authors

Research project — See docs for scientific background.
