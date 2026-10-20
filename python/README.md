# NLM Python Bindings

This directory contains the Python bindings for the NLM (Neural Learning Machine) C++ neural simulation framework.

## Building the Bindings

### Prerequisites
- CMake 3.16+
- C++ compiler with C++20 support
- Python development headers
- pybind11

### Build Instructions

#### Standard Build
```bash
mkdir build
cd build
cmake ..
make -j4
```

#### Install as Python Package
```bash
pip install -e .
```

### Testing

#### Run C++ Tests
```bash
mkdir build_test
cd build_test
cmake ..
make
ctest
```

## Python API Examples

### Basic Usage
```python
import pynlm as nlm

# Create default configuration
config = nlm.createDefaultConfig()

# Create brain
brain = nlm.createBrain(config)

# Create world
world = nlm.createSimpleWorld(100, 100, 20, 20)

# Create agent brain
agent_brain = nlm.createAgentBrain(brain)
agent_brain.initialize(world)

# Run simulation
reward = nlm.runEpisode(agent_brain, world, 1000)
print(f"Final reward: {reward}")
```

### Advanced Features
```python
import pynlm as nlm

# Access all neuromodulation systems
brain = nlm.createBrain(nlm.createDefaultConfig())

# Neuromodulation
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()

# Development
dev_system = brain.getDevelopmentSystem()
dev_system.setStage(nlm.DevelopmentalStage.CriticalPeriod)

# Reward system
reward = nlm.Reward()
reward.setValue(1.5)

# Advanced utilities
stats = nlm.getBrainStats(brain)
print(f"Brain statistics: {stats}")
```

## Files

- `bindings.cpp` - Main Python bindings implementation
- `bindings.cpp.backup` - Backup of previous version
- `CMakeLists.txt` - CMake build configuration
- `examples/` - Example scripts and documentation
- `tests/` - C++ unit tests

## Key Improvements

### 1. Complete Functionality
- ✅ Reward system bindings
- ✅ Neuromodulation system bindings
- ✅ Development system bindings
- ✅ Curiosity system bindings
- ✅ Novelty detection bindings
- ✅ Prediction error bindings

### 2. Comprehensive Error Handling
- ✅ Custom exception classes
- ✅ Parameter validation
- ✅ Runtime error checking
- ✅ Comprehensive documentation

### 3. Advanced Features
- ✅ Learning rate configuration
- ✅ Memory management utilities
- ✅ Performance monitoring
- ✅ State serialization
- ✅ Simulation utilities

### 4. API Design Improvements
- ✅ Helper functions for common operations
- ✅ Comprehensive Python docstrings
- ✅ Type hints
- ✅ Consistent method signatures
- ✅ Examples and documentation

### 5. Documentation
- ✅ Comprehensive API reference
- ✅ Installation and usage guides
- ✅ Example scripts
- ✅ Troubleshooting section
- ✅ Migration guide

## Version Information

- Python bindings version: 0.1.0
- C++ API version: Compatible with NLM framework
- Build system: CMake + pybind11
- Python support: 3.8+

## License

MIT License
