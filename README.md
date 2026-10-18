# NLM — 熙然

**An Experimental Artificial Developmental Brain**

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

### Prerequisites
- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# For C++ core library
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# For Python bindings
pip install pybind11 scikit-build-core
pip install -e .
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable (Phase 2) |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 integration test |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `pynlm` | Python Module | Python bindings |

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

### C++ Demos

#### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Other Phase Demos
- `./nlm` - Basic neural computation and plasticity demonstration (Phase 2)
- `./nlm_phase3_demo` - World interaction demonstration (Phase 3)
- `./nlm_phase4_demo` - Emerging cognition demonstration (Phase 4)
- `./nlm_phase6_demo` - Complete integration test (Phase 6)

### Python Library

#### Installation
```bash
# Install with scikit-build-core (recommended for development)
pip install -e .

# Or install from source
pip install build
python -m build
```

#### Basic Usage
```python
import pynlm

# Create a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
    
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

#### Advanced Usage Examples
The Python bindings now include:

1. **Memory System Access**: 
   - Working memory, episodic memory, and associative memory access
   - All memory systems are accessible through the Brain class

2. **Prediction System**: 
   - Access to neural prediction system
   - Prediction error signal tracking

3. **Cognition Systems**:
   - Neural planner for action planning
   - Concept formation for pattern discovery
   - Attention system for selective focus

4. **Neuromodulation Systems**:
   - Dopamine system for reward and reinforcement
   - Curiosity system for exploration motivation
   - Novelty system for novelty detection
   - Prediction error signal system

5. **Development System**:
   - Access to developmental processes
   - Plasticity rate modulation by age

6. **World Methods**:
   - Full world control and observation capabilities
   - Agent positioning and movement
   - World object management

7. **AgentBrain Methods**:
   - Complete agent interface for brain-world interaction
   - Sensory processing and motor decoding
   - Neuromodulation control
   - Developmental tracking

8. **Region Management**:
   - Brain region creation and management
   - Inter-region connection control
   - Region statistics and information

9. **Enhanced Error Handling**:
   - Better error messages and exception handling
   - Input validation and parameter checking
   - Graceful failure modes

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── pyproject.toml (Python package configuration)
├── test_pynlm.py (comprehensive Python test suite)
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
├── tests/             # C++ tests
├── python/            # Python bindings source
│   ├── bindings.cpp    # Updated with comprehensive Python API
│   └── CMakeLists.txt  # Python bindings build configuration
└── configs/
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete - Basic Neural Computation)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity
- Basic connectivity and learning demonstration

### Phase 3 (Complete - World Interaction)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity
- Agent world interaction demonstration

### Phase 4 (Complete - Emerging Cognition)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning
- Comprehensive cognition experiments

### Phase 5 (Complete - Lifetime Integration)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system
- Multi-seed experiments
- Damage recovery experiments
- Scaling experiments

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and neural dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created
- Comprehensive integration verification

## Python API Improvements

The Python bindings (pynlm) now provide comprehensive access to the NLM C++ framework:

### New Methods Added

#### Brain Class Extensions
- **Memory Systems**: `getWorkingMemory()`, `getEpisodicMemory()`, `getAssociativeMemory()`
- **Prediction System**: `getPredictionSystem()`
- **Cognition Systems**: `getPlanner()`, `getConceptFormation()`, `getAttention()`
- **Development System**: `getDevelopmentSystem()`
- **Neuromodulation Systems**: `getDopamine()`, `getCuriosity()`, `getNovelty()`, `getPredictionErrorSignal()`
- **Random Generator**: `getRandomGenerator()`
- **Inter-Region Connections**: `addInterRegionConnection()`, `removeInterRegionConnection()`

#### SimpleWorld Class Extensions
- **Vision Configuration**: `getVisionWidth()`, `getVisionHeight()`
- **Object Access**: `getObjectAt(x, y)`
- **Full Energy System**: Enhanced energy management methods
- **Complete World Control**: All world manipulation capabilities

#### AgentBrain Class Enhancements
- **Enhanced Neuromodulation**: Improved control over all neuromodulation subsystems
- **Advanced Development Tracking**: Better access to developmental processes
- **Complete Sensorimotor Interface**: Full agent functionality

### Testing

A comprehensive test suite (`test_pynlm.py`) has been added to verify:
- Basic functionality (brain creation, stepping, etc.)
- World interaction and control
- AgentBrain functionality
- Memory system access
- Prediction and cognition systems
- Neuromodulation systems
- Development system
- Save/load functionality
- Region management

All tests can be run with:
```bash
python test_pynlm.py
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

## Quick Start

For beginners, we recommend:

1. **Read the easy_usage.md** - For a quick introduction to NLM
2. **Run the Python examples** - Basic functionality is tested in test_pynlm.py
3. **Explore the C++ demos** - Run `./nlm_phase6_demo` for a complete integration test
4. **Read the documentation** - Check docs/ for detailed technical information

The NLM project provides a comprehensive framework for exploring artificial developmental brains and neural computation.