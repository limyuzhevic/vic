#!/usr/bin/env python3
"""
NLM Documentation and User Guide
===============================

This file contains the documentation and user guides for the NLM framework,
including API documentation, usage examples, and best practices.

This documentation is designed to help users get started with NLM and
understand how to use its various features effectively.
"""

# Quick Start Guide
guide = """
# NLM (Neural Learning Machine) Quick Start Guide

## Overview

NLM is an advanced neural network simulation framework that implements
artificial developmental brains capable of learning, memory, and autonomous
development. This documentation provides comprehensive guidance for using
the NLM framework.

## Installation

### Prerequisites

- Python 3.8 or higher
- C++ compiler with C++20 support
- CMake 3.16 or higher

### Installation from Source

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build the framework
mkdir build
cd build
cmake ..
make -j$(nproc)

# Install Python bindings
pip install .
```

### Installation with PyPI

```bash
pip install pynlm
```

## Basic Usage

### Creating a Brain

```python
from pynlm import Config, Brain

# Create configuration
config = Config()
config.set("neuron_count", 100)
config.set("region_count", 2)
config.set("simulation_timestep", 0.001)
config.set("random_seed", 42)

# Create brain
brain = Brain(config)
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Initializing and Simulating

```python
# Initialize brain (if supported)
if brain.initialize():
    print("Brain initialized successfully")
    
    # Run a simulation step
    brain.step(0, 0.0)
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    # Check statistics
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
```

### Running Experiments

```python
from pynlm import ExperimentRunner

# Create experiment runner
runner = ExperimentRunner(config)
runner.setBrain(brain)

# Run different experiments
experiments = ExperimentRunner.getAvailableExperiments()
for exp_name in experiments:
    if exp_name != "all":  # Skip "all" for individual tests
        print(f"\n=== Running Experiment: {exp_name} ===")
        success = runner.runExperiment(exp_name)
        if success:
            print(f"✓ Experiment {exp_name} completed successfully")
        else:
            print(f"✗ Experiment {exp_name} failed")
```

### Learning Experiment

```python
from pynlm import LearningExperiment

# Create learning experiment
experiment = LearningExperiment(brain, 123)

# Record initial state
experiment.recordInitialState()

# Run simulation (simplified)
for step in range(100):
    brain.step(step, step * 0.001)

# Record final state
experiment.recordFinalState()

# Analyze results
experiment.computeStatistics()

# Check if learning occurred
if experiment.didLearningOccur():
    print("✓ Learning detected!")
    print(f"Weight change: {experiment.getWeightChange():.4f}")
    print(f"Strengthened synapses: {experiment.getStrengthenedSynapses()}")
    print(f"Weakened synapses: {experiment.getWeakenedSynapses()}")
```

### Configuration Management

```python
from pynlm import ConfigManager

# Create config manager
manager = ConfigManager(config)

# Set configuration values
manager.setConfigValue("neuron_count", "200")
manager.setConfigValue("region_count", "3")
manager.setConfigValue("simulation_timestep", "0.002")

# Get configuration values
neuron_count = int(manager.getConfigValue("neuron_count", "100"))
region_count = int(manager.getConfigValue("region_count", "1"))
timestep = float(manager.getConfigValue("simulation_timestep", "0.001"))

print(f"Configuration: {neuron_count} neurons, {region_count} regions")
print(f"Timestep: {timestep}s")

# Save configuration to file
manager.saveConfigToFile("my_config.cfg")

# Load configuration from file
new_config = Config()
new_manager = ConfigManager(new_config)
new_manager.loadConfigFromFile("my_config.cfg")
```

### Command Line Interface

```python
from pynlm import CommandLineParser

# Parse command line arguments
parser = CommandLineParser(3, ["test", "--help", "--version"])

# Check for specific options
if parser.hasOption("--help"):
    parser.printHelp()

# Get option values
neuron_count = parser.getOption("--neuron_count", "100")
region_count = parser.getOption("--region_count", "1")
experiment = parser.getOption("--experiment", "all")

print(f"Command line config: {neuron_count} neurons, {region_count} regions")
print(f"Experiment: {experiment}")

# Apply to config
parser.applyToConfig(config)
```

### Visualization Interface

```python
from pynlm import VisualizationInterface, OutputFormat

# Create visualization interface
viz = VisualizationInterface(brain, OutputFormat.Text)
viz.setDimensions(80, 40)

# Render brain state (outputs to console)
viz.renderBrainState()

# Test different output formats
formats = [
    OutputFormat.Text,
    OutputFormat.JSON,
    OutputFormat.CSV,
    OutputFormat.HTML,
    OutputFormat.Plot,
    OutputFormat.ASCII
]

for fmt in formats:
    print(f"\n=== Testing {fmt} format ===")
    viz_fmt = VisualizationInterface(brain, fmt)
    try:
        viz_fmt.renderBrainState()
        print(f"✓ {fmt} format rendered successfully")
    except Exception as e:
        print(f"✗ {fmt} format failed: {e}")
```

### Agent Brain Interface

```python
from pynlm import AgentBrain, SimpleWorld

# Create agent brain
agent_brain = AgentBrain(brain)

# Test agent brain methods
print(f"Sensory input size: {agent_brain.getSensoryInputSize()}")
print(f"Motor output size: {agent_brain.getMotorOutputSize()}")

# Configure neuromodulation
agent_brain.enableRewardModulation(true)
agent_brain.enableCuriosity(true)
agent_brain.enableDevelopment(true)

print(f"Reward modulation enabled: {agent_brain.isRewardModulationEnabled()}")
print(f"Curiosity enabled: {agent_brain.isCuriosityEnabled()}")
print(f"Development enabled: {agent_brain.isDevelopmentEnabled()}")
```

### Complete Example: Neural Network Simulation

```python
from pynlm import Config, Brain, ExperimentRunner, VisualizationInterface

# Step 1: Configure the brain
config = Config()
config.set("neuron_count", 200)
config.set("region_count", 3)
config.set("simulation_timestep", 0.001)
config.set("random_seed", 42)

# Step 2: Create and initialize brain
brain = Brain(config)
if brain.initialize():
    print("✓ Brain initialized successfully")
    
    # Step 3: Run experiments
    runner = ExperimentRunner(config)
    runner.setBrain(brain)
    
    # Run basic connectivity experiment
    print("\n--- Running Basic Connectivity Experiment ---")
    if runner.runExperiment("basic"):
        print("✓ Basic connectivity test passed")
    
    # Reset and run plasticity experiment
    brain.reset()
    brain.initialize()
    
    print("\n--- Running Plasticity Learning Experiment ---")
    if runner.runExperiment("plasticity"):
        print("✓ Plasticity learning experiment passed")
    
    # Reset and run STDP experiment
    brain.reset()
    brain.initialize()
    
    print("\n--- Running STDP Verification Experiment ---")
    if runner.runExperiment("stdp"):
        print("✓ STDP verification experiment passed")
    
    # Step 4: Visualize results
    print("\n--- Brain State Visualization ---")
    viz = VisualizationInterface(brain, OutputFormat.Text)
    viz.setDimensions(100, 50)
    viz.renderBrainState()
    
    print("\n=== All experiments completed successfully! ===")
else:
    print("✗ Brain initialization failed")
```

## Advanced Features

### Python Bindings

The NLM framework provides extensive Python bindings for advanced usage:

```python
# Access all neural components
from pynlm import NeuronType, ActionType, MotorCommand, Vision, Audio

# Use sensory inputs
vision = Vision(16, 16)  # 16x16 vision input
audio = Audio(44100, 1024)  # Audio input

# Use motor actions
action = ActionType.MoveForward
motor_command = MotorCommand.MoveForward
```

### Configuration Management

```python
# Runtime configuration management
config_manager = ConfigManager(config)

# Set runtime parameters
config_manager.setConfigValue("neuron_count", "500")
config_manager.setConfigValue("connection_probability", "0.15")

# Get current values
print(f"Runtime neurons: {config_manager.getConfigValue('neuron_count')}")
print(f"Runtime connection probability: {config_manager.getConfigValue('connection_probability')}")

# Configuration history
history = config_manager.getConfigHistory()
print(f"Recent changes: {len(history)} operations")
```

### Experiment Framework

```python
# Create custom experiments
class CustomExperiment(ExperimentRunner):
    def runCustomExperiment(self):
        # Custom experiment logic
        pass

# Available experiment types
experiments = ["basic", "plasticity", "stdp", "all"]
```

### Visualization Options

```python
# Different visualization formats
from pynlm import OutputFormat

formats = [
    OutputFormat.Text,      # Console text output
    OutputFormat.JSON,      # JSON data export
    OutputFormat.CSV,       # CSV data export
    OutputFormat.HTML,      # HTML visualization
    OutputFormat.Plot,      # Plot format (GUI)
    OutputFormat.ASCII      # ASCII art visualization
]
```

## API Reference

### Core Classes

- **Config**: Configuration management
- **Brain**: Central neural simulation engine
- **AgentBrain**: Agent-world interface
- **SimpleWorld**: Simulation environment
- **ExperimentRunner**: Experiment management
- **LearningExperiment**: Learning analysis
- **VisualizationInterface**: Output visualization
- **ConfigManager**: Runtime configuration
- **CommandLineParser**: Command line argument parsing

### Data Types

- **NeuronType**: Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
- **ActionType**: Various movement and interaction actions
- **MotorCommand**: Low-level motor commands
- **OutputFormat**: Visualization output formats
- **DevelopmentalStage**: Brain development stages

### Usage Patterns

1. **Research and Development**: Use the experiment framework to test neural network behaviors
2. **Educational Purposes**: Use visualization tools to understand neural dynamics
3. **Real-time Applications**: Use agent brain interface for interactive simulations
4. **Configuration Testing**: Use configuration management for parameter sweeps

## Troubleshooting

### Common Issues

1. **Brain Initialization Failures**
   - Check that all required dependencies are installed
   - Verify configuration parameters are valid
   - Ensure sufficient system resources

2. **Visualization Issues**
   - Try different output formats if one fails
   - Check console output for error messages
   - Ensure brain has been properly initialized

3. **Configuration Problems**
   - Use ConfigManager for runtime configuration changes
   - Save and reload configurations to fix corruption
   - Validate configuration values before use

### Getting Help

- **Documentation**: Visit https://nlm.readthedocs.io for detailed documentation
- **Issues**: Report issues at https://github.com/nlm-project/nlm/issues
- **Discussions**: Join discussions at https://github.com/nlm-project/nlm/discussions
- **Support**: Contact the NLM team for professional support

## Best Practices

1. **Always initialize the brain** before running experiments
2. **Use configuration files** for experiment reproducibility
3. **Monitor system resources** during long-running simulations
4. **Use visualization** to debug and understand results
5. **Save brain state** periodically for checkpointing
6. **Test configurations** systematically for parameter exploration
7. **Use the experiment framework** for reproducible research
8. **Document your experiments** for future reference

## Performance Considerations

1. **Brain Size**: Larger brains require more memory and processing time
2. **Simulation Steps**: More simulation steps increase computation time
3. **Experiment Complexity**: Some experiments are more computationally intensive
4. **Output Format**: Some output formats are more resource-intensive

## Future Enhancements

The NLM framework is actively developed with planned enhancements:

1. **Advanced Learning Algorithms**: Incorporate new plasticity rules
2. **Distributed Computing**: Support for multi-GPU and distributed training
3. **Real-time Visualization**: Interactive web-based visualization
4. **Machine Learning Integration**: Bridge with deep learning frameworks
5. **Hardware Acceleration**: GPU and specialized hardware support
6. **Cloud Computing**: Integration with cloud platforms
7. **Container Support**: Docker and containerized deployment

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Acknowledgments

The NLM framework builds on years of research in computational neuroscience,
artificial intelligence, and neural network theory. Special thanks to all
contributors and the open source community.

---

*For more information, visit https://github.com/nlm-project/nlm or consult the official documentation.*
"""

# Print the full guide when executed
if __name__ == "__main__":
    print(guide)