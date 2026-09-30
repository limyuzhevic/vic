# NLM (熙然) - Neural Learning Machine

## Overview

**NLM (熙然, meaning "serene flow")** is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

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

### Current Status

All systems integrated into a coherent artificial brain with:
- Memory systems connected to neural processing
- Neuromodulation affecting plasticity and dynamics
- Prediction integrated with learning
- Development affecting plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Quick Start Guide

### Prerequisites

1. **Build System**: CMake 3.16+ with C++20 support
2. **Python**: Python 3.8+ with pip
3. **Build Tools**: Make or Ninja

### Installation

#### 1. Clone the repository

```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

#### 2. Build the C++ core

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### 3. Build Python bindings

```bash
python3 setup.py build
```

#### 4. Install Python module

```bash
python3 setup.py install
```

### Running Demos

The NLM project includes several demos that showcase different capabilities:

#### Phase 3: World Interaction Demo

```bash
# Run with default parameters
./build/nlm_phase3_demo

# Run with custom parameters (5 episodes, 500 steps each)
./build/nlm_phase3_demo 5 500

# Or use the command-line interface
nlm run demo phase3 --episodes 10 --steps 200
```

**What it demonstrates:**
- Agent in a 2D world
- Sensory input processing
- Action selection
- Learning through experience
- Memory integration

#### Phase 4: Research Demo

```bash
./build/nlm_phase4_demo
```

**What it demonstrates:**
- Advanced neural prediction
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention
- Predictive planning
- Self-model
- Social learning
- Continual learning

#### Phase 6: Integration Demo

```bash
./build/nlm_phase6_demo
```

**What it demonstrates:**
- Complete system integration
- Memory system connectivity
- Neuromodulation integration
- Prediction system
- Development effects
- Checkpoint system
- Replay and consolidation

### Python API Examples

#### Basic Brain Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
```

#### Agent in World

```python
import pynlm

# Create brain, world, and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Complete agent simulation loop
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Using Command-Line Interface

```bash
# Create a standard configuration
nlm config create --output my_config.json

# Create and run a simulation
nlm brain create --neurons 1000 --output demo.brain
nlm brain load demo.brain --steps 500

# Analyze brain metrics
nlm analyze --input demo.brain --output stats.json
```

#### Research Mode

```python
import pynlm

# Create configuration for research
config = pynlm.createDefaultConfig()

# Enable advanced features
brain = pynlm.createBrain(config)
brain.initialize()

# Run research simulation
brain = pynlm.run_example_simulation(500, "my_config.json")

# Get diagnostic information
diagnostics = pynlm.run_diagnostic(brain)
for key, value in diagnostics.items():
    print(f"{key}: {value}")
```

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

### Core Components

#### 1. Core Brain Systems
- **Neurons**: Leaky Integrate-and-Fire (LIF) models with event-driven spike propagation
- **Synapses**: Chemical and electrical connections with plasticity rules
- **Populations**: Groupings of neurons by type and function
- **Regions**: Anatomical organization of neural tissue

#### 2. Memory Systems
- **Working Memory**: Transient active information maintenance
- **Episodic Memory**: Experience storage and replay
- **Associative Memory**: Pattern associations and recall
- **Semantic Memory**: Abstract knowledge representation

#### 3. Neuromodulation Integration
- **Dopamine**: Reward and reinforcement signaling
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection and surprise
- **Prediction Error**: Learning signal for prediction systems

#### 4. Prediction System
- **Sensory Prediction**: Forward models of sensory input
- **Internal Models**: Generative models of the world
- **Prediction Error**: Mismatch signals for learning

#### 5. Cognitive Systems
- **Neural Planner**: Action sequence evaluation and planning
- **Concept Formation**: Pattern discovery and abstraction
- **Attentional Selection**: Competitive selection of neural activity

#### 6. Development Integration
- **Developmental Stages**: Critical periods and maturation
- **Structural Plasticity**: Synaptogenesis and pruning
- **Maturation**: Changes in neural excitability and connectivity

## Configuration

NLM uses a flexible configuration system based on JSON files. Configuration options include:

### Brain Parameters
```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "random_seed": 42,
    "simulation_timestep": 0.001
}
```

### Neuromodulation Parameters
```json
{
    "enable_dopamine": true,
    "enable_curiosity": true,
    "enable_novelty": true,
    "dopamine_level": 0.5,
    "curiosity_level": 0.3,
    "novelty_threshold": 0.1
}
```

### Memory Parameters
```json
{
    "working_memory_capacity": 100,
    "episodic_memory_max_episodes": 1000,
    "associative_memory_pattern_size": 50
}
```

### Development Parameters
```json
{
    "developmental_stage": "Initial",
    "plasticity_rate": 1.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001
}
```

Configuration files can be created using the `nlm config create` command or manually edited.

## Building from Source

### Prerequisites

- CMake 3.16 or higher
- C++ compiler with C++20 support
- Python 3.8 or higher
- pip
- pybind11
- scikit-build-core

### Build Steps

#### Linux/macOS

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DPYTHON_EXECUTABLE=$(which python3)

# Build with parallel jobs
make -j$(nproc)
```

#### Windows

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 16 2019" -A x64

# Build with MSBuild
cmake --build . --config Release
```

### Build Targets

The build system creates the following targets:

- `nlm`: Main demo executable (Phase 2)
- `nlm_phase3_demo`: Phase 3 demo executable
- `nlm_phase4_demo`: Phase 4 demo executable
- `nlm_phase6_demo`: Phase 6 demo executable
- `nlm_core`: Core C++ library
- `nlm_agent`: Agent brain interface library
- `nlm_world`: World simulation library
- `nlm_phase3`: Phase 3 experiment library
- `nlm_phase4`: Phase 4 experiment library

## Testing

### Running Tests

#### Unit Tests

```bash
# Run all unit tests
./build/tests/test_runner

# Or run specific tests
./build/tests/test_neuron
./build/tests/test_stdp
./build/tests/test_synapse
./build/tests/test_config
./build/tests/test_clock
./build/tests/test_random
./build/tests/test_types
./build/tests/test_brain
```

#### Integration Tests

```bash
# Run Phase 3 integration tests
./build/tests/test_phase3

# Run Phase 4 integration tests
./build/tests/test_phase4

# Run Phase 6 integration tests
./build/tests/test_phase6
```

### Test Coverage

The test suite covers:
- Neuron dynamics (LIF model, spike generation)
- Synaptic plasticity (STDP, Hebbian)
- Configuration system (loading, saving, validation)
- Random number generation
- Core types and data structures
- Brain integration and coordination
- Memory system integration
- Neuromodulation effects

## Contributing

### Code Quality Guidelines

1. **Modular Design**: Break large functions into smaller, focused functions
2. **Error Handling**: Use exceptions for error conditions and provide clear error messages
3. **Documentation**: Document all public APIs and complex algorithms
4. **Testing**: Write unit tests for new code and ensure existing tests pass
5. **Performance**: Profile code and optimize bottlenecks

### Best Practices

1. **Use RAII**: Leverage C++11/14/17 RAII idioms for resource management
2. **Avoid Raw Pointers**: Prefer smart pointers (unique_ptr, shared_ptr)
3. **Use Modern C++**: Take advantage of C++11/14/17 features
4. **Consistent Naming**: Use clear, descriptive variable and function names
5. **Code Organization**: Group related functionality in logical modules

### Submitting Changes

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Write tests for new functionality
5. Run the full test suite
6. Submit a pull request

## Troubleshooting

### Common Issues

#### 1. Build Failures

**Symptom**: CMake configuration fails or compilation errors

**Solution**:
- Ensure CMake 3.16+ is installed
- Check C++ compiler supports C++20
- Verify required dependencies are installed
- Try cleaning the build directory and rebuilding

#### 2. Python Module Import Errors

**Symptom**: `ImportError: No module named 'pynlm'`

**Solution**:
- Ensure Python bindings are built: `python3 setup.py build`
- Install the module: `python3 setup.py install`
- Check Python path: `python3 -c "import sys; print(sys.path)"`

#### 3. Runtime Errors

**Symptom**: Segmentation faults or crashes during simulation

**Solution**:
- Check for null pointer dereferences in code
- Verify configuration parameters are valid
- Run with smaller network sizes for debugging
- Check for memory leaks with valgrind or similar tools

#### 4. Performance Issues

**Symptom**: Simulations are too slow

**Solution**:
- Reduce network size for testing
- Check for unnecessary computations
- Enable compiler optimizations: `-O2` or `-O3`
- Consider using SIMD instructions for vectorized operations

### Getting Help

#### Issue Reports

If you encounter a bug or have a feature request:
1. Check the existing issues in the repository
2. Search for similar problems
3. Create a new issue with:
   - Clear reproduction steps
   - Expected vs. actual behavior
   - Relevant configuration files
   - Log output if available

#### Discussions

For general discussions about NLM:
- Join the project mailing list
- Participate in GitHub discussions
- Check the project documentation

#### Support

For commercial support or consulting:
- Contact the project authors
- Check for support options on the project website

## License

MIT License

Copyright (c) 2024 NLM Project

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
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## References

### Papers and Publications

1. **The Neural Learning Machine (NLM) Project**
   - Initial architecture and design principles
   - Available in the project documentation

2. **Phase 6: Final Integration**
   - Documentation of all systems integration
   - Available in `docs/PHASE6_FINAL_AUDIT.md`

3. **Scientific Background**
   - Available in `docs/SCIENCE.md`

### Related Projects

- [Project Website](https://github.com/nlm-project/nlm)
- [Documentation](https://nlm.readthedocs.io)
- [Discussions](https://github.com/nlm-project/nlm/discussions)

### Acknowledgements

The NLM project builds on ideas from:

- Computational neuroscience literature
- Artificial intelligence research
- Cognitive science studies
- Neural engineering developments

Special thanks to all contributors and researchers who have provided insights and feedback.

## Contact

### Project Authors

For questions, issues, or collaboration:
- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Project Discussions: https://github.com/nlm-project/nlm/discussions
- Email: nlm-project@research.org

### Support

Commercial support and consulting:
- Website: https://nlm-project.org/support
- Email: support@nlm-project.org

### Social Media

- Twitter: @NLM_Project
- LinkedIn: NLM Project

### License

MIT License - See LICENSE file for details

---

*This documentation is continuously updated. For the latest version, check the main branch.*

_Last updated: $(date +%Y-%m-%d)_*