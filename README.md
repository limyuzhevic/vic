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

### Key Improvements in Phase 6 Enhancement

#### 1. Enhanced Memory Management
- Fixed critical memory management bugs in MemoryPool implementations
- Added comprehensive memory tracking and leak detection
- Implemented robust slab allocation with thread safety

#### 2. Improved Performance
- Optimized SIMD integration with enhanced AVX-512, AVX2, and SSE implementations
- Added performance profiling tools for micro-optimization
- Implemented memory-efficient serialization with pooling

#### 3. Advanced Debugging & Profiling
- Added comprehensive debugging and profiling tools:
  - PerformanceProfiler: Time critical sections and operations
  - MemoryTracker: Detect leaks and analyze memory patterns
  - TimelineProfiler: Track event sequences and dependencies
  - StackTraceCapture: Capture stack traces for crash analysis
- Enhanced logging with context information and thread awareness

#### 4. Robust Checkpoint System
- Enhanced checkpoint system with transactional integrity
- Added incremental checkpointing with delta compression
- Implemented automatic recovery with rollback capability
- Added comprehensive checkpoint validation and verification

#### 5. Advanced Command Line Interface
- Created NLM Advanced CLI with comprehensive commands:
  - Brain management (create, load, save, delete)
  - Debug and profiling controls
  - Configuration management
  - Simulation control
  - Checkpoint operations

#### 6. Code Quality Improvements
- Fixed memory management bugs in critical paths
- Improved SIMD code organization and readability
- Enhanced error reporting and recovery mechanisms
- Added comprehensive API documentation

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

## Advanced Features

### 1. Enhanced Command Line Interface (CLI)

The NLM framework now includes an advanced Command Line Interface (CLI) for comprehensive system management. To use the CLI:

```bash
# Build the CLI
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run the CLI
./advanced_cli
```

**Available Commands:**

- **create <neurons> <regions>** - Create a new brain with specified parameters
- **load <path>** - Load a saved brain checkpoint
- **save <path>** - Save current brain state to checkpoint
- **delete <path>** - Delete a checkpoint
- **list [directory]** - List available checkpoints
- **debug [start|stop]** - Enable/disable memory tracking
- **profile <start|stop> <name>** - Profile specific operations
- **timeline** - Show recorded timeline events
- **memory** - Display memory statistics and leak detection
- **benchmark [name]** - Run performance benchmarks
- **run** - Execute Phase 6 integration experiment
- **step** - Execute one simulation step
- **config <key> <value>** - Set configuration values
- **showconfig** - Display current configuration
- **verify** - Verify brain integration
- **validate <path>** - Validate checkpoint integrity
- **help** - Show available commands
- **quit/exit** - Exit the CLI

### 2. Advanced Debugging and Profiling

#### PerformanceProfiler
```cpp
PerformanceProfiler profiler("operation_name");
profiler.start();
// ... perform operation ...
profiler.stop();
```

#### MemoryTracker
```cpp
MemoryTracker& tracker = getGlobalMemoryTracker();
// Memory tracking is automatically enabled
// Check for leaks:
if (tracker.hasLeaks()) {
    auto leaks = tracker.getLeakInfo();
    // Report leaks
}
```

#### TimelineProfiler
```cpp
TimelineProfiler& timeline = getGlobalTimelineProfiler();
timeline.startEvent("phase_1");
timeline.endEvent("phase_1");
timeline.saveToFile("timeline.json");
```

### 3. Enhanced Checkpoint System

The checkpoint system now supports:

- **Atomic Saves**: Transactional checkpoint writes with rollback capability
- **Incremental Checkpoints**: Efficient delta-based saves
- **Automatic Recovery**: Attempt recovery from corrupted checkpoints
- **Validation**: Comprehensive integrity checking
- **Parallel Operations**: Multiple checkpoints can be managed simultaneously

**Checkpoint Commands:**
- `save <path>` - Save current brain state
- `load <path>` - Load a saved brain
- `list [dir]` - List available checkpoints
- `validate <path>` - Validate checkpoint integrity
- `delete <path>` - Delete checkpoints

### 4. Integration Testing

The Phase 6 integration test now includes comprehensive verification:

- Memory system integration (working memory, episodic memory)
- Neuromodulation system integration (dopamine, curiosity, novelty)
- Prediction system integration
- Development system integration
- Checkpoint save/load verification
- Replay system functionality

### 5. Performance Optimizations

Key performance improvements include:

- **SIMD Optimization**: Enhanced vectorized operations for neural computation
- **Memory Pooling**: Efficient allocation/deallocation with pre-allocation
- **Event-Driven Processing**: Only active neurons consume computation
- **Sparse Connectivity**: Memory-efficient adjacency lists
- **Multithreading**: Parallel population updates with work-stealing

### 6. API Documentation

The NLM framework now includes comprehensive API documentation:

- **Core Components**: Brain, Neuron, Synapse, Memory Systems
- **Performance Tools**: SIMD utilities, memory pools, profiling
- **Integration Features**: Checkpoint system, CLI, debugging
- **Configuration**: Detailed configuration options and parameters

### 7. Building and Installation

**Build Instructions:**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

**Installation:**
```bash
sudo make install
```

**Running Tests:**
```bash
# Run Phase 6 integration demo
./nlm_phase6_demo

# Run advanced CLI
./advanced_cli
```

## License

MIT

## Authors

Research project — See docs for scientific background.
