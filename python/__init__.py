# NLM Python Bindings Module

This is the main package initializer for the NLM Python bindings module (pynlm).

The NLM (Neural Learning Machine) Python bindings provide a high-level interface to the NLM neural simulation framework, allowing users to create and simulate artificial brains directly from Python scripts.

## What You Can Do

- Create virtual brains with neurons and synapses
- Simulate brain activity and learning
- Interact with simulated environments
- Build intelligent agents that learn from experience
- Explore brain-inspired computation

## Quick Examples

See the detailed examples in README.md for:
- Creating a simple brain
- Complete agent simulations
- World interaction
- Learning and development

## Documentation

- **README.md** - Comprehensive documentation and examples
- **Python API** - Complete API reference in README.md
- **Examples** - Working code examples in README.md

## Installation

Install the package using pip:

```bash
pip install .
```

Or install in development mode:

```bash
pip install -e .
```

## Usage

```python
import pynlm

# Create a default brain configuration
config = pynlm.createDefaultConfig()

# Create and initialize a brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

## Submodules

The pynlm package provides access to various brain components:

- **Brain** - Neural simulation with spiking neurons
- **Config** - Brain configuration system
- **AgentBrain** - Interface for agent-world interaction
- **SimpleWorld** - Simulated environment
- **SensoryInput** - Input processing (vision, audio, internal signals)
- **Action** - Motor control and actions
- **WorldObject** - Objects in the simulated world

## Advanced Usage

For more advanced usage including:
- Agent creation and control
- World interaction
- Learning and development
- Memory systems
- Neuromodulation

See the examples in README.md and explore the individual classes.

## Development

For development documentation:
- See the source code in the `src/` directory
- Check the documentation in the `docs/` directory
- Review the build system in `CMakeLists.txt`
- Explore the Python bindings in the `python/` directory

## License

MIT License - See LICENSE file for details.

## Authors

Research project — See README.md for more information about the NLM project and authors.