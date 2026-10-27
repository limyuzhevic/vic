# NLM Python Package

This is the main package initialization for the NLM (Neural Learning Machine) Python bindings.
It provides a unified interface to the NLM neural simulation framework.

## Package Structure

The NLM Python package includes:

- **Core Classes**: Brain, Config, AgentBrain, SimpleWorld, etc.
- **Sensory Inputs**: Vision, Audio, InternalSignals
- **Motor Outputs**: Action, MotorCommand
- **Utilities**: Visualization tools, CLI interface, debugging tools
- **Examples**: Comprehensive example scripts and tutorials

## Installation

The NLM Python package requires:

```bash
pip install pynlm
```

Or install from source:

```bash
pip install -e .
```

## Basic Usage

### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain with the configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simulation
for i in range(100):
    brain.step(i)
```

### Complete Agent-World Simulation

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)

# Create brain and agent
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    
    # Get sensory input and process
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinking
    brain.step(step)
    
    # Agent action selection
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learning
    reward = 1.0 if step % 50 == 0 else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
```

## Configuration System

The NLM Python package includes a comprehensive configuration system that supports:

- **JSON Configuration**: Load configurations from JSON files
- **YAML Configuration**: Load configurations from YAML files
- **Command Line Arguments**: Configure from command line
- **Environment Variables**: Configure from environment variables
- **Runtime Configuration**: Modify configuration at runtime

### Configuration Types

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Load from JSON file
config.loadFromFile("config.json")

# Load from command line
import sys
config.loadFromArgs(len(sys.argv), sys.argv)

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.5)

# Get configuration values
neuron_count = config.get("brain.neuron_count")
if neuron_count:
    print(f"Neuron count: {neuron_count.value}")
```

## Advanced Features

### Visualization Tools

```python
from nlm.visualization import NLMTools

tools = NLMTools("visualizations")

# Create neuron activity plots
plot_file = tools.create_neuron_activity_plot(brain, current_step)

# Create region activity plots
region_plot = tools.create_region_activity_plot(brain, current_step)

# Save visualization report
report_file = tools.save_visualization_report(brain, current_step)
```

### Debugging Tools

```python
from nlm.debug import DebugTools

debugger = DebugTools(log_level="DEBUG", output_file="debug.log")

# Log simulation state
state = debugger.log_simulation_state(brain, step, world, agent)

# Validate simulation integrity
is_valid, issues = debugger.validate_simulation_integrity(brain, world, agent)

# Export debug data
debug_file = debugger.export_debug_data(brain, world, agent, "debug_data.json")
```

### Command Line Interface

```bash
# Show help
python -m nlm.cli help

# Run simulation
python -m nlm.cli simulate --config config.json --steps 1000

# Manage configuration
python -m nlm.cli config list
python -m nlm.cli config create --type advanced --output config.json

# Generate visualizations
python -m nlm.cli visualize --input simulation.json --output report.png

# Debug simulation
python -m nlm.cli debug --log-level DEBUG --output debug.log

# Monitor performance
python -m nlm.cli monitor --duration 30
```

## Examples

### Basic Examples

The `examples/` directory contains:

- **basic/**: Basic usage examples
- **advanced/**: Advanced usage examples
- **custom/**: Custom example scripts

### Example Commands

```bash
# Basic examples
python -m examples basic hello
python -m examples basic visualization
python -m examples basic quick_test

# Advanced examples
python -m examples advanced simple
python -m examples advanced agent
python -m examples advanced learning
python -m examples advanced development
python -m examples advanced config
python -m examples advanced all

# Custom examples
python -m examples custom my_example.py
```

## Performance Monitoring

### Performance Monitoring Tools

```python
from nlm.performance import PerformanceMonitor

monitor = PerformanceMonitor("performance_metrics.json")

# Record metrics
metrics = monitor.record_metrics(brain, step, world, agent)

# Generate performance report
report = monitor.generate_performance_report()

# Plot performance metrics
plot_file = monitor.plot_performance_metrics()
```

## Error Handling

### Exception Handling

```python
import pynlm

try:
    # Create brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation
    for i in range(100):
        brain.step(i)
        
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Error Types

The NLM Python bindings define the following exception types:

- **RuntimeError**: Runtime errors in NLM operations
- **ConfigError**: Configuration-related errors
- **SimulationError**: Simulation-related errors
- **VisualizationError**: Visualization-related errors

## API Reference

### Core Classes

#### Brain
- `Brain(config)`: Create a brain with configuration
- `initialize()`: Initialize the brain
- `step(step)`: Perform a simulation step
- `receiveSensoryInput(input)`: Inject sensory input
- `getTotalNeuronCount()`: Get total neuron count
- `getTotalSynapseCount()`: Get total synapse count
- `getFiringNeuronCount()`: Get firing neuron count
- `getAverageFiringRate()`: Get average firing rate

#### Config
- `createDefaultConfig()`: Create a default configuration
- `loadFromFile(filepath)`: Load configuration from file
- `loadFromArgs(argc, argv)`: Load configuration from command line
- `saveToFile(filepath)`: Save configuration to file
- `set(key, value)`: Set configuration value
- `get(key)`: Get configuration value
- `has(key)`: Check if configuration key exists

#### AgentBrain
- `AgentBrain(brain)`: Create an agent brain interface
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Decode motor command
- `applyRewardModulation(reward, predicted)`: Apply reward modulation
- `enableRewardModulation(enable)`: Enable/disable reward modulation

#### SimpleWorld
- `createSimpleWorld()`: Create a simple world
- `configure(width, height, visionWidth, visionHeight)`: Configure world
- `reset()`: Reset world
- `update(timestep)`: Update world
- `getSensoryPercept()`: Get sensory percept
- `getAgentBody()`: Get agent body

### Sensory Inputs

#### Vision
- `Vision(width, height, channels)`: Create vision input
- `setData(data)`: Set vision data

#### Audio
- `Audio(sampleRate, numSamples)`: Create audio input
- `setData(data)`: Set audio data

#### InternalSignals
- `InternalSignals()`: Create internal signals
- `addSignal(value)`: Add signal value

### Motor Outputs

#### Action
- `Action(type, parameters)`: Create action
- `getType()`: Get action type
- `getParameters()`: Get action parameters

### Visualization Classes

#### NLMTools
- `create_neuron_activity_plot(brain, timestep)`: Create neuron activity plot
- `create_region_activity_plot(brain, timestep)`: Create region activity plot
- `create_time_series_animation(brain_history)`: Create time series animation
- `create_network_graph(brain)`: Create network graph
- `save_visualization_report(brain, timestep)`: Save visualization report

### Debugging Classes

#### DebugTools
- `log_simulation_state(brain, step, world, agent)`: Log simulation state
- `validate_simulation_integrity(brain, world, agent)`: Validate simulation integrity
- `performance_monitor(brain, interval)`: Start performance monitoring
- `export_debug_data(brain, world, agent, filename)`: Export debug data

### Performance Monitoring Classes

#### PerformanceMonitor
- `record_metrics(brain, step, world, agent)`: Record performance metrics
- `generate_performance_report()`: Generate performance report
- `plot_performance_metrics()`: Plot performance metrics

## Best Practices

### Configuration Management

1. **Use Configuration Files**: Store configuration in JSON or YAML files for reproducibility
2. **Use Environment Variables**: Set environment variables for different environments
3. **Use Command Line Arguments**: Configure from command line for quick testing
4. **Document Configuration**: Document all configuration options

### Simulation Management

1. **Use Checkpoints**: Save checkpoints for long simulations
2. **Monitor Performance**: Monitor performance and resource usage
3. **Validate State**: Validate simulation state regularly
4. **Log Activities**: Log all important activities for debugging

### Error Handling

1. **Handle Exceptions**: Handle all exceptions gracefully
2. **Validate Inputs**: Validate all inputs before processing
3. **Provide Error Messages**: Provide clear error messages
4. **Test Error Cases**: Test error cases thoroughly

### Performance Optimization

1. **Use Efficient Algorithms**: Use efficient algorithms for simulation
2. **Monitor Performance**: Monitor performance regularly
3. **Optimize Configuration**: Optimize configuration for performance
4. **Use Parallel Processing**: Use parallel processing for large simulations

## Troubleshooting

### Common Issues

1. **ImportError: No module named 'pynlm'**:
   - Ensure NLM is installed: `pip install pynlm`
   - Ensure you're in the correct directory

2. **RuntimeError: Brain not initialized**:
   - Always call `brain.initialize()` before running simulation
   - Ensure configuration is valid

3. **ValueError: Invalid configuration**:
   - Check configuration file format
   - Ensure all required keys are present

4. **MemoryError: Out of memory**:
   - Reduce configuration size
   - Use smaller simulation
   - Monitor memory usage

5. **Segmentation fault**:
   - Check for null pointers
   - Validate all inputs
   - Use debugging tools

### Getting Help

- **Documentation**: Check `README.md` and `HOW_TO_USE.md` for detailed documentation
- **Examples**: Run examples to see usage patterns
- **Community**: Join the NLM community for support
- **Issues**: Report issues on GitHub

## License

This NLM Python package is licensed under the MIT License, same as the NLM core library.
See the LICENSE file for details.

## Version

NLM Python Package v1.0.0
NLM Core Library v0.1.0 (Phase 6: Final Integration)

For more information about NLM, visit the official documentation and GitHub repository.
