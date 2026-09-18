# Installing NLM Enhanced API

## Quick Start

This document explains how to use the enhanced NLM API. If you're new to NLM, start with the basic examples first.

## Prerequisites

```bash
# Install NLM
pip install .

# The enhanced API is automatically available as an extension to pynlm
```

## Quick Installation

```bash
# Install with enhanced API support
pip install -e .  # or pip install .
```

The enhanced API is included by default and extends the basic pynlm module with:

- Factory functions for agent creation
- Helper classes for simulation management
- Enhanced configuration presets
- Utility functions for common patterns
- Advanced agent features

## Enhanced API Installation

If you want to ensure the enhanced API is installed even if the basic pynlm module has issues:

```bash
# Install enhanced API as a separate package
pip install enhanced_nlm
```

Or for development:

```bash
pip install -e enhanced_api.py
```

## Migration Guide

### From Basic to Enhanced

#### Before (Basic API)

```python
import pynlm

# Basic setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### After (Enhanced API)

```python
import pynlm

# Enhanced setup - development agent
agent = pynlm.create_development_agent(
    stages=["initial", "critical_period"],
    curiosity_enabled=True,
    plasticity_multiplier=1.5
)

# World setup
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)

# Initialize enhanced agent
agent.initialize(world)

# Run enhanced simulation
simulation = pynlm.run_complete_simulation(
    agent,
    world,
    num_steps=100,
    headless=False
)
```

### Configuration Presets

#### Before

```python
config = pynlm.createDefaultConfig()
config.set("development.enabled", True)
config.set("learning.rate", 0.01)
```

#### After

```python
# Using presets
config = pynlm.get_preset_config("development")
config = pynlm.get_preset_config("curiosity_driven")
config = pynlm.get_preset_config("reward_based")
config = pynlm.get_preset_config("critical_period")
config = pynlm.get_preset_config("rl_integrated")
```

### Training

#### Before

```python
# Manual training loop
for episode in range(100):
    world.reset()
    agent.reset()
    
    for step in range(1000):
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

#### After

```python
# Enhanced training with evaluation
training_loop = pynlm.TrainingLoop(agent, world, config)
metrics = training_loop.train(
    num_episodes=200,
    evaluation_interval=50,
    save_interval=100
)

# Simple training
metrics = agent.train_with_preset(
    world,
    preset_name="curiosity_driven",
    num_episodes=100
)
```

### Agent Features

#### Before

```python
# Basic agent setup
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
```

#### After

```python
# Enhanced agent with automatic feature enabling
agent = pynlm.create_development_agent(
    curiosity_enabled=True,
    plasticity_multiplier=1.5,
    structural_plasticity_enabled=True,
    reward_modulation_enabled=True
)

# Check agent status
print(f"Is developing: {agent.is_developing}")
print(f"Curiosity level: {agent.curiosity_level}")
print(f"Novelty score: {agent.novelty_score}")
```

### Exploration Strategies

#### Before

```python
# Manual exploration management
exploration_rate = 0.1 * (1.0 - step / 10000)
```

#### After

```python
# Built-in exploration strategies
exploration_manager = pynlm.ExplorationManager(agent, world)
exploration_manager.set_strategy(pynlm.ExplorationStrategy.UCB)
exploration_manager.set_param("c", 1.0)

exploration_rate = exploration_manager.get_exploration_rate(step)
should_explore = exploration_manager.should_explore(step)
```

### Simulation Management

#### Before

```python
# Manual simulation management
history = []
for step in range(1000):
    world.update(0.1)
    # ... simulation logic
    history.append(step_data)
```

#### After

```python
# Enhanced simulation management
simulation = pynlm.run_complete_simulation(
    agent,
    world,
    num_steps=1000,
    save_interval=100,
    output_file="simulation.json"
)

simulation.save_history("simulation.json")
```

### Benchmarking

#### Before

```python
# Manual benchmarking
results = []
for agent in agents:
    for env in environments:
        # ... benchmarking logic
        results.append(metrics)
```

#### After

```python
# Enhanced benchmarking
benchmark = pynlm.BenchmarkSuite()
benchmark.add_benchmark("agent1", agent_factory, [env1, env2, env3])
benchmark.add_benchmark("agent2", agent_factory2, [env1, env2])

results = benchmark.run_all_benchmarks(num_episodes=100)
```

## Installation Issues and Solutions

### Issue 1: Enhanced API Not Available

```bash
# Problem: enhanced_api.py is not being imported
# Solution: Import the enhanced API
from enhanced_api import *
# or use pynlm.enhanced_api if properly integrated
```

### Issue 2: Missing Dependencies

```bash
# The enhanced API has no external dependencies beyond pynlm itself
```

### Issue 3: Performance Issues

```python
# For better performance in large simulations
simulation = pynlm.run_complete_simulation(
    agent,
    world,
    num_steps=10000,
    headless=True  # Reduces print output
)
```

### Issue 4: Memory Issues

```python
# For memory-constrained environments
agent = pynlm.create_standard_agent()  # Smaller agent
world = pynlm.createSimpleWorld()  # Basic world
```

## File Structure

```
./
├── enhanced_api.py          # Main enhanced API module
├── easy_usage.md           # This file
├── HOW_TO_USE.md           # Original documentation
├── README.md               # Project README
├── pyproject.toml         # Package configuration
└── examples/              # Example scripts
    ├── basic_examples.py   # Basic usage examples
    ├── advanced_examples.py # Advanced usage examples
    └── benchmark_examples.py # Benchmarking examples
```

## Example Scripts

### Basic Examples

```python
# basic_examples.py - Shows basic enhanced API usage
```

### Advanced Examples

```python
# advanced_examples.py - Shows advanced usage patterns
```

### Benchmark Examples

```python
# benchmark_examples.py - Shows benchmarking usage
```

## API Reference

### Core Modules

- `pynlm` - Original NLM bindings
- `enhanced_api` - Enhanced API module

### Enhanced Classes

- `EnhancedAgentBrain` - Enhanced agent with additional features
- `AgentFactory` - Factory class for agent creation
- `SimulationManager` - Comprehensive simulation management
- `TrainingLoop` - Training loop with evaluation
- `ExplorationManager` - Exploration strategy management
- `BenchmarkSuite` - Benchmarking framework

### Enhanced Functions

- `create_development_agent(**kwargs)` - Create development agent
- `create_curiosity_agent(**kwargs)` - Create curiosity-driven agent
- `create_reward_agent(**kwargs)` - Create reward-based agent
- `create_critical_period_agent(**kwargs)` - Create critical period agent
- `create_rl_agent(**kwargs)` - Create reinforcement learning agent
- `create_standard_agent(**kwargs)` - Create standard agent
- `run_complete_simulation(...)` - Run complete simulation
- `setup_agent_brain_with_world(...)` - Setup agent and world
- `get_preset_config(...)` - Get configuration preset
- `monitor_brain_performance(...)` - Monitor brain performance

### Configuration Presets

- `"development"` - Development agent
- `"curiosity_driven"` - Curiosity and exploration
- `"reward_based"` - Reward and reinforcement
- `"critical_period"` - Critical period learning
- `"rl_integrated"` - Reinforcement learning
- `"standard"` - Standard agent

## Common Issues and Solutions

### Issue: Import Error

```bash
ModuleNotFoundError: No module named 'enhanced_api'
```

**Solution:**
```python
# Add the directory to Python path
import sys
import os
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

import enhanced_api
```

### Issue: pynlm Not Available

```bash
ModuleNotFoundError: No module named 'pynlm'
```

**Solution:**
```bash
# Install NLM with enhanced API support
pip install -e .
# or
pip install enhanced_nlm
```

### Issue: Performance Degradation

**Solution:**
```python
# Use headless mode for production
agent = pynlm.create_development_agent()
world = pynlm.createSimpleWorld()
simulation = pynlm.run_complete_simulation(
    agent, world, 10000, headless=True
)
```

### Issue: Memory Usage

**Solution:**
```python
# Use smaller configurations
agent = pynlm.create_standard_agent()
agent = pynlm.create_reward_agent()
```

## Quick Setup for New Users

### 1. Quick Installation

```bash
# One-line installation
pip install enhanced_nlm

# Or for development
pip install -e enhanced_api.py
```

### 2. Quick Start

```python
# One-line setup for a complete agent
import pynlm

agent = pynlm.create_development_agent(
    curiosity_enabled=True,
    plasticity_multiplier=1.5
)

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

agent.initialize(world)
simulation = pynlm.run_complete_simulation(agent, world, 1000)
```

### 3. Quick Training

```python
# One-line training
metrics = agent.train_with_preset(
    world,
    preset_name="curiosity_driven",
    num_episodes=100
)
```

## Advanced Usage

### Custom Exploration Strategies

```python
from enhanced_api import ExplorationManager, ExplorationStrategy

# Create custom exploration strategy
def custom_exploration(agent, world, step):
    base_rate = 0.1
    return base_rate * (1.0 - step / 10000)

exploration_manager = ExplorationManager(agent, world)
exploration_manager.add_strategy(custom_exploration, "custom")
exploration_manager.set_strategy("custom")
```

### Custom Callback Functions

```python
# Add custom callback to simulation

def record_state(step, simulation):
    # Record custom metrics
    state = simulation.get_world_state()
    # ... process state

simulation.add_callback(record_state)
```

### Checkpoint Management

```python
# Manual checkpoint management
agent.save_checkpoint("checkpoint_100.bin")
agent.load_checkpoint("checkpoint_100.bin")
```

## Best Practices

### 1. Use Presets for Common Configurations

```python
# Always use presets for common configurations
agent = pynlm.create_development_agent()
agent = pynlm.create_curiosity_agent()
agent = pynlm.create_reward_agent()
```

### 2. Monitor Performance

```python
# Always monitor performance during training
training_loop = pynlm.TrainingLoop(agent, world)
metrics = training_loop.train(200, evaluation_interval=50)
print(f"Mean reward: {metrics['mean_reward']:.2f}")
```

### 3. Use Headless Mode for Production

```python
# Use headless mode for production simulations
simulation = pynlm.run_complete_simulation(
    agent, world, 10000, headless=True
)
```

### 4. Save Checkpoints

```python
# Always save checkpoints
training_loop = pynlm.TrainingLoop(agent, world, {\"save_interval\": 100})
metrics = training_loop.train(1000, save_interval=100)
```

### 5. Evaluate Regularly

```python
# Always evaluate regularly
training_loop = pynlm.TrainingLoop(agent, world)
metrics = training_loop.train(500, evaluation_interval=100)
```

## Troubleshooting

### Installation Issues

#### Problem: "ModuleNotFoundError: No module named 'enhanced_api'"

**Solution:**
```bash
# Add current directory to Python path
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

# Or install enhanced_nlm package
pip install enhanced_nlm
```

#### Problem: "ImportError: cannot import name 'pynlm'"

**Solution:**
```bash
# Install NLM with enhanced API
pip install -e .  # Installs enhanced API
# or
pip install enhanced_nlm  # Installs enhanced API separately
```

### Runtime Issues

#### Problem: "AttributeError: 'NoneType' object has no attribute 'initialize'"

**Solution:**
```python
# Check if agent is created properly
if agent:
    agent.initialize(world)
else:
    # Create agent with preset
    agent = pynlm.create_development_agent()
    agent.initialize(world)
```

#### Problem: Performance Issues

**Solution:**
```python
# Use headless mode for large simulations
simulation = pynlm.run_complete_simulation(
    agent, world, 10000, headless=True
)
```

#### Problem: Memory Issues

**Solution:**
```python
# Use smaller agent configurations
agent = pynlm.create_standard_agent()
```

### Custom Issues

#### Problem: Custom Exploration Not Working

**Solution:**
```python
from enhanced_api import ExplorationManager

exploration_manager = ExplorationManager(agent, world)
exploration_manager.set_strategy(ExplorationStrategy.UCB)
exploration_manager.set_param("c", 1.0)
```

#### Problem: Simulation Not Saving

**Solution:**
```python
# Check if output file is writable
simulation = pynlm.run_complete_simulation(
    agent, world, 1000, output_file="simulation.json"
)
```

## Next Steps

### 1. Learn Basic Usage

```python
# Start with basic examples
from enhanced_api import create_development_agent, run_complete_simulation

agent = create_development_agent()
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
agent.initialize(world)
result = run_complete_simulation(agent, world, 1000)
```

### 2. Try Advanced Features

```python
# Try advanced features
from enhanced_api import TrainingLoop, BenchmarkSuite

# Training
training_loop = TrainingLoop(agent, world)
metrics = training_loop.train(200, evaluation_interval=50)

# Benchmarking
benchmark = BenchmarkSuite()
results = benchmark.run_all_benchmarks(100)
```

### 3. Customize for Your Use Case

```python
# Customize for your specific use case
def custom_agent_factory():
    return pynlm.create_development_agent(
        curiosity_enabled=True,
        plasticity_multiplier=1.5,
        reward_modulation_enabled=True
    )

# Use custom factory in benchmark
benchmark.add_benchmark("custom", custom_agent_factory, [env1, env2, env3])
```

### 4. Integrate with Existing Code

```python
# Integrate with existing code
if hasattr(pynlm, 'EnhancedAgentBrain'):
    # Use enhanced features
    enhanced_agent = pynlm.EnhancedAgentBrain(agent)
    enhanced_agent.enable_reward_modulation(True)
    enhanced_agent.enable_curiosity(True)
```

### 5. Contribute Improvements

```python
# Contribute improvements to the enhanced API
# Add new presets
PRESETS["new_preset"] = {...}

# Add new exploration strategies
class CustomStrategy(ExplorationStrategy):
    def get_exploration_rate(self, step):
        # Custom implementation
        return 0.1
```

## License

The enhanced NLM API is licensed under the same terms as NLM.
See the LICENSE file for details.

## Support

For support with the enhanced NLM API:
- Check the examples directory for more usage patterns
- Read the original HOW_TO_USE.md for basic usage
- Consult the enhanced_api.py documentation for advanced features

---

**Happy Brain Simulating!**

The enhanced NLM API makes it easier to set up and run simulations while providing access to advanced features. Use the presets for common configurations and the helper classes for complex workflows.