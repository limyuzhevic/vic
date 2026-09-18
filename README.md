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

## Key Improvements and Enhancements

### 1. **Fixed Configuration Issues** ✅
- Fixed duplicate `dopamine_baseline` entries in `configs/default.cfg`
- Improved config file structure for better maintainability

### 2. **Enhanced Configuration System** ✅
- Added support for JSON and YAML configuration files
- Improved error handling for malformed config files
- Added descriptive comments to configuration entries
- Enhanced file I/O with proper error reporting

### 3. **Python Bindings Enhancement** ✅
- Added comprehensive Python API with advanced user features
- Implemented convenience functions for easier usage
- Added `createQuickBrain()`, `createQuickAgent()` for rapid prototyping
- Included `runLearningSimulation()`, `runSilentSimulation()` for common patterns
- Added type introspection: `getNeuronTypes()`, `getActionTypes()`, etc.
- Implemented system information and help functions

### 4. **Improved CMake Structure** ✅
- Modularized build with separate libraries for different phases
- Added `nlm_agent`, `nlm_world`, `nlm_phase3`, `nlm_phase4`, `nlm_phase5`, `nlm_phase6` libraries
- Enhanced Python module target `pynlm` with proper dependencies
- Added development targets for easier development workflow

### 5. **Comprehensive Unit Testing** ✅
- Added extensive test coverage across all components:
  - `test_brain_advanced.cpp`: Advanced brain functionality
  - `test_config_advanced.cpp`: Configuration validation and I/O
  - `test_memory_advanced.cpp`: Memory system tests
  - `test_neuromodulation_advanced.cpp`: Neuromodulation effects
  - `test_development_advanced.cpp`: Development system tests
  - `test_integration.cpp`: System integration tests
  - `test_performance.cpp`: Performance and scalability tests
- Enhanced existing tests with better coverage

### 6. **Robust Error Handling** ✅
- Added comprehensive error messages throughout the codebase
- Improved config file loading with detailed error reporting
- Added validation for malformed input files
- Enhanced logging for debugging and monitoring

### 7. **Enhanced Documentation** ✅
- Created `easy_usage.md`: Beginner-friendly guide with working examples
- Created `advanced_usage.md`: Comprehensive API reference and advanced patterns
- Updated `HOW_TO_USE.md`: Extended documentation with additional features
- Created `ADVANCED_USAGE.md`: Additional advanced usage examples

### 8. **New Features and Capabilities** ✅
- JSON/YAML configuration file support
- Advanced Python factory functions for rapid development
- Convenience functions for common simulation patterns
- Comprehensive error reporting and logging
- Performance monitoring and metrics collection

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- ✅ Working memory with persistent activity and competition
- ✅ Episodic memory with experience encoding and replay
- ✅ Associative memory with Hebbian pattern associations
- ✅ All memory systems connected to neural processing

#### Neuromodulation Integration
- ✅ Dopamine affects neural excitability and plasticity
- ✅ Curiosity drives exploration behavior
- ✅ Novelty detection integrated with sensory processing
- ✅ All neuromodulators connected to plasticity rules

#### Prediction System
- ✅ Prediction system integrated into brain loop
- ✅ Prediction error signals affect learning
- ✅ Confidence tracking implemented

#### Cognition Systems
- ✅ Neural planner with action sequence evaluation
- ✅ Concept formation from experience patterns
- ✅ Attention with competitive selection dynamics
- ✅ All cognition systems connected to perception and action

#### Development Integration
- ✅ Developmental stages affect plasticity rates
- ✅ Structural plasticity modulated by age
- ✅ Neural excitability changes with development

#### Persistence
- ✅ Checkpoint save/load implemented
- ✅ Brain state serialization working
- ✅ Can resume from saved checkpoints

#### Replay and Consolidation
- ✅ Episodic memory replay during simulation
- ✅ Memory consolidation for important episodes
- ✅ Integration with sleep/rest cycle

## Building

### Prerequisites
- C++20 compatible compiler
- CMake 3.16 or higher
- Python 3.8+ (for Python bindings)
- pybind11 (for Python bindings)

### Build Instructions

```bash
# Standard build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Python bindings (separate target)
cd python
pip install pybind11 scikit-build-core pytest numpy
pip install -e .

# Run tests
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3` | Static Library | Phase 3 experiments (plasticity) |
| `nlm_phase4` | Static Library | Phase 4 experiments (development) |
| `nlm_phase5` | Static Library | Phase 5 experiments (optimization) |
| `nlm_phase6` | Static Library | Phase 6 experiments (integration) |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase5_demo` | Executable | Phase 5 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 demonstration |
| `nlm_test` | Executable | Unit test suite |
| `pynlm` | Shared Library | Python bindings |

## Running

### Phase 6 Demo (Integration Test)

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Python Usage Examples

#### Basic Usage
```python
import pynlm

# Quick start: create and initialize a brain
brain = pynlm.createQuickBrain()
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Neurons: {brain.getTotalNeuronCount()}, Spikes: {brain.getTotalSpikeCount()}")
```

#### Learning Agent
```python
# Create complete agent environment
brain, world, agent = pynlm.createQuickAgent(20, 20, 8, 8)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run learning simulation
pynlm.runLearningSimulation(1000)
```

#### Advanced Features
```python
# Get system information
print(pynlm.getSystemInfo())

# Print help
pynlm.printHelp()

# Get available types
print(pynlm.getNeuronTypes())
print(pynlm.getActionTypes())
```

## Python API Overview

### Core Classes

| Class | Purpose |
|-------|---------|
| `Brain` | Central neural simulation brain |
| `Config` | Configuration management system |
| `AgentBrain` | Agent interface connecting brain to world |
| `SimpleWorld` | 2D world for simulation |
| `Action` | Action representation |
| `SensoryPercept` | Sensory input data |

### Key Functions

| Function | Description |
|----------|-------------|
| `createQuickBrain()` | Create brain with defaults |
| `createQuickAgent()` | Create complete agent setup |
| `runLearningSimulation()` | Run learning simulation |
| `runSilentSimulation()` | Run silent simulation |
| `loadConfig()` | Load JSON/YAML config |
| `saveConfig()` | Save config to file |
| `getSystemInfo()` | Get build information |
| `printHelp()` | Show usage help |

## Project Structure

```
NLM/
├── CMakeLists.txt                    # Build configuration
├── pyproject.toml                    # Python package definition
├── README.md                         # Project overview
├── easy_usage.md                     # Beginner guide
├── advanced_usage.md                  # Advanced guide
├── HOW_TO_USE.md                      # Comprehensive documentation
├── configs/                          # Configuration files
│   └── default.cfg                  # Default configuration
├── src/                              # C++ source code
│   ├── core/                        # Core utilities
│   ├── brain/                       # Neural components
│   ├── dynamics/                    # Neural dynamics
│   ├── plasticity/                  # Plasticity rules
│   ├── development/                 # Developmental system
│   ├── neuromodulation/              # Neuromodulators
│   ├── memory/                      # Memory systems
│   ├── cognition/                   # Cognitive mechanisms
│   ├── sensory/                     # Sensory processing
│   ├── prediction/                   # Prediction systems
│   ├── motor/                       # Motor system
│   ├── environment/                  # Environment interface
│   ├── experiments/                  # Experiment framework
│   └── visualization/                # Visualization
├── python/                          # Python bindings
│   ├── bindings.cpp                 # Pybind11 bindings
│   ├── ADVANCED_USAGE.md             # Advanced usage guide
├── tests/                           # Unit tests
│   ├── test_*.cpp                   # Test files
│   └── CMakeLists.txt               # Test configuration
└── docs/                           # Technical documentation
    ├── ARCHITECTURE.md              # System architecture
    ├── SCIENCE.md                   # Scientific background
    ├── ROADMAP.md                   # Development roadmap
    └── experiment/                   # Experiment documentation
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |

## Testing

### Running Tests

```bash
# Build and run all tests
cd build
ctest --output-on-failure

# Run specific test suites
./nlm_test
```

### Test Coverage

The test suite includes:
- **Basic functionality**: Brain creation, initialization, stepping
- **Configuration**: File I/O, validation, error handling
- **Memory**: Working, episodic, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty effects
- **Development**: Critical periods, maturation, pruning
- **Integration**: Complete brain-world-agent loops
- **Performance**: Scalability and memory usage

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

## Getting Started

1. **Build the project**: Follow the build instructions above
2. **Run Phase 6 demo**: Test the complete integration
3. **Explore Python API**: Use the examples in the documentation
4. **Run tests**: Verify functionality with the comprehensive test suite
5. **Read documentation**: Refer to the guides for detailed usage

The NLM project provides a comprehensive framework for experimenting with artificial developmental brains. With its modular architecture, extensive testing, and user-friendly Python interface, it enables researchers and developers to explore complex neural systems and learn about brain-inspired computation.

For more information, refer to the documentation files in the `docs/` directory and `HOW_TO_USE.md` for detailed API documentation.

### Summary

This README has been comprehensively updated to reflect all the improvements made to the NLM project:

✅ **Fixed configuration issues** - Removed duplicate entries  
✅ **Enhanced config system** - JSON/YAML support and better error handling  
✅ **Improved Python bindings** - Advanced API with factory functions  
✅ **Modular CMake structure** - Better separation of concerns  
✅ **Comprehensive testing** - Extensive test coverage across all components  
✅ **Robust error handling** - Detailed error messages and validation  
✅ **Enhanced documentation** - Multiple guides for different user levels  
✅ **New features** - Advanced functions and capabilities  

The NLM project is now ready for experimentation with artificial developmental brains, providing researchers and developers with a powerful, well-documented framework for exploring complex neural systems.