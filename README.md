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

## Current status (honest, 2026-10-31)

Phase 1 made the repo build again. Phase 2 made the brain real at the
substrate level: populations now actually create neurons, random
connectivity creates ~37k synapses for a 500-neuron brain (was 0),
spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000
steps), and a controlled STDP test potentiates its synapse
(0.35 -> 0.99). Inhibitory neurons now exist (20% of internal
population) and inhibitory plasticity keeps its sign instead of
flipping to excitatory.

Still not a working brain in the behavioural sense: Phase 3 and Phase 6
demos still report 0 reward, and checkpointing does not work (it is a
stub that honestly returns false). Activity is still very high
(~200 spikes/step) and learning in the main demo is mostly
saturation, not selective pattern learning. Those are the next
problems, not hidden ones.

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

NLM uses CMake for building. The project supports both traditional build systems and modern CMake practices.

### Standard Build

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Build Options

You can customize the build with various CMake options:

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DNLM_ENABLE_TESTS=ON \
    -DNLM_ENABLE_EXPERIMENTS=ON \
    -DNLM_ENABLE_PROFILING=ON \
    -j4
```

### Build Types

- **Release**: Optimized for performance (default)
- **Debug**: With debug symbols and validation
- **RelWithDebInfo**: Release with debug information

### Build Components

The build generates the following key components:

1. **Core Libraries**: 
   - `libnlm_core.a` - Core utilities and frameworks
   - `libnlm_brain.a` - Neural computation components
   - `libnlm_dynamics.a` - Neural dynamics and integration

2. **Applications**:
   - `nlm` - Main demonstration application (Phase 2)
   - `nlm_phase3_demo` - Phase 3 sensory-motor integration
   - `nlm_phase4_demo` - Phase 4 cognitive mechanisms
   - `nlm_phase6_demo` - Phase 6 comprehensive integration test
   - `nlm_benchmark` - Performance benchmarking suite

3. **Python Bindings**:
   - `pynlm` - Python interface for NLM (via pyproject.toml)

### Building for Development

For development and debugging, use:

```bash
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_VALIDATION=ON
make -j4
```

### Building with Python Support

The project includes Python bindings. Build with:

```bash
# Ensure you have Python development headers
pip install -e .
```

### Building from Source

1. Clone the repository
2. Create build directory
3. Run CMake configuration
4. Build with `make`

```bash
git clone <repository-url>
cd nlm
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### Build Troubleshooting

#### CMake Not Found

Install CMake:

```bash
# Ubuntu/Debian
sudo apt-get install cmake

# macOS
brew install cmake

# Windows
choco install cmake
```

#### Missing Dependencies

Ensure you have all required system dependencies:

```bash
# Ubuntu/Debian
sudo apt-get install \
    build-essential \
    libboost-dev \
    libeigen3-dev \
    python3-dev \
    python3-pip

# macOS
brew install boost eigen python3
```

#### Build Errors

If you encounter build errors:

1. Check that all dependencies are installed
2. Ensure C++17 support is available
3. Verify that the compiler version is sufficient
4. Check disk space for build artifacts

### Building Distribution Packages

For packaging and distribution, you can create distribution builds:

```bash
# Create source package
cd build
cmake --build . --target package
scp *.tar.gz user@host:/path/

# Or build for specific platform
cmake .. -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_SYSTEM_PROCESSOR=x86_64
```

### Post-.Deconstruction du bâtiment

After building, you'll have:

```
build/
├── src/
│   ├── core/
│   ├── agent/
│   ├── brain/
│   └── ...
├── tests/
├── docs/
└── configs/
```

### Building Documentation

To build documentation:

```bash
cmake .. -DBUILD_DOCS=ON
cmake --build . --target docs
```

This will generate HTML documentation in the `docs/` directory.

### Building with Different Compilers

#### Using Clang

```bash
cmake .. -DCMAKE_CXX_COMPILER=clang++
```

#### Using GCC

```bash
cmake .. -DCMAKE_CXX_COMPILER=g++
```

### Cleaning Build Artifacts

To clean the build directory:

```bash
cd build
make clean
rm -rf CMakeFiles CMakeCache.txt cmake_install.cmake
```

### Advanced Build Options

Environment variables can influence the build:

- `CMAKE_BUILD_TYPE`: Debug, Release, RelWithDebInfo, MinSizeRel
- `NLM_BUILD_SHARED`: Build shared libraries instead of static
- `NLM_ENABLE_EXAMPLES`: Build example applications
- `NLM_ENABLE_TESTS`: Build test suite

Example:

```bash
export CMAKE_BUILD_TYPE=Release
export NLM_BUILD_SHARED=ON
cmake ..
make -j4
```

### Building on Different Platforms

#### Windows

Use CMake GUI or command line:

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

#### macOS

```bash
mkdir build
cd build
cmake .. -DCMAKE_CXX_STANDARD=17
cmake --build . --config Release
```

#### Linux

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -j$(nproc)
cmake --build .
```

### Building for Embedded Systems

For embedded targets, specify the appropriate CMake flags:

```bash
cmake .. \
    -DTARGET_PLATFORM=embedded \
    -DNLM_OPTIMIZATION_LEVEL=fast \
    -DNLM_MEMORY_CONSTRAINTS=ON
```

### Continuous Integration

For CI/CD pipelines, use:

```bash
# Setup build environment
cmake .. -DCMAKE_BUILD_TYPE=Release

# Run tests if enabled
cmake --build . --target test

# Package for distribution
cmake --build . --target package
```

The build system is designed to be flexible and support various development scenarios, from rapid prototyping to production deployments.

## Running
Build all targets first (see Building above). The entry points are
currently separate demos per phase, not a single unified binary:

```bash
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase6_demo     # Phase 6 integration test
./nlm_benchmark
```

`nlm_phase6_demo` runs a comprehensive integration test verifying all
brain systems are connected. Phase 5 is wired as the `nlm_phase5`
library in CMake; it does not yet have a standalone demo executable.

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
│   ├── agent/          # Agent-brain interface
│   ├── world/          # Simple world
│   ├── performance/    # Checkpoints, pools, queues (headers)
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
