# NLM Examples Directory

This directory contains comprehensive example scripts demonstrating the NLM (Neural Learning Machine) Python API, with emphasis on the new convenience methods and enhanced features introduced in the improved API.

## Overview

The examples showcase the full capabilities of the NLM Python bindings, including:
- New convenience methods for Configuration, Brain, and AgentBrain classes
- Comprehensive error handling and robust programming practices
- Advanced simulation scenarios and real-world use cases
- Working code examples that can be copied, modified, and extended

## Example Scripts

### 1. example1_basic_convenience.py
**Basic Usage with New Convenience Methods**

This script demonstrates the core convenience methods available in the NLM API:

**New Features Demonstrated:**
- Config convenience methods: `getInt()`, `getFloat()`, `getBool()`, `getString()`
- Brain convenience methods: `getNeuronCountByType()`, `getBrainStats()`
- Safe configuration access with error handling

**What you'll learn:**
- How to use type-safe configuration retrieval
- How to get neuron counts by type (excitatory, inhibitory, sensory, motor)
- How to retrieve comprehensive brain statistics
- Basic error handling patterns

**Usage:**
```bash
python example1_basic_convenience.py
```

### 2. example2_safe_config.py
**Configuration Management with Error Handling**

This script demonstrates robust configuration management practices with comprehensive error handling:

**New Features Demonstrated:**
- Safe configuration value setting and retrieval
- Type-aware configuration methods with default values
- File I/O operations with error checking
- Configuration validation and conversion

**What you'll learn:**
- How to safely modify configuration values
- How to handle missing or invalid configuration keys
- How to load and save configuration files
- How to use configuration defaults for missing values
- Error handling for file operations

**Usage:**
```bash
python example2_safe_config.py
```

### 3. example3_brain_stats.py
**Brain Statistics and Analysis**

This script demonstrates comprehensive brain statistics analysis during simulation:

**New Features Demonstrated:**
- Dynamic brain statistics collection during simulation
- Neuron type analysis using `getNeuronCountByType()`
- Statistical analysis and summary calculations
- JSON output for results persistence

**What you'll learn:**
- How to collect and analyze brain statistics during simulation
- How to track neuron populations by type
- How to calculate and track simulation metrics
- How to export analysis results for further processing
- Statistical summary calculations

**Usage:**
```bash
python example3_brain_stats.py
```

### 4. example4_agent_features.py
**Agent Behavior with Enhanced Features**

This script demonstrates the AgentBrain convenience methods and complete agent simulation:

**New Features Demonstrated:**
- AgentBrain convenience methods: `getAgentStats()`, `enableAllSystems()`, `getSystemStatus()`
- Action recommendations based on brain state
- Comprehensive agent behavior tracking
- Complete agent-world interaction simulation

**What you'll learn:**
- How to use the new AgentBrain convenience methods
- How to enable/disable all agent systems at once
- How to get action recommendations based on brain state
- How to track and analyze agent behavior patterns
- How to simulate complete agent-environment interaction

**Usage:**
```bash
python example4_agent_features.py
```

### 5. example5_complete_simulations.py
**Complete Simulation Examples**

This script demonstrates advanced simulation scenarios including neurogenesis, learning, prediction/planning, and memory consolidation:

**New Features Demonstrated:**
- Multiple simulation scenarios in one script
- Integration of new convenience methods across different use cases
- Advanced simulation metrics tracking
- Development stage analysis

**What you'll learn:**
- How to run multiple simulation scenarios
- How to track development stages and metrics
- How to integrate different aspects of the NLM brain
- How to analyze simulation results comprehensively

**Usage:**
```bash
python example5_complete_simulations.py
```

## Common Patterns and Best Practices

### Error Handling

All example scripts demonstrate robust error handling patterns:

1. **Safe Configuration Access**: Use try-catch blocks around configuration operations
2. **Safe Statistics Retrieval**: Handle cases where methods might fail
3. **Default Values**: Always provide sensible defaults for missing values
4. **File Operations**: Check if operations succeed before proceeding

### Using Convenience Methods

**Before (Old API):**
```python
# Basic statistics
firing_rate = brain.getAverageFiringRate()
neuron_count = brain.getTotalNeuronCount()

# Configuration
if config.has("brain.neuron_count"):
    value = config.getInt("brain.neuron_count", 1000)
```

**After (New API):**
```python
# Enhanced statistics with convenience methods
brain_stats = brain.getBrainStats()
firing_rate = brain_stats.get('firing_rate', 0.0)

# Neuron type analysis
excitatory = brain.getNeuronCountByType(pynlm.NeuronType.Excitatory)
inhibitory = brain.getNeuronCountByType(pynlm.NeuronType.Inhibitory)

# Simplified configuration access
neuron_count = config.getInt("brain.neuron_count", 1000)
```

### Simulation Best Practices

1. **Initialize Properly**: Always call `initialize()` before starting simulation
2. **Track Metrics**: Use the new convenience methods to track simulation metrics
3. **Error Recovery**: Implement error handling for robust simulations
4. **Configuration**: Use the new convenience methods for configuration management

## Running Examples

### Prerequisites

1. Install NLM Python bindings:
```bash
pip install pynlm
```

2. Ensure the NLM C++ library is built and installed

### Running Individual Examples

```bash
# From the examples directory
python example1_basic_convenience.py

# Or from the workspace root
python examples/example1_basic_convenience.py
```

### Modifying Examples

These examples are designed to be easily modified:

1. **Change Configuration**: Modify the `config.set()` calls to change simulation parameters
2. **Add New Metrics**: Extend the statistics collection with new metrics
3. **Modify Simulation Steps**: Change the number of steps or add new logic
4. **Add Error Handling**: Extend the error handling with custom scenarios

### Example: Customizing example1_basic_convenience.py

```python
# Customize neuron count
config.set("brain.neuron_count", 2000)

# Add new configuration values
config.set("neuromod.dopamine.scale", 2.0)
config.set("plasticity.stdp.learning_rate", 0.01)

# Run longer simulation
for i in range(500):
    brain.step(i)
    # Add custom logic here
```

## Advanced Usage

### Integration with Other Scripts

The convenience methods make it easy to integrate NLM with other systems:

```python
# Use example3_brain_stats.py to create custom analysis tools
# Use example4_agent_features.py as a template for agent applications
# Combine multiple examples for complex simulation scenarios
```

### Performance Optimization

1. **Batch Statistics Collection**: Use the new `getBrainStats()` and `getAgentStats()` methods to reduce multiple method calls
2. **Configuration Caching**: Cache frequently used configuration values
3. **Efficient Simulation**: Use the convenience methods to streamline simulation loops

## Troubleshooting

### Common Issues and Solutions

1. **Import Error**
```bash
# Install the package
pip install pynlm

# Or install in development mode
pip install -e .
```

2. **Missing Configuration**
```python
# Use defaults
neuron_count = config.getInt("brain.neuron_count", 1000)
```

3. **Simulation Errors**
```python
# Check initialization
brain.initialize()  # Always call this first
```

## Further Reading

- **NLM Documentation**: Refer to `HOW_TO_USE.md` for comprehensive API documentation
- **Architecture Guide**: Refer to `docs/ARCHITECTURE.md` for system architecture
- **Science Documentation**: Refer to `docs/SCIENCE.md` for scientific background
- **Experiment Documentation**: Refer to `docs/EXPERIMENTS.md` for experiment descriptions

## License

These examples are part of the NLM (Neural Learning Machine) project and are licensed under the MIT License. See `LICENSE` for details.

## Authors

Research project — See docs for scientific background and authorship information.