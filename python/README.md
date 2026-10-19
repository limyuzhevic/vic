"""
# NLM Python Bindings - Comprehensive Examples and Documentation

This module provides comprehensive examples and documentation for the enhanced NLM Python bindings.
The NLM (熙然) project creates experimental artificial developmental brains with neural dynamics,
learning systems, and memory integration.

## Overview

The Python bindings expose the full C++ neural simulation framework through pybind11, enabling:

- Neural network simulation with spiking neurons
- Agent-brain interface for world interaction
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty detection)
- Developmental stages and structural plasticity
- Prediction and learning systems
- Episode management and learning
- Serialization and checkpointing
- Performance profiling

## Installation

The package is built with scikit-build-core using pybind11. Install from the source:

```bash
pip install -e .
# or build and install
python -m pip install .
```

## Basic Usage

```python
import nlm

# Create default configuration and brain
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)

# Create world for simulation
world = nlm.createSimpleWorld()

# Create agent brain interface
agent = nlm.createAgentBrain(brain)

print(f"Created NLM brain with {brain.getTotalNeuronCount()} neurons")
print(f"Agent expects sensory input of size {agent.getSensoryInputSize()}")
print(f"Agent produces motor output of size {agent.getMotorOutputSize()}")
```

## Key Features

### 1. Neural Brain Core
- LIF neuron dynamics with event-driven spike propagation
- Multiple brain regions with populations of neurons
- Excitation/inhibition balance management
- Synaptic plasticity (STDP, Hebbian)

### 2. Agent Interface
- Sensory input processing
- Motor command decoding
- Neuromodulation integration
- Development stage management

### 3. Memory Systems
- Working memory with persistent activity
- Episodic memory for experience recording
- Associative memory for pattern-based learning

### 4. Learning and Development
- Developmental stages (Initial → CriticalPeriod → Maturation → Adult → Aging)
- Curiosity-driven exploration
- Novelty detection
- Reward prediction error
- Structural plasticity

### 5. Simulation Features
- Episode-based learning
- Checkpoint save/load
- Performance profiling
- Batch processing capabilities

## Package Structure

```
nlm/
├── brain.py                # Brain core simulation
├── agent.py                # AgentBrain interface
├── config.py               # Configuration management
├── world.py                # World simulation
├── memory.py               # Memory systems
├── learning.py             # Learning mechanisms
├── development.py          # Developmental processes
├── serialization.py        # Save/load functionality
├── profiling.py            # Performance monitoring
└── utils.py                # Utility functions
```

## Error Handling

The bindings provide specific exception types:

- `NLMError`: General NLM runtime errors
- `ValidationError`: Input validation failures
- `SerializationError`: File I/O and serialization issues
- `CallbackError`: Callback mechanism errors

```python
import nlm

try:
    # Create brain and run simulation
    config = nlm.createDefaultConfig()
    brain = nlm.createBrain(config)
    
    # Validate input
    if not brain.getTotalNeuronCount() > 0:
        raise nlm.ValidationError("Brain must have neurons")
        
    print("Brain created successfully")
except nlm.ValidationError as e:
    print(f"Validation error: {e}")
except nlm.NLMError as e:
    print(f"NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Performance Considerations

- Use batch processing for multiple steps
- Enable checkpointing for long-running simulations
- Monitor performance metrics during execution
- Use numpy arrays for vector operations

## Integration

The NLM Python bindings can integrate with:

- Machine learning frameworks (TensorFlow, PyTorch)
- Neuroscience toolkits (NEURON, NetPyNE)
- Simulation environments (Gym, PyBullet)
- Data analysis tools (pandas, numpy)
- Visualization libraries (Matplotlib, Plotly)

## Next Steps

1. **Getting Started**: Start with the `tutorials/basic_usage.md` for fundamental usage
2. **Complete Example**: Run `examples/complete_example.py` for comprehensive demonstration
3. **Advanced Features**: Study `guides/advanced_features.md` for optimization techniques
4. **Integration**: Check `guides/integration_examples.md` for external system connections
5. **Error Handling**: Refer to `guides/error_handling.md` for robust error management
6. **Performance Optimization**: Study `guides/performance_optimization.md` for optimization techniques
7. **API Reference**: Use the `documentation/api_reference.md` for complete class and method documentation

## Research Disclaimer

NLM is a research project investigating computational brain-like systems. It does not claim to reproduce biological brains or provide human-level intelligence, consciousness, or sentience.

The project investigates:
- Integration of memory systems with neural dynamics
- Coordinated neuromodulation effects on plasticity
- Central prediction organizing principles
- Developmental stage modulation of learning
- Memory consolidation through replay
- Coherent brain loop functionality

## References

For more information, see:
- NLM Documentation: https://nlm.readthedocs.io
- NLM GitHub: https://github.com/nlm-project/nlm
- Related Publications: Check the docs/ directory for scientific papers

Copyright (c) 2026 NLM Authors. MIT License.
"""