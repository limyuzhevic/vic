# NLM Performance Profiler Examples
# This document provides comprehensive examples for using the NLM performance profiling system

## Overview

The NLM performance profiling system provides comprehensive performance measurement and analysis capabilities for the NLM framework. This document covers:

1. Basic profiling setup and usage
2. Advanced profiling techniques
3. Integration with NLM systems
4. Configuration and customization
5. Performance analysis and reporting

## Table of Contents

1. [Quick Start](#quick-start)
2. [Basic Profiling](#basic-profiling)
3. [Advanced Profiling](#advanced-profiling)
4. [Integration Examples](#integration-examples)
5. [Configuration Examples](#configuration-examples)
6. [Performance Analysis](#performance-analysis)
7. [Troubleshooting](#troubleshooting)

## Quick Start

### Basic Setup

The fastest way to get started with performance profiling is to:

1. Enable profiling in CMake:

```cmake
# Enable performance profiling
set(NLM_PERFORMANCE_ENABLED ON CACHE BOOL "Enable performance optimizations")
set(NLM_PROFILING_ENABLED ON CACHE BOOL "Enable performance profiling")
```

2. Initialize the profiler in your main application:

```cpp
#include "nlm/performance/ProfilerConfigManager.hpp"

int main() {
    // Initialize with default configuration
    nlm::profiling::initializeProfiler("configs/performance.cfg");
    
    // Start profiling
    nlm::profiling::startProfiler();
    
    // Your application code here
    // ...
    
    // Stop and generate reports
    nlm::profiling::stopProfiler();
    nlm::profiling::generateProfilerReport("performance_report.json");
    
    return 0;
}
```

3. Build and run:

```bash
cmake -B build -DNLM_PERFORMANCE_ENABLED=ON -DNLM_PROFILING_ENABLED=ON
cd build
make nlm
./nlm --config configs/performance.cfg
```

## Basic Profiling

### Manual Timing

Use the `PROFILE_SCOPE` macro for manual timing of specific operations:

```cpp
// Profile a computation function
void computeNeuralDynamics() {
    PROFILE_SCOPE("compute_neurons", "brain")
    
    // Your neural computation here
    for (int i = 0; i < 1000; ++i) {
        // Neural computation
        double result = sin(i * 0.1) * cos(i * 0.05);
    }
}
```

### Function-Level Profiling

Use `PROFILE_FUNCTION()` to profile entire functions:

```cpp
// Profile a function
void processSpikes() {
    PROFILE_FUNCTION()
    
    // Your spike processing code
    // ...
}
```

### Nested Timing

Use `PROFILE_NESTED` for hierarchical timing:

```cpp
void computeLayer(const std::vector<float>& input) {
    PROFILE_NESTED({"preprocess", "compute", "postprocess"}, "neural_network")
    
    // Preprocessing
    // ...
    
    // Core computation
    // ...
    
    // Postprocessing
    // ...
}
```

### Integration with NLM Systems

#### Brain System Profiling

```cpp
void Brain::step(SimulationStep step, Timestamp time) {
    PROFILE_BRAIN_PERFORMANCE(step)
    
    // Your step implementation
    // ...
    
    // Profile specific regions
    for (auto& region : regions_) {
        PROFILE_SCOPE("process_region", "brain")
        region->updateNeuronStates();
    }
}
```

#### Motor System Profiling

```cpp
void MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    PROFILE_MOTOR_PERFORMANCE(select_action)
    
    // Your action selection logic
    // ...
}
```

#### Sensory System Profiling

```cpp
void SensoryInput::process() {
    PROFILE_SENORY_PERFORMANCE(process_input)
    
    // Your sensory processing
    // ...
}
```

## Advanced Profiling

### Memory Profiling

Enable detailed memory tracking in your configuration:

```cpp
// In configs/debug_performance.cfg
performance.enable_memory_tracking = true
performance.memory_threshold_mb = 512.0
```

Access memory profiler data:

```cpp
auto& profiler = nlm::profiling::GlobalProfiler::instance().getProfiler();
auto& memoryProfiler = profiler.getMemoryProfiler();

for (const auto& sample : memoryProfiler.getSamples()) {
    std::cout << "Time: " << sample.timestamp << "ns, Memory: " 
              << sample.memoryUsage << " bytes, Context: " << sample.context << std::endl;
}
```

### CPU Profiling

Enable CPU tracking for performance analysis:

```cpp
// In configs/performance.cfg
performance.enable_cpu_tracking = true
performance.cpu_threshold_percent = 80.0
```

Analyze CPU usage:

```cpp
auto& cpuProfiler = profiler.getCPUProfiler();

for (const auto& sample : cpuProfiler.getSamples()) {
    double cpuUsagePercent = 100.0 * sample.cpuUsage / 1000000000; // Assuming 1GHz units
    std::cout << "Time: " << sample.timestamp << "ns, CPU Usage: " << cpuUsagePercent << "%" << std::endl;
}
```

### Thread-Specific Profiling

Enable thread-local profiling for detailed thread analysis:

```cpp
// In configs/production_performance.cfg
performance.thread_local_profiling = true
performance.global_aggregation = true
performance.max_threads = 0  // Auto-detect
```

Access thread-specific data:

```cpp
// Add thread-specific profiler
nlm::profiling::GlobalProfiler::instance().addThreadProfiler("worker_1");

// Get thread-specific profiler
auto& threadProfiler = nlm::profiling::GlobalProfiler::instance().getThreadProfiler(1);
```

### Performance Metrics Collection

Collect custom performance metrics:

```cpp
void collectCustomMetrics() {
    PROFILE_SCOPE("collect_metrics", "performance")
    
    // Your custom metric collection
    uint64_t memoryUsage = getCurrentMemoryUsage();
    uint64_t cpuUsage = getCurrentCPUUsage();
    
    nlm::profiling::GlobalProfiler::instance().getProfiler()
        .getPerformanceMetrics().record("custom_metric", 123456, "CUSTOM", "METRICS");
}
```

## Integration Examples

### Complete Application Integration

This example shows how to integrate performance profiling into a complete NLM application:

```cpp
#include "nlm/performance/ProfilerConfigManager.hpp"
#include "nlm/performance/NLMPerformanceProfiler.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"

class NLMApplication {
public:
    NLMApplication() {
        // Initialize profiler
        nlm::profiling::initializeProfiler("configs/performance.cfg");
        
        // Start profiling
        nlm::profiling::startProfiler();
        
        // Create brain
        PROFILE_SCOPE("brain_initialization", "application")
        brain_ = std::make_shared<Brain>(config_);
        
        // Run simulation loop
        runSimulation();
    }
    
private:
    void runSimulation() {
        PROFILE_SCOPE("simulation_loop", "application")
        
        for (SimulationStep step = 0; step < totalSteps_; ++step) {
            PROFILE_SCOPE("simulation_step", "simulation")
            
            // Process sensory input
            PROFILE_SENORY_PERFORMANCE(process_input)
            processSensorInput();
            
            // Update brain
            PROFILE_BRAIN_PERFORMANCE(neuromodulation)
            brain_->applyNeuromodulation(neuromodulator_);
            
            brain_->step(step, step * timestep_);
            
            // Generate output
            PROFILE_MOTOR_PERFORMANCE(action_selection)
            generateOutput();
            
            // Update profiler
            nlm::profiling::updateProfiler();
        }
    }
    
    void processSensorInput() {
        PROFILE_SENORY_PERFORMANCE(vision_processing)
        
        // Vision processing
        // ...
        
        PROFILE_SENORY_PERFORMANCE(audio_processing)
        // Audio processing
        // ...
    }
    
    void generateOutput() {
        PROFILE_MOTOR_PERFORMANCE(plan_action)
        
        // Action planning
        // ...
        
        PROFILE_MOTOR_PERFORMANCE(execute_action)
        // Action execution
        // ...
    }
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<Neuromodulator> neuromodulator_;
    SimulationStep totalSteps_ = 1000;
    double timestep_ = 0.001;
};
```

### Benchmarking and Performance Testing

Use the profiler for systematic performance testing:

```cpp
class PerformanceBenchmark {
public:
    PerformanceBenchmark() {
        nlm::profiling::initializeProfiler("configs/benchmark.cfg");
        nlm::profiling::startProfiler();
    }
    
    void runBenchmarkSuite() {
        // Baseline performance
        measureBaselinePerformance();
        
        // Stress test
        measureUnderLoad();
        
        // Scalability test
        measureScalability();
        
        // Stop and generate report
        nlm::profiling::stopProfiler();
        generateBenchmarkReport();
    }
    
private:
    void measureBaselinePerformance() {
        PROFILE_SCOPE("baseline_test", "benchmark")
        
        // Run application under normal load
        // ...
    }
    
    void measureUnderLoad() {
        PROFILE_SCOPE("stress_test", "benchmark")
        
        // Run application under heavy load
        // ...
    }
    
    void measureScalability() {
        PROFILE_SCOPE("scalability_test", "benchmark")
        
        // Run application with varying thread counts
        for (int threads = 1; threads <= 8; ++threads) {
            // Configure thread count
            // ...
            
            // Run benchmark
            // ...
        }
    }
    
    void generateBenchmarkReport() {
        nlm::profiling::generateProfilerReport("benchmark_results.json");
        
        // Analyze results
        analyzeBenchmarkResults();
    }
    
    void analyzeBenchmarkResults() {
        auto& profiler = nlm::profiling::GlobalProfiler::instance().getProfiler();
        const auto& metrics = profiler.getPerformanceMetrics();
        
        // Calculate performance statistics
        // ...
        
        // Generate comparison charts
        // ...
    }
};
```

## Configuration Examples

### Development Configuration (configs/debug_performance.cfg)

```yaml
# Debug mode - maximum profiling detail
profiling:
  enabled: true
  sample_interval_ms: 10
  report_interval_s: 30
  
thresholds:
  memory_threshold_mb: 512
  cpu_threshold_percent: 70
  operation_threshold_ns: 100000
  spike_rate_threshold_per_s: 50
  
tracking:
  enable_memory_tracking: true
  enable_cpu_tracking: true
  enable_io_tracking: true
  enable_network_tracking: true
  
analysis:
  detect_bottlenecks: true
  detect_regressions: true
  comparative_profiling: true
  
output:
  output_text_report: true
  output_csv_report: true
  output_json_report: true
  console_output: true
  real_time_monitoring: true
```

### Production Configuration (configs/production_performance.cfg)

```yaml
# Production mode - optimized for minimal overhead
profiling:
  enabled: true
  sample_interval_ms: 1000
  report_interval_s: 300
  
thresholds:
  memory_threshold_mb: 2048
  cpu_threshold_percent: 90
  operation_threshold_ns: 10000000
  spike_rate_threshold_per_s: 500
  
monitoring:
  enable_memory_tracking: true
  enable_cpu_tracking: true
  enable_io_tracking: false
  enable_network_tracking: false
  
analysis:
  detect_bottlenecks: true
  detect_regressions: true
  comparative_profiling: false
  
output:
  output_text_report: true
  output_csv_report: false
  output_json_report: true
  console_output: false
  real_time_monitoring: false
```

### Testing Configuration (configs/test_performance.cfg)

```yaml
# Testing configuration - balanced for comprehensive testing
profiling:
  enabled: true
  sample_interval_ms: 100
  report_interval_s: 60
  
thresholds:
  memory_threshold_mb: 1024
  cpu_threshold_percent: 80
  operation_threshold_ns: 1000000
  spike_rate_threshold_per_s: 100
  
integration:
  profile_brain: true
  profile_motor: true
  profile_sensor: true
  profile_neuromodulation: true
  profile_plasticity: true
  
output:
  output_text_report: true
  output_csv_report: true
  output_json_report: true
  console_output: true
  real_time_monitoring: true
```

## Performance Analysis

### Profile Report Analysis

The profiler generates reports in multiple formats. Here's how to analyze them:

#### JSON Reports

JSON reports contain structured data with all performance metrics:

```cpp
#include <nlohmann/json.hpp>

void analyzeJSONReport(const std::string& filename) {
    std::ifstream file(filename);
    nlohmann::json report = nlohmann::json::parse(file);
    
    // Extract statistics
    double totalTime = report["total_time_ns"];
    double avgOperationTime = report["average_operation_time_ns"];
    size_t operationCount = report["operation_count"];
    
    // Find bottlenecks
    for (const auto& operation : report["operations"]) {
        double duration = operation["duration_ns"];
        double stddev = operation["stddev_ns"];
        
        if (duration > 1000000) {  // 1ms threshold
            std::cout << "Bottleneck detected: " << operation["name"] 
                      << " takes " << duration << " ns" << std::endl;
        }
    }
}
```

#### CSV Reports

CSV reports are ideal for importing into spreadsheet software:

```cpp
#include <pandas/core/platform/common.hpp>  // If using pandas in Python

void analyzeCSVReport(const std::string& filename) {
    // Import CSV into pandas DataFrame (Python example)
    // auto df = pandas::read_csv(filename);
    
    // Analyze data
    // auto mean_duration = df["duration_ns"].mean();
    // auto max_duration = df["duration_ns"].max();
    // auto median_duration = df["duration_ns"].median();
    
    // Generate statistics
    // std::cout << "Mean duration: " << mean_duration << " ns" << std::endl;
    // std::cout << "Max duration: " << max_duration << " ns" << std::endl;
    // std::cout << "Median duration: " << median_duration << " ns" << std::endl;
}
```

#### Text Reports

Text reports provide human-readable summaries:

```cpp
void analyzeTextReport(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.find("Bottleneck detected:") != std::string::npos) {
            std::cout << "CRITICAL: " << line << std::endl;
        } else if (line.find("SLOW_OPERATION:") != std::string::npos) {
            std::cout << "WARNING: " << line << std::endl;
        } else {
            std::cout << line << std::endl;
        }
    }
}
```

### Performance Bottleneck Detection

The profiler automatically detects performance bottlenecks:

1. **Slow Operations**: Operations exceeding configured time thresholds
2. **High Variance**: Operations with high standard deviation
3. **Memory Issues**: Memory usage exceeding thresholds
4. **CPU Issues**: CPU usage exceeding thresholds
5. **Thread Contention**: Long-running thread operations

```cpp
void detectPerformanceBottlenecks() {
    nlm::profiling::GlobalProfiler::instance().checkForBottlenecks();
    
    // Check for alerts
    const auto& profiler = nlm::profiling::GlobalProfiler::instance().getProfiler();
    const auto& alerts = profiler.getAlertSystem().getAlerts();
    
    for (const auto& alert : alerts) {
        std::cout << "[ALERT] " << alert.severity << ": " 
                  << alert.name << " - " << alert.message << std::endl;
    }
}
```

### Performance Regression Detection

Compare current performance with baseline:

```cpp
void detectPerformanceRegression(const std::string& baselineFile) {
    // Load baseline results
    nlohmann::json baseline = loadPerformanceData(baselineFile);
    
    // Get current results
    nlohmann::json current = getCurrentPerformanceData();
    
    // Compare operations
    for (const auto& [operation, baselineStats] : baseline["operations"].items()) {
        auto currentStats = current["operations"][operation];
        
        double baselineMean = baselineStats["mean_ns"];
        double currentMean = currentStats["mean_ns"];
        
        // Check for regressions (5% or more degradation)
        double regressionThreshold = 0.05;
        if (currentMean > baselineMean * (1.0 + regressionThreshold)) {
            std::cout << "REGRESSION DETECTED: " << operation 
                      << " degraded from " << baselineMean << " to " << currentMean << " ns" << std::endl;
        }
    }
}
```

## Troubleshooting

### Common Issues

#### 1. Profiler Not Starting

**Symptoms**: No performance data collected

**Causes**:
- `NLM_PROFILING_ENABLED` not defined in build system
- `initializeProfiler()` not called
- Configuration file not found

**Solutions**:
```cmake
# Ensure profiling is enabled in CMake
set(NLM_PERFORMANCE_ENABLED ON CACHE BOOL "Enable performance optimizations")
set(NLM_PROFILING_ENABLED ON CACHE BOOL "Enable performance profiling")
```

```cpp
// Call initializeProfiler() in main()
nlm::profiling::initializeProfiler("configs/performance.cfg");
```

#### 2. High Performance Overhead

**Symptoms**: Profiling significantly slows down application

**Causes**:
- Profiling enabled in production build
- Too frequent sampling
- Detailed memory tracking enabled

**Solutions**:
```cpp
// Use production configuration
nlm::profiling::initializeProfiler("configs/production_performance.cfg");
```

```cpp
// Increase sample interval
ProfilerConfig config;
config.sampleIntervalMs = 1000;  // 1 second
config.enableMemoryTracking = false;
config.enableCPUTracking = true;
```

#### 3. Missing Output Files

**Symptoms**: No report files generated

**Causes**:
- No write permissions to output directory
- Output configuration disabled
- Profiler not stopped

**Solutions**:
```cpp
// Ensure output configuration
ProfilerConfig config;
config.outputTextReport = true;
config.outputCSVReport = true;
config.outputJSONReport = true;
```

```bash
# Check permissions
ls -la /path/to/output/
chmod 755 /path/to/output/
```

#### 4. Thread Safety Issues

**Symptoms**: Crashes or incorrect data when using multiple threads

**Causes**:
- Concurrent access to profiler data
- Missing thread synchronization

**Solutions**:
```cpp
// Use thread-local storage for thread-specific data
performance.thread_local_profiling = true
performance.global_aggregation = true

// Ensure thread-safe access
nlm::profiling::GlobalProfiler::instance().checkForBottlenecks();
```

### Debugging Tips

1. **Enable Console Output**: Set `console_output = true` in configuration

2. **Lower Sampling Frequency**: Increase `sample_interval_ms` to reduce overhead

3. **Check Configuration**: Verify all required settings are correct

4. **Use Breakpoints**: Set breakpoints in profiler initialization for debugging

5. **Monitor Memory Usage**: Watch memory usage when profiling with detailed tracking

## Performance Best Practices

### 1. Use Appropriate Profiling Levels

- **Debug Builds**: Use detailed profiling with frequent sampling
- **Release Builds**: Use minimal profiling with sparse sampling
- **Production**: Use only essential profiling with infrequent sampling

### 2. Configure Thresholds Wisely

```cpp
// Set appropriate thresholds for your application
ProfilerConfig config;
config.operation_threshold_ns = 1000000;  // 1ms threshold
config.memory_threshold_mb = 1024;         // 1GB threshold
config.cpu_threshold_percent = 80;         // 80% threshold
```

### 3. Balance Detail and Performance

```cpp
// Trade-off between detail and overhead
ProfilerConfig config;
config.enableMemoryTracking = true;    // High detail, high overhead
config.enableCPUTracking = true;       // High detail, high overhead
config.enableIOTracking = false;       // Low detail, low overhead
```

### 4. Use ScopedTimers for Hot Paths

```cpp
// For frequently called functions, use scoped timers
void hotFunction() {
    PROFILE_SCOPE("hot_function", "performance")
    
    // Hot path code
    // ...
}
```

### 5. Batch I/O Operations

```cpp
void batchOperation() {
    PROFILE_SCOPE("batch_operation", "performance")
    
    // Collect metrics
    // ...
    
    // Write once instead of frequently
    writePerformanceData();
}
```

## References

- [NLM Performance Documentation](docs/PERFORMANCE.md)
- [Performance Optimization Guide](docs/PERFORMANCE_OPTIMIZATION.md)
- [NLM Configuration Reference](docs/CONFIG_REFERENCE.md)
- [NLM Performance Profiler API Reference](docs/PERFORMANCE_PROFILER_API.md)

## See Also

- [NLM Core Performance Features](src/performance/Performance.hpp)
- [Parallel Processing System](src/performance/ParallelProcessing.hpp)
- [Memory Pool Allocator](src/performance/MemoryPool.hpp)
- [Checkpoint System](src/performance/CheckpointSystem.hpp)

## Version History

### Version 1.0 (Phase 6)
- Initial implementation
- Basic timing and metrics collection
- Integration with NLM systems

### Version 2.0 (Phase 8)
- Comprehensive performance profiling system
- Advanced metrics collection and analysis
- Real-time monitoring and alerts
- Multi-threaded support
- Configuration management

### Version 3.0 (Current)
- Complete performance profiling system
- Full integration with all NLM systems
- Production-ready with extensive configuration options
- Comprehensive documentation and examples
