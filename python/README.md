# NLM Enhanced Python Bindings - Quick Start Guide

This guide provides a quick overview of getting started with the enhanced NLM Python bindings.

## Quick Start

### 1. Install and Setup

```bash
pip install -e .
```

```python
from enhanced_bindings import NLMIntegration
NLMIntegration.setup_all_integrations()
```

### 2. Basic Usage

```python
import pynlm
from enhanced_bindings import *

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create a simple world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Create an agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run a simulation
for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Simulation complete!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### 3. Using Enhanced Features

```python
import pynlm
from enhanced_bindings import (
    EnhancedBrain, EnhancedConfig,
    simulation_step, agent_episode,
    SimulationRunner, DataCollector
)

# Create enhanced brain
brain = EnhancedBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
brain.initialize()

# Use context manager for simulation step
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

with simulation_step(world, brain, agent) as step_result:
    print(f"Step completed with {step_result.get('spikes_generated', 0)} spikes")

# Use SimulationRunner for experiment
runner = SimulationRunner(world, brain, agent)
results = runner.run_experiment(steps=100)

print(f"Experiment completed with {len(results)} steps")
```

### 4. Using Pythonic Interfaces

```python
from enhanced_bindings import EnhancedBrain, EnhancedConfig

# Create enhanced brain
brain = EnhancedBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
brain.initialize()

# Use Pythonic interface
print(f"Neuron count: {brain.neuron_count}")
print(f"Synapse count: {brain.synapse_count}")
print(f"Firing rate: {brain.firing_rate:.4f}")

# Use method chaining
brain.initialize().step(0).step(1).step(2)

# Get statistics
stats = brain.get_statistics()
print(f"Total spikes: {stats['total_spikes']}")
```

### 5. Using Configuration

```python
from enhanced_bindings import EnhancedConfig

# Create enhanced config
config = EnhancedConfig(pynlm.createDefaultConfig())

# Set values using dictionary-like access
config['brain.neuron_count'] = 200
config['brain.synapse_density'] = 0.15
config['world.vision_width'] = 10
config['world.vision_height'] = 10

# Get values
neuron_count = config.get_int('brain.neuron_count', 100)
synapse_density = config.get_float('brain.synapse_density', 0.1)

print(f"Neuron count: {neuron_count}")
print(f"Synapse density: {synapse_density}")
```

### 6. Using Data Collection and Analysis

```python
from enhanced_bindings import SimulationRunner, DataCollector, Analysis

# Create components
brain = EnhancedBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run experiment with data collection
runner = SimulationRunner(world, brain, agent)
results = runner.run_experiment(steps=100)

# Create data collector
data_collector = DataCollector()
for step_result in results['history']:
    data_collector.record(step_result)

# Analyze data
data = data_collector.get_data()
analysis = Analysis(data)
stats = analysis.compute_statistics()

print(f"Spike statistics:")
for key, value in stats.get('spikes', {}).items():
    print(f"  {key}: {value:.2f}")
```

## Key Features

### 1. Context Managers
- `simulation_step()` - Automatic state management
- `agent_episode()` - Complete episode management
- `world_session()` - World session with cleanup

### 2. Utility Classes
- `SimulationRunner` - Common simulation patterns
- `ExperimentRunner` - Parameter sweeps and experiments
- `DataCollector` - Metrics recording and callbacks
- `Analysis` - Post-simulation statistical analysis

### 3. Pythonic Interfaces
- `EnhancedBrain` - Pythonic wrapper with `__iter__`, `__len__`, `__getitem__`
- `EnhancedConfig` - Dictionary-like configuration interface
- Method chaining support
- Type-safe getter methods

### 4. Advanced Python Features
- CallbackManager and EventDrivenSimulation
- Decorator pattern support
- Async simulation support
- Statistical utilities

### 5. Ecosystem Integrations
- NumPy integration for vectorized operations
- Pandas integration for data analysis
- Matplotlib integration for visualization
- Scikit-learn integration for machine learning

### 6. Enhanced Error Handling
- Specific exception types (ValidationError, ConfigError, etc.)
- Type validation and parameter validation
- Graceful error recovery
- Structured error logging

## Documentation

### Main Documentation
- `COMPREHENSIVE_DOCUMENTATION.md` - Complete documentation
- `README.md` - Project overview and installation

### Examples
- `examples/basic_usage/` - Basic usage examples
- `examples/advanced_usage/` - Advanced usage examples
- `examples/error_handling/` - Error handling examples
- `examples/performance/` - Performance examples
- `examples/integration/` - Integration examples
- `examples/phase3/` - Phase 3 simulation examples
- `examples/phase4/` - Phase 4 simulation examples

### API Reference
- `docs/ARCHITECTURE.md` - Architecture documentation
- `docs/SCIENCE.md` - Scientific background
- Various other documentation files in the `docs/` directory

## Support and Resources

### Quick Help
- For questions and support, refer to the documentation
- Examples are available in the `examples/` directory
- Each example includes detailed comments and expected output

### Further Learning
- Read `COMPREHENSIVE_DOCUMENTATION.md` for complete documentation
- Study the examples in `examples/` directory
- Refer to the API reference for detailed method information
- Check the `README.md` for project information

## Getting Help

### Common Issues

1. **"My brain isn't doing anything"**
   - Did you call `brain.initialize()`?
   - Try increasing the number of steps

2. **"The agent isn't moving"**
   - Did you call `world.applyMotorCommand()`?
   - Check that `world.update()` is being called

3. **"Everything is 0"**
   - Brains need time to "warm up" - try more steps
   - Some neurons need input to fire - make sure sensory input is connected

### Troubleshooting

1. **Missing dependencies**
   ```python
   NLMIntegration.setup_all_integrations()
   ```

2. **Performance issues**
   - Use NumPy vectorization for numerical operations
   - Implement memory-efficient data structures
   - Profile your code to identify bottlenecks

3. **Error handling**
   - Use specific exception types for different error conditions
   - Implement proper error recovery
   - Log errors for debugging and analysis

### Community

For questions, feedback, or collaboration:
- Refer to the project documentation
- Check the examples for usage patterns
- Review the code for best practices
- Contact support if needed

## Conclusion

The enhanced NLM Python bindings provide a comprehensive and flexible framework for creating, simulating, and analyzing neural systems. They maintain full backward compatibility while adding powerful new features for:

- Resource management through context managers
- Simulation automation through utility classes
- Pythonic interfaces through enhanced wrappers
- Advanced programming through callbacks, observers, decorators
- Ecosystem integration through NumPy, Pandas, Matplotlib, and Scikit-learn
- Error handling through specific exception types with recovery information
- Performance optimization through vectorization and memory-efficient patterns

Whether you're just getting started with NLM or looking to build complex, sophisticated simulations, the enhanced bindings provide everything you need to create powerful neural systems efficiently and effectively.
