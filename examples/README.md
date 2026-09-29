# NLM Python Examples
# Comprehensive collection of working examples for NLM (Neural Learning Machine)

This directory contains practical examples for using NLM in Python, organized by complexity and use case.

## Overview

NLM is a sophisticated spiking neural network simulator written in C++ with Python bindings. These examples demonstrate how to:
- Create and configure virtual brains
- Run simulations with environmental interaction
- Implement learning and memory systems
- Monitor and analyze brain activity
- Build complete agent behaviors

## Examples Structure

### Beginner Examples (1-2 hours to complete)
- `simple_brain.py` - Basic brain creation and simulation
- `agent_world.py` - Simple agent-environment interaction
- `learning_demo.py` - Basic learning demonstration

### Intermediate Examples (2-4 hours to complete)
- `curiosity_learning.py` - Curiosity-driven exploration
- `memory_system.py` - Episodic memory usage
- `complete_agent.py` - Full agent with all subsystems

### Advanced Examples (4+ hours to complete)
- `performance_optimization.py` - Performance benchmarking
- `custom_config.py` - Advanced configuration
- `multi_agent.py` - Multiple agent coordination

### Utility Examples
- `analysis_tools.py` - Brain state analysis tools
- `error_handling.py` - Robust error handling patterns
- `visualization.py` - Basic visualization tools

## Installation

```bash
# Install NLM Python bindings
pip install .

# Or install in development mode
pip install -e .
```

## Example Usage

### Run a beginner example
```bash
python examples/simple_brain.py
```

### Run all examples
```bash
for example in examples/*.py; do
    echo "Running $example"
    python "$example"
done
```

## Running Examples

Each example includes comprehensive documentation and can be run independently:

```python
import pynlm

# Example 1: Simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

## Example Categories

### 1. Brain Creation and Basic Simulation
- Demonstrates how to create and initialize virtual brains
- Shows basic simulation loops
- Includes performance monitoring

### 2. Agent-Environment Interaction
- Complete agent workflow with world interaction
- Sensory input processing
- Motor command execution
- Reward-based learning

### 3. Learning and Memory Systems
- Curiosity-driven exploration
- Episodic memory recording
- Structural plasticity
- Neuromodulation integration

### 4. Performance and Optimization
- Memory usage profiling
- Simulation performance benchmarking
- Configuration optimization
- Advanced settings management

### 5. Error Handling and Robustness
- Comprehensive error handling patterns
- Validation and recovery
- Logging and monitoring

## Best Practices

### 1. Initialize Before Use
```python
brain.initialize()  # Always call before simulation
```

### 2. Use Appropriate Timesteps
```python
# Smaller timesteps for stable learning
brain.step(step, timestep=0.001)

# Larger timesteps for faster simulation
brain.step(step, timestep=0.1)
```

### 3. Monitor Performance
```python
# Track simulation statistics
steps_per_second = brain.getStepsPerSecond()
memory_usage = brain.getMemoryUsage()
firing_rate = brain.getAverageFiringRate()
```

### 4. Enable Learning Systems
```python
# Configure learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
```

### 5. Proper Error Handling
```python
try:
    brain.step(step)
except pynlm.RuntimeError as e:
    print(f"Simulation error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Advanced Topics

### Configuration Management
```python
# Advanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("neuromod.dopamine.scale", 1.2)

brain = pynlm.createBrain(config)
```

### Performance Monitoring
```python
# Performance analysis
stats = {
    'simulation_speed': brain.getSimulationSpeed(),
    'memory_efficiency': brain.getMemoryEfficiency(),
    'neuron_utilization': brain.getNeuronUtilization(),
    'firing_rate': brain.getAverageFiringRate(),
    'plasticity_rate': brain.getPlasticityRate()
}
```

### Multi-Step Learning
```python
# Advanced learning patterns
for step in range(1000):
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes information
    brain.step(step)
    
    # Select and execute action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from experience
    reward = calculate_reward()
    novelty = calculate_novelty()
    agent.applyRewardModulation(reward, novelty)
```

## Testing Examples

### Running All Examples
```bash
# Install pytest
pip install pytest

# Run examples
python -m pytest examples/ -v

# Run specific example
python examples/simple_brain.py
```

### Example Test Results
Each example should output meaningful results:
- Neuron counts and firing rates
- Learning statistics
- Performance metrics
- Memory usage

## Contributing Examples

To add a new example:

1. Create a new Python file in the examples directory
2. Include comprehensive documentation
3. Add error handling
4. Include performance monitoring
5. Test the example thoroughly
6. Add to the examples index

## Example License

MIT License - See LICENSE file for details

## Authors

NLM Authors - Research project for computational brain simulation

This directory is maintained and updated regularly with new examples and improvements.
