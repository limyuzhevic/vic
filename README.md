# NLM (Neural Learning Machine) - Python Package

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain project that implements real spiking neural computation with Leaky Integrate-and-Fire (LIF) neurons, event-driven spike propagation, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This repository provides a comprehensive Python interface to the NLM C++ neural simulation framework, making it accessible for experimentation and research in computational neuroscience and artificial intelligence.

## Key Features

### Core Functionality
- **Real Spiking Neural Networks**: LIF neurons with event-driven dynamics
- **Multiple Plasticity Mechanisms**: STDP, Hebbian learning, structural plasticity
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty detection, prediction error
- **Cognitive Systems**: Planning, concept formation, attention
- **Development**: Developmental stages and plasticity modulation

### Python Interface
- **User-Friendly API**: Intuitive Python classes and methods
- **Builder Pattern**: Fluent interface for configuration
- **Simulation Runner**: High-level simulation management
- **Power User Tools**: Advanced debugging and analysis tools

### Advanced Features
- **Performance Monitoring**: Real-time performance tracking
- **Custom Environments**: User-defined simulation environments
- **Experimental Design**: Scientific experiment management
- **Visualization Tools**: Plotting and analysis utilities
- **Debugging Support**: Comprehensive debugging capabilities

## Installation

### Quick Installation

```bash
pip install -e .
```

### Development Installation

```bash
pip install -e .[dev]
```

### Advanced Features

```bash
pip install -e .[dev,documentation]
```

## Package Structure

```
/workspace/app/
├── .github/
│   └── workflows/                    # CI/CD workflows
├── docs/                              # Documentation
├── examples/                          # Example scripts
├── nlm/                               # Python package
│   ├── __init__.py                   # Package initialization
│   ├── __main__.py                   # Command-line interface
│   ├── pynlm.py                      # Core Python interface
│   ├── nlm_examples.py               # Example demonstrations
│   └── nlm_power_user.py             # Power user tools
├── python/                           # C++ Python bindings
│   └── bindings.cpp                  # Pybind11 bindings
├── src/                              # C++ source code
│   ├── agent/                        # Agent system
│   ├── brain/                        # Neural brain components
│   ├── core/                         # Core utilities
│   ├── experiments/                  # Experiment framework
│   ├── memory/                       # Memory systems
│   ├── neuromodulation/               # Neuromodulators
│   └── ...                           # Other subsystems
├── tests/                            # Test suite
│   ├── test_*.cpp                    # C++ unit tests
│   └── test_*.py                     # Python tests
├── CMakeLists.txt                    # C++ build configuration
├── pyproject.toml                   # Python package configuration
├── setup.py                         # Setup script
└── LICENSE                           # Software license
```

## Quick Start Guide

### 1. Basic Usage

```python
import nlm

# Create a brain with default configuration
brain = nlm.create_brain()
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Generated {brain.getTotalSpikeCount()} spikes")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### 2. Agent-World Simulation

```python
import nlm

# Create brain, world, and agent
brain = nlm.BrainBuilder().with_neuron_count(500).build().get()
brain.initialize()

world = nlm.create_simple_world()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent = nlm.create_agent_brain(brain)
agent.initialize(world)

# Run simulation
runner = nlm.SimulationRunner(agent, world)
stats = runner.run_episode(500, timestep=0.1, log_interval=100)

print(f"Simulation completed with curiosity: {stats['agent_stats']['curiosity_level']:.3f}")
```

### 3. Advanced Builder Pattern

```python
import nlm

# Create brain with advanced configuration
brain = (nlm.BrainBuilder()
    .with_neuron_count(1000)
    .with_plasticity(stdp_enabled=True, hebbian_enabled=True, structural_enabled=True)
    .build().get())

# Create agent with learning
agent = (nlm.AgentBuilder()
    .configure_world(width=15, height=15, vision_width=8, vision_height=8)
    .with_start_position(7.5, 7.5)
    .enable_learning(reward_modulation=True, curiosity=True)
    .build().get())

# Run complete simulation
runner = nlm.SimulationRunner(agent, agent.world)
results = runner.run_episode(1000, timestep=0.1)
```

## Examples

### Example 1: Silent Brain
```python
from nlm_examples import run_silent_brain_example

result = run_silent_brain_example()
print(f"Brain generated {result['total_spikes']} spikes")
```

### Example 2: Agent-World Simulation
```python
from nlm_examples import run_agent_world_example

result = run_agent_world_example()
print(f"Agent explored for {result['steps_completed']} steps")
```

### Example 3: Learning Demonstration
```python
from nlm_examples import run_learning_demonstration

result = run_learning_demonstration()
print(f"Learning completed with curiosity: {result['agent_stats']['final_curiosity']:.3f}")
```

### Example 4: Power User Advanced Simulation
```python
from nlm_power_user import create_advanced_simulation, PerformanceMonitor

# Create advanced simulation
sim = create_advanced_simulation({
    'neuron_count': 500,
    'learning_rate': 0.002
})

# Run with monitoring
monitor = sim['monitor']
monitor.start_monitoring()

for step in range(100):
    sim['agent'].processSensoryInput(sim['world'].getSensoryPercept())
    sim['agent'].getBrain().step(step)
    sim['world'].update(0.1)
    monitor.record_step(step, sim['agent'].getBrain(), sim['world'])

summary = monitor.stop_monitoring()
print(f"Average step time: {summary['avg_step_time']:.4f}s")
```

## Power User Features

### Performance Monitoring
```python
from nlm_power_user import PerformanceMonitor

monitor = PerformanceMonitor()
monitor.start_monitoring()

# Run simulation
for step in range(1000):
    brain.step(step)
    monitor.record_step(step, brain, world)

summary = monitor.stop_monitoring()
print(f"Performance summary: {summary}")
```

### Advanced Brain Building
```python
from nlm_power_user import AdvancedBrainBuilder

brain = (AdvancedBrainBuilder()
    .add_region('cortex', 'excitatory', 200)
    .add_region('hippocampus', 'inhibitory', 50)
    .set_connectivity('cortex', 'hippocampus', 0.2)
    .set_plasticity_rules('stdp', {'learning_rate': 0.005})
    .build())
```

### Custom Environments
```python
from nlm_power_user import CustomSimulationEnvironment

# Design custom environment
env = CustomSimulationEnvironment(width=30, height=30)
env.add_object('resource', 10, 10, {'value': 10.0})
env.add_object('hazard', 20, 20, {'value': -5.0})
```

### Experiment Design
```python
from nlm_power_user import ExperimentDesigner

# Design scientific experiment
exp = ExperimentDesigner("Plasticity Study")
exp.add_parameter("neuron_count", [100, 200, 300])
exp.add_parameter("learning_rate", [0.001, 0.01])
exp.add_trial("trial_1", {"neuron_count": 200, "learning_rate": 0.01}, "control")
exp.add_trial("trial_2", {"neuron_count": 300, "learning_rate": 0.001}, "experimental")

results = exp.run_experiment()
analysis = exp.analyze_results(results)
```

## Command Line Interface

### Available Commands

```bash
nlm check                    # Check environment setup
nlm demo                     # Run quick demo
nlm example silent           # Run silent brain example
nlm example agent-world      # Run agent-world example
nlm example learning         # Run learning demonstration
nlm example curiosity        # Run curiosity example
nlm example plasticity       # Run plasticity demonstration
nlm example all              # Run all examples
nlm docs                     # Build documentation
```

### Usage Examples

```bash
# Check if environment is properly set up
nlm check

# Run a quick demonstration
nlm demo

# Run specific examples
nlm example silent
nlm example all
```

## Testing

### Running Tests

```bash
# Run all tests
python nlm_test.py

# Run specific test modules
python -m pytest nlm_examples.py -v
python -m pytest nlm_power_user.py -v

# Run C++ tests (if built)
cd build && ctest --output-on-failure
```

### Test Coverage

The package includes comprehensive tests:

1. **Basic Functionality Tests**: Brain creation, initialization, simulation
2. **Builder Pattern Tests**: ConfigBuilder, BrainBuilder, AgentBuilder
3. **Simulation Tests**: Complete agent-world simulations
4. **Power User Tests**: Performance monitoring, advanced features
5. **Example Tests**: All example demonstrations

## Performance Considerations

### Optimization Tips

1. **Batch Processing**: Process multiple steps before world updates
2. **Memory Management**: Monitor memory usage
3. **Parallel Processing**: Use multiple threads for independent simulations
4. **Hardware Acceleration**: Leverage GPU capabilities when available

### Scaling Guidelines

- **Small Scale** (< 100 neurons): Basic builders and simple environments
- **Medium Scale** (100-1000 neurons): Standard builders with monitoring
- **Large Scale** (> 1000 neurons): Advanced builders and custom environments
- **Research Scale** (> 10000 neurons): Specialized hardware and distributed computing

## Troubleshooting

### Common Issues

**Q: "ModuleNotFoundError: No module named 'nlm'"**
A: Install the package properly:
```bash
pip install -e .
```

**Q: "ImportError: libstdc++.so.6: cannot open shared object file"**
A: Ensure C++ dependencies are installed:
```bash
# Ubuntu/Debian
sudo apt-get update && sudo apt-get install g++-9

# macOS (Homebrew)
brew install gcc
```

**Q: "Segmentation fault"**
A: Check for null pointer dereferences, ensure proper initialization, verify memory allocation.

**Q: "Performance issues"**
A: Use `PerformanceMonitor` to identify bottlenecks, reduce simulation complexity.

### Getting Help

#### Documentation
- **Documentation**: https://nlm.readthedocs.io
- **Examples**: See `nlm_examples.py`
- **Power User Guide**: See `NLM_POWER_USER_DOCS.md`

#### Community Support
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: Project discussions forum

## License

This project is licensed under the MIT License. See `LICENSE` file for details.

## Acknowledgments

The NLM project acknowledges:
- Open source community contributions
- Researchers in computational neuroscience
- Users who provide feedback and report issues
- Supporters of artificial intelligence research

## Future Development

### Planned Features
1. Enhanced visualization tools
2. Distributed computing support
3. Machine learning framework integration
4. Cloud deployment capabilities
5. Real-time interaction features

### Research Directions
1. Neuromorphic computing integration
2. Explainable AI tools
3. Transfer learning applications
4. Cognitive modeling extensions
5. Developmental psychology applications

## Contact Information

For support, questions, or collaboration opportunities:

- **Email**: support@nlm-project.org
- **GitHub**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io

---

*Version: 0.1.0*
*Last Updated: October 2026*
*License: MIT*