# README for Python Package

This directory contains the Python bindings for NLM (Neural Learning Machine).

## Installation

```bash
pip install -e .
```

or install from source:

```bash
cd python
mkdir build
cd build
cmake ..
make
pip install .
```

## Basic Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize a brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

## Examples

Check out the easy_usage.md and HOW_TO_USE.md files for comprehensive examples.

## Advanced Features

The enhanced API provides these additional features for experienced users:

1. **Template Configurations**: `createConfigFromTemplate("explorer")`, `createConfigFromTemplate("planner")`
2. **Validation**: `config.validate()` for error checking
3. **Batch Operations**: `batchUpdateAgents()`, `batchInjectCurrent()`
4. **Performance Monitoring**: `profileSimulation()`
5. **Checkpointing**: `createCheckpoint()`, `restoreCheckpoint()`
6. **Multi-Agent Systems**: `createMultiAgentSystem()`
7. **Configuration from Dictionary**: `createConfigFromDict(config_dict)`
8. **Error Handling**: `withErrorHandling()`, `safeExecute()`
9. **World Presets**: `worldPreset("maze")`, `worldPreset("open")`
10. **Brain Types**: `brainTemplate("simple")`, `brainTemplate("complex")`

## Development

To build the Python bindings from source:

```bash
mkdir build
cd build
cmake ..
make
```

The Python module will be created as `pynlm.cpython-<version>-<platform>.so`.

## License

MIT
