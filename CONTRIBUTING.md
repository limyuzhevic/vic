# NLM (熙然) - Contributing Guide

## Welcome to NLM!

Thank you for your interest in contributing to the NLM (Neural Learning Machine) project. This guide will help you contribute effectively and understand the project's structure and development process.

## Project Overview

NLM is an experimental artificial developmental brain project that simulates biological neural systems. It implements:

- **Phase 1-5**: Individual system components
- **Phase 6**: Final integration of all systems into a coherent brain

The project is organized around several key subsystems:
- **Brain**: Central neural simulation with neurons and synapses
- **Memory**: Working, episodic, semantic, procedural, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error
- **Cognition**: Planning, concept formation, and attention
- **Development**: Age-dependent changes and critical periods
- **Environment**: Simple world simulation
- **Prediction**: Predictive coding and error computation

## Development Environment

### Prerequisites

#### C++ Development
- **Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **C++20** standard support

#### Python Development (for bindings)
- **Python**: 3.8+
- **pybind11**: Version 2.11.0+
- **scikit-build-core**: Version 0.5.0+

### Building the Project

#### Standard Build
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)
```

#### Python Bindings
```bash
# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python module
pip install .

# Or install in development mode
pip install -e .
```

#### Running Tests
```bash
cd build
ctest --output-on-failure
```

## Code Structure

### Core Source Code (`src/`)
```
/core/           # Core utilities and types
/brain/           # Neural components (integrated)
/dynamics/        # Neural dynamics
/plasticity/      # Plasticity rules
/development/     # Developmental system
/neuromodulation/ # Neuromodulators (integrated)
/memory/         # Memory systems (integrated)
/prediction/      # Prediction systems (integrated)
/cognition/       # Cognitive mechanisms (integrated)
/sensory/         # Sensory processing
/motor/           # Motor system
/environment/     # Environment interface
/experiments/     # Experiment framework (Phase 6)
/visualization/    # Visualization
```

### Python API
The Python API provides a high-level interface for using NLM:

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
    print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
```

## Contribution Guidelines

### Code Quality

1. **Follow Existing Patterns**: NLM uses modern C++ with event-driven design and PIMPL pattern for implementation.

2. **Documentation**: Every class, function, and method should have:
   - Clear documentation strings
   - Parameter descriptions
   - Return value descriptions
   - Example usage when applicable

3. **Error Handling**: Use exceptions for exceptional conditions and error codes for expected failures.

4. **Logging**: Use the provided logging system (`NLM_LOG_INFO`, `NLM_LOG_ERROR`, etc.) for all user-visible messages.

### Testing

1. **Unit Tests**: Each component should have comprehensive unit tests.

2. **Integration Tests**: Test cross-component interactions.

3. **Performance Tests**: For critical performance-sensitive components.

4. **Documentation Tests**: Examples in documentation should be verifiable.

### Performance Considerations

NLM is designed for efficient neural simulation:

- Use event-driven updates for spikes
- Implement memory pools for allocations
- Use SIMD optimizations where possible
- Minimize memory allocations during simulation

## Adding New Features

### Neural Components
1. Add new neuron types in `src/brain/Neuron.hpp`
2. Add synaptic models in `src/brain/Synapse.hpp`
3. Update plasticity rules in `src/plasticity/`

### Memory Systems
1. Add new memory types in `src/memory/`
2. Update memory integration in `src/brain/Brain.hpp`

### Neuromodulation
1. Add new neuromodulators in `src/neuromodulation/`
2. Update agent integration in `src/agent/AgentBrain.hpp`

### Experiments
1. Create new experiment types in `src/experiments/`
2. Update experiment framework in `src/experiments/Experiment.hpp`

## Debugging and Development

### Using GDB
```bash
gdb ./nlm
catch stop
run
# Use 'bt' to see stack trace
# Use 'continue' to resume
```

### Logging Levels
NLM provides several logging levels:
- `Debug`: Detailed debugging information
- `Info`: General information about program flow
- `Warning`: Warnings about potential issues
- `Error`: Errors that prevent operation
- `Critical`: Critical failures

Enable debug logging:
```python
import pynlm
pynlm.Logger::getGlobal().setLevel(nlm.LogLevel.Debug)
```

### Profiling
```bash
# With CMake profiling support
make profiled
./nlm --profile
```

## License

This project is licensed under the MIT License. See `LICENSE` file for details.

## Code of Conduct

Please treat everyone with respect and follow the project's code of conduct.

## Acknowledgments

This project builds on years of research in computational neuroscience and artificial intelligence. The development team thanks all contributors and research partners.

## Getting Help

If you have questions or need assistance:

1. Check the documentation in the `docs/` directory
2. Join the NLM Discord community (if available)
3. File issues on GitHub with detailed descriptions
4. Check existing issues for similar problems

---

*Last updated: September 12, 2026*
