# NLM Neural Learning Machine Documentation

Welcome to the NLM (Neural Learning Machine) documentation. NLM is an experimental
artificial developmental brain that implements a complete neural system with
integrated memory, prediction, cognition, and neuromodulation systems.

## Overview

NLM represents a computational approach to artificial brain development,
inspired by biological principles of neural computation. The system evolves
through developmental stages, acquiring increasingly complex capabilities
through interaction with an environment.

### Key Features

- **Real Spiking Neural Computation**: Leaky Integrate-and-Fire (LIF) neurons
  with event-driven dynamics
- **Integrated Brain Systems**: Memory, prediction, cognition, and neuromodulation
- **Developmental Architecture**: Plasticity changes with age and experience
- **Closed-Loop Architecture**: Continuous interaction with world consequences
- **Experimental Framework**: Configurable for various research scenarios

## Quick Start

### Installation

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run Phase 6 integration demo
./nlm
```

### Python Bindings

```python
import nlm

# Create configuration
config = nlm.createDefaultConfig()
config.set("neuron_count", 1000)

# Create brain
brain = nlm.createBrain(config)
brain.initialize()

# Create simulation world
world = nlm.createSimpleWorld()
world.configure(20, 20, 16, 16)

# Run simulation
for step in range(1000):
    percept = world.getSensoryPercept()
    brain.receiveSensoryInput(percept)
    brain.step(step)
```

## Architecture

NLM operates as a closed-loop cognitive architecture:

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

## Core Components

### Neural System

- **Neurons**: Leaky Integrate-and-Fire (LIF) model with event-driven dynamics
- **Synapses**: Multiple types (excitatory, inhibitory, electrical, modulatory)
- **Brain Regions**: Organized neural populations with specialized functions
- **Spike System**: Event-driven spike propagation with delays

### Memory Systems

- **Working Memory**: Transient active information maintenance
- **Episodic Memory**: Experience encoding and replay
- **Associative Memory**: Pattern associations and similarity-based retrieval
- **Semantic Memory**: Generalized knowledge representations
- **Procedural Memory**: Learned action sequences

### Neuromodulation

- **Dopamine**: Reward prediction error and reinforcement learning
- **Curiosity**: Exploration motivation and novelty detection
- **Novelty**: Sensory novelty detection and change detection
- **Prediction Error**: Temporal difference learning signals

### Development System

- **Synaptogenesis**: Initial connection formation
- **Pruning**: Elimination of weak connections
- **Maturation**: Structural and functional development
- **Critical Periods**: windows of enhanced plasticity

### Prediction System

- **Internal Model**: Forward model of world dynamics
- **Predictive Coding**: Minimizing prediction errors
- **Confidence Estimation**: Uncertainty quantification

### Cognition

- **Planning**: Action sequence evaluation and selection
- **Concept Formation**: Pattern discovery and categorization
- **Attention**: Competitive selection of information
- **Self-Model**: Internal representation of agent state

### Environment Interface

- **Sensory Processing**: Vision, audio, touch, proprioception
- **Motor Output**: Action execution and feedback
- **World Model**: Internal representation of environment
- **Reward System**: Reinforcement learning signals

## Configuration System

NLM uses a flexible configuration system supporting multiple formats:

### JSON Configuration

```json
{
    "neuron_count": 1000,
    "simulation_timestep": 0.001,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01,
    "development_synaptogenesis_rate": 0.001,
    "enable_checkpointing": true,
    "enable_replay": true,
    "log_level": "INFO"
}
```

### Command Line Arguments

```bash
./nlm --neurons 1000 --steps 5000 --log-level DEBUG --checkpoint mybrain.bin
```

### Simple Configuration File

```
# NLM Configuration File
neuron_count = 1000
simulation_timestep = 0.001
connection_probability = 0.1
plasticity_learning_rate = 0.01
```

## Building and Running

### Building from Source

```bash
# Create build directory and configure
cmake_minimum_required(VERSION 3.16)
project(NLM VERSION 0.1.0 LANGUAGES CXX)

# Build
make -j4
```

### Running Demos

#### Phase 6 Integration Demo

This demo verifies that all brain systems are properly integrated and functional:

```bash
./nlm --neurons 500 --steps 2000
```

#### Phase 3 Neural Computation Demo

```bash
./nlm_phase3_demo
```

#### Phase 4 Cognitive Architecture Demo

```bash
./nlm_phase4_demo
```

### Running Tests

#### C++ Tests

```bash
cd build
ctest
```

#### Python Tests

```bash
pytest tests/
```

## Development

### Python Bindings

The NLM framework includes comprehensive Python bindings for research and experimentation:

```python
# Core classes
from nlm import Brain, Config, AgentBrain, SimpleWorld

# Factory functions
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
world = nlm.createSimpleWorld()
agent_brain = nlm.createAgentBrain(brain)

# Configuration management
config.set("random_seed", 42)
config.set("neuron_count", 1000)
config.loadFromFile("config.json")
config.saveToFile("output.json")

# Simulation loop
for step in range(1000):
    percept = world.getSensoryPercept()
    brain.receiveSensoryInput(percept)
    brain.step(step)
    action = brain.produceAction()
```

### Contributing

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Write tests and documentation
5. Submit a pull request

## Research and Limitations

### Scientific Limitations

NLM is a research project investigating computational brain-like systems. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No realistic cortical architecture
- No claim of consciousness, intelligence, or human-like cognition

### Research Focus Areas

Phase 6 investigates:

- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Performance Optimizations

Phase 6 includes performance optimizations for large-scale simulation:

- Memory pools for efficient object allocation
- Event queues for spike propagation
- SIMD vectorization for neural computations
- Parallel processing capabilities

## License

MIT License. See LICENSE file for details.

## Support

For questions, issues, or collaboration opportunities:

- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Documentation: https://nlm.readthedocs.io
- Project Repository: https://github.com/nlm-project/nlm

## References

- [Brains in a Vat](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC1073670/) - Research on artificial brains
- [Neural Engineering Framework](https://www.openmost.org/definitions/nef.pdf) - Neural computation principles
- [Dynamic Neural Field Theory](https://link.springer.com/book/10.1007/978-3-642-53882-4) - Cognitive modeling

---

*This documentation is under construction. Contributions to documentation are welcome!*
