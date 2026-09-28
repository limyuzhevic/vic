#!/usr/bin/env python3
"""
Python Examples for NLM Advanced Python Usage

This directory contains practical Python examples demonstrating advanced
usage of the NLM Python API, including configuration, simulation control,
data analysis, and performance optimization.
"""

# README for python_examples directory

This directory contains advanced Python examples for NLM usage, organized as follows:

## Core Examples (examples/)

The examples/ directory contains 10 comprehensive examples demonstrating:

1. Basic brain simulation
2. Agent-environment interaction
3. Memory system usage
4. Neuromodulation and learning
5. Development system
6. Advanced agent behavior
7. Performance monitoring
8. Checkpoint and persistence
9. Curriculum learning
10. Meta-learning

Each example provides:
- Practical usage of NLM Python API
- Complete working code
- Explanations of concepts
- Visualization of results

## Advanced Python Techniques (python_examples/)

The python_examples/ directory contains specialized examples focusing on:

1. **Configuration Management** - Advanced configuration techniques
2. **Simulation Control** - Programmatic simulation management
3. **Data Analysis** - Post-simulation analysis tools
4. **Performance Optimization** - Python-level performance tuning
5. **Error Handling** - Robust error handling and recovery
6. **Integration Patterns** - Integration with external systems
7. **Custom Extensions** - Extending NLM capabilities
8. **Batch Processing** - Multiple simulation runs
9. **Visualization Tools** - Python-based visualization
10. **Testing Frameworks** - Automated testing and validation

## Usage Instructions

### Running Examples

To run an example from the examples/ directory:

```bash
# Navigate to the directory
# Run with Python
python example1_basic_brain.py

# Or use python3
python3 example2_agent_world.py
```

### Running Advanced Python Examples

To run an example from the python_examples/ directory:

```bash
cd python_examples
python example1_configuration.py
```

### Dependencies

All examples require:
- Python 3.8+
- NumPy
- Matplotlib (for visualization examples)
- Pandas (for data analysis examples)

### Installation

Ensure NLM Python bindings are installed:

```bash
# Install from source
pip install -e .

# Or install from wheel
pip install pynlm
```

### Example Structure

Each example follows this structure:

1. **Import necessary modules**
2. **Define configuration and parameters**
3. **Create NLM components (brain, world, agent)**
4. **Configure and initialize components**
5. **Run simulation or demonstrate functionality**
6. **Analyze and display results**
7. **Provide explanations and insights**

### Key Concepts Demonstrated

The examples demonstrate:

- **Configuration Management**: Creating and modifying NLM configurations
- **Simulation Control**: Programmatic control of simulation parameters
- **Memory Systems**: Working with working memory, episodic memory
- **Neuromodulation**: Understanding dopamine, curiosity, novelty
- **Development**: Developmental stages and progression
- **Performance**: Profiling and optimization techniques
- **Persistence**: Checkpoint saving and loading
- **Learning**: Curriculum learning and meta-learning
- **Integration**: Combining multiple NLM systems
- **Analysis**: Post-simulation data analysis and visualization

### Advanced Topics Covered

- **Parameter Tuning**: Optimizing simulation performance
- **Error Handling**: Robust code with comprehensive error handling
- **Data Visualization**: Creating plots and visualizations of NLM behavior
- **Batch Processing**: Running multiple simulations efficiently
- **Testing**: Writing tests for NLM functionality
- **Integration**: Working with external libraries and systems
- **Custom Extensions**: Extending NLM with custom functionality
- **Performance Monitoring**: Real-time monitoring of simulation metrics

### Learning Path

Suggested learning progression:

1. **Start with basic examples** (example1_basic_brain.py)
2. **Progress to agent examples** (example2_agent_world.py)
3. **Explore memory systems** (example3_memory_systems.py)
4. **Study neuromodulation** (example4_neuromodulation.py)
5. **Understand development** (example5_development.py)
6. **Implement advanced features** (examples 6-10)
7. **Study advanced Python techniques** (python_examples/)

### Example Categories

#### Core Functionality Examples
- Brain simulation and neural dynamics
- Agent-environment interaction
- Memory system usage
- Neuromodulation and learning
- Development system

#### Advanced Behavior Examples
- Advanced agent planning
- Performance monitoring
- Checkpoint and persistence
- Curriculum learning
- Meta-learning

#### Python-Specific Examples
- Configuration management
- Simulation control
- Data analysis
- Performance optimization
- Error handling

Each example is self-contained and can be run independently, but they also build upon concepts from previous examples.

### File Organization

- **examples/**: Comprehensive NLM usage examples
- **python_examples/**: Advanced Python-specific examples
- **docs/**: Documentation files
- **README.md**: This file

### Getting Started

1. **Install NLM Python bindings**
2. **Choose an example to start with**
3. **Run the example and observe output**
4. **Explore additional examples for more advanced features**
5. **Modify examples to create custom implementations**

### Further Reading

For more advanced usage, refer to:
- `INTEGATION.md` - System integration guide
- `PERFORMANCE.md` - Performance optimization guide
- `TROUBLESHOOTING.md` - Advanced troubleshooting
- `PYTHON_ADVANCED.md` - Advanced Python techniques

This directory provides a comprehensive foundation for working with NLM through practical Python examples, covering everything from basic usage to advanced applications.