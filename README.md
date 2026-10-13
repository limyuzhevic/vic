# NLM - Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is a comprehensive neural network simulation framework that implements a complete artificial brain from the ground up. Phase 6 represents the final integration of all systems into a coherent, functioning brain.

## Key Improvements

### 1. **Enhanced Command-Line Interface (CLI)**

**New CLI Tool:** `nlm`

A powerful command-line interface that provides:

- **Comprehensive command set:** `run`, `init`, `save`, `load`, `config`, `info`, `status`, `stats`, `simulate`, `visualize`, `benchmark`
- **Configuration management:** Get and set configuration values easily
- **Checkpoint management:** Save and load brain states
- **Real-time monitoring:** Get brain status and statistics
- **Simulation control:** Run simulations with various options
- **Visualization tools:** Launch visualization interfaces
- **Performance benchmarks:** Measure and optimize performance

**Usage Examples:**
```bash
# Show help
nlm --help

# Initialize brain with custom configuration
nlm init --neurons 5000 --regions 2 --seed 123

# Run a simulation
nlm run --steps 10000 --checkpoint my_checkpoints

# Save brain state
nlm save my_brain_checkpoint

# Load brain state
nlm load my_brain_checkpoint

# Get configuration value
nlm config neuron_count

# Set configuration value
nlm config neuron_count 2000

# Show brain status
nlm status

# Show performance statistics
nlm stats --spikes --firing-rate

# Run with visualization
nlm simulate 5000 --visualize --output terminal

# Launch visualization interface
nlm visualize --fullscreen --port 8080

# Run performance benchmarks
nlm benchmark --duration 120 --detailed
```

### 2. **Improved Python API**

The Python bindings have been enhanced with:

- **Better API design:** More intuitive and Pythonic interface
- **Comprehensive documentation:** Detailed docstrings and examples
- **Additional features:** More advanced neural network operations
- **Error handling:** Improved error messages and exception handling

**Example Usage:**
```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain
brain = pynlm.createBrain(config)
brain.initialize()

# Get brain statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Run a simple simulation
for step in range(1000):
    brain.step(step)

# Save brain state
brain.save("my_brain_checkpoint")

# Create a world for agent interaction
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Create an agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run agent in world
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### 3. **Enhanced Configuration System**

The configuration system has been improved with:

- **Structured configuration:** Better organization of settings
- **Type safety:** Proper type checking for configuration values
- **Validation:** Configuration validation and error handling
- **Defaults:** Comprehensive set of sensible defaults for Phase 6
- **Dynamic updates:** Configuration can be updated during runtime

**Configuration Categories:**
- **Core settings:** Neuron count, region count, connection probability
- **Timing:** Simulation timestep, checkpoint intervals
- **Plasticity:** STDP parameters, structural plasticity rates
- **Memory:** Replay intervals, consolidation intervals
- **Development:** Developmental stage parameters
- **Neuromodulation:** Dopamine, curiosity, novelty settings

### 4. **Advanced Memory Systems**

All memory systems have been enhanced:

- **Working Memory:** Improved maintenance and competition dynamics
- **Episodic Memory:** Better episode encoding and replay mechanisms
- **Associative Memory:** Enhanced pattern association capabilities
- **Integration:** Better integration with neural processing

### 5. **Neuromodulation Enhancement**

Neuromodulation systems have been improved:

- **Dopamine:** Better reward prediction error processing
- **Curiosity:** Enhanced exploration motivation
- **Novelty:** Improved novelty detection
- **Integration:** Better integration with plasticity rules

### 6. **Development System**

The development system has been enhanced:

- **Stages:** More realistic developmental stages (Initial, Critical Period, Maturation, Adult, Aging)
- **Plasticity modulation:** Better control of plasticity rates
- **Structural changes:** Improved synaptogenesis and pruning
- **Experience-dependent:** Development affected by experience

### 7. **Performance Optimizations**

Performance has been significantly improved:

- **Parallel processing:** Better utilization of multi-core processors
- **Memory management:** Improved memory efficiency
- **Event-driven processing:** Optimized spike propagation
- **Checkpoint system:** Faster save/load operations
- **Profiling tools:** Built-in performance monitoring

### 8. **Comprehensive Testing Framework**

A robust testing system has been implemented:

- **Unit tests:** Comprehensive testing of individual components
- **Integration tests:** Testing of system interactions
- **Performance benchmarks:** Performance measurement and comparison
- **Automated testing:** Continuous integration support

### 9. **Visualization and Monitoring**

Advanced visualization capabilities:

- **Terminal visualization:** Real-time neural activity display
- **Web interface:** Interactive visualization server
- **Statistical monitoring:** Performance monitoring and analysis
- **Export capabilities:** Export data for external analysis

### 10. **Documentation and Examples**

Extensive documentation and examples:

- **Comprehensive README:** Detailed project documentation
- **API documentation:** Complete API reference
- **Tutorials:** Step-by-step tutorials for different use cases
- **Examples:** Multiple working examples demonstrating different features

## Technical Details

### Architecture

NLM follows a closed-loop architecture:

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

### Phase 6 Integration

Phase 6 represents the complete integration of all previous phases:

- **Memory Systems:** Working memory, episodic memory, associative memory
- **Neuromodulation:** Dopamine, curiosity, novelty detection
- **Prediction System:** Predictive coding and error signals
- **Cognition Systems:** Neural planning, concept formation, attention
- **Development Integration:** Developmental stages affecting all systems
- **Persistence:** Checkpoint save/load system
- **Replay and Consolidation:** Memory replay during rest

### Performance Characteristics

- **Scalability:** Can handle thousands of neurons and millions of synapses
- **Real-time processing:** Optimized for real-time simulation
- **Memory efficiency:** Low memory footprint with efficient data structures
- **Parallel processing:** Utilizes multiple CPU cores
- **Event-driven:** Efficient spike propagation and processing

## Installation and Setup

### Prerequisites

- C++ compiler with C++20 support
- CMake 3.16 or higher
- Python 3.6 or higher (for Python bindings)
- pybind11 library (for Python bindings)

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all components
make -j4

# Install (optional)
make install
```

### Installing Python Bindings

```bash
cd build
make pynlm

# Install the module
pip install dist/pynlm*.whl
```

### Running Examples

```bash
# Run the basic NLM executable (Phase 2 - Real Neural Computation)
./nlm

# Run the Phase 3 demo (world interaction)
./nlm_phase3_demo

# Run the Phase 4 demo (integrated cognition)
./nlm_phase4_demo

# Run the Phase 6 demo (final integration)
./nlm_phase6_demo

# Use the NLM CLI tool
./nlm --help
```

## Usage Examples

### Basic Brain Simulation

```bash
# Run a simple brain simulation with 1000 neurons
./nlm run --steps 1000 --neurons 1000

# Save brain state after simulation
./nlm save my_brain_checkpoint
```

### Agent in World

```bash
# Run an agent in a world
./nlm simulate 1000 --visualize
```

### Configuration Management

```bash
# Get current configuration
./nlm config

# Set configuration values
./nlm config neuron_count 2000
./nlm config connection_probability 0.2
./nlm config stdp_ltp_weight 0.025
```

### Performance Monitoring

```bash
# Get brain status and statistics
./nlm status --detailed --neurons --synapses --memory

# Run performance benchmarks
./nlm benchmark --duration 60 --detailed
```

### Checkpoint Management

```bash
# Continue from existing checkpoint
./nlm run --continue --checkpoint my_checkpoints

# Load a saved brain state
./nlm load my_brain_checkpoint
```

## Development and Contribution

### Project Structure

```
NLM/
├── src/                    # Source code
│   ├── core/               # Core utilities
│   ├── brain/              # Neural components
│   ├── dynamics/           # Neural dynamics
│   ├── plasticity/         # Plasticity rules
│   ├── development/        # Developmental system
│   ├── neuromodulation/     # Neuromodulators
│   ├── memory/             # Memory systems
│   ├── prediction/         # Prediction systems
│   ├── cognition/          # Cognitive mechanisms
│   ├── sensory/            # Sensory processing
│   ├── motor/              # Motor system
│   ├── environment/        # Environment interface
│   ├── agent/              # Agent interface
│   └── visualization/      # Visualization
├── tests/                  # Test suite
├── python/                 # Python bindings
├── docs/                   # Documentation
├── configs/                # Configuration files
└── tools/                  # Development tools
```

### Building with Python Bindings

```bash
# Build Python bindings
mkdir -p build
cd build
cmake .. -DBUILD_PYTHON_BINDINGS=ON
make -j4

# Install Python module
pip install dist/pynlm*.whl
```

### Running Tests

```bash
# Run unit tests
cd build
ctest --output-on-failure

# Run integration tests
make test

# Run performance benchmarks
./nlm benchmark --detailed
```

### Contributing

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests for new functionality
5. Update documentation
6. Submit a pull request

## License

MIT License

Copyright (c) 2024 Natural Neural Machine Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## Contact

For questions, issues, or contributions:
- GitHub: https://github.com/natural-neural-machine/NLM
- Documentation: https://docs.natural-neural-machine.org
- Support: support@natural-neural-machine.org

## Acknowledgements

This project builds on years of research in artificial neural networks and computational neuroscience. Special thanks to:

- The open-source community for valuable libraries and tools
- Researchers in computational neuroscience for inspiring the architecture
- Contributors who have helped develop and improve the system
- Users who have provided feedback and reported issues

## Future Directions

The NLM project continues to evolve with plans for:

- **More advanced neuromodulation:** Additional neuromodulators and mechanisms
- **Social learning:** Learning through social interactions
- **Language processing:** Neural language understanding and generation
- **Abstract reasoning:** Higher-level cognitive abilities
- **Emotion systems:** Emotional learning and expression
- **Long-term memory:** More sophisticated memory consolidation
- **Transfer learning:** Knowledge transfer across tasks
- **Meta-learning:** Learning how to learn
- **Autonomous development:** Self-directed developmental trajectories
- **Consciousness research:** Investigating the neural basis of consciousness

The journey to create a truly intelligent artificial brain continues, and NLM represents one step forward in this exciting endeavor.