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
## Python API

NLM provides comprehensive Python bindings through the `pynlm` module, making it accessible for research and development. The Python API is designed to be both powerful and easy to use.

### Installation

```bash
# Install from source (recommended for development)
pip install -e .

# Or install the built package
pip install pynlm
```

### Quick Start

```python
import pynlm

# Create a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Examples

NLM includes comprehensive examples in the `examples/` directory:

- `examples/simple_brain.py` - Basic brain creation and simulation
- `examples/agent_world.py` - Agent-environment interaction
- `examples/learning_demo.py` - Learning demonstrations
- `examples/curiosity_learning.py` - Curiosity-driven exploration
- `examples/memory_system.py` - Memory system usage
- `examples/complete_agent.py` - Complete agent with all subsystems

### Running Examples

```bash
# Run all examples
for example in examples/*.py; do
    echo "Running $example"
    python "$example"
done
```

## Building (Native C++)

NLM is primarily a C++ project with Python bindings. For those who want to run the native simulation or contribute to development:

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
mkdir build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4

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

### Python Integration

The Python bindings are automatically built and installed alongside the C++ components. The `pyproject.toml` file ensures proper integration with Python's packaging ecosystem.

## Python API Overview

The NLM Python API provides access to the full functionality of the C++ neural simulation framework through an intuitive interface.

### Core Classes

#### `Config`
Configuration management for NLM systems

#### `Brain`
Central neural simulation brain class

#### `AgentBrain`
Agent interface connecting NLM brain to world

#### `SimpleWorld`
Simple 2D world for NLM simulation

#### `SensoryPercept`
Sensory percept data container

### Key Features

1. **Native Performance**: Core computations run in optimized C++
2. **Python Convenience**: High-level API for rapid prototyping
3. **Memory Management**: Automatic garbage collection
4. **Error Handling**: Python-friendly exception types
5. **Type Safety**: Basic type checking and conversion
6. **Documentation**: Comprehensive docstrings and examples

### Performance Considerations

- **Simulation Speed**: C++ backend provides high-performance neural simulations
- **Memory Efficiency**: Configurable based on neuron count and connectivity
- **Integration**: Easy to integrate with other Python libraries (numpy, matplotlib, etc.)

### Usage Examples

#### Basic Brain Simulation
```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Agent-Environment Interaction
```python
import pynlm

# Setup complete agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Examples Directory

The `examples/` directory contains comprehensive working examples that demonstrate:

- Basic brain creation and simulation
- Agent-environment interaction
- Learning and memory systems
- Curiosity-driven exploration
- Performance optimization
- Error handling
- Advanced configuration

### Running Examples

```bash
# Install required dependencies
pip install matplotlib numpy

# Run specific example
python examples/simple_brain.py

# Run all examples
for example in examples/*.py; do
    python "$example"
done
```

## Configuration

### Brain Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |

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
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Architecture

NLM implements a brain-inspired architecture with the following core components:

- **Core Utilities**: Configuration management, random number generation, logging
- **Brain**: Neural computation with LIF neurons, synapses, and plasticity
- **Dynamics**: Neural dynamics including spike propagation and synchronization
- **Plasticity**: Learning rules including STDP, Hebbian, and structural plasticity
- **Development**: Developmental stages and age-dependent plasticity
- **Neuromodulation**: Dopamine, curiosity, and novelty systems
- **Memory**: Working, episodic, and associative memory systems
- **Cognition**: Planning, attention, and concept formation
- **Sensory**: Vision, audio, and internal signal processing
- **Prediction**: Forward prediction and error-based learning
- **Motor**: Action selection and execution
- **Environment**: World simulation and interaction
- **Experiments**: Experiment framework and metrics
- **Visualization**: Visualization interfaces and tools

## Building (Native C++)

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
