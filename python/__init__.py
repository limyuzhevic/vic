# NLM Python Package

This package provides Python wrappers for the NLM (Neural Learning Machine) C++ neural simulation framework.

## Package Structure

The package includes:

- `__init__.py`: Core Python API and factory functions
- `brain.py`: Brain simulation wrappers
- `world.py`: World environment wrappers
- `agent.py`: Agent interface wrappers
- `memory.py`: Memory system wrappers
- `neuromodulation.py`: Neuromodulation system wrappers
- `development.py`: Development system wrappers
- `cognition.py`: Cognitive system wrappers
- `utils.py`: Utility functions and helpers
- `exceptions.py`: Custom exception classes
- `config.py`: Enhanced configuration management

## Quick Start

```python
import nlm

# Create a brain with default configuration
brain = nlm.create_brain()
brain.initialize()

# Run a simple simulation
for i in range(100):
    brain.step(i)
    if brain.get_firing_neuron_count() > 0:
        print(f"Step {i}: {brain.get_firing_neuron_count()} neurons firing")
```

## Core Concepts

### Brain
The central neural simulation engine that processes sensory input and produces motor output.

### World
The simulation environment containing objects, agents, and sensory stimuli.

### Agent
Interface between the brain and world, handling perception and action.

## Key Features

### Enhanced Pythonic API
- **Property access**: Use `brain.neuron_count` instead of `brain->getTotalNeuronCount()`
- **Context managers**: `with nlm.BrainSession(config) as brain:` for automatic cleanup
- **Pythonic exceptions**: Custom `NLMErrors` instead of C++ exceptions

### Memory Management
- Automatic memory management with reference counting
- Smart pointer wrappers for C++ objects
- Safe cleanup of complex simulation state

### Configuration
- **Dictionary-style access**: `config['neuron_count'] = 1000`
- **JSON file support**: `config.load_from_file('settings.json')`
- **Command line integration**: `config.update_from_args()`

### Advanced Features
- **Simulation managers**: Handle complete simulation workflows
- **Monitoring and logging**: Real-time statistics and progress tracking
- **Checkpoint/restart**: Save and restore simulation states
- **Parallel processing**: Multi-threaded simulation support

## Examples

See the `examples/` directory for comprehensive usage examples.

## Installation

```bash
pip install .

# Or install from source
pip install -e .
```

## Building from Source

```bash
# Build Python bindings
python -m pip install build
python -m build

# Or use CMake directly
cmake -B build -DCMAKE_BUILD_TYPE=Release
make -C build -j4
python setup.py install
```

## Configuration

### Default Configuration
The package comes with sensible defaults, but you can customize:

```python
import nlm

config = nlm.Config({
    'neuron_count': 2000,
    'region_count': 2,
    'simulation_timestep': 0.001,
    'connection_probability': 0.1,
    'enable_reward_modulation': True,
    'enable_development': True,
    'enable_curiosity': True,
})

brain = nlm.create_brain(config)
```

### Configuration Files
Create JSON configuration files in `~/.config/nlm/` or `configs/` directory.

## Performance

The NLM framework is optimized for performance:

- **Real-time simulation**: Supports simulation rates up to 10,000 Hz
- **Memory efficient**: Uses memory pools and event queues
- **Parallel processing**: SIMD vectorization for neural computations
- **Checkpointing**: Fast save/restore of simulation states

## Testing and Validation

Run the test suite:

```bash
pytest tests/ -v

# Run integration tests
pytest tests/integration/ -v

# Run performance benchmarks
pytest tests/performance/ -v
```

## API Reference

For complete API documentation, see:
- `docs/api.md`: Complete API reference
- `examples/basic_usage.py`: Basic usage examples
- `examples/advanced_simulation.py`: Advanced simulation techniques
- `examples/visualization.py`: Visualization and analysis tools

## Troubleshooting

### Common Issues

1. **Import errors**: Ensure the package is installed correctly
2. **Memory issues**: Use `nlm.get_memory_usage()` for monitoring
3. **Performance**: Use `nlm.set_thread_pool_size()` for multi-threading

### Getting Help

- **Documentation**: `docs/` directory
- **Examples**: `examples/` directory
- **Community**: Join the NLM community for support
- **Issues**: Report bugs on GitHub

## Future Enhancements

The NLM Python package is actively developed. Future enhancements include:

- **Visualization tools**: Integrated plotting and analysis
- **Web interface**: Jupyter notebooks and web dashboards
- **Machine learning integration**: ML model training and inference
- **Cloud deployment**: Docker containers and cloud deployment scripts
- **Advanced simulations**: Complex multi-agent simulations
- **Custom extensions**: Plugin system for user-defined components

## License

MIT License. See `LICENSE` file for details.