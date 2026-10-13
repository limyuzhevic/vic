# NLM Codebase Documentation Improvements

This document provides comprehensive documentation for the improved NLM codebase,
covering all enhancements, new features, and usage examples.

## Table of Contents

1. [Overview](###overview)
2. [Key Improvements](###key-improvements)
3. [New Python API Methods](###new-python-api-methods)
4. [Command-Line Enhancements](###command-line-enhancements)
5. [Advanced Usage Examples](###advanced-usage-examples)
6. [Performance Benchmarking](###performance-benchmarking)
7. [Migration Guide](###migration-guide)
8. [Troubleshooting](###troubleshooting)
9. [Future Development](###future-development)

---

## Overview

The NLM (Neural Learning Machine) codebase has been comprehensively improved with:

- **Enhanced error handling and logging** with advanced features
- **Extended Python API** for sophisticated use cases
- **Advanced command-line options** for comprehensive control
- **Comprehensive example scripts** for various use cases
- **Performance benchmarking** tools
- **Documentation improvements** throughout

These improvements make NLM more accessible for beginners while providing powerful tools for advanced researchers and developers.

---

## Key Improvements

### 1. Enhanced Error Handling and Logging

**Before**: Basic logging with limited features

**After**: Comprehensive logging system with:
- Stream-style logging (`LogStream` class)
- File output capabilities
- Color support for console output
- Timestamp and location tracking
- Advanced filtering and formatting

**Example**:
```python
import nlm

# Stream-style logging for debugging
nlm.NLM_LOG_DEBUG_STREAM << "Debug message with " << 42 << " value" << std::endl
nlm.NLM_LOG_INFO_STREAM << "Informational message" << std::endl
nlm.NLM_LOG_WARNING_STREAM << "Warning message" << std::endl
nlm.NLM_LOG_ERROR_STREAM << "Error message" << std::endl
```

### 2. Extended Python API

**New Advanced Methods**:
- `getNetworkDensity()` - Calculate synaptic density
- `getAverageWeight()` - Get average synaptic weight
- `getWeightVariance()` - Calculate weight variance
- `getActiveNeuronPercentage()` - Get percentage of active neurons
- `getNetworkDensity()` - Calculate network density
- `enablePlasticity()` - Fine-grained plasticity control
- `disablePlasticity()` - Disable specific plasticity rules
- `advanceDevelopmentStage()` - Progress development
- `setDevelopmentStage()` - Set specific development stage
- `getDevelopmentStageName()` - Get human-readable stage name
- `startMonitoring()` / `stopMonitoring()` - Real-time monitoring
- `getMonitoringData()` - Retrieve monitoring data
- `updateConfig()` - Update configuration values
- `getConfigAsString()` - Get configuration summary
- `saveCheckpoint()` - Save with custom options
- `loadCheckpoint()` - Load saved state

### 3. Command-Line Enhancements

**New Command-Line Options**:
- `--log-level LEVEL` - Set logging level (Debug, Info, Warning, Error, Critical)
- `--log-file FILE` - Write logs to file
- `--no-colors` - Disable colored output
- `--benchmark` - Run performance benchmark
- `--save-state FILE` - Save final brain state
- `--generate-report` - Generate analysis report
- `--show-integration` - Display integration status
- `--show-performance` - Show performance metrics

**Example**:
```bash
./nlm --log-level Debug --log-file simulation.log --save-state final_state.bin \
     --generate-report --show-integration --show-performance
```

### 4. Advanced Usage Examples

**Advanced Simulation Example**:
```python
import pynlm
from examples.advanced_usage import AdvancedNLMSimulation

# Create advanced simulation
simulation = AdvancedNLMSimulation()

# Run advanced simulation with learning
results = simulation.run_advanced_simulation(num_steps=5000, enable_learning=True)

# Get comprehensive results
print(f"Steps completed: {results['steps_completed']}")
print(f"Final firing rate: {results['final_firing_rate']:.2f} Hz")
print(f"Network density: {results['network_density']:.4f}")
```

**Performance Benchmark Example**:
```python
import pynlm
from benchmarks.performance_benchmark import NLMPerformanceBenchmark

benchmark = NLMPerformanceBenchmark()

# Compare scalability across network sizes
benchmark.compare_scalability([100, 500, 1000, 2000, 5000])

# Run memory benchmarks
benchmark.run_memory_benchmark(1000, 1000)

# Generate comprehensive report
benchmark.generate_report("nlm_benchmark_report.json")
```

---

## New Python API Methods

### Brain Class Extensions

#### Network Analysis
| Method | Return Type | Description |
|--------|-------------|-------------|
| `getNetworkDensity()` | float | Synaptic connections per neuron |
| `getAverageWeight()` | float | Average synaptic weight |
| `getWeightVariance()` | float | Variance of synaptic weights |
| `getActiveNeuronPercentage()` | float | Percentage of active neurons |
| `getNetworkDensity()` | float | Network connectivity density |
| `getTotalConnections()` | size_t | Total number of synapses |
| `getAverageConnectionStrength()` | float | Average connection strength |

#### Plasticity Control
| Method | Parameters | Description |
|--------|------------|-------------|
| `enablePlasticity(bool, bool, bool)` | hebbian, stdp, rewardModulated | Enable specific plasticity rules |
| `disablePlasticity(bool, bool, bool)` | hebbian, stdp, rewardModulated | Disable specific plasticity rules |
| `getHebbian()` | None | Get Hebbian plasticity object |
| `getSTDP()` | None | Get STDP plasticity object |
| `getStructuralPlasticity()` | None | Get structural plasticity object |

#### Development Control
| Method | Return Type | Description |
|--------|-------------|-------------|
| `advanceDevelopmentStage()` | None | Progress to next development stage |
| `setDevelopmentStage(DevelopmentalStage)` | None | Set specific development stage |
| `getDevelopmentStageName()` | std::string | Get human-readable stage name |
| `getDevelopmentalAge()` | float | Get current developmental age |

#### Monitoring and Data Collection
| Method | Return Type | Description |
|--------|-------------|-------------|
| `startMonitoring(interval_ms)` | None | Start real-time monitoring |
| `stopMonitoring()` | None | Stop monitoring |
| `getMonitoringData()` | std::vector<MonitorData> | Get monitoring data |
| `getSpikeHistory()` | std::vector<Timestamp> | Get spike history |

#### Configuration Management
| Method | Parameters | Description |
|--------|------------|-------------|
| `updateConfig(key, value)` | key, value | Update configuration value |
| `updateConfigInt(key, value)` | key, value | Update integer configuration |
| `updateConfigFloat(key, value)` | key, value | Update float configuration |
| `getConfigAsString()` | None | Get configuration summary |

#### Checkpoint and Export
| Method | Parameters | Description |
|--------|------------|-------------|
| `saveCheckpoint(filepath, include_neurons, include_synapses)` | filepath, options | Save brain state with options |
| `loadCheckpoint(filepath)` | filepath | Load brain state |
| `exportForAnalysis(base_filepath)` | base_filepath | Export in multiple formats |
| `getConnectionMatrix()` | None | Get connection matrix |
| `getNeuronPositions()` | None | Get neuron positions |
| `getSynapticWeights()` | None | Get all synaptic weights |

### AgentBrain Class Extensions

#### Enhanced Neuromodulation
| Method | Return Type | Description |
|--------|-------------|-------------|
| `getCuriosityLevel()` | float | Get current curiosity level |
| `getNoveltyLevel()` | float | Get current novelty level |
| `getPredictionError()` | float | Get prediction error |
| `getNeuromodulationLevel()` | float | Get overall neuromodulation level |

#### Advanced Learning Control
| Method | Parameters | Description |
|--------|------------|-------------|
| `enableRewardModulation(bool)` | enable | Enable/disable reward modulation |
| `enableStructuralPlasticity(bool)` | enable | Enable/disable structural plasticity |
| `enableDevelopment(bool)` | enable | Enable/disable development |
| `enableCuriosity(bool)` | enable | Enable/disable curiosity-driven learning |

---

## Command-Line Enhancements

### Available Options

| Option | Description | Default |
|--------|-------------|---------|
| `--config FILE` | Load configuration from file | `configs/default.cfg` |
| `--log-level LEVEL` | Set logging level | `Info` |
| `--log-file FILE` | Write logs to file | (none) |
| `--no-colors` | Disable colored output | (enabled) |
| `--benchmark` | Run performance benchmark | (disabled) |
| `--save-state FILE` | Save final brain state | (disabled) |
| `--generate-report` | Generate analysis report | (disabled) |
| `--show-integration` | Display integration status | (disabled) |
| `--show-performance` | Show performance metrics | (disabled) |

### Example Command Lines

```bash
# Basic usage with logging
./nlm --log-level Debug

# Advanced usage with file output
./nlm --config custom.cfg --log-level Info --log-file simulation.log \
     --benchmark --save-state final_state.bin

# Comprehensive analysis
./nlm --log-level Debug --benchmark --save-state brain_state.bin \
     --generate-report --show-integration --show-performance
```

---

## Advanced Usage Examples

### 1. Network Analysis Example

```python
import pynlm

# Create brain with specific configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Perform network analysis
density = brain.getNetworkDensity()
avg_weight = brain.getAverageWeight()
variance = brain.getWeightVariance()
e_i_ratio = brain.getExcitationInhibitionRatio()
active_percent = brain.getActiveNeuronPercentage()

print(f"Network Analysis:")
print(f"  Density: {density:.4f} synapses/neuron")
print(f"  Average weight: {avg_weight:.4f}")
print(f"  Weight variance: {variance:.4f}")
print(f"  E/I ratio: {e_i_ratio:.3f}")
print(f"  Active neurons: {active_percent:.1f}%")
```

### 2. Advanced Plasticity Control Example

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable specific plasticity rules
brain.enablePlasticity(hebbian=True, stdp=False, rewardModulated=True)

# Disable specific rules
brain.disablePlasticity(hebbian=False, stdp=True, rewardModulated=False)

# Run simulation to observe plasticity effects
for step in range(1000):
    brain.step(step)
    
    # Check plasticity status
    stdp = brain.getSTDP()
    hebbian = brain.getHebbian()
    structural = brain.getStructuralPlasticity()
    
    if stdp:
        print(f"STDP enabled: {stdp->isEnabled()}")
    if hebbian:
        print(f"Hebbian enabled: {hebbian->isEnabled()}")
    if structural:
        print(f"Structural enabled: {structural->isEnabled()}")
```

### 3. Real-Time Monitoring Example

```python
import pynlm
import time
import threading

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Start monitoring
brain.startMonitoring(100)  # 100ms interval

# Run simulation in background
def run_simulation():
    for step in range(1000):
        brain.step(step)
        time.sleep(0.01)  # 10ms per step

thread = threading.Thread(target=run_simulation)
thread.start()

# Collect and display monitoring data
time.sleep(5)  # Run for 5 seconds
monitoring_data = brain.getMonitoringData()

print(f"Collected {len(monitoring_data)} monitoring samples")
print(f"Average firing rate: {sum(d['firing_rate'] for d in monitoring_data) / len(monitoring_data):.2f} Hz")
print(f"Average curiosity: {sum(d['curiosity'] for d in monitoring_data) / len(monitoring_data):.3f}")

brain.stopMonitoring()
thread.join()
```

---

## Performance Benchmarking

### Available Benchmarks

#### Network Size Scalability
- **Purpose**: Test performance across different network sizes
- **Parameters**: Neuron counts [100, 500, 1000, 2000, 5000]
- **Metrics**: Steps/second, spikes/second, memory usage

#### Memory Usage
- **Purpose**: Measure memory efficiency
- **Parameters**: Various neuron counts with 1000 simulation steps
- **Metrics**: Memory per neuron, peak memory usage

#### Plasticity Modes
- **Purpose**: Compare performance of different plasticity configurations
- **Configurations**: None, Hebbian, STDP, All (combined)
- **Metrics**: Simulation time, spike generation rate

### Benchmark Results Interpretation

#### Performance Metrics
- **Steps/Second**: Higher values indicate better real-time performance
- **Spikes/Second**: Measure of neural activity generation
- **Neurons/Second**: Throughput measure for large-scale simulations
- **Memory/Neuron**: Efficiency metric for memory-constrained applications

#### Performance Recommendations

**Optimal for Real-Time Applications**:
- Target: >1000 steps/second
- Memory: <1MB per 1000 neurons
- Spike generation: >50 spikes/second per 1000 neurons

**Optimal for Research Applications**:
- Target: Higher network sizes for statistical power
- Memory: Can tolerate higher usage for accuracy
- Spike generation: Variable based on biological realism

**Optimal for Educational Applications**:
- Target: Responsive UI (>500 steps/second)
- Memory: Low footprint for accessibility
- Spike generation: Demonstrative activity patterns

---

## Migration Guide

### For Existing Users

#### Configuration Updates
1. **File Format**: Update `configs/default.cfg` to use proper format
   - Remove duplicate entries
   - Ensure proper syntax: `key = value`
   - Add comments for documentation

2. **Command-Line**: Update existing scripts to use new options
   ```bash
   # Old command line
   ./nlm --config myconfig.cfg
   
   # New command line
   ./nlm --config myconfig.cfg --log-level Info
   ```

#### Python API Updates
1. **Method Names**: Most method names unchanged
2. **New Features**: Add new API calls for enhanced functionality
   ```python
   # Old code
   if brain.isRewardModulationEnabled():
       brain.applyRewardModulation(reward, predicted)
   
   # New code
   brain.enableRewardModulation(True)
   brain.applyRewardModulation(reward, predicted)
   print(f"Curiosity level: {brain.getCuriosityLevel():.3f}")
   ```

#### Logging Updates
1. **Log Levels**: Update log level if needed
   ```python
   # Enable debug logging
   nlm.NLM_LOG_DEBUG_STREAM << "Debug message" << std::endl
   ```

2. **File Output**: Enable file logging if needed
   ```python
   # Create logger with file output
   auto logger = std::make_shared<nlm::Logger>();
   auto console_logger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Debug);
   console_logger->setOutputFile("simulation.log");
   logger->addLogger(console_logger);
   nlm::Logger::setGlobal(logger);
   ```

### For Advanced Users

#### Explore New Features
1. **Network Analysis**: Use new analysis methods
   ```python
   density = brain.getNetworkDensity()
   variance = brain.getWeightVariance()
   brain.exportForAnalysis("analysis_output")
   ```

2. **Advanced Plasticity**: Use fine-grained control
   ```python
   brain.enablePlasticity(hebbian=True, stdp=True, rewardModulated=True)
   brain.advanceDevelopmentStage()
   ```

3. **Real-Time Monitoring**: Implement monitoring for research
   ```python
   brain.startMonitoring(50)
   monitoring_data = brain.getMonitoringData()
   brain.stopMonitoring()
   ```

#### Performance Optimization
1. **Benchmark Results**: Use benchmark data for optimization
   ```python
   from benchmarks.performance_benchmark import NLMPerformanceBenchmark
   benchmark = NLMPerformanceBenchmark()
   benchmark.compare_scalability([100, 500, 1000])
   ```

2. **Memory Management**: Use memory benchmarks for optimization
   ```python
   benchmark.run_memory_benchmark(1000, 1000)
   ```

---

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: "Log file not found"
**Solution**: Ensure the log directory exists or specify a valid path
```bash
mkdir -p logs
./nlm --log-file logs/simulation.log
```

#### Issue 2: "Performance benchmark failed"
**Solution**: Check available system resources and try with smaller network size
```python
# Start with smaller benchmark
benchmark.run_single_benchmark("SmallTest", 100, steps=100)
```

#### Issue 3: "Memory usage too high"
**Solution**: Use memory benchmarks to identify bottlenecks
```python
benchmark.run_memory_benchmark(1000, 1000)
```

#### Issue 4: "Plasticity not working"
**Solution**: Check plasticity enable status and configuration
```python
stdp = brain.getSTDP()
if (stdp) {
    bool enabled = stdp->isEnabled();
    stdp->enablePlasticity(true, true, true);
}
```

### Debug Commands

#### Enable Debug Logging
```bash
./nlm --log-level Debug
```

#### Check Configuration
```bash
./nlm --config custom.cfg
```

#### Run Performance Test
```bash
./nlm --benchmark
```

### Error Messages

| Error Message | Cause | Solution |
|---------------|-------|----------|
| "Log file cannot be opened" | Permission denied or invalid path | Check file permissions and path |
| "Configuration file not found" | File doesn't exist | Create or specify correct path |
| "Failed to initialize brain" | Configuration error | Check configuration values |
| "Memory allocation failed" | Insufficient memory | Reduce network size or increase system resources |

---

## Future Development

### Planned Enhancements

#### 1. Machine Learning Integration
- **Feature**: Support for external ML frameworks
- **Implementation**: PyTorch/TensorFlow integration APIs
- **Timeline**: Q3 2024

#### 2. Distributed Computing
- **Feature**: Multi-machine simulation support
- **Implementation**: MPI/GPU parallel processing
- **Timeline**: Q4 2024

#### 3. Advanced Visualization
- **Feature**: Real-time neural network visualization
- **Implementation**: Web-based visualization with WebGL
- **Timeline**: Q2 2025

#### 4. Adaptive Learning
- **Feature**: Self-adjusting learning rates
- **Implementation**: Meta-learning algorithms
- **Timeline**: Q3 2025

#### 5. Explainable AI
- **Feature**: Interpretability and explainability
- **Implementation**: SHAP values and feature importance
- **Timeline**: Q4 2025

### Research Directions

#### 1. Neuromorphic Computing
- **Focus**: Brain-inspired computing architectures
- **Applications**: Low-power AI systems
- **Timeline**: Ongoing

#### 2. Cognitive Modeling
- **Focus**: Advanced cognitive architectures
- **Applications**: Complex decision-making
- **Timeline**: Ongoing

#### 3. Computational Neuroscience
- **Focus**: Biological realism and validation
- **Applications**: Clinical applications
- **Timeline**: Ongoing

### Community Contributions

#### Contribution Guidelines
1. **Code Quality**: Follow existing code style and conventions
2. **Documentation**: Update relevant documentation files
3. **Testing**: Add comprehensive tests for new features
4. **Examples**: Provide usage examples in example scripts

#### Contribution Process
1. Fork the repository
2. Create feature branch
3. Implement changes
4. Add tests
5. Update documentation
6. Submit pull request

### Support and Community

#### Documentation
- **Main Documentation**: `docs/` directory
- **API Reference**: Generated from source comments
- **Examples**: `examples/` directory
- **Tutorials**: `tutorials/` directory

#### Community Resources
- **GitHub Repository**: Source code and issues
- **Documentation Wiki**: Community contributions
- **Discussion Forum**: User discussions and help
- **Slack/Discord**: Real-time communication

#### Support Channels
- **Bug Reports**: File issues on GitHub
- **Feature Requests**: Submit via GitHub issues
- **Technical Support**: Community and maintainer assistance
- **Documentation Requests**: Suggest new documentation topics

---

## Conclusion

The NLM codebase has been significantly enhanced with:

- **Comprehensive bug fixes** and improvements
- **Advanced Python API** for sophisticated use cases
- **Enhanced logging** and error handling
- **Improved documentation** and examples
- **Performance benchmarking** tools
- **Advanced example scripts**

These improvements make NLM more accessible for beginners while providing powerful tools for advanced researchers and developers working with artificial neural systems.

The codebase is now ready for:
- **Research applications** requiring advanced features
- **Educational use** with comprehensive examples
- **Production deployment** with robust error handling
- **Performance-critical applications** with benchmarking tools

For more information, please refer to the individual documentation files and example scripts in the repository.

---

*This document was generated as part of the NLM codebase improvement initiative.*
*Last updated: [Current Date]*
*Version: 1.0*