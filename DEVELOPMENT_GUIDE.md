# NLM Development Guide

This guide provides development information for contributing to the NLM project.

## Building the C++ Extension

The Python bindings require building the C++ extension. Use these commands:

### Using CMake Directly

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=/path/to/python

# Build the extension
make -j4
```

### Using pip

```bash
cd python
pip install -e .
```

### Using setup.py

```bash
cd python
python setup.py build_ext --inplace
```

## Python Development

### Running Tests

```bash
cd python
pytest tests/ -v
```

### Code Quality

Use black for formatting and isort for imports:

```bash
black pynlm.py
isort pynlm.py
```

### Type Checking

Install mypy and run:

```bash
pip install mypy
mypy pynlm.py
```

## Adding New Features

### Python Interface

1. **Add new functions** to `pynlm.py`
2. **Update `__all__`** with new exports
3. **Add tests** in `tests/test_pynlm_basic.py`
4. **Update documentation** in `README.md`

### C++ Extension

The Python bindings use pybind11 to expose C++ functionality. To add new bindings:

1. **Modify `bindings.cpp`** (if it exists) or create it
2. **Use pybind11 macros** to expose C++ classes and functions
3. **Test thoroughly** with Python tests
4. **Update documentation** with new usage examples

## Project Structure

```
NLM/
├── python/              # Python bindings and interface
│   ├── pynlm.py        # Core Python interface
│   ├── setup.py        # Build configuration
│   ├── pyproject.toml   # Modern Python package config
│   ├── README.md        # Python user documentation
│   ├── main.py         # CLI interface
│   └── tests/          # Test suite
├── src/                 # C++ core implementation
├── tests/              # C++ tests
└── configs/            # Configuration files
```

## Configuration

### pyproject.toml

The main Python package configuration:

- `project.name`: Package name (pynlm)
- `project.dependencies`: Required Python packages
- `project.optional-dependencies`: Optional features
- `tool.setuptools`: Package discovery configuration
- `tool.pytest.ini_options`: Test configuration

### setup.py

Legacy setuptools configuration with CMake integration for building the C++ extension.

## Testing Strategy

### Python Tests

- **Unit Tests**: Test individual functions and classes
- **Integration Tests**: Test Python-C++ interactions
- **System Tests**: Test complete workflows

### C++ Tests

- **Unit Tests**: Test neural components and algorithms
- **Integration Tests**: Test system connectivity
- **Performance Tests**: Test computational efficiency

## Performance Optimization

### Python Level

1. **Use convenience functions** with optimized defaults
2. **Batch operations** when possible
3. **Monitor memory usage** for large simulations
4. **Use appropriate timestep** (0.001 recommended)

### C++ Level

1. **Optimize neural computations**
2. **Use SIMD instructions** for vectorized operations
3. **Implement efficient memory management**
4. **Parallelize computationally intensive tasks**

## Best Practices

### Python Code

- Use descriptive variable names
- Add type hints for better documentation
- Write comprehensive docstrings
- Follow PEP 8 style guide
- Use context managers for resource management

### C++ Code

- Use RAII for resource management
- Write clean, readable code with comments
- Use appropriate data structures
- Consider memory alignment for performance
- Write unit tests for critical algorithms

## Contributing

### Pull Requests

1. **Fork the repository**
2. **Create a feature branch**
3. **Write tests** for new functionality
4. **Update documentation**
5. **Run full test suite**
6. **Submit pull request**

### Code Reviews

- Check for style consistency
- Verify test coverage
- Ensure documentation is updated
- Review performance implications
- Confirm compatibility with existing code

## Common Issues and Solutions

### "Could not import NLM C++ extension"

Solution: Build the extension with:

```bash
cd python
pip install -e .
```

### "ImportError: cannot import name '...')"

Solution: Check that the C++ extension is built and the function is properly exposed.

### Slow Performance

Solution:
- Reduce neuron count
- Use larger timestep
- Check for memory leaks
- Consider hardware acceleration

## Troubleshooting

### Build Failures

Check CMake output for specific errors. Common issues:
- Missing dependencies (pybind11, numpy)
- Compiler version compatibility
- Python development headers

### Runtime Errors

Check error messages and logs. Common issues:
- Invalid configuration parameters
- Memory allocation failures
- Invalid neural connectivity

## Future Development

### Suggested Enhancements

1. **Visualization Tools**: Add matplotlib plotting capabilities
2. **Advanced Analytics**: Statistical analysis and pattern detection
3. **Machine Learning Integration**: Connect with TensorFlow/PyTorch
4. **Web Interface**: Create interactive web-based brain explorer
5. **Simulation Toolkit**: Library for custom simulation scenarios

### Research Directions

1. **Hybrid Models**: Combine neural simulation with machine learning
2. **Multi-Agent Systems**: Simulate multiple interacting brains
3. **Real-world Applications**: Apply to robotics, gaming, education
4. **Neuromorphic Computing**: Target hardware acceleration

## Conclusion

This development guide provides the foundation for contributing to the NLM project. The improved structure and documentation make it easier to extend the system while maintaining its core scientific value and performance characteristics.

The project continues to evolve as a platform for investigating computational brain-like systems and understanding how neural computation can give rise to intelligent behavior.