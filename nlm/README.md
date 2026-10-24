# NLM Python Package

This is the main package for the NLM (Neural Learning Machine) Python bindings.

## Package Structure

```
nlm/
├── __init__.py              # Package initialization
├── core.py                  # Core classes (Brain, Config, AgentBrain, SimpleWorld)
├── sensory.py              # Sensory systems (Vision, Audio, InternalSignals)
├── motor.py                # Motor system (Action, MotorCommand)
├── agent.py                # Agent system (AgentBrain)
├── world.py                # World system (AgentBody, WorldObject)
├── types.py                # Type definitions and enumerations
├── wrapper.py              # High-level wrapper classes (NLMSimulation, etc.)
├── factory.py              # Factory functions
├── constants.py            # Constants and utilities
├── __main__.py             # Command-line interface
└── __pycache__/            # Python cache
```

## Key Features

### Core Classes
- **Brain**: Main neural simulation brain class
- **AgentBrain**: High-level interface connecting brain to world
- **SimpleWorld**: 2D simulation world for agent interaction
- **Config**: Configuration management

### Sensory Systems
- **Vision**: 2D visual input processing
- **Audio**: Sound signal processing
- **InternalSignals**: Internal neural signals

### Motor System
- **Action**: Motor action representation
- **MotorCommand**: Low-level motor commands

### High-Level Wrappers
- **NLMSimulation**: High-level simulation manager
- **NLMExperiment**: Experiment framework
- **NLMPlotting**: Visualization tools
- **NLMDeveloper**: Development tools

### Additional Features
- Complete error handling
- Command-line interface
- Factory functions
- Type definitions
- Constants and utilities

## Installation

Install via pip:

```bash
pip install -e .
```

Or install in development mode:

```bash
pip install -e /path/to/nlm
```

## Usage Examples

### Quick Start

```python
import nlm

# Create simulation with fluent API
simulation = nlm.NLMSimulation().quick_start(
    neurons=1000,
    width=800,
    height=600,
    timestep=0.001,
    seed=42
)

# Run simulation
results = simulation.run_simulation(1000, verbose=True)
```

### Brain Simulation

```python
import nlm

brain = nlm.Brain()
brain.initialize()

# Run simulation
for step in range(100):
    brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 20.0)
    brain.step(step, step * 0.001)

# Get results
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Agent Simulation

```python
import nlm

# Create agent in world
simulation = nlm.NLMSimulation().quick_start()
simulation.world.configure(800, 600, 40, 40)

# Add world objects
simulation.world.add_resource_object(100, 100, 1.0)
simulation.world.add_hazard_object(700, 500, -1.0)

# Place agent
simulation.world.setAgentStart(100, 100)

# Run simulation
results = simulation.run_simulation(2000)
```

## Command-Line Interface

Run the built-in command-line interface:

```bash
python -m nlm demo
python -m nlm agent --steps 1000 --seed 42
python -m nlm learning --experiences 5
python -m nlm brain-test
```

## Testing

Run the test suite:

```bash
pytest
```

## Development

### Running Tests

```bash
# Run all tests
pytest -v

# Run specific test module
pytest tests/test_core.py -v
```

### Running Examples

```bash
# Run demo
python -m nlm demo

# Run agent simulation
python -m nlm agent --steps 1000
```

## Documentation

See the following files for comprehensive documentation:

- `NLM.md` - Complete package documentation
- `INSTALLATION.md` - Installation guide
- Individual docstrings - API documentation

## License

MIT License
