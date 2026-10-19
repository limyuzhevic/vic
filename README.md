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

## Quick Start

### For Users
1. **Build from source**:
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)
   ./nlm
   ```

2. **Use Python bindings**:
   ```bash
   pip install -e .
   ```

3. **Run basic tests**:
   ```bash
   ./nlm --help  # See available options
   ./nlm_phase3_demo  # Run Phase 3 demo
   ```

### For Contributors
1. **Fork and clone** the repository
2. **Run tests** to ensure everything works:
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   make -j$(nproc)
   ctest --output-on-failure
   ```

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

### C++ Project (Recommended for Development)

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets (using all available CPU cores)
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Python Package (User-Friendly)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install in development mode
pip install -e .

# Or install from source
pip install .
```

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

### Main Simulation
```bash
./nlm [--config configs/your_config.cfg]
```

### Phase 3 Demo
```bash
./nlm_phase3_demo
```

### Phase 4 Demo
```bash
./nlm_phase4_demo
```

## Project Structure

```
NLM/
├── CMakeLists.txt                    # Main build configuration
├── README.md                          # This file
├── pyproject.toml                     # Python package configuration
├── docs/                              # Documentation
│   ├── ARCHITECTURE.md                # System architecture overview
│   ├── SCIENCE.md                     # Scientific background
│   ├── ROADMAP.md                     # Development roadmap
│   ├── EXPERIMENTS.md                 # Experiment descriptions
│   ├── LIMITATIONS.md                 # Current limitations
│   └── PHASE6_FINAL_AUDIT.md          # Phase 6 audit report
├── src/                               # Source code
│   ├── core/                          # Core utilities and types
│   ├── brain/                         # Neural components (integrated)
│   ├── dynamics/                      # Neural dynamics
│   ├── plasticity/                    # Plasticity rules
│   ├── development/                   # Developmental system
│   ├── neuromodulation/               # Neuromodulators (integrated)
│   ├── memory/                        # Memory systems (integrated)
│   ├── prediction/                    # Prediction systems (integrated)
│   ├── cognition/                     # Cognitive mechanisms (integrated)
│   ├── sensory/                       # Sensory processing
│   ├── motor/                         # Motor system
│   ├── environment/                   # Environment interface
│   ├── experiments/                   # Experiment framework (Phase 6)
│   └── visualization/                 # Visualization
├── tests/                             # Test suite
│   ├── CMakeLists.txt                 # Test build configuration
│   └── ...                            # Test files
├── configs/                           # Configuration files
├── python/                            # Python bindings
│   ├── bindings.cpp                   # Pybind11 bindings
│   └── CMakeLists.txt                 # Python bindings build
├── .kilo/                             # Kilo development framework
└── ...                                # Other project files
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

## Getting Help

- **Documentation**: See `docs/ARCHITECTURE.md` and `docs/SCIENCE.md`
- **Python Usage**: See `HOW_TO_USE.md` and `easy_usage.md`
- **Issue Tracking**: Report issues on GitHub
- **Development**: See `.kilo/` directory for development tools

## Quick Contributing Guide

1. **Read the documentation** first - understand the architecture
2. **Run tests** before making changes
3. **Follow the code style** - modern C++20 with clear comments
4. **Update documentation** when adding new features
5. **Write tests** for new functionality
6. **Run benchmarks** when optimizing performance

## Development Commands

The project includes Kilo development framework for enhanced development experience:

```bash
# Check available Kilo commands
.kilo/command/*.md

# Run development workflows
.kilo/agent/*.md

# Check agent manager
AGENTS.md
```

## Future Improvements

Based on Phase 6 analysis, future development focuses on:
- Enhanced Python API with more advanced features
- Better configuration validation and schema
- Plugin architecture for extensibility
- Improved debugging and visualization tools
- Performance optimizations for large-scale simulations
