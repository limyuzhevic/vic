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
└── configs/
```

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

## Advanced Python API (Phase 7)

**NEW in Phase 7: Enhanced Python Bindings with Runtime Features**

NLM now provides an enhanced Python API with comprehensive control over all brain systems:

### 1. **Expanded Core API**
- Low-level neuron and synapse access
- Full memory system control
- Advanced neuromodulation
- Comprehensive plasticity systems

### 2. **Dynamic Command System**
- Runtime feature addition
- Custom command registration
- Dynamic brain management
- On-the-fly simulation control

### 3. **Advanced Monitoring & Analysis**
- Performance benchmarking
- Real-time monitoring
- Debug and validation tools
- Advanced data analysis

### 4. **Enhanced Development Support**
- Development stage control
- Maturation management
- Age-dependent plasticity
- Cognitive development

### API Versions

**Basic API** (`python/bindings.cpp`)
- Simple, user-friendly interface
- Essential functionality for beginners
- Quick setup and use
- Standard configuration options

**Enhanced API** (`python/bindings_enhanced.cpp`)
- Comprehensive brain component access
- Advanced learning and development controls
- Performance optimization tools
- Runtime feature addition
- Debugging and monitoring capabilities

### Enhanced Class Examples

```python
import pynlm

# Access low-level neural components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Neuron control
neurons = brain.getAllNeurons()
for neuron in neurons[:5]:
    neuron.setThreshold(-45.0)
    neuron.setType(pynlm.NeuronType.Modulatory)

# Synapse management
synapse = pynlm.createSynapse(
    pynlm.SynapseId(0),
    neurons[0].getId(),
    neurons[1].getId()
)
synapse.setWeight(1.5)
synapse.setDelay(5)

# Memory systems
working_memory = pynlm.createWorkingMemory()
working_memory.setCapacity(1000)

episodic_memory = pynlm.createEpisodicMemory()
episodic_memory.storeExperience(" learned something", 0.8, time.time())

# Neuromodulation systems
reward = pynlm.createReward()
reward.setLearningRate(0.01)
curiosity = pynlm.createCuriosity()
curiosity.setLevel(0.7)

# Plasticity systems
stdp = pynlm.createSTDP()
structural = pynlm.createStructuralPlasticity()

# Development system
development = pynlm.createDevelopmentSystem()
development.setStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Dynamic command system
command_registry = pynlm.createCommandRegistry()
```

### Performance Optimization

```python
import pynlm

# Configure memory pools
memory_config = pynlm.createMemoryPoolConfig()
memory_config.blockSize = 4096
memory_config.numBlocks = 256
memory_config.enablePreallocation = True

# Configure SIMD vectorization
simd_config = pynlm.createSIMDConfig()
simd_config.enableVectorization = True
simd_config.vectorWidth = 512

# Configure parallel processing
events = pynlm.createEventQueue()
events.setMaxWorkers(4)
events.setMaxQueueSize(1000)
```

## Python Package Information

### Package Name
- **Name**: `pynlm`
- **Version**: `0.2.0`
- **Description**: Python bindings for NLM neural simulation framework

### Installation

```bash
# Install with basic API (recommended for most users)
pip install .

# Install with enhanced API (advanced users)
pip install -e .[tools]

# Build from source
cmake .. -DENHANCED_PYTHON_BINDINGS=ON
make
pip install .
```

### Dependencies

**Required:**
- Python >= 3.8
- scikit-build-core
- pybind11 >= 2.11.0
- numpy >= 1.20

**Optional (Advanced Features):**
- pandas >= 1.0
- scipy >= 1.7
- matplotlib >= 3.0
- jupyter >= 1.0

## Python Testing

### Python Examples

```bash
cd examples
python basic_simulation.py
python advanced_simple.py
python memory_analysis.py
python performance_benchmark.py
python dynamic_commands.py
```

### Python Testing

```bash
# Run Python tests
pytest tests/

# Run with coverage
pytest --cov=pynlm --cov-report=html tests/

# Run examples
python examples/basic_simulation.py
python examples/advanced_simple.py
```

## Building with Enhanced Python Bindings

```bash
# Standard build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install basic Python bindings
pip install -e .

# Enhanced build with advanced Python API
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENHANCED_PYTHON_BINDINGS=ON
make

# Install enhanced Python bindings
pip install -e .[tools]

# Run tests
ctest --output-on-failure
```

## Development

### Python Development

```bash
# Python development setup
pip install -e .[dev]

# Run Python tests
pytest tests/

# Type checking
mypy pynlm/

# Code formatting
black pynlm/
isort pynlm/
```

## Contributing

The Phase 7 Advanced Python API enhances the NLM framework with:

1. **Enhanced Python Bindings**: Comprehensive access to neural components
2. **Runtime Features**: Dynamic command system for flexible experimentation
3. **Advanced Tools**: Performance monitoring and debugging capabilities
4. **Research Support**: Low-level neural control for advanced research

## Future Enhancements

### Phase 8 Planned Features

1. **Multi-Agent Systems**: Communication between multiple NLM instances
2. **Hardware Acceleration**: GPU support for neural computation
3. **Real-time Interfaces**: Live interaction and control
4. **Cloud Deployment**: Distributed computing capabilities
5. **Advanced Learning**: Meta-learning and transfer learning

## License

MIT

## Authors

Research project — See docs for scientific background.

