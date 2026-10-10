# NLM - Neural Learning Machine

An experimental artificial developmental brain project with significant improvements for user experience and code quality.

---

## 🚀 What's New

### Major Improvements

1. **Fixed Critical Memory Leak** - Config class Pimpl pattern properly implemented
2. **Enhanced Python API** - New convenience functions make getting started trivial
3. **Improved Documentation** - Comprehensive guides and examples
4. **Better Error Handling** - Robust file I/O and configuration management
5. **JSON/YAML Support** - Modern configuration file formats

### Key Features

- ✅ **Zero Memory Leaks** - Fixed all Pimpl pattern issues
- ✅ **Easy Python API** - One-liner brain creation available
- ✅ **Rich Learning Capabilities** - Memory, plasticity, development, neuromodulation
- ✅ **Phase 6 Complete** - All systems integrated and functional
- ✅ **Production Ready** - Build system, testing, documentation

---

## 📖 Quick Start

### Method 1: One-Liner (Recommended)

```python
import nlm

# Create and initialize a brain with sensible defaults
brain = nlm.create_default_brain()
print(f"Brain created with {brain.getTotalNeuronCount()} neurons!")
```

### Method 2: Complete Example

```python
import nlm

# Create custom brain
config = nlm.create_default_config_with_params(
    neuron_count=2000,
    region_count=3
)
brain = nlm.create_default_brain(config)

# Set up world and agent
world = nlm.create_simple_world()
world.configure(30, 30, 12, 12)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Run learning episode
for step in range(100):
    world.update(timestep=0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())

# See results
print(nlm.visualize_brain_state(brain))
```

---

## 🛠️ Technical Improvements

### 1. Config Class (Memory Leak Fixed)

**Before:** Serious memory leak in Pimpl implementation
**After:** Properly implemented with JSON/YAML support

```cpp
// Fixed implementation with proper memory management
class Config {
public:
    Config();
    ~Config();
    
    // Load from JSON/YAML or key=value format
    bool loadFromFile(const std::string& filepath);
    
    // Proper template implementations with error handling
    template<typename T>
    std::optional<T> get(const std::string& key) const;
};
```

### 2. Python Convenience Functions

**Added High-Level API:**

| Function | Purpose | Example |
|----------|---------|---------|
| `create_default_config()` | Default config | `config = nlm.create_default_config()` |
| `create_default_config_with_params()` | Custom config | `config = nlm.create_default_config_with_params(1000, 2)` |
| `create_default_brain()` | Default brain | `brain = nlm.create_default_brain()` |
| `run_simple_learning_experiment()` | Quick test | `result = nlm.run_simple_learning_experiment(brain, world, 500)` |
| `visualize_brain_state()` | See brain stats | `viz = nlm.visualize_brain_state(brain)` |

### 3. Enhanced Build System

**Improved Installation:**

```bash
# Install with scikit-build-core (modern Python packaging)
pip install pybind11 scikit-build-core
pip install -e .  # Development mode

# Or build from source
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

---

## 📚 Documentation

### Main Guides

1. **NLM_PYTHON_QUICK_START.md** - Quick start for Python users
2. **HOW_TO_USE.md** - Complete API reference and examples
3. **BUILD.md** - Build and installation instructions
4. **easy_usage.md** - Beginner-friendly usage guide

### Technical Documentation

- **docs/ARCHITECTURE.md** - System architecture and components
- **docs/SCIENCE.md** - Scientific background and models
- **docs/ROADMAP.md** - Development roadmap and phases
- **docs/EXPERIMENTS.md** - Experiment descriptions and results

---

## 🧪 Examples

### Simple Brain

```python
import nlm

# One line to create a functioning brain
brain = nlm.create_default_brain()
brain.initialize()

# Brain thinks and learns
for step in range(10):
    brain.step(step)

print(f"Brain created {brain.getTotalSpikeCount()} spikes!")
```

### Learning Agent

```python
import nlm

# Create learning system
brain = nlm.create_default_brain()
world = nlm.create_simple_world()
agent = nlm.createAgentBrain(brain)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run learning episode
result = nlm.run_simple_learning_experiment(brain, world, 100)
print(result)
```

### Complete Demo

```bash
# Run the comprehensive demo
python3 nlm_demo.py
```

---

## 🏗️ Project Structure

```
NLM/
├── src/                    # C++ source code
│   ├── brain/              # Neural computation
│   ├── memory/             # Memory systems
│   ├── cognition/          # Cognitive functions
│   ├── plasticity/         # Learning rules
│   └── ...                 # Other components
│
├── python/                 # Python bindings
│   └── bindings.cpp        # Pybind11 interface
│
├── docs/                   # Documentation
│   ├── ARCHITECTURE.md     # System design
│   ├── SCIENCE.md         # Scientific background
│   └── ...                # Other docs
│
├── nlm_demo.py            # Demo script
├── NLM_PYTHON_QUICK_START.md  # Quick start guide
├── HOW_TO_USE.md           # API documentation
├── BUILD.md                # Build instructions
├── easy_usage.md           # Beginner guide
└── ...                     # Other files
```

---

## 📊 Key Statistics

### Phase Status
- **Phase 1**: ✅ Complete - Project skeleton and core types
- **Phase 2**: ✅ Complete - Real neural computation (LIF, STDP, Hebbian)
- **Phase 3**: ✅ Complete - World interaction and learning
- **Phase 4**: ✅ Complete - Cognitive functions (attention, planning, etc.)
- **Phase 5**: ✅ Complete - Performance optimizations
- **Phase 6**: ✅ Complete - Final integration (current state)

### Project Metrics
- **C++ Files**: ~40 source files
- **Python Bindings**: 1 file with 747 lines (including new convenience functions)
- **Tests**: Comprehensive test suite
- **Documentation**: 5+ guides and examples

### Learning Capabilities
- **Working Memory**: Persistent activity and competitive selection
- **Episodic Memory**: Experience encoding and replay
- **Associative Memory**: Pattern associations
- **Prediction System**: Temporal sequence learning
- **Neuromodulation**: Dopamine, curiosity, novelty
- **Development**: Age-dependent plasticity and maturation

---

## 🎯 Design Principles

1. **No ML Frameworks** - Implements neural computation from scratch
2. **No Transformers** - Uses spiking dynamics and competitive attention
3. **Biological Inspiration** - Incrementally more accurate models
4. **Modularity** - Clear interfaces between components
5. **Determinism** - Reproducible experiments via explicit seeds
6. **Performance** - Data-oriented design for large-scale simulation
7. **Extensibility** - Clear patterns for adding new components
8. **No Predefined Knowledge** - All learning from experience

---

## 🚀 Getting Started

### 1. Install Dependencies

```bash
# Python packages
pip install pybind11 scikit-build-core numpy pytest

# Build and install
pip install -e .
```

### 2. Verify Installation

```python
import nlm

# Test the convenience functions
brain = nlm.create_default_brain()
print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")

world = nlm.create_simple_world()
print("✓ World created")

config = nlm.create_default_config()
print("✓ Config created")
```

### 3. Run Examples

```bash
# Run the demo script
python3 nlm_demo.py

# View documentation
less NLM_PYTHON_QUICK_START.md
```

---

## 🔧 Troubleshooting

### Common Issues

**"ImportError: No module named 'pynlm'"**
```bash
pip install --force-reinstall .
```

**"CMake cannot find Python"**
```bash
pip install pybind11 scikit-build-core
cmake .. -DPYTHON_EXECUTABLE=$(which python3)
```

**"Brain not firing neurons"**
```python
# Inject some input
brain = nlm.create_default_brain()
brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 10.0)
```

**Memory usage too high**
```python
# Use smaller configuration
brain = nlm.create_default_brain()
brain.config.set("neuron_count", 500)
```

---

## 📈 Performance Tips

1. **For Testing**: Use `neuron_count=500`, `simulation_timestep=0.01`
2. **For Development**: Enable checkpoints frequently
3. **For Production**: Use `neuron_count=5000+`, optimization enabled
4. **Memory**: Use `brain.save()` to checkpoint long experiments

---

## 📝 Version Information

- **Phase**: 6 (Final Integration)
- **Status**: Production Ready
- **License**: MIT
- **Documentation**: Comprehensive guides included

---

## ✨ Final Notes

NLM is a research project investigating computational brain-like systems. We make **NO claims** that NLM accurately reproduces biological brains, but it provides a flexible framework for experimenting with artificial developmental systems.

Key achievements:
- ✅ Fixed all memory leaks
- ✅ Made Python API beginner-friendly
- ✅ Added comprehensive documentation
- ✅ Improved error handling and robustness
- ✅ Maintained backward compatibility
- ✅ Enhanced learning and adaptation capabilities

Enjoy experimenting with your artificial brain!

---

*Last Updated: $(date +%Y-%m-%d)*
