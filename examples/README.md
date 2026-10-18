# NLM Python Bindings Examples

This directory contains example scripts demonstrating the enhanced features of the NLM Python bindings.

## Overview

The NLM (Neural Learning Machine) Python bindings have been significantly enhanced with the following features:

### 🚀 New Features

1. **Comprehensive Documentation** - Detailed docstrings for all classes and methods
2. **Convenience Factory Functions** - Quick setup functions for common use cases
3. **Preset Configurations** - Pre-configured settings for different simulation types
4. **Context Managers** - Automatic resource cleanup and state management
5. **Enhanced Error Handling** - Specific exception types for better error management
6. **Pythonic Interfaces** - Properties and improved method signatures
7. **Utility Functions** - Helper functions for common tasks
8. **Performance Optimizations** - Built-in optimization options

### 📝 Key Improvements

#### 1. Enhanced Documentation
- Comprehensive docstrings with parameter descriptions
- Return value documentation
- Usage examples in docstrings
- Type hints for better IDE support

#### 2. Convenience Factory Functions
- `createQuickSimulation()` - Quick setup with default parameters
- `createCompleteAgentSetup()` - Full agent-world setup
- `createWorldForAgent()` - World configured for agent use

#### 3. Preset Configurations
- `createBalancedConfig()` - General-purpose balanced configuration
- `createHighPerformanceConfig()` - Optimized for large simulations
- `createLearningFocusedConfig()` - Optimized for neural plasticity

#### 4. World Configurations
- `createStandardWorld()` - Standard size world (20x20)
- `createLargeWorld()` - Large world (50x50)
- `createSmallWorld()` - Small world (10x10)

#### 5. Context Managers
- Automatic cleanup of brain and agent instances
- State management for safe resource handling

#### 6. Enhanced Error Handling
- Custom exception types (`NLMError`, `ValidationError`, `ConfigurationError`)
- Better error messages and debugging information

#### 7. Utility Functions
- `runSimulationSteps()` - Run simulation with specified steps
- `runAgentEpisode()` - Run complete agent episode
- `optimizeForSpeed()` / `optimizeForAccuracy()` - Performance optimizations
- `createVisualizationConfig()` / `createDebugConfig()` - Utility configurations

### 🛠️ How to Use These Examples

#### Running Examples

```bash
# Run all examples
cd /path/to/nlm
python examples/main.py

# Run specific examples
python examples/main.py --example 1  # Basic usage
python examples/main.py --example 5  # Agent simulation
```

#### Installing Requirements

The examples require:
- pynlm package (already installed)
- Python 3.8 or higher

#### Example Usage Patterns

**Basic Usage:**
```python
import pynlm

# Create and simulate a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for step in range(100):
    brain.step(step)
```

**Advanced Agent Simulation:**
```python
import pynlm

# Use convenience factory
brain, world, agent = pynlm.createCompleteAgentSetup()

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
pynlm.runAgentEpisode(agent, world, 100)
```

**Context Manager Usage:**
```python
import pynlm

brain, world, agent = pynlm.createCompleteAgentSetup()

# Automatic cleanup with context manager
with brain as managed_brain:
    managed_brain.initialize()
    for step in range(50):
        managed_brain.step(step)

# Brain automatically reset after context
```

### 📂 Available Examples

The examples directory contains:

- `__init__.py` - Package initialization
- `main.py` - Comprehensive example suite
- `examples/enhanced_example.py` - Advanced feature demonstrations
- `examples/basic_example.py` - Simple usage examples
- `examples/performance_example.py` - Performance optimization examples

### 🎯 Example Categories

1. **Basic Examples** - Simple usage patterns
2. **Enhanced Examples** - Advanced features and Pythonic interfaces
3. **Performance Examples** - Optimization and benchmarking
4. **Integration Examples** - Complex agent-world simulations
5. **Error Handling Examples** - Exception management

### 📚 Documentation Links

- [Main Documentation](README.md) - Project overview and setup instructions
- [Easy Usage Guide](easy_usage.md) - Quick start for beginners
- [How to Use Guide](HOW_TO_USE.md) - Comprehensive API documentation

### 🔧 Quick Start

```bash
# Install the package
pip install .

# Run examples
python examples/main.py

# View specific examples
ls examples/*.py
```

### 📈 Usage Tips

1. **For Beginners**: Start with `examples/main.py` or the basic examples
2. **For Advanced Users**: Use the enhanced examples to explore new features
3. **For Performance**: Use preset configurations and optimization functions
4. **For Learning**: Use the context managers for safe resource management

### 🐛 Troubleshooting

If you encounter issues:

1. **Module Not Found**: Ensure pynlm is installed (`pip install pynlm`)
2. **Build Errors**: The package may need to be rebuilt from source
3. **Runtime Errors**: Check that the C++ core is properly built
4. **Documentation**: Refer to the main documentation files for setup instructions

### 🤝 Contributing

This examples directory is part of the NLM Python bindings enhancements. Contributions are welcome for:

- Additional example patterns
- New feature demonstrations
- Documentation improvements
- Performance optimizations

### 📝 License

This project is part of the NLM (Neural Learning Machine) project and is licensed under the MIT License.

---

*For more information about NLM, visit the [NLM documentation](README.md)*