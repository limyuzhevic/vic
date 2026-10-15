# pynlm - Python Bindings for NLM

This package provides Python bindings for the NLM (Neural Learning Machine) project,
which is an experimental artificial developmental brain system implemented in C++.

The NLM simulates real spiking neural computation with:
- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)
- Integrated memory, prediction, cognition, and neuromodulation systems

## Installation

### From Source

```bash
# Build and install the C++ extension
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Install Python bindings
pip install -e .
```

### Using pip (if available)

```bash
pip install pynlm
```

## Usage Examples

### Basic Brain Creation

```python
import pynlm

# Create a new brain with default settings
brain = pynlm.create_brain(neuron_count=500, timestep=0.001)
if brain:
    print("Brain initialized successfully")
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
```

### Running Learning Experiments

```python
import pynlm

# Test learning and synaptic plasticity
brain = pynlm.create_brain()
if brain:
    results = pynlm.test_learning(brain, steps=1000)
    print(f"Learning results: {results}")
```

### Phase 6 Integration Demo

```python
import pynlm

# Run the complete Phase 6 integration test
results = pynlm.run_phase6_demo()
print(f"Integration successful: {results['total_reward'] > 0}")
```

## API Reference

### Core Classes

- `Brain` - Main neural system interface
- `Neuron` - LIF neuron model
- `Synapse` - Neural connection with plasticity
- `Config` - Configuration management
- `Logger` - System logging

### Memory Systems

- `WorkingMemory` - Transient active information
- `EpisodicMemory` - Experience storage
- `AssociativeMemory` - Pattern associations

### Neuromodulation

- `Dopamine` - Reward and reinforcement
- `Curiosity` - Exploration motivation
- `Novelty` - Novelty detection
- `PredictionError` - Prediction error signals

### Environment Interface

- `SimpleWorld` - Virtual environment
- `SensoryPercept` - Sensory input processing
- `Action` - Motor output commands

### Development System

- `DevelopmentSystem` - Cognitive development stages
- `DevelopmentalStage` - Developmental phase enumeration

## Performance Considerations

The NLM project is computationally intensive due to the real spiking neural simulation.
For best performance:

- Use larger timesteps (e.g., 0.001 instead of 0.0001)
- Limit the number of neurons for quick testing (e.g., 100-500 neurons)
- Consider using multi-threading if available
- Monitor memory usage for large simulations

## Licensing

MIT License. See the LICENSE file for details.

## Project Links

- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io