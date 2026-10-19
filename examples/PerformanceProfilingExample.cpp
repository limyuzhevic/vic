# Performance Profiling System Examples
#
# This document provides comprehensive examples for using the NLM performance profiling system
# implemented as part of Phase 8: Performance Profiling.
#
# The profiling system provides detailed performance measurement and analysis for the NLM framework,
# including timing, memory usage, CPU usage, and bottleneck detection.
#
# To compile and run examples:
# 1. Enable profiling in CMake: set(NLM_PERFORMANCE_ENABLED ON) set(NLM_PROFILING_ENABLED ON)
# 2. Build: make nlm
# 3. Run: ./nlm --config examples/example_config.cfg

## 1. Basic Performance Profiling

### Example 1: Simple Timing and Metrics Collection

This example demonstrates basic profiling capabilities:

```cpp
#include "nlm/performance/NLMPerformanceProfiler.hpp"
#include "nlm/performance/ProfilerConfigManager.hpp"

int main() {
    // Initialize profiler with default configuration
    nlm::profiling::initializeProfiler();
    
    // Start global profiling
    nlm::profiling::startProfiler();
    
    // Simulate some work
    PROFILE_SCOPE("total_work", "main")
    
    for (int i = 0; i < 100; ++i) {
        // Profile individual operations
        PROFILE_SCOPE("computation", "main")
        
        double result = 0.0;
        for (int j = 0; j < 1000; ++j) {
            result += std::sin(j * 0.1) * std::cos(j * 0.05);
        }
        
        // Record operation metrics
        nlm::profiling::recordOperation("computation", 123456, "math", "optimization");
    }
    
    // Stop profiling
    nlm::profiling::stopProfiler();
    
    // Generate reports
    nlm::profiling::generateProfilerReport("performance_report.json");
    
    return 0;
}
```

### Example 2: Configuration-Based Profiling

This example shows how to use configuration files:

```cpp
// File: examples/basic_example.cpp

#include "nlm/performance/ProfilerConfigManager.hpp"
#include "nlm/performance/NLMPerformanceProfiler.hpp"

int main() {
    // Initialize profiler with configuration file
    nlm::profiling::initializeProfiler("configs/example_config.cfg");
    
    // Get current configuration
    const auto& config = nlm::profiling::getProfilerConfig();
    
    // Log configuration
    std::cout << "Profiler Configuration:" << std::endl;
    std::cout << "  Enabled: " << (config.enabled ? "Yes" : "No") << std::endl;
    std::cout << "  Output File: " << config.outputFile << std::endl;
    std::cout << "  Sample Interval: " << config.sampleIntervalMs << "ms" << std::endl;
    
    // Start profiling
    nlm::profiling::startProfiler();
    
    // Your application code here
    // ...
    
    // Stop profiling
    nlm::profiling::stopProfiler();
    
    // Generate reports
    nlm::profiling::generateProfilerReport("performance_results.json");
    
    return 0;
}
```

### Example Configuration File

```yaml
# File: examples/example_config.cfg

# Basic profiling configuration
profiling:
  enabled: true
  output_file: "performance_results.json"
  sample_interval_ms: 100
  report_interval_s: 60

# Performance thresholds
thresholds:
  memory_threshold_mb: 1024
  cpu_threshold_percent: 80
  operation_threshold_ns: 1000000
  spike_rate_threshold_per_s: 100

# Profiling targets
profiling_targets:
  profile_brain: true
  profile_motor: true
  profile_sensor: true
  profile_neuromodulation: true
  profile_plasticity: true

# Output options
output:
  output_text_report: true
  output_csv_report: true
  output_json_report: true
  console_output: true
  real_time_monitoring: true
```

## 2. Advanced Performance Profiling

### Example 3: Real-Time Monitoring

This example demonstrates real-time monitoring capabilities:

```cpp
// File: examples/real_time_example.cpp

#include "nlm/performance/NLMPerformanceProfiler.hpp"
#include "nlm/performance/ProfilerConfigManager.hpp"

class RealTimeMonitor {
public:
    RealTimeMonitor() {
        nlm::profiling::initializeProfiler("configs/real_time_config.cfg");
        nlm::profiling::startProfiler();
    }
    
    void runMonitoringLoop() {
        const auto& profiler = nlm::profiling::GlobalProfiler::instance();
        
        while (monitoringActive_) {
            // Update profiler
            nlm::profiling::updateProfiler();
            
            // Get system statistics
            auto stats = profiler.getSystemStats();
            
            // Check for bottlenecks
            profiler.checkForBottlenecks();
            
            // Display status
            displayStatus(stats);
            
            // Sleep for monitoring interval
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    void displayStatus(const Profiler::SystemPerformanceStats& stats) {
        std::cout << "[" << getTimestamp() << "] Performance Status:" << std::endl;
        std::cout << "  Average Operation Duration: " << stats.avgOperationDuration << " ns" << std::endl;
        std::cout << "  Max Operation Duration: " << stats.maxOperationDuration << " ns" << std::endl;
        std::cout << "  Total Operations: " << stats.totalOperations << std::endl;
        std::cout << "  Memory Usage: " << stats.totalMemoryUsage << " bytes" << std::endl;
        std::cout << "  CPU Usage: " << stats.totalCPUUsage << "%" << std::endl;
    }
    
private:
    std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        return std::string(std::ctime(&time));
    }
    
    bool monitoringActive_ = true;
};
```

### Example Real-Time Configuration

```yaml
# File: examples/real_time_config.cfg

profiling:
  enabled: true
  output_file: "real_time_results.json"
  sample_interval_ms: 50
  report_interval_s: 30

thresholds:
  memory_threshold_mb: 512
  cpu_threshold_percent: 70
  operation_threshold_ns: 500000

monitoring:
  enable_memory_tracking: true
  enable_cpu_tracking: true
  enable_io_tracking: true
  enable_network_tracking: true

analysis:
  detect_bottlenecks: true
  detect_regressions: true

output:
  output_text_report: true
  output_csv_report: true
  output_json_report: true
  console_output: true
  real_time_monitoring: true
```

### Example 4: Memory Profiling

This example focuses on memory profiling:

```cpp
// File: examples/memory_example.cpp

#include "nlm/performance/NLMPerformanceProfiler.hpp"
#include "nlm/performance/ProfilerConfigManager.hpp"

class MemoryProfilerExample {
public:
    MemoryProfilerExample() {
        nlm::profiling::initializeProfiler("configs/memory_config.cfg");
        nlm::profiling::startProfiler();
        
        // Enable memory tracking
        auto& profiler = nlm::profiling::GlobalProfiler::instance().getProfiler();
        profiler.getMemoryProfiler().start();
    }
    
    void runMemoryTest() {
        const auto& profiler = nlm::profiling::GlobalProfiler::instance();
        auto& memoryProfiler = profiler.getMemoryProfiler();
        
        // Allocate and deallocate memory
        for (int i = 0; i < 100; ++i) {
            PROFILE_SCOPE("memory_allocation", "memory_test")
            
            // Allocate memory
            std::vector<float> data(1000000, 1.0f);  // 4MB
            
            // Do some work
            double sum = std::accumulate(data.begin(), data.end(), 0.0f);
            
            // Sample memory usage
            memoryProfiler.sample("allocation_test");
            
            // Deallocate (vector destructor)
        }
        
        // Stop memory profiling
        memoryProfiler.stop();
        
        // Generate memory report
        analyzeMemoryUsage(memoryProfiler);
    }
    
    void analyzeMemoryUsage(const MemoryProfiler& memoryProfiler) {
        const auto& samples = memoryProfiler.getSamples();
        
        std::cout << "Memory Usage Analysis:" << std::endl;
        for (const auto& sample : samples) {
            std::cout << "  Time: " << sample.timestamp << "ns, "
                      << "Memory: " << sample.memoryUsage << " bytes, "
                      << "Context: " << sample.context << std::endl;
        }
        
        // Find peak memory usage
        uint64_t peakMemory = 0;
        for (const auto& sample : samples) {
            if (sample.peakMemory > peakMemory) {
                peakMemory = sample.peakMemory;
            }
        }
        
        std::cout << "  Peak Memory Usage: " << peakMemory << " bytes" << std::endl;
    }
};
```

### Example Memory Configuration

```yaml
# File: examples/memory_config.cfg

profiling:
  enabled: true
  output_file: "memory_results.json"
  sample_interval_ms: 100

thresholds:
  memory_threshold_mb: 256
  cpu_threshold_percent: 50

monitoring:
  enable_memory_tracking: true
  enable_cpu_tracking: false
  enable_io_tracking: false
  enable_network_tracking: false

output:
  output_text_report: true
  output_csv_report: true
  output_json_report: true
  console_output: true
```

## 3. Integration with NLM Systems

### Example 5: Brain System Profiling

This example shows how to integrate profiling with the NLM brain system:

```cpp
// File: examples/brain_example.cpp

#include "nlm/performance/NLMPerformanceProfiler.hpp"
#include "brain/Brain.hpp"

class BrainProfilingExample {
public:
    BrainProfilingExample() {
        nlm::profiling::initializeProfiler("configs/brain_config.cfg");
        nlm::profiling::startProfiler();
        
        // Create brain
        brain_ = std::make_shared<Brain>(config_);
        brain_->initialize();
    }
    
    void runBrainSimulation() {
        PROFILE_SCOPE("brain_simulation", "brain")
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Profile sensory input processing
            PROFILE_SENORY_PERFORMANCE(process_input)
            processSensoryInput();
            
            // Profile brain step
            PROFILE_BRAIN_PERFORMANCE(brain_step)
            brain_->step(step, step * 0.001);
            
            // Profile motor output
            PROFILE_MOTOR_PERFORMANCE(select_action)
            generateAction();
            
            // Update profiler
            nlm::profiling::updateProfiler();
        }
    }
    
private:
    void processSensoryInput() {
        PROFILE_SENORY_PERFORMANCE(vision_processing)
        
        // Vision processing
        // ...
        
        PROFILE_SENORY_PERFORMANCE(audio_processing)
        // Audio processing
        // ...
    }
    
    void generateAction() {
        PROFILE_MOTOR_PERFORMANCE(action_selection)
        
        // Action planning and execution
        // ...
    }
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
};
```

### Example Brain Configuration

```yaml
# File: examples/brain_config.cfg

profiling:
  enabled: true
  output_file: "brain_results.json"
  sample_interval_ms: 100

thresholds:
  memory_threshold_mb: 2048
  cpu_threshold_percent: 80

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
```

### Example 6: Motor System Profiling

This example focuses on motor system profiling:

```cpp
// File: examples/motor_example.cpp

#include "nlm/performance/NLMPerformanceProfiler.hpp"
#include "motor/MotorSystem.hpp"

class MotorProfilingExample {
public:
    MotorProfilingExample() {
        nlm::profiling::initializeProfiler("configs/motor_config.cfg");
        nlm::profiling::startProfiler();
        
        // Create motor system
        motor_ = std::make_shared<MotorSystem>();
    }
    
    void runMotorTest() {
        auto* brain = getBrain();  // Assume brain is available
        
        for (int i = 0; i < 100; ++i) {
            PROFILE_SCOPE("motor_test", "motor")
            
            // Profile action selection
            PROFILE_MOTOR_PERFORMANCE(select_action)
            auto action = motor_->selectAction(*brain->getMotorRegion());
            
            // Profile action execution
            PROFILE_MOTOR_PERFORMANCE(execute_action)
            executeAction(action);
            
            // Record motor operation metrics
            nlm::profiling::recordMotorOperation("action_selection", i * 1000);
        }
    }
    
private:
    void executeAction(const std::unique_ptr<Action>& action) {
        PROFILE_MOTOR_PERFORMANCE(action_execution)
        
        // Action execution
        // ...
    }
    
    std::shared_ptr<MotorSystem> motor_;
};
```

## 4. Performance Analysis Examples

### Example 7: Bottleneck Detection

This example demonstrates bottleneck detection:

```cpp
// File: examples/bottleneck_example.cpp

#include "nlm/performance/NLMPerformanceProfiler.hpp"
#include "nlm/performance/ProfilerConfigManager.hpp"

class BottleneckDetectionExample {
public:
    BottleneckDetectionExample() {
        nlm::profiling::initializeProfiler("configs/bottleneck_config.cfg");
        nlm::profiling::startProfiler();
    }
    
    void runBottleneckDetection() {
        const auto& profiler = nlm::profiling::GlobalProfiler::instance();
        
        // Run workload
        runWorkload();
        
        // Stop profiling
        nlm::profiling::stopProfiler();
        
        // Detect bottlenecks
        detectAndReportBottlenecks(profiler);
    }
    
    void runWorkload() {
        PROFILE_SCOPE("workload", "bottleneck")
        
        for (int i = 0; i < 1000; ++i) {
            // Simulate various operations
            PROFILE_SCOPE("cpu_intensive", "bottleneck")
            cpuIntensiveOperation(i);
            
            PROFILE_SCOPE("memory_intensive", "bottleneck")
            memoryIntensiveOperation(i);
            
            PROFILE_SCOPE("io_intensive", "bottleneck")
            ioIntensiveOperation(i);
            
            nlm::profiling::updateProfiler();
        }
    }
    
    void detectAndReportBottlenecks(const nlm::profiling::GlobalProfiler& profiler) {
        profiler.checkForBottlenecks();
        
        const auto& alerts = profiler.getProfiler().getAlertSystem().getAlerts();
        
        std::cout << "=== Bottleneck Detection Report ===" << std::endl;
        std::cout << "Total Alerts: " << alerts.size() << std::endl;
        
        for (const auto& alert : alerts) {
            std::cout << "[" << alert.severity << "] " << alert.name << ": "
                      << alert.message << " (Threshold: " << alert.threshold << ")" << std::endl;
        }
        
        if (alerts.empty()) {
            std::cout << "No bottlenecks detected." << std::endl;
        }
    }
    
private:
    void cpuIntensiveOperation(int i) {
        // CPU-intensive computation
        double result = 0.0;
        for (int j = 0; j < 1000000; ++j) {
            result += std::sin(j * 0.1) * std::cos(j * 0.05);
        }
    }
    
    void memoryIntensiveOperation(int i) {
        // Memory-intensive operations
        std::vector<double> data(1000000, 1.0);
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
    }
    
    void ioIntensiveOperation(int i) {
        // I/O-intensive operations
        std::string data(1000000, 'A');
        // Write to string stream (simulated I/O)
        std::stringstream ss;
        ss << data;
    }
};
```

### Example Bottleneck Configuration

```yaml
# File: examples/bottleneck_config.cfg

profiling:
  enabled: true
  output_file: "bottleneck_results.json"
  sample_interval_ms: 100

thresholds:
  memory_threshold_mb: 512
  cpu_threshold_percent: 70
  operation_threshold_ns: 100000

analysis:
  detect_bottlenecks: true
  detect_regressions: true

output:
  output_text_report: true
  output_csv_report: true
  output_json_report: true
  console_output: true
```

### Example 8: Performance Regression Detection

This example shows how to detect performance regressions:

```cpp
// File: examples/regression_example.cpp

#include "nlm/performance/NLMPerformanceProfiler.hpp"
#include "nlm/performance/ProfilerConfigManager.hpp"
#include <nlohmann/json.hpp>

class PerformanceRegressionExample {
public:
    PerformanceRegressionExample() {
        nlm::profiling::initializeProfiler("configs/regression_config.cfg");
        nlm::profiling::startProfiler();
    }
    
    void runRegressionTest() {
        // Run current workload
        runCurrentWorkload();
        
        // Stop profiling
        nlm::profiling::stopProfiler();
        
        // Generate current results
        generateCurrentResults();
        
        // Load baseline results
        nlohmann::json baseline = loadBaselineResults();
        
        // Compare and detect regressions
        compareWithBaseline(baseline);
    }
    
    void runCurrentWorkload() {
        PROFILE_SCOPE("workload", "regression")
        
        for (int i = 0; i < 100; ++i) {
            PROFILE_SCOPE("operation", "regression")
            
            // Simulate work
            double result = computeSomething(i);
            
            // Record metrics
            nlm::profiling::recordOperation("computation", getTimestampNs(), "computation", "regression");
        }
    }
    
    void generateCurrentResults() {
        nlm::profiling::generateProfilerReport("current_results.json");
    }
    
    nlohmann::json loadBaselineResults() {
        std::ifstream file("baseline_results.json");
        return nlohmann::json::parse(file);
    }
    
    void compareWithBaseline(const nlohmann::json& baseline) {
        // Load current results
        nlohmann::json current = nlohmann::json::parse(std::ifstream("current_results.json"));
        
        std::cout << "=== Performance Regression Analysis ===" << std::endl;
        
        // Compare operations
        for (const auto& [operation, baselineStats] : baseline["operations"].items()) {
            auto currentStats = current["operations"][operation];
            
            double baselineMean = baselineStats["mean_ns"];
            double currentMean = currentStats["mean_ns"];
            
            // Check for regressions (5% threshold)
            double regressionThreshold = 0.05;
            double improvement = (currentMean - baselineMean) / baselineMean;
            
            if (improvement > regressionThreshold) {
                std::cout << "REGRESSION DETECTED: " << operation 
                          << " degraded by " << (improvement * 100) << "%" << std::endl;
                std::cout << "  Baseline: " << baselineMean << " ns, Current: " << currentMean << " ns" << std::endl;
            } else if (improvement < -regressionThreshold) {
                std::cout << "IMPROVEMENT DETECTED: " << operation 
                          << " improved by " << (-improvement * 100) << "%" << std::endl;
                std::cout << "  Baseline: " << baselineMean << " ns, Current: " << currentMean << " ns" << std::endl;
            } else {
                std::cout << "STABLE: " << operation << " (change: " << (improvement * 100) << "%)" << std::endl;
            }
        }
    }
    
private:
    double computeSomething(int i) {
        double result = 0.0;
        for (int j = 0; j < 100000; ++j) {
            result += std::sin(j * 0.1) * std::cos(j * 0.05);
        }
        return result;
    }
    
    uint64_t getTimestampNs() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
};
```

### Example Regression Configuration

```yaml
# File: examples/regression_config.cfg

profiling:
  enabled: true
  output_file: "regression_results.json"
  sample_interval_ms: 100
  comparative_profiling: true
  baseline_file: "baseline_results.json"

thresholds:
  memory_threshold_mb: 1024
  cpu_threshold_percent: 80

analysis:
  detect_regressions: true
  comparative_profiling: true

output:
  output_text_report: true
  output_csv_report: true
  output_json_report: true
  console_output: true
```

## 5. Advanced Profiling Techniques

### Example 9: Thread-Specific Profiling

This example demonstrates thread-specific profiling:

```cpp
// File: examples/thread_example.cpp

#include "nlm/performance/NLMPerformanceProfiler.hpp"
#include "nlm/performance/ProfilerConfigManager.hpp"
#include <thread>
#include <vector>

class ThreadProfilingExample {
public:
    ThreadProfilingExample() {
        nlm::profiling::initializeProfiler("configs/thread_config.cfg");
        nlm::profiling::startProfiler();
        
        // Enable thread-local profiling
        nlm::profiling::GlobalProfiler::instance().addThreadProfiler("main");
    }
    
    void runMultiThreadedTest() {
        // Create multiple threads
        std::vector<std::thread> threads;
        for (int i = 0; i < 4; ++i) {
            threads.emplace_back(&ThreadProfilingExample::workerThread, this, i);
        }
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Generate thread-specific report
        generateThreadReport();
    }
    
    void workerThread(int threadId) {
        // Add thread-specific profiler
        nlm::profiling::GlobalProfiler::instance().addThreadProfiler("worker_" + std::to_string(threadId));
        
        PROFILE_SCOPE("thread_work", "thread" + std::to_string(threadId))
        
        for (int i = 0; i < 100; ++i) {
            // Simulate thread work
            double result = computeThreadWork(i);
            
            // Record thread-specific metrics
            nlm::profiling::GlobalProfiler::instance().
                getThreadProfiler(threadId).
                recordOperation("thread_work", getTimestampNs(), "thread", "worker_" + std::to_string(threadId));
        }
    }
    
    void generateThreadReport() {
        const auto& profiler = nlm::profiling::GlobalProfiler::instance();
        
        std::cout << "=== Thread-Specific Profiling Report ===" << std::endl;
        
        // Generate report for each thread
        for (size_t threadId = 0; threadId < 5; ++threadId) {
            auto& threadProfiler = profiler.getThreadProfiler(threadId);
            
            std::cout << "Thread " << threadId << ": " << threadProfiler.getPerformanceMetrics().getSnapshotCount() 
                      << " operations recorded" << std::endl;
        }
    }
    
private:
    double computeThreadWork(int i) {
        double result = 0.0;
        for (int j = 0; j < 100000; ++j) {
            result += std::sin(j * 0.1) * std::cos(j * 0.05);
        }
        return result;
    }
    
    uint64_t getTimestampNs() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    std::shared_ptr<MotorSystem> motor_;
};
```

### Example Thread Configuration

```yaml
# File: examples/thread_config.cfg

profiling:
  enabled: true
  output_file: "thread_results.json"
  sample_interval_ms: 100

multi_threading:
  thread_local_profiling: true
  global_aggregation: true
  max_threads: 8

thresholds:
  memory_threshold_mb: 2048
  cpu_threshold_percent: 80

output:
  output_text_report: true
  output_csv_report: true
  output_json_report: true
  console_output: true
```

## 6. Configuration Management Examples

### Example 10: Configuration File Examples

This example shows different configuration file templates:

#### Example: Debug Configuration

```ini
# File: configs/debug_performance.cfg

[profiling]
enabled = true
output_file = debug_performance_results.json
sample_interval_ms = 10
report_interval_s = 30

[thresholds]
memory_threshold_mb = 512
cpu_threshold_percent = 70
operation_threshold_ns = 100000

[monitoring]
enable_memory_tracking = true
enable_cpu_tracking = true
enable_io_tracking = true
enable_network_tracking = true

[analysis]
detect_bottlenecks = true
detect_regressions = true

[output]
output_text_report = true
output_csv_report = true
output_json_report = true
console_output = true
real_time_monitoring = true
```

#### Example: Production Configuration

```ini
# File: configs/production_performance.cfg

[profiling]
enabled = true
output_file = performance_results.json
sample_interval_ms = 1000
report_interval_s = 300

[thresholds]
memory_threshold_mb = 2048
cpu_threshold_percent = 90
operation_threshold_ns = 10000000

[monitoring]
enable_memory_tracking = true
enable_cpu_tracking = true
enable_io_tracking = false
enable_network_tracking = false

[analysis]
detect_bottlenecks = true
detect_regressions = true

[output]
output_text_report = true
output_csv_report = false
output_json_report = true
console_output = false
real_time_monitoring = false
```

#### Example: Testing Configuration

```ini
# File: configs/test_performance.cfg

[profiling]
enabled = true
output_file = test_performance_results.json
sample_interval_ms = 100
report_interval_s = 60

[thresholds]
memory_threshold_mb = 1024
cpu_threshold_percent = 80
operation_threshold_ns = 1000000

[integration]
profile_brain = true
profile_motor = true
profile_sensor = true
profile_neuromodulation = true
profile_plasticity = true

[output]
output_text_report = true
output_csv_report = true
output_json_report = true
console_output = true
real_time_monitoring = true
```

## 7. Integration with Build System

### Example 11: CMake Integration

This example shows how to integrate profiling with the build system:

```cmake
# File: CMakeLists.txt

# Enable profiling
if (NLM_PERFORMANCE_ENABLED)
    set(NLM_PROFILING_ENABLED ON CACHE BOOL "Enable performance profiling")
    
    # Add performance profiling library
    add_library(nlm_performance
        src/performance/NLMPerformanceProfiler.cpp
    )
    
    target_include_directories(nlm_performance PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/performance
    )
    
    target_link_libraries(nlm_performance PUBLIC Threads::Threads)
    
    # Link to main executable
    target_link_libraries(nlm PUBLIC nlm_performance)
endif()
```

### Example 12: Build Configuration

```bash
# Enable profiling during build
cmake -DNLM_PERFORMANCE_ENABLED=ON -DNLM_PROFILING_ENABLED=ON -DCMAKE_BUILD_TYPE=Debug

# Build with profiling
make -j4

# Run with specific configuration
./nlm --config configs/debug_performance.cfg

# Generate reports
./nlm --generate_reports performance_results.json
```

## 8. Command-Line Interface Examples

### Example 13: Command-Line Options

```bash
# Run with default configuration
./nlm

# Run with specific configuration file
./nlm --config my_config.cfg

# Generate reports for specific time range
./nlm --generate_reports performance_results.json --start-time 2024-01-01T00:00:00 --end-time 2024-01-02T00:00:00

# Enable verbose output
./nlm --verbose

# Set output directory
./nlm --output-dir /path/to/output/
```

### Example 14: Shell Script for Performance Testing

```bash
#!/bin/bash

# File: scripts/run_performance_test.sh

set -e

echo "Running Performance Test Suite..."

# Enable profiling
export NLM_PERFORMANCE_ENABLED=1
export NLM_PROFILING_ENABLED=1

# Run test with profiling
./nlm --config configs/test_performance.cfg

# Generate reports
./nlm --generate_reports performance_test_results.json

# Analyze results
python3 analyze_performance.py performance_test_results.json

# Compare with baseline
if [ -f baseline_results.json ]; then
    python3 compare_performance.py performance_test_results.json baseline_results.json
fi

echo "Performance test completed. Results saved to performance_test_results.json"
```

## 9. Python Integration Examples

### Example 15: Python Performance Analysis

```python
# File: python/analyze_performance.py

import json
import pandas as pd
import matplotlib.pyplot as plt

def analyze_performance_results(filename):
    # Load performance data
    with open(filename, 'r') as f:
        data = json.load(f)
    
    # Convert to DataFrame
    df = pd.json_normalize(data['operations'])
    
    # Calculate statistics
    mean_time = df['mean_ns'].mean()
    median_time = df['mean_ns'].median()
    max_time = df['mean_ns'].max()
    stddev = df['mean_ns'].std()
    
    print("Performance Analysis Results:")
    print(f"  Mean Operation Time: {mean_time:.2f} ns")
    print(f"  Median Operation Time: {median_time:.2f} ns")
    print(f"  Max Operation Time: {max_time:.2f} ns")
    print(f"  Standard Deviation: {stddev:.2f} ns")
    
    # Identify bottlenecks
    slow_operations = df[df['mean_ns'] > 1000000]  # Operations > 1ms
    print(f"\nSlow Operations (>1ms): {len(slow_operations)}")
    for _, op in slow_operations.iterrows():
        print(f"  - {op['name']}: {op['mean_ns']:.2f} ns")
    
    # Generate visualization
    plt.figure(figsize=(12, 8))
    
    # Plot operation times
    plt.subplot(2, 2, 1)
    df.plot(x='name', y='mean_ns', kind='bar', ax=plt.gca())
    plt.title('Operation Times')
    plt.xticks(rotation=45)
    plt.ylabel('Time (ns)')
    
    # Plot performance distribution
    plt.subplot(2, 2, 2)
    plt.hist(df['mean_ns'], bins=50, alpha=0.7)
    plt.title('Operation Time Distribution')
    plt.xlabel('Time (ns)')
    plt.ylabel('Count')
    
    # Save plots
    plt.tight_layout()
    plt.savefig(f"{filename}_plots.png")
    
    return {
        'mean_time': mean_time,
        'median_time': median_time,
        'max_time': max_time,
        'stddev': stddev,
        'slow_operations_count': len(slow_operations)
    }

if __name__ == "__main__":
    if len(sys.argv) > 1:
        analyze_performance_results(sys.argv[1])
    else:
        print("Usage: python analyze_performance.py <results_file.json>")
```

## 10. Documentation Examples

### Example 16: API Documentation Examples

```cpp
// File: src/performance/NLMPerformanceProfiler.hpp

/**
 * 
 * # Example: Basic Timing
 * 
 * This example demonstrates basic timing functionality:
 * 
 * @code
 * #include "nlm/performance/NLMPerformanceProfiler.hpp"
 * 
 * int main() {
 *     nlm::profiling::initializeProfiler();
 *     nlm::profiling::startProfiler();
 *     
 *     // Profile a computation
 *     PROFILE_SCOPE("computation", "example")
 *     
 *     double result = computeSomething();
 *     
 *     nlm::profiling::stopProfiler();
 *     nlm::profiling::generateProfilerReport("results.json");
 *     
 *     return 0;
 * }
 * @endcode
 * 
 * This will generate a report containing timing information for the "computation" operation.
 * 
 */

/**
 * 
 * # Example: Integration with Brain System
 * 
 * This example shows how to integrate profiling with the NLM brain system:
 * 
 * @code
 * void Brain::step(SimulationStep step, Timestamp time) {
 *     PROFILE_BRAIN_PERFORMANCE(step)
 *     
 *     // Your brain processing code
 *     // ...
 *     
 *     // Profile specific regions
 *     for (auto& region : regions_) {
 *         PROFILE_SCOPE("process_region", "brain")
 *         region->updateNeuronStates();
 *     }
 * }
 * @endcode
 * 
 * This will profile brain operations and provide detailed timing information.
 * 
 */
```

## Conclusion

The NLM performance profiling system provides comprehensive performance measurement and analysis capabilities. This document has covered:

1. **Basic Profiling**: Simple timing and metrics collection
2. **Advanced Profiling**: Memory profiling, real-time monitoring, thread-specific profiling
3. **Integration Examples**: Integration with NLM systems (brain, motor, sensory)
4. **Performance Analysis**: Bottleneck detection, regression analysis
5. **Configuration Management**: Different configuration file templates
6. **Build Integration**: CMake and build system integration
7. **Command-Line Interface**: Command-line options and shell scripts
8. **Python Integration**: Python analysis and visualization
9. **Documentation Examples**: API documentation with code examples

The profiling system is designed to be flexible, allowing users to:

- Choose appropriate profiling levels for their needs
- Configure thresholds based on their requirements
- Integrate profiling with existing NLM code
- Analyze performance using various report formats
- Detect and fix performance issues

For more information, refer to the specific documentation files and examples in the repository.
