# NLM (Neural Learning Machine) - Enhanced Documentation

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

This enhanced documentation provides comprehensive information about the NLM framework, including enhanced features and best practices for using the Python bindings and C++ API.

## Quick Start

### Python Installation and Usage

```bash
# Install the enhanced Python bindings
pip install -e .

# Test installation
python -c "import pynlm; print(pynlm.get_version()); print(pynlm.get_features())"
```

### Basic Python Example

```python
import pynlm
import time

# Create a default configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)

# Create brain and world
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation for 100 steps
start_time = time.time()
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Simulation completed in {time.time() - start_time:.2f} seconds")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Advanced Python Example with Enhanced Features

```python
import pynlm
import numpy as np

# Create learning environment
env = pynlm.createLearningEnvironment()

brain = env["brain"]
world = env["world"]
agent = env["agent"]

# Enable all features
agent.toggleAllFeatures()

# Run learning simulation
for episode in range(50):
    world.reset()
    agent.reset()
    
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply neuromodulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
    
    if episode % 10 == 0:
        print(f"Episode {episode}:")
        print(f"  Firing rate: {brain.getAverageFiringRate():.3f} Hz")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Developmental stage: {agent.getDevelopmentalStage()}")

print("Learning simulation completed!")
```

## Enhanced Python API Features

### 1. Enhanced Error Handling

The enhanced Python bindings provide comprehensive error handling with custom exception types:

```python
try:
    import pynlm
    config = pynlm.createDefaultConfig()
    config.set("invalid_key", "value")
    pynlm.validateConfig(config)
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.BrainInitializationError as e:
    print(f"Brain initialization error: {e}")
```

### 2. NumPy Array Support

The enhanced bindings support NumPy arrays for efficient data exchange:

```python
import numpy as np
import pynlm

# Create vision data as NumPy array
vision_data = np.random.rand(64, 64, 3).astype(np.float32)

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Create world and get sensory input
world = pynlm.createSimpleWorld()
percept = world.getSensoryPercept()

# Set vision data from NumPy array
vision = percept.getVision()
vision.setData(vision_data)

# Convert to NumPy array for processing
processed_data = vision.to_numpy()
print(f"Vision data shape: {processed_data.shape}")
```

### 3. Phase 6 Integration Utilities

Run Phase 6 integration tests directly from Python:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Run quick integration test
if pynlm.runPhase6Demo(config, "quick"):
    print("✓ Phase 6 integration test passed!")

# Run full integration test
result = pynlm.runPhase6Demo(config, "full")
if result["success"]:
    print(f"✓ Full integration test passed!")
    print(f"  Total reward: {result['total_reward']}")
    print(f"  Average firing rate: {result['avg_firing_rate']} Hz")
```

### 4. Brain Benchmarking

Benchmark brain performance:

```python
import pynlm

# Create multiple brains
brains = []
for i in range(3):
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 500 + i * 500)
    brain = pynlm.createBrain(config)
    pynlm.initializeBrain(brain)
    brains.append(brain)

# Benchmark all brains
results = pynlm.benchmarkBrains(brains, 100)

for result in results:
    print(f"Brain {result['index']}:")
    print(f"  Neurons: {result['neurons']}")
    print(f"  Synapses: {result['synapses']}")
    print(f"  Steps/sec: {result['steps_per_sec']:.0f}")
    print(f"  Total spikes: {result['total_spikes']}")
```

### 5. Enhanced Configuration Management

Advanced configuration features:

```python
import pynlm

# Create and configure
config = pynlm.createDefaultConfig()

# Set multiple values
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.15)
config.set("stdp_ltp_weight", 0.02)
config.set("stdp_ltd_weight", 0.015)

# Get values with defaults
neurons = config.getOr("neuron_count", 500)
regions = config.getOr("region_count", 1)

# Convert to dictionary
config_dict = config.to_dict()
print(f"Configuration: {config_dict}")

# Validate configuration
if pynlm.validateConfig(config):
    print("✓ Configuration is valid")
else:
    print("✗ Configuration is invalid")
```

## Enhanced C++ API

### Enhanced Demo Application

The enhanced demo application provides comprehensive testing and demonstration capabilities:

```bash
# Build all targets
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run different demo modes
./nlm_enhanced_demo --mode demo --neurons 500 --steps 1000
./nlm_enhanced_demo --mode benchmark --neurons 1000 --steps 500
./nlm_enhanced_demo --mode test
./nlm_enhanced_demo --mode analyze
./nlm_enhanced_demo --mode interactive
```

### Command Line Options

```
./nlm_enhanced_demo [options]

Modes:
  --mode demo        Run comprehensive demo (default)
  --mode test        Run validation and integration tests
  --mode benchmark   Run performance benchmarks
  --mode analyze     Analyze network properties
  --mode interactive  Interactive mode

Options:
  --config <file>    Configuration file (default: configs/default.cfg)
  --neurons <count>  Number of neurons (default: 1000)
  --steps <count>    Number of simulation steps (default: 5000)
  --timestep <time>  Simulation timestep (default: 0.001s)
  --verbose          Enable verbose output
  --no-verbose       Disable verbose output
  --no-results       Don't save results
  --help             Show help information
```

### Enhanced Demo Features

The enhanced demo includes:

1. **Basic Connectivity Test** - Verifies spike propagation through the network
2. **Plasticity Learning Experiment** - Demonstrates activity-dependent synaptic changes
3. **STDP Verification** - Validates spike-timing-dependent plasticity
4. **Integration Test** - Tests all integrated brain systems
5. **Network Analysis** - Provides detailed analysis of network properties
6. **Learning Experiment** - Runs comprehensive learning simulations

## Performance Monitoring

### Python Performance Monitoring

Track brain performance in real-time:

```python
import pynlm
import time

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
pynlm.initializeBrain(brain)

start_time = time.time()
steps_completed = 0

for step in range(1000):
    brain.step(step, step * 0.001)
    steps_completed += 1
    
    if step % 100 == 0:
        metrics = brain.getPerformanceMetrics()
        print(f"Step {step}:")
        print(f"  Steps/sec: {metrics['steps_per_sec']:.0f}")
        print(f"  Neurons: {metrics['neurons']}")
        print(f"  Synapses: {metrics['synapses']}")
        print(f"  Active neurons: {metrics['active_neurons']}")
        print(f"  Firing neurons: {metrics['firing_neurons']}")

end_time = time.time()
total_time = end_time - start_time

print(f"\nPerformance Summary:")
print(f"  Total steps: {steps_completed}")
print(f"  Total time: {total_time:.2f}s")
print(f"  Steps/sec: {steps_completed/total_time:.0f}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
```

### C++ Performance Monitoring

The enhanced C++ demo includes built-in performance monitoring:

```bash
# Run benchmark mode for detailed performance analysis
./nlm_enhanced_demo --mode benchmark --neurons 1000 --steps 500
```

## Error Handling and Robustness

### Enhanced Exception Types

The enhanced framework provides specific exception types for different error scenarios:

- **NLMException**: Base exception for all NLM errors
- **ValidationError**: For configuration and input validation errors
- **ConfigurationError**: For configuration-related errors
- **BrainInitializationError**: For brain initialization failures

### Error Recovery Strategies

1. **Graceful Degradation**: Components continue to function if optional features fail
2. **Fallback Values**: Default values are used when configuration is missing
3. **Validation**: Comprehensive validation of inputs and configurations
4. **Logging**: Detailed error logging for debugging
5. **Retry Mechanisms**: Automatic retries for transient failures

## Configuration Options

### Enhanced Configuration Schema

The enhanced configuration system supports:

1. **Primitive Types**: Integers, floats, booleans, strings
2. **Complex Types**: Nested configurations, arrays
3. **Source Tracking**: Configuration sources (command line, file, defaults)
4. **Validation**: Custom validation rules
5. **Serialization**: Save and load configuration in JSON format

### Example Configuration

```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "enable_checkpointing": true,
    "enable_replay": true,
    "enable_development": true,
    "checkpoint_dir": "./checkpoints"
}
```

## Installation and Setup

### Python Installation

```bash
# Install from source
pip install -e .

# Install development dependencies
pip install pybind11 scikit-build-core pytest numpy

# Test installation
python -m pytest tests/ -v
```

### C++ Installation

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4

# Install (optional)
sudo make install
```

### Python Environment Setup

```python
import pynlm

# Check version and features
print(f"NLM Version: {pynlm.get_version()}")
print("Available features:")
for feature in pynlm.get_features():
    print(f"  - {feature}")

# Create learning environment
env = pynlm.createLearningEnvironment()
print(f"Created environment with {env['brain'].getTotalNeuronCount()} neurons")
```

## Advanced Features

### 1. Multi-Brain Benchmarking

Compare performance across different brain configurations:

```python
import pynlm
import time

# Create brains with different configurations
configs = [
    {"neuron_count": 500, "connection_probability": 0.1},
    {"neuron_count": 1000, "connection_probability": 0.15},
    {"neuron_count": 2000, "connection_probability": 0.2},
]

brains = []
for cfg in configs:
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", cfg["neuron_count"])
    config.set("connection_probability", cfg["connection_probability"])
    brain = pynlm.createBrain(config)
    pynlm.initializeBrain(brain)
    brains.append(brain)

# Run benchmark
results = pynlm.benchmarkBrains(brains, 100)

print("Benchmark Results:")
for result in results:
    print(f"  Neurons: {result['neurons']}, Steps/sec: {result['steps_per_sec']:.0f}")
```

### 2. Advanced Agent Features

Enable and monitor advanced agent features:

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.toggleAllFeatures()

# Monitor feature status
status = agent.getFeaturesStatus()
print("Agent Feature Status:")
for feature, enabled in status.items():
    print(f"  {feature}: {'✓' if enabled else '✗'}")

# Run simulation with full feature set
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply neuromodulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)
```

### 3. Enhanced Visualization and Analysis

Analyze network properties:

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
pynlm.initializeBrain(brain)

# Run simulation
for step in range(500):
    brain.step(step, step * 0.001)

# Get comprehensive analysis
analysis = brain.getPerformanceMetrics()
print("Network Analysis:")
print(f"  Total neurons: {analysis['neurons']}")
print(f"  Total synapses: {analysis['synapses']}")
print(f"  Average firing rate: {analysis['avg_firing_rate']:.3f} Hz")
print(f"  E/I ratio: {analysis['e_iratio']:.3f}")
print(f"  Total spikes: {analysis['total_spikes']}")

# Get region distribution
regions = brain.getRegions()
print(f"\nRegion Distribution:")
for region in regions:
    print(f"  Region {region.getId().index()} ({region.getName()}):")
    print(f"    Neurons: {region.getTotalNeuronCount()}")
    print(f"    Synapses: {region.getSynapseCount()}")
    print(f"    Average weight: {region.getAverageSynapticWeight():.4f}")
```

## Migration Guide

### From Basic to Enhanced Python API

If you're migrating from the basic Python API to the enhanced version, here are the key changes:

1. **New Exception Types**: Use specific exception types for better error handling
2. **Enhanced Configuration**: New configuration methods and validation
3. **NumPy Support**: Automatic NumPy array conversion and support
4. **Advanced Functions**: New utility functions like `runPhase6Demo`, `benchmarkBrains`
5. **Better Documentation**: Comprehensive examples and documentation

### From Basic to Enhanced C++ Demo

Key enhancements in the C++ demo:

1. **Multiple Modes**: demo, test, benchmark, analyze, interactive
2. **Enhanced CLI**: More command-line options and flexible configuration
3. **Better Error Handling**: Comprehensive error checking and recovery
4. **Performance Monitoring**: Built-in performance metrics and benchmarking
5. **Visualization**: Better output formatting and result display

## Troubleshooting

### Common Issues and Solutions

#### Issue: Python import fails
**Solution**: Ensure the Python module is properly installed:
```bash
pip install -e .
python -c "import pynlm; print(pynlm.__version__)"
```

#### Issue: C++ compilation fails
**Solution**: Ensure all dependencies are installed:
```bash
apt-get install cmake g++ python3-dev python3-numpy
```

#### Issue: Brain initialization fails
**Solution**: Check configuration and ensure all required parameters are set:
```python
import pynlm
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)
config.set("region_count", 1)
config.set("connection_probability", 0.1)

brain = pynlm.createBrain(config)
try:
    pynlm.initializeBrain(brain)
except pynlm.BrainInitializationError as e:
    print(f"Brain initialization failed: {e}")
```

#### Issue: Performance is slow
**Solution**: Optimize configuration and hardware requirements:
- Use smaller network sizes for development
- Enable hardware acceleration if available
- Use optimized build types (Release with -O3)

## Conclusion

The enhanced NLM framework provides comprehensive tools for neural simulation and research. Key improvements include:

1. **Enhanced Python API**: Better error handling, NumPy support, and advanced features
2. **Improved C++ Demo**: Multiple modes, comprehensive testing, and performance monitoring
3. **Advanced Configuration**: Flexible configuration management with validation
4. **Robust Error Handling**: Specific exception types and recovery mechanisms
5. **Performance Monitoring**: Built-in benchmarking and metrics collection
6. **Better Documentation**: Comprehensive examples and migration guides

The enhanced framework is suitable for both research and production use, providing the flexibility needed for complex neural simulations while maintaining ease of use for beginners.

## References

- [NLM Documentation](docs/README.md) - Core documentation
- [Phase 6 Integration](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 audit results
- [Architecture](docs/ARCHITECTURE.md) - System architecture
- [Science](docs/SCIENCE.md) - Scientific background
- [Research Methods](docs/RESEARCH_METHODS.md) - Research methodology

For more information, visit the NLM project documentation or contact the development team.
