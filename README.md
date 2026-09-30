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

## Python Bindings

NLM includes Python bindings for easier use in Python scripts:

```bash
# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# Install the Python module
pip install .
```

### Python API Example

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)
    
print(f"Simulation complete! {brain.getTotalNeuronCount()} neurons simulated.")
```

For detailed Python usage documentation, see:
- `easy_usage.md` - Quick start guide for beginners
- `HOW_TO_USE.md` - Complete Python API documentation

## Configuration Files

NLM supports configuration files for customizing brain parameters:

### Default Configuration
`configs/default.cfg` - Main configuration file with all parameters

### Quick Start Configuration  
`configs/quick_start.cfg` - Simplified configuration for testing

### Architecture Documentation
`docs/CONFIGURATION.md` - Comprehensive configuration guide

## Code Improvements

This version includes significant improvements:

### Core C++ Code

1. **Enhanced Logger** (`src/core/Logger/Logger.cpp`):
   - Thread-safe timestamp generation using `std::gmtime`
   - Improved log level consistency (Warning, Critical)
   - Better message sanitization and error handling
   - Platform-independent color output control

2. **Brain Integration** (`src/brain/Brain.cpp`):
   - Fixed STDP parameter inconsistencies (matching main.cpp)
   - Improved constructor initialization order
   - Better system initialization sequencing

3. **Neuron Safety** (`src/brain/Neuron.cpp`):
   - Enhanced validation in `stepLIF()` method
   - Safety checks for timestep and membrane parameters
   - Improved random initialization with validation
   - Better error handling and logging

4. **Configuration System** (`src/core/Config/Config.cpp`):
   - File existence and permission validation
   - File size limits (1MB max)
   - Better error messages and logging
   - Platform-independent file operations

### Documentation Improvements

1. **Configuration Guide** (`docs/CONFIGURATION.md`):
   - Comprehensive configuration file format
   - Parameter descriptions and ranges
   - Multiple configuration examples
   - Validation and troubleshooting section

2. **Easy Usage** (`easy_usage.md`):
   - Beginner-friendly Python API guide
   - Step-by-step examples
   - Common patterns and mini-projects

3. **Configuration Examples**:
   - Default configuration with all parameters
   - Quick start configuration for testing
   - Example templates for different use cases

## Files Added/Modified

### New Files:
- `docs/CONFIGURATION.md` - Comprehensive configuration guide
- `configs/default.cfg` - Default configuration file
- `configs/quick_start.cfg` - Quick start configuration

### Modified Files:
- `src/core/Logger/Logger.cpp` - Enhanced logging with thread safety
- `src/brain/Brain.cpp` - Fixed STDP parameters and initialization
- `src/brain/Neuron.cpp` - Improved safety and validation
- `src/core/Config/Config.cpp` - Better error handling and validation

## Testing

The build process includes unit tests:

```bash
# Build with tests enabled
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
make -j4

# Run tests
ctest --output-on-failure
```

## Platform Support

NLM is designed to be cross-platform:

- **Compilers**: GCC 7+, Clang 9+, MSVC 2019+
- **CMake**: 3.16+
- **Python**: 3.8+ (for bindings)
- **File Systems**: POSIX, Windows, macOS
- **Architecture**: x86_64, ARM64 (where C++20 is available)

## Performance Optimizations

### Phase 5 Optimizations (Preserved)
- Memory pools for efficient allocation
- Event queues for spike processing
- SIMD vectorization where possible
- Parallel processing for large networks

### Phase 6 Integration Optimizations
- Efficient memory management
- Optimized spike event handling
- Fast synaptic propagation
- Reduced memory footprint

## Future Enhancements

Based on community feedback, planned improvements include:

1. **Enhanced Python API**: More intuitive Python interface
2. **GUI Tools**: Visual configuration and visualization
3. **Advanced Learning**: More sophisticated plasticity rules
4. **Distributed Computing**: Support for multi-GPU and distributed systems
5. **Hardware Acceleration**: CUDA/OpenCL support for faster simulations

## Support

For support, documentation, and community:

- [Documentation](https://nlm.readthedocs.io)
- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Issue Tracker](https://github.com/nlm-project/nlm/issues)
- [Discussions](https://github.com/nlm-project/nlm/discussions)

## License

MIT License. See `LICENSE` file for details.

## Acknowledgments

Thanks to the open-source community for pybind11, scikit-build-core, and other essential tools that make this project possible.

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

## License

MIT

## Authors

Research project — See docs for scientific background.
