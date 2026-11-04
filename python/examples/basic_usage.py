# NLM Python Bindings - Comprehensive Examples

This file contains comprehensive examples demonstrating how to use the improved NLM Python bindings.
It covers basic usage, advanced features, and real-world applications.

## Examples Covered:

1. **Basic Simulation** - Simple brain initialization and stepping
2. **Agent-World Integration** - Complete agent-environment interaction
3. **Brain Inspection and Debugging** - Tools for exploring brain state
4. **Advanced Learning** - Using neuromodulation and plasticity
5. **Performance Monitoring** - Tools for simulation analysis
6. **Batch Processing** - Multiple simulations and parameter sweeps
7. **Memory Management** - Working with brain checkpoints and serialization
8. **Text Processing** - Phase 6 text-as-a-sense capabilities

## Getting Started

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 100 steps
for i in range(100):
    brain.step(i)

# Get statistics
stats = pynlm.getBrainStatistics(brain)
print(pynlm.formatSimulationResults(stats))
```