# NLM (Neural Learning Machine) - Project Documentation

An Experimental Artificial Developmental Brain Simulator

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Documentation Structure

### Quick Start
- [easy_usage.md](./easy_usage.md) - Beginner-friendly usage guide with practical examples
- [QUICK_REFERENCE.md](./docs/QUICK_REFERENCE.md) - Essential commands and patterns reference

### Advanced Usage
- [HOW_TO_USE.md](./HOW_TO_USE.md) - Comprehensive guide including building and installation
- [ADVANCED_USAGE.md](./docs/ADVANCED_USAGE.md) - Advanced features and power user techniques

### Technical Architecture
- [docs/ARCHITECTURE.md](./docs/ARCHITECTURE.md) - System architecture and design patterns
- [docs/SCIENCE.md](./docs/SCIENCE.md) - Scientific background and theoretical foundations

### Project Information
- [README.md](./README.md) - This file - project overview and current status
- [CHANGELOG.md](./CHANGELOG.md) - Development history and improvements
- [LICENSE](./LICENSE) - Project license

## Current Status: Phase 6 - Final Integration

✅ **Phase 6: FINAL INTEGRATION** - Complete

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- ✅ Working memory with persistent activity and competition
- ✅ Episodic memory with experience encoding and replay
- ✅ Associative memory with Hebbian pattern associations
- ✅ All memory systems connected to neural processing

#### Neuromodulation Integration
- ✅ Dopamine affects neural excitability and plasticity
- ✅ Curiosity drives exploration behavior
- ✅ Novelty detection integrated with sensory processing
- ✅ All neuromodulators connected to plasticity rules

#### Prediction System
- ✅ Prediction system integrated into brain loop
- ✅ Prediction error signals affect learning
- ✅ Confidence tracking implemented

#### Cognition Systems
- ✅ Neural planner with action sequence evaluation
- ✅ Concept formation from experience patterns
- ✅ Attention with competitive selection dynamics
- ✅ All cognition systems connected to perception and action

#### Development Integration
- ✅ Developmental stages affect plasticity rates
- ✅ Structural plasticity modulated by age
- ✅ Neural excitability changes with development

#### Persistence
- ✅ Checkpoint save/load implemented
- ✅ Brain state serialization working
- ✅ Can resume from saved checkpoints

#### Replay and Consolidation
- ✅ Episodic memory replay during simulation
- ✅ Memory consolidation for important episodes
- ✅ Integration with sleep/rest cycle

## Key Improvements in This Release

### 🐛 Bug Fixes
- **Fixed Configuration Bug**: Resolved duplicate `dopamine_baseline` in `configs/default.cfg`

### 🚀 Python API Enhancements
- **Added Convenience Methods**: 15+ new methods for common operations
- **Enhanced Error Handling**: Custom exception types with detailed error messages
- **Type Safety**: Improved parameter validation and type checking
- **Batch Operations**: Performance-optimized batch processing methods
- **Advanced Features**: 20+ new advanced methods for power users

### 📚 Documentation Improvements
- **Enhanced Beginner Guide**: Added practical tips and troubleshooting
- **Advanced User Guide**: Comprehensive coverage of advanced features
- **Quick Reference**: Essential commands and patterns for quick access
- **Integrated Documentation**: Better organization and cross-references

### 🛠️ Development Tools
- **Configuration Management**: Advanced config validation and templates
- **Performance Monitoring**: Comprehensive performance tracking
- **Export/Import**: Brain state export for analysis
- **Behavioral Strategies**: Predefined behavioral control systems
- **Dynamic Environment Creation**: Procedural content generation

## Quick Start Examples

### Beginner Example
```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Agent-Environment Example
```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation finished!")
```

## Building and Installation

### Prerequisites
- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps
```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Installing Python Bindings
```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── configs/              # Configuration files
│   └── default.cfg
├── docs/                 # Documentation
│   ├── ADVANCED_USAGE.md
│   ├── ARCHITECTURE.md
│   ├── HOW_TO_USE.md
│   ├── QUICK_REFERENCE.md
│   └── ... (other documentation)
├── python/              # Python bindings
│   ├── bindings.cpp     # Enhanced Python bindings
│   └── CMakeLists.txt
├── src/                 # C++ source code
│   ├── brain/           # Neural components
│   ├── agent/           # Agent systems
│   ├── world/           # World simulation
│   ├── memory/          # Memory systems
│   ├── plasticity/      # Plasticity rules
│   ├── development/     # Developmental system
│   ├── neuromodulation/ # Neuromodulators
│   ├── prediction/      # Prediction systems
│   ├── cognition/      # Cognitive mechanisms
│   ├── sensory/         # Sensory processing
│   ├── motor/           # Motor system
│   ├── environment/     # Environment interface
│   └── experiments/     # Experiment framework
├── tests/              # Test suite
└── build/              # Build directory (generated)
```

## Getting Help

### Documentation
- **Quick Start**: [easy_usage.md](./easy_usage.md) or [QUICK_REFERENCE.md](./docs/QUICK_REFERENCE.md)
- **Advanced Usage**: [ADVANCED_USAGE.md](./docs/ADVANCED_USAGE.md)
- **Technical Details**: [docs/ARCHITECTURE.md](./docs/ARCHITECTURE.md)

### Community
- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Documentation**: https://nlm.readthedocs.io
- **Issues**: Report bugs and request features

### Support
For questions about using NLM, please refer to the documentation first. The documentation includes:

- Comprehensive examples for all major use cases
- Troubleshooting sections for common issues
- Detailed API references
- Advanced usage patterns for power users

## Scientific Disclaimer

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

## License

MIT License - See [LICENSE](./LICENSE) for details.

## Contributors

Research project — See individual source files for author information.

## Acknowledgements

Thanks to all contributors and research team members who have helped develop NLM.

---

*Last updated: $(date +%Y-%m-%d)*
*Version: 0.1.0*
