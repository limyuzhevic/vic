#!/usr/bin/env bash
# Installation script for NLM examples

# Create examples directory structure
mkdir -p examples
mkdir -p examples/basic
mkdir -p examples/learning
mkdir -p examples/advanced
mkdir -p examples/visualization
mkdir -p examples/custom

# Copy example files to appropriate directories
cp examples/advanced_demo.py examples/
cp -r docs/ examples/docs/

# Make Python files executable
chmod 755 examples/advanced_demo.py

# Create requirements.txt for examples
print "# NLM Examples Dependencies" > examples/requirements.txt
print "matplotlib>=3.0" >> examples/requirements.txt
print "numpy>=1.19" >> examples/requirements.txt
print "scipy>=1.5" >> examples/requirements.txt

# Create README.md for examples
cat > examples/README.md << 'EOF'
# NLM Examples

This directory contains comprehensive examples for the NLM (熙然) neural simulation framework.

## Overview

The examples demonstrate various aspects of NLM:

1. **Basic Examples**: Simple NLM setup and simulation
2. **Learning Examples**: Learning mechanisms and plasticity
3. **Advanced Examples**: Multi-agent and training sessions
4. **Visualization Examples**: Data visualization and analysis
5. **Custom Extensions**: Custom NLM implementations

## Usage

### Running Examples

To run the comprehensive examples:

```bash
cd /path/to/nlm/examples
python advanced_demo.py
```

This will run all examples in sequence.

### Individual Examples

Each example group can be run individually by importing the appropriate class:

```python
from examples.advanced_demo import BasicNLMExample, LearningExamples, AdvancedExamples

# Run basic examples
BasicNLMExample.simple_brain_simulation()

# Run learning examples
LearningExamples.learning_experiment()

# Run advanced examples
AdvancedExamples.multi_agent_simulation()
```

### Dependencies

The examples require additional packages:

```bash
pip install -r requirements.txt
```

### Example Categories

#### Basic Examples
- Simple brain and world simulations
- Basic interaction patterns
- Initialization and configuration

#### Learning Examples
- Plasticity and adaptation demonstrations
- Memory system examples
- Learning experiment setups

#### Advanced Examples
- Multi-agent environments
- Complete training sessions
- Visualization and analysis

#### Custom Extensions
- Custom validator implementations
- Enhanced convenience wrappers
- Performance benchmarking

#### Practical Examples
- Perceptual learning applications
- Decision-making simulations
- Adaptive behavior examples

## Requirements

To use these examples, you need:

1. NLM Python bindings installed
2. Required Python packages (matplotlib, numpy, scipy)
3. A working NLM installation

## Notes

- Some examples are conceptual and may require adaptation
- Examples demonstrate typical usage patterns
- Real-world applications may require additional configuration
- Performance characteristics vary with system resources

## Getting Help

For help with examples:

1. Check the Python API documentation
2. Refer to the NLM documentation
3. Review example code comments
4. Consult NLM forums or support channels

## Future Enhancements

This examples collection will be continuously enhanced with:

- New use cases and applications
- Performance optimization examples
- Integration with other frameworks
- Advanced debugging techniques
- Research use cases
EOF

# Create .gitignore for examples
cat > examples/.gitignore << 'EOF'
# Python cache files
__pycache__/
*.pyc
*.pyo

# Virtual environments
venv/
.env/

# Temporary files
*.tmp
*.log

# Plot files
*.png
*.jpg
*.svg

# Test output
output/
reports/

# Checkpoint files
checkpoints/
analysis/

# Example outputs
example_output/
results/
EOF

# Make the script executable
chmod 755 examples/setup.sh

echo "Examples setup completed!"
echo "You can now run the examples using: python examples/advanced_demo.py"
