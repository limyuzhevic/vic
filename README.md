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

## Python Bindings

NLM provides Python bindings for working with the neural simulation framework, enabling programmatic access to all brain systems.

### Installation

Install the NLM Python bindings:

```bash
pip install -e .
```

For enhanced bindings with additional functionality:

```bash
pip install -e .[dev]
```

### Basic Usage

```python
import nlm

# Create a default configuration
config = nlm.createConfig()

# Set custom parameters
config.set("random_seed", 42)
config.set("neuron_count", 1000)
config.set("region_count", 1)

# Create brain with configuration
brain = nlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step, step * 0.001)

# Get results
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")

# Clean up
brain.reset()
```

### Enhanced Bindings

For advanced functionality and comprehensive simulation control, use the enhanced bindings:

```python
import enhanced_pynlm as nlm

# Enhanced configuration and simulation control
config = nlm.createEnhancedConfig()
brain = nlm.createEnhancedBrain(config)
brain.initialize()

# Advanced memory management
import enhanced_pynlm.memory as memory
stats = memory.getWorkingMemoryStats(brain)
print(f"Memory utilization: {stats['workingMemoryUtilization']}")

# Performance monitoring
import enhanced_pynlm.performance as performance
metrics = performance.getBrainPerformanceMetrics(brain)
print(f"Performance: {metrics['stepsPerSecond']} steps/sec")

# Development control
import enhanced_pynlm.debug as debug
brain_info = debug.inspectBrainState(brain)
print(f"Brain status: {brain_info}")
```

For detailed documentation on the enhanced bindings, see `ENHANCED_PYTHON_BINDINGS.md`.

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Enhanced Python API Features

The enhanced Python bindings provide comprehensive access to all NLM brain systems:

#### Core Systems
- **Brain Simulation**: Complete neural simulation with LIF neurons and event-driven spike propagation
- **Memory Systems**: Working memory, episodic memory, and associative memory integration
- **Neuromodulation**: Dopamine, curiosity, reward, and novelty systems
- **Prediction Systems**: Forward and inverse models for prediction error
- **Cognition**: Attention, planning, and concept formation
- **Development**: Multi-stage developmental system with critical periods
- **Sensory Processing**: Vision, audio, and internal signal processing
- **Motor Control**: Action selection and motor output
- **World Simulation**: Environment interaction and action execution

#### Enhanced Features
- **Batch Operations**: Efficient processing of multiple operations
- **Performance Monitoring**: Real-time performance metrics and profiling
- **Memory Management**: Advanced memory system control and optimization
- **Debug and Development Tools**: Brain state inspection and debugging utilities
- **Experiment Configuration**: Phase 6 experiment templates and analysis
- **Simulation Control**: Advanced step control and time management
- **Error Handling**: Comprehensive error handling and validation
- **Logging**: Flexible logging system with multiple levels

#### Key Classes
- `Config`: Configuration management with type safety
- `Brain`: Enhanced brain simulation with comprehensive control
- `AgentBrain`: Agent brain interface with advanced control
- `SimpleWorld`: World simulation with enhanced features
- `VisualizationInterface`: Visualization with 3D rendering options

#### Submodules
- `core`: Core NLM functionality
- `simulation`: Simulation control utilities
- `memory`: Memory system utilities
- `neuromodulation`: Neuromodulation utilities
- `development`: Development system utilities
- `experiment`: Experiment configuration utilities
- `performance`: Performance monitoring utilities
- `debug`: Debug and development utilities

## Building from Source

### Dependencies

The NLM framework requires:
- C++17 compiler (GCC 7+, Clang 5+, or MSVC 2017+)
- CMake 3.16 or higher
- Python 3.8 or higher
- pybind11 >= 2.11.0
- scikit-build-core >= 0.5.0

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with parallel jobs
cmake --build . --config Release -- -j4
```

### Python Bindings

The Python bindings are built automatically as part of the build process using scikit-build. After building the C++ library, the Python bindings will be installed to the Python site-packages directory.

## Running the NLM Demo

### Phase 2 Demo (Basic)

Run the Phase 2 neural computation demo:

```bash
./nlm
```

This demonstrates the basic neural computation capabilities of NLM, including:
- Real LIF neuron dynamics
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity
- Structural plasticity
- Learning experiments with measurable changes

### Phase 6 Demo (Integration Test)

Run the Phase 6 integration demo:

```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected:

- Memory system integration (working, episodic, associative)
- Neuromodulation integration (dopamine, curiosity, reward, novelty)
- Prediction system integration
- Cognition system integration (attention, planning, concept formation)
- Development system integration
- Checkpoint and replay functionality

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── ENHANCED_PYTHON_BINDINGS.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── python/
│   ├── bindings.cpp                # Original Python bindings
│   ├── bindings.h                  # Original bindings header
│   ├── EnhancedPythonBindings.h    # Enhanced bindings header
│   └── EnhancedPythonBindings.cpp  # Enhanced bindings implementation
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
│   └── visualization/ # Visualization
├── tests/             # Unit and integration tests
└── configs/          # Configuration files
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

## Testing

### Running Tests

Run the test suite using pytest:

```bash
pip install pytest
pytest tests/
```

### Available Tests

The test suite includes:
- `test_types.cpp` - Core type testing
- `test_config.cpp` - Configuration testing
- `test_synapse.cpp` - Synapse testing
- `test_main.cpp` - Main functionality testing
- `test_brain.cpp` - Brain testing (if available)
- `test_random.cpp` - Random number generator testing
- `test_stdp.cpp` - STDP testing
- `test_clock.cpp` - Clock testing
- `test_neuron.cpp` - Neuron testing

### Python Tests

Python tests for the bindings:

```bash
# Run Python tests
pytest python_tests/
```

## Configuration

### Default Configuration

The NLM framework uses a configuration system to control simulation parameters. Default values are set in the code but can be overridden via:

1. **Configuration file**: Load from JSON file
2. **Command line arguments**: Override parameters from command line
3. **Programmatic API**: Set values in Python bindings

### Configuration Keys

Common configuration keys include:

- `random_seed`: Random number generator seed
- `simulation_timestep`: Simulation time step size
- `neuron_count`: Total number of neurons
- `region_count`: Number of brain regions
- `connection_probability`: Probability of synaptic connections
- `stdp_ltp_weight`: STDP LTP weight
- `stdp_ltd_weight`: STDP LTD weight
- `stdp_tau`: STDP time constant
- `synaptogenesis_rate`: Structural plasticity rate
- `pruning_rate`: Neural pruning rate
- `checkpoint_dir`: Directory for checkpoint files
- `replay_interval`: Memory replay interval
- `consolidation_interval`: Memory consolidation interval

### Configuration File Example

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "checkpoint_dir": "./checkpoints",
    "replay_interval": 100,
    "consolidation_interval": 1000
}
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

## Support and Contact

For support and questions:
1. Check the documentation in the `docs/` directory
2. Review examples in the Python bindings
3. Report issues through the project repository
4. Join the NLM research community for discussions

## Future Directions

The NLM project continues to evolve with:

1. **Enhanced Scalability**: Better performance on larger brain models
2. **Improved Integration**: Better connectivity between all brain systems
3. **Advanced Learning**: More sophisticated learning algorithms
4. **Developmental Studies**: Deeper investigation of developmental processes
5. **Memory Research**: Advanced memory consolidation and replay mechanisms
6. **Neuromodulation Studies**: Detailed investigation of neuromodulator interactions
7. **Cross-System Integration**: Better coordination between all brain systems
8. **Educational Tools**: Better tools for teaching neural computation
9. **Research Applications**: Applications for neuroscience and artificial intelligence research

## Contributing

To contribute to the NLM project:

1. **Code Contributions**: Submit pull requests with new features or bug fixes
2. **Documentation**: Improve documentation and add examples
3. **Testing**: Add new tests and improve existing tests
4. **Research**: Conduct research and contribute findings to the project
5. **Community**: Participate in discussions and help other users

### Guidelines for Contributors

- Follow the existing code style and conventions
- Add comprehensive tests for new functionality
- Update documentation with clear examples
- Write clear commit messages
- Use meaningful variable and function names
- Add appropriate comments for complex code
- Ensure backward compatibility where possible
- Test on multiple platforms and configurations

### Code Style

The NLM project follows standard C++ and Python coding conventions:

- C++11+ syntax with modern features
- Consistent naming conventions (camelCase for functions, UPPER_CASE for constants)
- Comprehensive documentation for all public APIs
- Comprehensive error handling and validation
- Efficient memory management with modern C++ practices

### Testing Standards

All code changes must include:
- Comprehensive unit tests
- Integration tests where applicable
- Performance benchmarks
- Documentation updates
- Examples demonstrating new functionality

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

## License

MIT

## Authors

Research project — See docs for scientific background.
