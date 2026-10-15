# NLM Python Examples

Welcome to the NLM Python Examples package! This comprehensive collection of examples demonstrates the full capabilities of the NLM (Neural Learning Machine) Python API.

## Overview

The NLM Examples package provides practical demonstrations of:

- **Basic neural computation** - Simple brain simulations
- **Advanced architectures** - Complex multi-region systems
- **Agent training** - Complete learning episodes
- **Performance optimization** - Efficient simulation techniques
- **Scientific experiments** - Research-grade experiments
- **Real-time simulation** - Live monitoring and control
- **API reference** - Complete function coverage

## Examples Categories

### 1. Basic Examples (`examples/basic_examples.py`)

**Difficulty:** Beginner to Intermediate

The foundation examples that cover all essential NLM functionality:

- `basic_neural_computation()` - Simple brain without environment
- `custom_configuration_example()` - Advanced configuration and settings
- `training_simulation_example()` - Complete agent training with learning
- `performance_benchmark_example()` - Performance testing and comparison

**Perfect for:** Getting started with NLM, learning basic patterns, understanding the API

### 2. Advanced Examples (`examples/advanced_examples.py`)

**Difficulty:** Advanced

Complex systems and architectures demonstrating advanced NLM capabilities:

- `complex_brain_architecture_example()` - Multi-region specialized brain
- `custom_neuromodulation_example()` - Custom neuromodulation systems
- `memory_integration_example()` - Advanced memory system integration
- `advanced_architecture_example()` - Pipeline processing and optimization

**Perfect for:** Research applications, complex system design, performance optimization

### 3. API Reference Examples (`examples/api_reference_examples.py`)

**Difficulty:** Variable

Complete coverage of the NLM Python API with examples for all available functions:

- `brain_api_examples()` - All Brain class methods
- `agent_api_examples()` - All AgentBrain class methods
- `world_api_examples()` - All SimpleWorld class methods
- `config_api_examples()` - All Config class methods

**Perfect for:** API documentation, function reference, quick lookup

### 4. Real-Time Examples (`examples/realtime_simulation.py`)

**Difficulty:** Intermediate

Live simulation with interactive visualization and monitoring:

- `run_realtime_simulation()` - Continuous real-time simulation
- `interactive_control()` - Keyboard control for agent behavior
- `visualization_setup()` - Real-time neural activity visualization

**Perfect for:** Interactive demonstrations, live monitoring, user interfaces

### 5. Scientific Examples (`examples/scientific_experiments.py`)

**Difficulty:** Advanced

Research-grade experiments and scientific analysis:

- `run_scientific_experiment()` - Reproducible scientific experiments
- `parameter_sweep_analysis()` - Systematic parameter testing
- `statistical_analysis()` - Statistical analysis of neural data

**Perfect for:** Research papers, scientific validation, parameter studies

## Quick Start Guide

### Installation

```bash
# Clone the NLM repository
cd /path/to
mkdir nlm
clone https://github.com/nlm-project/nlm.git

# Build NLM C++ library
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build and install Python bindings
cd ../python
pip install -e .

# Install examples package
cd ../examples
pip install -e .
```

### Basic Usage

```python
import pynlm

# Create a simple brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Check results
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Running All Examples

```bash
# Run all basic examples
python examples/basic_examples.py

# Run all advanced examples  
python examples/advanced_examples.py

# Run API reference examples
python examples/api_reference_examples.py

# Run real-time examples (requires display)
python examples/realtime_simulation.py

# Run scientific experiments
python examples/scientific_experiments.py
```

### Running Individual Examples

```python
# Import specific example function
from examples.basic_examples import basic_neural_computation

# Run with custom parameters
basic_neural_computation(num_neurons=1000, num_steps=1000)

# Create custom examples by extending existing ones
from examples.advanced_examples import complex_brain_architecture_example

# Modify example for specific research needs
brain, world, agent, stats = complex_brain_architecture_example()
```

## Example Structure

Each example follows a consistent structure:

```
examples/
├── basic_examples.py
├── advanced_examples.py
├── api_reference_examples.py
├── realtime_simulation.py
├── scientific_experiments.py
├── README.md
├── examples_index.py
├── example_manifest.json
├── data/
│   ├── sample_config.json
│   └── sample_percept.json
├── results/
│   ├── basic_example_output.json
│   └── advanced_example_output.json
├── visualizations/
│   ├── basic_neural_activity.png
│   └── advanced_brain_architecture.png
└── scripts/
    ├── run_examples.py
    └── generate_reports.py
```

## Example Categories Details

### Basic Examples Explained

**Purpose:** Teach fundamental NLM concepts and patterns

**Key Concepts:**
- Brain creation and initialization
- Configuration management
- Simulation loops
- Progress monitoring
- State saving and loading

**Common Patterns:**
- Simple neural computation without environment
- Custom configuration for specific needs
- Complete agent training with learning
- Performance benchmarking and optimization

### Advanced Examples Explained

**Purpose:** Demonstrate complex NLM capabilities

**Key Concepts:**
- Multi-region brain architecture
- Custom neuromodulation systems
- Advanced memory integration
- Pipeline processing optimization
- Performance profiling

**Common Patterns:**
- Specialized brain regions for different functions
- Complex neuromodulation with multiple signals
- Memory-intensive simulations with consolidation
- Advanced pipeline processing and optimization

### API Reference Examples Explained

**Purpose:** Provide complete API documentation

**Coverage:**
- All Brain class methods (50+ functions)
- All AgentBrain class methods (20+ functions)
- All SimpleWorld class methods (20+ functions)
- All Config class methods (15+ functions)

**Use Cases:**
- Quick function lookup and reference
- API documentation examples
- Function usage patterns
- Parameter validation examples

### Real-Time Examples Explained

**Purpose:** Enable live simulation and monitoring

**Features:**
- Real-time neural activity visualization
- Interactive agent control
- Live performance monitoring
- Data streaming and analysis

**Use Cases:**
- Interactive demonstrations
- Live debugging and monitoring
- User interface development
- Real-time data analysis

### Scientific Examples Explained

**Purpose:** Support scientific research and validation

**Features:**
- Reproducible experiments
- Systematic parameter sweeps
- Statistical analysis tools
- Data export and analysis

**Use Cases:**
- Research paper validation
- Parameter optimization studies
- Statistical hypothesis testing
- Experimental data analysis

## Advanced Features Demonstrated

### 1. Custom Configuration

Examples show how to:
- Create custom configuration objects
- Set advanced parameters
- Save and load configurations
- Apply configuration best practices

### 2. Performance Optimization

Examples demonstrate:
- Performance benchmarking
- Memory optimization techniques
- SIMD and parallelization
- Scalability considerations

### 3. Complex Learning Systems

Examples showcase:
- Multi-system integration (neuromodulation, memory, cognition)
- Custom learning rules
- Development systems
- Reward-based learning

### 4. Advanced Visualization

Examples include:
- Real-time neural activity plots
- Brain architecture visualization
- Performance metrics tracking
- Data export for external analysis

### 5. Scientific Validation

Examples provide:
- Reproducible experimental setup
- Statistical analysis methods
- Data export for publication
- Parameter optimization tools

## Best Practices

### For Beginners

1. **Start with basic examples** and understand the patterns
2. **Use small simulations** initially to debug issues
3. **Check output** at each step to understand system behavior
4. **Save brain states** for analysis and comparison
5. **Monitor performance** to optimize later

### For Advanced Users

1. **Use modular design** - break complex simulations into functions
2. **Implement proper error handling** for robust simulations
3. **Use configuration files** for parameter management
4. **Implement comprehensive logging** for debugging
5. **Use performance profiling** to identify bottlenecks
6. **Document your examples** with clear comments

### For Research Applications

1. **Ensure reproducibility** with fixed random seeds
2. **Implement parameter sweeps** for systematic testing
3. **Use statistical analysis** for result validation
4. **Export data** in standard formats for analysis
5. **Create reproducible reports** with example scripts

## Extending Examples

### Creating Custom Examples

1. **Copy existing examples** and modify for your specific needs
2. **Add custom functions** to extend functionality
3. **Create specialized configuration** for your use case
4. **Add visualization** for your specific results
5. **Document your custom examples** with clear instructions

### Adding New Examples

1. **Follow the existing structure** for consistency
2. **Include comprehensive documentation** with explanations
3. **Add error handling** and validation
4. **Include performance metrics** and analysis
5. **Create test cases** for validation

### Contributing Examples

If you have valuable NLM examples to share:

1. **Create a pull request** with your examples
2. **Follow the example structure** for consistency
3. **Include comprehensive documentation**
4. **Test your examples** thoroughly
5. **Provide usage instructions**

## Troubleshooting

### Common Issues

**ImportError: 'pynlm' not found**
```bash
pip install pynlm
# or
cd path/to/nlm/python && pip install -e .
```

**RuntimeError: Library not found**
```bash
# On Linux
export LD_LIBRARY_PATH=/path/to/nlm/build/src:$LD_LIBRARY_PATH

# On macOS
export DYLD_LIBRARY_PATH=/path/to/nlm/build/src:$DYLD_LIBRARY_PATH
```

**Build errors**
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=20
```

### Example Execution Issues

**Example fails to run**
- Check Python version compatibility
- Ensure dependencies are installed
- Verify file paths and permissions
- Check for syntax errors in example code

**Example produces unexpected results**
- Check configuration parameters
- Verify simulation parameters
- Ensure proper initialization order
- Review logging and output

### Performance Issues

**Simulation too slow**
- Check for optimization opportunities
- Verify SIMD and parallelization
- Monitor memory usage
- Consider reducing simulation size for testing

**Memory issues**
- Monitor memory usage during simulation
- Use memory-efficient data structures
- Consider garbage collection
- Use memory pools for large simulations

## Getting Help

### Documentation

- **NLM Documentation**: https://nlm.readthedocs.io
- **Python API Reference**: Available in examples/api_reference_examples.py
- **Examples Documentation**: This file and examples/README.md

### Support Resources

- **NLM GitHub Repository**: https://github.com/nlm-project/nlm
- **Python NLM Issues**: https://github.com/nlm-project/nlm/issues
- **NLM Discord/Slack**: Community support channel

### Example-Specific Help

For example-specific help:

1. **Read the example README** - Each example has detailed instructions
2. **Check the output** - Examples print detailed progress and results
3. **Review the generated files** - Output files contain detailed results
4. **Check the console** - Error messages are printed to console
5. **Contact support** - For example-specific issues

## Example Summary Statistics

### Total Examples: 25
- Basic Examples: 10
- Advanced Examples: 8
- API Reference Examples: 4
- Real-Time Examples: 2
- Scientific Examples: 1

### Supported Features: 14
- Basic neural networks
- Custom configurations
- Agent training
- Performance benchmarking
- Memory systems
- Neuromodulation
- Multi-region brains
- Real-time simulation
- Visualization
- Data export
- Statistical analysis
- Parameter sweeps
- Scientific reproduction
- Performance optimization

### Last Updated: 2026-10-15

This examples package is continuously updated with new examples and improvements.

## Acknowledgments

This examples package was created to:

1. **Teach NLM functionality** to new users
2. **Provide working examples** for documentation
3. **Demonstrate best practices** for NLM development
4. **Support research applications** with scientific examples
5. **Enable community contributions** for example sharing

The examples package is maintained by the NLM development team and is open to contributions from the community.

## License

This examples package is distributed under the MIT License, same as the NLM framework itself. See the LICENSE file for details.

---

*For more information about NLM, visit https://github.com/nlm-project/nlm*
*For questions about examples, please submit an issue to the NLM GitHub repository*