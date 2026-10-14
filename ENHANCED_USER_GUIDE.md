# NLM Enhanced System - User Guide

## Overview

Welcome to the NLM (Neural Learning Machine) Enhanced System! This guide provides comprehensive information on using the enhanced NLM system with all its new features and capabilities.

## What Has Been Improved

### 1. Configuration System
- **Enhanced Parser**: Full JSON/YAML parser with automatic format detection
- **Advanced Types**: Support for matrices, nested objects, and complex data structures
- **Configuration Profiles**: Preset profiles for different use cases (scientific, performance, debug)
- **Validation**: Comprehensive validation with custom rules and error messages
- **Environment Variables**: Support for NLM-prefixed environment variables
- **Statistics**: Configuration statistics and analysis tools

### 2. Main Executable
- **Flexible CLI**: Enhanced command-line interface with multiple experiment types
- **Configuration Management**: Automatic loading, saving, and merging of configurations
- **Experiment Types**: Basic connectivity, plasticity learning, STDP verification, and custom experiments
- **Verbose Output**: Detailed logging and progress reporting
- **Results Management**: Save and load simulation results

### 3. Brain Architecture
- **Simplified Integration**: Cleaner separation of concerns with focused methods
- **Enhanced Functionality**: Complete Phase 6 integrated artificial brain
- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Prediction System**: Advanced temporal sequence learning
- **Cognition Systems**: Planning, attention, and concept formation
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error
- **Development**: Developmental stage management and progression

### 4. Python API
- **Comprehensive Bindings**: Enhanced Python bindings with all C++ functionality
- **Type Safety**: Type-safe getters and parameter validation
- **Error Handling**: Comprehensive error handling and recovery
- **Examples**: Multiple working examples for different use cases
- **Performance**: Performance utilities and benchmarking
- **Integration**: Easy integration with Python ecosystem

### 5. Build System
- **Modern CMake**: C++20 standard compliance with modern CMake practices
- **Cross-Platform**: Better support for different platforms
- **Documentation**: Enhanced build documentation and examples
- **Installation**: Easy installation with package management

## Quick Start Guide

### Basic Usage

#### 1. Create a Brain
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### 2. Run a Simple Simulation
```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
```

#### 3. Learning Simulation
```python
import pynlm

# Configure for learning
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromod.dopamine.scale", 2.0)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=5, visionHeight=5)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)

# Run learning simulation
for step in range(1000):
    world.update(0.05)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward
    reward = world.getReward(action)
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.05)
```

### Advanced Usage

#### 1. Configuration Management
```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Set advanced parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.hebbian.enable", False)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.dopamine.scale", 1.5)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")
```

#### 2. Experiment Types
```python
import pynlm

# Run different experiment types
experiments = {
    "connectivity": pynlm.run_connectivity_experiment,
    "plasticity": pynlm.run_plasticity_experiment,
    "stdp": pynlm.run_stdp_experiment,
    "development": pynlm.run_development_experiment,
}

# Choose experiment
experiment_name = "plasticity"
if experiment_name in experiments:
    brain = experiments[experiment_name]()
    print(f"Experiment completed: {experiment_name}")
```

#### 3. Performance Testing
```python
import pynlm
import time

# Configure for performance testing
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Benchmark brain creation
start_time = time.time()
brain = pynlm.createBrain(config)
brain.initialize()
init_time = time.time() - start_time

# Benchmark simulation
start_time = time.time()
for i in range(1000):
    brain.step(i)
simulation_time = time.time() - start_time

print(f"Initialization: {init_time:.3f}s")
print(f"1000 steps: {simulation_time:.3f}s ({simulation_time/1000:.3f}ms/step)")
```

#### 4. Error Handling
```python
import pynlm

try:
    # Load configuration with error handling
    config = pynlm.safe_load_config("config.json")
    
    # Validate configuration
    if pynlm.validate_config(config):
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("Configuration loaded and validated successfully")
        
except Exception as e:
    print(f"Error: {e}")
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

### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Configuration Profiles

### Default Profile
- **neuron_count**: 1000
- **synapse_density**: 0.1
- **connection_probability**: 0.05
- **plasticity.stdp.enable**: true
- **plasticity.stdp.learning_rate**: 0.001
- **plasticity.hebbian.enable**: true
- **neuromod.dopamine.scale**: 1.0
- **neuromod.curiosity.enable**: true
- **neuromod.novelty.enable**: true

### Scientific Profile
- **neuron_count**: 5000
- **synapse_density**: 0.15
- **connection_probability**: 0.05
- **plasticity.stdp.enable**: true
- **plasticity.stdp.learning_rate**: 0.001
- **plasticity.hebbian.enable**: true
- **neuromod.dopamine.scale**: 1.0
- **neuromod.curiosity.enable**: true
- **neuromod.novelty.enable**: true
- **simulation.timestep**: 0.0001

### Performance Profile
- **neuron_count**: 2000
- **synapse_density**: 0.08
- **connection_probability**: 0.02
- **plasticity.stdp.enable**: true
- **plasticity.stdp.learning_rate**: 0.0005
- **plasticity.hebbian.enable**: false
- **neuromod.dopamine.scale**: 0.5
- **neuromod.curiosity.enable**: false
- **neuromod.novelty.enable**: false
- **simulation.timestep**: 0.001

### Debug Profile
- **neuron_count**: 100
- **synapse_density**: 0.2
- **connection_probability**: 0.1
- **plasticity.stdp.enable**: true
- **plasticity.stdp.learning_rate**: 0.01
- **plasticity.hebbian.enable**: true
- **neuromod.dopamine.scale**: 2.0
- **neuromod.curiosity.enable**: true
- **neuromod.novelty.enable**: true
- **logging.level**: debug
- **checkpoint.enabled**: false

## Experiment Types

### 1. Basic Connectivity Test
Tests basic neural connectivity by injecting current into neurons and measuring spike propagation.

### 2. Plasticity Learning Experiment
Enables plasticity on synapses and applies repeated input patterns to stimulate learning.

### 3. STDP Verification
Tests spike-timing dependent plasticity by creating correlated pre-post activity patterns.

### 4. Custom Experiment
Allows users to create custom experiments with additional parameters.

## Performance Considerations

### Recommended Settings

#### For Research
- **neuron_count**: 5000-10000
- **synapse_density**: 0.15
- **simulation.timestep**: 0.0001
- **plasticity.stdp.enable**: true
- **plasticity.hebbian.enable**: true
- **neuromod.curiosity.enable**: true

#### For Production
- **neuron_count**: 1000-2000
- **synapse_density**: 0.1
- **simulation.timestep**: 0.001
- **plasticity.stdp.enable**: true
- **plasticity.hebbian.enable**: false
- **neuromod.curiosity.enable**: false

#### For Development/Debugging
- **neuron_count**: 100-500
- **synapse_density**: 0.2
- **simulation.timestep**: 0.01
- **plasticity.stdp.enable**: true
- **plasticity.hebbian.enable**: true
- **neuromod.dopamine.scale**: 2.0
- **logging.level**: debug

## Troubleshooting

### Common Issues

#### "My brain isn't doing anything"
- **Check**: Did you call `brain.initialize()` before `brain.step()`?
- **Try**: Increase the number of steps or add sensory input

#### "The agent isn't moving"
- **Check**: Did you call `world.applyMotorCommand()`?
- **Try**: Check that `world.update()` is being called

#### "Everything is 0"
- **Check**: Brains need time to "warm up" - try more steps
- **Try**: Add sensory input to neurons

#### "Configuration loading fails"
- **Check**: Ensure the file exists and is readable
- **Try**: Use the `safe_load_config()` function with error handling

### Getting Help

#### Command Line Help
```bash
nlm --help
```

#### Documentation
- `README.md` - Basic usage information
- `HOW_TO_USE.md` - Detailed usage guide
- `easy_usage.md` - Quick start guide
- `docs/ARCHITECTURE.md` - System architecture
- `docs/SCIENCE.md` - Scientific background
- `docs/EXPERIMENTS.md` - Experiment descriptions

#### Examples
- `examples/basic.py` - Basic simulation example
- `examples/agent.py` - Complete agent simulation
- `examples/learning.py` - Learning simulation
- `examples/performance.py` - Performance benchmark

## Version Information

- **Version**: NLM Enhanced System v2.0.0
- **C++ Standard**: C++20
- **Build Type**: Release
- **Python Bindings**: Pybind11
- **Configuration Formats**: JSON, YAML, INI, TOML
- **Documentation**: Comprehensive

## License

This software is provided under the MIT License. See the LICENSE file for details.

## Acknowledgments

- **Contributors**: NLM Development Team
- **Sponsors**: NLM Project Supporters
- **Support**: For issues and questions, please visit the NLM GitHub repository

## Support

For technical support, please visit:
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Issues**: https://github.com/nlm-project/nlm/issues
- **Discussions**: https://github.com/nlm-project/nlm/discussions

---

**Last Updated**: October 2026

**Next Version**: 2.1.0 (planned features include:
- Multi-agent systems
- Reinforcement learning algorithms
- Advanced visualization tools
- Cloud integration capabilities
)