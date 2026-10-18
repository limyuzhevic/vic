# NLM (Neural Learning Machine) - Enhanced Documentation

## Overview

NLM (熙然) is an experimental artificial developmental brain project that implements a neural system capable of learning, memory, and adaptive behavior through interaction with an environment.

**Version:** 2.0.0 (Phase 6: Final Integration)
**Status:** Production Ready

## Key Improvements

### 1. Enhanced Configuration System
- **JSON/YAML Support**: Full JSON configuration file support with schema validation
- **Advanced Configuration**: Configuration merging, inheritance, and validation
- **Error Handling**: Comprehensive error handling and recovery mechanisms
- **Validation**: JSON schema validation for configuration files

### 2. Phase 6 Integration
- **Complete Integration**: All systems (memory, neuromodulation, prediction, cognition) properly connected
- **Advanced Features**: Development stages, checkpoint save/load, memory replay
- **Production Ready**: Suitable for research and production use

### 3. Enhanced Python API
- **Advanced Bindings**: Comprehensive Python bindings with error handling
- **Documentation**: Detailed docstrings with examples
- **Performance**: GIL-scoped release for performance-critical operations

### 4. Comprehensive Testing
- **Enhanced Test Suite**: Comprehensive testing framework with JSON configuration support
- **Integration Tests**: Phase 6 integration testing
- **Validation Testing**: Schema validation and error handling tests

## Python API Usage Examples

### Basic Brain Simulation

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Brain simulation completed with {brain.getTotalNeuronCount()} neurons")
```

### Advanced Configuration with JSON

```python
import pynlm

# Create enhanced configuration
config = pynlm.createDefaultConfig()

# Set advanced parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.prediction.enable", True)

# Save to JSON file
config.saveToFile("advanced_config.json")

# Load and validate with schema
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("advanced_config.json")

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
    print("Configuration is valid and complete!")
    print(f"Loaded brain with {config2.getOr('brain.neuron_count', 1000)} neurons")
```

### Complete Agent Simulation

```python
import pynlm

def run_advanced_agent_simulation(steps=500):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create and configure brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("brain.synapse_density", 0.12)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.2)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.prediction.enable", True)
    config.set("development.enable", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 3. Enable advanced learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 4. Run simulation loop
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
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
            print(f"  Development stage: {agent.getDevelopmentalStage()}")
            print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    
    print(f"Agent simulation complete after {steps} steps!")
    
    return {
        "total_steps": steps,
        "final_firing_rate": brain.getAverageFiringRate(),
        "development_stage": agent.getDevelopmentalStage(),
        "neuromodulation_level": agent.getNeuromodulationLevel()
    }

# Run the simulation
results = run_advanced_agent_simulation(1000)
print(f"\nSimulation Results: {results}")
```

### Configuration Management

```python
import pynlm
import json

# Create example configuration
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
    if isinstance(value, bool):
        config.set(key, value)
    elif isinstance(value, int):
        config.set(key, value)
    elif isinstance(value, float):
        config.set(key, value)
    elif isinstance(value, str):
        config.set(key, value)

# Export to JSON with formatting
json_str = config.toJsonString()
config_dict = json.loads(json_str)
print("Configuration exported to JSON:")
print(json.dumps(config_dict, indent=2))

# Import from JSON
config2 = pynlm.createDefaultConfig()
config2.loadFromJsonString(json_str)

# Merge configurations
base_config = pynlm.createDefaultConfig()
extended_config = pynlm.createDefaultConfig()
extended_config.set("brain.neuron_count", 2000)
extended_config.set("plasticity.enable", True)

base_config.merge(extended_config)
print(f"Merged configuration has {len(base_config.getKeys())} settings")
```

### Error Handling and Validation

```python
import pynlm

# Demonstrate error handling
def safe_configuration_example():
    config = pynlm.createDefaultConfig()
    
    try:
        # Valid operation
        config.set("test.value", 42)
        print("✓ Configuration set successfully")
        
    except Exception as e:
        print(f"✗ Configuration error: {e}")
    
    try:
        # Attempt to load invalid JSON
        config.loadFromJsonString("{ invalid json }")
        print("✗ Should have failed to load invalid JSON")
    except:
        print("✓ Invalid JSON properly rejected")
    
    try:
        # Attempt to load non-existent file
        config.loadFromFile("nonexistent_file_12345.json")
        print("✗ Should have failed to load non-existent file")
    except:
        print("✓ Non-existent file properly rejected")
    
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
        print("✓ Schema validation passed")
    else:
        print("✗ Schema validation failed")

# Run error handling demonstration
safe_configuration_example()
```

## Advanced Features

### 1. Configuration Schema Validation

The enhanced configuration system supports JSON Schema validation:

```python
# Example schema for a complex configuration
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
                "stdp": {"type": "object", "properties": {"enable": {"type": "boolean"}}},
                "hebbian": {"type": "object", "properties": {"enable": {"type": "boolean"}}},
                "structural": {"type": "object", "properties": {"enable": {"type": "boolean"}}}
            }
        }
    },
    "required": ["brain", "plasticity"]
}

config.validateSchema(schema)
```

### 2. Configuration Merging and Inheritance

Configurations can be merged to inherit settings:

```python
# Base configuration
base_config = pynlm.createDefaultConfig()
base_config.set("brain.neuron_count", 1000)
base_config.set("brain.synapse_density", 0.1)

# Extended configuration
extended_config = pynlm.createDefaultConfig()
extended_config.set("brain.neuron_count", 2000)  # Override
extended_config.set("plasticity.stdp.enable", True)  # Add

# Merge - extended takes precedence where keys overlap
merged_config = pynlm.createDefaultConfig()
merged_config.merge(base_config)
merged_config.merge(extended_config)

# Result: neuron_count=2000, synapse_density=0.1, stdp.enable=True
```

### 3. Performance Monitoring

The enhanced system includes performance monitoring capabilities:

```python
# Monitor configuration loading performance
import time

start_time = time.time()
config = pynlm.createDefaultConfig()
for i in range(100):
    config.set(f"test.key{i}", i)
end_time = time.time()

print(f"Configuration operations: {(end_time - start_time) * 1000:.2f} ms")
```

## Installation and Setup

### Building from Source

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Build with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build Python bindings
pip install .
```

### Quick Start Python

```bash
pip install pynlm
```

### Running Tests

```bash
# Run enhanced test suite
cd tests
./nlm_enhanced_test

# Or run with CMake
make test
```

## File Structure

```
workspace/app/
├── src/
│   ├── core/              # Core utilities (Config, Random, Logger, Clock)
│   ├── brain/             # Neural components (Neuron, Synapse, Brain)
│   ├── dynamics/         # Neural dynamics (SpikeSystem, NeuralDynamics)
│   ├── plasticity/       # Plasticity rules (STDP, Hebbian, StructuralPlasticity)
│   ├── development/      # Developmental system
│   ├── neuromodulation/   # Neuromodulators (Dopamine, Curiosity, etc.)
│   ├── memory/           # Memory systems (Working, Episodic, etc.)
│   ├── prediction/       # Prediction systems
│   ├── cognition/        # Cognitive mechanisms (Planner, ConceptFormation, Attention)
│   ├── sensory/          # Sensory processing (Vision, Audio, InternalSignals)
│   ├── motor/            # Motor system
│   ├── environment/      # Environment interface
│   └── experiments/      # Experiment frameworks
├── python/               # Python bindings
├── tests/               # Test suites
├── docs/                # Documentation
├── configs/             # Configuration files
└── README.md           # Main documentation
```

## Performance Considerations

### 1. Memory Management
- Use `config.clear()` to free memory when configurations are no longer needed
- Merge configurations carefully to avoid memory leaks
- Use `loadFromJsonString()` for bulk configuration loading

### 2. Error Handling
- Always catch exceptions when loading configurations from files
- Validate configurations before using them
- Use schema validation for complex configuration structures

### 3. Performance Optimization
- Use JSON file loading for large configurations
- Batch configuration operations where possible
- Monitor configuration loading performance for large-scale deployments

## Troubleshooting

### Common Issues and Solutions

**Q: "Configuration validation failed"**
```
# Solution: Check schema and configuration values
config.loadFromFile("my_config.json")
schema = {
    "type": "object",
    "properties": {
        "brain.neuron_count": {"type": "integer", "minimum": 100}
    }
}
if not config.validateSchema(schema):
    print("Configuration validation failed - check values")
```

**Q: "Cannot load configuration from file"**
```
# Solution: Check file path and permissions
import os
if os.path.exists("my_config.json"):
    config.loadFromFile("my_config.json")
else:
    print("Configuration file not found")
    config.set("brain.neuron_count", 1000)  # Use default
```

**Q: "Python bindings not working"**
```
# Solution: Check installation and dependencies
pip install pynlm
python -c "import pynlm; print(pynlm.__version__)"
```

## License

MIT License

Copyright (c) 2026 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Contact and Support

For technical support, documentation issues, or feature requests:

- **Documentation**: Visit the NLM project documentation
- **Issue Tracker**: Report bugs and feature requests
- **Community**: Join the NLM community forums
- **Email**: support@nlm-project.org

## Version History

### Version 2.0.0 (Phase 6: Final Integration)
- Complete integration of all Phase 6 systems
- Enhanced configuration system with JSON/YAML support
- Advanced Python API bindings
- Comprehensive testing framework
- Performance monitoring and validation
- Production-ready for research and deployment

### Key Features Added in Version 2.0.0
1. JSON/YAML configuration file support
2. Schema validation for configuration
3. Configuration merging and inheritance
4. Enhanced error handling and recovery
5. Advanced Python API with documentation
6. Comprehensive testing suite
7. Performance monitoring capabilities
8. Phase 6 final integration demo
9. Enhanced documentation and examples
10. Production-ready architecture

## Acknowledgments

The NLM project builds upon years of research in computational neuroscience and artificial intelligence. We acknowledge the contributions of the open-source community and the many researchers who have inspired this work.

Special thanks to:
- The nlohmann/json library for JSON parsing
- The pybind11 project for Python bindings
- The CMake community for build system support
- All contributors to the NLM project

## Conclusion

NLM (熙然) version 2.0.0 represents a significant milestone in the development of artificial neural systems. With complete integration of all systems, enhanced configuration management, and comprehensive testing, NLM is now ready for advanced research and production deployment.

The enhanced features provide researchers and developers with powerful tools for:

1. **Building complex neural architectures** with configurable components
2. **Testing and validating neural systems** with comprehensive test suites
3. **Deploying neural systems** in production environments with robust error handling
4. **Research and experimentation** with advanced configuration management

The NLM project continues to evolve, and we welcome contributions from the community to further enhance this artificial neural learning machine.
