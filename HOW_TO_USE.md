# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

**Version:** 2.0.0 (Phase 6: Final Integration)
**Enhanced Features:** JSON/YAML configuration, schema validation, advanced error handling

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the enhanced NLM Python API
4. Advanced configuration and validation

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher (with nlohmann_json support)
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
# Note: Enhanced CMake configuration with JSON support
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs (Enhanced)

After successful compilation, you will have (NEW in version 2.0.0):

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable (Phase 6 integration) |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase6_demo` | Executable | **NEW: Phase 6 integration demo** |
| `nlm_test` | Executable | **REPLACED: Legacy test suite (deprecated)** |
| `nlm_enhanced_test` | Executable | **NEW: Comprehensive enhanced test suite** |

### Build Options (Enhanced)

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Build with JSON schema validation support
cmake .. -DENABLE_JSON_SCHEMA_VALIDATION=ON

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy nlohmann-json

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl

# Verify installation
python -c "import pynlm; print(f'NLM Version: {pynlm.__version__}')"
```

---

## Part 3: Using the Enhanced NLM Python API

### Basic Brain Simulation (Unchanged)

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)

print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
```

### Enhanced Configuration with JSON Support (NEW)

```python
import pynlm

# Create enhanced configuration
config = pynlm.createDefaultConfig()

# Set advanced parameters using JSON-style keys
config.set("brain.neuron_count", 1500)
config.set("brain.synapse_density", 0.12)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.prediction.enable", True)
config.set("development.enable", True)

# Save to JSON file
config.saveToFile("enhanced_config.json")

# Load and validate with schema
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("enhanced_config.json")

# Example schema for validation
schema = {
    "type": "object",
    "properties": {
        "brain.neuron_count": {"type": "integer", "minimum": 100},
        "brain.synapse_density": {"type": "number", "minimum": 0.0, "maximum": 1.0},
        "plasticity.stdp.enable": {"type": "boolean"}
    },
    "required": ["brain.neuron_count", "brain.synapse_density"]
}

if config2.validateSchema(schema):
    print("✓ Configuration is valid!")
    print(f"✓ Loaded brain with {config2.getOr('brain.neuron_count', 1000)} neurons")
else:
    print("✗ Configuration validation failed!")
```

### Advanced Agent Simulation (Enhanced)

```python
import pynlm

def run_enhanced_agent_simulation(steps=500):
    """Run a complete enhanced agent simulation with brain and world."""
    
    # 1. Create and configure brain with enhanced parameters
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("brain.synapse_density", 0.12)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.prediction.enable", True)
    config.set("development.enable", True)
    config.set("checkpoint.enable", True)
    config.set("replay.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 3. Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 4. Run simulation with enhanced monitoring
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print enhanced progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
            print(f"  Development stage: {agent.getDevelopmentalStage()}")
            print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
    
    print(f"✓ Enhanced agent simulation complete after {steps} steps!")
    
    return {
        "total_steps": steps,
        "final_firing_rate": brain.getAverageFiringRate(),
        "development_stage": agent.getDevelopmentalStage(),
        "neuromodulation_level": agent.getNeuromodulationLevel(),
        "curiosity_level": agent.getCuriosityLevel(),
        "novelty_level": agent.getNoveltyLevel()
    }

# Run the enhanced simulation
results = run_enhanced_agent_simulation(1000)
print(f"\n🎉 Enhanced Simulation Results: {results}")
```

### Configuration Management and Validation (NEW)

```python
import pynlm
import json

# Create example configuration with validation
config = pynlm.createDefaultConfig()

# Set Phase 6 parameters
phase6_params = {
    "brain.neuron_count": 1000,
    "brain.synapse_density": 0.1,
    "plasticity.stdp.enable": True,
    "plasticity.stdp.learning_rate": 0.001,
    "plasticity.hebbian.enable": True,
    "plasticity.structural.enable": True,
    "neuromod.dopamine.scale": 1.0,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True,
    "neuromod.prediction.enable": True,
    "development.enable": True,
    "checkpoint.enable": True,
    "replay.enable": True
}

# Apply all parameters
for key, value in phase6_params.items():
    config.set(key, value)

# Export to JSON with formatting
json_str = config.toJsonString()
config_dict = json.loads(json_str)
print("📄 Configuration exported to JSON:")
print(json.dumps(config_dict, indent=2))

# Import from JSON
config2 = pynlm.createDefaultConfig()
config2.loadFromJsonString(json_str)

# Merge configurations (new feature)
base_config = pynlm.createDefaultConfig()
extended_config = pynlm.createDefaultConfig()
extended_config.set("brain.neuron_count", 2000)  # Override
extended_config.set("plasticity.enable", True)  # Add

# Merge - extended takes precedence
base_config.merge(extended_config)
print(f"🔗 Merged configuration has {len(base_config.getKeys())} settings")

# Schema validation example
schema = {
    "type": "object",
    "properties": {
        "required_field": {"type": "string"},
        "optional_field": {"type": "number"}
    },
    "required": ["required_field"]
}

config.set("required_field", "test")
config.set("optional_field", 3.14)

if config.validateSchema(schema):
    print("✅ Schema validation passed")
else:
    print("❌ Schema validation failed")
```

---

## Part 4: Advanced NLM Features

### 1. Configuration Schema Validation (NEW)

The enhanced configuration system supports JSON Schema validation:

```python
# Complex schema for a complete configuration
schema = {
    "type": "object",
    "properties": {
        "brain": {
            "type": "object",
            "properties": {
                "neuron_count": {"type": "integer", "minimum": 100},
                "synapse_density": {"type": "number", "minimum": 0.0, "maximum": 1.0}
            },
            "required": ["neuron_count", "synapse_density"]
        },
        "plasticity": {
            "type": "object",
            "properties": {
                "stdp": {
                    "type": "object",
                    "properties": {
                        "enable": {"type": "boolean"},
                        "learning_rate": {"type": "number", "minimum": 0.0}
                    }
                },
                "hebbian": {"type": "object", "properties": {"enable": {"type": "boolean"}}},
                "structural": {"type": "object", "properties": {"enable": {"type": "boolean"}}}
            }
        }
    },
    "required": ["brain", "plasticity"]
}

# Validate configuration
if config.validateSchema(schema):
    print("✅ Complex schema validation passed")
else:
    print("❌ Complex schema validation failed")
```

### 2. Configuration Merging and Inheritance (NEW)

Configurations can be merged to inherit settings with precedence:

```python
# Base configuration
base_config = pynlm.createDefaultConfig()
base_config.set("brain.neuron_count", 1000)
base_config.set("brain.synapse_density", 0.1)

# Extended configuration (overrides and adds)
extended_config = pynlm.createDefaultConfig()
extended_config.set("brain.neuron_count", 2000)  # Override
extended_config.set("plasticity.stdp.enable", True)  # Add
extended_config.set("neuromod.curiosity.enable", True)  # Add

# Merge configurations
base_config.merge(extended_config)

# Result validation
print(f"✓ Neuron count: {base_config.getOr('brain.neuron_count', 1000)} (should be 2000)")
print(f"✓ Synapse density: {base_config.getOr('brain.synapse_density', 0.1)} (should be 0.1)")
print(f"✓ STDP enabled: {base_config.getOr('plasticity.stdp.enable', False)} (should be True)")
print(f"✓ Curiosity enabled: {base_config.getOr('neuromod.curiosity.enable', False)} (should be True)")
```

### 3. Performance Monitoring and Benchmarking (ENHANCED)

The enhanced system includes performance monitoring:

```python
import time

# Monitor configuration loading performance
start_time = time.time()
config = pynlm.createDefaultConfig()
for i in range(1000):
    config.set(f"test.key{i}", i)
end_time = time.time()

config_time = (end_time - start_time) * 1000
print(f"⚡ Configuration operations: {config_time:.2f} ms")

# Monitor brain simulation performance
start_time = time.time()
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(100):
    brain.step(step)
end_time = time.time()

brain_time = (end_time - start_time) * 1000
print(f"🧠 Brain simulation (100 steps): {brain_time:.2f} ms")
print(f"📊 Average step time: {brain_time/100:.2f} ms")
```

---

## Part 5: Running Tests

### Running Enhanced Test Suite

```bash
# Run comprehensive enhanced test suite
./nlm_enhanced_test

# Or with CMake
make nlm_enhanced_test
ctest -R nlm_enhanced_test
```

### Test Results

The enhanced test suite includes:

**Core Functionality Tests:**
- Configuration system tests (JSON/YAML, schema validation)
- Brain initialization and basic functionality
- Neural component tests (neurons, synapses, plasticity)
- Random number generation and simulation clock

**Phase 6 Integration Tests:**
- Memory system integration (working + episodic)
- Neuromodulation integration (dopamine, curiosity, novelty)
- Prediction system integration
- Development system integration
- Checkpoint save/load functionality
- Memory replay and consolidation

**Enhanced Feature Tests:**
- Configuration schema validation
- Configuration merging and inheritance
- Error handling and recovery
- Performance monitoring
- JSON/YAML file format support

---

## Troubleshooting

### Common Issues and Solutions

**Q: "JSON schema validation failed"**
```
# Solution: Check schema and configuration values
schema = {
    "type": "object",
    "properties": {
        "brain.neuron_count": {"type": "integer", "minimum": 100}
    }
}

config.set("brain.neuron_count", 50)  # Too small
if not config.validateSchema(schema):
    print("❌ Validation failed - neuron count should be >= 100")
```

**Q: "Python bindings installation issue"**
```
# Solution: Check installation and dependencies
pip install pynlm
python -c "import pynlm; print('NLM Python bindings loaded successfully')"
```

**Q: "Configuration file not found"**
```
# Solution: Check file path and create example
config.loadFromFile("my_config.json")
# If file doesn't exist, use default
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
```

**Q: "Phase 6 integration demo not working"**
```
# Solution: Ensure all dependencies are built
make nlm_phase6_demo
./nlm_phase6_demo
```

### Additional Resources

- **Documentation**: Visit the NLM project documentation for detailed API references
- **Issue Tracker**: Report bugs and feature requests
- **Community**: Join the NLM community forums for discussions
- **Examples**: Check the examples directory for additional usage patterns

---

## License

MIT License

Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

---

## Version Information

**Version:** 2.0.0 (Phase 6: Final Integration)
**Status:** Production Ready
**Build:** Enhanced with JSON/YAML configuration support
**API:** Comprehensive Python bindings with validation
**Tests:** Comprehensive testing framework with schema validation

### Key Enhancements in Version 2.0.0:

1. **JSON/YAML Configuration System**: Full JSON configuration file support with schema validation
2. **Advanced Configuration**: Configuration merging, inheritance, and validation
3. **Error Handling**: Comprehensive error handling and recovery mechanisms
4. **Enhanced Python API**: Advanced Python bindings with performance optimizations
5. **Comprehensive Testing**: Enhanced test suite with JSON configuration tests
6. **Phase 6 Integration**: Complete integration of all Phase 6 systems
7. **Performance Monitoring**: Performance monitoring and benchmarking capabilities
8. **Documentation**: Enhanced documentation with examples and best practices
9. **Production Ready**: Ready for research and production deployment

---

## Conclusion

NLM (熙然) version 2.0.0 represents a significant milestone in the development of artificial neural systems. With complete integration of all systems, enhanced configuration management, and comprehensive testing, NLM is now ready for advanced research and production deployment.

The enhanced features provide researchers and developers with powerful tools for:

1. **Building complex neural architectures** with configurable components
2. **Testing and validating neural systems** with comprehensive test suites
3. **Deploying neural systems** in production environments with robust error handling
4. **Research and experimentation** with advanced configuration management

The NLM project continues to evolve, and we welcome contributions from the community to further enhance this artificial neural learning machine.