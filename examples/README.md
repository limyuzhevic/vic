# NLM Advanced Usage Examples

This directory contains comprehensive examples for using NLM (Neural Learning Machine) at different skill levels. These examples demonstrate the full capabilities of the Phase 6 integrated brain system.

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain project that simulates complex neural systems with integrated memory, prediction, cognition, and neuromodulation systems. This directory provides practical examples for getting started and advancing your NLM skills.

## Structure

The examples are organized by skill level and application type:

- **Beginner Examples**: Basic usage and fundamental concepts
- **Intermediate Examples**: Configuration customization and multi-agent systems
- **Advanced Examples**: Research applications and development studies
- **Research Examples**: Ablation studies, scaling studies, and performance analysis
- **Performance Examples**: Optimization and benchmarking

## Quick Start

To get started with NLM examples:

1. **Install NLM** (if not already installed)
   ```bash
   pip install -e .
   ```

2. **Run basic examples**
   ```bash
   cd examples/beginner
   python example1_basic_brain.py
   python example2_agent_world.py
   ```

3. **Explore intermediate examples**
   ```bash
   cd examples/intermediate
   python example4_custom_config.py
   python example5_multi_agent.py
   ```

## Getting Help

- **Documentation**: See `README.md`, `HOW_TO_USE.md`, and `easy_usage.md` for comprehensive guidance
- **Source Code**: Examine the examples to understand implementation patterns
- **Community**: Consult project documentation for advanced topics and support

## Example Categories

### Beginner Examples

These examples are designed for users new to NLM, covering basic concepts and workflows:

1. **Basic Brain Simulation** - Simple brain creation and simulation
2. **Agent in World Environment** - Basic agent-world interaction
3. **Complete Learning Episode** - Full NLM feature demonstration

### Intermediate Examples

These examples require some understanding of NLM concepts and demonstrate more advanced usage:

1. **Configuration Customization** - Brain parameter optimization
2. **Multi-Agent System** - Interconnected agents and shared worlds
3. **Developmental Trajectory** - Age-dependent plasticity studies

### Advanced Examples

These examples demonstrate advanced NLM usage patterns and research applications:

1. **Checkpoint-Based Learning** - Recovery and long-term learning
2. **Performance Optimization** - Benchmarking and optimization techniques

### Research Examples

These examples are designed for research applications and scientific studies:

1. **Ablation Studies** - Component removal and impact analysis
2. **Scaling Studies** - Brain size scaling and performance analysis

### Performance Examples

These examples focus on performance optimization and benchmarking:

1. **Memory Pool Optimization** - Memory-efficient configurations
2. **Benchmarking Suite** - Comprehensive performance testing

## Running Examples

### Individual Examples

Run any example directly:
```python
# From the examples directory
python beginner/example1_basic_brain.py
python intermediate/example4_custom_config.py
python advanced/example7_checkpoint_learning.py
```

### Batch Processing

Run all examples in a category:
```bash
# Run all beginner examples
cd examples/beginner
for example in *.py; do
    python "$example"
    echo "Completed $example"
    echo
    sleep 2
done

# Run all examples in a category
for category in beginner intermediate advanced research performance; do
    echo "=== Running $category examples ==="
    cd "$category"
    for example in *.py; do
        python "$example"
    done
    cd ../
    echo
    sleep 5
done
```

### Comparative Analysis

Run comparative examples that analyze different configurations:
```bash
# Run comparative examples
python beginner/example3_complete_learning.py
python intermediate/example5_multi_agent.py
python advanced/example8_performance_optimization.py
python research/example9_ablation_studies.py
```

## Example File Conventions

### File Structure

Each example follows this structure:

```
examples/
├── beginner/
│   ├── example1_basic_brain.py      # Basic simulation
│   ├── example2_agent_world.py      # Agent-world interaction
│   └── example3_complete_learning.py  # Full feature demo
├── intermediate/
│   ├── example4_custom_config.py      # Configuration customization
│   ├── example5_multi_agent.py        # Multi-agent systems
│   └── example6_developmental.py       # Development studies
├── advanced/
│   ├── example7_checkpoint_learning.py  # Checkpointing
│   └── example8_performance_optimization.py  # Performance analysis
├── research/
│   ├── example9_ablation_studies.py    # Component ablation
│   └── example10_scaling_studies.py    # Brain size scaling
└── performance/
    ├── example11_memory_optimization.py  # Memory efficiency
    └── example12_benchmarking.py        # Comprehensive benchmarking
```

### Code Style

Examples follow these conventions:

1. **Clear imports** with necessary comments
2. **Function definitions** for each example
3. **Comprehensive docstrings** explaining the example purpose
4. **Progress output** showing simulation results
5. **Error handling** for common issues
6. **Parameter documentation** for customization

### Documentation Standards

Each example includes:

1. **Purpose description** explaining what the example demonstrates
2. **Code comments** explaining key sections
3. **Output examples** showing expected results
4. **Customization guidelines** for adapting the example
5. **Next steps** suggesting further exploration

## Contributing Examples

If you have useful NLM examples to share, consider contributing by:

1. **Adding new examples** to appropriate categories
2. **Documenting existing examples** with additional details
3. **Creating comparative examples** that analyze different approaches
4. **Providing benchmark examples** for performance analysis
5. **Submitting research examples** that demonstrate novel NLM applications

## Example Categories Explained

### Beginner Examples

The beginner examples focus on:

- **Basic functionality**: Simple brain creation and simulation
- **Core concepts**: Understanding NLM's integrated systems
- **Practical usage**: Real-world application patterns
- **Step-by-step guidance**: Clear implementation examples

### Intermediate Examples

The intermediate examples build on basics by:

- **Configuration optimization**: Custom brain parameters
- **System complexity**: Multi-agent interactions
- **Development dynamics**: Age-dependent learning
- **Advanced features**: Full NLM capability utilization

### Advanced Examples

The advanced examples explore:

- **Long-term learning**: Checkpoint-based and recovery mechanisms
- **Performance analysis**: Benchmarking and optimization techniques
- **System monitoring**: Comprehensive performance metrics
- **Resource management**: Memory and computational efficiency

### Research Examples

The research examples support:

- **Scientific investigation**: Component removal and impact analysis
- **Scaling studies**: Performance across different configurations
- **Comparative analysis**: Different approaches evaluation
- **Hypothesis testing**: Research question validation

### Performance Examples

The performance examples provide:

- **Optimization studies**: Memory and computational efficiency
- **Benchmarking suites**: Comprehensive performance testing
- **Resource analysis**: Memory usage and performance trade-offs
- **Comparative analysis**: Different optimization strategies

## Example Quality Criteria

All examples must meet these criteria:

1. **Functionality**: Examples must run and demonstrate NLM capabilities
2. **Documentation**: Clear explanations of purpose and implementation
3. **Educational Value**: Examples should teach NLM concepts and usage
4. **Practical Application**: Real-world relevance and applicability
5. **Code Quality**: Readable, maintainable, and well-structured code
6. **Error Handling**: Robust handling of common issues and edge cases
7. **Customization**: Examples should be easily adaptable to different use cases

## Next Steps After Examples

After working with examples, consider:

1. **Understanding the codebase**: Explore `src/` to see how examples work
2. **Reading documentation**: Review `README.md`, `HOW_TO_USE.md`, `easy_usage.md`
3. **Running demos**: Try the built-in demo executables
4. **Contributing**: Share your own examples with the community
5. **Research**: Apply NLM to your specific research questions

## Example License

These examples are provided under the same MIT license as NLM. Feel free to use, modify, and extend them for your own projects.

---

**Happy NLM coding!**

*Note: Some examples may require additional computational resources or time to complete. Adjust parameters as needed for your hardware and research goals.*
