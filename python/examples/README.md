# NLM Python Examples Directory

This directory contains example scripts demonstrating the NLM Python API capabilities.

## Overview

The examples are organized by complexity and use case:

### Basic Examples (0-5)
- Quick introduction to the NLM Python API
- Simple brain creation and initialization
- Basic simulation loops

### Intermediate Examples (10-20)
- World interaction and sensory processing
- Complete learning agents
- Multi-system integration

### Advanced Examples (30-40)
- Complex simulation scenarios
- Development and growth processes
- Long-running experiments

### Utility Examples (50-70)
- Demonstration of new convenience functions
- Error handling best practices
- Performance monitoring techniques

### Comparison Examples (60-80)
- Configuration comparison
- Performance benchmarking
- System capability analysis

## Example Structure

Each example file follows a consistent structure:

1. **Description**: Brief overview of the example's purpose
2. **Imports**: Required NLM imports
3. **Function**: Main function demonstrating the concept
4. **Execution**: Example execution with output
5. **Comments**: Inline documentation and explanations

## How to Use Examples

### Running Examples

All examples can be run directly from the command line:

```bash
# From within the examples directory
cd python/examples
python 00_hello_world.py

# Or run specific examples
python 10_vision_world.py
python 20_complete_agent.py
```

### Installing and Running All Examples

1. **Ensure NLM is installed**:
```bash
pip install pynlm
```

2. **Navigate to the examples directory**:
```bash
cd /path/to/nlm/python/examples
```

3. **Run examples in order** (recommended for learning):
```bash
python 00_hello_world.py
python 10_vision_world.py
python 20_complete_agent.py
python 30_exploration.py
python 40_developmental.py
python 50_convenience_demo.py
python 60_config_comparison.py
python 70_error_handling.py
python 80_performance_monitoring.py
```

## Example-by-Example Guide

### 00_hello_world.py
**Simplest possible NLM example**

This is the absolute minimum code needed to create and use an NLM brain.
It demonstrates the basic workflow:
1. Create brain with default configuration
2. Initialize the brain
3. Run a simulation step
4. Print basic statistics

**Recommended learning**: Run this first to understand the basic API.

### 10_vision_world.py
**Brain in a visual world**

This example shows how to:
- Create a 2D world with visual sensors
- Set up an agent with vision capabilities
- Run a complete perception-action loop
- Enable learning systems (reward and curiosity)

**Recommended learning**: Run after hello world to understand world interaction.

### 20_complete_agent.py
**Full-featured learning agent**

This demonstrates a complete learning agent with:
- Research-grade configuration
- All learning systems enabled
- Extended simulation for behavior analysis
- Comprehensive metric tracking

**Recommended learning**: Good intermediate example for understanding advanced features.

### 30_exploration.py
**World exploration and navigation**

This shows:
- Agent navigation in a complex world
- Resource finding and interaction
- Long-term behavior learning
- Performance monitoring

**Recommended learning**: Good for understanding agent behavior and exploration.

### 40_developmental.py
**Developmental processes and maturation**

This demonstrates:
- Developmental stage transitions
- Age-dependent learning changes
- Long-term plasticity effects
- Stage-specific behaviors

**Recommended learning**: Best for understanding development and life-cycle processes.

### 50_convenience_demo.py
**New convenience functions**

This showcases the new high-level API functions:
- `createQuickBrain()`: Quick brain setup
- `createAgentSetup()`: Complete agent configuration
- `run_episode()`: Complete simulation wrapper
- Python property access

**Recommended learning**: Essential for using the new simplified API.

### 60_config_comparison.py
**Configuration comparison**

This compares different configuration presets:
- Default configuration (general purpose)
- Minimal configuration (quick testing)
- Research configuration (advanced experiments)

**Recommended learning**: Good for understanding configuration options and choosing appropriate settings.

### 70_error_handling.py
**Robust error handling**

This demonstrates:
- Graceful error handling
- Exception types available
- Best practices for robust code

**Recommended learning**: Essential for production code and robust applications.

### 80_performance_monitoring.py
**Performance monitoring**

This shows:
- Performance measurement techniques
- Benchmarking different configurations
- Analysis of simulation efficiency

**Recommended learning**: Important for optimizing performance and understanding system capabilities.

## Learning Path Recommendations

### For Beginners

1. Start with `00_hello_world.py` - understand the basic API
2. Move to `10_vision_world.py` - understand world interaction
3. Try `20_complete_agent.py` - explore learning systems

### For Intermediate Users

1. Work through `30_exploration.py` - understand behavior
2. Study `40_developmental.py` - understand development
3. Master `50_convenience_demo.py` - use new high-level functions

### For Advanced Users

1. Analyze `60_config_comparison.py` - optimize configurations
2. Implement `70_error_handling.py` - robust production code
3. Use `80_performance_monitoring.py` - performance optimization

## API Integration

These examples demonstrate the integration of new Python API features:

### New Convenience Functions

- **Quick setup**: `createQuickBrain()`, `createAgentSetup()`
- **High-level simulation**: `run_episode()`
- **Configuration presets**: `createDefaultConfig()`, `createMinimalConfig()`, `createResearchConfig()`

### Enhanced Documentation

- **Type hints**: Available for all functions and classes
- **Comprehensive docstrings**: Detailed parameter and return information
- **Property access**: Intuitive attribute-based access to brain properties

### Improved Error Handling

- **Descriptive errors**: Clear error messages for debugging
- **Exception types**: Custom exception classes for better error handling
- **Validation**: Parameter validation and type checking

## Extensibility

The examples can be easily extended:

### Adding New Examples

```python
# Create new example files
python/examples/90_advanced_feature.py
```

### Modifying Existing Examples

```python
# Customize examples for specific use cases
# Modify parameters, add features, extend functionality
```

### Creating Tutorials

```python
# Create tutorial-style examples
python/examples/tutorial_basic_usage.py
```

## Troubleshooting

### "Examples don't run"

```bash
# Ensure NLM is installed
pip install pynlm

# Check Python path
python -c "import pynlm; print(pynlm.__version__)"
```

### "Import errors"

```bash
# Install with all dependencies
pip install pynlm[all]

# Or install from source
pip install -e .
```

### "No output or errors"

```bash
# Check for syntax errors
python -m py_compile python/examples/00_hello_world.py

# Run with verbose output
python -v python/examples/00_hello_world.py 2>&1 | grep pynlm
```

## Future Enhancements

### Planned Examples

1. **Multi-agent systems**: Multiple agents interacting
2. **Advanced learning**: Complex learning scenarios
3. **Real-time applications**: Interactive applications
4. **Visualization**: Neural activity visualization
5. **Distributed computing**: Parallel simulation

### Custom Examples

Users can create their own examples:

```python
# Save custom example
python custom_example.py -> python/examples/99_custom_example.py
```

## Conclusion

The NLM Python examples directory provides comprehensive examples for users at all skill levels. The examples demonstrate the full power of the NLM Python API while remaining accessible to beginners.

Start with the basic examples and gradually work your way through the more advanced ones. The convenience functions make it easy to create complete simulations, while the examples show how to leverage all the new features of the enhanced Python API.
