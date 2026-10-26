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

## Python Bindings (pynlm)

NLM includes comprehensive **Python bindings** (`pynlm`) that expose the entire NLM simulation framework through a Pythonic interface. This makes it easy to create, simulate, and study artificial brains without needing to work directly with C++ code.

### Key Features

- **Complete API**: Full access to all NLM brain systems
- **Easy Integration**: Simple Python interface for complex simulations
- **Documentation**: Comprehensive documentation with examples
- **Testing**: Extensive test suite for validation
- **Development Support**: Tools for rapid prototyping and experimentation

### Getting Started

```bash
# Install Python bindings
pip install -e .

# Run Python tests
python test_python_bindings.py
```

```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate for 100 steps
for i in range(100):
    brain.step(i)

print(f"Simulation complete. {brain.getTotalSpikeCount()} spikes generated.")
```

### Python Examples

The `python_docs.md` file contains:
- Complete API documentation
- Usage examples for all major classes
- Configuration parameter details
- Troubleshooting guide
- Best practices
- Advanced usage patterns

### Testing

The `test_python_bindings.py` file provides:
- Unit tests for all Python bindings
- Integration tests for agent-world loops
- Checkpoint functionality tests
- Examples matching documentation patterns

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

### Available Targets

After building, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3` | Static Library | Phase 3 experiments |
| `nlm_phase4` | Static Library | Phase 4 experiments |
| `nlm_phase5` | Static Library | Phase 5 experiments |
| `nlm_phase6` | Static Library | Phase 6 integration |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase5_demo` | Executable | Phase 5 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 integration demo |
| `nlm_phase6_test` | Executable | Phase 6 integration tests |

## Running

### Phase 6 Demo (Integration Test)

```bash
# Run the comprehensive Phase 6 integration test
./nlm_phase6_demo

# Run specific Phase 6 tests
./nlm_phase6_test
```

This runs a comprehensive integration test verifying all brain systems are properly connected and working together.

### Python Examples

```bash
# Run Python test suite to validate bindings
python test_python_bindings.py

# View Python documentation
open python_docs.md
```

## Project Structure

```
NLM/
├── CMakeLists.txt              # Build configuration
├── README.md                   # This file
├── python_docs.md              # Python API documentation
├── test_python_bindings.py     # Python test suite
├── docs/                      # Documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md
├── src/                       # Source code
│   ├── core/                 # Core utilities
│   ├── brain/                # Neural components (integrated)
│   ├── dynamics/             # Neural dynamics
│   ├── plasticity/           # Plasticity rules
│   ├── development/          # Developmental system
│   ├── neuromodulation/       # Neuromodulators (integrated)
│   ├── memory/               # Memory systems (integrated)
│   ├── prediction/           # Prediction systems (integrated)
│   ├── cognition/            # Cognitive mechanisms (integrated)
│   ├── sensory/              # Sensory processing
│   ├── motor/                # Motor system
│   ├── environment/          # Environment interface
│   ├── experiments/          # Experiment framework (Phase 6)
│   └── visualization/        # Visualization
├── tests/                     # C++ unit test suite
├── python/                    # Python bindings source
│   └── bindings.cpp           # C++ bindings implementation
├── configs/                   # Configuration files
└── easy_usage.md              # Beginner-friendly usage guide
└── HOW_TO_USE.md              # Comprehensive user guide
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
- **Python bindings completed**
- **Comprehensive documentation created**

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

## Key Improvements in Phase 6

### Integration Achievements

1. **Memory Integration**: All memory systems (working, episodic, associative) are now fully integrated and operational
2. **Neuromodulation**: Dopamine, curiosity, and novelty systems work together to coordinate learning and behavior
3. **Prediction System**: Prediction error drives attention, learning, and action selection
4. **Cognition**: Neural planner, concept formation, and self-model systems are integrated
5. **Development**: Comprehensive developmental stages modulate multiple system rates
6. **Persistence**: Full checkpoint system enables saving and loading of brain states
7. **Replay**: Memory replay mechanisms support consolidation

### User Experience Improvements

1. **Python Bindings**: Complete Python API for easy access to NLM functionality
2. **Documentation**: Comprehensive documentation with examples and troubleshooting
3. **Testing**: Extensive test suites for both C++ and Python APIs
4. **Build System**: Complete integration of Phase 6 targets into CMake
5. **Command-line Tools**: Multiple demonstration executables for different phases

### Technical Improvements

1. **Phase 6 Build Targets**: All Phase 6 experiments and demos are now properly integrated into the build system
2. **Integration Tests**: Comprehensive test suite for verifying system integration
3. **Code Quality**: Resolved documentation discrepancies and improved code organization
4. **Performance**: Optimized hot paths and integrated performance infrastructure

## Python for Researchers and Developers

The Python bindings make NLM particularly valuable for:

- **Research**: Easy experimentation with different brain configurations
- **Education**: Teaching computational neuroscience concepts
- **Prototyping**: Rapid development of new brain architectures
- **Integration**: Combining NLM with other Python scientific tools
- **Testing**: Automated testing and validation of brain simulations

## Getting Help

### Documentation

- `python_docs.md` - Complete Python API documentation
- `HOW_TO_USE.md` - Comprehensive user guide for all NLM features
- `easy_usage.md` - Beginner-friendly quick start guide

### Testing and Validation

- `test_python_bindings.py` - Python binding test suite
- `tests/` - C++ unit test suite

### Online Resources

- Project documentation in `docs/` directory
- GitHub repository with additional examples
- Issue tracker for bug reports and feature requests

## License

MIT License. See LICENSE file for details.

## Authors

Research project — See docs for scientific background.

---

**Phase 6 represents the culmination of years of development, creating a coherent artificial brain system that integrates memory, neuromodulation, prediction, cognition, and development into a single functional unit.**

The NLM project continues to evolve, with Python bindings and improved user experience making it more accessible to researchers, educators, and developers working in computational neuroscience and artificial intelligence.
