# Python Bindings for NLM - Placeholder

This project is intended to have Python bindings for the NLM (Neural Learning Machine) C++ framework, but they are not yet implemented.

## Current Status

The NLM project is a C++ brain simulation framework that includes:

### C++ Executables
- `nlm` - Main simulation executable (Phase 2)
- `nlm_phase3_demo` - Phase 3 world interaction demo
- `nlm_phase4_demo` - Phase 4 cognition experiments
- `nlm_phase5_demo` - Phase 5 personality/sandbox demo
- `nlm_phase6_demo` - Phase 6 integration test
- `nlm_benchmark` - Performance benchmark

### C++ Libraries
- Core neural simulation components
- Multiple brain systems (memory, prediction, cognition, neuromodulation, etc.)
- Various plasticity mechanisms (STDP, Hebbian, structural)

### Python Interface
**Not yet implemented**

The project is configured with `pyproject.toml` to build Python bindings using pybind11 and scikit-build-core, and the documentation (easy_usage.md, HOW_TO_USE.md) describes Python usage. However, there are no actual Python binding files.

## What This Project Should Have

A proper Python bindings implementation would include:

### Python Module Structure
```python
pynlm/
├── __init__.py
├── nlm.py          # Main API
├── config.py       # Configuration
├── brain.py        # Brain class
├── neuron.py       # Neuron class
├── synapse.py      # Synapse class
├── world.py        # World simulation
├── agent.py        # Agent interface
└── tests/          # Python tests
```

### Core Classes Exposed to Python
- `createBrain(config)` - Create a virtual brain
- `createSimpleWorld()` - Create a simple world simulation
- `createAgentBrain(brain)` - Create an agent that controls a brain
- Various experiment classes for different phases

### Usage Example (Currently Documented but Not Implemented)
```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it think
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

## Why This Matters

Having proper Python bindings is important because:

1. **Accessibility**: Python is more accessible than C++ for many developers
2. **Ease of use**: Python provides a simpler API for common operations
3. **Research integration**: Python is commonly used in AI/ML research
4. **Testing**: Python is easier for testing and prototyping
5. **Community**: Python has a larger user base and more community support

## Next Steps

To implement Python bindings:

1. **Create binding files** using pybind11 to expose C++ functionality to Python
2. **Implement the main API** classes and functions
3. **Update documentation** to reflect the actual implementation
4. **Add tests** for the Python bindings
5. **Configure build system** to properly build the Python module

## Alternative: Focus on C++ First

If Python bindings are not a priority, consider:

1. **Remove Python references** from documentation
2. **Update pyproject.toml** to reflect C++ project structure
3. **Keep easy_usage.md** as Python usage guide for those who want to use Python bindings (when implemented)
4. **Create a separate Python project** for the bindings (outside this repository)

## Conclusion

The Python bindings are a planned feature that would significantly improve the project's accessibility and usability. Until they are implemented, users should use the C++ executables and libraries directly, or wait for the Python bindings to be completed.
