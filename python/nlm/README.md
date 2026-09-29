# NLM Python Package

This package provides Python bindings for the NLM (Neural Learning Machine) 
computational brain simulator.

## Package Structure

```
python/
├── nlm/
│   ├── __init__.py              # Main package initialization
│   ├── __main__.py              # CLI entry point
│   ├── cli.py                   # Command-line interface
│   ├── benchmark.py             # Performance benchmarks
│   ├── examples/                # Usage examples
│   ├── tests/                   # Unit tests
│   └── utils.py                 # Utility functions
├── requirements.txt             # Dependencies
├── pyproject.toml              # Modern Python packaging config
└── README.md                   # Package documentation
```

## Quick Start

```python
import nlm

# Create a default configuration
config = nlm.createDefaultConfig()

# Create a brain
brain = nlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

## Core Classes

### Config
Configuration management for NLM parameters.

### Brain
Main neural simulation engine implementing spiking neural networks.

### SimpleWorld
2D environment for agent simulation.

### AgentBrain
Interface connecting brain to world interactions.

## Development

### Install
```bash
pip install -e .
```

### Run Tests
```bash
pytest python/nlm/tests/
```

### Build Documentation
```bash
sphinx-build -b html docs/ docs/_build/
```

## API Reference

See the individual module documentation for detailed API references.

## License

MIT License - See LICENSE file for details.