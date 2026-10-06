# NLM Phase 2: Quick Start Guide

## 🚀 Getting Started with Enhanced NLM

This guide provides quick access to the most important features of the enhanced NLM Phase 2 system.

## 1. Quick Installation and Setup

### Prerequisites:
- C++20 compiler
- CMake 3.16 or higher
- Python 3.7+ (for Python bindings)

### Build Instructions:
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4
```

### Verify Installation:
```bash
# Run basic connectivity test
./nlm --mode basic --steps 1000

# Run plasticity experiment
./nlm --mode plasticity --steps 5000 --neurons 2000
```

## 2. Core Command-Line Interface

### Basic Usage:
```bash
# Show help information
./nlm --help

# Show version information
./nlm --version

# Run with custom configuration
./nlm --mode integration --timestep 0.0005 --save-brain brain.checkpoint
```

### Available Simulation Modes:

| Mode | Description | Typical Use Case |
|------|-------------|------------------|
| `basic` | Basic neural connectivity test | Initial testing and validation |
| `plasticity` | Plasticity learning experiment | Synaptic adaptation studies |
| `stdp` | STDP verification experiment | Spike-timing-dependent plasticity |
| `integration` | Full integration test | Complete system validation |
| `development` | Developmental cycle simulation | Developmental biology studies |
| `neuromod` | Neuromodulation experiment | Dopamine and curiosity systems |
| `memory` | Memory systems test | Working memory validation |
| `custom` | Custom simulation with parameters | Research experiments |

### Key Command-Line Options:

| Option | Description | Example |
|--------|-------------|---------|
| `--mode MODE` | Simulation mode | `--mode plasticity` |
| `--steps STEPS` | Number of simulation steps | `--steps 5000` |
| `--neurons N` | Number of neurons | `--neurons 2000` |
| `--regions R` | Number of brain regions | `--regions 2` |
| `--timestep T` | Simulation timestep in seconds | `--timestep 0.0005` |
| `--log-level LEVEL` | Logging level | `--log-level DEBUG` |
| `--output-dir DIR` | Output directory | `--output-dir ./experiments` |
| `--save-brain FILE` | Save brain state | `--save-brain brain.checkpoint` |
| `--load-brain FILE` | Load brain state | `--load-brain saved_brain.checkpoint` |
| `--benchmark` | Run performance benchmarks | `--benchmark` |
| `--verbose, -V` | Enable verbose output | `-V` |
| `--quiet, -Q` | Suppress informational output | `-Q` |

## 3. Advanced Configuration

### Configuration File Format:
```ini
# NLM Configuration File
# Example for research experiments

random_seed = 42
simulation_timestep = 0.0005
neuron_count = 5000
region_count = 2
connection_probability = 0.15
plasticity_learning_rate = 0.05
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.03
stdp_time_constant = 20.0
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
dopamine_baseline = 0.1
reward_discount_factor = 0.99
environment_name = GridWorld
log_level = INFO
log_to_file = true
max_simulation_steps = 10000
visualization_enabled = true
```

### Command-Line Configuration:
```bash
# Override configuration via command line
./nlm --mode plasticity --steps 10000 --neurons 10000 --timestep 0.001

# Load configuration from file
./nlm --mode integration --config my_config.cfg

# Save configuration to file
./nlm --mode development --save-brain brain.checkpoint --config export_config.cfg
```

## 4. Python Integration

### Basic Python Usage:
```python
#!/usr/bin/env python3
"""Example Python script for NLM interaction"""

from nlm import AdvancedConfig, AdvancedExperiment, NeuralVisualization, RealTimeMonitor

# Create advanced configuration
config = AdvancedConfig.createPreset("research")
config.setParameter("neuron_count", 10000)
config.setParameter("simulation_timestep", 0.0001)

# Create brain
brain = createBrain(config)

# Run experiment
experiment = AdvancedExperiment(brain, world)
results = experiment.runExperimentSuite("plasticity_learning", {
    "steps": 5000,
    "learningRate": 0.02,
    "neuronCount": 10000
})

# Print results
print(f"Experiment completed: {results['success']}")
print(f"Final spike count: {results['finalSpikeCount']}")
print(f"Average firing rate: {results['averageFiringRate']}")

# Visualize results
viz = NeuralVisualization(brain)
connectivity = viz.getConnectivityMatrix()
activity = viz.getActivityHeatmap()

# Monitor performance
monitor = RealTimeMonitor(brain)
status = monitor.getSystemStatus()
print(f"System status: {status}")
```

### Python Configuration Examples:
```python
# Create default configuration
from nlm import AdvancedConfig
config = AdvancedConfig()

# Set parameters
config.setParameter("neuron_count", 5000)
config.setParameter("simulation_timestep", 0.001)
config.setParameter("plasticity_learning_rate", 0.05)

# Validate configuration
validation = config.validate()
if validation["valid"]:
    print("Configuration is valid")
    # Save configuration
    config.saveToFile("my_config.json")
```

## 5. Performance Monitoring

### Real-Time Monitoring:
```python
from nlm import RealTimeMonitor

monitor = RealTimeMonitor(brain)

# Start monitoring with callback
monitor.startMonitoring(1.0, lambda status: print(f"Status: {status}"))

# Get current system status
status = monitor.getSystemStatus()
print(f"Average firing rate: {status['averageFiringRate']}")
print(f"Total spikes: {status['totalSpikes']}")
print(f"Active neurons: {status['activeNeuronCount']}")

# Control simulation
monitor.controlSimulation("inject_current", 25.0)
monitor.controlSimulation("reset")

# Stop monitoring
monitor.stopMonitoring()
```

### Performance Benchmarks:
```bash
# Run benchmark mode
./nlm --mode integration --benchmark

# Analyze performance
./nlm --mode plasticity --benchmark --analyze-activity

# Compare performance across modes
./nlm --mode basic --benchmark
./nlm --mode integration --benchmark
```

## 6. Experiment Management

### Running Specific Experiments:
```bash
# Basic connectivity test
./nlm --mode basic --steps 1000 --verbose

# Plasticity learning experiment
./nlm --mode plasticity --steps 5000 --neurons 2000 --log-level INFO

# STDP verification
./nlm --mode stdp --steps 2000 --verbose

# Full integration test
./nlm --mode integration --timestep 0.0005 --save-brain brain.checkpoint

# Development cycle simulation
./nlm --mode development --benchmark --analyze-activity

# Neuromodulation experiment
./nlm --mode neuromod --steps 3000 --verbose

# Memory systems test
./nlm --mode memory --steps 2000 --verbose

# Custom experiment
./nlm --mode custom --steps 10000 --neurons 5000 --regions 3
```

### Experiment Results Analysis:
```python
# Load experiment results
from nlm import AdvancedExperiment

experiment = AdvancedExperiment(brain, world)
results = experiment.loadResults("experiment_results.json")

# Analyze results
print(f"Experiment type: {results['experimentType']}")
print(f"Duration: {results['duration']} seconds")
print(f"Success: {results['success']}")

# Get performance metrics
if 'performanceMetrics' in results:
    metrics = results['performanceMetrics']
    print(f"Average firing rate: {metrics['averageFiringRate']}")
    print(f"Total spikes: {metrics['totalSpikes']}")
```

## 7. Error Handling and Debugging

### Python Error Handling:
```python
from nlm import AdvancedErrorHandler

error_handler = AdvancedErrorHandler()

# Log error with context
try:
    # Your code here
    result = risky_operation()
except Exception as e:
    error_handler.logError(
        type(e).__name__,
        str(e),
        "my_function()"
    )

# Check error history
if error_handler.hasError("RuntimeError"):
    last_error = error_handler.getLastError()
    print(f"Last error: {last_error['message']}")

# Get error statistics
stats = error_handler.getErrorStatistics()
print(f"Total errors: {stats['totalErrors']}")

# Clear error history
error_handler.clearErrorHistory()
```

### Command-Line Error Handling:
```bash
# Run with different log levels for debugging
./nlm --mode plasticity --log-level DEBUG --verbose

# Run with quiet mode for production
./nlm --mode integration --log-level INFO --quiet

# Run with error checking
./nlm --mode development --benchmark 2>&1 | grep ERROR
```

## 8. Advanced Usage

### Configuration Presets:
```python
from nlm import AdvancedConfig

# Development preset for research
research_config = AdvancedConfig.createPreset("research")

# Production preset for deployment
production_config = AdvancedConfig.createPreset("production")

# Research preset for scientific experiments
experimental_config = AdvancedConfig.createPreset("research")
```

### Visualization and Analysis:
```python
from nlm import NeuralVisualization

viz = NeuralVisualization(brain)

# Generate connectivity matrix
connectivity = viz.getConnectivityMatrix()

# Get activity heatmap
activity = viz.getActivityHeatmap()

# Get time series data
time_series = viz.getTimeSeriesData(steps=100)

# Create network graph
network = viz.getNetworkGraph()

# Create animation frames
frames = viz.createAnimationFrames(duration=60, fps=10)
```

### Machine Learning Integration:
```python
from nlm import NeuralMLModel

model = NeuralMLModel(brain)

# Train with reinforcement learning
model.trainWithReinforcementLearning(episodes=100, learningRate=0.01)

# Predict activity
input_data = [0.1, 0.2, 0.3, 0.4, 0.5]
prediction = model.predictActivity(input_data)

# Get performance metrics
metrics = model.getPerformanceMetrics()
print(f"Training accuracy: {metrics['trainingAccuracy']}")
print(f"Convergence rate: {metrics['convergenceRate']}")
```

## 9. Troubleshooting

### Common Issues and Solutions:

#### 1. Memory Errors
**Problem**: "Out of memory" during simulation
**Solution**: 
```bash
# Reduce simulation size
./nlm --mode plasticity --steps 1000 --neurons 1000

# Increase system memory or use swap
```

#### 2. Configuration Errors
**Problem**: "Invalid configuration" 
**Solution**:
```bash
# Use default configuration
./nlm --mode basic

# Check configuration with validation
./nlm --mode custom --validate-config
```

#### 3. Performance Issues
**Problem**: "Simulation too slow"
**Solution**:
```bash
# Increase timestep
./nlm --mode plasticity --timestep 0.01

# Reduce simulation steps
./nlm --mode integration --steps 1000
```

#### 4. Python Integration Issues
**Problem**: "Module not found" or import errors
**Solution**:
```python
# Install Python dependencies
pip install -r requirements.txt

# Check Python path
python -c "import sys; print(sys.path)"
```

## 10. Advanced Features

### Feature Matrix:

| Feature | C++ Command-Line | Python API | Description |
|---------|------------------|------------|-------------|
| Basic Connectivity | `--mode basic` | `run_basic_test()` | Neural signal propagation |
| Plasticity Learning | `--mode plasticity` | `run_plasticity_experiment()` | Synaptic weight adaptation |
| STDP Verification | `--mode stdp` | `run_stdp_experiment()` | Spike-timing-dependent plasticity |
| Full Integration | `--mode integration` | `run_integration_test()` | Complete system validation |
| Development Cycle | `--mode development` | `run_development_test()` | Developmental biology studies |
| Neuromodulation | `--mode neuromod` | `run_neuromodulation_test()` | Dopamine and curiosity systems |
| Memory Systems | `--mode memory` | `run_memory_test()` | Working memory validation |
| Custom Experiments | `--mode custom` | `run_custom_experiment()` | User-defined experiments |
| Configuration | `--config FILE` | `load_config()` | Load configuration from file |
| Performance | `--benchmark` | `benchmark()` | Performance measurement |
| Visualization | `--analyze-activity` | `visualize()` | Network analysis and visualization |
| Error Handling | `--verbose` | `error_handler` | Comprehensive error reporting |
| Real-time Monitoring | `--help` | `monitor` | Live system monitoring |

## 11. Quick Reference

### Essential Commands:
```bash
# Help and version
./nlm --help
./nlm --version

# Quick tests
./nlm --mode basic --steps 100
./nlm --mode plasticity --steps 1000

# Production runs
./nlm --mode integration --timestep 0.001 --save-brain brain.checkpoint

# Research experiments
./nlm --mode custom --steps 10000 --neurons 5000 --regions 3
```

### Essential Python Commands:
```python
# Basic setup
from nlm import AdvancedConfig, AdvancedExperiment

# Create configuration
config = AdvancedConfig.createPreset("research")

# Run experiment
experiment = AdvancedExperiment(brain, world)
results = experiment.runExperimentSuite("plasticity_learning", {"steps": 2000})

# Get results
print(f"Success: {results['success']}")
print(f"Spikes: {results['finalSpikeCount']}")
```

## 12. Getting Help

### Documentation:
- **Main Documentation**: `docs/PH2_IMPLEMENTATION_COMPLETE.md`
- **Implementation Guide**: `docs/IMPLEMENTATION_GUIDE.md`
- **Quick Start**: This guide
- **API Reference**: `python/advanced_bindings.cpp`

### Support:
- **GitHub Issues**: Report bugs and feature requests
- **Documentation**: Comprehensive API documentation
- **Examples**: Working code examples in repository

## Conclusion

This quick start guide provides immediate access to the enhanced NLM Phase 2 system. The new platform offers:

- **15+ command-line options** for flexible simulation control
- **8 experiment modes** for comprehensive neural testing
- **Advanced Python integration** for research workflows
- **Real-time monitoring** for system optimization
- **Comprehensive error handling** for reliable operation
- **Detailed documentation** for easy adoption

The enhanced NLM Phase 2 system is ready for immediate use in:
- **Academic research** and neural network studies
- **Industrial applications** and production deployments
- **Educational use** and neural network teaching
- **Scientific discovery** and advanced experimentation

Start exploring the enhanced NLM Phase 2 system today and experience the next generation of neural simulation capabilities!