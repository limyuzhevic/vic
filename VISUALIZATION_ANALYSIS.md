# NLM Visualization and Analysis Documentation

## Overview

The NLM Visualization and Analysis framework provides comprehensive tools for visualizing, analyzing, and understanding neural learning machine simulations. It includes multiple analysis classes for different aspects of neural network behavior, from network topology to time series analysis and performance metrics.

This framework is designed for:
- **Research Visualization** - Publication-quality visualizations
- **Debugging** - Real-time monitoring and debugging
- **Performance Analysis** - Bottleneck identification and optimization
- **Statistical Analysis** - Hypothesis testing and validation
- **Machine Learning** - Activity patterns as ML features
- **Education** - Teaching neural network concepts

---

## Core Components

### 1. NeuralVisualization

**Purpose**: Comprehensive neural network visualization with multiple output formats and customizable parameters.

**Key Features**:
- Network topology visualization
- Activity visualization
- Synaptic weight visualization
- Connection strength visualization
- Heat map generation
- File export (PNG, SVG, ASCII)
- Animation support
- Custom color schemes and layouts
- Metadata export

**Python API**:
```python
import pynlm

# Create neural visualization
brain = pynlm.createQuickTestBrain()
visualizer = pynlm.NeuralVisualization(brain, pynlm.NeuralVisualization.OutputFormat.PNG, pynlm.NeuralVisualization.ColorScheme.Thermal)

# Visualize network topology
network_data = visualizer.visualizeNetwork()

# Visualize current neuron activity
activity_data = visualizer.visualizeActivity()

# Visualize synaptic weights
weight_data = visualizer.visualizeWeights()

# Visualize connection strength
connection_data = visualizer.visualizeConnections()

# Save visualization to file
visualizer.saveVisualization("network.png", pynlm.NeuralVisualization.OutputFormat.PNG)

# Set output format
visualizer.setOutputFormat(pynlm.NeuralVisualization.OutputFormat.SVG)

# Set color scheme
visualizer.setColorScheme(pynlm.NeuralVisualization.ColorScheme.Plasma)

# Set custom colormap
custom_colormap = {0.0: (0.0, 0.0, 1.0), 0.5: (1.0, 1.0, 1.0), 1.0: (1.0, 0.0, 0.0)}
visualizer.setCustomColormap(custom_colormap)
```

**Key Methods**:
- `visualizeNetwork()` - Visualize network topology
- `visualizeActivity()` - Visualize current neuron activity
- `visualizeWeights()` - Visualize synaptic weights
- `visualizeConnections()` - Visualize connection strength
- `saveVisualization()` - Save visualization to file
- `setOutputFormat()` - Set output format
- `getOutputFormat()` - Get current output format
- `setColorScheme()` - Set color scheme
- `getColorScheme()` - Get current color scheme
- `setCustomColormap()` - Set custom colormap
- `getCustomColormap()` - Get custom colormap

### 2. NeuralActivityAnalyzer

**Purpose**: Advanced neural activity analysis including firing pattern analysis, oscillation detection, and network cohesion computation.

**Key Features**:
- Firing pattern analysis
- Oscillation detection
- Network cohesion computation
- Neuron identification by activity
- Functional connectivity matrix computation
- Significant event detection
- Time-varying statistics
- Neuron clustering
- Network entropy computation
- Activity profile generation

**Python API**:
```python
import pynlm

# Create neural activity analyzer
analyzer = pynlm.NeuralActivityAnalyzer(brain)

# Analyze firing patterns
firing_patterns = analyzer.analyzeFiringPatterns()

# Detect oscillations
oscillations = analyzer.detectOscillations()

# Compute network cohesion
cohesion = analyzer.computeCohesion()

# Identify neurons by activity
identified_neurons = analyzer.identifyNeurons()

# Compute functional connectivity matrix
connectivity_matrix = analyzer.computeConnectivityMatrix()

# Detect significant events
events = analyzer.detectEvents(
    event_types=[
        pynlm.NeuralActivityAnalyzer.EventResult.EventType.Burst,
        pynlm.NeuralActivityAnalyzer.EventResult.EventType.SilentPeriod,
        pynlm.NeuralActivityAnalyzer.EventResult.EventType.SpikeRipples
    ],
    significance_threshold=0.95
)

# Compute network entropy
network_entropy = analyzer.computeEntropy()

# Generate activity profile
activity_profile = analyzer.generateActivityProfile()

# Identify neuron clusters
clusters = analyzer.identifyClusters()

# Get time-varying statistics
stats = analyzer.getTimeVaryingStatistics()
```

**Key Methods**:
- `analyzeFiringPatterns()` - Analyze firing patterns
- `detectOscillations()` - Detect oscillations
- `computeCohesion()` - Compute network cohesion
- `identifyNeurons()` - Identify neurons by activity
- `computeConnectivityMatrix()` - Compute functional connectivity matrix
- `detectEvents()` - Detect significant events
- `computeEntropy()` - Compute network entropy
- `generateActivityProfile()` - Generate activity profile
- `identifyClusters()` - Identify neuron clusters
- `getTimeVaryingStatistics()` - Get time-varying statistics

### 3. TimeSeriesAnalyzer

**Purpose**: Time series analysis for neural recording data with power spectrum computation, stationarity detection, and forecasting.

**Key Features**:
- Power spectrum computation
- Stationarity detection
- Autocorrelation analysis
- ARIMA model fitting
- Regime change detection
- Future prediction
- Statistical moments computation
- Frequency domain features
- Time-frequency representation
- Model validation

**Python API**:
```python
import pynlm
import numpy as np

# Create time series analyzer
time_series_data = np.array([...])  # Your time series data
sampling_rate = 1000.0  # Hz
ts_analyzer = pynlm.TimeSeriesAnalyzer(time_series_data, sampling_rate)

# Compute power spectrum
power_spectrum = ts_analyzer.computePowerSpectrum()

# Detect stationarity
stationarity_adf = ts_analyzer.detectStationarity("adf")
stationarity_pp = ts_analyzer.detectStationarity("pp")
stationarity_kpss = ts_analyzer.detectStationarity("kpss")

# Compute autocorrelation
autocorrelation = ts_analyzer.computeAutoCorrelation()

# Fit ARIMA model
arima_config = pynlm.TimeSeriesAnalyzer.ARIMAConfig()
arima_config.p = 2  # Autoregressive order
arima_config.d = 1  # Differencing order
arima_config.q = 0  # Moving average order

arima_model = ts_analyzer.fitARIMA(arima_config)

# Predict future
predictions = ts_analyzer.predictFuture(10)  # Predict next 10 time steps

# Detect regime changes
regimes = ts_analyzer.detectRegimes()

# Compute statistical moments
moments = ts_analyzer.computeStatisticalMoments()

# Get frequency domain features
frequency_features = ts_analyzer.computeFrequencyDomainFeatures()

# Get time-frequency representation
time_frequency = ts_analyzer.computeTimeFrequencyRepresentation()

# Validate model
validation_results = ts_analyzer.validateModel(arima_model)
```

**Key Methods**:
- `computePowerSpectrum()` - Compute power spectrum
- `detectStationarity()` - Detect stationarity (ADF, PP, KPSS)
- `computeAutoCorrelation()` - Compute autocorrelation
- `fitARIMA()` - Fit ARIMA model
- `predictFuture()` - Predict future values
- `detectRegimes()` - Detect regime changes
- `computeStatisticalMoments()` - Compute statistical moments
- `computeFrequencyDomainFeatures()` - Compute frequency domain features
- `computeTimeFrequencyRepresentation()` - Compute time-frequency representation
- `validateModel()` - Validate model

### 4. PerformanceMetrics

**Purpose**: System performance analysis including efficiency computation, scalability metrics, and performance profiling.

**Key Features**:
- Simulation efficiency computation
- Scalability metrics
- Performance report generation
- Implementation comparison
- Performance profiling
- Bottleneck identification
- Optimization recommendations
- Performance trend analysis
- Performance score calculation
- Heat map generation
- Variance analysis

**Python API**:
```python
import pynlm

# Create performance metrics
perf_metrics = pynlm.PerformanceMetrics(brain)

# Compute efficiency
benchmark = pynlm.PerformanceMetrics.Benchmark()
benchmark.execution_time = 10.0
benchmark.memory_usage = 500.0
benchmark.cpu_usage = 80.0
benchmark.steps_completed = 1000
efficiency = perf_metrics.computeEfficiency(benchmark)

# Compute scalability
benchmarks = [
    pynlm.PerformanceMetrics.Benchmark(name="small", execution_time=5.0, memory_usage=200.0, cpu_usage=60.0, steps_completed=500),
    pynlm.PerformanceMetrics.Benchmark(name="medium", execution_time=10.0, memory_usage=500.0, cpu_usage=80.0, steps_completed=1000),
    pynlm.PerformanceMetrics.Benchmark(name="large", execution_time=20.0, memory_usage=1000.0, cpu_usage=90.0, steps_completed=2000),
]
scalability = perf_metrics.computeScalability(benchmarks)

# Generate performance report
text_report = perf_metrics.generateReport("text")
json_report = perf_metrics.generateReport("json")
html_report = perf_metrics.generateReport("html")

# Compare implementations
implementations = ["impl1", "impl2", "impl3"]
comparison = perf_metrics.compareImplementations(implementations)

# Profile performance
profile = perf_metrics.profilePerformance()
print(f"CPU utilization: {profile.cpuUtilization}%")
print(f"Memory utilization: {profile.memoryUtilization}%")
print(f"Disk I/O: {profile.diskIO}")
print(f"Network I/O: {profile.networkIO}")

# Generate optimization recommendations
recommendations = perf_metrics.generateOptimizationRecommendations()

# Identify bottlenecks
bottlenecks = perf_metrics.identifyBottlenecks()

# Get performance score
score = perf_metrics.getPerformanceScore()

# Generate heat map
perf_metrics.generateHeatMap("performance_heatmap.png")

# Analyze variance
variance_analysis = perf_metrics.analyzeVariance()
```

**Key Methods**:
- `computeEfficiency()` - Compute efficiency
- `computeScalability()` - Compute scalability metrics
- `generateReport()` - Generate performance report
- `compareImplementations()` - Compare implementations
- `profilePerformance()` - Profile performance
- `generateOptimizationRecommendations()` - Generate optimization recommendations
- `identifyBottlenecks()` - Identify performance bottlenecks
- `getPerformanceScore()` - Get performance score
- `generateHeatMap()` - Generate performance heat map
- `analyzeVariance()` - Analyze performance variance

---

## Convenience Functions

**Purpose**: High-level convenience functions for common visualization and analysis tasks.

**Available Functions**:
- `createVisualizer()` - Factory function for creating visualization objects
- `analyzeNetwork()` - Quick network analysis function
- `generatePerformanceReport()` - Generate comprehensive performance reports
- `createTimeSeriesAnalyzer()` - Create time series analyzer from experimental metrics
- `visualizeExperimentalResults()` - Visualize experimental results
- `compareExperimentalRuns()` - Compare multiple experimental runs
- `exportVisualizationResults()` - Export visualization results to multiple formats
- `loadExperimentalConfiguration()` - Load experimental configuration from file
- `runComprehensiveAnalysis()` - Run complete analysis pipeline
- `createCombinedVisualization()` - Create combined visualization from brain and metrics
- `generateComprehensiveReport()` - Generate comprehensive analysis report

**Python API**:
```python
import pynlm

# Create quick visualizer
visualizer = pynlm.createVisualizer(brain, "PNG")

# Quick network analysis
analysis = pynlm.analyzeNetwork(brain, "full")
firing_patterns = analysis.analyzeFiringPatterns()

# Generate performance report
perf_report = pynlm.generatePerformanceReport(brain)
efficiency = perf_report.computeEfficiency(benchmark)

# Create time series analyzer
ts_analyzer = pynlm.createTimeSeriesAnalyzer(metrics, 1000.0)

# Visualize experimental results
results = pynlm.visualizeExperimentalResults(experiment_metrics)

# Compare experimental runs
comparison = pynlm.compareExperimentalRuns([metrics1, metrics2, metrics3], "total_reward")

# Export visualization results
pynlm.exportVisualizationResults(visualization_data, "output_dir", formats=["png", "svg", "json"])

# Load experimental configuration
config = pynlm.loadExperimentalConfiguration("config.json")

# Run comprehensive analysis
comprehensive_results = pynlm.runComprehensiveAnalysis(brain, metrics)

# Create combined visualization
combined = pynlm.createCombinedVisualization(brain, metrics, "combined_analysis.png")

# Generate comprehensive report
report = pynlm.generateComprehensiveReport(brain, metrics, visualization_results)
```

---

## Usage Examples

### Example 1: Basic Neural Network Visualization

```python
import pynlm

# Create a brain
brain = pynlm.createQuickTestBrain()

# Create neural visualization
visualizer = pynlm.NeuralVisualization(brain, pynlm.NeuralVisualization.OutputFormat.PNG)

# Visualize network topology
network_data = visualizer.visualizeNetwork()
visualizer.saveVisualization("network_topology.png")

# Visualize activity
activity_data = visualizer.visualizeActivity()
visualizer.saveVisualization("activity.png")

# Visualize weights
weight_data = visualizer.visualizeWeights()
visualizer.saveVisualization("synaptic_weights.png")

print("Visualization complete!")
```

### Example 2: Neural Activity Analysis

```python
import pynlm

# Create brain and run simulation
brain = pynlm.createQuickTestBrain()
brain.initialize()

# Run simulation for a few steps
for i in range(100):
    brain.step(i)

# Create neural activity analyzer
analyzer = pynlm.NeuralActivityAnalyzer(brain)

# Analyze firing patterns
firing_patterns = analyzer.analyzeFiringPatterns()
print(f"Firing patterns: {firing_patterns}")

# Detect oscillations
oscillations = analyzer.detectOscillations()
print(f"Oscillation frequency: {oscillations.frequency:.3f} Hz")
print(f"Oscillation power: {oscillations.power:.3f}")

# Compute network cohesion
cohesion = analyzer.computeCohesion()
print(f"Network cohesion: {cohesion:.3f}")

# Identify neurons by activity
identified_neurons = analyzer.identifyNeurons()
print(f"Identified {len(identified_neurons)} neurons")

# Compute functional connectivity matrix
connectivity_matrix = analyzer.computeConnectivityMatrix()
print(f"Connectivity matrix shape: {connectivity_matrix.shape}")
```

### Example 3: Time Series Analysis

```python
import pynlm
import numpy as np

# Create time series data (simulated neural data)
time_steps = 1000
sampling_rate = 1000.0
time_series_data = np.random.randn(time_steps) * 0.1 + np.cumsum(np.random.randn(time_steps) * 0.01)

# Create time series analyzer
ts_analyzer = pynlm.TimeSeriesAnalyzer(time_series_data, sampling_rate)

# Compute power spectrum
power_spectrum = ts_analyzer.computePowerSpectrum()

# Detect stationarity
stationarity_adf = ts_analyzer.detectStationarity("adf")
stationarity_pp = ts_analyzer.detectStationarity("pp")

print(f"ADF stationarity p-value: {stationarity_adf.p_value:.3f}")
print(f"PP stationarity p-value: {stationarity_pp.p_value:.3f}")

# Compute autocorrelation
autocorrelation = ts_analyzer.computeAutoCorrelation()
print(f"Autocorrelation at lag 1: {autocorrelation[1]:.3f}")
print(f"Autocorrelation at lag 10: {autocorrelation[10]:.3f}")

# Fit ARIMA model
arima_config = pynlm.TimeSeriesAnalyzer.ARIMAConfig()
arima_config.p = 1  # AR order
arima_config.d = 1  # Difference order
arima_config.q = 1  # MA order

arima_model = ts_analyzer.fitARIMA(arima_config)
print(f"ARIMA model AIC: {arima_model.aic:.3f}")
print(f"ARIMA model BIC: {arima_model.bic:.3f}")

# Predict future
predictions = ts_analyzer.predictFuture(5)
print(f"Future predictions: {predictions}")

# Detect regime changes
regimes = ts_analyzer.detectRegimes()
print(f"Detected {len(regimes)} regime changes")
```

### Example 4: Performance Analysis

```python
import pynlm

# Create brain and run simulation
brain = pynlm.createQuickTestBrain()
brain.initialize()

# Run simulation for longer time
for i in range(1000):
    brain.step(i)

# Create performance metrics
perf_metrics = pynlm.PerformanceMetrics(brain)

# Compute efficiency
benchmark = pynlm.PerformanceMetrics.Benchmark()
benchmark.execution_time = 10.0
benchmark.memory_usage = 500.0
benchmark.cpu_usage = 80.0
benchmark.steps_completed = 1000
efficiency = perf_metrics.computeEfficiency(benchmark)

print(f"Efficiency score: {efficiency:.3f}")

# Compute scalability
benchmarks = [
    pynlm.PerformanceMetrics.Benchmark(name="small", execution_time=5.0, memory_usage=200.0, cpu_usage=60.0, steps_completed=500),
    pynlm.PerformanceMetrics.Benchmark(name="medium", execution_time=10.0, memory_usage=500.0, cpu_usage=80.0, steps_completed=1000),
    pynlm.PerformanceMetrics.Benchmark(name="large", execution_time=20.0, memory_usage=1000.0, cpu_usage=90.0, steps_completed=2000),
]

scalability = perf_metrics.computeScalability(benchmarks)
print(f"Scalability score: {scalability:.3f}")

# Generate performance report
report = perf_metrics.generateReport("text")
print("Performance report:")
print(report)

# Profile performance
profile = perf_metrics.profilePerformance()
print(f"\nPerformance profile:")
print(f"CPU utilization: {profile.cpuUtilization}%")
print(f"Memory utilization: {profile.memoryUtilization}%")
print(f"Disk I/O: {profile.diskIO}")
print(f"Network I/O: {profile.networkIO}")
print(f"Steps per second: {profile.stepsPerSecond}")
```

### Example 5: Comprehensive Analysis Pipeline

```python
import pynlm
import numpy as np

# Create brain and run simulation
brain = pynlm.createQuickTestBrain()
brain.initialize()

# Run simulation
for i in range(500):
    brain.step(i)

# Run comprehensive analysis
comprehensive_results = pynlm.runComprehensiveAnalysis(brain)

# Get visualization results
visualizations = comprehensive_results.visualizations
print(f"Generated {len(visualizations)} visualizations")

# Get analysis results
analysis_results = comprehensive_results.analysis_results
print(f"Analysis results: {analysis_results}")

# Get performance metrics
performance_results = comprehensive_results.performance_results
print(f"Performance score: {performance_results.performance_score:.3f}")

# Get experiment metrics
experiment_results = comprehensive_results.experiment_results
print(f"Experiment metrics: {experiment_results}")

# Export results
pynlm.exportVisualizationResults(comprehensive_results, "comprehensive_analysis", formats=["png", "json", "csv"])

print("Comprehensive analysis complete!")
```

---

## Best Practices

### Visualization Best Practices
```python
# Use appropriate output formats
visualizer = pynlm.NeuralVisualization(brain, pynlm.NeuralVisualization.OutputFormat.SVG)

# Use appropriate color schemes
visualizer.setColorScheme(pynlm.NeuralVisualization.ColorScheme.Thermal)

# Set reasonable image sizes
visualizer.setImageSize(1920, 1080)

# Use appropriate dpi for publication
visualizer.setDPI(300)

# Add informative labels and annotations
visualizer.enableLabels(True)
visualizer.addAnnotations([...])
```

### Analysis Best Practices
```python
# Use appropriate analysis parameters
analyzer = pynlm.NeuralActivityAnalyzer(brain)
analyzer.setSignificanceThreshold(0.95)
analyzer.setMinClusterSize(10)

# Validate analysis results
analyzer.validateResults()

# Use cross-validation for time series analysis
ts_analyzer = pynlm.TimeSeriesAnalyzer(data, sampling_rate)
ts_analyzer.setCrossValidationSplits(5)

# Use appropriate ARIMA model selection
arima_config = pynlm.TimeSeriesAnalyzer.ARIMAConfig()
for p in [0, 1, 2]:
    for d in [0, 1]:
        for q in [0, 1, 2]:
            arima_config.p = p
            arima_config.d = d
            arima_config.q = q
            model = ts_analyzer.fitARIMA(arima_config)
            # Evaluate model
```

### Performance Analysis Best Practices
```python
# Use appropriate benchmarking
benchmark = pynlm.PerformanceMetrics.Benchmark()
benchmark.name = "neural_network_test"
benchmark.execution_time = time.time() - start_time
benchmark.memory_usage = get_memory_usage()
benchmark.cpu_usage = get_cpu_usage()
benchmark.steps_completed = brain.getTotalSpikeCount()

# Use appropriate comparison baselines
baselines = ["impl1", "impl2", "impl3"]
comparison = perf_metrics.compareImplementations(baselines)

# Use appropriate performance metrics
score = perf_metrics.getPerformanceScore()
recommendations = perf_metrics.generateOptimizationRecommendations()
```

---

## Integration with Other NLM Components

### Integration with Brain
- Direct integration with Brain instances
- Real-time visualization capabilities
- Activity-based analysis
- Network topology visualization

### Integration with Experiment Management
- Export analysis results to experiment metrics
- Import visualization data into experiment reports
- Comparative analysis across experiments
- Performance tracking and optimization

### Integration with Training Systems
- Visualization of training progress
- Performance analysis during training
- Learning curve visualization
- Optimization recommendation generation

### Integration with Convenience Functions
- High-level analysis pipelines
- Automated visualization generation
- Comprehensive report generation
- Multi-format export support

---

## Performance Considerations

### Visualization Performance
```python
# Use efficient visualization formats
visualizer = pynlm.NeuralVisualization(brain, pynlm.NeuralVisualization.OutputFormat.ASCII)

# Use appropriate resolution for performance
visualizer.setImageSize(800, 600)

# Use caching for repeated visualizations
visualizer.enableCaching(True)

# Use parallel processing for large networks
visualizer.enableParallelProcessing(True)
```

### Analysis Performance
```python
# Use efficient analysis algorithms
analyzer = pynlm.NeuralActivityAnalyzer(brain)
analyzer.useEfficientAlgorithms(True)

# Use appropriate data types
analyzer.useFloat32(True)

# Use memory-mapped files for large datasets
analyzer.useMemoryMapping(True)
```

### Export Performance
```python
# Use efficient export formats
pynlm.exportVisualizationResults(data, "output", formats=["json", "csv"])

# Use compression for large files
pynlm.exportVisualizationResults(data, "output", compression=True)

# Use streaming for large datasets
pynlm.exportVisualizationResults(data, "output", streaming=True)
```

---

## Conclusion

The NLM Visualization and Analysis framework provides comprehensive tools for visualizing, analyzing, and understanding neural learning machine simulations. It includes:

- **Multiple visualization classes** - NeuralVisualization for comprehensive network visualization
- **Multiple analysis classes** - NeuralActivityAnalyzer, TimeSeriesAnalyzer, PerformanceMetrics for different types of analysis
- **Convenience functions** - High-level APIs for common visualization and analysis tasks
- **Export capabilities** - Multiple output formats and export options
- **Integration support** - Seamless integration with other NLM components
- **Performance optimization** - Efficient algorithms and performance monitoring

This framework enables researchers and developers to:
- Visualize neural network topologies
- Analyze neural activity patterns
- Perform time series analysis
- Evaluate system performance
- Generate comprehensive reports
- Export results in multiple formats
- Debug and optimize neural networks

The visualization and analysis framework is an essential component of the NLM system, providing the infrastructure needed for scientific visualization and analysis of neural learning machines.

---

*Last updated: 2026-09-28*

This documentation provides comprehensive guidance for using the NLM Visualization and Analysis framework to visualize, analyze, and understand neural learning machine simulations and experiments.