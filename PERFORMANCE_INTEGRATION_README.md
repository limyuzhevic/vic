# NLM Performance Benchmarking and Monitoring Tools

This directory contains a comprehensive set of performance benchmarking and monitoring tools for the NLM (Neural Learning Machine) artificial brain system. These tools provide:

- **Performance Benchmark Suite**: Comprehensive benchmarking of brain performance at different scales
- **Monitoring Tools**: Real-time performance monitoring and system resource tracking
- **Benchmarking Framework**: Standardized benchmark suites and performance regression testing
- **Monitoring Infrastructure**: Dashboard for real-time monitoring and performance alerting
- **Integration with Existing Systems**: Seamless integration with Phase 6 demo, visualization tools, and Python API

## Overview

The NLM performance integration provides a complete solution for monitoring, benchmarking, and analyzing the performance of artificial brain systems. It includes:

1. **Performance Monitoring System** (`src/monitoring/`)
   - Real-time performance metrics collection
   - System resource monitoring (CPU, memory, threads)
   - Brain state monitoring (neurons, synapses, firing rates)
   - Cognitive performance metrics (working memory, novelty, curiosity)
   - Performance bottleneck detection and alerting

2. **Benchmark Suite** (`src/benchmarking/`)
   - Standardized benchmark configurations for different network scales
   - Multiple performance optimization configurations
   - Memory, CPU, and connectivity benchmarking
   - Stress testing capabilities
   - Comparative analysis across versions

3. **Integration Framework** (`src/integration/`)
   - Integration with existing NLM demos (Phase 2, Phase 6)
   - Integration with experiment infrastructure
   - Integration with visualization tools
   - Integration with agent systems
   - Integration with Python API

4. **Example Executables**
   - `nlm_monitor`: Performance monitoring demonstration
   - `nlm_benchmark`: Comprehensive benchmark suite demonstration
   - `nlm_perf_demo`: Phase 6 integration demonstration

5. **Python Integration Script**
   - `performance_integration_example.py`: Complete integration demonstration script

## Key Features

### Performance Monitoring System
- **Real-time Metrics**: Collects and displays real-time performance metrics
- **System Resources**: Monitors CPU, memory, and thread usage
- **Brain Metrics**: Tracks neuron count, synapse count, firing rates, simulation speed
- **Cognitive Metrics**: Monitors working memory, novelty, curiosity, prediction accuracy
- **Bottleneck Detection**: Identifies performance bottlenecks and provides recommendations
- **Alerting System**: Configurable alerts for performance issues
- **Historical Analysis**: Stores and analyzes historical performance data

### Benchmark Suite
- **Multiple Scales**: Tests brain performance from 100 to 10,000+ neurons
- **Configuration Testing**: Compares different performance optimizations:
  - Memory-optimized configurations
  - Speed-optimized configurations
  - SIMD (Single Instruction, Multiple Data) optimization
  - Multithreading optimizations
- **Comprehensive Analysis**: Provides detailed performance analysis, scaling trends, and quality assessment
- **Regression Testing**: Compares against baseline performance to detect regressions
- **Stress Testing**: Tests system performance under load conditions

### Integration Framework
- **Phase 6 Integration**: Seamlessly integrates with the Phase 6 integration demo
- **Experiment Integration**: Integrates with existing experiment infrastructure
- **Visualization Integration**: Works with existing visualization tools
- **Agent Integration**: Integrates with agent brain systems
- **Python API Integration**: Provides comprehensive Python API for integration

## Getting Started

### Prerequisites
- NLM development environment (CMake, C++ compiler)
- Build the NLM core library
- Build the performance tools

### Building the Performance Tools

1. Configure CMake:
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

2. Build the tools:
```bash
cmake --build build --target all
```

3. Install the tools (optional):
```bash
cmake --install build --prefix /usr/local
```

### Running the Examples

1. **Performance Monitor Demo**:
```bash
./build/nlm_monitor
```

2. **Benchmark Suite Demo**:
```bash
./build/nlm_benchmark
```

3. **Phase 6 Integration Demo**:
```bash
./build/nlm_perf_demo
```

4. **Python Integration Script**:
```bash
python3 performance_integration_example.py [--demo phase6] [--output reports]
```

## Python Integration Script

The `performance_integration_example.py` script provides a comprehensive demonstration of the performance integration tools. It includes:

- **Benchmark Suite Execution**: Runs comprehensive benchmark tests
- **Monitoring Integration**: Demonstrates real-time monitoring capabilities
- **Phase 6 Integration**: Shows integration with the Phase 6 demo
- **Report Generation**: Generates detailed text, HTML, and JSON reports
- **Interactive Examples**: Provides examples of how to use the tools programmatically

### Usage Examples

```python
# Basic integration
from performance_integration_example import PerformanceIntegrationDemo

demo = PerformanceIntegrationDemo("my_reports")
results = demo.run_full_integration_demo()

# Run specific components
benchmark_results = demo.run_benchmark_suite()
monitoring_results = demo.run_monitoring_integration()
phase6_results = demo.run_phase6_integration()

# Generate custom reports
reports = demo.generate_reports()
```

```bash
# Command line interface
python3 performance_integration_example.py --demo phase6 --output my_reports
python3 performance_integration_example.py --demo full
python3 performance_integration_example.py --help
```

## Files Structure

```
/workspace/app/src/
├── monitoring/                    # Performance monitoring system
│   ├── PerformanceMonitor.hpp     # Main monitoring header
│   ├── PerformanceMonitor.cpp     # Monitoring implementation
│   ├── PerformanceMonitorMain.cpp  # Monitoring demo executable
│   └── ...                        # Other monitoring components
│
├── benchmarking/                  # Benchmark suite
│   ├── BenchmarkSuite.hpp        # Benchmark suite header
│   ├── BenchmarkSuite.cpp        # Benchmark implementation
│   └── ...                        # Other benchmark components
│
├── integration/                   # Integration framework
│   ├── PerformanceIntegration.hpp # Integration header
│   ├── PerformanceIntegration.cpp # Integration implementation
│   └── ...                        # Integration components
│
├── experiments/                    # Example executables
│   ├── Phase6Demo.cpp            # Phase 6 integration demo
│   └── ...                        # Other demo executables
│
├── benchmark.cpp                 # Benchmark suite demo executable
└── monitoring/PerformanceMonitorMain.cpp # Monitoring demo executable

/workspace/app/
├── performance_integration_example.py  # Python integration script
├── CMakeLists.txt                    # CMake configuration
├── build/                            # Build directory (optional)
└── performance_reports/               # Example output directory (optional)
```

## Configuration

### Performance Monitor Configuration

The performance monitor can be configured with:

- **Monitoring Interval**: Frequency of metric collection (default: 1 second)
- **Alert Thresholds**: Configuration for performance alerts
- **Output Format**: Choice of text, JSON, or HTML reports
- **Monitoring Scope**: Control which metrics to collect

### Benchmark Suite Configuration

The benchmark suite can be configured with:

- **Neuron Scales**: Network sizes to test (default: 100, 500, 1000, 2000 neurons)
- **Step Counts**: Simulation steps for each scale (default: 1000, 1000, 500, 200 steps)
- **Performance Configurations**: Different optimization strategies
- **Benchmark Phases**: Which benchmark phases to run (basic, scaling, configuration, stress)
- **Output Directory**: Directory for report storage

### Integration Configuration

The integration framework can be configured with:

- **Demo Types**: Which demo to integrate with (Phase 2, Phase 6, full)
- **Integration Depth**: Level of integration (basic, comprehensive)
- **Report Format**: Format of generated reports (text, HTML, JSON)
- **Alerting**: Enable/disable performance alerting

## Output Reports

The performance integration tools generate comprehensive reports in:

### Text Reports
- `integration_summary.txt`: Comprehensive text summary report
- `benchmark_results.txt`: Detailed benchmark results
- `monitoring_results.txt`: Monitoring session results
- `phase6_integration_results.txt`: Phase 6 integration analysis

### HTML Reports
- `integration_report.html`: Interactive HTML report with tables and charts
- `benchmark_analysis.html`: Benchmark analysis with visualizations
- `performance_dashboard.html`: Real-time performance dashboard

### JSON Reports
- `integration_summary.json`: Structured JSON summary
- `benchmark_results.json`: Detailed benchmark results in JSON format
- `monitoring_results.json`: Monitoring data in JSON format

## Example Output

### Text Report Example
```
NLM Performance Integration Summary Report
Generated: 2026-10-16 15:26:02
================================================================================

INTEGRATION STATUS: PASS
INTEGRATION SCORE: 87.3%

BENCHMARK RESULTS:
=================
Total Benchmarks Run: 4
Total Neurons Tested: 3100
Average Performance: 245.6 steps/sec
Benchmark Quality: 0.823
Regression Risk: 0.156

PERFORMANCE ISSUES DETECTED:
=============================
  - Average performance below threshold (100 steps/sec)
  - High memory usage detected

MONITORING RESULTS:
==================
Monitoring Status: Active
Metrics Collected: 1000
Performance Score: 0.824
System Health: 65.2% CPU, 72.1% Memory

ACTIVE ALERTS:
=============
  - High CPU Usage (WARNING): CPU usage is 92.3%
  - Low Performance (ERROR): Simulation speed is 87.2 steps/sec

SYSTEM INTEGRATION RESULTS:
==========================
Integration Status: PASS
Systems Integrated: 6

  - memory_systems: 92.1% score
  - neuromodulation_systems: 88.5% score
  - prediction_system: 91.3% score
  - development_system: 89.7% score
  - checkpointing: 94.2% score
  - replay_system: 86.8% score

PERFORMANCE RECOMMENDATIONS:
===========================
1. Optimize neural processing for improved simulation speed
2. Monitor memory usage for potential bottlenecks
3. Review system configuration for integration issues
4. Implement performance alerting for proactive monitoring
5. Establish regular benchmarking schedules

CONCLUSION:
===========
The NLM performance integration successfully demonstrates comprehensive monitoring
and benchmarking capabilities for the artificial brain system.
```

### JSON Report Example
```json
{
  "integration_summary": {
    "status": "PASS",
    "score": 0.873,
    "issues_count": 2,
    "recommendations_count": 5
  },
  "performance_results": {
    "benchmarks": {
      "total_run": 4,
      "total_neurons": 3100,
      "average_performance": 245.6,
      "quality_score": 0.823,
      "regression_risk": 0.156
    },
    "memory_usage": {
      "average_mb": 512.3,
      "min_mb": 128.0,
      "max_mb": 1024.0
    }
  },
  "monitoring_results": {
    "status": "Active",
    "performance_score": 0.824,
    "active_alerts_count": 2,
    "system_metrics": {
      "cpu_usage_percent": 65.2,
      "memory_usage_percent": 72.1,
      "thread_count": 4
    }
  }
}
```

## Advanced Usage

### Custom Benchmark Configurations

Users can create custom benchmark configurations by modifying the benchmark suite parameters:

```cpp
// Example custom configuration
nlm::BenchmarkConfig customConfig;
customConfig.neuronCounts = {50, 100, 200, 500};
customConfig.stepCounts = {500, 1000, 2000, 5000};

nlm::BenchmarkConfig::Configuration customOpt;
customOpt.name = "custom_memory_opt";
customOpt.enableMemoryPool = true;
customOpt.enableSOAVectorization = true;
customOpt.enableEventQueue = true;
customOpt.enableSparseConnectivity = false;
customOpt.enableMultithreading = false;
customOpt.simdLevel = "SSE";

benchmarkSuite.addConfiguration(customOpt.name, customOpt);
```

### Custom Performance Alerts

Users can define custom performance alerts:

```cpp
// Example custom alert
monitor.getPerformanceAlertSystem()->addAlert(
    "custom_memory_alert",
    2048.0,  // Threshold (MB)
    "WARNING",
    "Memory usage exceeded 2 GB",
    [](const PerformanceMetrics& m) { return m.brainMemoryUsage > 2048.0; }
);
```

### Integration with Custom Experiments

Users can integrate with custom experiments:

```cpp
// Example experiment integration
auto experimentRunner = std::make_shared<ExperimentRunner>();
nlm::IntegrationResult integrationResult = integrator.integrateWithExperimentRunner(experimentRunner);
```

## Troubleshooting

### Common Issues

1. **Build Errors**: Ensure all dependencies are installed and CMake is configured correctly.

2. **Runtime Errors**: Check that the NLM core library is built and available.

3. **Missing Headers**: Ensure the include paths are set correctly.

4. **Performance Issues**: Monitor system resources and adjust configuration as needed.

### Help and Documentation

- **General Help**: `python3 performance_integration_example.py --help`
- **Example Usage**: Check the example scripts in the `examples/` directory
- **API Documentation**: Refer to the header files in `src/monitoring/`, `src/benchmarking/`, and `src/integration/`
- **Bug Reports**: File issues at the NLM GitHub repository

## License

The NLM Performance Benchmarking and Monitoring Tools are part of the NLM project and are licensed under the NLM License. See the `LICENSE` file for more details.

## Acknowledgments

The development of these performance tools was supported by the NLM project and benefited from contributions from the NLM development community.

---

For more information about the NLM project and its performance tools, visit the NLM documentation website or contact the NLM development team.

This comprehensive performance integration system provides users with powerful tools for monitoring, benchmarking, and analyzing the performance of NLM artificial brain systems.
"""
