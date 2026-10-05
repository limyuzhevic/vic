# NLM User Guide

## Welcome to NLM

# Neural Learning Machine (NLM) is an experimental artificial brain project that simulates brain-like systems through computational neuroscience. This guide provides comprehensive instructions for using NLM as both a researcher and developer.

## Quick Start

### Prerequisites

- **Operating System**: Linux, macOS, or Windows
- **Compiler**: C++17 compatible compiler (g++ 9+, clang 12+)
- **Build Tools**: CMake 3.15+
- **Python**: 3.8+ (for Python bindings)
- **Memory**: At least 4GB RAM
- **Storage**: At least 100MB free space

### Installation and Building

#### Basic Build

```bash
# Clone the repository
mkdir nlm_workspace
cd nlm_workspace
git clone https://github.com/your-org/nlm.git

# Create build directory and compile
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Build with Python Bindings

```bash
# With Python bindings enabled
cd nlm_workspace
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PYTHON=ON
make -j4

# Install Python bindings
pip install -e python/
```

#### Build with All Features

```bash
# Enable all optional features
cd nlm_workspace
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DNLM_ENABLE_PYTHON=ON \
         -DNLM_ENABLE_VISUALIZATION=ON \
         -DNLM_ENABLE_PERFORMANCE_MONITORING=ON \
         -DNLM_ENABLE_PLUGINS=ON
make -j4
```

## Basic Usage

### Running Phase 6 Integration Demo

The NLM project includes a comprehensive integration demo that tests all systems together:

```bash
# Run Phase 6 demo (requires compiled binary)
cd build
./nlm_phase6_demo
```

This demo runs a complete integration test verifying:
- All brain systems are connected
- Memory systems work correctly
- Neuromodulation affects plasticity
- Prediction is integrated with learning
- Development affects plasticity rates

### Creating a Simple Neural Simulation

#### Using the C++ API

Create a simple neural simulation using the enhanced C++ API:

```cpp
#include "nlm_py.cpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "world/SimpleWorld.hpp"

int main() {
    // Create configuration
    nlm::Config config;
    config.set("simulation.max_steps", 1000);
    config.set("brain.global_connectivity", 0.05);
    config.set("plasticity.stdp.enabled", true);
    
    // Create brain
    auto brain = nlm::createBrain(config);
    
    // Create world
    auto world = nlm::createSimpleWorld();
    world->configure(100.0, 100.0, 20, 20);
    
    // Initialize brain
    brain->initialize();
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        // Get sensory input
        auto percept = world->getSensoryPercept();
        brain->receiveSensoryInput(percept);
        
        // Step brain
        brain->step(step);
        
        // Apply action
        auto action = brain->produceAction();
        world->applyMotorCommand(action);
        
        // Update world
        world->update(step * 0.1);
        
        // Print progress every 100 steps
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                      << "Spikes: " << brain->getTotalSpikeCount()
                      << ", Neurons: " << brain->getTotalNeuronCount()
                      << ", Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        }
    }
    
    std::cout << "Simulation completed successfully!" << std::endl;
    return 0;
}
```

#### Using the Python API

The enhanced Python API provides a more accessible interface:

```python
import nlm_py
from nlm_py import Brain, Config, SimpleWorld, createBrain, createSimpleWorld
from nlm_py import PerformanceMonitor, StatisticsCollector

# Create configuration
config = Config()
config.set("simulation.max_steps", 1000)
config.set("brain.global_connectivity", 0.05)
config.set("plasticity.stdp.enabled", True)
config.set("plasticity.hebbian.enabled", True)

# Create brain and world
brain = createBrain(config)
world = createSimpleWorld()
world.configure(100.0, 100.0, 20, 20)

# Create monitoring tools
monitor = PerformanceMonitor(brain)
stats = StatisticsCollector(brain)

# Initialize brain
brain.initialize()

# Run simulation with monitoring
for step in range(1000):
    # Record statistics
    stats.add_metric("firing_rate", brain.getAverageFiringRate())
    stats.add_metric("active_neurons", brain.getActiveNeuronCount())
    
    # Step simulation
    brain.step(step)
    
    # Get performance metrics
    perf = monitor.getMetrics()
    
    # Periodic output
    if step % 100 == 0:
        print(f"Step {step}: FPS={perf.fps:.2f}, "
              f"Memory={perf.memory_usage:.2f}MB, "
              f"Active neurons={brain.getActiveNeuronCount()}")

# Generate final report
stats.generate_report()
print("Advanced simulation completed with monitoring")
```

## Configuration Management

### Creating Configurations

#### Basic Configuration

```cpp
nlm::Config config;

// Set basic parameters
config.set("simulation.max_steps", 5000);
config.set("simulation.temporal_decay", 0.95);
config.set("brain.global_connectivity", 0.1);
config.set("neuron_count", 100);
config.set("region_count", 10);
config.set("simulation_timestep", 0.1);
config.set("connection_probability", 0.2);

// Save configuration
config.saveToFile("simulation_config.json");
```

#### Python Configuration

```python
from nlm_py import Config

# Create configuration
config = Config()

# Set configuration parameters
config.set("simulation.max_steps", 5000)
config.set("simulation.temporal_decay", 0.95)
config.set("brain.global_connectivity", 0.1)
config.set("neuron_count", 100)
config.set("region_count", 10)
config.set("simulation_timestep", 0.1)
config.set("connection_probability", 0.2)

# Save configuration
config.save_to_file("simulation_config.json")

# Load configuration
config2 = Config()
config2.load_from_file("simulation_config.json")
```

### Configuration Templates

#### Development Configuration

```cpp
nlm::Config config;

// Optimized for development/testing
config.set("simulation.max_steps", 100);
config.set("brain.global_connectivity", 0.2);
config.set("neuron_count", 10);
config.set("region_count", 2);
config.set("simulation_timestep", 0.05);
config.set("connection_probability", 0.3);
config.set("plasticity.stdp.enabled", true);
config.set("plasticity.hebbian.enabled", true);
config.set("plasticity.structural.enabled", false);
```

#### Research Configuration

```cpp
nlm::Config config;

// Optimized for research
config.set("simulation.max_steps", 50000);
config.set("brain.global_connectivity", 0.05);
config.set("neuron_count", 1000);
config.set("region_count", 50);
config.set("simulation_timestep", 0.01);
config.set("connection_probability", 0.1);
config.set("plasticity.stdp.enabled", true);
config.set("plasticity.hebbian.enabled", true);
config.set("plasticity.structural.enabled", true);
config.set("plasticity.stdp.learning_rate", 0.005);
config.set("plasticity.hebbian.learning_rate", 0.002);
```

## Working with the World

### Creating and Configuring Worlds

#### Basic World Setup

```python
from nlm_py import SimpleWorld

# Create world
world = SimpleWorld()

# Configure world dimensions
world.configure(width=100.0, height=100.0, visionWidth=20, visionHeight=20)

# Set agent properties
world.setAgentStart(50.0, 50.0)  # Starting position
world.setAgentMaxEnergy(1000.0)   # Maximum energy
world.setAgentEnergyDecayRate(0.01)  # Energy decay per timestep

# Set environmental properties
world.setTerrainType("flat")     # Terrain type
world.setWeatherConditions("clear")  # Weather

# Get world information
print(f"World size: {world.get_width()} x {world.get_height()}")
print(f"Agent start: ({world.getAgentStartX()}, {world.getAgentStartY()})")
```

#### World Simulation Loop

```python
from nlm_py import SimpleWorld, createSimpleWorld

# Create world
world = createSimpleWorld()
world.configure(100.0, 100.0, 20, 20)

# Simulation loop
for timestep in range(1000):
    # Get current sensory input
    percept = world.getSensoryPercept()
    
    # Update world state
    world.update(timestep * 0.1)
    
    # Check for events
    events = world.getEvents()
    for event in events:
        handle_world_event(event)
    
    # Apply environmental effects
    apply_environmental_effects()
    
    # Check if agent is still alive
    if not world.isAgentAlive():
        print("Agent died!")
        break
    
    # Periodic status output
    if timestep % 100 == 0:
        status = world.getStatus()
        print(f"Timestep {timestep}: {status}")
```

### Sensory Processing

#### Vision System

```python
from nlm_py import Vision

# Create vision system
vision = Vision()

# Configure vision parameters
vision.setFOV(90.0)                    # Field of view
vision.setNoiseLevel(0.01)             # Sensory noise
vision.setResolution(640, 480)          # Image resolution
vision.setColorMode("grayscale")       # Color processing

# Process visual input
import numpy as np
visual_input = np.random.randn(20, 20, 3)  # Simulated visual input
sensory_data = vision.processInput(visual_input)

print(f"Vision output shape: {sensory_data.shape}")
print(f"Vision intensity range: {sensory_data.min():.3f} - {sensory_data.max():.3f}")
```

#### Audio System

```python
from nlm_py import Audio

# Create audio system
audio = Audio()

# Configure audio parameters
audio.setSamplingRate(44100)    # Audio sampling rate
audio.setNoiseLevel(0.005)      # Auditory noise
audio.setDirectionality(True)  # Sound localization

# Process audio input
import numpy as np
audio_input = np.random.randn(1000)  # Simulated audio data
sensory_data = audio.processInput(audio_input)

print(f"Audio output: {sensory_data}")
```

## Neural Processing

### Neuron Management

#### Creating and Configuring Neurons

```python
from nlm_py import NeuronId, createNeuronId
from nlm_py import NeuronType

# Create neuron IDs
neuron_id = createNeuronId(42)
print(f"Neuron ID: {neuron_id}")
print(f"Neuron index: {neuron_id.index()}")

# Configure neuron properties
# (Assuming brain has neuron configuration methods)
# brain.configureNeuron(neuron_id, NeuronType.Internal)
# brain.setNeuronThreshold(neuron_id, -55.0)
# brain.setNeuronRestingPotential(neuron_id, -70.0)
```

#### Synaptic Operations

```python
from nlm_py import SynapseId, createSynapseId, SynapseType

# Create synapse IDs
synapse_id = createSynapseId(100)

# Configure synapse properties
# (Assuming brain has synapse configuration methods)
# brain.configureSynapse(synapse_id, SynapseType.Excitatory)
# brain.setSynapticWeight(synapse_id, 0.5)
# brain.setSynapticDelay(synapse_id, 5.0)
```

### Plasticity Management

#### STDP Plasticity

```python
from nlm_py import createBrain, Config

# Enable STDP plasticity
config = Config()
config.set("plasticity.stdp.enabled", True)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("plasticity.stdp.time_constant_pre", 20.0)
config.set("plasticity.stdp.time_constant_post", 20.0)

brain = createBrain(config)
brain.initialize()
```

#### Hebbian Plasticity

```python
# Enable Hebbian plasticity
config = Config()
config.set("plasticity.hebbian.enabled", True)
config.set("plasticity.hebbian.learning_rate", 0.005)
config.set("plasticity.hebbian.threshold", 0.1)
config.set("plasticity.hebbian.associability", 0.5)

brain = createBrain(config)
brain.initialize()
```

#### Structural Plasticity

```python
# Enable structural plasticity
config = Config()
config.set("plasticity.structural.enabled", True)
config.set("plasticity.structural.growth_rate", 0.001)
config.set("plasticity.structural.retraction_rate", 0.0005)
config.set("plasticity.structural.max_connections", 1000)

brain = createBrain(config)
brain.initialize()
```

## Neuromodulation

### Reward-Modulated Plasticity

```python
# Enable reward modulation
config = Config()
config.set("neuromodulation.reward.enabled", True)
config.set("neuromodulation.reward.strength", 1.0)
config.set("neuromodulation.reward.dopamine_level", 0.5)

brain = createBrain(config)
brain.initialize()
```

### Curiosity-Driven Exploration

```python
# Enable curiosity modulation
config = Config()
config.set("neuromodulation.curiosity.enabled", True)
config.set("neuromodulation.curiosity.strength", 0.8)
config.set("neuromodulation.curiosity.surprise_threshold", 0.5)

brain = createBrain(config)
brain.initialize()
```

### Prediction Error

```python
# Enable prediction error
config = Config()
config.set("neuromodulation.prediction_error.enabled", True)
config.set("neuromodulation.prediction_error.weight", 1.0)
config.set("neuromodulation.prediction_error.decay_rate", 0.9)

brain = createBrain(config)
brain.initialize()
```

## Development

### Developmental Stages

```python
from nlm_py import DevelopmentalStage, createBrain, Config

# Enable development
config = Config()
config.set("development.enabled", True)
config.set("development.rate", 0.001)

brain = createBrain(config)
brain.initialize()

# Set developmental stage
brain.setDevelopmentalStage(DevelopmentalStage.CriticalPeriod)
```

### Age-Dependent Plasticity

```python
# Configure age-dependent plasticity
config = Config()
config.set("development.age_dependent_plasticity", True)
config.set("development.plasticity_peak_age", 50.0)
config.set("development.plasticity_decay_rate", 0.01)

brain = createBrain(config)
brain.initialize()
```

## Memory Systems

### Working Memory

```python
# Enable working memory
config = Config()
config.set("memory.working_memory.enabled", True)
config.set("memory.working_memory.capacity", 100)
config.set("memory.working_memory.decay_rate", 0.01)
config.set("memory.working_memory.consolidation_strength", 0.1)

brain = createBrain(config)
brain.initialize()

# Use working memory
# brain.storeInWorkingMemory(pattern, strength)
# retrieved_pattern = brain.getWorkingMemory()
```

### Episodic Memory

```python
# Enable episodic memory
config = Config()
config.set("memory.episodic_memory.enabled", True)
config.set("memory.episodic_memory.capacity", 1000)
config.set("memory.episodic_memory.consolidation_rate", 0.001)
config.set("memory.episodic_memory.replay_rate", 0.1)

brain = createBrain(config)
brain.initialize()

# Record episodic memory
# brain.recordEpisodicMemory(event, context)
# replay = brain.replayEpisodicMemory(timestamp)
```

## Visualization

### Basic Visualization

```python
from nlm_py import VisualizationManager, createVisualizationManager
from nlm_py import VisualizationConfig

# Create visualization configuration
viz_config = VisualizationConfig()
viz_config.enabled = True
viz_config.enable_3d = True
viz_config.window_width = 1920
viz_config.window_height = 1080
viz_config.show_neurons = True
viz_config.show_synapses = True
viz_config.show_world = True

# Create visualization manager
viz = createVisualizationManager(brain, world)
viz.configure(viz_config)

# Start visualization
viz.start()

# Update visualization during simulation
for step in range(1000):
    brain.step(step)
    viz.update(step)

# Stop visualization
viz.stop()
```

### Advanced Visualization

```python
# Advanced visualization with custom settings
viz_config = VisualizationConfig()
viz_config.enabled = True
viz_config.enable_3d = True
viz_config.window_width = 1920
viz_config.window_height = 1080
viz_config.show_neurons = True
viz_config.show_synapses = True
viz_config.show_world = True
viz_config.neuron_size = 0.2
viz_config.synapse_alpha = 0.7
viz_config.refresh_rate = 60.0
viz_config.background_color = "dark_blue"
viz_config.neuron_color = "red"
viz_config.synapse_color = "blue"
viz_config.colormap = "viridis"

viz.configure(viz_config)
viz.start()
```

## Plugin System

### Plugin Management

```python
from nlm_py import PluginManager, createPluginManager

# Create plugin manager
plugin_manager = createPluginManager(brain)

# Discover available plugins
plugins = plugin_manager.discover_plugins()
for plugin in plugins:
    print(f"Found plugin: {plugin.name} v{plugin.version}")

# Load and enable a plugin
plugin_manager.load_plugin("novelty_plugin")
plugin_manager.enable_plugin("novelty_plugin")

# Configure plugin
plugin_manager.configure_plugin("novelty_plugin", {
    "enabled": True,
    "threshold": 0.5,
    "sensitivity": 1.0
})

# Get plugin state
state = plugin_manager.get_plugin_state("novelty_plugin")
print(f"Plugin active: {state.active}")
print(f"Plugin configuration: {state.config}")
```

### Built-in Plugins

The following plugins are included by default:

- **Visualization Plugin**: 3D visualization and rendering
- **Statistics Plugin**: Performance and neural statistics
- **Novelty Plugin**: Surprise and novelty detection
- **Reward Plugin**: Reward processing and modulation
- **Curiosity Plugin**: Exploration and curiosity
- **Prediction Plugin**: Prediction error processing
- **Memory Plugin**: Memory management and consolidation
- **Development Plugin**: Developmental processes
- **Plasticity Plugin**: Plasticity rule management
- **Neuromodulation Plugin**: Neuromodulator processing

## Performance Monitoring

### Basic Performance Monitoring

```python
from nlm_py import PerformanceMonitor, createPerformanceMonitor

# Create performance monitor
monitor = createPerformanceMonitor(brain)

# Start monitoring
monitor.start()

# Get performance metrics
metrics = monitor.getMetrics()
print(f"FPS: {metrics.fps}")
print(f"Memory usage: {metrics.memory_usage} MB")
print(f"CPU utilization: {metrics.cpu_utilization}%")
print(f"Spike count: {metrics.spike_count}")
print(f"Neuron count: {metrics.neuron_count}")
print(f"Synapse count: {metrics.synapse_count}")
print(f"Average firing rate: {metrics.average_firing_rate} Hz")
print(f"Active neurons: {metrics.active_neurons}")
print(f"Firing neurons: {metrics.firing_neurons}")
print(f"Energy usage: {metrics.energy_usage}")
print(f"Steps completed: {metrics.steps_completed}")

# Export performance data
monitor.exportToCSV("performance.csv")
monitor.exportToJSON("performance.json")

# Stop monitoring
monitor.stop()
```

### Advanced Performance Monitoring

```python
# Advanced performance monitoring with custom metrics
monitor = createPerformanceMonitor(brain)

# Add custom metrics
monitor.add_custom_metric("custom_metric_1", 42.0)
monitor.add_custom_metric("custom_metric_2", 3.14)

# Get detailed metrics
detailed_metrics = monitor.getDetailedMetrics()
print(f"Detailed metrics: {detailed_metrics}")

# Performance profiling
profile = monitor.startProfiling("simulation")

# Run simulation
for step in range(1000):
    brain.step(step)
    # Check for performance issues
    if monitor.isPerformanceCritical():
        print("Performance issue detected!")
        break

# Get profiling results
profiling_results = monitor.getProfilingResults()
profile.printResults()
```

## Statistics and Analytics

### Statistics Collection

```python
from nlm_py import StatisticsCollector, createStatisticsCollector

# Create statistics collector
stats = createStatisticsCollector(brain)

# Collect various statistics
stats.add_metric("firing_rate", brain.getAverageFiringRate())
stats.add_metric("active_neurons", brain.getActiveNeuronCount())
stats.add_metric("spike_count", brain.getTotalSpikeCount())
stats.add_metric("excitatory_inhibition_ratio", brain.getExcitationInhibitionRatio())
stats.add_metric("neuron_plasticity", get_neuron_plasticity())
stats.add_metric("synapse_strength", get_average_synapse_strength())

# Generate comprehensive report
report = stats.generate_report()
print(report)

# Export statistics
stats.export_to_json("simulation_stats.json")
stats.export_to_csv("simulation_stats.csv")
```

### Advanced Analytics

```python
# Advanced analytics with custom aggregation
stats = createStatisticsCollector(brain)

# Collect time-series data
for step in range(1000):
    brain.step(step)
    stats.add_metric(f"step_{step}_firing_rate", brain.getAverageFiringRate())
    stats.add_metric(f"step_{step}_active_neurons", brain.getActiveNeuronCount())

# Calculate statistics
firing_rates = stats.get_metric_values("step_*_firing_rate")
active_neurons = stats.get_metric_values("step_*_active_neurons")

mean_firing_rate = sum(firing_rates) / len(firing_rates)
std_firing_rate = calculate_standard_deviation(firing_rates)

print(f"Mean firing rate: {mean_firing_rate:.2f} Hz")
print(f"Standard deviation: {std_firing_rate:.2f} Hz")
print(f"Firing rate coefficient of variation: {std_firing_rate/mean_firing_rate:.3f}")
```

## Error Handling

### Custom Exceptions

```python
from nlm_py import NLMError, ConfigurationError, SimulationError, VisualizationError

# Error handling example
try:
    # Create brain with invalid configuration
    brain = createBrain(invalid_config)
    brain.initialize()
    
except ConfigurationError as e:
    print(f"Configuration error: {e}")
    # Handle configuration error
    
except SimulationError as e:
    print(f"Simulation error: {e}")
    # Handle simulation error
    
except VisualizationError as e:
    print(f"Visualization error: {e}")
    # Handle visualization error
    
except NLMError as e:
    print(f"NLM error: {e}")
    # Handle generic NLM error
    
except Exception as e:
    print(f"Unexpected error: {e}")
    # Handle unexpected error
```

## Examples

### Example 1: Basic Neural Network Simulation

```python
"""Example: Basic neural network simulation"""

import nlm_py
from nlm_py import Brain, Config, SimpleWorld, createBrain, createSimpleWorld

# Create configuration
config = Config()
config.set("simulation.max_steps", 1000)
config.set("brain.global_connectivity", 0.05)
config.set("plasticity.stdp.enabled", True)

# Create brain and world
brain = createBrain(config)
world = createSimpleWorld()
world.configure(100.0, 100.0, 20, 20)

# Initialize
brain.initialize()

# Run simulation
for step in range(1000):
    # Get sensory input
    percept = world.getSensoryPercept()
    brain.receiveSensoryInput(percept)
    
    # Step brain
    brain.step(step)
    
    # Apply action
    action = brain.produceAction()
    world.applyMotorCommand(action)
    
    # Update world
    world.update(step * 0.1)

print("Simulation completed")
```

### Example 2: Advanced Simulation with Monitoring

```python
"""Example: Advanced simulation with performance monitoring"""

import nlm_py
from nlm_py import Brain, Config, SimpleWorld
from nlm_py import PerformanceMonitor, StatisticsCollector

# Create components
config = Config()
brain = Brain(config)
world = SimpleWorld()

# Create monitoring tools
monitor = PerformanceMonitor(brain)
stats = StatisticsCollector(brain)

# Initialize
brain.initialize()

# Run simulation with monitoring
for step in range(1000):
    # Record statistics
    stats.add_metric("firing_rate", brain.getAverageFiringRate())
    stats.add_metric("active_neurons", brain.getActiveNeuronCount())
    
    # Step simulation
    brain.step(step)
    
    # Get performance metrics
    perf = monitor.getMetrics()
    
    # Periodic output
    if step % 100 == 0:
        print(f"Step {step}: FPS={perf.fps:.2f}, "
              f"Memory={perf.memory_usage:.2f}MB, "
              f"Active neurons={brain.getActiveNeuronCount()}")

# Generate final report
stats.generate_report()
print("Advanced simulation completed with monitoring")
```

### Example 3: Visualization Example

```python
"""Example: Simulation with 3D visualization"""

import nlm_py
from nlm_py import Brain, Config, SimpleWorld
from nlm_py import VisualizationManager

# Create components
config = Config()
brain = Brain(config)
world = SimpleWorld()

# Initialize brain
brain.initialize()

# Create visualization
viz = VisualizationManager(brain, world)
viz.configure(
    window_width=1920,
    window_height=1080,
    enable_3d=True,
    show_neurons=True,
    show_synapses=True,
    show_world=True,
    neuron_size=0.1,
    synapse_alpha=0.6
)

# Start visualization
viz.start()

# Run simulation with real-time visualization
for step in range(1000):
    brain.step(step)
    viz.update(step)

# Stop visualization
viz.stop()
print("Visualization simulation completed")
```

### Example 4: Plugin-Based Simulation

```python
"""Example: Plugin-based simulation with extended functionality"""

import nlm_py
from nlm_py import Brain, Config
from nlm_py import PluginManager, VisualizationPlugin, StatisticsPlugin

# Create brain
config = Config()
brain = Brain(config)

# Create plugin manager
plugin_manager = PluginManager(brain)

# Discover and load plugins
plugin_manager.discover_plugins()

# Enable specific plugins
plugin_manager.enable_plugin("visualization_plugin")
plugin_manager.enable_plugin("statistics_plugin")

# Configure plugins
plugin_manager.configure_plugin("visualization_plugin", {
    "enabled": True,
    "refresh_rate": 30,
    "show_neurons": True
})

plugin_manager.configure_plugin("statistics_plugin", {
    "enabled": True,
    "metrics": ["firing_rate", "spike_count", "energy"]
})

# Initialize and run
brain.initialize()

for step in range(1000):
    # Plugin-enhanced simulation step
    brain.step(step)
    
    # Get plugin-enhanced statistics
    enhanced_stats = plugin_manager.get_enhanced_statistics()
    
    # Check if plugins have generated insights
    if "novelty_insights" in enhanced_stats:
        handle_novelty_detection(enhanced_stats["novelty_insights"])

# Cleanup
plugin_manager.disable_all_plugins()
print("Plugin-based simulation completed")
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Import Errors
**Problem**: Cannot import nlm_py or its modules.

**Solution**: Ensure all dependencies are installed:

```bash
pip install numpy matplotlib pyopengl
```

#### Issue 2: Performance Issues
**Problem**: Simulation is slow or memory usage is high.

**Solutions**:
1. Use batch processing for large simulations
2. Monitor memory usage with PerformanceMonitor
3. Optimize configuration for your hardware
4. Use NumPy arrays for bulk operations

```python
# Use batch operations
brain.batch_steps(steps=range(1000))

# Monitor memory usage
monitor = PerformanceMonitor(brain)
```

#### Issue 3: Visualization Not Working
**Problem**: 3D visualization is not displaying correctly.

**Solutions**:
1. Check OpenGL support
2. Verify graphics drivers are updated
3. Try 2D fallback mode
4. Check system resources

```python
# Test OpenGL support
from nlm_py import test_opengl_support
if not test_opengl_support():
    print("OpenGL not available, falling back to 2D visualization")
    viz = VisualizationManager(brain)
    viz.enable_3d(False)
```

#### Issue 4: Configuration Errors
**Problem**: Configuration file cannot be loaded or parsed.

**Solutions**:
1. Check file format (JSON, YAML, or key=value)
2. Validate configuration syntax
3. Ensure required fields are present
4. Check file permissions

```python
from nlm_py import ConfigValidator

# Validate configuration
config = {"max_steps": 1000, "timestep": 0.1}
errors = ConfigValidator.validate_config(config)
if errors:
    for error in errors:
        print(f"Configuration error: {error}")
```

### Getting Help

- **Documentation**: Check the `nlm_utils.py` module for additional utilities
- **Examples**: Look at the `examples/` directory for complete working examples
- **Community**: Join the NLM discussion forum for help
- **Issues**: Report bugs in the GitHub repository

## Quick Reference

### Basic Commands

| Operation | Python | C++ |
|-----------|--------|-----|
| Create Brain | `createBrain(config)` | `createBrain(config)` |
| Initialize | `brain.initialize()` | `brain.initialize()` |
| Step Simulation | `brain.step(step)` | `brain.step(step)` |
| Get Firing Rate | `brain.getAverageFiringRate()` | `brain.getAverageFiringRate()` |
| Get Active Neurons | `brain.getActiveNeuronCount()` | `brain.getActiveNeuronCount()` |
| Get Spike Count | `brain.getTotalSpikeCount()` | `brain.getTotalSpikeCount()` |
| Save Config | `config.save_to_file("config.json")` | `config.saveToFile("config.json")` |
| Load Config | `config.load_from_file("config.json")` | `config.loadFromFile("config.json")` |

### Configuration Options

| Parameter | Default | Description |
|-----------|---------|-------------|
| simulation.max_steps | 10000 | Maximum simulation steps |
| simulation.timestep | 0.1 | Simulation timestep |
| brain.global_connectivity | 0.1 | Global connectivity probability |
| neuron_count | 100 | Number of neurons |
| region_count | 10 | Number of regions |
| simulation_timestep | 0.1 | Simulation timestep |
| connection_probability | 0.1 | Synaptic connection probability |

### Performance Metrics

The PerformanceMonitor tracks these metrics:

- **FPS**: Frames per second (simulation speed)
- **Memory Usage**: Memory consumption in MB
- **CPU Utilization**: CPU usage percentage
- **Spike Count**: Total number of spikes
- **Neuron Count**: Total number of neurons
- **Synapse Count**: Total number of synapses
- **Average Firing Rate**: Average firing rate in Hz
- **Active Neurons**: Number of active neurons
- **Firing Neurons**: Number of firing neurons
- **Energy Usage**: Energy consumption
- **Steps Completed**: Number of completed steps

### Keyboard Shortcuts (Visualization)

| Key | Action |
|-----|--------|
| ESC | Close visualization window |
| r | Toggle rotation |
| w | Toggle wireframe mode |
| s | Toggle show neurons |
| c | Toggle show synapses |
| v | Toggle show world |
| 1-5 | Change color map |
| Mouse Drag | Rotate view |
| Mouse Scroll | Zoom in/out |

## Conclusion

NLM provides a comprehensive framework for creating and simulating neural networks. Whether you're a researcher, developer, or student, NLM offers the tools you need to build sophisticated neural simulations with minimal effort.

The enhanced Python API makes it easy to get started while providing advanced features for complex simulations. The C++ core ensures maximum performance for production environments. The plugin system allows you to extend functionality without modifying the core code.

Start with the basic examples and gradually explore more advanced features as you become comfortable with the system. The documentation and examples will guide you through the full range of NLM's capabilities.

Happy simulating!
