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

## Current status (honest, 2026-11-02)

Phase 6 is complete: all systems (memory, neuromodulation, prediction, cognition, development, plasticity) are integrated into a coherent artificial brain. The main demo (`./nlm`) now demonstrates the complete integrated system:

- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity
- Neuromodulation (dopamine, curiosity, novelty)
- Working, episodic, and associative memory
- Neural prediction and planning
- Concept formation and attention
- Developmental stages and maturation
- Checkpoint persistence and replay

The integrated brain shows activity-dependent synaptic modification and functions as a unified learning system.

Still not perfect: reward signals in behavioral demos are limited, and learning selectivity could be improved. Those are the next challenges, not hidden ones.

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

### Python Bindings

To build the Python bindings (required for `pip install .`):

```bash
pip install pybind11 scikit-build-core pytest numpy
pip install .
```

> Note: Python bindings are now built by default in `pyproject.toml`. A plain C++ build requires `-DNLM_BUILD_PYTHON_BINDINGS=ON`.

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main Phase 6 integrated brain demo |
| `nlm_phase3_demo` | Executable | Phase 3: World interaction demo |
| `nlm_phase4_demo` | Executable | Phase 4: Advanced cognition demo |
| `nlm_phase5_demo` | Executable | Phase 5: Performance and checkpoint demo |
| `nlm_phase6_demo` | Executable | Phase 6: Legacy integration test |
| `nlm_phase6_text_demo` | Executable | Phase 6: Text-as-sense demo |
| `nlm_benchmark` | Executable | Performance benchmark |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |

### Python Module

The Python module `pynlm` includes:

- **Brain class**: Core neural simulation with integrated systems
- **Config class**: Configuration management for all parameters
- **AgentBrain class**: Interface between brain and world
- **SimpleWorld class**: 2D environment with sensory processing
- **Visualization classes**: Vision, audio, and internal signal processing
- **Action classes**: Motor commands and world interaction

## Running

Build all targets first (see Building above). The entry points are:

```bash
./nlm                 # Phase 6: Main integrated brain demo (replaces Phase 2)
./nlm_phase3_demo     # Phase 3: World interaction demo
./nlm_phase4_demo     # Phase 4: Advanced cognition demo  
./nlm_phase5_demo     # Phase 5: Performance and checkpoint demo
./nlm_phase6_demo     # Phase 6: Integration test (legacy)
./nlm_phase6_text_demo # Phase 6: Text-as-sense demo
./nlm_benchmark        # Performance benchmark
```

### Main Demo (Phase 6)

`./nlm` now runs the complete integrated Phase 6 demo, which demonstrates:

- All neural systems (memory, prediction, cognition, neuromodulation)
- Working memory with persistent activity
- Neuromodulation (dopamine, curiosity, novelty)
- Prediction systems with error signals
- Neural planning and concept formation
- Developmental stages and structural plasticity
- Checkpoint persistence and replay capabilities

### Python API

NLM includes comprehensive Python bindings (`pynlm` module) for:

```python
import pynlm

# Create a complete brain with configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent and world for embodied intelligence
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Run simulation loop
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Configuration

Comprehensive configuration options are available in `configs/default.cfg`:

- Neural parameters (neuron count, connection probability)
- Plasticity settings (STDP, Hebbian, structural)
- Neuromodulation parameters (dopamine, curiosity, novelty)
- Memory system parameters
- Development and maturation settings
- Performance and optimization options
- Logging and visualization settings

### Key Features for Advanced Users

1. **Modular Architecture**: Independent systems for memory, prediction, cognition
2. **Neuromodulation**: Dopamine, curiosity, novelty drive learning
3. **Development**: Age-dependent plasticity and maturation
4. **Integration**: All systems work together as a unified brain
5. **Persistence**: Checkpoint save/load for long-running simulations
6. **Performance**: SIMD vectorization, memory pools, event queues
7. **Extensibility**: Easy to add new modules and systems

## Project Structure

```
NLM/
├── CMakeLists.txt
├── pyproject.toml              # Modern Python packaging
├── README.md
├── configs/                    # Configuration files
│   └── default.cfg
├── easy_usage.md               # Quick start for beginners
├── HOW_TO_USE.md                # Comprehensive usage guide
├── docs/                       # Documentation
│   ├── ARCHITECTURE.md         # System architecture
│   ├── SCIENCE.md              # Scientific background
│   ├── ROADMAP.md              # Development roadmap
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/                        # Source code
│   ├── core/                   # Core utilities
│   ├── agent/                  # Agent-brain interface
│   ├── world/                  # Simple world
│   ├── performance/            # Performance optimizations
│   ├── brain/                  # Neural components (integrated)
│   ├── dynamics/               # Neural dynamics
│   ├── plasticity/             # Plasticity rules
│   ├── development/            # Developmental system
│   ├── neuromodulation/        # Neuromodulators (integrated)
│   ├── memory/                 # Memory systems (integrated)
│   ├── prediction/             # Prediction systems (integrated)
│   ├── cognition/              # Cognitive mechanisms (integrated)
│   ├── sensory/                # Sensory processing
│   ├── motor/                  # Motor system
│   ├── environment/            # Environment interface
│   ├── experiments/            # Experiment framework (Phase 6)
│   └── visualization/          # Visualization
├── tests/                      # Unit tests
└── python/                     # Python bindings
    └── bindings.cpp
```

## Phase Status Summary

### Phase 1 (Complete)
- Project skeleton and build system
- Core types and configuration management
- Neural interfaces and basic types

### Phase 2 (Complete) 
- Real Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Basic structural plasticity (synaptogenesis/pruning)

### Phase 3 (Complete)
- World interaction loop and sensory-motor coordination
- Reward prediction error and learning signals
- Developmental stages and age-dependent plasticity
- Novelty detection and curiosity-driven exploration

### Phase 4 (Complete)
- Neural prediction system with forward models
- Working memory with persistent activity
- Episodic-like memory and pattern completion
- Concept formation from experience
- Neural attention (competitive selection)
- Predictive planning and decision making
- Self-model and internal state representation
- Social learning and observational learning
- Continual learning and catastrophic interference prevention

### Phase 5 (Complete)
- Performance optimizations (SIMD vectorization)
- Memory pools and event queues for efficiency
- Parallel processing capabilities
- Checkpoint system for persistence

### Phase 6: Final Integration (Complete)
All systems integrated into a coherent artificial brain:

- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
- **Prediction System**: Forward models and confidence tracking
- **Cognition Systems**: Neural planner, concept formation, attention
- **Development Integration**: Age-dependent plasticity, structural changes
- **Persistence**: Checkpoint save/load, replay and consolidation
- **Replay and Consolidation**: Episodic replay, memory strengthening
- **Final Integration**: All systems working together as unified brain

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

## Key Improvements Made

1. **Configuration Management**: Comprehensive config file with validation
2. **Python API**: Full-featured bindings with error handling and documentation
3. **Memory Systems**: Working, episodic, and associative memory integration
4. **Neuromodulation**: Coordinated dopamine, curiosity, and novelty systems
5. **Prediction Systems**: Forward models with confidence tracking
6. **Development Integration**: Age-dependent plasticity and maturation
7. **Performance Optimizations**: SIMD, memory pools, parallel processing
8. **Persistence**: Checkpoint save/load and replay capabilities
9. **Documentation**: Comprehensive user guides and examples
10. **Error Handling**: Robust logging and error recovery

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

## License

MIT

## Authors

Research project — See docs for scientific background.
