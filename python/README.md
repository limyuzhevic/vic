# NLM Python Examples - README

This directory contains example scripts for using NLM Python bindings.
These examples cover basic to advanced usage patterns and can help you
get started with NLM or learn new features.

## Quick Start

### Prerequisites

1. **Install NLM Python bindings**:
   ```bash
   pip install -e .
   ```

2. **Build NLM C++ core** (if not already built):
   ```bash
   mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4
   ```

3. **Run examples**:
   ```bash
   # Run simple examples
   python simple_brain_example.py
   python agent_example.py
   python learning_example.py
   
   # Run advanced examples
   python advanced_example.py
   ```

## Examples by Category

### Basic Examples (Getting Started)

| File | Description | Difficulty |
|------|-------------|------------|
| `simple_brain_example.py` | Basic brain creation and simulation | Beginner |
| `agent_example.py` | Complete agent-world simulation | Intermediate |
| `learning_example.py` | Learning with plasticity | Intermediate |
| `comparison_example.py` | Brain configuration comparison | Intermediate |

### Advanced Examples (Advanced Usage)

| File | Description | Difficulty |
|------|-------------|------------|
| `advanced_example.py` | Multi-agent systems and complex experiments | Advanced |
| `experiment_example.py` | Experiment framework usage | Advanced |
| `benchmark_example.py` | Performance benchmarking | Advanced |

### Utility Scripts

| File | Description | Purpose |
|------|-------------|---------|
| `example_script.py` | Comprehensive demonstration script | Running all examples |
| `COMPREHENSIVE_GUIDE.md` | Complete documentation guide | Reference |

## Running Examples

### Step 1: Installation Verification

```bash
# Test if NLM is properly installed
python -c "import pynlm; print(f'NLM version: {pynlm.__version__}')"
```

### Step 2: Run Basic Examples

```bash
# Run simple brain example
python python/simple_brain_example.py

# Run agent example
python python/agent_example.py

# Run learning example
python python/learning_example.py
```

### Step 3: Run Advanced Examples

```bash
# Run advanced examples (may take longer)
python python/advanced_example.py
```

### Step 4: Run All Examples

```bash
# Run comprehensive example script
python python/examples/example_script.py
```

## Example Descriptions

### Basic Examples

#### 1. Simple Brain Example (`simple_brain_example.py`)
**What it does**: Creates a brain and runs a basic simulation.

**Key concepts demonstrated**:
- Basic brain creation with `createBrain()`
- Brain initialization with `initialize()`
- Running simulation steps with `step()`
- Monitoring brain statistics

**Best for**: Beginners new to NLM Python bindings.

#### 2. Agent Example (`agent_example.py`)
**What it does**: Complete agent-world simulation with sensory-motor loops.

**Key concepts demonstrated**:
- Creating brain, world, and agent components
- Sensory input processing with `processSensoryInput()`
- Motor output decoding with `decodeMotorCommand()`
- Action execution in the world with `applyMotorCommand()`
- Learning feature activation

**Best for**: Understanding complete agent behavior.

#### 3. Learning Example (`learning_example.py`)
**What it does**: Learning experiment with plasticity mechanisms.

**Key concepts demonstrated**:
- Learning-optimized brain configuration
- Brain configuration for learning
- Monitoring learning progress through statistics
- Plasticity mechanisms in action

**Best for**: Understanding learning and plasticity.

### Advanced Examples

#### 4. Advanced Example (`advanced_example.py`)
**What it does**: Multi-agent systems, performance benchmarking, and experimental framework.

**Key concepts demonstrated**:
- Multi-agent systems with different configurations
- Performance benchmarking of different brain setups
- Experimental framework for controlled testing
- Advanced state management and persistence
- Complex learning scenarios with neuromodulation

**Best for**: Advanced users and researchers.

## Configuration Examples

### Brain Configuration Templates

```python
import pynlm

# Quick presets for common use cases
templates = {
    'small': pynlm.createConfigWithNeuronCount(500),
    'medium': pynlm.createConfigWithNeuronCount(1000),
    'large': pynlm.createConfigWithNeuronCount(5000),
    'learning': pynlm.createLearningConfig(),
    'exploration': pynlm.createExplorationConfig(),
}

# Use template
brain = pynlm.createBrain(templates['learning'])
```

### Common Patterns

```python
# Basic pattern
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Learning pattern
config = pynlm.createLearningConfig()
brain = pynlm.createBrain(config)
brain.initialize()
pynlm.configureBrainForLearning(brain)

# Exploration pattern
config = pynlm.createExplorationConfig()
brain = pynlm.createBrain(config)
brain.initialize()
```

## Troubleshooting

### Common Issues and Solutions

1. **Import Error - No module named 'pynlm'**:
   ```bash
   # Reinstall NLM Python bindings
   pip install -e .
   
   # If that doesn't work, install dependencies
   pip install pybind11 scikit-build-core
   ```

2. **Brain not firing or showing activity**:
   - Check if `initialize()` was called before `step()`
   - Try increasing simulation steps
   - Check brain configuration parameters

3. **Agent not moving**:
   - Ensure world updates are happening
   - Check motor command decoding
   - Verify action application in world

4. **Memory or performance issues**:
   - Reduce brain size with `createConfigWithNeuronCount()`
   - Use `optimizeBrainForSpeed()` for faster performance
   - Clear references when done

5. **Example taking too long**:
   - Reduce simulation steps in example files
   - Use smaller brain configurations
   - Run `optimizeBrainForSpeed()` before simulation

### Getting Help

1. **Check documentation**: See main NLM documentation for detailed API reference
2. **Run examples**: Examples demonstrate correct usage patterns
3. **Inspect source**: Look at example source code for implementation details
4. **Community**: Connect with other NLM users for support

## Example Development Guidelines

### Adding New Examples

1. **Start simple**: Begin with basic concepts
2. **Add complexity**: Gradually introduce new features
3. **Include output**: Provide meaningful output to demonstrate results
4. **Add comments**: Explain complex parts of the code
5. **Handle errors**: Include proper error handling

### Example Structure

```python
import pynlm

print("Example: Feature Description")
print("=" * 50)

def demonstrate_feature():
    """Demonstrate specific NLM feature."""
    # Implementation
    
def main():
    """Main function."""
    try:
        demonstrate_feature()
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()
```

### Best Practices

1. **Always initialize**: Call `initialize()` before using brain or agent
2. **Use meaningful names**: Choose descriptive variable names
3. **Add comments**: Explain complex parts of the code
4. **Include error handling**: Handle potential exceptions gracefully
5. **Add timing**: Measure and report performance metrics
6. **Clean up**: Clear references when done

## Testing Examples

### Unit Testing

If you want to test example functionality:

```bash
# Install testing dependencies
pip install pytest numpy

# Create test directory
mkdir tests

# Create example tests
# (Test files would go here)
```

### Integration Testing

```bash
# Test all examples
for example in python/*.py; do
    echo "Testing $example"
    python $example
    echo "Result: $?"
done
```

## Contributing

### Adding Examples

To contribute new examples:

1. Create a new example file in the `python/` directory
2. Follow the existing structure and style
3. Include comprehensive documentation
4. Test the example thoroughly
5. Add to this README if the example is significant

### Improving Examples

To improve existing examples:

1. Fix any bugs or issues
2. Add missing documentation
3. Improve code clarity
4. Add new features or use cases
5. Update compatibility notes

### Documentation

Keep documentation up-to-date:

1. Update examples README when adding new examples
2. Update main documentation when examples change
3. Keep examples consistent with current NLM API
4. Add migration notes for deprecated features

## License

This examples directory is part of the NLM project and follows the same licensing terms.

See the main NLM repository for full license information.

---

**Last Updated**: $(date)
**Version**: 0.1.0
**Authors**: NLM Project Contributors

## Related Resources

1. **Main NLM Documentation**: See `docs/` directory for detailed documentation
2. **Python API Reference**: Check `python/bindings.cpp` for complete API
3. **Configuration Files**: See `configs/` directory for configuration templates
4. **Build Instructions**: See `README.md` for building from source

**Need Help?**

- Check the main NLM documentation
- Run examples to see actual usage
- Look at example source code for implementation details
- Contact NLM project team for technical support