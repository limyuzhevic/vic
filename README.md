# pynlm - Python Interface to NLM (Neural Learning Machine)

## Overview

The pynlm package provides a comprehensive Python interface to the NLM (Neural Learning Machine) artificial brain simulation framework. It enables researchers and developers to create, simulate, and experiment with artificial developmental brain systems using Python.

NLM is an experimental computational brain project that implements real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems. The Python interface provides access to the high-performance C++ simulation engine while offering a user-friendly, research-oriented programming experience.

## Key Features

### Core Neural Computation
- **LIF Neurons**: Leaky Integrate-and-Fire neuron dynamics with realistic membrane properties
- **Event-Driven Processing**: Efficient spike propagation with synaptic delays and timing
- **Multiple Plasticity Rules**: STDP (spike-timing dependent plasticity), Hebbian learning, and structural plasticity
- **Realistic Neural Dynamics**: Biologically plausible neuron models and synaptic mechanisms

### Integrated Memory Systems
- **Working Memory**: Transient storage for active neural patterns and attentional focus
- **Episodic Memory**: Experience recording, replay, and consolidation
- **Associative Memory**: Pattern association, recall, and memory binding
- **Memory Integration**: Coordinated interaction between memory systems

### Cognitive Architecture
- **Prediction Systems**: Forward models and prediction error computation
- **Neural Planning**: Action sequence evaluation and decision making
- **Concept Formation**: Pattern discovery and abstraction
- **Attention Mechanisms**: Competitive selection and cognitive focus

### Neuromodulation
- **Dopamine**: Reward prediction error processing and reinforcement learning
- **Curiosity**: Exploration motivation and novelty detection
- **Novelty**: Novelty detection and surprise signals
- **Prediction Error**: Forward model error computation

### Development and Maturation
- **Developmental Stages**: Age-dependent plasticity control (Initial, Critical Period, Maturation, Adult, Aging)
- **Maturation**: Cognitive function development over time
- **Critical Periods**: Sensitive periods for learning and plasticity
- **Adaptive Plasticity**: Changing learning rates during development

### Environment Interaction
- **World Simulation**: 2D continuous environments with physics
- **Sensory Perception**: Vision, proprioception, and internal sensing
- **Motor Actions**: Low-level motor commands and execution
- **Agent Interface**: Complete perception-action loops

### Performance Optimizations
- **SIMD Vectorization**: Parallel processing of neural operations
- **Memory Pools**: Efficient allocation/deallocation
- **Event-Driven Architecture**: Optimized spike propagation
- **Checkpoint System**: Persistent state management
- **Configurable Thread Pools**: Parallel processing support

## Package Structure

```
pynlm/
├── __init__.py                    # Package entry point and public API
├── brain.py                       # Core brain simulation
│   ├── Brain                      # Main brain class
│   ├── Config                     # Configuration system
│   ├── NeuralRegion              # Brain regions
│   └── NeuralPopulation          # Neuron populations
├── world.py                       # World simulation
│   ├── World                      # Environment simulation
│   ├── WorldObject               # World entities
│   ├── SensoryPercept            # Sensory input
│   └── ActionResult             # Action results
├── agent.py                       # Agent brain interface
│   ├── AgentBrain                 # Brain-world interface
│   ├── AgentBody                 # Physical agent state
│   └── MotorCommand             # Motor commands
├── exceptions.py                  # Exception classes
├── utils.py                       # Utility functions
│   ├── ErrorHandler              # Error handling
│   ├── DebugLogger              # Debug logging
│   ├── PerformanceMonitor       # Performance tracking
│   └── get_version()            # Package info
├── types.py                       # Type definitions
│   ├── NeuronId, SynapseId, etc. # Unique identifiers
│   ├── NeuronType, SynapseType  # Enumeration types
│   ├── ActionType, MotorCommand # Action types
│   ├── DevelopmentalStage      # Development stages
│   ├── FiringState             # Neuron firing states
│   └── PhysicsConstants       # Physical constants
└── test_pynlm.py                  # Package test suite
```

## Installation

### From PyPI (Recommended)

```bash
pip install pynlm
```

### From Source

```bash
# Clone the repository
cd /path/to/nlm

# Build C++ extensions
cmake -B build
cmake --build build -j4

# Install Python package
pip install -e .
```

### Development Installation

```bash
# Install in development mode
cd /path/to/nlm
pip install -e .

# Install development dependencies
pip install -r requirements.txt
```

## Quick Start

### Basic Brain Simulation

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simulation
for step in range(1000):
    brain.step(step)

# Monitor brain activity
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent-World Simulation

```python
import pynlm

# Create brain and configure for learning
config = pynlm.createDefaultConfig()
config.neuron_count = 500
config.region_count = 1
brain = pynlm.createBrain(config)
brain.initialize()

# Create world environment
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, vision_width=20, vision_height=20)

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# Set up interesting environment
world.addObject(pynlm.WorldObject(25, 25, pynlm.WorldObjectType.Resource, 50.0, 2.0))
world.addObject(pynlm.WorldObject(75, 75, pynlm.WorldObjectType.Hazard, 15.0, 2.0))

# Main interaction loop
for step in range(500):
    world.update(0.1)  # Update environment
    percept = world.getSensoryPercept()  # Get sensory input
    agent.processSensoryInput(percept)   # Brain perceives
    brain.step(step)                     # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides
    result = world.applyMotorCommand(action, world.getSimulationTime())  # Act
    
    # Learn from experience
    agent.applyRewardModulation(result.reward, agent.getPredictionError())
    
    # Log occasionally
    if step % 50 == 0:
        print(f"Step {step}: Action {action.name}, Reward {result.reward:+.1f}")
```

### Advanced Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Basic brain parameters
config.neuron_count = 1000
config.region_count = 2
config.connection_probability = 0.1
config.simulation_timestep = 0.001

# Plasticity parameters
config.stdp_ltp_weight = 0.02
config.stdp_ltd_weight = 0.012
config.stdp_tau = 20.0
config.hebbian_weight = 0.01

# Structural plasticity
config.synaptogenesis_rate = 0.0001
config.pruning_rate = 0.00001

# Development
config.developmental_stage = "Adult"
config.critical_period_end = 100.0

# Neuromodulation
config.dopamine_enabled = True
config.curiosity_enabled = True
config.novelty_enabled = True

# Performance
config.enable_vectorization = True
config.enable_parallel = True
config.checkpoint_interval = 1000

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

## Core API

### Brain Module

```python
# Main brain class
brain = pynlm.createBrain(config)
brain.initialize()

# Simulation
brain.step(step)
brain.reset()

# Configuration
config = brain.get_config()
brain.set_config(new_config)

# Statistics
firing_rate = brain.getAverageFiringRate()
total_spikes = brain.getTotalSpikeCount()
active_neurons = brain.getActiveNeuronCount()
regions = brain.getRegionIds()
```

### World Module

```python
# World simulation
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100, vision_width=20, vision_height=20)

# Environment management
world.update(timestep)
world.addObject(obj)
world.removeObject(x, y)

# Perception
percept = world.getSensoryPercept()
vision_data = percept.get_vision(x, y)

# Action execution
result = world.applyMotorCommand(action, time)
reward = result.reward
success = result.success
```

### Agent Module

```python
# Agent brain interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Learning systems
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_development(True)

# Processing
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)
agent.updateDevelopment(timestep)

# State access
stage = agent.get_developmental_stage()
curiosity = agent.get_curiosity_level()
dopamine = agent.get_neuromodulation_level()
```

### Configuration

```python
# Configuration system
config = pynlm.createDefaultConfig()

# Access configuration values
neuron_count = config.neuron_count
connection_prob = config.connection_probability
simulation_timestep = config.simulation_timestep

# Set configuration values
config.neuron_count = 1000
config.set("stdp_ltp_weight", 0.05)

# Validation
config_dict = {
    'neuron_count': config.neuron_count,
    'simulation_timestep': config.simulation_timestep,
}
errors = pynlm.validate_config(config_dict)
```

## Error Handling

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.NLMError as e:
    print(f"NLM error: {e}")
```

## Examples and Demos

The package includes several example scripts:

### Basic Examples
- `examples/01_hello_brain.py`: Simple brain simulation
- `examples/02_brain_world.py`: Brain-world interaction
- `examples/03_agent_learning.py`: Complete agent learning loop

### Advanced Examples
- `examples/04_memory_systems.py`: Working with memory systems
- `examples/05_plasticity_experiments.py`: Plasticity and learning experiments

### Running Examples

```bash
python examples/01_hello_brain.py
python examples/02_brain_world.py
python examples/03_agent_learning.py
python examples/04_memory_systems.py
python examples/05_plasticity_experiments.py
```

## Testing

### Running the Test Suite

```bash
# Run all tests
python -m pytest tests/ -v

# Run specific test modules
python -m pytest tests/test_brain.py -v
python -m pytest tests/test_agent.py -v
python -m pytest tests/test_world.py -v
python -m pytest tests/test_types.py -v

# Run with coverage
python -m pytest tests/ --cov=pynlm --cov-report=html
```

### Test Coverage

The test suite includes:
- **Unit Tests**: Testing individual components and classes
- **Integration Tests**: Testing component interactions
- **Performance Tests**: Testing with large-scale simulations
- **Regression Tests**: Ensuring no functionality is broken

## Command Line Interface

The NLM package includes a command-line interface for common operations:

### Basic Commands

```bash
# Show version information
nlm-cli --version

# Show system information
nlm-cli --info

# Run a quick demo
nlm-cli demo --steps 100

# Run the test suite
nlm-cli test
```

### Advanced Commands

```bash
# Run all example scripts
nlm-cli --examples

# Build the package
nlm-cli build
```

### Command Line Options

```bash
nlm-cli [OPTIONS] COMMAND

Options:
  --version           Show version information and exit
  --info              Show system information and exit
  --examples          Run all example scripts
  --steps INTEGER     Number of simulation steps for demo (default: 100)

Commands:
  help                Show this help message
  run-examples        Run all example scripts
  demo                Run a demo simulation
  test                Run the test suite
  build               Build the package
```

## Development

### Setting Up Development Environment

```bash
# Install development dependencies
pip install -r requirements.txt

# Install in development mode
pip install -e .
```

### Code Style

The project follows these coding standards:
- **Python Style**: PEP 8 with black formatting
- **Type Hints**: Comprehensive type annotations
- **Documentation**: Docstrings with type hints
- **Testing**: Unit tests with good coverage

### Code Quality Checks

```bash
# Check code style
black --check python/
flake8 python/
mypy python/

# Format code
black python/

# Run tests
pytest tests/ -v

# Type checking
mypy python/
```

### Building and Packaging

```bash
# Build source distribution and wheel
python setup.py sdist bdist_wheel

# Install from built package
pip install dist/pynlm-*.whl

# Build documentation
sphinx -b html docs/ docs/_build/html/
```

## Performance Considerations

### Large-Scale Simulations

The NLM framework is optimized for large-scale simulations:

- **Memory Efficiency**: Uses memory pools for efficient allocation
- **Parallel Processing**: Supports SIMD and multi-threading
- **Event-Driven**: Efficient spike propagation
- **Checkpointing**: Save and resume large simulations

### Performance Tips

1. **Use appropriate configuration**: Match configuration to simulation size
2. **Enable optimizations**: SIMD and parallel processing
3. **Use checkpointing**: For long-running simulations
4. **Monitor performance**: Use built-in performance monitoring

### Scaling Guidelines

- **Small simulations**: < 100 neurons, < 1000 steps
- **Medium simulations**: 100-1000 neurons, 1000-10000 steps
- **Large simulations**: 1000+ neurons, 10000+ steps
- **Massive simulations**: 10000+ neurons, 100000+ steps

## Research Applications

### Cognitive Science Research

- **Memory formation**: Study how memories are formed and consolidated
- **Learning mechanisms**: Investigate different learning rules and their effects
- **Attention and selection**: Explore how the brain focuses attention
- **Decision making**: Study action selection and planning

### Artificial Intelligence Research

- **Developmental AI**: Create systems that develop from simple beginnings
- **Neuromodulation**: Study the role of reward and curiosity
- **Meta-learning**: Investigate learning how to learn
- **Transfer learning**: Study generalization across tasks

### Neuroscience Research

- **Neural dynamics**: Study spiking neuron dynamics
- **Plasticity**: Investigate synaptic plasticity mechanisms
- **Network dynamics**: Explore emergent properties in neural networks
- **Memory systems**: Study the neural basis of memory

## Future Directions

### Planned Enhancements

1. **3D World Simulation**: Extension to three-dimensional environments
2. **Sensory Modalities**: Additional sensory channels (audio, proprioception)
3. **Social Learning**: Multi-agent coordination and social learning
4. **Gene Expression**: Biological-inspired gene regulatory networks
5. **Hardware Acceleration**: GPU and specialized hardware support
6. **Cloud Integration**: Distributed simulation across multiple machines

### Research Areas

- **Neuromorphic Computing**: Hardware implementations of NLM
- **Brain-Computer Interfaces**: Real-world applications
- **Cognitive Architecture**: General intelligence principles
- **Developmental Psychology**: Computational models of cognitive development

## Contributing

### Contribution Guidelines

1. **Code quality**: Follow PEP 8 and write comprehensive tests
2. **Documentation**: Keep documentation up to date
3. **Code reviews**: Participate in peer review process
4. **Testing**: Ensure no regressions are introduced

### Getting Started

1. **Fork the repository**: Create your own copy
2. **Create a branch**: Work on specific features
3. **Make changes**: Implement your changes
4. **Write tests**: Add tests for new functionality
5. **Update documentation**: Document your changes
6. **Submit a pull request**: Merge your changes into main

### Code of Conduct

This project follows the NeurorSE Code of Conduct. Please be respectful and
constructive in all interactions.

## License

NLM is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgements

The NLM project is developed as part of a research effort to create
artificial systems that develop and learn in a manner inspired by biological
brains. We acknowledge:

- The open-source community for valuable libraries and tools
- Research funding agencies supporting brain-inspired AI
- Contributors who have provided feedback and reported issues
- Users who have provided valuable feedback and used the system in research

## Contact

- **Website**: https://nlm.org
- **Documentation**: https://docs.nlm.org
- **GitHub**: https://github.com/nlm-project/nlm
- **Email**: contact@nlm.org
- **Discussions**: https://github.com/nlm-project/nlm/discussions

The NLM project represents a step toward creating artificial systems that
develop and learn in a manner inspired by biological brains.
