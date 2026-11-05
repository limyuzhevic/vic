# NLM Profiling Utilities

Comprehensive Performance Profiling Framework for the NLM (Neural Learning Machine) Project

## Overview

The NLM Profiling Utilities provide a complete set of profiling tools designed to help users identify performance bottlenecks, optimize brain configurations, understand learning dynamics, compare different simulation approaches, and make data-driven optimization decisions.

### Key Features

- **Modern Python profiling techniques** using state-of-the-art libraries
- **Comprehensive metrics collection** for all aspects of brain simulation
- **Multiple output formats** (text, JSON, CSV) for different use cases
- **Seamless integration** with existing NLM brain API and CLI tools
- **Actionable optimization recommendations** based on analysis results
- **Extensible architecture** for adding new metrics and analysis
- **Production-ready** with minimal performance overhead

## Installation

### Prerequisites

- Python 3.8 or higher
- NLM framework with pynlm bindings

### Installation

```bash
# Install from source
pip install -e .

# Or install specific components
pip install nlm-profiling[performance]
pip install nlm-profiling[benchmark]
pip install nlm-profiling[memory]
pip install nlm-profiling[spike]
pip install nlm-profiling[learning]
pip install nlm-profiling[timeline]
```

### Development Installation

```bash
# Install in development mode with all dependencies
git clone https://github.com/nlm-project/nlm-profiling.git
cd nlm-profiling
pip install -e .[dev]
```

## Usage

### Quick Start

```python
# Import and create a profiling utility
from nlm_profiling import PerformanceMeasurementModule

# Create and run performance measurement
module = PerformanceMeasurementModule(steps=100)
result = module.run_profiling(steps=100)

# Generate insights
insights = module.generate_performance_insights(result)

# Export results
from nlm_profiling import OutputFormat
module.export_results(OutputFormat.JSON, "performance_results.json")
```

### Command Line Interface

The NLM Profiling Utilities provide comprehensive CLI interfaces for all profiling modules:

```bash
# Performance Measurement Framework
nlm-performance-measurement --steps 100
nlm-performance-measurement --steps 500 --json
nlm-performance-measurement --brain my_brain.py --steps 200

# Benchmark Harness
nlm-benchmark --config-type high_performance
nlm-benchmark --steps 200 --runs 5
nlm-benchmark --json --config-type balanced

# Memory Profiler
nlm-memory-profiler --steps 100
nlm-memory-profiler --steps 500 --tracking-interval 50
nlm-memory-profiler --json --steps 200

# Spike Rate Analyzer
nlm-spike-analyzer --steps 100
nlm-spike-analyzer --steps 500 --analysis-mode statistical
nlm-spike-analyzer --json --steps 300

# Learning Performance Metrics
nlm-learning-metrics --steps 100
nlm-learning-metrics --steps 500 --analysis-mode convergence
nlm-learning-metrics --json --steps 200

# Timeline Profiler
nlm-timeline-profiler --steps 100
nlm-timeline-profiler --steps 500 --analysis-depth detailed
nlm-timeline-profiler --json --steps 300

# Export Utilities
nlm-export --input results.json --output analysis.txt --format text
nlm-export --create-template library
nlm-export --create-template web
```

### Python API Examples

#### Performance Measurement

```python
from nlm_profiling import PerformanceMeasurementModule, OutputFormat

# Basic usage
module = PerformanceMeasurementModule(steps=100)
result = module.run_profiling(steps=100)

# Advanced usage with custom configuration
from nlm_profiling import PerformanceMeasurementConfig
config = PerformanceMeasurementConfig(
    steps=500,
    metric_interval=10,
    enable_continuity=True
)
module = PerformanceMeasurementModule(config)
result = module.run_profiling(steps=500)

# Generate and use insights
insights = module.generate_performance_insights(result)
for recommendation in insights.get("recommendations", []):
    print(f"Recommendation: {recommendation}")

# Export results
output = module.export_results(OutputFormat.JSON, "results.json")
```

#### Benchmark Harness

```python
from nlm_profiling import BenchmarkHarnessModule, Configuration, OutputFormat

# Create benchmark harness
harness = BenchmarkHarnessModule(
    steps=100,
    runs=3,
    warmup_steps=10
)

# Add configurations
configs = [
    Configuration("default", {
        "neuron_count": 100,
        "region_count": 2,
        "plasticity": True,
        "learning_rate": 0.01
    }),
    Configuration("high_performance", {
        "neuron_count": 500,
        "region_count": 3,
        "plasticity": True,
        "learning_rate": 0.001,
        "simd_enabled": True
    })
]

# Add and run benchmark
for config in configs:
    harness.add_configuration(config)

result = harness.run_profiling()
insights = harness.generate_benchmark_insights(result)

# Export comparative report
report = harness.generate_comparative_report([result])
```

#### Memory Profiler

```python
from nlm_profiling import MemoryProfilerModule, OutputFormat

# Basic memory profiling
profiler = MemoryProfilerModule(steps=100)
result = profiler.run_profiling(steps=100)

# Advanced memory profiling
from nlm_profiling import MemoryTrackingConfig
from nlm_profiling import MemoryAnalysisMode
config = MemoryTrackingConfig(
    steps=200,
    tracking_interval=20,
    analysis_mode=MemoryAnalysisMode.GROWTH_ANALYSIS
)
profiler = MemoryProfilerModule(config)
result = profiler.run_profiling(steps=200)

# Generate insights
insights = profiler.generate_memory_insights(result)
```

#### Spike Rate Analyzer

```python
from nlm_profiling import SpikeRateAnalyzerModule, OutputFormat

# Basic spike analysis
analyzer = SpikeRateAnalyzerModule(steps=100)
result = analyzer.run_profiling(steps=100)

# Advanced spike analysis
from nlm_profiling import SpikeRateAnalyzerConfig
from nlm_profiling import SpikeAnalysisMode
config = SpikeRateAnalyzerConfig(
    steps=200,
    analysis_mode=SpikeAnalysisMode.STATISTICAL,
    enable_pattern_detection=True,
    enable_burst_analysis=True
)
analyzer = SpikeRateAnalyzerModule(config)
result = analyzer.run_profiling(steps=200)

# Generate insights
insights = analyzer.generate_spike_insights(result)
```

#### Learning Performance Metrics

```python
from nlm_profiling import LearningPerformanceMetricsModule, OutputFormat

# Basic learning analysis
learner = LearningPerformanceMetricsModule(steps=100)
result = learner.run_profiling(steps=100)

# Advanced learning analysis
from nlm_profiling import LearningPerformanceMetricsConfig
from nlm_profiling import LearningAnalysisMode
config = LearningPerformanceMetricsConfig(
    steps=200,
    analysis_mode=LearningAnalysisMode.CONVERGENCE,
    convergence_threshold=0.01,
    enable_adaptation_analysis=True
)
learner = LearningPerformanceMetricsModule(config)
result = learner.run_profiling(steps=200)

# Generate insights
insights = learner.generate_learning_insights(result)
```

#### Timeline Profiler

```python
from nlm_profiling import TimelineProfilerModule, OutputFormat

# Basic timeline profiling
profiler = TimelineProfilerModule(steps=100)
result = profiler.run_profiling(steps=100)

# Advanced timeline profiling
from nlm_profiling import TimelineAnalysisConfig
config = TimelineAnalysisConfig(
    steps=200,
    event_sampling_rate=10.0,
    enable_operation_profiling=True,
    enable_step_analysis=True,
    analysis_depth="detailed"
)
profiler = TimelineProfilerModule(config)
result = profiler.run_profiling(steps=200)

# Generate insights
insights = profiler.generate_timeline_insights(result)
```

#### Export Utilities

```python
from nlm_profiling import ProfilerExporter, ExportFactory, OutputFormat

# Create exporter
exporter = ProfilerExporter()

# Export results
config = ExportFactory.create_config("json", pretty_print=True)
output = exporter.export_profiling_results(results, config)

# Export in multiple formats
formats = [OutputFormat.TEXT, OutputFormat.JSON, OutputFormat.CSV]
export_results = exporter.export_multiple_formats(results, "output/file", formats)

# Export with insights
insights = {"performance_score": 8.5}
output = exporter.export_with_insights(results, config, insights)

# Create templates
template = exporter.create_integration_template("library")
```

## Integration Examples

### Integration with Existing NLM Tools

```python
import subprocess
import json

# Run NLM performance profiler
result = subprocess.run([
    "nlm", "performance", "benchmark",
    "--num-brains", "5",
    "--benchmark-steps", "100"
], capture_output=True, text=True)

# Parse and analyze results
if result.returncode == 0:
    data = json.loads(result.stdout)
    print(f"Benchmark completed in {data['duration']:.2f}s")
    print(f"Performance: {data['steps_per_second']:.1f} steps/sec")
```

### Integration with Brain API

```python
from nlm_profiling import PerformanceMeasurementModule
from pynlm import createBrain, createDefaultConfig

# Create brain instance
config = createDefaultConfig()
brain = createBrain(config)
brain.initialize()

# Create profiling module with brain
profiler = PerformanceMeasurementModule(steps=100)
result = profiler.run_profiling(brain=brain)

# Analyze and optimize
insights = profiler.generate_performance_insights(result)
```

### Integration with Custom Applications

```python
from nlm_profiling import get_all_utilities, OutputFormat

# Get all available utilities
utilities = get_all_utilities()

# Create comprehensive analysis
for name, utility in utilities.items():
    result = utility.run_profiling(steps=100)

# Export comprehensive report
from nlm_profiling import create_exporter
exporter = create_exporter()
exporter.export_multiple_formats(results, "comprehensive_report")
```

## Configuration Options

### Common Options

- `steps`: Number of simulation steps (default: 100)
- `verbose`: Enable verbose logging (default: False)
- `json`: Output JSON (default: False)

### Performance Measurement

- `steps`: Number of steps to profile (default: 100)
- `metric_interval`: Metric collection interval (default: 1)
- `enable_continuity`: Enable continuous collection (default: True)

### Benchmark Harness

- `steps`: Number of benchmark steps (default: 100)
- `runs`: Number of benchmark runs (default: 3)
- `warmup_steps`: Warm-up steps (default: 10)

### Memory Profiler

- `steps`: Number of steps to profile (default: 100)
- `tracking_interval`: Memory snapshot interval (default: 10)
- `analysis_mode`: Analysis type (default: "growth_analysis")

### Spike Rate Analyzer

- `steps`: Number of steps to analyze (default: 100)
- `analysis_mode`: Analysis mode (default: "statistical")
- `enable_pattern_detection`: Enable pattern detection (default: True)
- `enable_burst_analysis`: Enable burst analysis (default: True)

### Learning Performance Metrics

- `steps`: Number of steps to analyze (default: 100)
- `analysis_mode`: Analysis mode (default: "convergence")
- `convergence_threshold`: Convergence threshold (default: 0.01)

### Timeline Profiler

- `steps`: Number of steps to profile (default: 100)
- `event_sampling_rate`: Event sampling rate (default: 1.0)
- `analysis_depth`: Analysis depth (default: "detailed")

### Export Utilities

- `format_type`: Output format (text, json, csv)
- `pretty_print`: Enable pretty printing (default: False)
- `include_metadata`: Include metadata (default: True)

## API Reference

See the `nlm_profiling/` directory for complete API documentation.

## File Structure

```
nlm-profiling/
├── nlm_profiling/
│   ├── __init__.py          # Package initialization
│   ├── base.py              # Core framework
│   ├── performance.py       # Performance measurement module
│   ├── benchmark.py        # Benchmark harness module
│   ├── memory.py           # Memory profiler module
│   ├── spike.py            # Spike rate analyzer module
│   ├── learning.py         # Learning performance metrics module
│   ├── timeline.py         # Timeline profiler module
│   └── export.py           # Export utilities
├── docs/                    # Documentation
│   └── ...                 # Documentation files
├── examples/                # Usage examples
│   └── ...                 # Example scripts
├── tests/                   # Test files
│   └── ...                 # Test suite
├── setup.py                # Installation script
├── pyproject.toml          # Project configuration
├── README.md                # This file
└── LICENSE                 # License file
```

## Development

### Setting Up Development Environment

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm-profiling.git
cd nlm-profiling

# Install in development mode
pip install -e .[dev]

# Install development dependencies
pip install pytest pytest-cov flake8 black mypy
```

### Running Tests

```bash
# Run all tests
pytest

# Run specific test module
pytest tests/test_performance.py

# Run tests with coverage
pytest --cov=nlm_profiling --cov-report=html

# Run linting
flake8 nlm_profiling/
black --check nlm_profiling/
```

### Contributing

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests for new functionality
5. Run the test suite
6. Submit a pull request

### Code Quality Standards

- Follow PEP 8 style guidelines
- Use type hints for better code documentation
- Write comprehensive docstrings
- Add tests for all new functionality
- Use meaningful variable and function names
- Keep functions focused and single-purpose

## Testing

### Test Structure

The test suite includes:

- **Unit tests** for individual modules and classes
- **Integration tests** for module interactions
- **Performance tests** for benchmarking
- **Configuration tests** for configuration management
- **Export tests** for export functionality

### Running Tests

```bash
# Run the full test suite
pytest

# Run specific test modules
pytest tests/test_performance_measurement.py
pytest tests/test_benchmark_harness.py
pytest tests/test_memory_profiler.py
pytest tests/test_spike_rate_analyzer.py
pytest tests/test_learning_performance.py
pytest tests/test_timeline_profiler.py
pytest tests/test_export_utilities.py

# Run tests with verbose output
pytest -v

# Run tests with coverage
pytest --cov=nlm_profiling --cov-report=term-missing
```

### Test Coverage

The test suite aims for high code coverage:
- Core framework: > 90%
- Profiling utilities: > 85%
- Export utilities: > 80%
- CLI interfaces: > 75%

## Performance Considerations

### Profiling Overhead

The profiling utilities are designed to minimize performance overhead:

- **Lightweight metric collection**: Use efficient data structures
- **Configurable sampling**: Adjust sampling frequency based on needs
- **Lazy evaluation**: Only collect metrics when needed
- **Memory efficient**: Use memory pools and efficient data structures

### Best Practices

1. **Use appropriate sampling rates**: Balance between detail and performance
2. **Cache frequently accessed data**: Reduce repeated computations
3. **Use efficient algorithms**: Choose algorithms with optimal time complexity
4. **Monitor performance**: Track profiling overhead during operation
5. **Optimize for common use cases**: Focus on the most common scenarios

## Common Issues and Solutions

### Issue: High Profiling Overhead

**Solution**:
- Reduce sampling frequency
- Use more efficient algorithms
- Cache frequently accessed data
- Use lightweight data structures

### Issue: Memory Usage Increases Without Bound

**Solution**:
- Implement proper memory cleanup
- Use weak references for temporary objects
- Monitor memory usage and implement memory management
- Consider using memory pools

### Issue: Spike Rate Analysis Fails

**Solution**:
- Check if pynlm is properly installed
- Verify brain API compatibility
- Use appropriate sampling rates
- Check for null or invalid data

### Issue: Export Operations Fail

**Solution**:
- Check file permissions and disk space
- Verify output format specifications
- Implement proper error handling
- Use appropriate file paths

## Troubleshooting

### Common Error Messages

1. **Import Error**: "No module named 'pynlm'"
   - Solution: Ensure pynlm is properly installed

2. **Configuration Error**: "Invalid parameter value"
   - Solution: Check parameter types and values

3. **File Error**: "Permission denied"
   - Solution: Check file permissions and disk space

4. **Memory Error**: "Memory allocation failed"
   - Solution: Check available memory and implement memory management

### Debugging Tips

1. **Enable verbose logging**: Use the `verbose` flag for detailed logging
2. **Check error messages**: Review error messages for specific details
3. **Validate input parameters**: Ensure all parameters are valid
4. **Use small test cases**: Start with small test cases for debugging
5. **Check dependencies**: Verify all required dependencies are installed

## Future Enhancements

### Planned Features

1. **Real-time monitoring**: Implement real-time monitoring capabilities
2. **Distributed profiling**: Support distributed profiling across multiple machines
3. **GPU acceleration**: Implement GPU-accelerated profiling for large-scale simulations
4. **Machine learning integration**: Integrate machine learning for performance prediction
5. **Cloud integration**: Support cloud-based profiling and analysis

### New Metrics and Analysis

1. **Energy efficiency metrics**: Measure energy consumption and efficiency
2. **Network topology analysis**: Analyze brain network topology and structure
3. **Temperature monitoring**: Monitor simulation temperature and thermal dynamics
4. **Hardware resource utilization**: Monitor CPU, memory, and GPU usage
5. **Software architecture analysis**: Analyze software architecture and performance

### Enhanced Export Capabilities

1. **Interactive dashboards**: Create interactive web dashboards for visualization
2. **Report templates**: Provide customizable report templates
3. **API integration**: Enhance API integration capabilities
4. **Batch processing**: Implement batch processing for multiple simulations
5. **Schedule exports**: Support scheduled export operations

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgments

The NLM Profiling Utilities are built on the foundation of the NLM (Neural Learning Machine) framework and its community of contributors. We acknowledge the contributions of all developers and users who have made this project possible.

## Contact Information

For questions, feedback, or contributions, please visit the project repository or contact the development team.

---

*This README is continuously updated. Please check the project repository for the latest version.*