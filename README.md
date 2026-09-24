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
├── configs/
└── docs/UPGRADES.md      # This file - improvements and bug fixes
```

## Recent Improvements and Bug Fixes

This document describes the key improvements and bug fixes made to the NLM Phase 6 (Final Integration) project.

### 1. Enhanced Configuration Management (main.cpp)

**Fixed:**
- Improved command-line argument parsing with better error handling
- Added comprehensive flag support for all configuration options
- Fixed config file override logic to prevent overwriting file settings unnecessarily
- Added proper warning messages for missing configuration files
- Enhanced help documentation with detailed usage examples

**Changes Made:**
- `main.cpp`: Added robust argument parsing with `--help`, `--version`, and comprehensive configuration options
- Added error handling for invalid command-line arguments
- Improved config loading precedence: file → command line args → defaults

### 2. Fixed Synapse Loading Bug (Brain.cpp)

**Fixed:**
- **Critical Bug:** Synapse loading from checkpoint files was not actually restoring synaptic weights or connectivity
- The save/load functionality was broken, preventing brain state persistence

**Changes Made:**
- `Brain.cpp`: Enhanced the `load()` method to properly recreate synapses from checkpoint data
- Added synapse weight restoration, delay preservation, and plasticity flag recreation
- Fixed synapse recreation logic to ensure proper connectivity in loaded brain states
- Added comprehensive synapse count validation and logging

### 3. Improved Memory System Initialization (Brain.cpp)

**Enhanced:**
- Added proper null pointer checks for all memory system initialization
- Added comprehensive error logging for failed memory system initialization
- Improved initialization sequence with better error reporting

**Changes Made:**
- `Brain.cpp`: Enhanced `initialize()` method with error handling for:
  - Working memory initialization
  - Episodic memory initialization
  - Associative memory initialization
  - Neural planner initialization
  - Concept formation initialization
  - Attentional selection initialization
  - Novelty detection and curiosity system initialization

### 4. Enhanced Unit Testing (tests/)

**Added:**
- New test case for brain save/load functionality
- Improved test coverage for persistent memory functionality
- Added comprehensive error handling in test runner

**Changes Made:**
- `tests/test_brain.cpp`: Added `testBrainSaveLoad()` test case
- Updated `tests/test_main.cpp` with proper forward declarations
- Enhanced test runner error handling with detailed error reporting

### 5. Improved Error Handling and Logging

**Enhanced:**
- Added comprehensive error handling throughout main simulation loop
- Improved error messages with specific failure details
- Enhanced exception handling for graceful error recovery

**Changes Made:**
- `main.cpp`: Added try-catch blocks around simulation tests
- Added error reporting for failed test execution
- Improved logging for debugging and monitoring

## Build Instructions

### Standard Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Build Tests
```bash
cd build
make nlm_test
```

### Run Tests
```bash
cd build
./nlm_test
```

### Run Main Demo
```bash
./nlm_phase6_demo
```

## Testing Improvements

The test suite has been enhanced with:

1. **Comprehensive Error Handling**: All tests now catch and report exceptions gracefully
2. **Checkpoint Testing**: New test verifies brain state persistence
3. **Integration Testing**: Tests verify all major systems work together
4. **Performance Validation**: Tests verify memory and performance characteristics

## Configuration Options

The demo now supports extensive command-line configuration:

```bash
nlm_phase6_demo [OPTIONS]

Options:
  --help, -h                 Show this help message
  --version                  Show version information
  --config[=FILE]            Load configuration from file
  --seed[=VALUE]             Set random seed
  --steps[=VALUE]            Set number of simulation steps
  --neurons[=VALUE]          Set number of neurons
  --regions[=VALUE]          Set number of regions
  --connection-prob[=VALUE]  Set connection probability
  --timestep[=VALUE]         Set simulation timestep
  --full-experiment          Run full experimental mode
```

## Known Limitations

1. **Checkpoint Performance**: Loading large brain states can be slow
2. **Memory Usage**: Large neural networks consume significant memory
3. **Development Features**: Some developmental stages may require longer simulation times

## Future Enhancements

Consider the following potential improvements:

1. **Advanced Configuration**: YAML/JSON configuration files
2. **Parallel Processing**: SIMD optimization for faster computation
3. **Distributed Computing**: Multi-node brain simulation
4. **Visualization Tools**: Real-time brain state visualization
5. **Extensible Architecture**: Plugin system for custom brain modules

## License

MIT License

## Authors

Research project — See docs for scientific background.
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

## Technical Documentation

For detailed information about code improvements, bug fixes, and architectural changes, please refer to:

- **UPGRADES.md** - Comprehensive documentation of all improvements and bug fixes
- **docs/UPGRADES.md** - Full technical details and implementation notes
- **docs/PHASE6_FINAL_AUDIT.md** - Original Phase 6 audit findings

## For Developers

### Understanding the Improvements

This project has been significantly enhanced with the following key improvements:

1. **Configuration System**: Robust argument parsing and configuration management
2. **Synapse Loading**: Fixed critical bug in brain state persistence
3. **Memory Systems**: Improved initialization with proper error handling
4. **Testing**: Comprehensive test suite including save/load validation
5. **Error Handling**: Graceful failure recovery and detailed error reporting
6. **Documentation**: Complete technical documentation and upgrade history

### Building and Running

The project follows standard CMake build procedures. For the latest build instructions and improvements, see the UPGRADES.md file.

### Code Quality

- **Maintainability**: Clear error handling and documentation
- **Reliability**: Comprehensive testing and validation
- **Performance**: Optimized for real-time neural simulation
- **Compatibility**: Fully backward compatible with existing code

## Research and Development

This project represents Phase 6: Final Integration of the NLM (Neural Learning Machine) research initiative. It investigates whether memory systems, neuromodulation, prediction, and development can integrate to form a coherent artificial brain system.

### Key Research Questions Addressed

1. **Memory Integration**: Can working, episodic, and associative memories integrate with neural dynamics?
2. **Neuromodulation**: Can dopamine, curiosity, and novelty detection coordinate plasticity?
3. **Prediction Processing**: Can predictive coding become a central organizing principle?
4. **Developmental Effects**: Can developmental stages modulate learning rates?
5. **Replay Benefits**: Can memory replay during rest reinforce consolidation?

## Performance Characteristics

### System Specifications
- **Neural Scale**: Up to 1000+ neurons
- **Simulation Speed**: Real-time capable with optimized algorithms
- **Memory Usage**: Efficient use of event-driven processing
- **Persistence**: Checkpoint saving for long-running simulations

### Expected Performance
- **Small Networks** (100-500 neurons): < 1 second/step
- **Medium Networks** (500-1000 neurons): 1-2 seconds/step
- **Large Networks** (1000+ neurons): 2-5 seconds/step

## License

MIT License

## Copyright Notice

© 2026 NLM Research Team

For more information about the NLM research project, visit the project documentation or contact the research team.
