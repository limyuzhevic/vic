# NLM Advanced Systems Documentation
# Comprehensive Guide for Expert Users

This document provides comprehensive documentation for the advanced systems added to the NLM (Neural Learning Machine) framework. It covers the advanced command line interface, scripting system, debugging capabilities, and performance monitoring features.

## Table of Contents

1. [Advanced Command Line Interface](#advanced-command-line-interface)
    - [CLI Overview](#cli-overview)
    - [Available Commands](#available-commands)
    - [Configuration Options](#configuration-options)
    - [Advanced Examples](#advanced-examples)

2. [Scripting System](#scripting-system)
    - [Script Overview](#script-overview)
    - [Script Structure](#script-structure)
    - [Script Examples](#script-examples)
    - [Script Functions](#script-functions)

3. [Debugging System](#debugging-system)
    - [Debug Overview](#debug-overview)
    - [Event Tracing](#event-tracing)
    - [Neuron and Synapse Tracing](#neuron-and-synapse-tracing)
    - [Debug Commands](#debug-commands)
    - [Trace Export](#trace-export)

4. [Performance Monitoring System](#performance-monitoring-system)
    - [Performance Overview](#performance-overview)
    - [Performance Metrics](#performance-metrics)
    - [Performance Analysis](#performance-analysis)
    - [Performance Profiling](#performance-profiling)
    - [Performance Benchmarking](#performance-benchmarking)

5. [Integration Guide](#integration-guide)
    - [System Integration](#system-integration)
    - [API Usage](#api-usage)
    - [Best Practices](#best-practices)

---

## Advanced Command Line Interface

### CLI Overview

The NLM Advanced Command Line Interface provides expert-level control over the neural learning machine simulation. It extends the basic NLM CLI with advanced features including:

- Complex configuration management
- Batch script execution
- Experiment automation
- Performance profiling and benchmarking
- Brain state export/import
- Advanced debugging options
- Multi-threaded execution support
- Custom action patterns
- Detailed performance metrics

### Available Commands

#### Core Commands

| Command | Description | Example |
|---------|-------------|---------|
| `--help` | Show help information | `nlm_advanced --help` |
| `--version` | Show version information | `nlm_advanced --version` |
| `--config <file>` | Load configuration from file | `nlm_advanced --config myconfig.cfg` |
| `--batch <script>` | Run batch script from file | `nlm_advanced --batch script.txt` |
| `--debug` | Enable debug logging | `nlm_advanced --debug` |
| `--profiling` | Enable performance profiling | `nlm_advanced --profiling` |
| `--benchmark` | Run benchmark tests | `nlm_advanced --benchmark` |

#### Experiment Commands

| Command | Description | Example |
|---------|-------------|---------|
| `--experiment <name>` | Run specific experiment | `nlm_advanced --experiment plasticity` |
| `--steps <count>` | Number of simulation steps | `nlm_advanced --steps 1000` |
| `--duration <seconds>` | Total simulation duration | `nlm_advanced --duration 60` |
| `--threads <count>` | Number of worker threads | `nlm_advanced --threads 4` |

#### Configuration Commands

| Command | Description | Example |
|---------|-------------|---------|
| `--neurons <count>` | Number of neurons | `nlm_advanced --neurons 1000` |
| `--regions <count>` | Number of brain regions | `nlm_advanced --regions 5` |
| `--probability <float>` | Connection probability | `nlm_advanced --probability 0.15` |
| `--seed <seed>` | Random seed | `nlm_advanced --seed 42` |

#### Output Commands

| Command | Description | Example |
|---------|-------------|---------|
| `--quiet` | Minimal output | `nlm_advanced --quiet --steps 100` |
| `--json` | Output in JSON format | `nlm_advanced --json --export results.json` |
| `--csv <file>` | Export results to CSV | `nlm_advanced --csv results.csv --steps 500` |
| `--log <file>` | Log output to file | `nlm_advanced --log debug.log --debug` |

#### Advanced Feature Commands

| Command | Description | Example |
|---------|-------------|---------|
| `--enable-curiosity` | Enable curiosity-driven exploration | `nlm_advanced --enable-curiosity` |
| `--enable-development` | Enable developmental stages | `nlm_advanced --enable-development` |
| `--enable-plasticity` | Enable structural plasticity | `nlm_advanced --enable-plasticity` |
| `--enable-reward` | Enable reward modulation | `nlm_advanced --enable-reward` |
| `--checkpoint <interval>` | Checkpoint interval | `nlm_advanced --checkpoint 1000` |
| `--replay <episodes>` | Replay memory episodes | `nlm_advanced --replay 10` |

### Configuration Options

The advanced CLI supports flexible configuration through command line arguments and configuration files.

#### Configuration File Format

Configuration files use a simple key=value format:

```ini
# Example configuration file
neuron_count = 1000
region_count = 5
connection_probability = 0.15
random_seed = 42
simulation_timestep = 0.001
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

#### Command Line Arguments

Command line arguments override configuration file settings:

```bash
nlm_advanced \
    --config myconfig.cfg \
    --neurons 2000 \
    --steps 500 \
    --experiment plasticity \
    --enable-curiosity \
    --enable-development \
    --enable-plasticity \
    --enable-reward
```

### Advanced Examples

#### Example 1: Running a Plasticity Experiment

```bash
nlm_advanced \
    --neurons 2000 \
    --regions 10 \
    --probability 0.12 \
    --experiment plasticity \
    --steps 2000 \
    --profiling \
    --enable-curiosity \
    --enable-development \
    --enable-plasticity \
    --export plasticity_results.nlm
```

This runs a plasticity experiment with 2000 neurons across 10 regions, using curiosity-driven exploration and developmental stages, with performance profiling enabled.

#### Example 2: Performance Benchmarking

```bash
nlm_advanced \
    --benchmark \
    --iterations 10 \
    --steps 1000 \
    --quiet
```

This runs a performance benchmark, executing 10 iterations of 1000 simulation steps each with minimal output.

#### Example 3: Batch Processing

```bash
nlm_advanced \
    --batch experiments/batch1.txt \
    --neurons 1500 \
    --experiment learning \
    --enable-reward \
    --export batch_results.nlm
```

This processes a batch script containing multiple experiment configurations, creating results for each batch item.

#### Example 4: Debug Mode with Tracing

```bash
nlm_advanced \
    --debug \
    --neurons 1000 \
    --steps 100 \
    --enable-curiosity \
    --enable-development \
    --export debug_trace.nlm
```

This runs a debug simulation with curiosity and development enabled, exporting the brain state with full debug tracing.

---

## Scripting System

### Script Overview

The NLM Scripting System provides a flexible scripting interface for automating brain simulations and experiments. Scripts can:

- Create and configure brains
- Set up world environments
- Connect agents to brains
- Run simulations
- Export/import brain states
- Perform batch operations
- Debug and analyze results

### Script Structure

Scripts are plain text files with a simple command-oriented syntax:

```python
# Example script
script_name = "experiment_1"

# Create brain
brain = createBrain("default_config")

# Create world
world = createWorld("demo_world", 20, 20)

# Create agent
agent = createAgent("demo_agent")

# Initialize agent
agent.initialize(world)

# Run simulation
runSimulation(agent, 100)

# Export results
exportState("experiment_1_results.nlm")
```

### Script Examples

#### Example 1: Basic Brain Simulation

```python
# Basic brain simulation script
script_name = "basic_simulation"

# Create default brain
brain = createBrain()
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        logBrainStatus(step)

# Export results
exportBrain("basic_simulation_results.nlm")

print("Basic simulation complete")
```

#### Example 2: Complex Experiment Script

```python
# Complex experiment script
script_name = "plasticity_experiment"

# Create brain with custom configuration
config = {"neurons": 2000, "regions": 5, "probability": 0.15}
brain = createBrain(config)

# Create world
world = createWorld("experiment_world", 30, 30, 16, 16)

# Create and initialize agent
agent = createAgent("plasticity_agent")
agent.initialize(world)
agent.enableCuriosity(true)
agent.enableDevelopment(true)
agent.enablePlasticity(true)
agent.enableRewardModulation(true)

# Run experiment
runExperiment(agent, world, "plasticity", 500)

# Export results
exportBrain("plasticity_results.nlm")
exportDebugInfo("plasticity_debug.txt")

print("Plasticity experiment complete")
```

#### Example 3: Batch Processing Script

```python
# Batch processing script
script_name = "batch_processing"

# Define experiment configurations
experiments = [
    {"name": "plasticity", "neurons": 1000, "steps": 200},
    {"name": "learning", "neurons": 1500, "steps": 300},
    {"name": "replay", "neurons": 2000, "steps": 400},
    {"name": "development", "neurons": 2500, "steps": 500}
]

# Create base brain
brain = createBrain()

# Run each experiment
for exp in experiments:
    print(f"Running experiment: {exp['name']}")
    
    # Reset brain for each experiment
    brain.reset()
    brain.initialize()
    
    # Configure brain
    config = {"neurons": exp['neurons'], "steps": exp['steps']}
    configureBrain(brain, config)
    
    # Run experiment
    runExperiment(brain, exp['name'], exp['steps'])
    
    # Export results
    exportBrain(f"{exp['name']}_results.nlm")
    
    # Save debug info
    exportDebugInfo(f"{exp['name']}_debug.txt")

print("Batch processing complete")
```

### Script Functions

| Function | Description | Parameters | Returns |
|----------|-------------|------------|---------|
| `createBrain(config)` | Create a new brain with configuration | config (dict) | Brain object |
| `createWorld(name, width, height, visionWidth, visionHeight)` | Create a world | name, dimensions | World object |
| `createAgent(name)` | Create an agent brain | name | Agent object |
| `configureBrain(brain, config)` | Configure brain with parameters | brain, config | None |
| `initializeAgent(agent, world)` | Initialize agent with world | agent, world | None |
| `runExperiment(brain, experimentName, steps)` | Run experiment | brain, name, steps | None |
| `runSimulation(agent, steps)` | Run basic simulation | agent, steps | None |
| `exportBrain(filename)` | Export brain state | filename | None |
| `exportDebugInfo(filename)` | Export debug information | filename | None |
| `enableCuriosity(agent, enable)` | Enable/disable curiosity | agent, boolean | None |
| `enableDevelopment(agent, enable)` | Enable/disable development | agent, boolean | None |
| `enablePlasticity(agent, enable)` | Enable/disable plasticity | agent, boolean | None |
| `enableRewardModulation(agent, enable)` | Enable/disable reward modulation | agent, boolean | None |
| `logBrainStatus(step)` | Log brain status | step | None |

---

## Debugging System

### Debug Overview

The NLM Debugging System provides comprehensive debugging capabilities for expert users, including:

- Event tracing and logging
- Neuron-level activity tracking
- Synapse-level activity tracking
- Interactive debugging interface
- Breakpoint management
- Trace export and analysis
- Visual debugging tools

### Event Tracing

The event tracing system records all significant events during simulation execution:

#### Event Types

| Event Type | Description | Example |
|------------|-------------|---------|
| SIMULATION_STEP | Simulation step execution | "Step 100 completed" |
| NEURON_SPIKE | Neuron firing event | "Neuron 42 spiked at time 0.1s" |
| SYNAPSE_ACTIVITY | Synapse activation | "Synapse 7 (0→5) weight: 0.15" |
| SENSOR_INPUT | Sensory input received | "Vision input received: 256 values" |
| MOTOR_OUTPUT | Motor command generated | "Action: MoveForward" |
| MEMORY_UPDATE | Memory system update | "Working memory updated" |
| PLASTICITY_EVENT | Plasticity rule applied | "STDP update: synapse weight changed" |
| NEUROMODULATION_EVENT | Neuromodulation signal | "Dopamine level: 0.3" |
| DEVELOPMENT_EVENT | Development stage change | "Developmental stage: CriticalPeriod" |
| REPLAY_EVENT | Memory replay | "Episodic memory replay: 5 episodes" |

#### Event Recording Commands

```bash
# Enable event recording
nlm_advanced --debug --enable-debug-events

# Filter events by type
nlm_advanced --debug --event-filter SPIKE,SYNAPSE,NEUROMOD

# Record to file
nlm_advanced --debug --log debug_events.txt
```

### Neuron and Synapse Tracing

#### Neuron Tracing

Neuron tracing provides detailed information about individual neuron activity:

- Membrane potential history
- Spike times and frequencies
- Firing patterns
- Synaptic input/output
- Plasticity flags

**Example:**

```bash
# Enable neuron tracing
nlm_advanced --debug --neuron-tracing

# Get neuron statistics
nlm_advanced --debug --neuron-stats 42
```

#### Synapse Tracing

Synapse tracing provides detailed information about individual synapse activity:

- Weight changes
- Eligibility traces
- Pre/post synaptic spike counts
- Plasticity rule application
- Synaptic history

**Example:**

```bash
# Enable synapse tracing
nlm_advanced --debug --synapse-tracing

# Get synapse statistics
nlm_advanced --debug --synapse-stats 7
```

### Debug Commands

| Command | Description | Example |
|---------|-------------|---------|
| `--step-breakpoint <step>` | Set breakpoint at specific step | `nlm_advanced --debug --step-breakpoint 100` |
| `--neuron-breakpoint <id>` | Set breakpoint at neuron | `nlm_advanced --debug --neuron-breakpoint 42` |
| `--synapse-breakpoint <id>` | Set breakpoint at synapse | `nlm_advanced --debug --synapse-breakpoint 7` |
| `--clear-breakpoints` | Clear all breakpoints | `nlm_advanced --debug --clear-breakpoints` |
| `--continue` | Continue execution | `nlm_advanced --debug --continue` |
| `--step` | Execute single step | `nlm_advanced --debug --step` |
| `--print <variable>` | Print debug variable | `nlm_advanced --debug --print neuron_count` |
| `--watch <variable>` | Watch variable changes | `nlm_advanced --debug --watch firing_rate` |
| `--trace <on|off>` | Enable/disable tracing | `nlm_advanced --debug --trace on` |

### Trace Export

The debug system can export trace data in various formats:

#### Export Commands

```bash
# Export trace to CSV
nlm_advanced --debug --export-trace trace.csv

# Export trace to JSON
nlm_advanced --debug --export-trace trace.json --format json

# Export detailed debug report
nlm_advanced --debug --export-debug debug_report.txt
```

#### Export Formats

| Format | Description | Use Case |
|--------|-------------|----------|
| CSV | Comma-separated values | Spreadsheets, analysis tools |
| JSON | JavaScript Object Notation | Web applications, APIs |
| Text | Plain text format | Human reading, documentation |
| Binary | Binary format | Large datasets, efficiency |

---

## Performance Monitoring System

### Performance Overview

The NLM Performance Monitoring System provides comprehensive performance analysis and profiling capabilities for expert users:

- Real-time performance metrics
- Detailed profiling and bottleneck analysis
- Performance benchmarking
- Performance regression testing
- Resource usage monitoring
- Optimization recommendations

### Performance Metrics

The performance monitoring system tracks multiple metrics:

#### Core Metrics

| Metric | Unit | Description | Typical Range |
|--------|------|-------------|---------------|
| CPU Usage | % | Central processing unit utilization | 0-100% |
| Memory Usage | MB | Memory consumption | 0-8192 |
| Simulation Time | ms | Time per simulation step | 0.1-1000 |
| Neuron Processing Time | ms | Time to process all neurons | 0.1-500 |
| Synapse Processing Time | ms | Time to process all synapses | 0.1-500 |
| I/O Time | ms | Time for I/O operations | 0.1-100 |
| Serialization Time | ms | Time for data serialization | 0.1-100 |
| Latency | ms | Response time | 0.1-100 |
| Throughput | steps/s | Steps processed per second | 1-1000 |
| Consistency | % | Data consistency | 90-100 |
| Accuracy | % | Simulation accuracy | 95-100 |
| Response Time | ms | Time to respond to events | 0.1-50 |

#### Performance Categories

| Category | Description | Example Metrics |
|----------|-------------|-----------------|
| CPU Performance | Processing speed | simulation_time, neuron_processing_time |
| Memory Performance | Memory usage and efficiency | memory_usage, latency |
| I/O Performance | Input/output operations | io_time, serialization_time |
| Network Performance | Network communication | latency, throughput |
| Storage Performance | Data storage | serialization_time, export_time |

### Performance Analysis

#### Performance Analysis Features

The performance analysis system provides:

1. **Statistical Analysis**: Mean, median, standard deviation, percentiles
2. **Bottleneck Identification**: Identify performance bottlenecks
3. **Trend Analysis**: Analyze performance trends over time
4. **Anomaly Detection**: Detect performance anomalies
5. **Optimization Recommendations**: Generate optimization recommendations

#### Performance Analysis Commands

```bash
# Enable performance monitoring
nlm_advanced --profiling --analysis

# Set performance thresholds
nlm_advanced --profiling --threshold-cpu 80 --threshold-memory 2048

# Generate performance report
nlm_advanced --profiling --report performance_report.txt

# Analyze performance data
nlm_advanced --profiling --analyze --output analysis.json
```

### Performance Profiling

#### Profiling Features

Performance profiling tracks function execution times and resource usage:

- **Function Profiling**: Track individual function execution times
- **Category Profiling**: Group functions by category (neuron, synapse, I/O)
- **Resource Profiling**: Track CPU, memory, and I/O resource usage
- **Time Series Analysis**: Analyze performance over time
- **Bottleneck Identification**: Identify performance bottlenecks

#### Profiling Commands

```bash
# Enable profiling
nlm_advanced --profiling --enable-profiling

# Profile specific categories
nlm_advanced --profiling --profile-categories neuron,synapse,io

# Get top functions
nlm_advanced --profiling --top-functions 10

# Analyze performance
nlm_advanced --profiling --analyze --output profile_analysis.txt
```

### Performance Benchmarking

#### Benchmarking Features

Performance benchmarking compares performance against baseline metrics:

- **Benchmark Suite**: Run a series of performance benchmarks
- **Regression Testing**: Detect performance regressions
- **Comparison Testing**: Compare against different configurations
- **Performance Targets**: Set performance targets and goals

#### Benchmarking Commands

```bash
# Run benchmark suite
nlm_advanced --benchmark

# Run specific benchmark
nlm_advanced --benchmark --benchmark-name neuron_creation

# Run benchmark with iterations
nlm_advanced --benchmark --iterations 10 --steps 1000

# Generate benchmark report
nlm_advanced --benchmark --report benchmark_report.txt
```

---

## Integration Guide

### System Integration

The NLM advanced systems can be integrated into existing NLM projects in several ways:

#### 1. Python Integration

```python
import pynlm
from nlm_advanced import AdvancedCLI, ScriptingSystem, DebuggingSystem, PerformanceMonitoring

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize advanced systems
advanced = AdvancedCLI()
scripting = ScriptingSystem()
debugging = DebuggingSystem()
performance = PerformanceMonitoring()

# Integrate with brain
advanced.attachBrain(brain)
scripting.attachBrain(brain)
debugging.attachBrain(brain)
performance.attachBrain(brain)

# Run experiment
advanced.runExperiment("plasticity", 1000)
```

#### 2. C++ Integration

```cpp
#include <nlm/core/brain/Brain.hpp>
#include <nlm/advanced/AdvancedCLI.hpp>
#include <nlm/scripting/ScriptingSystem.hpp>
#include <nlm/debugging/DebuggingSystem.hpp>
#include <nlm/performance/PerformanceMonitoring.hpp>

// Create brain
auto brain = std::make_shared<nlm::Brain>(config);

// Initialize advanced systems
AdvancedCLI advancedCLI;
ScriptingSystem scriptingSystem;
DebuggingSystem debuggingSystem;
PerformanceMonitoring performanceMonitor;

// Integrate systems
advancedCLI.attachBrain(brain);
scriptingSystem.attachBrain(brain);
debuggingSystem.attachBrain(brain);
performanceMonitor.attachBrain(brain);

// Run experiment
advancedCLI.runExperiment("plasticity", 1000);
```

### API Usage

#### Core API Methods

| Class | Method | Description | Parameters | Returns |
|-------|--------|-------------|------------|---------|
| AdvancedCLI | `attachBrain(brain)` | Attach brain to CLI | brain | None |
| AdvancedCLI | `runExperiment(name, steps)` | Run experiment | name, steps | None |
| AdvancedCLI | `exportBrain(filename)` | Export brain state | filename | None |
| ScriptingSystem | `attachBrain(brain)` | Attach brain to scripting | brain | None |
| ScriptingSystem | `loadScript(filename)` | Load script from file | filename | None |
| ScriptingSystem | `runScript(name)` | Run script by name | name | None |
| DebuggingSystem | `attachBrain(brain)` | Attach brain to debugger | brain | None |
| DebuggingSystem | `addBreakpoint(type, value)` | Add breakpoint | type, value | None |
| DebuggingSystem | `exportTrace(filename)` | Export trace data | filename | None |
| PerformanceMonitoring | `attachBrain(brain)` | Attach brain to monitoring | brain | None |
| PerformanceMonitoring | `recordSample(metrics)` | Record performance sample | metrics | None |
| PerformanceMonitoring | `getReport()` | Get performance report | - | string |

### Best Practices

#### 1. Configuration Management

- Use configuration files for complex settings
- Override configuration with command line arguments when needed
- Validate configuration before starting simulation
- Use environment variables for sensitive data

#### 2. Performance Optimization

- Profile your application before optimization
- Focus on bottlenecks identified by performance monitoring
- Use appropriate data structures for your use case
- Consider parallelization for large simulations

#### 3. Debugging Strategy

- Enable event tracing for complex debugging
- Use breakpoints to pause execution at specific points
- Export trace data for offline analysis
- Use visualizations to understand complex patterns

#### 4. Scripting Best Practices

- Keep scripts simple and focused on single tasks
- Use descriptive variable and function names
- Add comments to explain complex operations
- Test scripts incrementally

#### 5. Performance Monitoring Best Practices

- Monitor performance throughout development
- Set appropriate performance thresholds
- Regularly review performance reports
- Establish baseline performance metrics

---

## Conclusion

The NLM advanced systems provide a comprehensive set of tools for expert users working with neural learning machines. These systems enable:

- **Advanced Command Line Interface**: Expert-level control over simulations
- **Scripting System**: Flexible automation and experimentation
- **Debugging System**: Comprehensive debugging and analysis capabilities
- **Performance Monitoring**: Detailed performance analysis and optimization

By integrating these systems into your NLM projects, you can achieve:

- More efficient development workflows
- Better debugging capabilities
- Improved performance optimization
- Enhanced experimentation capabilities
- Better integration with external tools and systems

The documentation provided here should help you get started with the advanced systems and develop effective strategies for using them in your NLM projects.

---

*Document Version: 1.0.0*
*Last Updated: 2024-01-15*
*NLM Advanced Systems Team*

---

*This documentation is a work in progress. Please report any issues or suggest improvements to the documentation team.*

---