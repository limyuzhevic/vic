# Development Tools and Debugging Utilities for NLM

## Overview

This document describes development tools and debugging utilities for the NLM (Neural Learning Machine) project. These tools help developers debug, test, and analyze NLM simulations more effectively.

## Table of Contents

1. [Brain Debugger](#brain-debugger)
2. [Memory Inspector](#memory-inspector)
3. [Visualization Tools](#visualization-tools)
4. [Performance Profiler](#performance-profiler)
5. [Configuration Debugger](#configuration-debugger)
6. [Neural Activity Monitor](#neural-activity-monitor)
7. [Checkpoint Tools](#checkpoint-tools)
8. [Integration Test Framework](#integration-test-framework)

## Brain Debugger

The Brain Debugger provides comprehensive debugging capabilities for NLM brain simulations.

### Features

- **Step-by-step execution**: Run brain steps with detailed logging
- **State inspection**: Inspect brain state at any point
- **Error tracking**: Track and report errors with context
- **Breakpoint support**: Set breakpoints in simulation
- **Memory inspection**: View memory system state
- **Neuromodulation monitoring**: Track neuromodulator levels

### Usage

```python
import pynlm
from brain_debugger import BrainDebugger

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create debugger
debugger = BrainDebugger(brain)

# Run simulation with debugging
for step in range(100):
    debugger.step(step)
    
    # Set breakpoint at specific step
    if step == 50:
        debugger.breakpoint("Halfway through simulation")
        
    # Inspect state
    state = debugger.getState()
    print(f"Step {step}: {state}")
```

### API Reference

| Method | Description |
|--------|-------------|
| `BrainDebugger(brain)` | Create debugger for brain |
| `step(step)` | Run one step with debugging |
| `breakpoint(message)` | Set breakpoint |
| `getState()` | Get current brain state |
| `getNeuronState(neuronId)` | Get specific neuron state |
| `getSynapseState(synapseId)` | Get specific synapse state |
| `getMemoryState()` | Get memory system state |
| `getNeuromodulationState()` | Get neuromodulation state |

## Memory Inspector

The Memory Inspector provides detailed analysis of memory systems.

### Features

- **Working memory analysis**: View active traces, capacity, decay |
- **Episodic memory analysis**: View episodes, relevance, replay |
- **Associative memory analysis**: View associations, strength |
- **Memory statistics**: Compute memory metrics |
- **Memory visualization**: Visualize memory patterns |
- **Memory debugging**: Debug memory issues

### Usage

```python
import pynlm
from memory_inspector import MemoryInspector

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create memory inspector
inspector = MemoryInspector(brain)

# Analyze working memory
wm_state = inspector.analyzeWorkingMemory()
print(f"Working memory traces: {wm_state['trace_count']}")
print(f"Working memory capacity: {wm_state['capacity']}")
print(f"Working memory utilization: {wm_state['utilization']:.2%}")

# Analyze episodic memory
em_state = inspector.analyzeEpisodicMemory()
print(f"Episodic memory episodes: {em_state['episode_count']}")
print(f"Max episodes: {em_state['max_episodes']}")
print(f"Replay enabled: {em_state['replay_enabled']}")

# Analyze associative memory
am_state = inspector.analyzeAssociativeMemory()
print(f"Associative memory connections: {am_state['connection_count']}")

# Get memory recommendations
recommendations = inspector.getMemoryRecommendations()
for rec in recommendations:
    print(f"Recommendation: {rec}")
```

### API Reference

| Method | Description |
|--------|-------------|
| `MemoryInspector(brain)` | Create inspector for brain |
| `analyzeWorkingMemory()` | Analyze working memory |
| `analyzeEpisodicMemory()` | Analyze episodic memory |
| `analyzeAssociativeMemory()` | Analyze associative memory |
| `getMemoryStatistics()` | Get memory statistics |
| `visualizeMemoryPattern()` | Visualize memory pattern |
| `getMemoryRecommendations()` | Get memory optimization recommendations |

## Visualization Tools

The Visualization Tools provide graphical representation of NLM simulations.

### Features

- **Neuron activity visualization**: Show neuron firing patterns |
- **Network graph visualization**: Visualize neural connectivity |
- **Memory visualization**: Show memory contents and patterns |
- **Time series visualization**: Plot simulation metrics over time |
- **Heat map visualization**: Visualize activation patterns |
- **3D visualization**: Interactive 3D neural network view |

### Usage

```python
import pynlm
from visualization import Visualizer

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create visualizer
visualizer = Visualizer(brain)

# Create visualization containers
fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))

# Visualize neuron activity
visualizer.plot_neuron_activity(ax1, steps=100, neurons=50)

# Visualize network graph
visualizer.plot_network_graph(ax2)

# Visualize memory patterns
visualizer.plot_memory_patterns(ax3)

# Plot time series
visualizer.plot_time_series(ax4, ['firing_rate', 'curiosity', 'novelty'])

plt.tight_layout()
plt.show()
```

### Available Visualizations

- `plot_neuron_activity()`: Plot neuron firing over time
- `plot_network_graph()`: Visualize neural connectivity
- `plot_memory_patterns()`: Show memory content
- `plot_time_series()`: Plot simulation metrics
- `plot_heat_map()`: Show activation patterns
- `plot_3d_network()`: 3D neural network view

## Performance Profiler

The Performance Profiler measures and analyzes NLM simulation performance.

### Features

- **Step timing**: Measure time per step |
- **Memory usage**: Track memory consumption |
- **CPU profiling**: Profile CPU usage |
- **I/O profiling**: Profile file I/O |
- **Bottleneck detection**: Identify performance bottlenecks |
- **Optimization recommendations**: Suggest improvements

### Usage

```python
import pynlm
from performance_profiler import PerformanceProfiler

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create profiler
profiler = PerformanceProfiler(brain)

# Run simulation with profiling
for step in range(1000):
    profiler.start_step(step)
    brain.step(step)
    profiler.end_step()
    
    # Print performance every 100 steps
    if step % 100 == 0:
        stats = profiler.getPerformanceStats()
        print(f"Step {step}:")
        print(f"  Time per step: {stats['time_per_step']*1000:.2f} ms")
        print(f"  Memory usage: {stats['memory_usage']/1024/1024:.2f} MB")
        print(f"  CPU usage: {stats['cpu_usage']:.1f}%")
        
        # Check for bottlenecks
        if profiler.isBottleneckDetected():
            print(f"  Bottleneck detected: {profiler.getBottleneck()}")
            print(f"  Recommendations: {profiler.getRecommendations()}")

# Get final performance report
report = profiler.generatePerformanceReport()
print(report)
```

### Performance Metrics

| Metric | Description |
|--------|-------------|
| `time_per_step` | Average time per simulation step |
| `memory_usage` | Current memory usage |
| `cpu_usage` | CPU utilization percentage |
| `i_o_efficiency` | I/O efficiency |
| `bottleneck_type` | Type of bottleneck detected |
| `optimization_score` | Performance optimization score |

## Configuration Debugger

The Configuration Debugger helps identify and fix configuration issues.

### Features

- **Configuration validation**: Validate configuration settings |
- **Error detection**: Detect configuration errors |
- **Suggestion generation**: Suggest fixes for issues |
- **Configuration comparison**: Compare configurations |
- **Default values**: Show default values |
- **Option documentation**: Show option descriptions

### Usage

```python
import pynlm
from config_debugger import ConfigDebugger

# Create configuration with potential issues
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", -1)  # Invalid value
config.set("brain.synapse_density", 1.5)  # Invalid value (should be <= 1.0)

# Create debugger
debugger = ConfigDebugger(config)

# Validate configuration
validation = debugger.validate()
if not validation['valid']:
    print("Configuration validation failed:")
    for error in validation['errors']:
        print(f"  ERROR: {error}")
        
    print("\nSuggestions:")
    for suggestion in validation['suggestions']:
        print(f"  SUGGESTION: {suggestion}")

# Get default values
print("\nDefault configuration values:")
defaults = debugger.getDefaultValues()
for key, value in defaults.items():
    print(f"  {key}: {value}")

# Get option documentation
print("\nConfiguration options:")
options = debugger.getOptions()
for option, description in options.items():
    print(f"  {option}: {description}")
```

### Validation Results

| Result | Description |
|--------|-------------|
| `valid` | Whether configuration is valid |
| `errors` | List of configuration errors |
| `warnings` | List of configuration warnings |
| `suggestions` | List of suggestions for fixes |

## Neural Activity Monitor

The Neural Activity Monitor provides real-time monitoring of neural activity.

### Features

- **Real-time plotting**: Live plotting of neural activity |
- **Activity statistics**: Compute activity statistics |
- **Pattern detection**: Detect activity patterns |
- **Anomaly detection**: Detect anomalous activity |
- **Export data**: Export activity data |
- **Alerting**: Set alerts for abnormal activity

### Usage

```python
import pynlm
from neural_activity_monitor import NeuralActivityMonitor

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create activity monitor
monitor = NeuralActivityMonitor(brain)

# Start monitoring
monitor.startMonitoring()

# Run simulation with monitoring
for step in range(500):
    brain.step(step)
    
    # Get activity summary
    activity = monitor.getActivitySummary()
    print(f"Step {step}: {activity['firing_rate']:.2f} firing rate")
    
    # Check for anomalies
    if monitor.detectAnomaly():
        alert = monitor.getAlert()
        print(f"ALERT: {alert}")
        
    # Export activity data periodically
    if step % 100 == 0:
        monitor.exportActivity(f"activity_step_{step}.json")

# Stop monitoring
monitor.stopMonitoring()
```

### Activity Metrics

| Metric | Description |
|--------|-------------|
| `firing_rate` | Current firing rate |
| `activity_pattern` | Current activity pattern |
| `excitatory_inhibitory_ratio` | E/I ratio |
| `pattern_complexity` | Pattern complexity |
| `novelty_score` | Novelty detection score |
| `stability_index` | Stability indicator |

## Checkpoint Tools

The Checkpoint Tools provide utilities for managing checkpoints.

### Features

- **Checkpoint validation**: Validate checkpoint files |
- **Checkpoint comparison**: Compare checkpoints |
- **Checkpoint repair**: Repair corrupted checkpoints |
- **Checkpoint migration**: Migrate between formats |
- **Checkpoint analysis**: Analyze checkpoint contents |
- **Checkpoint backup**: Create checkpoint backups

### Usage

```python
import pynlm
from checkpoint_tools import CheckpointTools

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation to create checkpoint
for step in range(100):
    brain.step(step)

# Save checkpoint
CheckpointTools.saveCheckpoint(brain, "simulation_checkpoint.bin")

# Validate checkpoint
if CheckpointTools.validateCheckpoint("simulation_checkpoint.bin"):
    print("Checkpoint is valid")
    
    # Load checkpoint into new brain
    config2 = pynlm.createDefaultConfig()
    brain2 = pynlm.createBrain(config2)
    loaded = CheckpointTools.loadCheckpoint(brain2, "simulation_checkpoint.bin")
    
    if loaded:
        print("Checkpoint loaded successfully")
        print(f"Loaded brain has {brain2.getTotalNeuronCount()} neurons")

# Compare checkpoints
if CheckpointTools.checkpointExists("simulation_checkpoint.bin"):
    stats1 = CheckpointTools.analyzeCheckpoint("simulation_checkpoint.bin")
    print(f"Checkpoint size: {stats1['file_size']} bytes")
    print(f"Checkpoint neurons: {stats1['neuron_count']}")
    print(f"Checkpoint synapses: {stats1['synapse_count']}")

# Create backup
CheckpointTools.createBackup("simulation_checkpoint.bin", "backup_simulation_checkpoint.bin")
print("Backup created successfully")
```

### Checkpoint Operations

| Operation | Description |
|-----------|-------------|
| `saveCheckpoint(brain, path)` | Save brain state to checkpoint |
| `loadCheckpoint(brain, path)` | Load brain state from checkpoint |
| `validateCheckpoint(path)` | Validate checkpoint file |
| `analyzeCheckpoint(path)` | Analyze checkpoint contents |
| `checkpointExists(path)` | Check if checkpoint exists |
| `createBackup(source, destination)` | Create checkpoint backup |
| `listCheckpoints(directory)` | List checkpoints in directory |

## Integration Test Framework

The Integration Test Framework provides comprehensive testing for NLM integrations.

### Features

- **System integration tests**: Test system integrations |
- **Performance benchmarks**: Benchmark system performance |
- **Memory tests**: Test memory systems |
- **Prediction tests**: Test prediction systems |
- **Cognitive tests**: Test cognitive systems |
- **Neuromodulation tests**: Test neuromodulation systems |
- **Checkpoint tests**: Test checkpoint functionality |
- **Regression tests**: Detect regressions |

### Usage

```python
import pynlm
from integration_test_framework import IntegrationTestFramework

# Create integration test framework
framework = IntegrationTestFramework()

# Run system integration tests
print("Running system integration tests...")
system_tests = framework.runSystemIntegrationTests()
for test_name, result in system_tests.items():
    print(f"{test_name}: {result['status']} - {result['message']}")

# Run performance benchmarks
print("\nRunning performance benchmarks...")
performance_tests = framework.runPerformanceTests()
for test_name, metrics in performance_tests.items():
    print(f"{test_name}:")
    print(f"  Time: {metrics['execution_time']:.3f}s")
    print(f"  Memory: {metrics['memory_usage']:.2f}MB")
    print(f"  CPU: {metrics['cpu_usage']:.1f}%")

# Run memory tests
print("\nRunning memory tests...")
memory_tests = framework.runMemoryTests()
for test_name, result in memory_tests.items():
    print(f"{test_name}: {result['status']}")
    if result['status'] == 'PASS':
        print(f"  Traces: {result['trace_count']}")
        print(f"  Capacity: {result['capacity']}")

# Run prediction tests
print("\nRunning prediction tests...")
prediction_tests = framework.runPredictionTests()
for test_name, result in prediction_tests.items():
    print(f"{test_name}: {result['status']}")
    print(f"  Error: {result['error']:.4f}")
    print(f"  Confidence: {result['confidence']:.4f}")

# Generate test report
report = framework.generateTestReport()
print(f"\nTest Report:")
print(report)
```

### Test Categories

| Category | Description |
|----------|-------------|
| `system_integration` | Test system integration |
| `performance` | Test performance benchmarks |
| `memory` | Test memory systems |
| `prediction` | Test prediction systems |
| `cognitive` | Test cognitive systems |
| `neuromodulation` | Test neuromodulation systems |
| `checkpoint` | Test checkpoint functionality |
| `regression` | Test for regressions |

## Installation

### Installation of Development Tools

```bash
# Install core development tools
pip install -r dev_requirements.txt

# Install visualization tools
pip install matplotlib plotly seaborn

# Install profiling tools
pip install memory_profiler line_profiler psutil

# Install integration testing tools
pip install pytest pytest-cov pytest-html

# Install additional tools
pip install jupyter notebook ipykernel
```

### Setting Up the Development Environment

```bash
# Clone the NLM repository
if [ ! -d "nlm-dev" ]; then
    git clone https://github.com/nlm-project/nlm.git nlm-dev
fi

cd nlm-dev

# Install development packages
pip install -e .

# Install development tools
pip install -r requirements/dev.txt

# Set up Jupyter notebooks
cd docs
jupyter nbconvert --to script notebook.ipynb --output notebook.py
```

## Usage Examples

### Running All Development Tools

```python
import pynlm
from brain_debugger import BrainDebugger
from memory_inspector import MemoryInspector
from visualization import Visualizer
from performance_profiler import PerformanceProfiler
from config_debugger import ConfigDebugger
from neural_activity_monitor import NeuralActivityMonitor
from checkpoint_tools import CheckpointTools
from integration_test_framework import IntegrationTestFramework

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("=== NLM Development Tools ===")

# 1. Run Brain Debugger
print("\n1. Running Brain Debugger...")
debugger = BrainDebugger(brain)
for step in range(20):
    debugger.step(step)

# 2. Run Memory Inspector
print("\n2. Running Memory Inspector...")
inspector = MemoryInspector(brain)
wm_state = inspector.analyzeWorkingMemory()
print(f"Working memory: {wm_state['trace_count']} traces, {wm_state['capacity']} capacity")

# 3. Run Performance Profiler
print("\n3. Running Performance Profiler...")
profiler = PerformanceProfiler(brain)
for step in range(100):
    profiler.start_step(step)
    brain.step(step)
    profiler.end_step()
stats = profiler.getPerformanceStats()
print(f"Performance: {stats['time_per_step']*1000:.2f} ms/step")

# 4. Run Configuration Debugger
print("\n4. Running Configuration Debugger...")
debugger = ConfigDebugger(config)
validation = debugger.validate()
print(f"Configuration valid: {validation['valid']}")

# 5. Run Neural Activity Monitor
print("\n5. Running Neural Activity Monitor...")
monitor = NeuralActivityMonitor(brain)
monitor.startMonitoring()
for step in range(50):
    brain.step(step)
activity = monitor.getActivitySummary()
print(f"Activity: {activity['firing_rate']:.2f} firing rate")

# 6. Run Checkpoint Tools
print("\n6. Running Checkpoint Tools...")
CheckpointTools.saveCheckpoint(brain, "dev_checkpoint.bin")
print(f"Checkpoint saved: {CheckpointTools.checkpointExists('dev_checkpoint.bin')}")

# 7. Run Integration Tests
print("\n7. Running Integration Tests...")
framework = IntegrationTestFramework()
system_tests = framework.runSystemIntegrationTests()
print(f"System integration tests: {len(system_tests)} completed")

print("\n=== Development Tools Complete ===")
```

## Configuration

### Development Configuration File

Create a `dev_config.json` file in the project root:

```json
{
    "brain": {
        "neuron_count": 200,
        "region_count": 2,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    },
    "neuromod": {
        "dopamine": {
            "scale": 1.0
        },
        "curiosity": {
            "enable": true
        }
    },
    "debug": {
        "enable_brain_debugger": true,
        "enable_memory_inspector": true,
        "enable_performance_profiler": true,
        "log_level": "DEBUG"
    }
}
```

### Command Line Arguments

```bash
# Run with development configuration
python dev_tools.py --config dev_config.json

# Run specific tools
python dev_tools.py --tool brain_debugger
python dev_tools.py --tool memory_inspector
python dev_tools.py --tool performance_profiler

# Run all tools
python dev_tools.py --all
```

## Getting Help

### Documentation

- **API Reference**: `docs/api_reference.md`
- **Development Guide**: `docs/development_guide.md`
- **Tool Documentation**: Individual tool documentation
- **Examples**: `examples/` directory

### Community

- **GitHub Issues**: `https://github.com/nlm-project/nlm/issues`
- **Discussions**: `https://github.com/nlm-project/nlm/discussions`
- **Contributing**: `CONTRIBUTING.md`

### Support

- **Bug Reports**: Create issues on GitHub
- **Feature Requests**: Submit feature requests on GitHub
- **Questions**: Ask questions on Discussions

## Acknowledgements

The development tools are built on the excellent work of the NLM project team. Special thanks to all contributors who have made these development tools possible.

## License

MIT License - See LICENSE file for details

---

*Last updated: 2026-10-19*
*Version: 1.0.0*
*Development Tools*

The development tools provide comprehensive support for debugging, testing, and analyzing NLM simulations. These tools help developers ensure the quality, performance, and correctness of NLM implementations while providing insights into neural activity and system behavior.