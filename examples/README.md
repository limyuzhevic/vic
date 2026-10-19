# Enhanced NLM Examples

This directory contains advanced examples for the improved NLM (Neural Learning Machine) framework.

## Overview

These examples demonstrate the new capabilities introduced in the recent enhancements:

1. **Enhanced Configuration** - JSON/YAML configuration support
2. **Advanced Debugging** - Comprehensive debugging and profiling tools
3. **Performance Optimization** - Performance monitoring and analysis
4. **Configuration Testing** - Configuration validation and testing
5. **Memory Management** - Memory monitoring and analysis
6. **Integration Testing** - Using the experiment framework

## Running Examples

To run these examples, ensure you have the NLM Python bindings installed:

```bash
pip install pynlm

# Or install from source
pip install -e .
```

Then run the examples:

```bash
python3 examples/enhanced_debugging_examples.py
```

## Example Categories

### 1. Basic Debugging
`example_basic_debugging()` - Demonstrates basic debugging features

This example shows how to:
- Create and configure a brain
- Enable advanced debugging features
- Monitor neural activity during simulation
- Analyze brain state and performance

### 2. Advanced Profiling
`example_advanced_profiling()` - Demonstrates advanced performance profiling

This example shows how to:
- Set up performance monitoring
- Track timing and resource usage
- Analyze network performance
- Profile memory usage

### 3. Configuration Testing
`example_configuration_testing()` - Demonstrates configuration testing

This example shows how to:
- Test different brain configurations
- Validate configuration settings
- Compare performance across different settings
- Test network connectivity

### 4. Memory Monitoring
`example_memory_monitoring()` - Demonstrates memory monitoring

This example shows how to:
- Monitor memory usage during simulation
- Track allocation and deallocation
- Profile memory patterns
- Analyze memory efficiency

### 5. Experiment Framework
`example_experiment_framework()` - Demonstrates the experiment framework

This example shows how to:
- Use the Phase 6 integrated experiment framework
- Run comprehensive integration tests
- Use checkpointing and replay capabilities
- Analyze system integration

## Advanced Usage

### Using the Debugger

```python
import pynlm

# Create configuration and brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Configure debug settings
config.set("debug.level", 4)  # Debug level
config.set("debug.neurons.enable", True)  # Enable neuron debugging
config.set("debug.synapses.enable", True)  # Enable synapse debugging

# Start debugging session
# debugger = pynlm.createDebugger(config)
# session_id = debugger.startDebugSession("research_experiment")

# Run simulation with debugging
for step in range(1000):
    brain.step(step)
    
    # Get debug information
    # debug_info = debugger.getDebugInfo()
    # active_neurons = debug_info.getActiveNeurons()
    
    # Log events
    # debugger.logEvent("simulation_step", f"Step {step} completed")

# Print performance report
# print(debugger.getPerformanceReport())

# Export debug state
# debugger.exportDebugState("debug_state.json")
```

### Configuration Testing

```python
import pynlm

def test_configuration(neuron_count, connection_probability):
    """Test a specific configuration"""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.connection_probability", connection_probability)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Analyze results
    return {
        "neuron_count": brain.getTotalNeuronCount(),
        "synapse_count": brain.getTotalSynapseCount(),
        "firing_rate": brain.getAverageFiringRate(),
        "active_neurons": brain.getFiringNeuronCount()
    }

# Test different configurations
configs = [
    (500, 0.05),
    (1000, 0.1),
    (2000, 0.15),
]

results = []
for neurons, conn_prob in configs:
    result = test_configuration(neurons, conn_prob)
    result["config"] = f"{neurons}_neurons_{conn_prob}_conn"
    results.append(result)
    print(f"Config {result['config']}: {result}")
```

### Performance Profiling

```python
import pynlm
import time

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable profiling
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)

print("Starting performance profiling...")
start_time = time.time()

# Run simulation
for step in range(500):
    world.update(0.05)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        elapsed = time.time() - start_time
        print(f"Step {step}: {elapsed:.2f}s elapsed, "
              f"firing rate: {brain.getAverageFiringRate():.2f} Hz")

print(f"Total simulation time: {time.time() - start_time:.2f}s")
```

### Memory Analysis

```python
import pynlm

def analyze_memory_usage(config):
    """Analyze memory usage during simulation"""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Configure memory monitoring
    config.set("debug.memory.enable", True)
    config.set("debug.memory.track_allocations", True)
    
    # In real implementation, this would interface with C++ debugger
    # debugger = pynlm.createDebugger(config)
    
    # Run simulation with memory tracking
    memory_profile = {}
    for step in range(100):
        brain.step(step)
        
        # Get memory usage (placeholder for real implementation)
        # memory_profile[step] = debugger.getMemoryProfile()
    
    return memory_profile

# Analyze memory usage
config = pynlm.createDefaultConfig()
memory_data = analyze_memory_usage(config)

# Process memory data
for step, profile in memory_data.items():
    print(f"Step {step}: {profile['current_usage']} bytes used")
```

## Configuration Files

### Basic Configuration
Create `examples/basic_config.json`:

```json
{
    "brain": {
        "neuron_count": 1000,
        "connection_probability": 0.1
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 1000
    }
}
```

### Research Configuration
Create `examples/research_config.yaml`:

```yaml
brain:
  neuron_count: 5000
  connection_probability: 0.15
  region_count: 3

simulation:
  timestep: 0.001
  max_steps: 10000
  enable_checkpointing: true

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
  hebbian:
    enable: true
    strength: 0.5

neuromodulation:
  curiosity:
    enable: true
    exploration_rate: 0.3

debug:
  level: 3
  neurons:
    enable: true
    track_connections: true
  synapses:
    enable: true
    track_dynamics: true
```

### Debug Configuration
Create `examples/debug_config.json`:

```json
{
    "debug": {
        "level": 4,
        "neurons": {
            "enable": true,
            "track_connections": true,
            "track_firing_patterns": true
        },
        "synapses": {
            "enable": true,
            "track_dynamics": true,
            "track_plasticity": true
        },
        "memory": {
            "enable": true,
            "track_allocations": true,
            "profile_usage": true
        }
    },
    "brain": {
        "neuron_count": 2000
    }
}
```

## Running the Enhanced Examples

### Python Examples
Run the enhanced examples:

```bash
python3 examples/enhanced_debugging_examples.py
```

### Command Line Examples
Run the enhanced command-line interface:

```bash
# Help and advanced options
./nlm --advanced-help

# Run with debugging enabled
./nlm --phase6 --enable-development --output results.json

# Load custom configuration
./nlm --config examples/debug_config.json --steps 2000

# Performance monitoring
./nlm --phase6 --debug-neurons --debug-synapses --output performance.json
```

## Integration with Development Tools

### VS Code Integration
Create a `.vscode/tasks.json` file for running NLM examples:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Run Enhanced Examples",
            "type": "process",
            "command": "python3",
            "args": ["examples/enhanced_debugging_examples.py"],
            "problemMatcher": [],
            "group": {
                "kind": "build",
                "isDefault": true
            }
        },
        {
            "label": "Run NLM with Debugging",
            "type": "process",
            "command": "./nlm",
            "args": ["--phase6", "--enable-development", "--debug-neurons", "--debug-synapses"],
            "problemMatcher": [],
            "group": "build"
        }
    ]
}
```

## Common Issues and Solutions

### Issue 1: Debugger Not Working
**Solution**: Ensure the debugger module is enabled in CMake:
```bash
cmake -DNLM_ENABLE_DEBUGGER=1 ..
make
```

### Issue 2: Configuration Validation Errors
**Solution**: Use the configuration validation examples:
```python
# Validate configuration before use
validator = pynlm.createConfigValidator(config)
issues = validator.validate()
if issues:
    print("Configuration issues:", issues)
```

### Issue 3: Memory Usage Too High
**Solution**: Configure memory pools and optimization:
```json
"performance": {
    "memory_pool": {
        "enable": true,
        "neuron_pool_size": 10000
    }
}
```

## Troubleshooting

### Debugging Common Problems
1. **Import Error**: Ensure pynlm is properly installed
2. **Configuration Errors**: Check syntax in JSON/YAML files
3. **Performance Issues**: Use profiling examples to identify bottlenecks
4. **Memory Issues**: Use memory monitoring examples to track usage

### Getting Help
- Check the NLM documentation in docs/
- Review the examples in this directory
- Use the enhanced help system: `./nlm --advanced-help`
- Consult the NLM GitHub repository for issues and discussions

## License

This example code is provided under the MIT License as part of the NLM project.

## Contributors

This enhanced examples collection was created as part of the NLM improvement efforts.

For more information, visit the NLM documentation and examples directories.
