# NLM Enhanced API Examples

This directory contains comprehensive examples for the Enhanced NLM API.

## Overview

The Enhanced NLM API provides Pythonic interfaces for neural network simulation with features like:
- Context managers for structured training
- Analysis tools for brain activity
- Visualization utilities
- Enhanced error handling and validation

## Example Categories

### 1. Basic Examples
- `basic_brain.py` - Creating and simulating a simple brain
- `config_examples.py` - Configuration with the ConfigBuilder

### 2. Agent-World Interaction
- `agent_training.py` - Complete agent training example
- `world_simulation.py` - World simulation with visualization

### 3. Analysis and Visualization
- `analysis_examples.py` - Brain activity analysis
- `visualization_examples.py` - Data visualization

### 4. Advanced Usage
- `advanced_training.py` - Advanced training techniques
- `custom_interfaces.py` - Custom Pythonic interfaces

### 5. Performance and Optimization
- `performance_monitoring.py` - Performance monitoring
- `memory_management.py` - Memory management best practices

## Running Examples

Most examples can be run directly:

```bash
python examples/basic_brain.py
python examples/agent_training.py
```

Some examples require additional dependencies:

```bash
pip install matplotlib numpy pandas
```

## Example Structure

Each example includes:
1. Import statements
2. Setup code
3. Main execution logic
4. Results and analysis
5. Error handling

## Common Patterns

### Training Loop Pattern
```python
from pynlm.enhanced import create_enhanced_brain, create_enhanced_world, create_enhanced_agent

brain = create_enhanced_brain(name="TrainingBrain")
brain.initialize()

with training_session(brain, steps=1000) as session:
    session.brain.enable_learning(True)
    session.run(1000)
    analysis = session.analyze()
```

### Agent-World Pattern
```python
from pynlm.enhanced import create_enhanced_agent, create_enhanced_world

brain = create_enhanced_brain()
world = create_enhanced_world()
world.configure_with_defaults()
agent = create_enhanced_agent(brain, name="MyAgent")

episode_stats = agent.run_episode(world, max_steps=100)
```

## Quick Start

1. Choose an example from the examples directory
2. Install any required dependencies (matplotlib, numpy, etc.)
3. Run the example
4. Explore the results and modify as needed

## Example API Usage

The examples demonstrate the Enhanced API features:

- **Context Managers**: For structured training and episodes
- **Analysis Tools**: For understanding brain behavior
- **Visualization**: For creating plots and visualizations
- **Error Handling**: For robust error handling
- **Configuration**: For flexible configuration

## Advanced Topics

Some examples cover advanced topics:

- Custom neural network architectures
- Multi-agent systems
- Reinforcement learning
- Real-time simulation
- Performance optimization

## Getting Help

- Read the `README.md` for general API information
- Check `HOW_TO_USE.md` for basic usage
- See `docs/` for detailed technical documentation
- Visit the NLM project website for more resources
