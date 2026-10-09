# NLM (熙然) - Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that implements a neural system which begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is **NOT**:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM **IS** intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Key Improvements in This Version

This version (Phase 2) includes significant improvements over the original implementation:

### 🚀 **Major Improvements**

1. **Enhanced Command-Line Interface**
   - Comprehensive CLI with multiple commands
   - Configuration file support (JSON/YAML)
   - Better help system and documentation
   - Multiple experiment modes

2. **Improved Error Handling**
   - Robust exception handling throughout
   - Input validation and parameter checking
   - Graceful error recovery
   - Detailed error messages

3. **Configuration System**
   - JSON/YAML support in addition to simple key=value format
   - Configuration validation
   - File-based configuration management
   - Command-line argument parsing

4. **Code Quality Improvements**
   - Better separation of concerns
   - Comprehensive documentation and comments
   - Memory management fixes
   - Thread-safe operations

5. **Python Bindings**
   - Complete Python API for all core functionality
   - Easy-to-use interface for brain simulation
   - Support for agent-world interactions
   - Learning and development capabilities

6. **Testing Infrastructure**
   - Enhanced unit test suite
   - JSON parsing tests
   - Error handling tests
   - Integration tests

7. **Performance Monitoring**
   - Built-in performance tracking
   - Memory usage monitoring
   - Timing measurements
   - Progress reporting

## Architecture

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

## Building

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
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

## Python Installation

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy pandas

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

## Running

### C++ Executable

The main executable provides comprehensive testing and demonstration capabilities:

```bash
./nlm --help
```

### Key Command Line Options

- `--help`: Show help message
- `--version`: Show version information
- `--config FILE`: Load configuration from file
- `--neuron-count N`: Set number of neurons
- `--timestep T`: Set simulation timestep
- `--seed SEED`: Set random seed
- `--dry-run`: Run configuration validation only
- `--verbose`: Enable verbose logging
- `--quiet`: Suppress informational messages

### Example Usage

```bash
# Basic simulation
./nlm --neuron-count 1000 --seed 123

# With configuration file
./nlm --config myconfig.cfg --verbose

# Dry run (validate configuration)
./nlm --dry-run --neuron-count 500
```

## Python Examples

The repository includes comprehensive examples in `example.py`:

### Installation

```bash
pip install .
```

### Running Examples

```bash
python example.py
```

### Available Examples

1. **Basic Brain Simulation**: Create and run a simple brain
2. **Agent in World Simulation**: Complete agent-world interaction
3. **Learning Experiment**: Demonstrate learning and adaptation
4. **Configuration Management**: Save and load configurations
5. **Error Handling**: Test error handling capabilities

## Python API Documentation

### Core Classes

#### Brain
- `createBrain(config)`: Create a new brain
- `initialize()`: Initialize the brain
- `step(step_number)`: Run one simulation step
- `reset()`: Reset brain state
- `getTotalNeuronCount()`: Get total neurons
- `getFiringNeuronCount()`: Get currently firing neurons
- `getAverageFiringRate()`: Get average firing rate

#### AgentBrain
- `createAgentBrain(brain)`: Create agent interface
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command
- `applyRewardModulation(reward, predicted)`: Apply reward

#### SimpleWorld
- `createSimpleWorld()`: Create a simple world
- `configure(width, height, visionWidth, visionHeight)`: Configure world
- `update(timestep)`: Update world state
- `getSensoryPercept()`: Get sensory perception

### Utility Functions

- `runSimulation(brain, world, agent, num_steps)`: Run complete simulation
- `run_agent_simulation(num_steps)`: High-level agent simulation

### Configuration

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 1000)
config.set("plasticity.rate", 0.01)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")
```

## Project Structure

```
NLM/
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
│
├── tests/              # Test suite
│   ├── test_config_enhanced.cpp  # Enhanced config tests
│   ├── test_config.cpp          # Original config tests
│   └── ...                     # Other test files
│
├── docs/              # Documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   └── ...
│
├── python_bindings.cpp   # Complete Python bindings
├── example.py           # Python examples
├── easy_usage.md        # Easy usage guide
├── HOW_TO_USE.md        # Comprehensive usage guide
├── README.md            # This file
└── pyproject.toml       # Python package configuration
```

## Features and Capabilities

### Neural Computation

- **Leaky Integrate-and-Fire (LIF) neurons**
- **Event-driven spike propagation** with synaptic delays
- **STDP and Hebbian plasticity rules**
- **Structural plasticity** (synaptogenesis/pruning)
- **Multiple neuron types**: excitatory, inhibitory, sensory, motor, modulatory

### Learning Systems

- **Reward modulation** (dopamine-based learning)
- **Curiosity-driven exploration**
- **Novelty detection**
- **Prediction error signals**
- **Developmental stages** affecting plasticity rates

### Memory Systems

- **Working memory** with persistent activity
- **Episodic memory** with experience encoding and replay
- **Associative memory** with Hebbian pattern associations
- **Semantic and procedural memory**
- **Memory consolidation** during rest/ replay

### Cognitive Functions

- **Neural planning** with action sequence evaluation
- **Concept formation** from experience patterns
- **Attention** with competitive selection dynamics
- **Self-model** and internal representation
- **Social learning** capabilities

### World Interaction

- **Sensory processing** (vision, audio, internal signals)
- **Motor control** with action selection
- **Environmental interaction** with resources and hazards
- **Reward computation** based on outcomes

### Development and Maturation

- **Developmental stages**: initial, critical period, maturation, adult, aging
- **Structural plasticity** modulated by age
- **Neural excitability** changes with development
- **Experience-dependent** maturation

### Monitoring and Analysis

- **Performance metrics** and timing information
- **Memory usage** tracking
- **Spike activity** analysis
- **Learning progress** monitoring
- **Integration verification** for all systems

## Testing

The project includes comprehensive tests:

### C++ Tests

- **test_config.cpp**: Configuration tests
- **test_config_enhanced.cpp**: Enhanced config tests with JSON support
- **test_neuron.cpp**: Neuron functionality tests
- **test_stdp.cpp**: STDP plasticity tests
- **test_brain.cpp**: Brain integration tests
- **test_main.cpp**: Main test runner

### Running Tests

```bash
# Build with tests
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

### Test Coverage

- **Configuration validation**: JSON parsing, file I/O, error handling
- **Memory management**: Leak detection, pointer safety
- **Neural computation**: LIF dynamics, spike propagation
- **Plasticity**: STDP, Hebbian learning, structural changes
- **Integration**: System connectivity and coordination

## Performance Optimizations

### Memory Management

- **Smart pointers**: Automatic memory management
- **Move semantics**: Efficient object transfer
- **Memory pools**: Reduced allocation overhead
- **Cache-friendly data structures**: Better spatial locality

### Computational Efficiency

- **Event-driven simulation**: Only process active neurons
- **Vectorization**: SIMD instructions where appropriate
- **Parallel processing**: Multi-core utilization
- **Optimized algorithms**: Efficient plasticity updates

### Profiling and Monitoring

- **Wall clock timing**: Real-time performance measurement
- **CPU time tracking**: Processor utilization monitoring
- **Memory usage estimation**: Resource consumption tracking
- **Progress reporting**: Real-time status updates

## Scientific Background

NLM is a research project investigating computational brain-like systems. It makes **NO claims** that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

### Research Questions

NLM Phase 6 investigates:

- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## License

MIT License. See the LICENSE file for details.

## Authors

Research project — See docs for scientific background.

## Contributing

We welcome contributions to improve NLM! Please follow these guidelines:

1. **Fork the repository**
2. **Create a feature branch**
3. **Implement your changes**
4. **Add tests**
5. **Update documentation**
6. **Submit a pull request**

### Development Guidelines

- Follow the existing code style and conventions
- Add comprehensive tests for new functionality
- Document all public APIs
- Maintain backward compatibility where possible
- Use modern C++ features (C++20)
- Ensure code is well-tested and well-documented

## Future Development

### Phase 8: Advanced Features

- **Neuromodulatory networks**: More sophisticated neuromodulation
- **Social learning**: Multi-agent interactions
- **Language capabilities**: Symbol grounding and syntax
- **Abstract reasoning**: Logical deduction and inference
- **Consciousness studies**: Theories of mind and self-awareness
- **Scale to larger networks**: Cortical column models

### Python Enhancements

- **Data visualization**: Plot neural activity and learning curves
- **Machine learning integration**: Combine with modern ML frameworks
- **Web interface**: Browser-based visualization and control
- **Real-time interaction**: Live simulation control
- **Cloud deployment**: Serverless simulation environments

## Support

For questions, issues, or feature requests:

- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Documentation**: https://nlm.readthedocs.io

## Acknowledgments

This project builds on research in:
- Computational neuroscience
- Neuromorphic engineering
- Machine learning
- Cognitive science

Special thanks to contributors and researchers in the field.
