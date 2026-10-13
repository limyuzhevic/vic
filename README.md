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

**PHASE 6: FINAL INTEGRATION** ✅ **COMPLETE**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems ✅
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration ✅
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System ✅
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems ✅
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration ✅
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence ✅
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation ✅
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

#### Phase 6 Demo ✅
- Complete integration test program
- `nlm_phase6_demo` executable
- Comprehensive verification of all systems

## Project Structure (Phase 6 Enhanced)

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
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   ├── visualization/ # Visualization
│   └── performance/   # Performance profiling framework (NEW)
├── tests/             # Unit test framework
│   ├── test_main.cpp
│   ├── test_working_memory.cpp
│   ├── test_episodic_memory.cpp
│   ├── test_associative_memory.cpp
│   └── ...
├── docs/              # Documentation (ENHANCED)
│   └── TEST_OVERVIEW.md      # New test documentation
└── configs/
```

## Building and Running

### Basic Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Performance Profiling
```bash
./performance_demo
```

The performance demo showcases the new profiling framework and benchmarks.

### Running Unit Tests
```bash
./nlm_test
```

Runs all unit tests including:
- Types and configuration tests
- Neuron dynamics tests
- Synapse transmission tests
- Plasticity tests
- Brain integration tests
- **NEW**: Memory system tests (working, episodic, associative)
- **NEW**: Performance profiling tests

## Key Improvements (Phase 6)

### Code Quality Improvements

#### 1. Brain.cpp Refactoring ✅
- **Before**: 1117-line monolithic file
- **After**: Well-organized with clear section headers
- **Benefits**: Improved readability, maintainability, and documentation
- **Documentation**: Comprehensive parameter explanations and usage notes

#### 2. Memory System Documentation ✅
- **Enhanced**: NeuralWorkingMemory.cpp with detailed documentation
- **Features**: Pattern encoding/decoding, competition, decay, maintenance
- **Advanced**: Granular neuron control, strengthening, replay mechanisms

#### 3. Bug Fixes ✅
- **Fixed**: Code duplication in ExperimentRunner.cpp
- **Fixed**: Missing saveResults() method implementation
- **Fixed**: Excessive logging in Phase6IntegratedExperiment.cpp

#### 4. Header Guards ✅
- **All**: All .hpp files have #pragma once guards
- **Consistent**: Uniform header inclusion protection

#### 5. Naming Conventions ✅
- **Standardized**: Consistent camelCase usage throughout
- **Documented**: Clear naming patterns in documentation

### Testing Improvements

#### 1. Comprehensive Unit Tests ✅
- **Existing**: Types, config, random, clock, neuron, synapse, stdp, brain
- **NEW**: Working memory tests (10 test functions)
- **NEW**: Episodic memory tests (13 test functions)
- **NEW**: Associative memory tests (10 test functions)

#### 2. Performance Profiling Framework ✅
- **NEW**: PerformanceProfiler for timing and memory tracking
- **NEW**: PerformanceBenchmarkSuite for component benchmarking
- **NEW**: MemoryProfiler for leak detection
- **NEW**: IntegrationTestProfiler for system-wide profiling
- **NEW**: Performance demo executable

### Advanced Features

#### 1. Working Memory System
```cpp
// Store patterns as neural activity
std::vector<float> pattern = {0.5f, 0.8f, 0.3f};
workingMemory.store(pattern, 1.0f);

// Retrieve stored patterns
std::vector<float> memory = workingMemory.retrieve();

// Maintain active traces with competition
workingMemory.update(timeStep);
```

#### 2. Episodic Memory System
```cpp
// Store experiences with temporal context
EpisodicMemoryItem episode;
episode.sensoryState = {0.7f, 0.9f, 0.2f};
episode.temporalContext = 1000;
episode.locationContext = {5, 5};

episodicMemory.storeEpisode(episode);

// Retrieve similar episodes
auto similar = episodicMemory.retrieveSimilar(sensoryState);
```

#### 3. Associative Memory System
```cpp
// Create pattern associations
associativeMemory.createAssociation(patternA, patternB, strength);

// Retrieve through spreading activation
auto associations = associativeMemory.spreadActivation(pattern);
```

#### 4. Neuromodulation Integration
```cpp
// Dopamine affects learning and plasticity
brain->getDopamine()->setLevel(0.8f);

// Curiosity drives exploration
brain->getCuriosity()->enableExploration(true);

// Novelty detection integrates with attention
brain->getNovelty()->setSensitivity(0.6f);
```

### Performance Optimizations

#### 1. Logging Optimization ✅
- **Reduced**: Logging frequency in experiment loops (1000 → 10000 steps)
- **Configurable**: Logging levels and filtering
- **Performance**: Minimal impact on system performance

#### 2. Memory Management ✅
- **Tracking**: Detailed memory allocation/deallocation tracking
- **Leaks**: Memory leak detection and analysis
- **Efficiency**: Optimized memory pools and event queues

#### 3. CPU Profiling ✅
- **Instructions**: Cycle counting and efficiency metrics
- **Cache**: Cache miss analysis where possible
- **Benchmarks**: Scalable benchmarks (small, medium, large)

## Scientific Background

### Research Focus (Phase 6)

NLM investigates computational principles underlying brain-like systems:

#### Memory Integration
- Whether memory systems can integrate with neural dynamics
- How neuromodulation affects plasticity in coordinated ways
- Whether prediction can become a central organizing principle

#### Developmental Neuroscience
- Whether developmental stages can modulate learning
- How replay reinforces memory consolidation
- Whether the complete brain loop functions coherently

### Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No realistic cortical architecture

## Technical Architecture

### Closed-Loop System
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

### Neural Computation Components

- **Real LIF Neuron Dynamics**: Spike generation and propagation
- **Event-Driven Computation**: Asynchronous spike processing
- **Multiple Plasticity Mechanisms**: STDP, Hebbian, structural
- **Full Developmental Lifecycle**: Growth, maturation, aging
- **Integrated Memory Systems**: Working, episodic, associative
- **Neuromodulation**: Dopamine, curiosity, novelty
- **Predictive Capabilities**: Forward models and error signals

## License

MIT

## Authors

Research project — See docs for scientific background.

## Quick Start

### For Researchers
1. **Build**: `mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4`
2. **Run Demo**: `./nlm_phase6_demo` (verify all systems)
3. **Profile Performance**: `./performance_demo` (identify bottlenecks)
4. **Run Tests**: `./nlm_test` (validate implementations)

### For Developers
1. **Examine Code**: `src/` contains all implementations
2. **Understand Documentation**: `docs/` contains comprehensive documentation
3. **Extend Functionality**: Add new memory types or neuromodulators
4. **Improve Performance**: Use profiling framework to identify bottlenecks

## Contact

For technical questions, bug reports, or collaboration opportunities, please refer to the project documentation and issue tracker.

**Phase 6 Status**: ✅ **COMPLETE - All Systems Integrated**
**Test Coverage**: ✅ **Comprehensive**
**Documentation**: ✅ **Complete**
**Performance**: ✅ **Optimized and Profiled**

The NLM project has successfully completed Phase 6 with all systems integrated, comprehensive testing, and detailed documentation. The framework is ready for research, development, and educational use.