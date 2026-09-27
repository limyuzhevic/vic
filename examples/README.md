# Examples

This directory contains example scripts demonstrating different aspects of using the NLM (Neural Learning Machine) Python API.

## Getting Started

The examples assume you have the `pynlm` module installed. If not, install it with:

```bash
pip install pynlm
```

## Available Examples

### 1. Silent Brain
**File:** `silent_brain.py`

A simple example that creates a virtual brain and runs it without any world interaction. This is useful for:
- Testing basic brain functionality
- Understanding how neurons work
- Performance benchmarking

### 2. Brain Watching a World
**File:** `brain_watching_world.py`

Shows how a brain interacts with a world environment, receiving sensory input and producing actions. Demonstrates:
- World configuration and setup
- Sensory processing
- Motor action generation
- Basic learning capabilities

### 3. Complete Agent Simulation
**File:** `complete_agent.py`

A comprehensive example showing a complete agent simulation where the brain learns through interaction with the environment. Features:
- Full agent lifecycle management
- Neuromodulation (reward, curiosity)
- Developmental tracking
- Environmental interaction
- Statistical monitoring

### 4. Pattern: Run a Simulation
**File:** `pattern_simulation.py`

Demonstrates the common pattern for running complete agent simulations:
1. Update world
2. Get sensory percept
3. Process sensory input
4. Run brain step
5. Decode motor command
6. Apply action to world

### 5. Pattern: Get Brain Statistics
**File:** `pattern_stats.py`

Shows how to monitor and track brain statistics during simulation, including:
- Neuron firing statistics
- Spike counts
- Development stages
- Network metrics

## Running Examples

Each example can be run directly with Python:

```bash
# Run silent brain example
python examples/silent_brain.py

# Run brain watching world example
python examples/brain_watching_world.py

# Run complete agent example
python examples/complete_agent.py

# Run pattern examples
python examples/pattern_simulation.py
python examples/pattern_stats.py
```

## Learning Path

### For Beginners
1. Start with `silent_brain.py` - understand basic brain creation
2. Move to `brain_watching_world.py` - add world interaction
3. Try `complete_agent.py` - full learning simulation

### For Advanced Users
1. Study `pattern_simulation.py` and `pattern_stats.py` - reusable patterns
2. Experiment with configuration options
3. Create custom variants of the examples

## Modifying Examples

You can modify these examples to explore different scenarios:

- Change world size and vision parameters
- Adjust learning rates via configuration
- Add custom reward functions
- Modify action selection strategies
- Track additional metrics

All examples follow the same structure and use the same NLM API, so you can combine elements from different examples to create your own simulations.

## Troubleshooting

### "ModuleNotFoundError: No module named 'pynlm'"

Install the package:
```bash
pip install pynlm
```

### "ImportError: cannot import name 'createBrain'"

Make sure you're using a recent version of the pynlm module. Try reinstalling:
```bash
pip install --upgrade pynlm
```

### Simulation runs too slowly

Reduce the number of steps or world size for faster testing. The examples are optimized for demonstration purposes.

## Extending the Examples

You can extend these examples by:

1. Adding new brain configurations
2. Creating custom world environments
3. Implementing your own agent behavior
4. Adding visualization or logging
5. Running multiple simulations with different parameters

Each example includes comments explaining the key concepts and suggests areas for customization.

## Related Resources

- [HOW_TO_USE.md](HOW_TO_USE.md) - Detailed usage documentation
- [easy_usage.md](easy_usage.md) - Quick start guide for beginners
- [docs/python_api.md](docs/python_api.md) - Complete Python API reference
- NLM C++ documentation for advanced features