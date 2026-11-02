# NLM Python Examples - Complete Structure

This directory contains practical examples for using the NLM Python API.

## Directory Structure

```
examples/
├── README.md                      # This file
├── basics/                        # Fundamental concepts
│   ├── basic_brain.py              # Simple brain creation and simulation
│   ├── brain_step.py               # Brain stepping and statistics
│   ├── config_examples.py          # Configuration examples
│   ├── minimal_example.py          # Quick minimal test
│   └── config_test.py              # Configuration file I/O test
│
├── agent_world/                    # Agent and world interaction
│   ├── simple_agent.py             # Basic agent simulation
│   └── agent_training.py           # Learning and training examples
│
├── advanced/                       # Advanced features
│   └── plasticity_demo.py          # Plasticity learning demonstrations
│
└── jupyter/                       # Jupyter notebooks (for documentation)
    └── comprehensive_tutorial.ipynb
```

## Examples Summary

| Example | Category | Difficulty | Main Concepts |
|---------|----------|------------|---------------|
| `basic_brain.py` | Basics | Easy | Brain creation, simulation |
| `brain_step.py` | Basics | Medium | Advanced stepping, monitoring |
| `config_examples.py` | Basics | Easy | Configuration management |
| `minimal_example.py` | Basics | Easy | Quick test |
| `config_test.py` | Basics | Easy | Configuration file I/O |
| `simple_agent.py` | Agent | Medium | Agent-world interaction |
| `agent_training.py` | Agent | Hard | Learning and training |
| `plasticity_demo.py` | Advanced | Hard | Plasticity mechanisms |

## Getting Started

### 1. Install Dependencies
```bash
pip install matplotlib numpy
```

### 2. Run Basic Examples
```bash
# Run basic brain example
python examples/basics/basic_brain.py

# Run configuration examples
python examples/basics/config_examples.py
```

### 3. Run Agent Examples
```bash
# Run simple agent example
python examples/agent_world/simple_agent.py

# Run training example
python examples/agent_world/agent_training.py
```

### 4. Run Advanced Examples
```bash
# Run plasticity demo
python examples/advanced/plasticity_demo.py
```

### 5. Run All Examples
```bash
#!/bin/bash
# Run all examples sequentially
echo "Running NLM Python Examples"
echo "==========================="

for example in examples/basics/*.py; do
    echo "\n--- $example ---"
    python "$example"
    echo "---"
done

echo "\nRunning agent examples..."
for example in examples/agent_world/*.py; do
    echo "\n--- $example ---"
    python "$example"
    echo "---"
done

echo "\nAll examples completed!"
```

## Example Categories

### Basics Examples
- **basic_brain.py**: Fundamental brain creation and simulation
- **brain_step.py**: Advanced simulation control and monitoring
- **config_examples.py**: Configuration management and parameter tuning
- **minimal_example.py**: Quick test for getting started
- **config_test.py**: Configuration file I/O testing

### Agent World Examples
- **simple_agent.py**: Basic agent-world interaction and embodied cognition
- **agent_training.py**: Learning and training over multiple episodes

### Advanced Examples
- **plasticity_demo.py**: Plasticity mechanisms (STDP and Hebbian learning)

## Example Features

### Common Features
- Comprehensive documentation and comments
- Real-time monitoring and statistics
- Visualization outputs (PNG files)
- Error handling and validation
- Performance timing

### Example-Specific Features

#### basic_brain.py
- Basic brain creation with custom configuration
- Simulation statistics logging
- Performance timing
- Progress output

#### brain_step.py
- Advanced stepping methods (step with and without time)
- Comprehensive statistics collection
- Multiple visualization plots
- Output file generation

#### config_examples.py
- Configuration creation and modification
- Configuration validation
- JSON file I/O (save/load)
- Backward compatibility demonstration

#### simple_agent.py
- Complete agent-world simulation
- Embodied cognition demonstration
- Neuromodulation (dopamine, curiosity, novelty)
- Development system integration
- Motor output and sensory input processing

#### agent_training.py
- Multi-episode training simulation
- Epsilon-greedy exploration strategy
- Learning dynamics tracking
- Statistical analysis and visualization
- Progress monitoring

#### plasticity_demo.py
- STDP (Spike-Timing-Dependent Plasticity) demonstration
- Hebbian learning demonstration
- Activity correlation analysis
- Network statistics tracking
- Multi-panel visualization

## Running Examples Locally

### From Command Line
```bash
# Change to project directory
cd /path/to/nlm

# Install dependencies (if not already installed)
pip install matplotlib numpy

# Run a specific example
python examples/basics/basic_brain.py

# Run all basics examples
for example in examples/basics/*.py; do
    python "$example"
    echo "---"
done
```

### From Python
```python
import subprocess
import sys

# Run specific examples
subprocess.run([sys.executable, "examples/basics/basic_brain.py"])
subprocess.run([sys.executable, "examples/agent_world/simple_agent.py"])
```

### In Jupyter Notebooks
```python
# Import and run examples
exec(open("examples/basics/basic_brain.py").read())
exec(open("examples/agent_world/simple_agent.py").read())
```

## Example Customization

### Modify Parameters
Change configuration values in examples:
```python
# In any example
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.2)
config.set("simulation_timestep", 0.0005)
config.set("plasticity.stdp.learning_rate", 0.005)
```

### Add Custom Features
Extend examples with your own code:
```python
# Add custom monitoring
def monitor_brain(brain, step):
    # Add your monitoring logic here
    firing_rate = brain.getAverageFiringRate()
    ei_ratio = brain.getExcitationInhibitionRatio()
    print(f"Step {step}: FR={firing_rate:.2f} Hz, E/I={ei_ratio:.3f}")

# Use in simulation loop
for step in range(100):
    brain.step(step)
    monitor_brain(brain, step)
```

### Create New Examples
Create new example files:
```python
# examples/custom/your_feature.py
"""
Your custom feature example.
"""
import pynlm

def your_feature_example():
    # Your implementation here
    pass

if __name__ == "__main__":
    your_feature_example()
```

## Dependencies and Installation

### Required Dependencies
- `pynlm`: The NLM neural simulation library
- `matplotlib`: For plotting and visualization
- `numpy`: For numerical operations
- `time`: For timing (standard library)

### Installation
```bash
# From source (development)
pip install -e .  # Install in development mode

# Or regular install
pip install .

# Or install with all dependencies
pip install matplotlib numpy scikit-build-core
```

### Dependency Verification
```python
import pynlm
import matplotlib
import numpy

print("NLM version:", pynlm.__version__ if hasattr(pynlm, '__version__') else 'unknown')
print("Matplotlib version:", matplotlib.__version__)
print("NumPy version:", numpy.__version__)
```

## Troubleshooting

### Common Issues

1. **Missing Dependencies**:
   ```bash
   pip install matplotlib numpy
   ```

2. **Import Errors**:
   ```python
   import pynlm
   # Check installation
   print(pynlm.__version__)
   ```

3. **Simulation Slow**:
   ```python
   # Reduce simulation size
   config.set("brain.neuron_count", 100)
   ```

4. **Memory Issues**:
   ```python
   # Run garbage collection
   import gc
   gc.collect()
   ```

5. **Output File Permissions**:
   ```bash
   chmod 644 *.png
   ```

### Debugging Examples

Add debugging to examples:
```python
import logging
logging.basicConfig(level=logging.DEBUG)

# Add debug prints
print(f"Debug: Step {step}, Spikes: {brain.getTotalSpikeCount()}")
```

## Example Best Practices

### 1. Code Structure
```python
"""
Example: Brief description

Key features:
- List key features
- Explain what the example demonstrates

Usage:
- How to run the example
- Any special requirements
"""
import pynlm
import matplotlib.pyplot as plt

def example_function():
    """Example function with clear documentation."""
    # Implementation
    pass

if __name__ == "__main__":
    example_function()
```

### 2. Error Handling
```python
def safe_operation():
    """Demonstrate safe error handling."""
    try:
        # Your code here
        pass
    except Exception as e:
        print(f"Error: {e}")
        raise
```

### 3. Performance
```python
import time

start_time = time.time()

# Your simulation code here

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
```

### 4. Documentation
```python
"""
Example: Clear description

This example demonstrates X, Y, and Z.

Key concepts:
1. Concept 1: Explanation
2. Concept 2: Explanation
3. Concept 3: Explanation

Expected output:
- Brief description of what the example shows
- Example output format

References:
- Link to documentation
- Link to related examples
"""
```

## Contributing Examples

### Add Your Example

1. **Choose Directory**: Select appropriate category (basics, agent_world, advanced)
2. **Create File**: Create your example file with `.py` extension
3. **Document**: Add comprehensive comments and documentation
4. **Test**: Test your example thoroughly
5. **Update**: Update this README or create a README in your directory

### Example Guidelines

- **Educational Focus**: Examples should teach something useful
- **Self-Contained**: Examples should run independently
- **Well-Documented**: Include comprehensive comments and docstrings
- **Tested**: Include error handling and validation
- **Visual**: Include visualizations where appropriate
- **Performance**: Include timing and performance considerations

### Adding to Examples Table

Update the examples table in this README or create a directory-specific README:

```markdown
| Example | Category | Difficulty | Main Concepts |
|---------|----------|------------|---------------|
| `basic_brain.py` | Basics | Easy | Brain creation, simulation |
| `your_example.py` | Basics | Easy | Your concepts here |
```

## Example Testing

### Run All Examples
```bash
# Quick test of all examples
python -c "
import sys
for example in ['examples/basics/basic_brain.py', 'examples/basics/minimal_example.py']:
    try:
        exec(open(example).read())
        print(f'✓ {example}')
    except Exception as e:
        print(f'✗ {example}: {e}')
        sys.exit(1)
print('All basic examples passed!')
"
```

### Performance Testing
```python
import time
import pynlm

# Performance test with different configurations
for neuron_count in [100, 500, 1000]:
    start = time.time()
    config = pynlm.createDefaultConfig()
    config.set('brain.neuron_count', neuron_count)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(50):
        brain.step(step)
    
    elapsed = time.time() - start
    print(f'Neurons: {neuron_count}, Time: {elapsed:.2f}s, Rate: {50/elapsed:.1f} steps/s')
```

## Example Evolution

### Version Control
```bash
# Version control for examples
git add examples/
git commit -m "Add new example: basic_brain.py"
git log --oneline --graph
```

### Example Updates
- Examples will be updated as NLM evolves
- New features will be demonstrated in advanced examples
- Performance improvements will be shown in benchmarks

## Conclusion

These examples provide a comprehensive introduction to the NLM Python API and demonstrate the framework's capabilities across different use cases. From basic simulations to advanced agent training, the examples showcase the flexibility and power of the NLM neural simulation framework.

### Next Steps for Contributors

1. **Explore**: Run existing examples to understand the API
2. **Learn**: Study the code structure and patterns
3. **Create**: Add your own examples to help other users
4. **Improve**: Enhance existing examples with new features
5. **Share**: Contribute examples to the community

The examples directory will continue to grow as the NLM framework evolves, providing users with practical examples for every aspect of the neural simulation framework.
