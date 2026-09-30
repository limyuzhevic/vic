# NLM Enhanced Python Bindings - Examples Directory

This directory contains comprehensive examples for all enhanced NLM Python binding features, organized by category.

## Overview

The examples demonstrate the full capabilities of the enhanced NLM Python bindings, from basic usage to advanced features. They serve as both documentation and testing for the enhanced bindings.

## Directory Structure

```
examples/
├── basic_usage/                    # Basic usage examples (matching easy_usage.md)
│   ├── example1_simplest_brain.py
│   ├── example2_silent_brain.py
│   ├── example3_brain_in_world.py
│   ├── example4_enhanced_brain.py
│   ├── example5_enhanced_config.py
│   └── example6_method_chaining.py
│   └── README.md
├── advanced_usage/                  # Advanced usage examples
│   ├── example1_context_managers.py
│   ├── example2_simulation_runner.py
│   ├── example3_experiment_runner.py
│   ├── example4_data_collector.py
│   ├── example5_pythonic_interfaces.py
│   └── example6_advanced_features.py
│   └── README.md
├── error_handling/                  # Error handling examples
│   ├── example1_enhanced_exceptions.py
│   ├── example2_type_validation.py
│   ├── example3_parameter_validation.py
│   ├── example4_state_validation.py
│   ├── example5_error_recovery.py
│   └── example6_error_logging.py
│   └── README.md
├── performance/                      # Performance optimization examples
│   ├── example1_vectorization.py
│   ├── example2_memory_optimization.py
│   ├── example3_parallel_processing.py
│   ├── example4_benchmarking.py
│   ├── example5_optimization_tips.py
│   └── example6_profiling.py
│   └── README.md
├── integration/                      # Python ecosystem integration examples
│   ├── example1_numpy_integration.py
│   ├── example2_pandas_integration.py
│   ├── example3_matplotlib_integration.py
│   ├── example4_sklearn_integration.py
│   ├── example5_complete_integration.py
│   └── example6_custom_integrations.py
│   └── README.md
├── phase3/                          # Phase 3 simulation examples
│   └── *.py
├── phase4/                          # Phase 4 simulation examples
│   └── *.py
└── README.md                        # Main examples overview
```

## How to Use Examples

### Running Examples

```bash
# Navigate to an example directory
cd examples/basic_usage

# Run an example
python example1_simplest_brain.py

# Run all examples in the directory
for example in *.py; do
    python "$example"
done
```

### Viewing Documentation

```bash
# View the example documentation
cat README.md

# View comprehensive documentation
less ../COMPREHENSIVE_DOCUMENTATION.md
```

### Running Tests

```bash
# Run all examples as tests
pytest examples/

# Run specific example category
pytest examples/basic_usage/
pytest examples/advanced_usage/
pytest examples/error_handling/
pytest examples/performance/
pytest examples/integration/
```

## Example Categories

### Basic Usage Examples
The `basic_usage/` directory contains examples that demonstrate the basic NLM functionality while introducing enhanced features:

- **Simplest possible example** - Minimal brain creation (matches original `easy_usage.md`)
- **Silent brain test** - Testing brain functionality with context managers
- **Brain in a simple world** - Complete simulation with enhanced utilities
- **EnhancedBrain with statistics** - Pythonic interface enhancements
- **EnhancedConfig with dictionary-like access** - Configuration utilities
- **Method chaining with EnhancedBrain** - Method chaining support

**Key Features Demonstrated:**
- Backward compatibility with original NLM API
- Enhanced context managers (`simulation_step()`)
- Utility functions (`run_simulation()`, `DataCollector`)
- Pythonic interfaces (`EnhancedBrain`, `EnhancedConfig`)
- Type-safe configuration methods
- Method chaining support

### Advanced Usage Examples
The `advanced_usage/` directory contains examples that demonstrate advanced NLM functionality using all enhanced features:

- **Context Manager Patterns** - All three context managers with nested usage
- **Utility Classes** - SimulationRunner for experiment management
- **ExperimentRunner** - Parameter sweeps and structured experiments
- **DataCollector and Analysis** - Comprehensive data collection and post-analysis
- **Pythonic Interfaces** - EnhancedBrain and EnhancedConfig with all features
- **Advanced Python Features** - Callbacks, observers, decorators, statistical utilities

**Key Features Demonstrated:**
- Resource management through context managers
- Experiment automation through utility classes
- Data collection with callback support
- Statistical analysis and pattern detection
- Pythonic interfaces and method chaining
- Event-driven programming and callback systems
- Decorator and async support

### Error Handling Examples
The `error_handling/` directory contains examples that demonstrate enhanced error handling and validation:

- **Enhanced Exception Types** - Custom exception classes with detailed recovery information
- **Type Validation** - Strict type checking with helpful error messages
- **Parameter Validation** - Range checking, null pointer detection, bounds checking
- **State Validation** - State checking and error recovery
- **Error Recovery Patterns** - Graceful error handling and recovery
- **Error Logging** - Structured error logging and reporting

**Key Features Demonstrated:**
- Specific exception types (ValidationError, ConfigError, ResourceError, StateError)
- Type safety and validation
- Parameter validation with recovery suggestions
- State checking and error recovery
- Graceful error handling and recovery patterns
- Structured error logging and analysis

### Performance Examples
The `performance/` directory contains examples that demonstrate performance optimization techniques:

- **Vectorization Examples** - NumPy-based vectorized operations
- **Memory Management** - Memory pool optimizations and efficient data structures
- **Parallel Processing** - SIMD vectorization and parallelization techniques
- **Benchmarking** - Performance testing and comparison
- **Optimization Techniques** - Tips for improving NLM performance
- **Profiling** - Performance profiling and analysis

**Key Features Demonstrated:**
- NumPy vectorization for numerical operations
- Memory-efficient data structures
- Parallel processing techniques
- Performance benchmarking and profiling
- Optimization best practices
- Memory management and garbage collection

### Integration Examples
The `integration/` directory contains examples that demonstrate integration with the Python ecosystem:

- **NumPy Integration** - Vectorized operations with NumPy
- **Pandas Integration** - Data analysis with DataFrames
- **Matplotlib Integration** - Visualization with Matplotlib
- **Scikit-learn Integration** - Machine learning with Scikit-learn
- **Complete Integration Examples** - All integrations working together
- **Custom Integration Patterns** - Advanced integration patterns

**Key Features Demonstrated:**
- NumPy integration for vectorized numerical operations
- Pandas integration for structured data analysis
- Matplotlib integration for scientific visualization
- Scikit-learn integration for machine learning
- Complete end-to-end integration patterns
- Custom integration solutions

### Phase 3 and Phase 4 Examples
The `phase3/` and `phase4/` directories contain examples that demonstrate Phase 3 and Phase 4 neural mechanisms:

- **Phase 3**: Working memory, episodic memory, associative memory examples
- **Phase 4**: Neural prediction, concept formation, attention, planning examples

**Key Features Demonstrated:**
- Advanced neural mechanisms from Phase 3
- Complex cognitive functions from Phase 4
- Integration of multiple brain systems
- Real-world simulation scenarios

## How to Create New Examples

### 1. New Example Category

Create a new directory in `examples/` for the category:

```bash
mkdir -p examples/new_category
```

### 2. Add Documentation

Create a `README.md` file in the new directory:

```markdown
# New Category Examples

[Description of the examples]

## How to Use

[Instructions for running examples]

## Example Categories

[Description of each example in the category]

## Dependencies

[Required Python packages]

## Key Features Demonstrated

[List key features demonstrated by examples]

## Learning Path

[Suggested learning path through the examples]
```

### 3. Create Example Files

Create example Python files in the new directory:

```python
# example1_basic_example.py
"""
Description of the example.
"""

import pynlm
from enhanced_bindings import *

# Example code
print("Example output")
```

### 4. Test Examples

Test the examples to ensure they work correctly:

```bash
# Navigate to the example directory
cd examples/new_category

# Run an example
python example1_basic_example.py

# Test with pytest
pytest example1_basic_example.py
```

### 5. Update Documentation

Update `COMPREHENSIVE_DOCUMENTATION.md` to include the new category:

```markdown
### New Category Examples
[Description and examples]
```

## Example Template

Here's a template for creating new examples:

```python
"""
Example Template

[Description of the example]

Features demonstrated:
- [Feature 1]
- [Feature 2]
- [Feature 3]
"""

import pynlm
from enhanced_bindings import *

print("=== Example Template ===")

# Example code that demonstrates the feature
# ...

print("=== Example Complete ===")
```

## Best Practices for Examples

### 1. Code Quality

- Follow PEP 8 coding standards
- Use descriptive variable names
- Include comprehensive comments
- Write clear docstrings
- Use type hints where appropriate

### 2. Documentation

- Include detailed docstrings
- Add comments explaining complex code
- Document input parameters and output
- Include usage examples
- Show expected output

### 3. Testing

- Test with different inputs and configurations
- Handle edge cases and error conditions
- Validate input parameters
- Test error handling and recovery
- Benchmark performance where relevant

### 4. Reproducibility

- Include all necessary imports
- Set random seeds for reproducible results
- Document dependencies and versions
- Provide clear installation instructions

### 5. Learning Value

- Start with simple examples
- Gradually introduce complexity
- Build on previous examples
- Demonstrate best practices
- Show common pitfalls and solutions

## Common Example Patterns

### 1. Basic Brain Example

```python
import pynlm
from enhanced_bindings import EnhancedBrain

# Create enhanced brain
brain = EnhancedBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
brain.initialize()

# Use Pythonic interface
print(f"Neuron count: {brain.neuron_count}")
print(f"Synapse count: {brain.synapse_count}")

# Step the brain
brain.step(0)

# Get statistics
stats = brain.get_statistics()
print(f"Total spikes: {stats['total_spikes']}")
```

### 2. Context Manager Example

```python
import pynlm
from enhanced_bindings import simulation_step, EnhancedBrain

# Create components
brain = EnhancedBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Use context manager
with simulation_step(world, brain, agent) as step_result:
    print(f"Step completed with {step_result.get('spikes_generated', 0)} spikes")
```

### 3. Data Collection Example

```python
import pynlm
from enhanced_bindings import SimulationRunner, DataCollector, Analysis

# Create components
brain = EnhancedBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run experiment with data collection
runner = SimulationRunner(world, brain, agent)
results = runner.run_experiment(steps=100)

# Collect data
data_collector = DataCollector()
for step_result in results['history']:
    data_collector.record(step_result)

# Analyze data
data = data_collector.get_data()
analysis = Analysis(data)
stats = analysis.compute_statistics()

print(f"Spike statistics:")
for key, value in stats.get('spikes', {}).items():
    print(f"  {key}: {value:.2f}")
```

### 4. Configuration Example

```python
import pynlm
from enhanced_bindings import EnhancedConfig

# Create enhanced config
config = EnhancedConfig(pynlm.createDefaultConfig())

# Set values using dictionary-like access
config['brain.neuron_count'] = 200
config['brain.synapse_density'] = 0.15
config['world.vision_width'] = 10
config['world.vision_height'] = 10

# Get values
neuron_count = config.get_int('brain.neuron_count', 100)
synapse_density = config.get_float('brain.synapse_density', 0.1)

print(f"Neuron count: {neuron_count}")
print(f"Synapse density: {synapse_density}")
```

## Example Categories Summary

| Category | Description | Key Features |
|----------|-------------|--------------|
| `basic_usage` | Basic NLM functionality with enhanced features | Context managers, Pythonic interfaces |
| `advanced_usage` | Advanced features and patterns | Utility classes, callbacks, observers |
| `error_handling` | Error handling and validation | Exception types, recovery patterns |
| `performance` | Performance optimization | NumPy, vectorization, memory management |
| `integration` | Python ecosystem integration | NumPy, Pandas, Matplotlib, Scikit-learn |
| `phase3` | Phase 3 neural mechanisms | Working memory, episodic memory, associative memory |
| `phase4` | Phase 4 neural mechanisms | Neural prediction, concept formation, attention, planning |

## Getting Help

### Common Issues

1. **"Example not working"**
   - Check that all imports are correct
   - Verify the example doesn't rely on unavailable dependencies
   - Run the example with detailed error output

2. **"Performance issues"**
   - Use vectorization for numerical operations
   - Profile the example to identify bottlenecks
   - Optimize memory usage for large datasets

3. **"Error handling not working"**
   - Check that exception types are correctly caught
   - Verify error recovery logic
   - Add debug output to understand error flow

### Getting Help

- Refer to the project documentation in `COMPREHENSIVE_DOCUMENTATION.md`
- Check the examples directory for usage patterns
- Review the code for best practices
- Contact support if needed for specific issues

## Conclusion

The examples directory provides comprehensive coverage of the enhanced NLM Python bindings, demonstrating all features and providing learning paths for users at different skill levels. Each example category builds on the previous ones, creating a structured learning experience that covers everything from basic functionality to advanced features.

Whether you're just getting started with NLM or looking to build complex, sophisticated simulations, the examples provide everything you need to create powerful neural systems efficiently and effectively.
