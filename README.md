# Enhanced NLM Python Bindings

## Overview

This repository contains the enhanced Python bindings for the NLM (Neural Learning Machine) framework. The enhanced API provides comprehensive improvements over the original bindings, addressing all the requirements specified in the enhancement request.

## Key Improvements

### 1. Enhanced Configuration API
- **YAML/JSON file support**: Load from both JSON and YAML configuration files
- **Advanced parameter validation**: Validate configuration values and provide detailed error messages
- **Better configuration templates**: Pre-built templates for common use cases
- **Environment variable support**: Load configuration from environment variables

### 2. Advanced Brain Interface
- **More intuitive methods**: Simplified API for common operations
- **Better error handling and diagnostics**: Comprehensive error reporting and debugging tools
- **Visualization integration**: Real-time visualization of neural networks
- **Advanced debugging capabilities**: Step-by-step execution and neuron/synapse inspection

### 3. Enhanced Agent Interface
- **Better interaction with the world**: Improved sensory processing and motor control
- **Advanced sensory processing**: Feature extraction, novelty detection, and smoothing
- **More sophisticated action selection**: Reward shaping, exploration policies, and action value estimation
- **Reward shaping and learning enhancement**: Multiple learning algorithms (TD, Policy Gradient, Actor-Critic)

### 4. World and Environment Tools
- **Better world configuration**: Enhanced world settings and object management
- **Advanced world object management**: Dynamic object creation, modification, and removal
- **Simulation control and monitoring**: Real-time monitoring of simulation state
- **Environment scripting**: Support for custom environment configurations

### 5. Memory and Learning Tools
- **Better access to memory systems**: Enhanced interfaces to working, episodic, semantic, procedural, and associative memory
- **Advanced learning visualization**: Real-time visualization of learning progress
- **Pattern analysis tools**: Feature extraction and pattern recognition
- **Knowledge extraction utilities**: Tools for extracting and analyzing knowledge from memory

### 6. Performance and Monitoring Tools
- **Real-time performance monitoring**: CPU usage, memory usage, and simulation speed tracking
- **Memory usage tracking**: Detailed memory consumption analysis
- **Advanced statistics collection**: Comprehensive statistics on neural activity
- **Profiling tools**: Performance profiling for optimization

### 7. Educational and Debugging Tools
- **Step-by-step execution**: Detailed logging of simulation steps
- **Neuron and synapse visualization**: Visual representation of neural networks
- **Learning progress tracking**: Monitor and visualize learning curves
- **Comparative analysis tools**: Compare performance across different configurations

### 8. Advanced Scripting and Automation
- **Pythonic API for batch operations**: Easy creation and management of multiple configurations
- **Experiment scripting**: Framework for automated experiment execution
- **Automated testing frameworks**: Built-in testing and validation tools
- **Custom simulation builders**: API for creating custom simulation setups

## Files Included

### Core Bindings (`python/bindings.cpp`)
- Enhanced configuration classes with YAML/JSON support
- Advanced brain interface with visualization and diagnostics
- Enhanced agent interface with sophisticated sensory/motor processing
- Complete backward compatibility with original API

### Documentation (`docs/`, `*.md` files)
- `EnhancedPythonBindings.md`: Comprehensive API documentation
- `ENHANCED_BINDINGS_SETUP.md`: Complete installation and setup guide
- Various example files with usage patterns

### Example Scripts (`examples/`)
- `enhanced_bindings_example.py`: Complete usage examples for all features
- Demonstrates all enhancement areas with practical code

### Additional Resources
- `examples/enhanced_bindings_example.py`: Comprehensive example script
- `docs/EnhancedPythonBindings.md`: Complete API documentation
- `ENHANCED_BINDINGS_SETUP.md`: Installation and setup guide

## Installation

### Quick Start
```bash
# Clone the repository
git clone https://github.com/yourusername/nlm.git
cd nlm

# Install dependencies
pip install pybind11 numpy matplotlib scipy

# Configure and build
mkdir build
cd build
cmake .. -DNLM_PERFORMANCE_ENABLED=ON
make -j$(nproc)

# Install Python module
cd ../python
pip install -e .
```

### For Developers
The enhanced bindings are designed for development and research. They include:
- Comprehensive error handling and debugging tools
- Extensive documentation and examples
- Support for multiple configuration formats
- Performance monitoring and optimization tools

## Usage Examples

### Basic Usage
```python
import pynlm

# Create enhanced configuration
config = pynlm.EnhancedConfig()
config.loadFromFile("config.yaml")

# Create enhanced brain
brain = pynlm.createEnhancedBrain(config)

# Initialize with diagnostics
if brain.initializeWithDiagnostics():
    print("Brain initialized successfully")
    
    # Run simulation
    brain.stepAdvanced(timestep=0.01)
    
    # Get debug information
    debug_info = brain.getDebugInfo()
    print(f"Active neurons: {debug_info.activeNeurons}")
```

### Advanced Usage
```python
import pynlm

# Create agent with world
world = pynlm.createSimpleWorld()
world.configure(width=200.0, height=150.0)

agent = pynlm.createEnhancedAgentBrain(brain)
agent.initializeWithWorld(world)

# Process sensory input
analysis = agent.analyzeSensoryInput(percept)
print(f"Novelty score: {analysis.noveltyScore:.3f}")

# Apply advanced reward modulation
agent.applyRewardModulationEnhanced(
    reward=1.0,
    predictedReward=0.8,
    useTDLearning=True,
    usePolicyGradient=True,
    useActorCritic=True
)
```

## Backward Compatibility

The enhanced bindings maintain full backward compatibility with the original API:

```python
import pynlm

# Original classes still work
config = pynlm.Config()
brain = pynlm.Brain(config)
world = pynlm.SimpleWorld()
agent = pynlm.AgentBrain(brain)

# Original methods still work
brain.initialize()
brain.step(0)
agent.initialize(world)
agent.processSensoryInput(percept)
```

## Testing

### Running Examples
```bash
# Run comprehensive examples
python examples/enhanced_bindings_example.py
```

### API Reference
For detailed API documentation, see:
- `docs/EnhancedPythonBindings.md` for complete method documentation
- `examples/enhanced_bindings_example.py` for practical usage examples

## Performance Considerations

The enhanced bindings include performance monitoring and optimization features:

1. **Performance Monitoring**: Enable with `brain.getPerformanceMetrics()`
2. **Configuration Validation**: Use `config.validate()` to ensure optimal settings
3. **Memory Management**: Monitor memory usage in long-running simulations
4. **Batch Operations**: Use for multiple experiment configurations

## Future Enhancements

The enhanced bindings will continue to evolve with:

1. **Real-time monitoring**: Web-based dashboards
2. **Cloud integration**: Distributed simulation support
3. **Advanced visualization**: 3D rendering and network analysis
4. **Machine learning integration**: Neural architecture search
5. **Export capabilities**: Export to various data formats

## Support

For support with the enhanced Python bindings:

1. **Documentation**: Check the `docs/` directory
2. **Examples**: Run example scripts in the `examples/` directory
3. **Issue tracking**: Report issues on GitHub
4. **Community**: Join the NLM community forum

## License

The enhanced Python bindings are distributed under the same license as the NLM framework.

## Authors

This enhanced Python API was developed as part of the NLM framework enhancement project.

---

*For more information, visit the NLM project documentation.*
