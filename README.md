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

### Python API Usage

```bash
# Install Python bindings
pip install .

# Run Python demo
python -c "
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    print(f'Step {step}: {brain.getFiringNeuronCount()} firing neurons')
"
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── HOW_TO_USE.md          # Comprehensive user guide
├── easy_usage.md          # Beginner-friendly guide
├── AGENTS.md              # Complete API reference (this file)
├── docs/                  # Scientific and technical documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/                   # C++ source code
│   ├── core/              # Core utilities
│   ├── brain/             # Neural components (integrated)
│   ├── dynamics/          # Neural dynamics
│   ├── plasticity/        # Plasticity rules
│   ├── development/       # Developmental system
│   ├── neuromodulation/    # Neuromodulators (integrated)
│   ├── memory/            # Memory systems (integrated)
│   ├── prediction/        # Prediction systems (integrated)
│   ├── cognition/         # Cognitive mechanisms (integrated)
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Motor system
│   ├── environment/       # Environment interface
│   ├── experiments/       # Experiment framework (Phase 6)
│   └── visualization/     # Visualization
├── python/                # Python bindings
│   └── bindings.cpp       # Pybind11 bindings
├── tests/                 # Test suite
└── configs/               # Configuration files
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

## Quick Start Guide

### 1. Building from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build C++ core
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install .
```

### 2. Running the Integration Test

```bash
# Run the Phase 6 integration test
cd build
./nlm_phase6_demo
```

### 3. Using the Python API

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

## Documentation Structure

### Core Documentation Files

1. **README.md** (this file) - Project overview and quick start
2. **HOW_TO_USE.md** - Comprehensive usage guide with examples
3. **easy_usage.md** - Beginner-friendly guide with simple examples
4. **AGENTS.md** - Complete API reference for all classes and methods

### Technical Documentation

5. **docs/ARCHITECTURE.md** - System architecture overview
6. **docs/SCIENCE.md** - Scientific background and principles
7. **docs/ROADMAP.md** - Project roadmap and future directions
8. **docs/EXPERIMENTS.md** - Experiment descriptions and results

### Development Documentation

9. **docs/PHASE6_FINAL_AUDIT.md** - Phase 6 implementation audit
10. **docs/LIMITATIONS.md** - Current limitations and future work

## Python API Features

### Key Classes

- **Config** - Configuration management with type conversion utilities
- **Brain** - Central neural simulation brain with integrated systems
- **AgentBrain** - Bridge connecting brain to world interface
- **SimpleWorld** - Simple 2D world for NLM development
- **SensoryInput** - Base class for sensory inputs (Vision, Audio, InternalSignals)
- **Action** - Action representation for motor output
- **WorldObject** - Object representation in the world

### Enumerations

- **NeuronType** - Neuron types (Excitatory, Inhibitory, Modulatory, etc.)
- **SynapseType** - Synapse types (Excitatory, Inhibitory, Modulatory, etc.)
- **DevelopmentalStage** - Developmental stages (Initial, CriticalPeriod, etc.)
- **ActionType** - Action types (MoveForward, TurnLeft, etc.)
- **WorldObjectType** - World object types (Resource, Hazard, Wall, etc.)

### Configuration Parameters

#### Brain Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

#### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

#### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

### Complete API Documentation

For comprehensive API documentation including:
- Detailed method signatures and return types
- Parameter descriptions and types
- Code examples for all classes
- Usage patterns and best practices
- Configuration parameter references

See **AGENTS.md** for the complete API reference.

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

## Contributing

### Documentation Contributions

1. **Add new examples** to howto or easy_usage guides
2. **Enhance AGENTS.md** with additional API details
3. **Update tutorials** with new use cases
4. **Fix errors** and improve clarity in existing documentation

### Code Contributions

1. Fork the repository
2. Create feature branch
3. Implement changes
4. Update documentation
5. Submit pull request

## Support and Questions

For questions about NLM, please refer to:
- **Documentation**: AGENTS.md, HOW_TO_USE.md, easy_usage.md
- **GitHub**: Issues and discussions
- **Examples**: Phase 6 integration test
- **Quick Start**: README.md

## Next Steps

1. **Read HOW_TO_USE.md** for detailed usage instructions
2. **Run the integration test** to see all systems working together
3. **Experiment with different configurations** in AGENTS.md
4. **Read docs/ARCHITECTURE.md** for technical details
5. **Read docs/SCIENCE.md** for scientific background

## Contact

For research questions and collaboration opportunities, please refer to the project documentation and GitHub repository.

---

*Documentation generated automatically from source code docstrings and comments.*
*Phase 6 (Final Integration) Complete*
*Last updated: $(date)*

## Version

- **C++ Core**: Phase 6 (Final Integration)
- **Python Bindings**: v0.1.0
- **Documentation**: Complete API reference in AGENTS.md