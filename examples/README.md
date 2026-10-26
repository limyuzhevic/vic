# NLM (Neural Learning Machine) - Core Components

This directory contains the core examples for NLM, demonstrating
fundamental usage patterns and advanced experimentation.

## Included Examples

### Basic Examples (`basic_examples.py`)
Introduces core NLM concepts:
- Basic brain creation and simulation
- Configuration management
- Memory systems
- Neuromodulation
- Checkpointing
- Experiment framework
- Performance monitoring

### Advanced Examples (`advanced_examples.py`)
Demonstrates complex usage patterns:
- Experiment design and comparative analysis
- Memory system optimization
- Neuromodulation experiments
- Development stages investigation
- Parameter sensitivity analysis

### Practical Tutorials (`tutorials.py`)
Step-by-step learning guides:
- Getting started with NLM
- Configuration management
- Memory systems
- Neuromodulation
- Checkpointing
- Performance monitoring
- Experiment framework
- Sample project creation

## Usage

Run the examples from the project root:

```bash
python3 examples/basic_examples.py
python3 examples/advanced_examples.py
python3 examples/tutorials.py
```

Each example provides detailed output explaining the concepts and results.

## Learning Path

1. **Start with `basic_examples.py`** for fundamental understanding
2. **Progress to `advanced_examples.py`** for complex experimentation
3. **Use `tutorials.py`** for guided learning with practical tasks
4. **Create your own examples** based on the patterns demonstrated

## Project Structure

The examples directory follows NLM's modular design:

- `examples/` - Main example files
- `examples/` subdirectories for specialized examples
- `examples/` includes performance monitoring and debugging tools

## Best Practices

When using these examples:

1. **Customize parameters** for your specific research questions
2. **Monitor performance** using built-in profiling tools
3. **Document your experiments** with clear parameter settings
4. **Compare configurations** to identify optimal parameter ranges
5. **Iterate systematically** based on experimental results

## Example Usage Pattern

A typical example follows this pattern:

```python
import nlm

# 1. Configure the brain
brain = nlm.Brain()

# 2. Run simulation
result = brain.run_steps(100)

# 3. Analyze results
print(f"Reward: {result['total_reward']:.4f}")
print(f"Firing rate: {result['average_firing_rate']:.2f} Hz")

# 4. Save results (optional)
brain.save_checkpoint("my_brain.bin")
```

## Further Reading

For more advanced usage, refer to:

- **Phase 6 Integration Documentation**: `docs/PHASE6_INTEGRATION.md`
- **Configuration Reference**: `docs/ARCHITECTURE.md`
- **Python API Documentation**: `python/bindings.cpp`
- **Performance Optimization**: `docs/PERFORMANCE.md`

## Troubleshooting

Common issues and solutions:

- **Import errors**: Ensure all dependencies are installed
- **Simulation failures**: Check configuration parameters
- **Memory issues**: Reduce simulation size or increase available memory
- **Performance problems**: Use monitoring tools to identify bottlenecks

## Contributing

Enhance the examples by:

1. Adding new experiment designs
2. Creating parameter optimization scripts
3. Developing visualization tools
4. Contributing performance improvements
5. Adding use case examples

## License

MIT License. See the main repository for full license information.

---

*These examples are designed to help users quickly get started with NLM and develop their own neural simulation experiments.*
