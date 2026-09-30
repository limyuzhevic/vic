# NLM (Neural Learning Machine) - HOW TO USE

**Phase 6: Final Integration Enhanced Edition**

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20 with Python bindings. Phase 6 represents the final integration of all systems into a coherent artificial brain.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the enhanced NLM Python API
4. Advanced configuration options
5. Command-line interface usage

## Part 1: Building the C++ Project from Source

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

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 integration demonstration |
| `nlm_test` | Executable | Unit test suite |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install

# Build with Python bindings
-DPYBIND11=ON
-DPYTHON_EXECUTABLE=/usr/bin/python3

# Build with analysis tools
-DANALYSIS_ENABLED=ON
-DANALYZER_BACKEND=python
```

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

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
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Check available modules
print(dir(pynlm))

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")

# Test advanced features
brain.enableAdvancedAnalysis()
brain.enableDetailedLogging()
print("Advanced features enabled")
```

## Part 3: Using the Enhanced Python API

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation with Enhanced Features

```python
import pynlm

# Create brain with enhanced configuration
config = pynlm.createConfig()
config.loadFromArgs(["--preset=debug", "--enable-spike-tracing", "--verbose"])
brain = pynlm.createBrain(config)
brain.initialize()

# Enable advanced analysis
brain.enableAdvancedAnalysis()
brain.enablePerformanceMonitoring()
brain.enableDetailedLogging()

# Run simulation for 1000 steps with analysis
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        print(f"  Analysis: {brain.getAnalysisReport()}")
```

### Advanced Configuration Options

#### Command-Line Presets
```python
# Import the advanced configuration module
import pynlm.config as config_module

# Use preset configurations
config = config_module.createConfig()
config.apply_preset("debug")
config.apply_preset("production")
config.apply_preset("research")

# Custom configuration with command-line overrides
config = config_module.createConfig()
config.loadFromArgs([
    "--neuron-count=2000",
    "--region-count=4",
    "--plasticity-mode=both",
    "--dopamine-level=0.5",
    "--curiosity-level=0.3",
    "--enable-spike-tracing",
    "--log-level=Debug"
])
```

#### Configuration File Usage
```python
import pynlm

# Create configuration
config = pynlm.createConfig()

# Set configuration values
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", True)
config.set("neuromodulation.dopamine.scale", 1.0)

# Save configuration to file
config.saveToFile("custom_config.json")

# Load configuration from file
loaded_config = pynlm.createConfig()
loaded_config.loadFromFile("custom_config.json")
```

#### JSON Configuration
```python
import pynlm
import json

# Create advanced JSON configuration
advanced_config = {
    "config": {
        "neuron_count": 2000,
        "region_count": 2,
        "connection_probability": 0.15,
        "plasticity": {
            "stdp": { "learning_rate": 0.02 },
            "hebbian": { "learning_rate": 0.01 },
            "structural": { "enabled": True }
        },
        "neuromodulation": {
            "dopamine": { "scale": 0.5 },
            "curiosity": { "exploration_bonus": 0.3 },
            "novelty": { "threshold": 0.1 }
        },
        "memory": {
            "working_memory": { "capacity": 500 },
            "episodic_memory": { "max_episodes": 2000 }
        },
        "development": {
            "enabled": True,
            "plasticity_modulation": { "Adult": 0.2 }
        }
    }
}

# Save JSON configuration
with open("advanced_config.json", "w") as f:
    json.dump(advanced_config, f, indent=2)

# Load JSON configuration
loaded_config = pynlm.createConfig()
loaded_config.loadFromFile("advanced_config.json")
```

### Enhanced Agent Example

```python
import pynlm

def run_enhanced_agent_simulation(num_steps=2000):
    """Run a complete agent simulation with all enhancements."""
    
    # 1. Create configuration with advanced options
    config = pynlm.createConfig()
    config.loadFromArgs([
        "--preset=debug",
        "--neuron-count=500",
        "--region-count=2",
        "--plasticity-mode=both",
        "--enable-spike-tracing",
        "--enable-memory-profiling",
        "--enable-performance-monitoring",
        "--log-level=Debug"
    ])
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Enable brain analysis
    brain.enableAdvancedAnalysis()
    brain.enablePerformanceMonitoring()
    brain.enableDetailedLogging()
    
    # 8. Run simulation loop with detailed logging
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print detailed progress
        if step % 100 == 0:
            print(f"=== Step {step} ===")
            print(f"  World: {world.getSimulationTime():.2f}s at ({world.getAgentPosition()[0]:.1f}, {world.getAgentPosition()[1]:.1f})")
            print(f"  Brain: {brain.getFiringNeuronCount()} firing neurons")
            print(f"  Agent: Curiosity={agent.getCuriosityLevel():.3f}, Novelty={agent.getNoveltyLevel():.3f}")
            print(f"  Analysis: {brain.getAnalysisReport()}")
            print()
    
    # 9. Generate final report
    print("=== Simulation Complete ===")
    print(f"Final Statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print(f"  Performance: {brain.getPerformanceMetrics()}")
    
    return brain, agent, world

# Run the enhanced simulation
brain, agent, world = run_enhanced_agent_simulation(2000)
```

### Advanced Analysis and Debugging

```python
import pynlm

# Create brain
config = pynlm.createConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable advanced analysis
brain.enableAdvancedAnalysis()
brain.enableDetailedLogging()
brain.enablePerformanceMonitoring()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get analysis results
analysis = brain.getAnalysisResults()
print(f"Analysis Report:")
print(f"  Network topology: {analysis.totalNeurons} neurons, {analysis.totalSynapses} synapses")
print(f"  Activity: {analysis.totalSpikes} total spikes, {analysis.averageFiringRate:.2f} Hz")
print(f"  Plasticity: {analysis.plasticityStatus}")
print(f"  Memory: {analysis.memoryUsage}")
print(f"  Performance: {analysis.performanceMetrics}")

# Get detailed statistics
stats = brain.getDetailedStatistics()
print(f"\nDetailed Statistics:")
print(f"  Neuron distribution: {stats.neuronStats}")
print(f"  Synapse weights: {stats.synapseStats}")
print(f"  Activity patterns: {stats.activityPatterns}")

# Generate visualization data
viz_data = brain.generateVisualizationData()
print(f"\nVisualization data available: {len(viz_data.weightMatrix)}x{len(viz_data.weightMatrix[0])} weight matrix")
print(f"  Firing rate distribution: {len(viz_data.firingRateDistribution)} bins")
print(f"  Connectivity distribution: {len(viz_data.connectivityDistribution)} categories")
```

### Configuration Validation

```python
import pynlm

# Create configuration with validation
config = pynlm.createConfig()

# Add some configurations
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Validate configuration
if not config.validate():
    print("Configuration validation failed!")
    for error in config.getValidationErrors():
        print(f"  ERROR: {error}")
else:
    print("Configuration valid!")

# Load from command-line with validation
config = pynlm.createConfig()
if config.loadFromArgs(["--neuron-count=2000", "--region-count=4"]):
    print("Command-line arguments loaded successfully")
else:
    print("Warning: Some arguments could not be parsed")
```

## Advanced Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.hebbian.learning_rate` | float | 0.001 | Hebbian learning rate |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |
| `neuromod.prediction_error.scale` | float | 1.0 | Prediction error scaling |

### Memory Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `memory.working_memory.capacity` | int | 1000 | Working memory capacity |
| `memory.working_memory.decay_rate` | float | 0.95 | Working memory decay rate |
| `memory.episodic_memory.max_episodes` | int | 10000 | Maximum episodic memory episodes |
| `memory.episodic_memory.consolidation_interval` | int | 1000 | Memory consolidation interval |

### Development Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.enabled` | bool | true | Enable development system |
| `development.plasticity_modulation.Initial` | float | 1.0 | Initial stage plasticity |
| `development.plasticity_modulation.CriticalPeriod` | float | 0.8 | Critical period plasticity |
| `development.plasticity_modulation.Maturation` | float | 0.5 | Maturation plasticity |
| `development.plasticity_modulation.Adult` | float | 0.2 | Adult plasticity |

### Analysis Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `analysis.enabled` | bool | true | Enable analysis tools |
| `analysis.backend` | string | "python" | Analysis backend |
| `analysis.log_level` | string | "Info" | Analysis log level |
| `analysis.export_format` | string | "json" | Default export format |

## Part 4: Example Scripts

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction with Enhanced Features

```python
import pynlm

# Setup with advanced configuration
config = pynlm.createConfig()
config.loadFromArgs([
    "--preset=debug",
    "--enable-spike-tracing",
    "--enable-memory-profiling",
    "--log-level=Debug"
])

# Create agent
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode with analysis
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())

# Analyze results
print("Episode Analysis:")
print(f"  Total activity: {brain.getTotalSpikeCount()} spikes")
print(f"  Performance: {brain.getPerformanceMetrics()}")
print(f"  Development: {brain.getDevelopmentalStage()}")
```

## Advanced Features

### 1. Configuration File Format

#### JSON Configuration
```json
{
    "config": {
        "neuron_count": 2000,
        "region_count": 2,
        "plasticity": {
            "stdp": { "learning_rate": 0.02 },
            "hebbian": { "learning_rate": 0.01 }
        },
        "neuromodulation": {
            "dopamine": { "scale": 0.5 },
            "curiosity": { "exploration_bonus": 0.3 }
        }
    }
}
```

#### Key-Value Configuration
```
# Comments start with #
key = value
another_key = "quoted value"

# Empty lines are ignored
# Lines with # or / are comments
```

### 2. Configuration Sources Priority

1. Command-line arguments (highest priority)
2. Configuration presets
3. Configuration files
4. Default values (lowest priority)

### 3. Configuration Validation

The configuration system validates:
- Parameter ranges (e.g., neuron_count > 0)
- Parameter consistency (e.g., region_count <= neuron_count)
- Data type compatibility
- Biological plausibility (e.g., probability values between 0.0 and 1.0)

### 4. Analysis Tools

The enhanced NLM includes comprehensive analysis tools:
- **Brain Analyzer**: Detailed analysis of brain state
- **Pattern Detection**: Automatic detection of patterns in neural activity
- **Anomaly Detection**: Detection of abnormal behaviors
- **Performance Monitoring**: Tracking and reporting performance metrics
- **Visualization Data**: Data export for visualization

### 5. Advanced Error Handling

The system provides comprehensive error handling:
- **Configuration validation**: Validates parameter ranges and consistency
- **Input validation**: Validates user input and command-line arguments
- **File I/O**: Handles file reading and writing errors gracefully
- **Memory management**: Prevents memory leaks and buffer overflows
- **Debug information**: Provides detailed error messages for debugging

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Verify all required libraries are linked

**Configuration validation errors:**
- Check parameter ranges
- Use preset configurations as starting point
- Validate with `config.validate()`

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [ADVANCED_OPTIONS.md](ADVANCED_OPTIONS.md) - Command-line options and presets

## Summary

This document provides comprehensive guidance for using the enhanced NLM Phase 6 implementation. The key improvements include:

1. **Enhanced Configuration System**: JSON/YAML support, preset configurations, command-line overrides
2. **Advanced User Interface**: Improved CLI, better error handling, progress monitoring
3. **Enhanced Neural Components**: Better encapsulation, validation, plasticity
4. **Comprehensive Analysis Tools**: Brain analysis, pattern detection, performance monitoring
5. **Expanded Configuration Options**: Extensive options for customization
6. **Improved Documentation**: Comprehensive documentation for all features

These improvements make NLM more accessible to beginners while providing powerful tools for advanced researchers. The system is now ready for Phase 6: Final Integration, where all components work together as a coherent artificial brain.
