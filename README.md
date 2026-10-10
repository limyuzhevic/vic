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

## Key Improvements in This Version

### 1. Enhanced Build System

#### New Build Scripts
- **`build.sh`**: Cross-platform build script for Linux/macOS
- **`nlm_build.py`**: Python-based build script for cross-platform support

#### Build System Enhancements
- Automated dependency checking
- Parallel building for faster compilation
- Cross-platform support (Windows, Linux, macOS)
- Comprehensive test automation
- Installation to system directories
- Progress reporting and error handling
- Configuration validation

#### Build Options
```bash
# Standard CMake build
cd /path/to/nlm
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Enhanced build script
chmod +x build.sh nlm_build.py
./build.sh build
./build.sh test

# Python build script
./nlm_build.py build
./nlm_build.py test
```

### 2. Enhanced Command-Line Interface

#### New Features Added
- **`--demo`**: Run comprehensive integration tests
- **`--verbose` / `-v`**: Enable detailed logging
- **`--timestep N`**: Set simulation timestep
- **`--neurons N`**: Set neuron count
- **`--steps N`**: Set number of simulation steps
- **`--config FILE`**: Load configuration from file
- **`--help` / `-h`**: Show help message
- **`--version`**: Show version information

#### Usage Examples
```bash
# Run enhanced demo with comprehensive testing
./nlm --demo --verbose

# Custom configuration
./nlm --neurons 1000 --steps 2000 --timestep 0.0001

# Load specific configuration
./nlm --config configs/my_config.cfg

# Show help
./nlm --help

# Show version
./nlm --version
```

### 3. Enhanced Demo Mode

New enhanced demo mode provides comprehensive system integration testing:

```bash
cd build
./nlm --demo --verbose
```

This runs comprehensive integration tests including:
- Memory system integration testing
- Neuromodulation system testing
- Prediction system validation
- Development system testing
- Detailed performance metrics
- Component connectivity verification

### 4. Improved Documentation

#### Documentation Files
- **`easy_usage.md`**: Beginner-friendly usage guide with examples
- **`HOW_TO_USE.md`**: Comprehensive technical documentation
- **`NLM_IMPROVEMENTS.md`**: Detailed improvement documentation

#### Documentation Content
- Quick start guides for beginners
- Comprehensive technical reference
- Build and installation instructions
- Command-line interface documentation
- API reference
- Troubleshooting guides

### 5. Completed Neuromodulation Systems

#### Implemented/Completed Systems
- **Dopamine System**: Complete reward signaling and plasticity modulation
- **Curiosity System**: Enhanced novelty detection and exploration drive
- **Novelty System**: Improved pattern detection and feature extraction
- **Reward System**: Complete reward computation from observations
- **Prediction Error System**: Enhanced error computation and history tracking

#### System Features
- Proper initialization and configuration
- State management and updates
- Integration with neural components
- Error handling and validation
- Comprehensive API documentation

### 6. Core Component Bug Fixes

#### Fixed Bugs
- **Neuron.cpp**: Fixed inconsistencies in current management
- **Synapse.cpp**: Fixed short-term plasticity parameter initialization
- **Brain.cpp**: Fixed configuration parameter access and initialization
- **Brain.cpp**: Fixed error handling and null pointer checks

#### Improvements
- Enhanced error handling and validation
- Improved code organization and structure
- Fixed memory management issues
- Enhanced debugging capabilities

## Building

### Prerequisites
- C++ Compiler: GCC 10+, Clang 12+, or MSVC 2019+
- CMake: Version 3.16 or higher
- Python: 3.8+ (for Python bindings)
- pybind11: Version 2.11.0+ (for Python bindings)

### Build Steps

#### Standard CMake Build
```bash
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Enhanced Build Script
```bash
cd /path/to/nlm
chmod +x build.sh nlm_build.py
./build.sh build
./build.sh test
```

#### Python Build Script
```bash
cd /path/to/nlm
chmod +x nlm_build.py
./nlm_build.py build
./nlm_build.py test
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 integration demo (NEW) |
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

# Enable Phase 6 integration tests
cmake .. -DENABLE_PHASE6_INTEGRATION=ON

# Enable Python bindings (if pybind11 available)
cmake .. -DPYTHON_EXECUTABLE=/usr/bin/python3
```

## Running

### Basic Simulation

```bash
# Run main simulation
cd build
./nlm

# Run Phase 3 demo
cd build
./nlm_phase3_demo

# Run Phase 4 demo
cd build
./nlm_phase4_demo

# Run Phase 6 integration demo (NEW)
cd build
./nlm_phase6_demo
```

### Enhanced Demo Mode

```bash
cd build
./nlm --demo --verbose
```

This runs comprehensive integration tests including:
- Memory system integration testing
- Neuromodulation system testing
- Prediction system validation
- Development system testing
- Detailed performance metrics
- Component connectivity verification

### Python API

```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize brain
brain.initialize()

# Run simulation
for i in range(1000):
    brain.step(i)
    if i % 100 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
```

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
│   └── visualization/ # Visualization
├── tests/
├── configs/
├── build.sh                # Enhanced build script
├── nlm_build.py            # Python build script
└── NLM_IMPROVEMENTS.md      # Improvement documentation
```

## Testing

### Enhanced Build Script Testing
```bash
cd /path/to/nlm
./build.sh test
```

This runs:
- Unit tests for all major components
- Integration tests for system components
- Performance benchmarks
- Regression testing
- Cross-platform test support

### Demo Mode Integration Tests
```bash
cd build
./nlm --demo --verbose
```

This provides comprehensive system integration testing with detailed reports and performance metrics.

## Future Enhancements

### Planned Improvements
1. **Advanced Neuromodulation**: Enhanced serotonin, acetylcholine, norepinephrine systems
2. **Advanced Learning**: Deep reinforcement learning integration
3. **Enhanced Visualization**: Real-time neural activity visualization
4. **Cloud Integration**: Distributed computing capabilities
5. **Advanced Memory Systems**: Long-term memory and consolidation
6. **Neurogenesis**: Dynamic neuron creation and growth

### Research Directions
1. **Biological Plausibility**: Enhanced biological accuracy
2. **Scalability**: Large-scale neural network simulation
3. **Real-time Applications**: Live neural network interaction
4. **Educational Tools**: Interactive neural learning platforms

## Conclusion

The NLM project has been significantly improved with:

1. **Enhanced Functionality**: Complete neuromodulation systems, better plasticity rules
2. **Improved Code Quality**: Fixed bugs, added documentation, better organization
3. **Better User Experience**: Comprehensive CLI, enhanced build system, detailed documentation
4. **Enhanced Testing**: Comprehensive test coverage, integration testing, performance benchmarks
5. **Future-Proof Design**: Modular architecture, extensible code, research-oriented features

The project is now ready for:

- Advanced research applications
- Educational use
- Production deployment
- Cross-disciplinary collaboration

## Version Information

- **Current Version**: 0.1.0 (Phase 2 Enhanced)
- **Build Date**: $(date +%Y-%m-%d)
- **Platform Support**: Windows, Linux, macOS
- **Python Support**: 3.8+
- **License**: MIT

---

*For more information, visit the NLM documentation and project repository.*
