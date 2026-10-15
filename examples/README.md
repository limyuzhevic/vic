# NLM Examples Directory

This directory contains example scripts demonstrating different use cases for the Neural Learning Machine (NLM) neural simulation framework.

## Overview

The NLM examples showcase the full range of capabilities provided by the improved Python bindings and command-line interface. Each example demonstrates specific aspects of the NLM framework:

- Basic brain operations and simulation
- Agent-world interaction and learning
- Training loops with exploration strategies
- Configuration management
- Performance benchmarking
- Developmental processes

## Running Examples

### Method 1: Run All Examples

```bash
# Change to the examples directory
cd /path/to/nlm/examples

# Run all examples (will create a 'results' directory)
python run_all_examples.py
```

### Method 2: Run Individual Examples

Each example can be run individually:

```bash
# Example 1: Basic Brain Operations
python basic_brain_example.py

# Example 2: Agent-World Interaction
python agent_world_example.py

# Example 3: Training Loop
python training_loop_example.py

# Example 4: Configuration Management
python configuration_example.py

# Example 5: Benchmarking
python benchmarking_example.py

# Example 6: Developmental Processes
python developmental_example.py
```

### Method 3: Using the Command Line Interface

```bash
# Create a brain simulation using CLI
nlm create example_brain --neurons 500 --steps 100

# Run a simulation using CLI
nlm run example_brain --steps 200

# Run a benchmark using CLI
nlm benchmark 1000

# Show help information
nlm help
```

## Examples Overview

### Example 1: Basic Brain Operations (`basic_brain_example.py`)

**Description**: Demonstrates basic brain creation, initialization, and simulation.

**Key Concepts**:
- Creating a brain with default configuration
- Running simulation steps
- Getting brain statistics (neurons, synapses, spikes, firing rate)
- Saving brain state

**What you'll learn**:
- How to create and initialize a brain
- How to run a basic simulation
- How to interpret brain statistics
- How to save brain state for later use

### Example 2: Agent-World Interaction (`agent_world_example.py`)

**Description**: Shows how an agent interacts with a simulated world.

**Key Concepts**:
- Creating and configuring a world
- Setting up an agent brain
- Sensory processing and motor control
- Reward-based learning
- Development system integration

**What you'll learn**:
- How agents perceive their environment
- How agents make decisions
- How reward signals affect learning
- How development changes brain properties over time

### Example 3: Training Loop (`training_loop_example.py`)

**Description**: Implements a training loop with exploration strategies.

**Key Concepts**:
- Exploration vs. exploitation strategies
- Reward collection during training
- Progress monitoring and logging
- Brain plasticity during training

**What you'll learn**:
- How to design training algorithms
- How to balance exploration and exploitation
- How to monitor training progress
- How to analyze training results

### Example 4: Configuration Management (`configuration_example.py`)

**Description**: Demonstrates configuration loading, validation, and saving.

**Key Concepts**:
- Creating and modifying brain configurations
- Configuration validation
- Loading and saving configurations
- Creating brains with custom configurations

**What you'll learn**:
- How to customize brain parameters
- How to validate configurations
- How to persist configuration settings
- How to use saved configurations

### Example 5: Benchmarking (`benchmarking_example.py`)

**Description**: Performance benchmarking for different brain sizes.

**Key Concepts**:
- Performance measurement
- Scalability analysis
- Comparing brain configurations
- Benchmarking tools

**What you'll learn**:
- How to measure performance
- How to analyze scalability
- How to compare different configurations
- How to use benchmarking for optimization

### Example 6: Developmental Processes (`developmental_example.py`)

**Description**: Shows how developmental processes change over time.

**Key Concepts**:
- Developmental stages
- Age-dependent changes
- Plasticity modulation during development
- Monitoring developmental progress

**What you'll learn**:
- How development affects brain properties
- How to monitor developmental stages
- How to implement age-dependent behaviors
- How to analyze developmental trajectories

## Expected Output

Running all examples will create a `results` directory with the following files:

```
results/
├── basic_brain_results.json
├── agent_world_results.json
├── training_loop_results.json
├── configuration_results.json
├── benchmark_results.json
└── developmental_results.json
```

Each JSON file contains the results of the corresponding example, including:
- Input parameters
- Key metrics and statistics
- Execution time
- Performance indicators

## Dependencies

All examples require:

- Python 3.8+
- NLM Python bindings
- NumPy (for some examples)

### Installation

```bash
# Install NLM dependencies
pip install pybind11 scikit-build-core numpy rapidjson

# Install NLM itself
pip install -e .
```

## Customization

Each example can be customized by modifying the parameters at the top of the script:

```python
# Example modifications
# 1. Change simulation parameters
num_steps = 500
brain_size = 2000

# 2. Modify agent behavior
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)

# 3. Change training settings
epochs = 20
learning_rate = 0.01

# 4. Customize world settings
world_size = 50
vision_width = 16
vision_height = 16
```

## Integration with Other Tools

### Using Examples with CLI

You can integrate examples with the NLM CLI tool:

```bash
# Create brain from example settings
nlm create custom_brain --neurons 1000 --steps 300

# Run example configuration
nlm config example_config create

# Run benchmark
nlm benchmark 500
```

### Using Examples in Projects

You can import the example scripts as modules:

```python
from examples.basic_brain_example import BasicBrainExample
from examples.agent_world_example import AgentWorldExample

# Run specific examples
example = BasicBrainExample()
example.run()
```

## Troubleshooting

### Common Issues

1. **NLM not available**
   - Ensure NLM is installed: `pip install -e .`
   - Check Python path: `python -c "import pynlm; print(pynlm.__version__)"`

2. **Missing dependencies**
   - Install dependencies: `pip install pybind11 scikit-build-core numpy rapidjson`
   - Check if all dependencies are installed

3. **Permission issues**
   - Ensure you have write permissions to the results directory
   - Check file permissions: `chmod +x *.py`

4. **Memory issues**
   - Large brain simulations may require more memory
   - Use smaller brain sizes for testing: `neurons=100` instead of `neurons=1000`

### Debugging

To debug specific examples, add logging statements:

```python
import logging

logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

# In your example code
logging.info(f"Processing step {step}")
logging.error(f"Error occurred: {e}")
```

## Learning Path

### For Beginners

Start with:
1. `basic_brain_example.py` - Learn the basics
2. `agent_world_example.py` - Understand agent-world interaction
3. `configuration_example.py` - Learn configuration management

### For Intermediate Users

Progress with:
1. `training_loop_example.py` - Implement learning algorithms
2. `benchmarking_example.py` - Analyze performance
3. `developmental_example.py` - Study developmental processes

### For Advanced Users

Create your own examples using:
- Custom brain architectures
- Advanced learning algorithms
- Complex environment interactions
- Performance optimization techniques

## Contributing Examples

If you have useful NLM examples, you can contribute them by:

1. Creating a new example script in the `examples/` directory
2. Adding documentation to the example
3. Including expected output format
4. Testing the example thoroughly
5. Submitting a pull request

Example template:

```python
"""
[Description of your example]

This example demonstrates [specific concept or technique].

[Your detailed explanation here]
"""

import pynlm
import numpy as np

def main():
    # Your example code here
    pass

if __name__ == "__main__":
    main()
```

## Acknowledgments

These examples build upon the foundation laid by:

- The original NLM C++ framework
- The improved Python bindings
- The command-line interface tools
- The configuration and validation system
- The performance benchmarking infrastructure

Special thanks to all contributors who have helped improve the NLM framework.

---

## Quick Start Summary

1. **Install NLM**: `pip install -e .`
2. **Run basic example**: `python examples/basic_brain_example.py`
3. **Explore more examples**: `ls examples/`
4. **Check results**: `ls results/`
5. **Use CLI**: `nlm help`

Enjoy exploring the NLM framework!

© 2026 NLM Project