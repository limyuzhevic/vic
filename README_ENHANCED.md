# NLM (熙然) - Neural Learning Machine

**Phase 6: Final Integration - Enhanced Version**

## Overview

NLM is an experimental artificial developmental brain system that investigates whether memory systems can integrate with neural dynamics, neuromodulation affects plasticity in a coordinated way, and whether the complete brain loop functions coherently.

This enhanced Phase 6 version includes comprehensive improvements:

- **Enhanced Configuration System**: JSON/YAML support, validation, and advanced options
- **Comprehensive Command-Line Interface**: Extensive configuration and execution options
- **Performance Monitoring**: Real-time performance tracking and profiling
- **Plugin Architecture**: Extensible plugin system for custom functionality
- **Advanced Error Handling**: Robust error detection and recovery
- **Documentation**: Complete documentation and examples

## Quick Start

### Installation

```bash
# Build from source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# For Python bindings (if needed)
pip install pybind11 scikit-build-core
pip install .
```

### Basic Usage

```bash
# Run with default configuration
./nlm

# Load custom configuration
./nlm --config myconfig.cfg

# Run with advanced features
./nlm --config config.cfg --benchmark --visualization
```

### Python Usage

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete! {brain.getTotalSpikeCount()} spikes generated.")
```

## Key Improvements

### 1. Enhanced Configuration System

**Fixed Configuration Bug:**
- Removed duplicate `dopamine_baseline` entries in `default.cfg`
- Updated Phase 1 comments to reflect Phase 6 implementation

**New Features:**
- JSON/YAML configuration file support
- Comprehensive configuration validation
- Advanced configuration options
- Configuration export for debugging

**Usage:**
```bash
# Load JSON configuration
./nlm --config config.json

# Validate configuration
./nlm --config config.cfg --validate

# Export configuration for debugging
./nlm --config config.cfg --export-config
```

### 2. Comprehensive Command-Line Interface

**New Options:**
- `--help, -h`: Display comprehensive help information
- `--version, -V`: Display version information
- `--config, -c <file>`: Load configuration from file
- `--set <key>=<value>`: Set configuration value
- `--benchmark, -b`: Run benchmark mode
- `--benchmark-duration <n>`: Set benchmark duration
- `--visualization, -v`: Enable visualization
- `--no-colors`: Disable colored output
- `--log-level <level>`: Set log level
- `--log-file <file>`: Log to file
- `--output, -o <file>`: Output results

**Example Usage:**
```bash
# Run with custom configuration and overrides
./nlm --config config.cfg \
     --set brain.neuron_count=5000 \
     --set plasticity.stdp.enable=true \
     --benchmark \
     --visualization \
     --output results.json
```

### 3. Performance Monitoring

**Features:**
- Real-time performance monitoring
- Benchmark mode with detailed metrics
- Memory usage tracking
- Steps per second calculation
- Configuration export for analysis

**Usage:**
```bash
# Run benchmark mode
./nlm --benchmark --benchmark-duration 1000

# Run with performance monitoring
./nlm --config config.cfg --benchmark --log-level DEBUG
```

### 4. Plugin Architecture

**Built-in Plugins:**
1. **AdvancedValidationPlugin**: Enhanced configuration validation
2. **PerformanceEnhancementPlugin**: Performance optimization
3. **LoggingEnhancementPlugin**: Advanced logging capabilities
4. **ConfigValidationPlugin**: Configuration validation and error checking

**Plugin Usage:**
```python
import pynlm

# Create agent with plugin support
agent = pynlm.createAgentBrain(brain)

# Enable specific plugins
agent.enableAdvancedValidation(True)
agent.enablePerformanceEnhancement(True)
agent.enableLoggingEnhancement(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### 5. Advanced Configuration

**Advanced Configuration File:**
```cfg
# advanced_config_example.cfg

core_settings {
    random_seed = 123456789
    simulation_timestep = 0.0005
    neuron_count = 20000
    region_count = 4
    connectivity_pattern = "small_world"
}

plasticity {
    hebbian.enable = true
    hebbian.learning_rate = 0.025
    stdp.enable = true
    stdp.ltp_weight = 0.03
    structural.enable = true
}

performance {
    threading.enable = true
    threading.max_threads = 12
    optimization.level = "aggressive"
}

output {
    logging.level = "DEBUG"
    logging.format = "json"
}
```

**Usage:**
```bash
# Load advanced configuration
./nlm --config advanced_config_example.cfg
```

## Documentation

### Main Documentation
- **README.md**: Project overview and basic usage
- **HOW_TO_USE.md**: Beginner's guide and examples
- **NLM_ADVANCED_USAGE.md**: Advanced usage guide and examples
- **IMPLEMENTATION_SUMMARY.md**: Comprehensive implementation summary

### Configuration Examples
- **configs/default.cfg**: Default configuration
- **advanced_config_example.cfg**: Advanced configuration example

### Plugin Documentation
- **src/plugins/PluginInterface.hpp**: Plugin interface API
- **src/plugins/AdvancedPlugins.cpp**: Built-in plugin implementations

## Examples

### Example 1: Basic Configuration
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### Example 2: Advanced Simulation
```python
import pynlm
import time

def run_performance_optimized_simulation(config, duration, output_file=None):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    agent.enablePerformanceEnhancement(True)
    agent.enableAdvancedValidation(True)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=200, height=200, visionWidth=16, visionHeight=16)
    world.reset()
    agent.initialize(world)
    
    start_time = time.time()
    
    for step in range(duration):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    end_time = time.time()
    
    if output_file:
        results = {
            "duration": duration,
            "elapsed_time": end_time - start_time,
            "steps_per_second": duration / (end_time - start_time),
        }
        import json
        with open(output_file, 'w') as f:
            json.dump(results, f, indent=2)
    
    return brain, world
```

### Example 3: Configuration Validation
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Validate configuration
validation_result = config.validate()
if not validation_result.valid:
    print("Configuration errors:")
    for error in validation_result.errors:
        print(f"  - {error}")
else:
    print("Configuration is valid")
```

### Example 4: Plugin System
```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Enable plugins
agent.enableAdvancedValidation(True)
agent.enablePerformanceEnhancement(True)
agent.enableLoggingEnhancement(True)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100)
world.reset()
agent.initialize(world)

# Run simulation with plugins
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Build Instructions

### From Source
```bash
# Standard build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run the main executable
./nlm

# Build with Python bindings
pip install .
```

### Running Tests
```bash
# Run existing tests
./nlm_test

# Run phase 3 demo
./nlm_phase3_demo

# Run phase 4 demo
./nlm_phase4_demo

# Run advanced demo
./nlm_advanced_demo
```

## Troubleshooting

### Common Issues

**Issue: Configuration validation fails**
```bash
# Solution: Check configuration files
# Ensure all required keys are present
# Use --validate flag
./nlm --config your_config.cfg --validate
```

**Issue: Performance is slow**
```bash
# Solution: Optimize configuration
./nlm --config config.cfg --performance.optimization.level=aggressive
./nlm --config config.cfg --threading.max_threads=8
```

**Issue: Memory usage is high**
```bash
# Solution: Reduce configuration values
./nlm --config config.cfg --set brain.neuron_count=1000
./nlm --config config.cfg --set memory_pool.enable=false
```

### Getting Help
```bash
# Comprehensive help
./nlm --help

# Configuration help
./nlm --help config

# Plugin help
./nlm --help plugins

# Performance help
./nlm --help performance
```

## Future Development

### Planned Enhancements
1. **Machine Learning Integration**: Integrate neural network training capabilities
2. **Distributed Computing**: Support for distributed simulations
3. **GPU Acceleration**: CUDA integration for faster computations
4. **Advanced Visualization**: Real-time visualization and interaction
5. **Cloud Integration**: Cloud deployment and management
6. **Plugin Marketplace**: Community plugin sharing

### Research Applications
1. **Neuroscience Research**: Study neural dynamics and plasticity
2. **AI Research**: Explore artificial neural systems
3. **Cognitive Modeling**: Model cognitive processes and behavior
4. **Optimization Problems**: Solve complex optimization problems
5. **Machine Learning**: Integrate with machine learning algorithms

## Conclusion

NLM Phase 6 provides a comprehensive neural simulation platform with:

- **Enhanced Configuration Management**: JSON/YAML support, validation, and advanced options
- **Comprehensive Command-Line Interface**: Extensive configuration and execution options
- **Performance Monitoring**: Real-time performance tracking and profiling
- **Plugin Architecture**: Extensible plugin system for custom functionality
- **Advanced Error Handling**: Robust error detection and recovery
- **Complete Documentation**: Comprehensive documentation and examples

These enhancements make NLM a powerful and flexible platform for neural simulation research and applications. The improved configuration system, performance monitoring, and plugin architecture enable users to conduct sophisticated experiments and develop custom solutions.

For more information, visit the NLM project documentation and examples.
