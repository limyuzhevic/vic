# Examples Directory

This directory contains example code and demonstrations for the NLM Python bindings.

## Overview

The examples directory provides working code samples that demonstrate various aspects of the NLM Python bindings:

1. **Quick Start** (`quick_start.py`) - Minimal working example to get started quickly
2. **Pythonic API** (`example_pythonic_api.py`) - Comprehensive demonstration of all Pythonic features

## How to Use Examples

### Quick Start

Run the quick start example to see a minimal working example:

```bash
python python/pynlm/examples/quick_start.py
```

This will create a basic simulation with configuration, brain, world, and agent brain, then run a simple simulation.

### Pythonic API Example

Run the comprehensive example to see all Pythonic features in action:

```bash
python python/pynlm/examples/example_pythonic_api.py
```

This will demonstrate:

- Pythonic configuration loading and validation
- Enhanced brain operations with context managers
- World simulation with spatial queries
- Agent brain interface with episode management
- Error handling and validation
- Performance benchmarking utilities
- Training environment setup

## Example Structure

Each example file follows a similar pattern:

1. **Imports**: Import pynlm as nlm and any additional modules
2. **Functions**: Define example functions demonstrating specific features
3. **Main execution**: Run examples with error handling
4. **Output**: Clear, informative output showing what's happening

## Example Features Demonstrated

### Basic Usage

- Creating configuration objects
- Initializing brain and world
- Running simulations
- Basic object manipulation

### Pythonic API Features

- Configuration loading from files
- Batch configuration updates
- Validation with error messages
- Context managers for resource management
- Convenience methods for common operations

### Advanced Usage

- Performance benchmarking
- Error handling and validation
- Large-scale configuration management
- Spatial queries and object management
- Training environment setup

### Agent Brain Features

- Episode management with context managers
- Sensory processing and observation
- Action selection and motor control
- Neuromodulation state management

## Running Examples

All examples are designed to be self-contained and can be run directly:

```bash
# Run specific example
python python/pynlm/examples/quick_start.py

# Run all examples programmatically
python python/pynlm/examples/example_pythonic_api.py
```

## Example Code Style

Examples follow best practices:

- **Clear documentation**: Each function has docstrings explaining purpose and usage
- **Error handling**: Robust error handling with descriptive messages
- **Informative output**: Clear output showing what's happening
- **Best practices**: Follows Pythonic conventions and best practices
- **Modularity**: Well-organized functions for easy understanding and reuse

## Contributing Examples

If you'd like to contribute new examples:

1. Create a new Python file in this directory
2. Follow the existing code style and conventions
3. Include clear documentation and examples
4. Add error handling for robustness
5. Make it self-contained and easy to understand

## Example Topics

Possible topics for new examples:

- **Multi-agent systems**: Managing multiple agents in the same world
- **Training loops**: Complete training examples with reward systems
- **Sensory processing**: Advanced sensory input processing
- **Custom objects**: Creating and managing custom world objects
- **Performance optimization**: Tips for optimizing NLM performance
- **Integration**: Integrating NLM with other Python libraries
- **Visualization**: Visualizing simulation results

## Example Templates

Here's a template for creating new examples:

```python
#!/usr/bin/env python3
"""
Example: [Topic Name]

Brief description of what this example demonstrates.
"""

import pynlm as nlm

def example_function_name():
    """Demonstrate specific functionality."""
    # Implementation
    pass

def main():
    """Run all examples in this file."""
    try:
        # Run examples
        example_function_name()
        
        print("\n✓ Example completed successfully!")
        
    except Exception as e:
        print(f"\n✗ Error: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
```

## Next Steps

After running the examples:

1. **Explore the source code**: Look at the test files to understand implementation details
2. **Customize examples**: Modify examples for your specific use case
3. **Create new examples**: Add examples for features not yet covered
4. **Extend functionality**: Add new Pythonic features to the bindings
5. **Share your work**: Contribute new examples to the NLM community

## Support

For help with examples or issues:

- Check the main NLM documentation
- Look at the test files for additional examples
- Consult the Pythonic API documentation in the main README.md
- Check for common issues and solutions in the troubleshooting section

---

*For more information, visit the main documentation in the root README.md or the NLM GitHub repository.*