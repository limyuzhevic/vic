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

**PHASE 2: REAL NEURAL COMPUTATION**

**IMPORTANT UPDATE: This project is currently in Phase 2. Phase 6 integration is planned but not yet implemented.**

Phase 2 implements the core biological neural computation foundations:

### Phase 2 Achievements

#### Core Neural Computation
- Real Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)

#### Key Capabilities
- Measurable synaptic changes through experience
- Network activity-dependent synaptic modification
- Functioning artificial neural substrate capable of changing its own synaptic connections

### Phase 2 Implementation Status

#### ✅ COMPLETED
- Real LIF neuron dynamics implemented
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)
- Learning experiment demonstrates measurable changes
- Network shows activity-dependent synaptic modification

#### 🚀 PLANNED FOR PHASE 3-6
- **Phase 3**: Memory systems (working, episodic, semantic)
- **Phase 4**: Cognitive systems (prediction, attention, planning)
- **Phase 5**: Performance optimizations and scaling
- **Phase 6**: Full integration of all systems into coherent brain loop

## Building

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run the Phase 2 demo
./nlm

# Or run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main Phase 2 simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration (planned) |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration (planned) |
| `nlm_test` | Executable | Unit test suite |

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

## Running

### Phase 2 Demo (Current Implementation)

```bash
# Run the Phase 2 demo
./nlm

# With custom configuration
./nlm --config=custom_config.cfg

# With command line overrides
./nlm --neuron_count=500 --random_seed=123
```

This runs a comprehensive integration test verifying all Phase 2 brain systems are connected.

### Phase 2 Demo Features

The Phase 2 demo demonstrates:

1. **Basic Connectivity Test**: Inject current into neurons and verify spike propagation
2. **Plasticity Learning Experiment**: Apply repeated input patterns to stimulate learning
3. **STDP Verification**: Test spike-timing dependent plasticity rules

Each test validates that:
- Neural dynamics work correctly
- Plasticity mechanisms produce measurable changes
- The brain can learn from experience

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── pyproject.toml
├── configs/           # Configuration files
│   └── default.cfg
├── src/              # Source code
│   ├── core/         # Core utilities
│   ├── brain/        # Neural components (Phase 2)
│   ├── dynamics/     # Neural dynamics
│   ├── plasticity/   # Plasticity rules (Phase 2)
│   ├── development/  # Developmental system
│   ├── neuromodulation/# Neuromodulators
│   ├── memory/       # Memory systems (planned for Phase 3)
│   ├── cognition/    # Cognitive mechanisms (planned for Phase 4)
│   ├── sensory/      # Sensory processing
│   ├── motor/        # Motor system
│   ├── environment/  # Environment interface
│   ├── experiments/  # Experiment framework
│   └── visualization/ # Visualization
├── tests/            # Test suite
├── docs/             # Documentation
├── examples/         # Example scripts
└── build/            # Build artifacts (created during build)
```

## Phase Roadmap

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Current: Real Neural Computation)
- Real LIF neuron dynamics ✅
- Event-driven spike propagation ✅
- STDP and Hebbian plasticity ✅
- Structural plasticity ✅

### Phase 3 (Planned: Memory Systems)
- Working memory with persistent activity
- Episodic memory with experience encoding and replay
- Semantic memory
- Associative memory

### Phase 4 (Planned: Cognitive Systems)
- Neural prediction system
- Working memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Planned: Performance Optimizations)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Planned: Final Integration)
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

Phase 2 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 2 DOES investigate:
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

## Contributing

**IMPORTANT**: This project is actively under development. Phase 2 is complete, but major features are planned for future phases.

When contributing:

1. **Phase Alignment**: Ensure your contributions align with the current Phase 2 implementation
2. **Scientific Integrity**: Maintain the project's focus on biologically-inspired neural computation
3. **Documentation**: Update documentation to reflect Phase 2 status accurately
4. **Testing**: Add tests for Phase 2 functionality
5. **Integration**: Design new features with Phase 3-6 integration in mind

### Development Guidelines

- **Code Quality**: Maintain high standards for scientific computation code
- **Performance**: Optimize for realistic neural simulations
- **Extensibility**: Design for future Phase 3-6 integration
- **Testing**: Comprehensive testing of Phase 2 and planned features
- **Documentation**: Accurate documentation of current capabilities

## Community

This is a research project. For technical discussions, please refer to the repository issues and documentation.

The project focuses on understanding how artificial brains can emerge from simple neural components through developmental processes.

---

**Note**: This project is in active development. The documentation may be updated frequently as new phases are completed. Please check the repository for the latest updates and clarifications.

For questions about the current Phase 2 implementation, please refer to the source code comments and examples.