# NLM Python Bindings - Quick Setup and Usage Guide

This guide provides a quick overview of the enhanced NLM Python bindings and how to get started using them.

## What Are the Enhanced NLM Python Bindings?

The NLM Python bindings are a Python interface to the NLM (Neural Learning Machine) neural simulation framework. Version 2.0 includes significant improvements:

- ✅ Better organization and modularity
- ✅ Comprehensive utility functions for configuration, simulation, and debugging
- ✅ Detailed examples and documentation
- ✅ Automated testing suite
- ✅ Performance monitoring tools
- ✅ Backward compatibility with all existing functionality

## Quick Start

### 1. Check if NLM is Installed

```python
import pynlm
print("✓ NLM Python bindings imported successfully")
```

If you see this message, you're good to go! If not, you may need to build the NLM C++ bindings.

### 2. Run a Quick Test

```bash
python python/quick_test.py
```

This will run a quick test of the basic NLM functionality.

### 3. Import Setup Utilities

```python
import nlm_setup as setup
```

The setup utilities provide additional tools for working with NLM.

## Basic Usage

### Create a Simple Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create a brain with the configuration
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run 10 simulation steps
for step in range(10):
    brain.step(step)

print("✓ Simple brain simulation completed!")
```

### Get Brain Statistics

```python
# Get comprehensive brain statistics
stats = pynlm.getBrainStatistics(brain)

# Print formatted results
print(pynlm.formatSimulationResults(stats))
```

### Use Setup Utilities

```python
# Check environment status
status = setup.check_environment()
print(f"NLM Available: {status['pynlm_available']}")

# Run a test simulation
results = setup.run_test_simulation(20)
print(f"Simulation completed with {results['steps_executed']} steps")

# Get performance benchmark
bench = setup.benchmark_brain_creation(5)
print(f"Performance: {bench['brains_per_second']:.2f} brains/second")
```

## Enhanced Features

### 1. Configuration Utilities

```python
# Create a configuration dictionary
config_dict = pynlm.createDefaultConfigDict()

# Display configuration
print(pynlm.configToString(config_dict))
```

### 2. Brain Inspection

```python
# Get brain state as a string
brain_state = pynlm.getBrainStateString(brain)
print(brain_state)

# Print brain state to console
pynlm.printBrainState(brain)

# Get available actions
actions = pynlm.getAvailableActions(brain)
print(f"Available actions: {len(actions)}")
```

### 3. Advanced Examples

Run the comprehensive examples:

```bash
python python/enhanced_examples.py
```

Or run specific examples:

```python
import python.enhanced_examples as examples

# Run basic usage example
examples.example_basic_usage()

# Run agent-world integration example
examples.example_agent_world_integration()

# Run advanced learning example
examples.example_advanced_learning()
```

### 4. Testing

Run the comprehensive test suite:

```bash
python python/test_bindings.py
```

## Advanced Usage

### Agent-World Integration

```python
import pynlm

# Create complete agent system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(10, 10)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Learning Features

```python
# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Apply reward
agent.applyRewardModulation(reward=10.0, predicted_reward=5.0)

# Update development
agent.updateDevelopment(0.1)
```

### Text Processing

```python
# Encode text
text_pattern = agent.encodeText("hello world")
print(f"Text encoded as: {text_pattern}")

# Get internal firing rates
rates = agent.getInternalFiringRates()
print(f"Internal firing rates: {rates[:5]}...")
```

## Examples Directory

The `python/examples/` directory contains additional example scripts:

```
python/examples/
├── basic_usage.py              # Basic simulation patterns
├── agent_world_demo.py          # Complete agent-environment integration
├── brain_inspection.py          # Brain state analysis and debugging
├── learning_demo.py             # Advanced learning with neuromodulation
├── performance_monitoring.py   # Simulation analysis tools
├── batch_processing.py          # Parameter sweeps and multiple simulations
└── memory_management.py         # Checkpointing and serialization
```

## File Structure

```
python/
├── bindings_improved.cpp        # Enhanced Python bindings
├── nlm_setup.py                # Setup and utility functions
├── enhanced_examples.py         # Comprehensive examples
├── test_bindings.py             # Test suite
├── quick_test.py                # Quick validation script
├── README.md                    # Documentation
├── examples/                    # Example scripts
├── ...                          # Other supporting files
```

## Installation

### If NLM is Already Built

```bash
pip install -e .
```

### If You Need to Rebuild

```bash
mkdir build
cd build
cmake -DNLM_BUILD_PYTHON_BINDINGS=ON ..
make
pip install -e .
```

## Troubleshooting

### "ImportError: No module named 'pynlm'"

**Solution:** Build the NLM C++ bindings:

```bash
mkdir build
cd build
cmake -DNLM_BUILD_PYTHON_BINDINGS=ON ..
make
```

Then install:

```bash
pip install -e .
```

### "ModuleNotFoundError: No module named 'nlm_setup'"

**Solution:** The setup utilities are in the same directory as the bindings. Make sure you're running the script from the correct location:

```bash
cd python
python quick_test.py
```

### "AttributeError: module 'pynlm' has no attribute 'createDefaultConfigDict'"

**Solution:** Make sure you're using the enhanced bindings. If the error persists, try rebuilding:

```bash
pip install -e . --force-reinstall
```

## Common Tasks

### 1. Quick Test
```bash
python python/quick_test.py
```

### 2. Basic Usage
```python
import pynlm

# Simple simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(10):
    brain.step(i)

print("✓ Simulation completed!")
```

### 3. Get Statistics
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(50):
    brain.step(i)

stats = pynlm.getBrainStatistics(brain)
print(pynlm.formatSimulationResults(stats))
```

### 4. Debug Brain
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for i in range(30):
    brain.step(i)

print(pynlm.getBrainStateString(brain))
```

### 5. Run Examples
```bash
python python/enhanced_examples.py
```

## Performance Tips

### 1. Reuse Objects

Reuse Brain and AgentBrain objects instead of creating new ones each time:

```python
# Better: Reuse objects
brain = pynlm.createBrain(config)
brain.initialize()
for i in range(100):
    brain.step(i)

# Avoid: Creating new objects each time
for episode in range(10):
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)  # Expensive!
    brain.initialize()
    for i in range(10):
        brain.step(i)
```

### 2. Use Utilities

Use the setup utilities for common operations:

```python
import nlm_setup as setup

# Use benchmarking
results = setup.benchmark_brain_creation(10)

# Use validation
issues = setup.validate_brain_state(brain)
```

### 3. Batch Processing

Use batch processing for multiple simulations:

```python
import nlm_setup as setup

# Run multiple simulations
simulations = []
for neurons in [50, 100, 200]:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    for step in range(50):
        brain.step(step)
    
    simulations.append(brain)
```

## Known Issues

1. **Build Required**: The enhanced bindings require the NLM C++ libraries to be built with Python bindings enabled.

2. **Python Version**: Requires Python 3.8 or higher.

3. **Dependencies**: No new dependencies beyond what was already required.

## Future Improvements

The enhanced bindings provide a foundation for future improvements:

1. **Additional Utilities**: More configuration and analysis tools
2. **Advanced Examples**: Complex simulation scenarios
3. **Documentation**: More detailed API documentation
4. **Testing**: More comprehensive test coverage
5. **Performance**: Optimization for large-scale simulations

## Conclusion

The enhanced NLM Python bindings provide:

- **Better organization** with modular, well-organized code
- **Comprehensive utilities** for configuration, simulation, and debugging
- **Detailed examples** for learning and reference
- **Automated testing** for validation
- **Performance monitoring** tools
- **Backward compatibility** with existing code

Get started today with these powerful tools for neural simulation!

For more information:
- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Documentation](https://nlm.readthedocs.io)
- [Issue Tracker](https://github.com/nlm-project/nlm/issues)

---

**Version 2.0 (Enhanced)**
*Improved Python bindings for NLM neural simulation*