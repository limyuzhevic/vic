# NLM Advanced Usage Guide
# Version 2.0.0
# Comprehensive documentation for advanced users and system administrators

## Overview

This document provides detailed information about the NLM (Neural Learning Machine) framework's advanced features, configuration options, and usage patterns. NLM is an experimental artificial developmental brain project that implements real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

## Table of Contents

1. [Introduction and Architecture](#introduction-and-architecture)
2. [Advanced Configuration](#advanced-configuration)
3. [Command Line Interface](#command-line-interface)
4. [Experiment Framework](#experiment-framework)
5. [Performance Optimization](#performance-optimization)
6. [Monitoring and Debugging](#monitoring-and-debugging)
7. [Python Bindings](#python-bindings)
8. [Visualization and Analysis](#visualization-and-analysis)
9. [Advanced Features](#advanced-features)
10. [Troubleshooting](#troubleshooting)
11. [Migration Guide](#migration-guide)

## Introduction and Architecture

### System Architecture

NLM implements a closed-loop brain system with the following components:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

### Key Design Principles

1. **Emergent Cognition**: Complex behaviors arise from simple neural interactions
2. **Integrated Systems**: Memory, prediction, and cognition work together seamlessly
3. **Developmental Trajectory**: Neural systems change over time through experience
4. **Neuromodulation-driven Learning**: Multiple neuromodulators coordinate learning
5. **Replay and Consolidation**: Memories are strengthened during rest periods

## Advanced Configuration

### Configuration File Format

NLM uses a flexible configuration system with support for:

#### Basic Key-Value Pairs
```ini
# Simple configuration
simulation_timestep = 0.001
neuron_count = 1000
```

#### Advanced Sections
```ini
[experiment.advanced_learning]
enabled = true
max_steps = 10000
reward_decay = 0.995
```

#### Complex Values
```ini
# Arrays and lists
objectives = ["reward", "novelty", "curiosity", "efficiency"]
constraint_violation_penalty = 1000.0

# Boolean values
performance_vectorization = true
stream_processing_enabled = false

# Floating-point values
learning_rate_adaptation = 0.005
exploration_epsilon = 0.1
```

### Configuration Categories

#### Neural Network Configuration

| Parameter | Description | Default | Range |
|-----------|-------------|---------|-------|
| `simulation_timestep` | Simulation time step in seconds | 0.001 | 0.0001 - 0.01 |
| `neuron_count` | Total number of neurons | 1000 | 100 - 10000 |
| `region_count` | Number of brain regions | 2 | 1 - 10 |
| `connection_probability` | Probability of synaptic connections | 0.1 | 0.01 - 1.0 |

#### Plasticity Configuration

| Parameter | Description | Default | Range |
|-----------|-------------|---------|-------|
| `stdp_ltp_weight` | STDP long-term potentiation weight | 0.01 | 0.001 - 0.1 |
| `stdp_ltd_weight` | STDP long-term depression weight | 0.012 | 0.001 - 0.1 |
| `stdp_time_constant` | STDP time constant in ms | 20.0 | 5.0 - 100.0 |
| `plasticity_learning_rate` | Hebbian learning rate | 0.01 | 0.001 - 0.1 |

#### Development Configuration

| Parameter | Description | Default | Range |
|-----------|-------------|---------|-------|
| `development_synaptogenesis_rate` | Rate of new synapse formation | 0.001 | 0.0001 - 0.01 |
| `development_pruning_rate` | Rate of synapse elimination | 0.0001 | 0.00001 - 0.001 |
| `development_maturation_rate` | Rate of neural maturation | 0.001 | 0.0001 - 0.01 |

#### Neuromodulation Configuration

| Parameter | Description | Default | Range |
|-----------|-------------|---------|-------|
| `dopamine_baseline` | Baseline dopamine level | 0.0 | 0.0 - 1.0 |
| `dopamine_peak` | Peak dopamine level | 0.0 | 0.0 - 1.0 |
| `dopamine_decay` | Dopamine decay rate | 0.8 | 0.5 - 0.95 |
| `novelty_threshold` | Novelty detection threshold | 0.7 | 0.0 - 1.0 |

#### Memory Configuration

| Parameter | Description | Default | Range |
|-----------|-------------|---------|-------|
| `working_memory_capacity` | Capacity of working memory | 100 | 10 - 1000 |
| `episodic_memory_capacity` | Capacity of episodic memory | 1000 | 100 - 10000 |
| `semantic_memory_capacity` | Capacity of semantic memory | 500 | 50 - 5000 |

#### Environment Configuration

| Parameter | Description | Default | Range |
|-----------|-------------|---------|-------|
| `environment_width` | World width in cells | 10 | 5 - 100 |
| `environment_height` | World height in cells | 10 | 5 - 100 |
| `environment_obstacle_density` | Density of obstacles (0.0-1.0) | 0.0 | 0.0 - 1.0 |
| `environment_reward_function` | Reward calculation function | "sparse" | "sparse", "dense", "shaped" |

### Configuration File Examples

#### Minimal Configuration (`minimal.cfg`)
```ini
random_seed = 42
simulation_timestep = 0.001
neuron_count = 100
region_count = 1
connection_probability = 0.1
```

#### Development Configuration (`development.cfg`)
```ini
random_seed = 12345
simulation_timestep = 0.0005
neuron_count = 2000
region_count = 3
connection_probability = 0.05

development_synaptogenesis_rate = 0.0005
development_pruning_rate = 0.00005
development_maturation_rate = 0.001

novelty_threshold = 0.7
curiosity_exploration_rate = 0.3
```

#### Production Configuration (`production.cfg`)
```ini
random_seed = 999
simulation_timestep = 0.001
neuron_count = 5000
region_count = 5
connection_probability = 0.03

# Performance optimization
performance_thread_count = 8
performance_numa_binding = true
performance_vectorization = true

# Logging
log_level = "INFO"
log_to_file = true
log_filename = "nlm_production.log"
log_file_rotation = true
log_file_max_size = "100 MB"
log_file_backup_count = 5

# Monitoring
metrics_enabled = true
metrics_collection_interval = 1.0
metrics_retention_days = 30
```

## Command Line Interface

### Basic Usage

```bash
# Run all experiments
nlm

# Run specific experiments
nlm connectivity
nlm plasticity
nlm stdp

# List available experiments
nlm --list

# Show help information
nlm --help

# Show version information
nlm --version

# Run with custom configuration
nlm --config advanced.cfg plasticity
```

### Advanced Command Line Options

| Option | Description | Example |
|--------|-------------|---------|
| `--config FILE` | Configuration file | `--config production.cfg` |
| `--help, -h` | Show help message | `nlm -h` |
| `--version` | Show version information | `nlm --version` |
| `--list` | List available experiments | `nlm --list` |

### Experiment Selection

You can run experiments individually or in any combination:

```bash
# Single experiment
nlm connectivity

# Multiple experiments
nlm connectivity plasticity stdp

# Experiment with custom configuration
nlm --config development.cfg plasticity stdp

# All experiments (default)
nlm
```

### Exit Codes

| Exit Code | Description |
|-----------|-------------|
| 0 | All experiments completed successfully |
| 1 | One or more experiments failed |
| 2 | Configuration error |
| 3 | Command line parsing error |
| 4 | System initialization error |

## Experiment Framework

### Overview

The experiment framework provides a structured approach to running and analyzing neural experiments. Each experiment has:

1. **Metadata**: Name, description, configuration
2. **Execution**: Run method that performs the experiment
3. **Results**: Metrics and data collected during execution
4. **Persistence**: Save/load functionality for experiments

### Available Experiments

#### Connectivity Experiment

Tests basic neural connectivity with LIF dynamics:

- **Purpose**: Verify that spikes propagate through neural networks
- **Key Metrics**: Spike generation, firing rates, network connectivity
- **Parameters**: Current injection, simulation steps, neuron thresholds

#### Plasticity Experiment

Demonstrates synaptic plasticity through experience:

- **Purpose**: Show how synaptic weights change through Hebbian and STDP learning
- **Key Metrics**: Weight changes, learning curves, synaptic consolidation
- **Parameters**: Learning rates, simulation duration, input patterns

#### STDP Experiment

Verifies spike-timing dependent plasticity:

- **Purpose**: Test that pre-before-post activity potentiates and post-before-pre depresses
- **Key Metrics**: Potentiation/depression ratios, timing sensitivity
- **Parameters**: Spike timing, synaptic weights, learning rules

### Custom Experiments

You can create custom experiments by extending the `Experiment` base class:

```cpp
class CustomExperiment : public Experiment {
public:
    CustomExperiment() : Experiment("custom") {
        setName("Custom Neural Experiment");
        addNote("Custom experiment for testing neural dynamics");
    }
    
    bool run(std::shared_ptr<Brain> brain) override {
        // Custom experiment logic here
        return true;
    }
};
```

### Experiment Configuration

Experiments can be configured through command line arguments and configuration files:

```bash
# Example: Run plasticity experiment with custom configuration
nlm --config custom.cfg --plasticity-steps 5000 --learning-rate 0.01
```

## Performance Optimization

### Performance Tuning Parameters

| Parameter | Effect | Recommended Value |
|-----------|--------|-------------------|
| `performance_thread_count` | Number of processing threads | 4-8 | 4-8 |
| `performance_numa_binding` | Enable NUMA-aware memory access | true | true |
| `performance_vectorization` | Enable SIMD instructions | true | true |
| `performance_cache_size` | CPU cache size allocation | 256 MB | 256 MB - 1 GB |
| `parallel_threads` | Number of parallel threads | 8 | 4-16 |

### Memory Optimization

#### Memory Management Configuration

```ini
# Memory pools for improved performance
memory_pool_enabled = true
memory_pool_size = 64 MB
memory_pool_align = 64

# Object pooling for frequently created/destroyed objects
object_pool_enabled = true
object_pool_max_size = 1000
```

#### Memory Efficiency Strategies

1. **Object Pooling**: Reuse neural and synaptic objects
2. **Memory Mapping**: Use memory-mapped files for large datasets
3. **Garbage Collection**: Disable GC in hot paths, use custom allocators
4. **NUMA Awareness**: Bind memory to appropriate NUMA nodes

### Performance Profiling

#### Built-in Profiling Tools

The framework includes several profiling tools:

1. **Time Profiling**: Measure execution time of specific operations
2. **Memory Profiling**: Track memory allocation and deallocation
3. **CPU Profiling**: Profile CPU usage and cache efficiency
4. **Network Profiling**: Measure I/O performance

#### Example Profiling Configuration

```ini
debug_enabled = true
debug_level = "DETAILED"
debug_profiling = true
debug_memory_tracking = true
debug_performance_monitoring = true

debug_profiling_snapshots = 10
debug_memory_threshold_mb = 512
debug_performance_threshold_ms = 100
```

## Monitoring and Debugging

### Monitoring Framework

#### Metrics Collection

NLM provides comprehensive metrics collection:

1. **System Metrics**: CPU, memory, network usage
2. **Neural Metrics**: Spike rates, firing patterns, connectivity
3. **Learning Metrics**: Weight changes, convergence rates, learning curves
4. **Behavioral Metrics**: Action selection, reward accumulation, exploration

#### Metrics Configuration

```ini
metrics_enabled = true
metrics_collection_interval = 1.0  # seconds
metrics_retention_days = 30
metrics_alert_threshold = 0.95  # alert when metrics exceed threshold
metrics_export_format = "prometheus"  # supported: "prometheus", "json", "csv"
```

### Debugging Tools

#### Debug Configuration

```ini
# Debug level controls verbosity
debug_level = "DETAILED"  # Options: OFF, ERROR, WARNING, INFO, DEBUG, DETAILED

debug_enabled = true

# Specific debug features
debug_profiling = true
debug_memory_tracking = true
debug_performance_monitoring = true
debug_breakpoints = [
    "plasticity_update",
    "memory_consolidation", 
    "development_stage_transition",
    "prediction_error_calculation"
]
```

#### Debug Commands

```bash
# Enable detailed debugging
nlm --debug-level DETAILED --debug-profiling

# Set specific breakpoints
nlm --debug-breakpoints plasticity_update memory_consolidation

# Export debug data
nlm --debug-export debug_data.json
```

## Python Bindings

### Overview

NLM provides Python bindings through the `pynlm` package, allowing users to:

1. **Access Neural Systems**: Interact with brain regions, neurons, and synapses
2. **Run Experiments**: Execute experiments programmatically
3. **Configure Systems**: Modify configuration and parameters
4. **Visualize Results**: Generate plots and analyze data

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm as nlm

# Create and configure brain
brain = nlm.Brain()
brain.config.set("neuron_count", 1000)
brain.config.set("simulation_timestep", 0.001)

# Initialize brain
brain.initialize()

# Run experiment
brain.run_experiment("connectivity")

# Access results
print(f"Total spikes: {brain.get_total_spike_count()}")
print(f"Final firing rate: {brain.get_average_firing_rate():.2f}")
```

### Advanced Python Features

#### Experiment Management

```python
# Create custom experiment
class CustomPythonExperiment(nlm.Experiment):
    def __init__(self):
        super().__init__("custom_python")
        self.add_note("Custom experiment implemented in Python")
    
    def run(self, brain):
        # Custom experiment logic
        for step in range(1000):
            brain.step(step, step * 0.001)
        return True

# Run custom experiment
experiment = CustomPythonExperiment()
result = brain.run_experiment(experiment)
```

#### Configuration Management

```python
# Load configuration from file
brain.config.load_from_file("advanced.cfg")

# Override configuration
brain.config.set("neuron_count", 2000, nlm.ConfigSource.RUNTIME)
brain.config.set("novelty_threshold", 0.8, nlm.ConfigSource.RUNTIME)

# Get configuration values
current_timestep = brain.config.get_or("simulation_timestep", 0.001)
```

#### Real-time Interaction

```python
# Real-time monitoring
brain.start_monitoring(1.0)  # Update every 1 second

while brain.is_monitoring():
    metrics = brain.get_current_metrics()
    print(f"Spikes: {metrics['total_spikes']}, "
          f"Firing: {metrics['firing_rate']:.2f}")
    time.sleep(1)

brain.stop_monitoring()
```

### Python API Reference

#### Core Classes

| Class | Description |
|-------|-------------|
| `nlm.Brain` | Main neural brain interface |
| `nlm.Config` | Configuration management |
| `nlm.Experiment` | Base experiment class |
| `nlm.ExperimentRunner` | Experiment management and execution |
| `nlm.Logger` | Logging framework |

#### Key Methods

```python
# Brain methods
brain = nlm.Brain()
brain.initialize()           # Initialize brain
brain.step(step, time)       # Simulation step
brain.inject_current(neuron_id, current)  # Inject current
brain.run_experiment(name)   # Run experiment
brain.save("checkpoint")     # Save brain state
brain.load("checkpoint")     # Load brain state

# Configuration methods
brain.config.set("key", value)
brain.config.get("key")
brain.config.load_from_file("config.cfg")
brain.config.save_to_file("current.cfg")

# Experiment methods
brain.run_experiment("connectivity")
brain.run_experiment(experiment)  # Custom experiment object
brain.list_experiments()          # Get list of available experiments
```

## Visualization and Analysis

### Visualization Framework

NLM provides comprehensive visualization capabilities:

#### Built-in Visualizers

1. **Network Visualization**: Graph-based neuron and synapse representation
2. **Spike Raster Plots**: Visualize spike timing and patterns
3. **Weight Dynamics**: Track synaptic weight changes over time
4. **Learning Curves**: Plot learning progress and convergence
5. **Development Trajectories**: Visualize neural development over time

#### Configuration

```ini
visualization_enabled = true
visualization_type = "3D"  # Options: "2D", "3D", "interactive"
visualization_update_rate = 60.0  # FPS
visualization_render_mode = "raytraced"  # Options: "software", "hardware", "raytraced"
visualization_quality = "high"  # Options: "low", "medium", "high"
visualization_max_frames = 1000
```

#### Python Visualization

```python
import pynlm as nlm
import matplotlib.pyplot as plt

# Create visualizer
visualizer = nlm.Visualizer(brain)

# Plot spike raster
spike_data = brain.get_spike_data()
visualizer.plot_spike_raster(spike_data)
plt.savefig("spike_raster.png")

# Plot weight dynamics
weight_data = brain.get_synaptic_weights()
visualizer.plot_weight_dynamics(weight_data)
plt.savefig("weight_dynamics.png")

# Show interactive plot
visualizer.show_interactive()
```

### Analysis Tools

#### Built-in Analyzers

1. **Statistical Analysis**: Compute statistics on neural data
2. **Pattern Detection**: Identify patterns in spike trains and activity
3. **Machine Learning**: Apply ML algorithms to neural data
4. **Comparative Analysis**: Compare different experimental conditions

#### Configuration

```ini
analysis_enabled = true
analysis_window_size = 1000  # Number of steps for analysis windows
analysis_overlap = 0.5        # Overlap between analysis windows
analysis_methods = ["statistics", "pattern_detection", "ml"]
```

#### Python Analysis

```python
import pynlm as nlm

# Create analyzer
analyzer = nlm.Analyzer(brain)

# Statistical analysis
stats = analyzer.compute_statistics("spike_trains")
print(f"Mean firing rate: {stats['mean_firing_rate']:.2f} Hz")
print(f"Fano factor: {stats['fano_factor']:.2f}")

# Pattern detection
patterns = analyzer.detect_patterns("spike_raster")
print(f"Detected patterns: {len(patterns)}")
for i, pattern in enumerate(patterns):
    print(f"  Pattern {i}: {pattern.type} with {pattern.confidence:.2f} confidence")

# Machine learning
ml_results = analyzer.apply_ml_model("spike_features", "classification")
print(f"ML accuracy: {ml_results['accuracy']:.2f}")
```

## Advanced Features

### Multi-objective Optimization

NLM supports multi-objective optimization for balancing competing objectives:

```ini
objectives = ["reward", "novelty", "curiosity", "efficiency"]
objective_weights = [0.4, 0.2, 0.2, 0.2]
constraint_violation_penalty = 1000.0
```

### Hierarchical Learning

Implement hierarchical learning with multiple abstraction levels:

```ini
# Level 1: Basic motor skills
[hierarchical.level1]
enabled = true
neurons = 100
tasks = ["move_forward", "turn_left", "turn_right"]

# Level 2: Complex behaviors
[hierarchical.level2]
enabled = true
neurons = 500
tasks = ["navigate", "avoid_obstacles", "reach_target"]

# Level 3: Abstract reasoning
[hierarchical.level3]
enabled = false
neurons = 2000
tasks = ["plan", "reason", "generalize"]
```

### Transfer Learning

Enable transfer learning between tasks:

```ini
transfer_learning_enabled = true
transfer_learning_rate = 0.3
source_task_weights = [0.5, 0.3, 0.2]
target_task_adaptation = "fast"  # Options: "slow", "medium", "fast"
```

### Social Learning

Implement social learning and imitation:

```ini
social_learning_enabled = true
social_network_size = 10
imitation_probability = 0.3
vicarious_reinforcement_enabled = true
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Experiment Fails to Initialize

**Symptoms**:
- Brain initialization fails
- Configuration errors
- Missing dependencies

**Solutions**:
1. Check configuration file syntax and completeness
2. Verify all required parameters are set
3. Ensure build includes all necessary components
4. Check system resources (memory, CPU)

#### Issue 2: Memory Usage Too High

**Symptoms**:
- High memory consumption
- System swapping
- Crashes due to memory exhaustion

**Solutions**:
1. Reduce neuron count or region count
2. Enable memory pooling
3. Disable memory-intensive features
4. Increase system memory or use memory-mapped files

#### Issue 3: Performance Issues

**Symptoms**:
- Slow simulation speed
- Long experiment execution time
- Poor responsiveness

**Solutions**:
1. Increase thread count
2. Enable vectorization
3. Disable non-essential features
4. Use smaller simulation timesteps

#### Issue 4: Visualization Problems

**Symptoms**:
- Graphics rendering errors
- Performance issues in visualization
- Missing visual elements

**Solutions**:
1. Reduce visualization quality
2. Disable hardware acceleration
3. Increase visualization update rate
4. Check graphics driver compatibility

### Debugging Commands

#### Command Line Debugging

```bash
# Enable detailed debugging
nlm --debug-level DEBUG --debug-profiling --debug-memory-tracking

# Set specific breakpoints
nlm --debug-breakpoints plasticity_update development_stage_transition

# Export debug information
nlm --debug-export debug_data.tar.gz
```

#### Python Debugging

```python
import pynlm as nlm
import logging

# Enable Python logging
logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger("nlm")

# Enable brain debugging
brain.enable_debugging(True)
brain.set_debug_level("DETAILED")

# Set Python breakpoints
brain.set_debug_breakpoint("plasticity_update")
brain.set_debug_breakpoint("memory_consolidation")
```

#### Remote Debugging

```bash
# SSH into the system running NLM
ssh user@host -L 8080:localhost:8080

# Connect to debugging server
curl http://localhost:8080/debug
```

### Error Messages

#### Common Error Messages

| Error Message | Cause | Solution |
|---------------|-------|----------|
| "Failed to initialize brain!" | Brain initialization failure | Check configuration and system resources |
| "Configuration file not found" | Missing config file | Provide correct path with --config option |
| "Memory allocation failed" | Out of memory | Reduce simulation size or increase system memory |
| "Invalid configuration value" | Invalid parameter value | Check configuration file format |
| "Division by zero" | Mathematical error | Check simulation parameters |

## Migration Guide

### From Previous Versions

#### Version 1.x to 2.0.x

**Key Changes**:

1. **New Experiment Framework**: Replaced old test functions with structured experiment system
2. **Enhanced Configuration**: Added support for advanced configuration sections and file-based configuration
3. **Improved Command Line Interface**: New command-line parser with better option handling
4. **Performance Optimizations**: Added thread pooling, memory management, and vectorization
5. **New Debugging Tools**: Comprehensive debugging and profiling framework

**Migration Steps**:

1. **Update Configuration**:
   - Convert old key=value format to new INI-style format
   - Add missing configuration sections
   - Update parameter names to match new schema

2. **Update Command Line Usage**:
   - Replace old test function calls with experiment names
   - Use new configuration options
   - Add `--help` or `--version` for usage information

3. **Update Code**:
   - Replace old test functions with new experiment classes
   - Update experiment execution to use ExperimentRunner
   - Add proper error handling and validation

4. **Performance Tuning**:
   - Enable performance optimizations
   - Configure memory management
   - Set up monitoring and debugging

#### Example Migration

**Old Version (Version 1.x)**:

```cpp
// Old main.cpp
int main() {
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run hardcoded tests
    runBasicConnectivityTest(brain);
    runPlasticityExperiment(brain);
    runStdpVerification(brain);
    
    return 0;
}
```

**New Version (Version 2.0.x)**:

```cpp
// New main.cpp
int main(int argc, char** argv) {
    // Parse command line arguments
    CommandLineParser parser(argc, argv);
    parser.parse();
    
    if (parser.isHelpRequested()) {
        parser.printHelp();
        return 0;
    }
    
    // Initialize system
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Load configuration
    auto config = std::make_shared<Config>();
    // ... configuration loading ...
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    // Run experiments based on command line arguments
    std::vector<std::string> experimentsToRun;
    if (parser.getSelectedExperiments().empty()) {
        experimentsToRun = {"connectivity", "plasticity", "stdp"};
    } else {
        experimentsToRun = parser.getSelectedExperiments();
    }
    
    for (const auto& expName : experimentsToRun) {
        std::shared_ptr<Experiment> experiment;
        if (expName == "connectivity") {
            experiment = std::make_shared<ConnectivityExperiment>();
        } else if (expName == "plasticity") {
            experiment = std::make_shared<PlasticityExperiment>();
        } else if (expName == "stdp") {
            experiment = std::make_shared<StdpExperiment>();
        }
        
        ExperimentRunner runner;
        auto environment = std::make_shared<Environment>("TestEnvironment", 10, 10);
        runner.runExperiment(experiment, brain, environment, 1000);
    }
    
    return 0;
}
```

### Backward Compatibility

NLM 2.0.x maintains backward compatibility with the following:

1. **Command Line Interface**: Most command line options work the same
2. **Configuration Format**: Basic key=value format still supported
3. **Experiment Functionality**: All original experiments produce same results
4. **Python Bindings**: Core API maintained with additions

**Note**: Some deprecated features may be removed in future versions. Check the release notes for details.

## Conclusion

NLM 2.0.0 represents a significant advancement in artificial neural computation, offering:

1. **Structured Experiment Framework**: Clean separation of concerns and improved maintainability
2. **Enhanced Configuration System**: Flexible configuration with multiple formats and sections
3. **Advanced Command Line Interface**: Comprehensive option handling and user-friendly interface
4. **Performance Optimizations**: Threading, memory management, and vectorization
5. **Comprehensive Monitoring**: Detailed metrics collection and analysis
6. **Rich Visualization**: Multiple visualization options and interactive tools
7. **Powerful Python Bindings**: Seamless integration with Python ecosystem
8. **Robust Debugging Tools**: Extensive debugging and profiling capabilities

The framework is designed to support research in artificial intelligence, neuroscience, and cognitive science, providing a flexible platform for exploring complex neural systems and behaviors.

---

*This documentation is continuously updated. For the latest version, visit the NLM documentation portal.*