# NLM Enhancement - Quick Start Guide

This document provides a quick start guide for using the enhanced NLM (Neural Learning Machine) project.

## Overview

The NLM project has been comprehensively enhanced with the following improvements:

1. **Enhanced Configuration System** - Support for JSON, YAML, and simple configuration formats
2. **Command Line Interface** - Powerful CLI tool for configuration and simulation management
3. **Error Handling System** - Advanced error reporting with recovery capabilities
4. **Performance Infrastructure** - Memory pools, event-driven processing, multithreading
5. **Comprehensive Documentation** - Detailed guides and examples
6. **Development Tools** - Enhanced debugging and analysis tools

## Installation

### Build from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON

# Build the project
make -j4
```

### Install

```bash
# Install to system
make install

# Or install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

### Python Bindings

```bash
# Install Python bindings
pip install .
```

## Basic Usage

### 1. Create a Configuration File

Create a configuration file (JSON format recommended):

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    },
    "neuromodulation": {
        "curiosity": {
            "enable": true,
            "exploration_rate": 0.5
        }
    }
}
```

Save this as `config.json`.

### 2. Load Configuration and Run Simulation

#### Using Command Line Interface

```bash
# Load configuration
nlm-cli config load config.json --format json --validate

# Create brain
nlm-cli brain create --neurons 1000 --regions 1

# Run simulation
nlm-cli brain step --steps 1000 --monitor

# Get statistics
nlm-cli brain stats --format human
```

#### Using Python API

```python
import nlm

# Create configuration
config = nlm.createDefaultConfig()
config.loadFromFile("config.json")

# Create and initialize brain
brain = nlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Monitor progress
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Save brain state
brain.save("simulation_results.bin")
```

### 3. Configuration Management

#### Create Custom Configuration

```python
import nlm

# Create configuration
config = nlm.createDefaultConfig()

# Set configuration values
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 2)
config.set("brain.connection_probability", 0.15)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("neuromod.curiosity.enable", True)

# Save configuration
config.saveToFile("my_config.json")
```

#### Configuration Templates

```python
import nlm

# Basic template
basic_config = {
    "brain.neuron_count": 100,
    "brain.region_count": 1,
    "brain.connection_probability": 0.05,
    "plasticity.stdp.enable": True,
    "neuromod.curiosity.enable": False
}

# Advanced template
advanced_config = {
    "brain.neuron_count": 1000,
    "brain.region_count": 2,
    "brain.connection_probability": 0.1,
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True,
    "plasticity.structural.enable": True,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True,
    "development.enable": True,
    "memory.working_memory.capacity": 100,
    "memory.episodic_memory.max_episodes": 1000
}

# Apply template
config = nlm.createDefaultConfig()
for key, value in advanced_config.items():
    config.set(key, value)
```

### 4. Error Handling

#### Basic Error Reporting

```python
import nlm

# Report configuration error
nlm.reportConfigError(1001, "Invalid neuron count")

# Report memory error
nlm.reportMemoryError(2001, "Memory allocation failed")

# Report system error
nlm.reportSystemError(3001, "Simulation step failed")
```

#### Error Context with Recovery

```python
import nlm

# Create error context with recovery actions
context = nlm.ErrorContext(nlm.ErrorCategory::Memory, 
                          nlm.ErrorSeverity::Error, 
                          2002, 
                          "Memory pool exhausted",
                          "brain.cpp", 
                          123)
context.addContext("AvailableMemory", "512MB")
context.addContext("PoolSize", "10000")
context.addRecoveryAction("Clear synaptic cache")
context.addRecoveryAction("Reduce neural population")

# Report error
nlm.reportError(context)
```

### 5. Advanced Features

#### Configuration Validation

```bash
# Validate configuration with strict mode
nlm-cli config validate --strict

# Validate and fix errors
nlm-cli config validate --fix
```

#### Auto-Save Configuration

```bash
# Enable auto-save
nlm-cli config autosave checkpoints/config.json 1000 --enabled

# Check auto-save status
nlm-cli config list --format json
```

#### Performance Monitoring

```bash
# Get brain statistics
nlm-cli brain stats --format human
nlm-cli brain stats --format json
nlm-cli brain stats --format csv
```

## Examples

### Example 1: Complete Simulation Workflow

```python
import nlm
import time

def run_simulation(config_file, output_file, num_steps):
    # Load configuration
    config = nlm.createDefaultConfig()
    config.loadFromFile(config_file)
    
    # Create and initialize brain
    brain = nlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent
    agent = nlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    start_time = time.time()
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    end_time = time.time()
    brain.save(output_file)
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")

# Run simulation
run_simulation("config.json", "final_brain.bin", 1000)
```

### Example 2: Configuration Management

```python
import nlm

# Load existing configuration
config = nlm.createDefaultConfig()
config.loadFromFile("my_config.json")

# Get configuration values
neuron_count = config.getOr("brain.neuron_count", 500)
learning_rate = config.getOr("plasticity.stdp.learning_rate", 0.001)

print(f"Configuration loaded:")
print(f"  Neurons: {neuron_count}")
print(f"  Learning rate: {learning_rate}")

# Modify configuration
config.set("brain.neuron_count", 1500)
config.set("plasticity.stdp.learning_rate", 0.002)

# Save modified configuration
config.saveToFile("updated_config.json")
print("Configuration updated and saved!")
```

### Example 3: Error Recovery

```python
import nlm

# Simulate an error scenario
try:
    # Attempt to create a brain with invalid configuration
    config = nlm.createDefaultConfig()
    config.set("brain.neuron_count", -100)  # Invalid value
    
    # This should trigger error recovery
    brain = nlm.createBrain(config)
    brain.initialize()
    
except nlm.NLMConfigurationException as e:
    print(f"Configuration error caught: {e.what()}")
    
    # Attempt recovery
    config.set("brain.neuron_count", 1000)  # Restore valid value
    print("Recovery successful!")
```

## Help and Documentation

### Get Help

```bash
# Get general help
nlm-cli help

# Get help for specific commands
nlm-cli help config
nlm-cli help brain
nlm-cli help simulate

# Get help for subcommands
nlm-cli config help
nlm-cli brain help
```

### Documentation Files

- `ENHANCEMENT_SUMMARY.md` - Complete enhancement documentation
- `docs/ENHANCED_USAGE_GUIDE.md` - Detailed usage guide
- `docs/example_config.md` - Configuration examples
- `configs/example_config.md` - Configuration file examples

## Troubleshooting

### Common Issues

1. **Configuration file not found**

   ```bash
   # Check if file exists
   ls -la config.json
   
   # Create example configuration
   nlm-cli config save config.json
   ```

2. **Invalid configuration values**

   ```bash
   # Validate configuration
   nlm-cli config validate --strict
   
   # View configuration errors
   nlm-cli config get brain.neuron_count
   ```

3. **Brain initialization fails**

   ```bash
   # Check brain configuration
   nlm-cli config list --format table
   
   # Create brain with minimal configuration
   nlm-cli brain create --neurons 10
   ```

4. **Simulation runs too slowly**

   ```bash
   # Reduce brain size
   nlm-cli config set brain.neuron_count 100
   
   # Reduce simulation steps
   nlm-cli brain step --steps 100
   ```

### Error Messages

| Error Message | Cause | Solution |
|---------------|-------|----------|
| "Failed to load configuration from: file" | Configuration file not found | Check file path and format |
| "Key not found: key" | Configuration key doesn't exist | Check key name or set value first |
| "Invalid configuration key or value" | Configuration validation failed | Check key and value format |
| "Brain initialization failed" | Brain initialization failed | Check configuration and try again |

## Future Enhancements

### Planned Improvements

1. **Machine Learning Integration**: Automated configuration optimization using ML
2. **Visualization Tools**: Advanced visualization for neural simulations
3. **Distributed Computing**: Support for distributed neural simulations
4. **Cloud Integration**: Cloud-based simulation deployment
5. **Real-time Monitoring**: Real-time performance monitoring and visualization
6. **Adaptive Learning**: Dynamic adaptation of simulation parameters

### Research Directions

1. **Neuromorphic Computing**: Integration with neuromorphic hardware
2. **Brain-Computer Interfaces**: Real-time interaction with biological systems
3. **Neural Prosthetics**: Development of neural prosthetic systems
4. **Cognitive Modeling**: Advanced cognitive modeling and simulation
5. **Artificial General Intelligence**: Development of AGI capabilities

## Conclusion

The enhanced NLM project provides:

1. **Better Usability**: Intuitive CLI and comprehensive documentation
2. **Improved Performance**: Optimized memory management and computational efficiency
3. **Enhanced Robustness**: Comprehensive error handling and recovery
4. **Greater Flexibility**: Multiple configuration formats and advanced features
5. **Better Maintainability**: Clean code structure and comprehensive testing

The NLM project is now better positioned to serve as a robust platform for neural simulation research and development, with enhanced capabilities for both new users and advanced researchers.

---

*For support or additional questions, refer to the documentation and community resources.*

Keywords: NLM enhancement, configuration system, command line interface, error handling, performance optimization, memory pools, error recovery, neural simulation, documentation, examples, best practices, development tools, debugging, advanced features, NLM CLI, configuration management, error handling system, performance infrastructure, neuroscience simulation, AI research, computational neuroscience