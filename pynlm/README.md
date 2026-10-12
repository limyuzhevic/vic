# Pynlm Python package - Modern Python Module Structure

This directory contains the Python module for NLM (Neural Learning Machine).

## Module Structure

```
pynlm/
├── __init__.py              # Main package interface
├── brain.py                 # Brain simulation classes
├── agent.py                 # Agent brain interface
├── world.py                 # World simulation
├── sensory.py               # Sensory input systems
├── motor.py                 # Motor and action systems
├── config.py                # Configuration management
├── types.py                 # Type definitions and enumerations
├── exceptions.py            # Custom exceptions
├── utils.py                 # Utility functions
└── examples/                # Example scripts
    ├── basic.py             # Basic usage examples
    ├── agent_simulation.py  # Complete agent example
    ├── advanced_features.py # Advanced feature demonstrations
    └── performance_tests.py # Performance benchmarking
```

## Package Features

### 1. Clean API Design
- Organized into logical modules for easy navigation
- Type hints and docstrings throughout
- Convenient factory functions for common use cases
- Error handling with clear messages

### 2. Enhanced Functionality
- Performance monitoring utilities
- Progress tracking for long simulations
- Validation utilities
- Quick reference guides
- Example patterns

### 3. Development Tools
- Comprehensive error handling
- Simulation progress reporting
- Configuration validation
- State management utilities

## Installation

### Development Installation
```bash
pip install -e .
```

### Regular Installation
```bash
pip install pynlm
```

### Build from Source
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
cd ..
pip install .
```

## Usage Examples

### Quick Start
```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete: {brain.getFiringNeuronCount()} neurons firing")
```

### Complete Agent Example
```python
import pynlm

def run_agent_simulation(world_size=20, num_steps=500):
    """Run a complete agent simulation with learning and development."""
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=world_size, height=world_size, 
                   visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    # Run simulation with progress tracking
    pynlm.run_with_progress(brain, world, agent, num_steps)
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_agent_simulation()
```

## Module References

### Core Modules
- `pynlm.brain`: Brain simulation and neural processing
- `pynlm.agent`: Agent-brain interface and processing
- `pynlm.world`: World simulation and environment
- `pynlm.sensory`: Sensory input systems (vision, audio, internal)
- `pynlm.motor`: Motor commands and action systems
- `pynlm.config`: Configuration management
- `pynlm.types`: Type definitions and enumerations

### Utilities
- `pynlm.exceptions`: Custom exception classes
- `pynlm.utils`: Helper functions and utilities

## Advanced Features

### 1. Progress Tracking
The `pynlm.run_with_progress()` function provides real-time progress reporting
and statistics collection for long-running simulations.

### 2. Configuration Validation
The `validate_simulation_setup()` function checks that all required components
are properly configured before running simulations.

### 3. Error Handling
Comprehensive error handling with descriptive messages and suggestions for
common issues.

### 4. State Management
Utilities for saving and loading brain states, with periodic checkpointing
options.

## Testing and Development

### Running Tests
```bash
# Install development dependencies
pip install -e .[dev]

# Run unit tests
pytest

# Run tests with coverage
pytest --cov=pynlm --cov-report=html
```

### Example Scripts
The `examples/` directory contains various demonstration scripts covering
basic usage, advanced features, and performance testing.

## Performance Considerations

### Large Simulations
For large-scale simulations, consider:
- Using appropriate configuration parameters
- Monitoring memory usage with progress tracking
- Saving checkpoints periodically to avoid data loss

### Development and Debugging
- Use `brain.logStatus()` for debugging output
- Monitor development stages with `brain.getDevelopmentalStage()`
- Track learning metrics with agent's neuromodulation functions

## Future Enhancements

Planned improvements include:
- Plugin architecture for custom neural models
- Advanced visualization tools
- Real-time monitoring interfaces
- Distributed computing support
- Additional sensory modalities

## License

MIT License. See LICENSE file for details.
