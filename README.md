# NLM — 熙然

**An Experimental Artificial Developmental Brain**

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

## Quick Start

For immediate use, check out the **Easy Usage Guide** (`easy_usage.md`) for beginners, and **How to Use** (`HOW_TO_USE.md`) for comprehensive instructions.

### Basic C++ Usage

```bash
# Build from source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run Phase 6 integration demo
./nlm_phase6_demo

# Run Phase 2 basic neural computation demo  
./nlm
```

### Python Usage

```bash
# Install Python bindings
pip install -e .

# Run Python examples
python -c "
import pynlm
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
for i in range(100):
    brain.step(i)
print(f'Simulation complete! Spikes: {brain.getTotalSpikeCount()}')
"
```

## Project Overview

### Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Key Achievements

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

## Building from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Instructions

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
| `nlm_phase6_demo` | Executable | Phase 6 integration demonstration |
| `nlm_test` | Executable | Unit test suite |

## Python API Quick Reference

### Basic Brain Operations

```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Access brain statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Agent and World Simulation

```python
# Create complete agent-world setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Run agent simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Experiment Examples

### Silent Brain

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Complete Agent with Learning

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

## Advanced Features for Expert Users

### Command Line Options

The main simulation executables support various command-line options:

```bash
# Show help
./nlm --help

# Load custom configuration
./nlm --config my_config.cfg

# Run with specific parameters
./nlm --neuron_count 2000 --connection_probability 0.2

# Enable verbose logging
./nlm --verbose
```

### Configuration File Format

NLM uses a simple key=value format for configuration files:

```ini
# Example configuration file
brain.neuron_count = 2000
brain.synapse_density = 0.15
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
environment.name = GridWorld
environment.width = 30
environment.height = 30
logging.level = INFO
logging.to_file = true
logging.filename = nlm_simulation.log
```

### Advanced Python API

For more advanced usage, the Python bindings support:

- **Custom neuron types and properties**
- **Advanced plasticity rules**
- **Complex environment interactions**
- **Batch processing and machine learning integration**
- **Real-time monitoring and debugging**

See the `src/advanced/` directory for advanced simulation capabilities.

## Development and Contribution

### Project Structure

The NLM project is organized by development phases:

- **Phase 1**: Project skeleton and core infrastructure
- **Phase 2**: Real neural computation with LIF neurons
- **Phase 3**: World interaction and agent systems
- **Phase 4**: Advanced cognitive capabilities
- **Phase 5**: Performance optimizations and parallel processing
- **Phase 6**: Final integration and comprehensive testing

### Adding New Features

To contribute new features:

1. **Read the documentation**: Understand the existing architecture and design patterns
2. **Follow the coding standards**: Use C++20 and the project's coding conventions
3. **Add comprehensive tests**: Include tests for all new functionality
4. **Update documentation**: Document new features in the appropriate documentation files
5. **Submit a pull request**: Include clear descriptions and examples

### Testing

Run the test suite to verify your changes:

```bash
# Run unit tests
cd build
./nlm_test

# Or use ctest
ctest --output-on-failure
```

## License

MIT

## Authors

Research project — See docs for scientific background.

## References

For detailed scientific background and methodology, see the `docs/` directory:

- `ARCHITECTURE.md` - System architecture overview
- `SCIENCE.md` - Scientific background
- `EXPERIMENTS.md` - Experiment descriptions
- `PHASE6_FINAL_AUDIT.md` - Phase 6 audit results
- `PERFORMANCE.md` - Performance analysis

## Getting Help

### Documentation

- **README.md** - This file - Quick start and overview
- **HOW_TO_USE.md** - Comprehensive usage guide
- **easy_usage.md** - Beginner-friendly tutorial
- **docs/** - Technical documentation and scientific background

### Community

For questions and discussions:
- Check the project repository for issues and discussions
- Refer to the documentation for common problems and solutions
- Review the code for examples and patterns

### Troubleshooting

**Common Issues:**

1. **"My brain isn't doing anything"**
   - Did you call `brain.initialize()`?
   - Try increasing the number of steps

2. **"The agent isn't moving"**
   - Did you call `world.applyMotorCommand()`?
   - Check that `world.update()` is being called

3. **"Everything is 0"**
   - Brains need time to "warm up" - try more steps
   - Some neurons need input to fire - make sure sensory input is connected

4. **"ImportError: No module named 'pynlm'"**
   ```bash
   pip install --force-reinstall .
   ```

5. **Segmentation faults**
   - Check that `initialize()` is called before `step()`
   - Ensure `reset()` is called before re-running simulation

## Future Directions

### Research Areas

NLM is an active research project exploring:

- **Neural development and maturation**
- **Memory consolidation and replay**
- **Neuromodulation in learning**
- **Predictive coding and brain models**
- **Embodied cognition and interaction**
- **Scalable neural architectures**

### Software Improvements

Ongoing improvements include:

- **Advanced visualization tools**
- **Real-time monitoring and debugging**
- **Machine learning integration**
- **Cloud deployment capabilities**
- **Enhanced simulation frameworks**
- **Cross-platform compatibility**

## Conclusion

NLM represents a significant step toward understanding how complex cognitive abilities can emerge from relatively simple neural systems through developmental processes and interaction with the environment. The project demonstrates that advanced capabilities like memory, learning, attention, and planning can be implemented using biologically-inspired neural mechanisms.

Phase 6 marks the culmination of years of development, bringing together all previous phases into a coherent, functional artificial brain system. The project continues to evolve, with ongoing research exploring new frontiers in computational neuroscience and artificial intelligence.

---

**Phase 6 Complete - The NLM brain is now a functioning artificial neural substrate capable of changing its own synaptic connections through experience.**