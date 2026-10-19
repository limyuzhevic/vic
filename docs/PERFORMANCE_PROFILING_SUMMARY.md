# NLM Performance Profiling System - Summary
#
# Phase 8: Performance Profiling Implementation
#
# This document provides a comprehensive overview of the NLM performance profiling system
# implemented as part of Phase 8 of the NLM framework development.
#
# ## Overview
#
# The NLM performance profiling system is a comprehensive performance measurement and analysis
# framework designed to help users understand and optimize their NLM deployments. It provides:
#
# 1. **Performance Measurement Core**: High-resolution timers, measurement points, nested timing,
#    thread-local storage, and global aggregation.
#
# 2. **Profiling Tools**: Timer classes, profilers, scoped timers, memory profilers, and CPU profilers.
#
# 3. **Performance Metrics Collection**: Spike rate measurements, computational complexity metrics,
#    memory consumption tracking, I/O performance measurements, and network latency measurements.
#
# 4. **Performance Analysis Tools**: Profile report generation (text, CSV, JSON), statistical analysis,
#    bottleneck detection, performance regression detection, and comparative profiling.
#
# 5. **Real-time Monitoring**: Live performance dashboards, threshold-based alerts, performance warning
#    systems, and resource usage monitoring.
#
# 6. **Integration with NLM Systems**: Brain performance profiling, motor system performance tracking,
#    sensory processing performance, and neuromodulation performance metrics.
#
# 7. **Configuration Support**: Enable/disable profiling via config, configure output format and
#    frequency, set performance thresholds, and configure profiling duration.
#
# 8. **Multi-threaded Support**: Thread-safe performance measurements, per-thread performance data,
#    global performance aggregation, and thread contention measurements.
#
# ## Implementation Details
#
# The performance profiling system is implemented in the following files:
#
# ### Core Performance Profiling Headers
#
# - `src/performance/NLMPerformanceProfiler.hpp`: Main profiling interface and classes
# - `src/performance/ProfilerConfigManager.hpp`: Configuration management
# - `src/performance/ProfilerConfig.hpp`: Configuration structure
#
# ### Performance Profiling Implementations
#
# - `src/performance/NLMPerformanceProfiler.cpp`: Core profiling implementation
# - `src/performance/ProfilerConfigManager.cpp`: Configuration file parsing
#
# ### Documentation and Examples
#
# - `docs/PERFORMANCE_PROFILING_GUIDE.md`: Comprehensive user guide
# - `docs/PERFORMANCE_ANALYSIS.md`: Performance analysis documentation
# - `examples/`: Collection of example applications
#
# - `examples/PerformanceProfilingExample.cpp`: Basic performance profiling example
# - `examples/brain_example.cpp`: Brain system profiling example
# - `examples/motor_example.cpp`: Motor system profiling example
# - `examples/memory_example.cpp`: Memory profiling example
# - `examples/bottleneck_example.cpp`: Bottleneck detection example
# - `examples/regression_example.cpp`: Performance regression detection example
# - `examples/thread_example.cpp`: Thread-specific profiling example
#
# ### Configuration Files
#
# - `configs/performance.cfg`: Default performance configuration
# - `configs/debug_performance.cfg`: Debug mode configuration
# - `configs/production_performance.cfg`: Production mode configuration
# - `configs/test_performance.cfg`: Testing mode configuration
#
# ### Integration with Existing Build
#
# The performance profiling system integrates seamlessly with the existing NLM build system:
#
# 1. **CMake Integration**: Added `nlm_performance` library to CMakeLists.txt
# 2. **Build Options**: `NLM_PERFORMANCE_ENABLED` and `NLM_PROFILING_ENABLED` build options
# 3. **Configuration**: Configuration files support the existing NLM configuration format
#
# ### Key Features
#
# #### 1. High-Resolution Timing
#
# The profiler uses `std::chrono::high_resolution_clock` for nanosecond-precision timing:
#
# ```cpp
# #include "nlm/performance/NLMPerformanceProfiler.hpp"
# 
# void example() {
#     PROFILE_SCOPE("computation", "example")
#     
#     // Your code here
#     // ...
# }
# ```
#
# #### 2. Multi-level Profiling
#
# The profiler supports multiple levels of profiling:
#
# - **ScopedTimers**: For simple timing of code blocks
# - **Function Profiling**: For profiling entire functions
# - **Nested Timing**: For hierarchical timing of related operations
# - **Thread-Specific Profiling**: For per-thread performance analysis
#
# #### 3. Memory and CPU Profiling
#
# The profiler includes dedicated memory and CPU profilers:
#
# ```cpp
# auto& profiler = nlm::profiling::GlobalProfiler::instance();
# auto& memoryProfiler = profiler.getMemoryProfiler();
# auto& cpuProfiler = profiler.getCPUProfiler();
# ```
#
# #### 4. Performance Metrics Collection
#
# The profiler collects various performance metrics:
#
# - **Timing Metrics**: Operation durations, execution times
# - **Memory Metrics**: Memory usage, allocation patterns
# - **CPU Metrics**: CPU usage, thread utilization
# - **Spike Metrics**: Neural spike rates, firing patterns
# - **I/O Metrics**: Input/output performance
# - **Network Metrics**: Network latency, bandwidth
#
# #### 5. Performance Analysis
#
# The profiler provides comprehensive performance analysis:
#
# - **Statistical Analysis**: Mean, median, standard deviation, percentiles
# - **Bottleneck Detection**: Identification of slow operations
# - **Regression Detection**: Comparison with baseline performance
# - **Comparative Profiling**: Multiple profile comparisons
#
# #### 6. Real-time Monitoring
#
# The profiler supports real-time monitoring:
#
# - **Live Dashboards**: Real-time performance visualization
# - **Threshold Alerts**: Automatic alerts when thresholds are exceeded
# - **Resource Monitoring**: Continuous monitoring of system resources
# - **Performance Warning System**: Early warning of performance issues
#
# #### 7. Integration with NLM Systems
#
# The profiler integrates with all NLM systems:
#
# - **Brain System**: Profile neural computation, plasticity, neuromodulation
# - **Motor System**: Profile action selection, planning, execution
# - **Sensory System**: Profile perception, processing, interpretation
# - **Neuromodulation System**: Profile reward, prediction error, learning
# - **Plasticity System**: Profile synaptic changes, learning rates
#
# #### 8. Configuration Management
#
# The profiler uses a comprehensive configuration system:
#
# ```cpp
# // Initialize profiler with configuration
# nlm::profiling::initializeProfiler("configs/performance.cfg");
# 
# // Get current configuration
# const auto& config = nlm::profiling::getProfilerConfig();
# ```
#
# Configuration options include:
#
# - **Basic Settings**: Enable/disable, output files, sampling intervals
# - **Thresholds**: Memory, CPU, operation, and spike rate thresholds
# - **Output Formatting**: Text, CSV, JSON report generation
# - **Integration Options**: Profile brain, motor, sensory systems
# - **Multi-threading**: Thread-local profiling, global aggregation
# - **Analysis Options**: Bottleneck detection, regression detection
# - **Alerting**: Threshold-based alerts, custom alert callbacks
# - **Advanced Options**: Memory tracking, CPU tracking, I/O tracking
#
# ## Usage Examples
#
# ### Example 1: Basic Usage
#
# ```bash
# # Enable profiling in CMake
# cmake -DNLM_PERFORMANCE_ENABLED=ON -DNLM_PROFILING_ENABLED=ON
# 
# # Build
# make -j4
# 
# # Run with performance configuration
# ./nlm --config configs/performance.cfg
# ```
#
# ```cpp
# // File: src/main.cpp
# 
# #include "nlm/performance/ProfilerConfigManager.hpp"
# 
# int main() {
#     // Initialize profiler
#     nlm::profiling::initializeProfiler("configs/performance.cfg");
#     
#     // Start profiling
#     nlm::profiling::startProfiler();
#     
#     // Your application code
#     // ...
#     
#     // Stop profiling
#     nlm::profiling::stopProfiler();
#     
#     // Generate reports
#     nlm::profiling::generateProfilerReport("performance_report.json");
#     
#     return 0;
# }
# ```
#
# ### Example 2: Integration with Brain System
#
# ```cpp
# // File: examples/brain_example.cpp
# 
# #include "nlm/performance/NLMPerformanceProfiler.hpp"
# #include "brain/Brain.hpp"
# 
# class BrainProfilingExample {
# public:
#     BrainProfilingExample() {
#         nlm::profiling::initializeProfiler("configs/brain_config.cfg");
#         nlm::profiling::startProfiler();
#         
#         brain_ = std::make_shared<Brain>(config_);
#         brain_->initialize();
#     }
#     
#     void runBrainSimulation() {
#         PROFILE_SCOPE("brain_simulation", "brain")
#         
#         for (SimulationStep step = 0; step < 1000; ++step) {
#             PROFILE_BRAIN_PERFORMANCE(brain_step)
#             brain_->step(step, step * 0.001);
#         }
#     }
#     
# private:
#     std::shared_ptr<Brain> brain_;
#     std::shared_ptr<Config> config_;
# };
# ```
#
# ### Example 3: Memory Profiling
#
# ```cpp
# // File: examples/memory_example.cpp
# 
# #include "nlm/performance/NLMPerformanceProfiler.hpp"
# 
# class MemoryProfilingExample {
# public:
#     MemoryProfilingExample() {
#         nlm::profiling::initializeProfiler("configs/memory_config.cfg");
#         nlm::profiling::startProfiler();
#     }
#     
#     void runMemoryTest() {
#         auto& profiler = nlm::profiling::GlobalProfiler::instance();
#         auto& memoryProfiler = profiler.getMemoryProfiler();
#         
#         // Memory-intensive operations
#         std::vector<double> data(1000000, 1.0);
#         
#         // Sample memory usage
#         memoryProfiler.sample("memory_test");
#     }
# };
# ```
#
# ### Example 4: Bottleneck Detection
#
# ```cpp
# // File: examples/bottleneck_example.cpp
# 
# #include "nlm/performance/NLMPerformanceProfiler.hpp"
# 
# class BottleneckDetectionExample {
# public:
#     void detectBottlenecks() {
#         nlm::profiling::initializeProfiler("configs/bottleneck_config.cfg");
#         nlm::profiling::startProfiler();
#         
#         // Run workload
#         runWorkload();
#         
#         nlm::profiling::stopProfiler();
#         
#         // Detect bottlenecks
#         nlm::profiling::GlobalProfiler::instance().checkForBottlenecks();
#     }
#     
# private:
#     void runWorkload() {
#         PROFILE_SCOPE("workload", "bottleneck")
#         
#         for (int i = 0; i < 1000; ++i) {
#             PROFILE_SCOPE("cpu_intensive", "bottleneck")
#             cpuIntensiveOperation(i);
#             
#             PROFILE_SCOPE("memory_intensive", "bottleneck")
#             memoryIntensiveOperation(i);
#         }
#     }
#     
#     void cpuIntensiveOperation(int i) {
#         // CPU-intensive computation
#         double result = 0.0;
#         for (int j = 0; j < 1000000; ++j) {
#             result += std::sin(j * 0.1) * std::cos(j * 0.05);
#         }
#     }
#     
#     void memoryIntensiveOperation(int i) {
#         // Memory-intensive operations
#         std::vector<double> data(1000000, 1.0);
#     }
# };
# ```
#
# ## Configuration Examples
#
# ### Debug Configuration
#
# ```ini
# # File: configs/debug_performance.cfg
#
# [profiling]
# enabled = true
# output_file = debug_performance_results.json
# sample_interval_ms = 10
# report_interval_s = 30
#
# [thresholds]
# memory_threshold_mb = 512
# cpu_threshold_percent = 70
# operation_threshold_ns = 100000
#
# [monitoring]
# enable_memory_tracking = true
# enable_cpu_tracking = true
# enable_io_tracking = true
# 
# [analysis]
# detect_bottlenecks = true
# detect_regressions = true
# ```
#
# ### Production Configuration
#
# ```ini
# # File: configs/production_performance.cfg
#
# [profiling]
# enabled = true
# output_file = performance_results.json
# sample_interval_ms = 1000
# report_interval_s = 300
#
# [thresholds]
# memory_threshold_mb = 2048
# cpu_threshold_percent = 90
# operation_threshold_ns = 10000000
#
# [monitoring]
# enable_memory_tracking = true
# enable_cpu_tracking = true
# enable_io_tracking = false
#
# [analysis]
# detect_bottlenecks = true
# detect_regressions = true
# ```
#
# ## Build Instructions
#
# ### Enabling Performance Profiling
#
# To enable performance profiling, modify your CMakeLists.txt:
#
# ```cmake
# # Enable performance optimizations
# set(NLM_PERFORMANCE_ENABLED ON CACHE BOOL "Enable performance optimizations")
# 
# # Enable performance profiling
# set(NLM_PROFILING_ENABLED ON CACHE BOOL "Enable performance profiling")
# 
# # Add performance profiling library
# add_library(nlm_performance
#     src/performance/NLMPerformanceProfiler.cpp
# )
# 
# # Link performance library to main executable
# target_link_libraries(nlm PUBLIC nlm_performance)
# ```
#
# ### Building with Performance Profiling
#
# ```bash
# # Configure with performance profiling enabled
# cmake -B build -DNLM_PERFORMANCE_ENABLED=ON -DNLM_PROFILING_ENABLED=ON
# 
# # Build
# cd build && make -j4
# 
# # Run with performance configuration
# ./nlm --config configs/performance.cfg
# ```
#
# ## Performance Considerations
#
# ### When Profiling is Disabled
#
# The profiling system has minimal overhead when disabled:
#
# - All profiler classes are empty/no-op
# - Macros expand to nothing
# - Memory usage is negligible
# - No performance impact on hot paths
#
# ### When Profiling is Enabled
#
# The profiling system is designed to have minimal overhead:
#
# - Use nanosecond-precision timers with minimal overhead
# - Thread-local storage reduces contention
# - Configurable sampling intervals
# - Optional output formats to reduce I/O overhead
# - Real-time monitoring can be disabled in production
#
# ## Performance Best Practices
#
# 1. **Use Appropriate Profiling Levels**:
#    - **Debug Builds**: Use detailed profiling with frequent sampling
#    - **Release Builds**: Use performance profiling with moderate sampling
#    - **Production**: Use essential profiling with infrequent sampling
#
# 2. **Configure Thresholds Wisely**:
#
# ```cpp
# ProfilerConfig config;
# config.operation_threshold_ns = 1000000;  # 1ms threshold
# config.memory_threshold_mb = 1024;         # 1GB threshold
# config.cpu_threshold_percent = 80;         # 80% threshold
# ```
#
# 3. **Balance Detail and Performance**:
#
# ```cpp
# // Trade-off between detail and overhead
# ProfilerConfig config;
# config.enableMemoryTracking = true;    // High detail, high overhead
# config.enableCPUTracking = true;       // High detail, high overhead
# config.enableIOTracking = false;       // Low detail, low overhead
# ```
#
# 4. **Use ScopedTimers for Hot Paths**:
#
# ```cpp
# void hotFunction() {
#     PROFILE_SCOPE("hot_function", "performance")
#     
#     // Hot path code
#     // ...
# }
# ```
#
# 5. **Batch I/O Operations**:
#
# ```cpp
# void batchOperation() {
#     PROFILE_SCOPE("batch_operation", "performance")
#     
#     // Collect metrics
#     // ...
#     
#     // Write once instead of frequently
#     writePerformanceData();
# }
# ```
#
# ## Troubleshooting
#
# ### Common Issues
#
# 1. **Profiler Not Starting**:
#    - Ensure `NLM_PROFILING_ENABLED` is defined
#    - Call `initializeProfiler()` in main()
#    - Check configuration file exists
#
# 2. **High Performance Overhead**:
#    - Use production configuration
#    - Increase sample interval
#    - Disable detailed tracking
#
# 3. **Missing Output Files**:
#    - Check file permissions
#    - Ensure output configuration is enabled
#    - Verify profiler is stopped
#
# 4. **Thread Safety Issues**:
#    - Use thread-local profiling
#    - Ensure thread-safe access
#
# ## Testing
#
# The performance profiling system includes tests:
#
# 1. **Unit Tests**: Test individual profiler components
# 2. **Integration Tests**: Test profiler integration with NLM systems
# 3. **Performance Tests**: Test performance under various workloads
# 4. **Configuration Tests**: Test configuration parsing and validation
#
# ### Running Tests
#
# ```bash
# # Build tests
# cd build && make nlm_test
# 
# # Run tests
# ./nlm_test
# ```
#
# ## References
#
# - [Existing Performance Infrastructure](../performance/Performance.hpp)
# - [Parallel Processing System](../performance/ParallelProcessing.hpp)
# - [Memory Pool Allocator](../performance/MemoryPool.hpp)
# - [Checkpoint System](../performance/CheckpointSystem.hpp)
#
# - [NLM Performance Documentation](docs/PERFORMANCE.md)
# - [NLM Configuration Reference](docs/CONFIG_REFERENCE.md)
#
# - [Performance Optimization Guide](docs/PERFORMANCE_OPTIMIZATION.md)
#
# ## Version History
#
# ### Version 1.0 (Phase 6)
# - Initial implementation
# - Basic timing and metrics collection
# - Integration with NLM systems
#
# ### Version 2.0 (Phase 8)
# - Comprehensive performance profiling system
# - Advanced metrics collection and analysis
# - Real-time monitoring and alerts
# - Multi-threaded support
# - Configuration management
#
# ### Version 3.0 (Current)
# - Complete performance profiling system
# - Full integration with all NLM systems
# - Production-ready with extensive configuration options
# - Comprehensive documentation and examples
#
# ## Conclusion
#
# The NLM performance profiling system provides comprehensive performance measurement and analysis
# capabilities for the NLM framework. It is designed to be:
#
# - **Comprehensive**: Covers all aspects of performance measurement
# - **Flexible**: Configurable for different use cases
# - **Performant**: Minimal overhead when profiling is disabled
# - **Integratable**: Seamlessly integrates with existing NLM code
# - **Documented**: Comprehensive documentation and examples
#
# The system is production-ready and can be used to:
#
# - Understand NLM performance characteristics
# - Identify performance bottlenecks
# - Compare performance across versions
# - Monitor performance in production
# - Optimize NLM performance
#
# For more information, refer to the specific documentation files and examples in the repository.
