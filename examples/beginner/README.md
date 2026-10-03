# Beginner Examples for NLM

This directory contains beginner-level examples that demonstrate fundamental NLM concepts through hands-on learning. Each example builds upon the previous one, starting with simple demonstrations and progressively introducing more complex neural mechanisms.

## Overview

Beginner examples focus on understanding core NLM concepts:

- **Single Neuron Learning**: Basic neural response to stimuli
- **Simple Sensorimotor Loop**: Complete agent-world interaction
- **Memory Formation**: How neural systems store and retrieve information
- **Plasticity Demonstration**: Visualizing synaptic changes over time

Each example includes both Python and C++ implementations, making the concepts accessible regardless of programming preference.

## Learning Path

1. **Single Neuron Learning** → Understand basic neuron dynamics
2. **Simple Sensorimotor Loop** → See how brains interact with environments
3. **Memory Formation** → Learn how neural systems encode memories
4. **Plasticity Demonstration** → Visualize learning through synaptic changes

## Example Structure

Each example includes:

- `README.md` - Comprehensive documentation
- `example_name.py` - Python implementation with Jupyter notebook
- `example_name.cpp` - C++ implementation
- `example_name.ipynb` - Interactive Jupyter notebook
- Tests in `tests/` - Unit tests for validation

## Running Examples

### Python Examples

```bash
# From project root
python examples/beginner/example_name/example_name.py

# With tests
python -m pytest examples/beginner/example_name/tests/ -v

# Jupyter notebook
jupyter notebook examples/beginner/example_name/example_name.ipynb
```

### C++ Examples

```bash
# Build and run
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DSINGLE_NEURON_LEARNING=ON
make example_name
./example_name
```

## Python Installation Requirements

```bash
pip install numpy matplotlib pandas scikit-learn jupyter
```

## Academic References

1. **Basic Neuron Modeling**: Dayan, G., & Abbott, L. F. (2001). Theoretical Neuroscience. MIT Press.
2. **LIF Neuron Dynamics**: Gerstner, W., Kistler, W., Naud, R., & Chauchy, G. (2014). Neuronal Dynamics. Cambridge University Press.
3. **Neural Plasticity**: Felsen, G. B., & Eccles, J. C. (1970). Perception: physiological and psychological evolution. Philosopher's imprint.
4. **Learning in Neural Networks**: Seung, H. S., & Lightfoot, D. (2000). Neural networks: computational approaches and applications. MIT press.

## Support and Troubleshooting

### Common Issues

1. **Missing Dependencies**: All Python examples include requirements.txt files
2. **Memory Usage**: Increase system memory for large neural networks
3. **Visualization Problems**: Ensure matplotlib backend is configured properly
4. **Running Tests**: Use `python -m pytest` with appropriate test directories

### Getting Help

- Check individual example READMEs for specific guidance
- Review the main `HOW_TO_USE.md` for general troubleshooting
- Join the NLM community for peer support
- Report issues through the project issue tracker

## Example Compatibility

These examples are designed to work with NLM version 0.1.0 and support:

- **Operating Systems**: Linux, macOS, Windows
- **Python Versions**: 3.8, 3.9,3.10, 3.11, 3.12
- **C++ Standards**: C++20
- **Architecture**: x86_64, ARM64

## Future Development

This examples directory will continue to expand with:

- More complex scenarios
- Visualization tools and analysis scripts
- Benchmarking and performance comparison
- Community-contributed examples
- Educational exercises and assignments

## Contributing

We welcome contributions to the examples directory:

1. **New Examples**: Propose and implement new learning scenarios
2. **Improvements**: Enhance existing examples with visualizations, tests, or documentation
3. **Translations**: Create language-specific versions of examples
4. **Reviews**: Provide feedback on example clarity and accuracy

## License

Examples are released under the MIT License, allowing for educational and commercial use with proper attribution.
