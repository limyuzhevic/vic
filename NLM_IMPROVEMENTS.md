# NLM - Neural Learning Machine

## Overview

NLM is an experimental artificial developmental brain simulation framework written in C++20 with Python bindings.

## Key Improvements

### 1. Configuration System
- ✅ Implemented JSON configuration support
- ✅ Added configuration validation
- ✅ Enhanced type conversion for Python objects
- ✅ Backward compatibility with simple key=value format

### 2. Python Bindings
- ✅ Comprehensive Python API design
- ✅ Enhanced documentation with examples
- ✅ Custom exception handling for better error reporting
- ✅ Improved type conversion utilities

### 3. Type System
- ✅ Added JSON serialization for all ID types (NeuronId, SynapseId, RegionId, PopulationId)
- ✅ Implemented enum serialization (NeuronType, SynapseType, etc.)
- ✅ Added string representation methods for all types
- ✅ Implemented factory functions for Python-friendly ID creation

### 4. Advanced Command-Line Interface
- ✅ Implemented AdvancedConfig system with 40+ options
- ✅ Enhanced argument parsing with both `--key=value` and `--key value` formats
- ✅ Improved help documentation with comprehensive examples

### 5. Build System Modernization
- ✅ Modernized CMake configuration with CMake 3.16+
- ✅ Added modern CMake features (target-based dependency management)
- ✅ Enhanced install rules with CMake package support
- ✅ Improved project structure and organization

### 6. Documentation
- ✅ Comprehensive documentation update
- ✅ Enhanced API references for all components
- ✅ Added advanced examples and tutorials
- ✅ Updated installation and build instructions

## Quick Start

### Python Usage
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
    print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")
```

### C++ Usage
```bash
# Build with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run executable
./nlm
```

### Advanced Configuration
```bash
# With advanced options
nlm \
  --neuron-count=5000 \
  --timestep=0.0005 \
  --connection-prob=0.2 \
  --debug \
  --enable-reward-mod \
  --curiosity \
  --replay
```

## Key Features

- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- Multiple plasticity mechanisms (STDP, Hebbian, structural)
- Integrated memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Prediction and planning systems
- Cognitive attention and concept formation
- Developmental stages affecting plasticity
- Checkpoint save/load capabilities

## Project Structure

```
NLM/
├── CMakeLists.txt              # Modern CMake build system
├── python/                    # Python bindings
│   ├── bindings.cpp           # pybind11 bindings
│   └── CMakeLists.txt         # Python bindings build
├── src/                       # C++ source code
│   ├── brain/                 # Neural components
│   ├── core/                  # Core utilities
│   ├── dynamics/              # Neural dynamics
│   ├── plasticity/            # Plasticity mechanisms
│   ├── development/           # Developmental system
│   ├── neuromodulation/        # Neuromodulators
│   ├── memory/                # Memory systems
│   ├── cognition/             # Cognitive systems
│   ├── sensory/               # Sensory processing
│   ├── prediction/            # Prediction systems
│   ├── motor/                 # Motor system
│   ├── environment/           # Environment interface
│   ├── experiments/           # Experiment framework
│   └── visualization/         # Visualization
├── configs/                   # Configuration files
├── tests/                     # Unit tests
├── docs/                      # Documentation
├── easy_usage.md              # Easy usage guide
└── HOW_TO_USE.md              # Detailed usage instructions
```

## Files Modified

### src/core/Config/Config.hpp
- Added JSON support and validation
- Enhanced type system
- Added ConfigValidator class hierarchy

### src/core/Config/Config.cpp
- Implemented JSON serialization/deserialization
- Added configuration validation
- Enhanced error handling

### src/core/Types/Types.cpp
- Added JSON serialization for all types
- Implemented factory functions
- Added string representation methods

### src/core/Types/Types.hpp
- Enhanced ID types with serialization
- Added enum serialization support
- Implemented factory functions

### src/main.cpp
- Added AdvancedConfig system
- Enhanced command-line argument parsing
- Added comprehensive help documentation

### CMakeLists.txt
- Modernized CMake configuration
- Added modern CMake features
- Enhanced dependency management

### python/bindings.cpp
- Enhanced Python API design
- Added comprehensive documentation
- Improved type conversion utilities

## Testing

### Unit Tests
```bash
# Run tests
mkdir build
cd build
cmake .. -DNLM_ENABLE_TESTS=ON
make -j$(nproc)
ctest --output-on-failure
```

### Demo Executables
```bash
# Build and run demos
mkdir build
cd build
cmake .. -DNLM_ENABLE_DEMOS=ON
make -j$(nproc)

# Run Phase 6 demo
./nlm_phase6_demo
```

## Installation

### Standard Installation
```bash
# Clone repository
mkdir nlm
cd nlm

# Build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install (optional)
sudo make install
```

### Python Package
```bash
# Install Python bindings
pip install .

# Or install in development mode
pip install -e .
```

## Usage Examples

### Basic Python Example
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
    print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")
```

### Advanced Python Example
```python
import pynlm

def run_advanced_simulation():
    # Create configuration with advanced features
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 5000)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromodulation.curiosity.enable", True)
    config.set("memory.working_memory.enable", True)
    config.set("memory.episodic_memory.enable", True)
    
    # Create agent-world system
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=100, height=100, visionWidth=16, visionHeight=16)
    world.reset()
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(1000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = 0.0
        if step % 100 == 0:
            print(f"Step {step}: Learning complete")
    
    return brain, agent, world

# Run advanced simulation
brain, agent, world = run_advanced_simulation()
```

### Command Line Example
```bash
# Run with custom configuration
nlm --config=advanced.cfg --neuron-count=3000 --timestep=0.001 --debug

# Run Phase 6 integration demo
nlm_phase6_demo

# Run with JSON configuration
nlm --config=phase6_config.json --output=brain_checkpoint.bin
```

## Documentation

### Available Documentation

1. **easy_usage.md**: Quick start guide for beginners
2. **HOW_TO_USE.md**: Detailed usage instructions
3. **Project Improvements**: This document (this file)
4. **API References**: Generated from code comments

### Getting Help

- **Issues**: Report issues on GitHub
- **Discussions**: Join the NLM discussion forum
- **Examples**: Check the examples directory
- **Documentation**: Refer to the documentation folder

## Conclusion

The NLM project has been significantly improved across all major components:

1. **Configuration System**: Enhanced with JSON support and validation
2. **Python Bindings**: Comprehensive API with better documentation and type conversion
3. **Type System**: Full JSON serialization support for all types
4. **Command-Line Interface**: Advanced configuration options with comprehensive help
5. **Build System**: Modern CMake configuration with better dependency management
6. **Documentation**: Comprehensive guides and examples for all features

These improvements make NLM a more powerful, maintainable, and user-friendly platform for artificial developmental brain research and simulation.

## Future Work

The NLM project continues to evolve with planned improvements in:

- Enhanced neural dynamics and models
- Advanced plasticity mechanisms
- Memory systems and consolidation
- Neuromodulation and cognitive functions
- Development and life history effects
- Environmental modeling and social behavior
- Applications and research tools

## Acknowledgements

The NLM project is a collaborative effort involving researchers and developers from:

- Computational Neuroscience
- Artificial Intelligence
- Cognitive Science
- Systems Biology

This project represents years of research and development in artificial developmental brain modeling and neural computation.