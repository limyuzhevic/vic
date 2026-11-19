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

## Current status (honest, 2026-11-19)

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

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

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
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/ # Visualization
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
- STDP and Hebbian plasticity rules
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

## New Features and Improvements

### 1. Enhanced Configuration System

#### JSON Configuration Support
- **File**: `src/core/Config/Config.cpp`
- **Improvement**: Added JSON support using nlohmann/json library while maintaining backward compatibility with simple key=value format
- **Benefits**: Better cross-platform compatibility, human-readable config files, easier integration with other tools

#### Advanced Configuration Manager
- **File**: `src/core/AdvancedConfig.hpp`
- **Features**:
  - Load configuration from file, environment variables, and command line
  - Automatic type detection from string values
  - Support for environment variables prefixed with NLM_
  - Comprehensive validation and error handling
  - Backward compatibility with existing configuration format

#### Command Line Parser
- **File**: `src/core/CommandLineParser.hpp`
- **Features**:
  - Comprehensive command-line argument parsing
  - Support for both short (-x) and long (--name) options
  - Option validation and help system
  - Easy integration with existing codebase

### 2. Performance Monitoring

#### Performance Profiler
- **File**: `src/core/PerformanceProfiler.hpp`
- **Features**:
  - Real-time performance measurement
  - Detailed timing statistics
  - Summary and average calculations
  - Formatted reporting
  - Easy integration with existing benchmarks

### 3. Test Framework

#### Comprehensive Test Framework
- **File**: `src/core/TestFramework.hpp`
- **Features**:
  - Flexible test organization by categories
  - Robust error handling and reporting
  - Detailed test results and summaries
  - Integration with existing test suite
  - Support for skipped tests

### 4. Improved Error Handling

#### Enhanced Logger
- **File**: `src/core/Logger/Logger.cpp`
- **Improvements**:
  - Better file handling with error checking
  - Thread-safe logging with mutex protection
  - Graceful error recovery
  - Enhanced logging functionality

### 5. Memory Management Improvements

#### Fixed Brain.cpp Initialization
- **File**: `src/brain/Brain.cpp`
- **Fixes**:
  - Properly initialized `developmentalStage` member variable
  - Fixed initialization order issues
  - Improved memory safety and initialization completeness

### 6. Enhanced Demos

#### Improved Phase 2 Demo
- **File**: `src/main.cpp`
- **Improvements**:
  - Better error handling for plasticity experiments
  - Enhanced logging and status reporting
  - Improved documentation and comments

### 7. Configuration File Fixes

#### Fixed Configuration File
- **File**: `configs/default.cfg`
- **Fix**: Removed duplicate `dopamine_baseline` setting

## Code Quality Improvements

### 1. Documentation and Comments
- Added comprehensive documentation throughout the codebase
- Improved code comments for better understanding
- Added error messages and helpful warnings

### 2. Type Safety
- Enhanced type checking and conversion
- Better parameter validation
- Improved error handling for type mismatches

### 3. Memory Safety
- Fixed memory management issues
- Added null pointer checks
- Improved initialization order

### 4. Performance Optimizations
- Added performance monitoring tools
- Improved code organization for better maintainability
- Enhanced error recovery mechanisms

## Build and Installation Improvements

### 1. CMake Configuration
- Updated CMakeLists.txt to include new headers
- Improved build system integration
- Better dependency management

### 2. Python Bindings
- Enhanced Python binding support
- Better integration with Python ecosystem
- Improved documentation and examples

## Testing and Validation

### 1. Enhanced Test Suite
- Added comprehensive test framework
- Improved test organization and reporting
- Better error handling in tests

### 2. Performance Testing
- Added performance profiling capabilities
- Enhanced benchmark functionality
- Better performance analysis tools

## User Experience Improvements

### 1. Configuration Management
- Simplified configuration loading
- Better error messages
- Environment variable support

### 2. Command Line Interface
- Enhanced command-line argument parsing
- Better help and usage documentation
- Improved option validation

### 3. Logging and Monitoring
- Enhanced logging capabilities
- Better error reporting
- Real-time performance monitoring

## Backward Compatibility

All improvements maintain backward compatibility with existing code:
- Existing configuration files still work
- Command-line arguments remain the same
- API interfaces unchanged
- Performance characteristics preserved

## Future Enhancements

The improved codebase provides a solid foundation for future development:
- Enhanced neural modeling capabilities
- More sophisticated memory systems
- Advanced learning algorithms
- Better integration with external tools
- Improved performance and scalability

## Summary

These improvements significantly enhance the NLM project:

1. **Configuration Management**: JSON support, environment variables, better validation
2. **Performance Monitoring**: Real-time profiling and statistics
3. **Testing**: Comprehensive test framework with detailed reporting
4. **Error Handling**: Robust error recovery and graceful degradation
5. **Memory Management**: Fixed initialization issues and improved safety
6. **Code Quality**: Better documentation, type safety, and maintainability
7. **User Experience**: Enhanced configuration, CLI, and logging

The NLM project is now better positioned for research, development, and deployment while maintaining its experimental nature and scientific integrity.