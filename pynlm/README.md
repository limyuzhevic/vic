"""
Installation and Usage Instructions for NLM Python Bindings

This document provides comprehensive instructions for using the NLM Python bindings,
which provide a high-level interface to the NLM (Neural Learning Machine) neural
simulation framework.
"""

## Quick Start

```bash
# Clone the repository
cd /path/to/nlm

# Create and activate a virtual environment (recommended)
python -m venv nlm_env
source nlm_env/bin/activate  # On Windows: nlm_env\Scripts\activate

# Install the package
pip install -e .

# Install with development dependencies (for testing)
pip install -e .[dev]
```

## Basic Usage

```python
from nlm import Brain, Config, AgentBrain, SimpleWorld

# Create a brain with default configuration
config = Config()
brain = Brain(config)

# Create world
world = SimpleWorld()
world.configure(640, 480, 320, 240)

# Create agent brain
agent_brain = AgentBrain(brain, world)

# Initialize
brain.initialize()
agent_brain.initialize(world)

# Run simulation loop
for step in range(100):
    action = brain.produce_action()
    print(f"Step {step}: {action}")
```

## Advanced Usage

```python
from nlm import create_default_config, SimulationLoop, get_brain_statistics

# Create simulation with all components
config = create_default_config()
brain = create_brain(config)
world = create_simple_world()
agent_brain = create_agent_brain(brain, world)

# Initialize with world
brain.initialize()
agent_brain.initialize(world)

# Create simulation loop
loop = SimulationLoop(brain, world, agent_brain)

# Run episode
actions = loop.run_episode(100)

# Get performance statistics
stats = loop.get_performance_stats()
print(f"Performance: {stats['steps_per_second']:.1f} steps/sec")

# Get brain statistics
brain_stats = loop.get_brain_stats()
print(f"Brain stats: {brain_stats}")
```

## Examples

See the `examples/` directory for comprehensive tutorials:

### 1. Basic Brain Simulation
`examples/basic_brain.py` - Simple brain simulation with action output

### 2. Agent in World
`examples/agent_world.py` - Agent navigating a world

### 3. Brain Save/Load
`examples/checkpointing.py` - Saving and loading brain states

### 4. Performance Analysis
`examples/analysis.py` - Analyzing brain performance and statistics

### 5. Advanced Features
`examples/advanced.py` - Using advanced features and customization

## Command Line Interface

```bash
# Run demo scripts
nlm-demo
nlm --help

# Run specific demos
python examples/demo.py --demo basic
python examples/demo.py --demo advanced
```

## API Reference

See `pynlm/` directory for complete API documentation.

### Core Classes

- `Brain` - Central neural simulation engine
- `Config` - Configuration system
- `AgentBrain` - Interface between brain and world
- `SimpleWorld` - 2D world simulation
- `NeuralRegion` - Neural region container

### Sensory Classes

- `Vision` - Visual input processing
- `Audio` - Audio input processing
- `InternalSignals` - Internal state signals
- `SensoryPercept` - Combined sensory inputs

### Motor Classes

- `Action` - Motor output actions
- `ActionResult` - Results from world interactions

### Agent Classes

- `AgentBody` - Agent physical state

### Utility Classes

- `SimulationLoop` - High-level simulation management
- `ActionSequence` - Action management and analysis

## Type Definitions

The API includes comprehensive type definitions:

- `NeuronId`, `SynapseId`, `RegionId`, `PopulationId` - Unique identifiers
- `NeuronType`, `SynapseType`, `DevelopmentalStage` - Enum types
- `ActionType`, `MotorCommand`, `WorldObjectType` - Enum types
- `SimulationStep`, `Timestamp`, `MembranePotential` - Numeric types

## Error Handling

The API includes comprehensive error handling:

```python
from nlm import NLMError, ConfigurationError, BrainError, WorldError

try:
    brain = Brain(config)
    brain.initialize()
except NLMError as e:
    print(f"NLM Error: {e}")
except ConfigurationError as e:
    print(f"Configuration Error: {e}")
except BrainError as e:
    print(f"Brain Error: {e}")
except WorldError as e:
    print(f"World Error: {e}")
```

## Saving and Loading

```python
from nlm import save_brain_checkpoint, load_brain_checkpoint

# Save brain with metadata
metadata = {
    'episode': 123,
    'score': 85.5,
    'timestamp': time.time()
}
save_brain_checkpoint(brain, 'brain_checkpoint.json', metadata)

# Load brain
load_brain_checkpoint(brain, 'brain_checkpoint.json')
```

## Performance Tips

1. **Reuse objects**: Create Brain and World objects once and reuse them
2. **Batch operations**: Use vector operations when possible
3. **Memory management**: Clear unused sensory data regularly
4. **Configuration caching**: Cache frequently used configuration values
5. **Step optimization**: Use optimized step methods for common patterns

## Development

### Running Tests

```bash
# Run unit tests
pytest tests/

# Run tests with coverage
pytest --cov=pynlm --cov-report=html

# Run specific test module
pytest tests/test_basic.py
```

### Adding New Features

The Python bindings are designed to be extensible. New features can be added by:

1. Creating new Python classes in `pynlm/`
2. Implementing new C++ bindings in `python/bindings.cpp`
3. Adding examples in `examples/`
4. Updating documentation

### Building from Source

```bash
# Build the C++ extensions (if needed)
cd build
mkdir -p build
cd build
cmake ..
make -j4
```

## Troubleshooting

### Common Issues

**Issue**: "ModuleNotFoundError: No module named 'nlm'"
**Solution**: Ensure you're installing the correct package:
```bash
pip install -e .
```

**Issue**: "ImportError: cannot import name 'X' from 'nlm'"
**Solution**: Check that you're using the right import path. All core classes are available from the main `nlm` package.

**Issue**: Performance issues
**Solution**: 
- Check for memory leaks
- Use efficient data structures
- Consider batch processing for large operations

### Getting Help

- Check the [GitHub Issues](https://github.com/nlm-project/nlm/issues)
- Join the [NLM Discord](https://discord.gg/nlm) community
- Read the [Documentation](https://nlm.readthedocs.io)

## License

MIT License. See `LICENSE` file for details.
