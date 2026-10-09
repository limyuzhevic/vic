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

**PHASE 6: ADVANCED INTEGRATION**

Phase 6 advances beyond basic integration by implementing sophisticated cognitive architectures and biological realism. The system now demonstrates Phase 4-6 capabilities with enhanced modularity and extensibility.

### Phase 6 Integration Achievements

#### Core Brain Systems
- **Phase 4 Cognitive Systems**: Implemented NeuralPlanner, ConceptFormation, SelfModel, and SocialLearning with proper neural mechanisms
- **Phase 5 Performance Infrastructure**: Memory pools, event queues, SIMD vectorization, and parallel processing for scalability
- **Phase 6 Real Environment**: Physics-based environment with realistic dynamics, weather systems, and resource management

#### Advanced Neuromodulation
- **Multi-layered Integration**: Enhanced AgentBrain with dopamine, curiosity, novelty detection, and prediction error signals
- **Memory-Gating**: Neuromodulators now connect directly to memory consolidation and retrieval processes
- **Adaptive Plasticity**: Dynamic plasticity rules that adapt based on developmental stage and environmental context

#### Memory Systems (Phase 4)
- **Working Memory**: NeuralWorkingMemory with persistent activity and competitive selection
- **Episodic Memory**: NeuralEpisodicMemory with pattern completion and replay mechanisms
- **Semantic Memory**: NeuralSemanticMemory for abstract knowledge representation
- **Procedural Memory**: NeuralProceduralMemory for learned action sequences
- **Associative Memory**: NeuralAssociativeMemory for pattern association learning

#### Prediction Systems (Phase 4)
- **Multi-step Prediction**: NeuralPrediction learns temporal sequences and action consequences
- **Prediction Error Tracking**: Comprehensive error signals for learning optimization
- **Confidence Estimation**: Uncertainty quantification for decision making
- **Hierarchical Prediction**: Multiple prediction levels for complex scenario modeling

#### Enhanced Motor System (Phase 6)
- **Biological Realism**: MotorSystem with cortical spreading activation and basal ganglia integration
- **Actor-Critic Learning**: Advanced reinforcement learning for action selection and value estimation
- **Temporal Planning**: ActionSequence planning with timing optimization
- **Dynamics Compensation**: Real-time compensation for physical dynamics

#### Development & Learning
- **Developmental Stages**: Progressive maturation from Initial → CriticalPeriod → Maturation → Adult
- **Adaptive Plasticity**: Plasticity rates that decrease with age and increase during critical periods
- **Experience-Dependent Learning**: Continual learning without catastrophic forgetting

#### Advanced Environment Integration
- **RealPhysics**: Full physics simulation with collisions, gravity, and fluid dynamics
- **Weather System**: Dynamic weather affecting behavior and sensory perception
- **Resource Management**: Energy, food, and material collection and consumption
- **Hazard Systems**: Environmental dangers requiring adaptive responses

### Modern C++17 Features

NLM implements cutting-edge C++17 best practices:

- **Modern Memory Management**: Smart pointers, RAII, and pImpl idiom
- **Event-Driven Architecture**: Lock-free queues and time-bucketed delayed processing
- **Parallel Processing**: Thread-safe neural processing with SIMD vectorization
- **Type Safety**: Strong typing with compile-time checks and validation
- **Template Metaprogramming**: Compile-time optimization and code generation
- **STL Best Practices**: Custom allocators and memory pools for performance

### Enhanced Python Bindings

The Python API has been enhanced with Phase 4-6 capabilities:

- **Advanced Agent API**: Full access to neuromodulation, prediction, and cognitive systems
- **Memory Management**: Python objects with garbage collection and automatic cleanup
- **Event-Driven Interface**: Asynchronous processing and real-time simulation
- **Configuration System**: Dynamic parameter adjustment and validation
- **Comprehensive Error Handling**: Python exceptions with detailed error information

### Improved Error Handling

Phase 6 introduces comprehensive error management:

- **Hierarchical Error Types**: Custom error codes for different failure modes
- **Runtime Validation**: Input validation and bounds checking
- **Recovery Mechanisms**: Graceful degradation and fallbacks
- **Diagnostics**: Detailed error reporting with context and suggestions
- **Testing Integration**: Comprehensive unit tests covering all error conditions

### Comprehensive Test Coverage

Phase 6 includes extensive testing infrastructure:

- **Unit Tests**: 30+ test files covering all subsystems
- **Integration Tests**: Phase4, Phase5, and Phase6 experiment suites
- **Performance Benchmarks**: Scaling and optimization testing
- **Error Handling Tests**: Robustness and failure mode testing
- **Regression Testing**: Continuous integration with automated testing

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

**Build Options:**
- `-DENABLE_TESTS=ON` - Build test suite
- `-DENABLE_PYTHON=ON` - Build Python bindings
- `-DENABLE_PERFORMANCE=ON` - Enable SIMD optimizations
- `-DEXPERIMENT_SUITE=Phase6` - Build Phase 6 experiments

## Running

### Phase 6 Integration Demo

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all Phase 4-6 systems work together as a coherent artificial brain.

### Python Examples

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable advanced systems
brain.enableWorkingMemory(True)
brain.enableEpisodicMemory(True)
brain.enablePrediction(True)
agent.enableNeuromodulation(True)

# Run with Phase 4-6 capabilities
for step in range(1000):
    brain.step(step)
    # Advanced cognitive processing
    brain.updateWorkingMemory()
    brain.consolidateEpisodicMemory()
    brain.updatePrediction()
```

### Performance Testing

```bash
./nlm_test --suite=Phase6 --benchmark
```

This runs the complete Phase 6 test suite with performance metrics.

## Key Features

### Phase 4 Cognitive Abilities
- **Prediction & Planning**: Anticipate action consequences and plan sequences
- **Concept Formation**: Discover patterns without predefined labels
- **Self-Model**: Maintain sensorimultimodal self-awareness
- **Social Learning**: Observe and imitate other agents

### Phase 5 Scalability
- **Memory Pools**: Pre-allocated neuron/synapse pools
- **Event Queues**: Lock-free processing with time-bucketing
- **Parallel Processing**: Multi-threaded neural computation
- **SIMD Optimization**: Vectorized neural updates

### Phase 6 Advanced Integration
- **Physics-Based Environment**: Realistic dynamics and interactions
- **Complex Behavior**: Weather effects, resource management, hazard avoidance
- **Real-Time Learning**: Continual adaptation without forgetting
- **Biological Fidelity**: Detailed neuromodulation and development

## Project Structure

```
NLM/
├── CMakeLists.txt                    # Modern CMake configuration
├── README.md                          # Current documentation
├── python/                           # Python bindings
│   ├── bindings.cpp                  # pybind11 bindings
│   └── CMakeLists.txt                # Python build system
├── docs/                             # Comprehensive documentation
│   ├── ARCHITECTURE.md               # Phase 4-6 integration overview
│   ├── SCIENCE.md                    # Biological background
│   ├── ROADMAP.md                    # Future development plan
│   ├── NEW_FEATURES.md               # Phase 6 improvements
│   ├── TUTORIALS.md                  # Usage tutorials
│   └── API_REFERENCE.md              # Complete API documentation
├── src/                              # Source code
│   ├── core/                        # Core utilities (C++17)
│   ├── brain/                       # Neural components
│   ├── dynamics/                    # Neural dynamics
│   ├── plasticity/                  # Plasticity rules
│   ├── development/                 # Developmental system
│   ├── neuromodulation/             # Neuromodulators
│   ├── memory/                      # Memory systems (Phase 4)
│   ├── prediction/                  # Prediction systems (Phase 4)
│   ├── cognition/                   # Cognitive mechanisms (Phase 4)
│   ├── sensory/                     # Sensory processing
│   ├── motor/                       # Motor system (Phase 6)
│   ├── environment/                 # Environment interface (Phase 6)
│   └── experiments/                 # Experiment framework (Phase 6)
├── tests/                           # Comprehensive test suite
│   ├── test_*.cpp                   # Unit tests
│   ├── experiments/                 # Phase experiments
│   └── benchmarks/                  # Performance benchmarks
└── configs/                         # Configuration files
```

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

## Support & Contact

For questions about Phase 4-6 implementation, Python bindings, or modern C++ features, refer to the comprehensive documentation in the docs/ directory.

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

## License

MIT

## Authors

Research project — See docs for scientific background.
