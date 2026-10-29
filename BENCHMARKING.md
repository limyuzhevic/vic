# BENCHMARKING GUIDE

NLM includes comprehensive performance profiling and benchmarking tools to help users evaluate and optimize their neural simulations.

## Overview

NLM provides several profiling components:

1. **Profiler** - General-purpose performance profiler for timing operations
2. **BenchmarkSystem** - System for running comprehensive benchmark suites and comparisons
3. **MemoryProfiler** - Tracks memory usage patterns and statistics
4. **ThroughputProfiler** - Measures computational throughput and efficiency

These tools can be used to:
- Compare performance across different configurations
- Identify bottlenecks in neural simulations
- Optimize resource usage
- Validate scalability with larger networks
- Track performance over time

## Quick Start

```bash
# Run basic profiler
./nlm demo --profile

# Run benchmark suite
./nlm benchmark --suite 5

# Compare configurations
./nlm benchmark --compare config1,config2,config3

# Export results to JSON
./nlm benchmark --output JSON

# Get performance summary
./nlm benchmark --stats
```

## Available Benchmark Suites

NLM provides five built-in benchmark suites:

### Suite 1: Basic Performance (Phase 2)
Tests core neural computation and plasticity:
- Neuron activation speed
- Synaptic transmission
- Memory efficiency
- Basic learning capabilities

### Suite 2: Plasticity & Learning (Phase 3)
Evaluates experience-driven learning:
- STDP effectiveness
- Hebbian learning strength
- Structural plasticity rates
- Memory formation and retention

### Suite 3: Cognitive Emergence (Phase 4)
Tests cognitive capabilities:
- Concept formation speed
- Planning accuracy
- Attention mechanisms
- Working memory capacity

### Suite 4: Architecture Scaling (Phase 5)
Evaluates performance at scale:
- Network scalability
- Memory usage patterns
- Parallel processing efficiency
- Resource optimization

### Suite 5: Integration Verification (Phase 6)
Validates complete brain integration:
- Memory system integration
- Neuromodulation effectiveness
- Prediction accuracy
- Cognitive performance

## Command Line Options

### General Benchmark Options

```
--suite N                    Run specific benchmark suite (1-5)
--compare A,B               Compare configurations A and B
--output FORMAT             Output format: JSON, CSV, or TABLE
--stats                     Print performance statistics
--help                      Show help information
```

### Advanced Options

```
--iterations N              Number of benchmark iterations (default: 5)
--config FILE              Configuration file for benchmarking
--metrics N                Comma-separated metrics: neurons, duration, throughput, memory
--parallel                  Enable parallel benchmarking
--export-all              Export all results to files
--timestamp               Include timestamps in output
```

## Configuration for Benchmarking

### Configuration File Format

Benchmark configurations support the same formats as NLM configuration (JSON, YAML, INI, key=value).

Example JSON benchmark configuration:
```json
{
  "benchmark": {
    "suite": 5,
    "iterations": 10,
    "output": "json",
    "parallel": true,
    "metrics": ["neurons", "duration", "throughput", "memory"]
  },
  "config": {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "enable_checkpoints": true
  }
}
```

### Built-in Configurations

```bash
# High performance configuration
./nlm benchmark --suite 5 --compare \
  "neuron_count=5000,region_count=4,threads=8" \
  "neuron_count=10000,region_count=8,threads=16"

# Research configuration
./nlm benchmark --suite 3 --iterations 20 \
  --config "neuron_count=2000,enable_memory=true"

# Production configuration
./nlm benchmark --suite 4 --compare \
  "neuron_count=10000,memory_limit=2048" \
  "neuron_count=20000,memory_limit=4096"
```

## Profiling Options

### Profiler Usage

```bash
./nlm demo --profile --verbose
```

The profiler will:
- Time neural computation steps
- Measure memory usage
- Track throughput metrics
- Output detailed timing information

### Advanced Profiling

```bash
./nlm experiment phase6 --profile --detailed-timing
./nlm visualize --metrics --sampling-interval 1000
```

## Performance Metrics

NLM measures several key performance metrics:

### Core Metrics

- **Neuron Speed**: Neurons processed per millisecond
- **Memory Efficiency**: Memory usage per neuron
- **Plasticity Rate**: Synaptic changes per second
- **Prediction Accuracy**: Prediction system confidence

### Scalability Metrics

- **Network Scalability**: Performance increase with neuron count
- **Memory Scalability**: Memory usage patterns at scale
- **Parallel Efficiency**: Speedup from multi-threading
- **Resource Optimization**: CPU/GPU utilization

### Quality Metrics

- **Learning Speed**: Time to achieve target performance
- **Memory Retention**: Duration of stored information
- **Adaptation Rate**: Response to changing environments
- **Cognitive Emergence**: Development of complex behaviors

## Output Formats

### JSON Format

```json
{
  "benchmark_suite": 5,
  "total_benchmarks": 25,
  "average_duration_ms": 1500,
  "average_neurons": 5000,
  "performance_metrics": {
    "neuron_speed": 3.33,
    "memory_efficiency": 0.002,
    "plasticity_rate": 0.15,
    "prediction_accuracy": 0.87
  },
  "scaling_analysis": {
    "neuron_scaling": 0.85,
    "memory_scaling": 0.92,
    "parallel_efficiency": 0.78
  },
  "recommendations": [
    "Consider increasing region_count for better parallelism",
    "Optimize connection_probability for target performance",
    "Enable memory pooling for large-scale simulations"
  ]
}
```

### CSV Format

```csv
name,suite,duration_ms,neurons,synapses,throughput,memory_timestamp
test_suite_1,1,1200,1000,10000,0.83,2.1,2024-01-15T10:30:00Z
test_suite_2,2,1800,2000,20000,1.11,3.8,2024-01-15T10:35:00Z
test_suite_3,3,2500,5000,50000,2.0,7.2,2024-01-15T10:40:00Z
```

### TABLE Format

| Benchmark | Suite | Duration (ms) | Neurons | Synapses | Throughput | Memory (MB) | Status |
|-----------|-------|---------------|---------|----------|-------------|-------------|---------|
| test_1    | 1     | 1200          | 1000    | 10000    | 0.83        | 2.1         | PASS    |
| test_2    | 2     | 1800          | 2000    | 20000    | 1.11        | 3.8         | PASS    |
| test_3    | 3     | 2500          | 5000    | 50000    | 2.0         | 7.2         | PASS    |

## Exporting Results

### File Export

```bash
./nlm benchmark --output JSON --export-all --directory results/
./nlm benchmark --output CSV --compare config1,config2 --file comparison.csv
```

### Comparison Analysis

```bash
# Compare multiple configurations
./nlm benchmark --compare \
  "basic_config.json" \
  "optimized_config.json" \
  "research_config.json" \
  --output JSON \
  --file analysis.json

# Generate performance report
./nlm benchmark --suite 5 --generate-report \
  --format HTML --file report.html
```

## Integration with Development Tools

### Profiling Integration

```bash
# Profile with custom metrics
./nlm demo --profile --metrics neurons,duration,memory

# Enable detailed timing for debugging
./nlm experiment phase6 --profile --detailed-timing --debug
```

### Automated Testing

```bash
#!/bin/bash
# Run automated performance tests
./nlm benchmark --suite 5 --iterations 10 > performance_results.json

# Check for performance regressions
if [ $(jq '.average_duration_ms' performance_results.json) -gt 2000 ]; then
    echo "Performance regression detected!"
    exit 1
fi

echo "Performance tests passed"
exit 0
```

## Performance Optimization Recommendations

Based on benchmark results, NLM provides optimization recommendations:

1. **For Large Networks (10k+ neurons)**:
   - Enable memory pooling
   - Increase region count for parallelism
   - Optimize connection probability

2. **For Real-time Applications**:
   - Use smaller network sizes
   - Enable detailed timing
   - Monitor throughput metrics

3. **For Research and Development**:
   - Use research presets
   - Enable comprehensive logging
   - Track learning curves

4. **For Production Systems**:
   - Use production presets
   - Enable checkpointing
   - Monitor resource usage

## Troubleshooting

### Common Issues

**Issue: Benchmark runs slowly**
Solution: Check system resources and enable parallel processing.

**Issue: Memory usage high**
Solution: Use memory profiling to identify leaks.

**Issue: Results inconsistent**
Solution: Use the same random seed for reproducible results.

### Debugging

```bash
# Enable debug logging
./nlm benchmark --verbose --debug

# Run with single thread for debugging
./nlm benchmark --suite 1 --parallel false

# Check system resources
free -h
# monitor memory usage during benchmark
```

## Future Enhancements

NLM benchmarking will continue to evolve with:

1. **Real-time monitoring**: Live visualization of benchmark results
2. **Cloud benchmarking**: Distributed benchmark execution
3. **AI-powered optimization**: Machine learning for parameter tuning
4. **Hardware acceleration**: GPU benchmarking support
5. **Standardized metrics**: Universal benchmarking standards

## Resources

- **Documentation**: https://nlm.readthedocs.io/benchmarking
- **Examples**: https://github.com/nlm-project/nlm/tree/main/examples/benchmark
- **Reference**: https://nlm.readthedocs.io/reference/profiling

---

*This benchmarking guide covers NLM Phase 6: Final Integration. For benchmarking documentation specific to other phases, please refer to the respective phase documentation.*

**For support and questions:**
- GitHub: https://github.com/nlm-project/nlm/issues
- Documentation: https://nlm.readthedocs.io
- Community: https://community.nlm.ai

NLM Benchmarking v1.0 - Built with precision, performance, and scalability in mind.
