# NLM (Neural Learning Machine) - Improved Python API

**Enhanced Python bindings for NLM with comprehensive error handling, validation, and improved usability**

This is the improved version of the NLM Python API (v2.0.0-improved) featuring:

- ✅ Comprehensive error handling and validation
- ✅ Consistent API design and parameter validation
- ✅ Python-friendly exception classes
- ✅ Improved documentation and type hints
- ✅ Better performance and stability
- ✅ Enhanced examples and tutorials

## Quick Start

```python
import pynlm

# Create and initialize brain with validation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Run simulation with error handling
try:
    for step in range(100):
        brain.step(step)
        
    print(f"Simulation completed successfully")
except pynlm.NLMError as e:
    print(f"Simulation failed: {e}")
```

## Installation

### From Source (Recommended)

```bash
# Install build dependencies
pip install scikit-build-core pybind11 numpy matplotlib

# Build and install the improved bindings
pip install .

# Or install in development mode for editing
pip install -e .
```

### Using pip

```bash
# Install stable version
pip install pynlm

# Install latest development version
pip install git+https://github.com/nlm-project/nlm.git
```

## API Overview

### Core Classes

#### `Config`
Configuration management with comprehensive validation:

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Set parameters with validation
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.connection_probability", 0.05)

# Validate configuration
if config.has("brain.neuron_count"):
    print(f"Neuron count: {config.get('brain.neuron_count')}")

# Save configuration
config.saveToFile("my_config.cfg")
```

#### `Brain`
Main neural simulation brain with validation:

```python
import pynlm

# Create brain with validation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain created: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")

# Safe simulation with error handling
try:
    for step in range(100):
        brain.step(step)
        if step % 10 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
except pynlm.NLMError as e:
    print(f"Simulation error: {e}")
```

#### `SimpleWorld`
2D simulation environment with validation:

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(10.0, 10.0)

print(f"World configured: {world.getWidth()}x{world.getHeight()} pixels")
print(f"Agent starting position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
```

#### `AgentBrain`
Interface between brain and world with comprehensive validation:

```python
import pynlm

# Create complete agent system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Initialize with validation
agent.initialize(world)

# Enable subsystems with validation
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print(f"Agent configured: {agent.getNeuromodulationLevel():.2f} neuromodulation level")
```

### Advanced Usage

#### Complete Agent Simulation

```python
import pynlm
import time

def run_agent_simulation(steps=500):
    """Run a complete agent simulation with error handling."""
    print("Starting agent simulation...")
    
    try:
        # 1. Create and configure components
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.1)
        
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # 2. Initialize world and agent
        world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        world.setAgentStart(15.0, 15.0)
        agent.initialize(world)
        
        # 3. Enable subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # 4. Run simulation
        start_time = time.time()
        for step in range(steps):
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Simulate brain
            brain.step(step)
            
            # Get motor command
            cmd = agent.decodeMotorCommand()
            
            # Apply to world
            result = world.applyMotorCommand(cmd, world.getSimulationTime())
            agent.applyRewardModulation(result.reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Print progress
            if step % 100 == 0:
                print(f"Step {step}:")
                print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
                print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
                print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
                print(f"  Reward: {result.reward:.3f}")
        
        end_time = time.time()
        
        # 5. Print results
        print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
        print(f"Final brain state: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
        print(f"Final agent state: curiosity={agent.getCuriosityLevel():.3f}, novelty={agent.getNoveltyLevel():.3f}")
        
        return brain, world, agent
        
    except pynlm.NLMError as e:
        print(f"Simulation failed: {e}")
        raise
    except Exception as e:
        print(f"Unexpected error: {e}")
        raise

# Run the simulation
brain, world, agent = run_agent_simulation(500)
```

#### Configuration Examples

```python
import pynlm

# Example 1: Small brain for quick testing
config1 = pynlm.createDefaultConfig()
config1.set("brain.neuron_count", 100)
config1.set("brain.synapse_density", 0.2)

# Example 2: Large brain for detailed simulation
config2 = pynlm.createDefaultConfig()
config2.set("brain.neuron_count", 5000)
config2.set("brain.synapse_density", 0.05)
config2.set("brain.synapse_weight_mean", 0.3)
config2.set("brain.synapse_weight_std", 0.1)

# Example 3: Custom plasticity settings
config3 = pynlm.createDefaultConfig()
config3.set("plasticity.stdp.enable", True)
config3.set("plasticity.stdp.learning_rate", 0.001)
config3.set("plasticity.hebbian.enable", True)
config3.set("plasticity.structural.enable", True)
```

## Error Handling

The improved API provides comprehensive error handling with specific exception classes:

```python
import pynlm

config = pynlm.createDefaultConfig()

try:
    # This will raise ConfigError
    config.set("invalid.parameter", "value")
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # This will raise BrainError
    for step in range(-10, 10):  # Invalid step range
        brain.step(step)
except pynlm.BrainError as e:
    print(f"Brain error: {e}")
except pynlm.NLMError as e:
    print(f"General NLM error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Exception Hierarchy

- `NLMError` - Base exception for all NLM errors
- `ConfigError` - Configuration-related errors
- `BrainError` - Brain-related errors
- `NeuronError` - Neuron-related errors
- `MemoryError` - Memory-related errors
- `WorldError` - World-related errors
- `AgentError` - Agent-related errors

## Examples and Demos

### Built-in Demos

```bash
# Run the basic demo
pynlm-demo

# Run specific examples
python -m pynlm.examples.basic
python -m pynlm.examples.advanced
python -m pynlm.examples.error_handling
```

### Example Scripts

The package includes several example scripts:

1. **Basic Example** (`examples/basic.py`)
   - Simple brain creation and simulation
   - Basic parameter testing

2. **Advanced Example** (`examples/advanced.py`)
   - Complete agent simulation
   - World interaction and learning
   - Neuromodulation and development

3. **Error Handling Example** (`examples/error_handling.py`)
   - Demonstration of exception handling
   - Parameter validation examples
   - Error recovery techniques

4. **Configuration Example** (`examples/configuration.py`)
   - Configuration management
   - File I/O examples
   - Parameter optimization

## Development and Contributing

### Project Structure

```
pynlm/
├── __init__.py                    # Package initialization
├── __main__.py                    # Command-line interface
├── core/                          # Core functionality
│   ├── __init__.py
│   ├── Config.py                  # Configuration management
│   ├── Brain.py                   # Brain simulation
│   ├── World.py                   # World simulation
│   └── ...                        # Other core modules
├── examples/                      # Example scripts
│   ├── __init__.py
│   ├── basic.py                   # Basic usage examples
│   ├── advanced.py                # Advanced examples
│   └── ...                        # More examples
├── tests/                         # Test suite
│   ├── __init__.py
│   ├── test_config.py             # Configuration tests
│   ├── test_brain.py              # Brain tests
│   └── ...                        # More tests
└── docs/                          # Documentation
    ├── README.md                  # This file
    ├── API_REFERENCE.md           # API documentation
    └── EXAMPLES.md                 # Examples documentation
```

### Testing

Run the test suite:

```bash
# Run all tests
pytest

# Run specific test modules
pytest tests/test_config.py
pytest tests/test_brain.py
pytest tests/test_agent.py

# Run tests with verbose output
pytest -v

# Run tests with coverage
pytest --cov=pynlm --cov-report=html
```

### Building from Source

```bash
# Clone the repository
cd /path/to/nlm
cd pynlm  # if this is a submodule

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j4

# Install
make install
```

## API Reference

For comprehensive API documentation, see:

1. **API Reference**: `docs/API_REFERENCE.md`
2. **Complete Examples**: `docs/EXAMPLES.md`
3. **Configuration Guide**: `docs/CONFIGURATION.md`
4. **Error Handling Guide**: `docs/ERROR_HANDLING.md`

## Performance Considerations

### Memory Usage

- Brain memory scales with neuron count: ~100KB per 1000 neurons
- Synapse memory scales with connectivity: ~10KB per 1000 synapses
- Configuration memory is minimal (~1KB)

### Simulation Performance

- Single timestep complexity: O(neuron_count + synapse_count)
- Memory access is optimized for cache locality
- SIMD vectorization available for numeric operations

### Scalability

- Tested up to 100,000 neurons
- Can be scaled by increasing available memory
- Parallel processing support for multi-core systems

## Known Limitations

### Python Binding Performance

- Python-C++ interface overhead for small simulations
- Object creation/destruction overhead
- Python GIL may limit parallel processing

### Configuration Validation

- Validation occurs at runtime, not at compile time
- Some validation only occurs during simulation

### Simulation Stability

- Very large networks may exhibit numerical instability
- Extreme parameter values may cause simulation issues
- Memory limits may be reached for very large simulations

## Future Enhancements

### Planned Features

1. **Enhanced Visualization**
   - Integration with matplotlib for brain state visualization
   - Real-time simulation monitoring
   - Neural activity plotting

2. **Advanced Configuration**
   - YAML/JSON configuration support
   - Configuration templates
   - Parameter optimization tools

3. **Parallel Processing**
   - Multi-threading support
   - GPU acceleration for numeric operations
   - Distributed simulation support

4. **Additional Features**
   - Reinforcement learning integration
   - Cognitive architecture extensions
   - Multi-agent system support

## License

MIT License - See LICENSE file for details.

## Copyright

Copyright © 2024 NLM Project Contributors

## Acknowledgements

The NLM project builds upon decades of research in:

- Spiking neural networks
- Computational neuroscience
- Artificial developmental systems
- Neuromodulation and learning

Special thanks to the open-source community for their contributions to the underlying C++ framework.

## Support

For issues and support, please visit:

- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Documentation: https://nlm.readthedocs.io
- Discord: https://discord.gg/nlm-community

## Getting Help

### Common Issues and Solutions

1. **ImportError: No module named 'pynlm'**
   ```bash
   pip install pynlm
   ```

2. **RuntimeError: Could not load pynlm**
   ```bash
   # Ensure you have the correct Python environment
   pip list | grep pynlm
   ```

3. **Segmentation Fault**
   - Check for invalid parameters in configuration
   - Verify that initialization was called before simulation
   - Use smaller simulation parameters for debugging

4. **Memory Issues**
   - Monitor memory usage during large simulations
   - Use efficient configuration to reduce memory footprint
   - Consider using smaller test configurations for development

### Getting Started

If you're new to NLM, try this getting started guide:

```python
# Minimal working example
import pynlm

# Create simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run short simulation
for step in range(10):
    brain.step(step)

print("Success! NLM simulation completed.")
```

### Advanced Topics

For advanced usage, refer to:

- Neural network architecture documentation
- Configuration management guide
- Performance optimization techniques
- Error handling best practices

## Conclusion

The improved NLM Python API provides a robust, user-friendly interface to the NLM C++ neural simulation framework. With comprehensive error handling, validation, and extensive examples, it enables researchers and developers to quickly prototype and deploy neural simulations.

The improvements focus on:

1. **Safety**: Comprehensive validation prevents crashes and undefined behavior
2. **Usability**: Python-friendly API with clear documentation and examples
3. **Performance**: Optimized bindings with minimal overhead
4. **Maintainability**: Clean code structure with consistent patterns
5. **Extensibility**: Modular design for future enhancements

This API makes it easier than ever to experiment with neural architectures, learn about brain-inspired computation, and build applications that leverage the power of artificial developmental systems.

---

*For the original documentation and older versions, see the main NLM documentation.*
