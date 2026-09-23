# NLM Python Examples

This directory contains example scripts demonstrating how to use the NLM Python bindings.

## Examples

### basic_examples.py
Basic examples for getting started with NLM:
- Simple brain simulation
- Agent-world interaction
- Configuration and customization

### advanced_examples.py
Advanced examples showing complex usage patterns:
- Neuromodulation features
- Checkpoint management
- Developmental stages
- Pattern analysis and statistics

## Running Examples

To run the examples, make sure you have the NLM Python bindings installed:

```bash
# Install the package
pip install -e .

# Run basic examples
python -m pynlm.examples.basic_examples

# Run advanced examples  
python -m pynlm.examples.advanced_examples
```

## Usage

The examples demonstrate:

1. **Basic functionality**: Creating and running simple brain simulations
2. **Agent-environment interaction**: How brains interact with simulated worlds
3. **Learning features**: Using neuromodulation, reward, and development
4. **State management**: Saving and loading simulation checkpoints
5. **Advanced patterns**: Pattern analysis and statistical tracking

Each example includes detailed comments explaining the concepts and code.

## Requirements

- Python 3.8+
- NLM Python bindings (pynlm)
- NumPy (optional, for advanced examples)

## Notes

- These examples assume the NLM Python bindings are properly installed
- Some examples require C++ compilation of the NLM core library
- The examples demonstrate both basic and advanced usage patterns
