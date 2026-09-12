# NLM - Advanced Usage Guide for Power Users

**This guide extends the beginner-friendly `easy_usage.md` with advanced features for experienced users and researchers.

## Quick Reference to New Features

| Feature | Command | Description |
|---------|---------|-------------|
| **Configuration Comparison** | `nlm-configcmp <file1> <file2>` | Compare two configuration files and show differences |
| **Advanced Benchmarking** | `nlm-benchmark [options]` | Detailed performance analysis with memory tracking |
| **Export/Import** | `nlm-export [options]` | Save/load brain states with compression and multiple formats |
| **Parameter Sweeps** | `nlm-experiment sweep [options]` | Automated parameter optimization and testing |
| **Validation Reports** | `nlm-config [options] --validate` | Generate comprehensive validation reports |
| **Network Analysis** | `nlm-benchmark --topology` | Analyze neural network topology and connectivity patterns |

## Advanced Configuration Management

### Configuration Templates

Create reusable configuration templates for common research setups:

```python
import pynlm

# Create a template for learning experiments
template_config = pynlm.createDefaultConfig()
template_config.set("neuron_count", 2000, pynlm.ConfigSource::Runtime)
template_config.set("plasticity_learning_rate", 0.05)
template_config.set("reward_discount_factor", 0.95)
template_config.set("replay_interval", 500)
template_config.set("consolidation_interval", 5000)

# Save template for reuse
template_config.saveToFile("learning_experiment.cfg")
```

### Configuration Inheritance

Configuration inheritance allows combining settings from multiple sources:

```python
import pynlm

# Base configuration
base_config = pynlm.createDefaultConfig()
base_config.set("neuron_count", 1000)

# Child configuration
child_config = pynlm.createDefaultConfig()
child_config.set("region_count", 4)  # Override base

# Create inherited configuration (child overrides parent)
inherited_config = pynlm.createDefaultConfig()
inherited_config.loadFromArgs(0, None)  # Empty args for demo

# In real usage, you'd merge configurations programmatically
# The advanced configuration management system handles this automatically
```

### Validation Reports

Generate comprehensive validation reports for your configuration:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Enable validation (default is enabled)
config.enableValidation(True)

# Perform validation and get detailed report
validation_report = config.validationErrors()

# Check if configuration is valid
if config.validate():
    print("✓ Configuration is valid")
else:
    print("✗ Configuration has validation errors:")
    print(validation_report)

# Output includes:
# - Invalid parameter values
# - Type mismatches
# - Range violations
# - Missing required parameters
# - Suggested fixes
```

## Advanced Benchmarking

### Performance Analysis with Memory Tracking

```python
import pynlm

# Create advanced benchmark
benchmark = pynlm.AdvancedBenchmark()

# Configure benchmark options
benchmark.setNeuronCount(2000)
benchmark.setStepCount(1000)
benchmark.enableMemoryTracking(True)
benchmark.enableSpikeAnalysis(True)
benchmark.enableTopologyAnalysis(True)

# Run comprehensive benchmark
results = benchmark.run()

# Access detailed performance metrics
print("=== Performance Metrics ===")
print(f"Steps per second: {results.stepsPerSecond}")
print(f"Memory usage: {results.memoryUsageMB} MB")
print(f"Average firing rate: {results.averageFiringRate}")
print(f"Total spikes: {results.totalSpikes}")
print(f"Memory efficiency: {results.memoryEfficiency}%")

print("\n=== Spike Pattern Analysis ===")
spike_stats = results.spikeAnalysis
print(f"Max simultaneous spikes: {spike_stats.maxSimultaneousSpikes}")
print(f"Average inter-spike interval: {spike_stats.averageInterSpikeInterval} ms")
print(f"Firing rate variability: {spike_stats.firingRateVariability}")

print("\n=== Network Topology ===")
topology = results.topologyAnalysis
print(f"Average path length: {topology.averagePathLength}")
print(f"Clustering coefficient: {topology.clusteringCoefficient}")
print(f"Degree distribution: {topology.degreeDistribution}")

# Generate detailed report
benchmark.generateReport("benchmark_results.html")
```

### Spike Timing Studies

Analyze precise spike timing and patterns:

```python
import pynlm

# Create spike analyzer
spike_analyzer = pynlm.SpikeAnalyzer()

# Run with experimental data
benchmark = pynlm.AdvancedBenchmark()
results = benchmark.run()

# Analyze different spike patterns
spike_patterns = spike_analyzer.analyzePatterns(results.spikeTimes)

print("=== Spike Pattern Analysis ===")
for pattern_name, stats in spike_patterns.items():
    print(f"\n{pattern_name}:")
    print(f"  Frequency: {stats.frequency} Hz")
    print(f"  Coefficient of variation: {stats.coefficientOfVariation}")
    print(f"  Synchrony index: {stats.synchronyIndex}")

# Detect burst patterns
bursts = spike_analyzer.detectBursts(results.spikeTimes, threshold=3.0)
print(f"\nDetected {len(bursts)} burst periods")
```

## Advanced Export/Import System

### Checkpoint with Compression

Save and load brain states with advanced compression:

```python
import pynlm

# Export with compression
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation first
for i in range(500):
    brain.step(i)

# Export with ZSTD compression (high compression, good speed)
export_manager = pynlm.AdvancedExportImport()
export_manager.exportBrain(
    brain,
    "advanced_checkpoint",
    compression=pynlm.CompressionFormat.ZSTD,
    level=9
)

# Load with automatic decompression
loaded_brain = export_manager.importBrain(
    "advanced_checkpoint",
    format=pynlm.ImportFormat.AUTO
)

print(f"✓ Exported checkpoint with {export_manager.getCompressionRatio():.1f}x compression ratio")
print(f"✓ Successfully loaded brain state with {loaded_brain.getTotalNeuronCount()} neurons")
```

### Multiple Export Formats

Export in different formats for different use cases:

```python
import pynlm

export_manager = pynlm.AdvancedExportImport()

# JSON format (human-readable, easy to parse)
export_manager.exportBrain(
    brain,
    "brain_state.json",
    format=pynlm.ExportFormat.JSON,
    includeMetadata=True
)

# Binary format (compact, fast)
export_manager.exportBrain(
    brain,
    "brain_state.bin",
    format=pynlm.ExportFormat.BINARY
)

# Compressed format (space-efficient)
export_manager.exportBrain(
    brain,
    "brain_state.gz",
    format=pynlm.ExportFormat.COMPRESS,
    compression=pynlm.CompressionFormat.GZIP,
    level=6
)

# Incremental backup for long-running experiments
export_manager.exportBrain(
    brain,
    "incremental_backup_",
    format=pynlm.ExportFormat.INCREMENTAL,
    backupPoint=True
)
```

### Export Metadata and Analysis

Include detailed metadata for research reproducibility:

```python
import pynlm

export_manager = pynlm.AdvancedExportImport()

export_manager.exportBrain(
    brain,
    "research_checkpoint",
    includeMetadata=True,
    metadata={
        "experiment_name": "memory_learning_study",
        "researcher": "Dr. Smith",
        "date": "2024-01-15",
        "parameters": {
            "neuron_count": 1500,
            "learning_rate": 0.05,
            "reward_decay": 0.95
        },
        "environment": {
            "width": 20,
            "height": 20,
            "vision_width": 8,
            "vision_height": 8
        },
        "analysis_results": {
            "final_firing_rate": 2.3,
            "total_spikes": 1847,
            "memory_retention": 0.87
        }
    }
)

# Verify metadata integrity
metadata = export_manager.verifyIntegrity("research_checkpoint")
print(f"✓ Metadata integrity verified: {metadata.valid}")
print(f"✓ Export timestamp: {metadata.timestamp}")
```

## Advanced Experimentation Tools

### Parameter Sweep Automation

Run systematic parameter sweeps for research:

```python
import pynlm

# Define parameter sweep configuration
sweep_config = pynlm.ParameterSweepConfig()
sweep_config.addParameter("neuron_count", [500, 1000, 2000])
sweep_config.addParameter("plasticity_learning_rate", [0.01, 0.05, 0.1])
sweep_config.addParameter("reward_discount_factor", [0.9, 0.95, 0.99])
sweep_config.setNumSteps(200)
sweep_config.setParallelExecution(True)
sweep_config.setSaveIntermediateResults(True)

# Run sweep
experiment_runner = pynlm.AdvancedExperimentRunner()
results = experiment_runner.runParameterSweep(
    sweep_config,
    "parameter_sweep_optimization",
    outputDirectory="sweep_results"
)

# Analyze results
print("=== Parameter Sweep Results ===")
for param_name, param_results in results.parameterResults.items():
    print(f"\n{param_name}:")
    for value, metrics in param_results.items():
        print(f"  Value {value}: Learning rate {metrics.learningRate:.3f}, "
              f"Accuracy {metrics.accuracy:.3f}, Time {metrics.timeMs:.1f}ms")

# Find optimal parameters
optimal_config = experiment_runner.findOptimalParameters(results)
print(f"\n✓ Optimal configuration: {optimal_config}")
```

### Genetic Algorithm Optimization

Use genetic algorithms to automatically find optimal configurations:

```python
import pynlm

# Configure genetic algorithm
genetic_config = pynlm.OptimizationConfig()
genetic_config.setAlgorithm(pynlm.AlgorithmType.GENETIC)
genetic_config.setPopulationSize(50)
genetic_config.setGenerations(100)
genetic_config.setMutationRate(0.1)
genetic_config.setCrossoverRate(0.8)

# Define fitness function (example: maximize learning with constraints)
def fitness_function(config):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    # Calculate fitness based on learning performance
    learning_score = brain.getAverageFiringRate() * brain.getExcitationInhibitionRatio()
    
    # Apply constraints
    if config.getOr("neuron_count", 0) > 5000:
        learning_score *= 0.5  # Penalty for too many neurons
    
    return learning_score

# Run optimization
experiment_runner = pynlm.AdvancedExperimentRunner()
optimization_result = experiment_runner.optimize(
    fitness_function,
    genetic_config,
    "brain_optimization",
    outputDirectory="optimization_results"
)

print(f"=== Optimization Results ===")
print(f"Best fitness: {optimization_result.bestFitness}")
print(f"Optimal neurons: {optimization_result.optimalConfiguration.getOr('neuron_count', 0)}")
print(f"Optimal learning rate: {optimization_result.optimalConfiguration.getOr('plasticity_learning_rate', 0)}")
print(f"Convergence generation: {optimization_result.convergenceGeneration}")

# Save optimal configuration
optimization_result.optimalConfiguration.saveToFile("optimal_brain.cfg")
```

### Batch Processing

Process multiple experiments in parallel:

```python
import pynlm

# Create batch processor
experiment_runner = pynlm.AdvancedExperimentRunner()

# Define multiple experiment configurations
experiments = [
    {"name": "small_brain", "neuron_count": 500, "steps": 100},
    {"name": "medium_brain", "neuron_count": 1000, "steps": 200},
    {"name": "large_brain", "neuron_count": 2000, "steps": 300},
    {"name": "very_large_brain", "neuron_count": 5000, "steps": 400}
]

# Run batch experiment
batch_results = experiment_runner.runBatch(
    experiments,
    "performance_comparison",
    outputDirectory="batch_results",
    parallel=True,
    maxWorkers=4
)

# Compare results across experiments
comparison = experiment_runner.compareExperiments(batch_results.experiments)
print("=== Batch Experiment Comparison ===")
print(f"Fastest: {comparison.fastestExperiment}")
print(f"Most efficient: {comparison.mostEfficientExperiment}")
print(f"Best learning performance: {comparison.bestLearningExperiment}")

# Generate comprehensive report
experiment_runner.generateBatchReport(batch_results, "batch_report.html")
```

## Command-Line Interface

### Advanced Commands

All advanced features are accessible via command-line:

```bash
# Configuration comparison
./nlm-configcmp original_config.json new_config.json --validate --report

# Advanced benchmarking
./nlm-benchmark --neurons 2000 --steps 1000 --memory --spikes --topology --output benchmark.json

# Export with compression
./nlm-export --input brain_state --output checkpoint.gz --compress zstd --level 9

# Parameter sweep
./nlm-experiment sweep --param neurons:[100,500,1000] --param steps:[100,500] --output sweep_results

# Genetic optimization
./nlm-experiment optimize --algorithm genetic --population 50 --generations 100 --output opt_results

# Batch processing
./nlm-experiment batch --experiments exp1.json,exp2.json,exp3.json --parallel --workers 4
```

### Help and Documentation

Get detailed help for each command:

```bash
# Get help for configuration comparison
./nlm-configcmp --help

# Get help for benchmarking
./nlm-benchmark --help

# Get help for export/import
./nlm-export --help

# Get help for experimentation
./nlm-experiment --help
```

## Performance Monitoring

### Real-Time Monitoring

Monitor system resources during long-running experiments:

```python
import pynlm
import time
import threading

# Create performance monitor
performance_monitor = pynlm.PerformanceMonitor()

# Start monitoring in background
performance_monitor.startMonitoring()

# Run experiment
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(1000):
    brain.step(i)
    
    # Print real-time metrics every 100 steps
    if i % 100 == 0:
        metrics = performance_monitor.getCurrentMetrics()
        print(f"Step {i}: Memory={metrics.memoryUsageMB:.1f}MB, "
              f"CPU={metrics.cpuUsage}%, "
              f"Spikes={brain.getTotalSpikeCount()}")

# Stop monitoring
performance_monitor.stopMonitoring()

# Generate performance report
performance_monitor.generateReport("performance_report.html")
```

### Memory Efficiency Analysis

Analyze and optimize memory usage:

```python
import pynlm

# Create memory profiler
memory_profiler = pynlm.MemoryProfiler()

# Profile different configurations
configs = [
    {"neuron_count": 500, "name": "small"},
    {"neuron_count": 1000, "name": "medium"},
    {"neuron_count": 2000, "name": "large"}
]

memory_results = {}
for config_spec in configs:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Profile memory usage
    memory_profiler.startProfiling()
    for step in range(config_spec["steps"]):
        brain.step(step)
    memory_usage = memory_profiler.stopProfiling()
    
    memory_results[config_spec["name"]] = {
        "memory_usage": memory_usage,
        "neurons": config_spec["neuron_count"],
        "memory_efficiency": config_spec["neuron_count"] / memory_usage
    }

# Compare memory efficiency
print("=== Memory Efficiency Analysis ===")
for name, results in memory_results.items():
    print(f"{name}: {results['neurons']} neurons, "
          f"{results['memory_usage']:.1f}MB, "
          f"Efficiency: {results['memory_efficiency']:.1f} neurons/MB")
```

## Integration with Existing Tools

### Python API Examples

The advanced features integrate seamlessly with the existing Python API:

```python
import pynlm

# Create brain with optimal configuration (from optimization results)
optimal_config = pynlm.createDefaultConfig()
optimal_config.set("neuron_count", 1500)
optimal_config.set("plasticity_learning_rate", 0.05)

brain = pynlm.createBrain(optimal_config)
brain.initialize()

# Use advanced benchmarking
benchmark = pynlm.AdvancedBenchmark(brain)
results = benchmark.run(500)

# Use advanced export
export_manager = pynlm.AdvancedExportImport()
export_manager.exportBrain(brain, "production_checkpoint")

# Use experiment tools
experiment_runner = pynlm.AdvancedExperimentRunner()
experiment_runner.runParameterSweep(sweep_config, "production_analysis")
```

## Research Applications

### Neuroscience Research

Applications for neuroscience research include:

- **Neural Development Studies**: Track how brain structures develop over time
- **Memory Consolidation**: Analyze how memories are consolidated and replayed
- **Learning Dynamics**: Study learning rates and plasticity mechanisms
- **Network Topology**: Investigate brain connectivity patterns and small-world properties

### Machine Learning Research

Applications for machine learning research include:

- **Neural Architecture Search**: Automatically discover optimal network configurations
- **Hyperparameter Optimization**: Find optimal training parameters using genetic algorithms
- **Performance Profiling**: Identify bottlenecks in neural network implementations
- **Resource Allocation**: Optimize memory usage and computational efficiency

## File Organization

The advanced features are organized in dedicated directories:

```
src/
├── advanced_benchmark/           # Performance benchmarking tools
├── advanced_export_import/        # Checkpoint export/import system
├── experiment_tools/              # Experimentation and optimization
├── core/                          # Core NLM functionality (enhanced)
└── ...                           # Other existing directories
```

## Getting Started

1. **Install Advanced Features**: Add `-DCMAKE_ENABLE_ADVANCED_FEATURES=ON` to CMake configuration
2. **Build the Tools**: Run `make` in the build directory
3. **Learn the APIs**: Study the Python API documentation and C++ headers
4. **Run Examples**: Use the provided example scripts and command-line tools
5. **Contribute**: Add your own advanced features and tools!

## Conclusion

The advanced features significantly extend the NLM framework's capabilities for research, development, and experimentation. They provide:

- **Comprehensive Performance Analysis**: From memory tracking to spike pattern studies
- **Powerful Experimentation Tools**: Parameter sweeps, genetic algorithms, and batch processing
- **Robust Configuration Management**: Templates, inheritance, and validation
- **Efficient Data Management**: Compression, incremental backups, and metadata
- **Research Support**: Integration with neuroscience and machine learning workflows

These features make NLM suitable for advanced research applications while maintaining backward compatibility with existing functionality.