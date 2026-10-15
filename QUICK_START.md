# Quick Start Guide

This guide provides rapid access to the most important features of the NLM project.

## What is NLM?

NLM (熙然) is an experimental artificial developmental brain system that implements real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

## Quick Installation

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Install Python bindings
pip install -e ./python
```

## Basic Usage

### Create a Brain

```python
import pynlm

# Create a simple brain with 100 neurons
brain = pynlm.create_brain(neuron_count=100, timestep=0.001)

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Run Learning Test

```python
# Test learning and synaptic plasticity
results = pynlm.test_learning(brain, steps=500)

print(f"Total spikes: {results['total_spikes']}")
print(f"Learning occurred: {results['avg_firing_rate'] > 0}")
```

### Run Phase 6 Demo

```python
# Run complete Phase 6 integration test
results = pynlm.run_phase6_demo(max_steps=1000)

print(f"Integration successful: {results['total_reward'] > 0}")
print(f"Memory episodes: {results['memory_episodes_stored']}")
```

## Command Line Interface

Use the built-in CLI for quick testing:

```bash
# Run Phase 6 demo (with 200 neurons for 1000 steps)
nlm-demo --neurons 200 --steps 1000

# Test learning (with 50 neurons for 200 steps)
nlm-test --neurons 50 --steps 200

# Show help
nlm-demo --help
```

## Testing

Run the test suite:

```bash
# Test Python bindings (requires C++ extension)
python -m pytest python/tests/ -v

# Run C++ tests
mkdir build && cd build
cmake ..
make
./test_runner
```

## Key Features

### Neural Computation
- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation
- Synaptic plasticity (STDP, Hebbian)

### Memory Systems
- Working memory for active information
- Episodic memory for experience storage
- Associative memory for pattern associations

### Neuromodulation
- Dopamine for reward processing
- Curiosity for exploration
- Novelty detection

### Cognition
- Neural planning and action selection
- Concept formation
- Attentional focus

### Development
- Developmental stages affecting plasticity
- Age-dependent structural changes
- Maturation processes

## Performance Tips

- Use smaller neuron counts (50-500) for testing
- Set timestep to 0.001 for balance of accuracy/speed
- Monitor memory usage for large simulations
- Use `--neurons 100 --steps 100` for quick tests

## License

MIT License. See LICENSE file for details.

## Support

For issues and questions:
- GitHub: https://github.com/nlm-project/nlm/issues
- Documentation: https://nlm.readthedocs.io