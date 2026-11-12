# Debug Configuration
# NLM Debug and Profiling Tools

## Overview
This directory contains debug and profiling tools for the NLM (Neural Learning Machine) Phase 2 implementation. The tools provide comprehensive debugging capabilities for development, testing, and analysis of neural simulation experiments.

## Key Features

### 1. Debug Configuration
- Step tracing with multiple verbosity levels
- Memory usage tracking and snapshots
- Performance profiling with detailed metrics
- Neuron and connection monitoring
- Plasticity event tracking
- Custom metrics collection

### 2. Performance Metrics
- Step timing analysis (min, max, mean, std dev, percentiles)
- Spike count statistics and firing rate analysis
- Resource usage monitoring
- Custom metrics support

### 3. Memory Management
- Memory snapshots at specified intervals
- Memory usage thresholds and alerts
- Memory leak detection capabilities

### 4. Neuron Activity Analysis
- Neuron heatmap generation for visualization
- Activity statistics and firing patterns
- Spatial distribution analysis

### 5. Spike Train Analysis
- Detailed spike recording and analysis
- Inter-spike interval statistics
- Per-neuron firing rate calculations
- Correlation and pattern analysis

### 6. Debug Reports
- Comprehensive text report generation
- Python-compatible data exports
- JSON serialization for external tools
- Integration with visualization tools

## Configuration Options

### DebugConfig Structure
- `enableStepTracing`: Enable detailed step-by-step logging
- `enableMemoryTracking`: Track memory usage and allocations
- `enablePerformanceProfiling`: Collect performance metrics
- `enableNeuronsMonitoring`: Monitor neuron activity
- `enableConnectionsMonitoring`: Track connection changes
- `enablePlasticityMonitoring`: Monitor plasticity events
- `traceLevel`: Logging verbosity (0=ERROR, 1=WARN, 3=DEBUG)
- `outputDirectory`: Directory for debug output files
- `neuronSamplingRate`: Rate for neuron activity sampling
- `enableNeuronHeatmap`: Generate neuron activity heatmap
- `enableSpiketrainRecording`: Record spike trains for analysis

### Usage Examples

#### Basic Debug Configuration
```python
import nlm

debug_config = nlm.DebugConfig()
debug_config.enableStepTracing = True
debug_config.enableMemoryTracking = True
debug_config.enablePerformanceProfiling = True
debug_config.traceLevel = 2  # INFO level

nlm.initializeDebugger(debug_config)
nlm.startDebugRecording()
```

#### Debug Experiment with Recording
```python
# Create brain and initialize
config = nlm.createLearningConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Start recording
nlm.startDebugRecording()

# Run experiment with debugging
for step in range(1000):
    brain.step(step, step * 0.001)
    nlm.recordStep(brain, 0.001, brain.getTotalSpikeCount(), step, step * 0.001)

# Stop recording and generate reports
nlm.stopDebugRecording()
nlm.generateDebugReport("experiment_1")
```

#### Performance Comparison
```python
# Run multiple iterations and compare performance
results = []
for i in range(3):
    brain.reset()
    brain.initialize()
    
    start_time = time.time()
    # Run experiment
    for step in range(duration):
        brain.step(step, step * 0.001)
    end_time = time.time()
    
    result = {
        'iteration': i + 1,
        'time_ms': (end_time - start_time) * 1000,
        'steps_per_sec': duration / (end_time - start_time),
        'total_spikes': brain.getTotalSpikeCount()
    }
    results.append(result)

# Analyze and compare results
for result in results:
    print(f"Iteration {result['iteration']}:")
    print(f"  Time: {result['time_ms']:.2f} ms")
    print(f"  Steps/sec: {result['steps_per_sec']:.0f}")
    print(f"  Spikes/sec: {result['total_spikes'] / (result['time_ms'] / 1000):.0f}")
```

## Output Files

The debug system generates the following files:

1. `performance_[filename].txt`
   - Performance metrics report
   - Step timing statistics
   - Spike count analysis
   - Custom metrics

2. `memory_[filename].txt`
   - Memory snapshot information
   - Memory usage trends
   - Memory allocation patterns

3. `heatmap_[filename].txt`
   - Neuron activity heatmap data
   - Activity statistics
   - Spatial distribution analysis

4. `spiketrain_[filename].txt`
   - Detailed spike train analysis
   - Inter-spike interval statistics
   - Firing rate calculations

5. `debug_log_[filename].txt`
   - Complete debug log
   - Configuration settings
   - Recording timestamps

6. `[filename].json`
   - Python-compatible data exports
   - Structured data for analysis
   - Integration with visualization tools

## Integration with Main Program

The debug system is integrated into the main NLM program through command-line options:

```bash
# Enable debug recording
nlm --mode ALL --debug

# Debug with specific configuration
nlm --mode PLASTICITY --debug-config debug_config.json

# Performance comparison with debug
nlm --benchmark --profile --debug
```

## Python Integration

The debug system provides Python-friendly interfaces:

```python
import pynlm as nlm
import json

# Create debug configuration
debug_config = nlm.DebugConfig()
debug_config.enableStepTracing = True
debug_config.enablePerformanceProfiling = True
nlm.initializeDebugger(debug_config)

# Start recording
nlm.startDebugRecording()

# Run simulation
brain = nlm.createBrain(nlm.createLearningConfig())
brain.initialize()

for step in range(100):
    brain.step(step, step * 0.001)
    nlm.recordStep(brain, 0.001, brain.getTotalSpikeCount(), step, step * 0.001)

# Stop and generate reports
nlm.stopDebugRecording()
nlm.generateDebugReport("simulation_1")

# Export to Python
nlm.exportDebugDataToPython("simulation_1")

# Load and analyze data
with open("debug_output/simulation_1.json", "r") as f:
    data = json.load(f)
    print(f"Simulation took {data['performance']['total_time']} ms")
```

## Benefits

1. **Enhanced Debugging**: Real-time monitoring and step-by-step analysis
2. **Performance Analysis**: Detailed metrics for optimization
3. **Memory Management**: Track and analyze memory usage
4. **Activity Visualization**: Heatmaps and spike train analysis
5. **Integration**: Seamless integration with Python ecosystem
6. **Reporting**: Comprehensive report generation
7. **Development Support**: Advanced debugging tools for developers

## Configuration File Example

Create `debug_config.json` for custom debug settings:

```json
{
    "enableStepTracing": true,
    "enableMemoryTracking": true,
    "enablePerformanceProfiling": true,
    "enableNeuronsMonitoring": true,
    "enableConnectionsMonitoring": true,
    "enablePlasticityMonitoring": true,
    "traceLevel": 2,
    "outputDirectory": "./debug_output",
    "neuronSamplingRate": 50,
    "enableNeuronHeatmap": true,
    "enableSpiketrainRecording": true
}
```

This configuration can be loaded programmatically:

```python
import json
import nlm

with open("debug_config.json", "r") as f:
    config_data = json.load(f)

debug_config = nlm.DebugConfig()
debug_config.enableStepTracing = config_data.get("enableStepTracing", false)
debug_config.enableMemoryTracking = config_data.get("enableMemoryTracking", false)
debug_config.enablePerformanceProfiling = config_data.get("enablePerformanceProfiling", false)
debug_config.traceLevel = config_data.get("traceLevel", 1)
debug_config.outputDirectory = config_data.get("outputDirectory", "./debug_output")

nlm.initializeDebugger(debug_config)
```

## Conclusion

The debug and profiling system provides comprehensive tools for developing, testing, and analyzing NLM simulations. It offers detailed insights into neural activity, performance characteristics, and system behavior, making it an essential tool for researchers and developers working with artificial neural networks.