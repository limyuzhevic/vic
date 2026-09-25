# Manual Integration Guide for NLM Performance Monitoring

## Overview

This guide provides detailed instructions for manually integrating the NLM performance monitoring and benchmarking system with the existing codebase. The performance monitoring infrastructure is designed to be optional and can be integrated incrementally.

## Required Manual Integrations

### 1. Brain.cpp Performance Hooks Integration

**File to Modify:** `src/brain/Brain.cpp`

**Changes Required:**

1. **Add Performance Monitoring Headers**
   ```cpp
   #include "../performance/Monitor.hpp"
   #include "../performance/Benchmark.hpp"
   ```

2. **Update Brain::Impl Structure**
   ```cpp
   struct Brain::Impl {
       // ... existing members ...
       
       // Add performance monitoring
       std::unique_ptr<nlm::PerformanceMonitor> performanceMonitor;
       std::unique_ptr<nlm::Benchmark> benchmark;
   
       // ... rest of existing members ...
   ```

3. **Update Constructor**
   Add performance monitoring initialization in the Impl constructor:
   ```cpp
   // Initialize performance monitoring if enabled
   bool perfMonitoringEnabled = config->getOr<bool>("performance.monitoring.enabled", true);
   if (perfMonitoringEnabled) {
       performanceMonitor = std::make_unique<nlm::PerformanceMonitor>();
       benchmark = std::make_unique<nlm::Benchmark>();
       
       // Configure performance monitoring
       double snapshotInterval = config->getOr<double>("performance.monitoring.snapshot_interval", 1.0);
       performanceMonitor->setMonitoringInterval(snapshotInterval);
       performanceMonitor->setSnapshotInterval(snapshotInterval);
       
       benchmark->enable(true);
       
       // Integrate performance monitoring with NLM systems
       performanceMonitor->integrateWithBrain(this);
       benchmark->integrateWithBrain(this);
   }
   ```

4. **Update initialize() Method**
   Add performance monitoring hooks:
   ```cpp
   bool Brain::initialize() {
       NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
       
       // Start performance monitoring if enabled
       if (pImpl->performanceMonitor) {
           pImpl->performanceMonitor->startBenchmark("initialization");
           pImpl->performanceMonitor->startPhase(nlm::BenchmarkPhase::Initialization);
       }
       
       // ... existing code ...
       
       // Start region creation timing
       if (pImpl->performanceMonitor) {
           pImpl->performanceMonitor->startTimer("region_creation");
       }
       
       // Create regions
       for (size_t i = 0; i < regionCount; ++i) {
           addRegion("Region_" + std::to_string(i + 1));
       }
       
       // Stop region creation timing
       if (pImpl->performanceMonitor) {
           pImpl->performanceMonitor->stopTimer("region_creation");
           pImpl->performanceMonitor->recordEvent("region_creation", pImpl->performanceMonitor->getCurrentCharacteristics().simdUtilization);
       }
       
       // Stop initialization benchmark
       if (pImpl->performanceMonitor) {
           pImpl->performanceMonitor->endPhase(nlm::BenchmarkPhase::Initialization);
           auto initResult = pImpl->performanceMonitor->stopBenchmark("initialization");
           
           NLM_LOG_INFO("Brain initialization complete in " + std::to_string(initResult.duration) + "s");
           NLM_LOG_INFO("Initialization performance: " + std::to_string(initResult.opsPerSecond) + " ops/sec");
       }
       
       // Initialize integrated systems with performance monitoring
       if (pImpl->workingMemory) {
           if (pImpl->performanceMonitor) {
               pImpl->performanceMonitor->startTimer("working_memory_init");
           }
           
           pImpl->workingMemory->initialize(this);
           pImpl->workingMemory->setCapacity(neuronCount / 10);
           
           if (pImpl->performanceMonitor) {
               pImpl->performanceMonitor->stopTimer("working_memory_init");
               pImpl->performanceMonitor->recordEvent("working_memory_init", pImpl->performanceMonitor->getCurrentCharacteristics().simdUtilization);
           }
       }
       
       // ... continue for other systems ...
       
       // Register spike handlers with performance monitoring
       pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
           ++pImpl->totalSpikesThisStep;
           ++pImpl->totalSpikesTotal;
           
           if (pImpl->performanceMonitor) {
               pImpl->performanceMonitor->recordTimelineEvent(nlm::TimelineEvent{
                   .timestamp = pImpl->currentTime,
                   .eventType = "spike_detection",
                   .threadId = std::hash<std::thread::id>{}(std::this_thread::get_id()),
                   .duration = 0.0,
                   .metadata = {
                       {"neuron_id", std::to_string(event.source_neuron)},
                       {"spike_time", std::to_string(event.spike_time)}
                   }
               });
           }
       });
       
       // ... rest of existing code ...
   }
   ```

5. **Update step() Method**
   Add performance monitoring in the step method:
   ```cpp
   void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
       // ... existing comment ...
       
       // Start performance monitoring for this step
       if (pImpl->performanceMonitor) {
           pImpl->performanceMonitor->startStep(currentStep, currentTime);
       }
       
       // ========== STEP 1: Process pending delayed spikes ==========
       if (pImpl->performanceMonitor) {
           pImpl->performanceMonitor->startTimer("delayed_spike_processing");
       }
       pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
       if (pImpl->performanceMonitor) {
           pImpl->performanceMonitor->stopTimer("delayed_spike_processing");
           pImpl->performanceMonitor->recordEvent("delayed_spike_processing", pImpl->performanceMonitor->getCurrentCharacteristics().simdUtilization);
       }
       
       // ========== STEP 2: Update all neurons ==========
       if (pImpl->performanceMonitor) {
           pImpl->performanceMonitor->startTimer("neuron_updates");
       }
       for (auto& region : pImpl->regions) {
           for (auto& pop : region->getPopulations()) {
               for (auto* neuron : pop->getNeurons()) {
                   neuron->stepLIF(currentTime, pImpl->timestep);
               }
           }
       }
       if (pImpl->performanceMonitor) {
           pImpl->performanceMonitor->stopTimer("neuron_updates");
           pImpl->performanceMonitor->recordEvent("neuron_updates", pImpl->performanceMonitor->getCurrentCharacteristics().simdUtilization);
       }
       
       // ========== STEP 3: Detect spikes ==========
       if (pImpl->performanceMonitor) {
           pImpl->performanceMonitor->startTimer("spike_detection");
       }
       for (auto& region : pImpl->regions) {
           for (auto& pop : region->getPopulations()) {
               for (auto* neuron : pop->getNeurons()) {
                   // ... existing spike detection code ...
               }
           }
       }
       if (pImpl->performanceMonitor) {
           pImpl->performanceMonitor->stopTimer("spike_detection");
           pImpl->performanceMonitor->recordEvent("spike_detection", pImpl->performanceMonitor->getCurrentCharacteristics().simdUtilization);
       }
       
       // ========== STEP 4-15: Continue for other steps ...
       
       // End step performance monitoring
       if (pImpl->performanceMonitor) {
           pImpl->performanceMonitor->endStep(currentStep, currentTime);
       }
       
       // ... rest of existing code ...
   }
   ```

6. **Add Performance System Accessors**
   Add accessors to the Brain class:
   ```cpp
   nlm::PerformanceMonitor* Brain::getPerformanceMonitor() {
       return pImpl->performanceMonitor.get();
   }
   
   nlm::Benchmark* Brain::getBenchmark() {
       return pImpl->benchmark.get();
   }
   ```

### 2. Python Bindings Integration

**File to Modify:** `python/bindings.cpp`

**Changes Required:**

1. **Add Python Bindings for PerformanceMonitor**
   ```cpp
   // Add to PYBIND11_MODULE(nlm, m)
   
   py::class_<nlm::PerformanceMonitor>(m, "PerformanceMonitor")
       .def(py::init<>())
       .def("enable", &nlm::PerformanceMonitor::enable, py::arg("enable"))
       .def("is_enabled", &nlm::PerformanceMonitor::isEnabled)
       .def("start_benchmark", &nlm::PerformanceMonitor::startBenchmark, py::arg("name"))
       .def("stop_benchmark", &nlm::PerformanceMonitor::stopBenchmark, py::arg("name"))
       .def("take_snapshot", &nlm::PerformanceMonitor::takeSnapshot)
       .def("get_snapshots", &nlm::PerformanceMonitor::getSnapshots)
       .def("get_benchmark_results", &nlm::PerformanceMonitor::getBenchmarkResults)
       .def("generate_report", &nlm::PerformanceMonitor::generateReport)
       .def("save_report", &nlm::PerformanceMonitor::saveReport, py::arg("filename"))
       .def("get_optimization_recommendations", &nlm::PerformanceMonitor::getOptimizationRecommendations)
       .def("apply_auto_optimizations", &nlm::PerformanceMonitor::applyAutoOptimizations);
   ```

2. **Add Python Bindings for Benchmark**
   ```cpp
   py::class_<nlm::Benchmark>(m, "Benchmark")
       .def(py::init<>())
       .def("configure", &nlm::Benchmark::configure, py::arg("config"))
       .def("enable", &nlm::Benchmark::enable, py::arg("enable"))
       .def("run", &nlm::Benchmark::run, py::arg("name"))
       .def("run_all", &nlm::Benchmark::runAll)
       .def("run_micro_benchmarks", &nlm::Benchmark::runMicroBenchmarks)
       .def("run_neuron_benchmarks", &nlm::Benchmark::runNeuronBenchmarks)
       .def("run_synapse_benchmarks", &nlm::Benchmark::runSynapseBenchmarks)
       .def("run_memory_benchmarks", &nlm::Benchmark::runMemoryBenchmarks)
       .def("run_parallel_benchmarks", &nlm::Benchmark::runParallelBenchmarks)
       .def("run_event_driven_benchmarks", &nlm::Benchmark::runEventDrivenBenchmarks)
       .def("run_integration_benchmarks", &nlm::Benchmark::runIntegrationBenchmarks)
       .def("get_benchmark_history", &nlm::Benchmark::getBenchmarkHistory)
       .def("get_scaling_results", &nlm::Benchmark::getScalingResults)
       .def("get_comparative_results", &nlm::Benchmark::getComparativeResults)
       .def("generate_flame_graph", &nlm::Benchmark::generateFlameGraph, py::arg("filename"))
       .def("generate_detailed_report", &nlm::Benchmark::generateDetailedReport, py::arg("filename"))
       .def("generate_performance_summary", &nlm::Benchmark::generatePerformanceSummary, py::arg("filename"));
   ```

3. **Add Configuration Support**
   Add benchmark configuration structure:
   ```cpp
   struct BenchmarkConfig {
       std::string name;
       nlm::BenchmarkType type;
       size_t neuronCount;
       size_t synapseCount;
       size_t steps;
       double timeStep;
       size_t warmupSteps;
       bool measureMemory;
       bool measureEnergy;
       bool generateFlameGraph;
       std::map<std::string, std::string> customSettings;
   };
   ```

### 3. CMake Integration

**File to Modify:** `CMakeLists.txt`

**Changes Required:**

1. **Add Performance Monitoring Libraries**
   ```cmake
   # Add performance monitoring libraries
   add_library(nlm_performance STATIC
       src/performance/Monitor.cpp
       src/performance/Benchmark.cpp
   )
   
   target_include_directories(nlm_performance PUBLIC
       $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/performance>
   )
   
   target_link_libraries(nlm_performance PUBLIC nlm_core)
   ```

2. **Add Conditional Compilation**
   ```cmake
   # Add performance monitoring options
   option(NLM_PERF_MONITORING_ENABLED "Enable performance monitoring" ON)
   option(NLM_BENCHMARKS_ENABLED "Enable benchmarking" ON)
   
   if(NLM_PERF_MONITORING_ENABLED)
       target_compile_definitions(nlm_performance PUBLIC NLM_PERF_ENABLED=1)
   endif()
   
   if(NLM_BENCHMARKS_ENABLED)
       target_compile_definitions(nlm_performance PUBLIC NLM_BENCHMARKS_ENABLED=1)
   endif()
   ```

3. **Update Main Executable**
   ```cmake
   # Link performance monitoring to main executable
   target_link_libraries(nlm PUBLIC nlm_performance)
   target_link_libraries(nlm_phase3_demo PUBLIC nlm_performance)
   target_link_libraries(nlm_phase4_demo PUBLIC nlm_performance)
   ```

### 4. Documentation Updates

**File to Modify:** `docs/PERFORMANCE.md`

**Changes Required:**

1. **Add Performance Monitoring Section**
   - Add overview of performance monitoring capabilities
   - Document PerformanceMonitor class and methods
   - Document Benchmark class and benchmark types

2. **Add Integration Examples**
   - Example 1: Basic performance monitoring in Brain
   - Example 2: Python bindings usage
   - Example 3: Benchmarking workflow

3. **Add Configuration Examples**
   - Configuration for performance monitoring
   - Configuration for benchmarking
   - Examples for different use cases

### 5. Example Benchmark Scripts

**Directory to Create:** `examples/benchmarks/`

**Required Files:**

1. **basic_benchmark.py** - Basic benchmark script
2. **neuron_scaling.py** - Neuron scaling benchmark
3. **comparative_benchmark.py** - Comparative benchmark (scalar vs SIMD)
4. **memory_bandwidth.py** - Memory bandwidth benchmark
5. **event_driven.py** - Event-driven performance benchmark
6. **integration_benchmark.py** - Integration benchmark

**Example File Structure:**

```python
"""Basic benchmark script demonstrating performance monitoring usage."""
import pynlm as nlm
import time

def basic_benchmark():
    """Run a basic benchmark of neuron updates."""
    print("Running basic neuron update benchmark...")
    
    # Create brain with configuration
    config = nlm.Config()
    config.set("brain.neuron_count", 1000)
    config.set("performance.monitoring.enabled", True)
    
    brain = nlm.Brain(config)
    brain.initialize()
    
    # Get performance monitor
    monitor = brain.get_performance_monitor()
    
    # Start benchmark
    monitor.start_benchmark("neuron_updates")
    
    # Run simulation
    start_time = time.time()
    for step in range(100):
        brain.step(step, step * 0.001)
    end_time = time.time()
    
    # Stop benchmark
    result = monitor.stop_benchmark("neuron_updates")
    
    print(f"Benchmark completed in {result.duration:.2f} seconds")
    print(f"Performance: {result.opsPerSecond:.0f} ops/sec")
    print(f"SIMD level: {result.simdLevel}")
    print(f"Memory usage: {result.memoryUsed / 1024 / 1024:.2f} MB")
    
    # Take snapshot
    snapshot = monitor.take_snapshot()
    print(f"Active neurons: {snapshot.activeNeurons}")
    print(f"Memory usage: {snapshot.memoryUsage} bytes")
    
    return result

if __name__ == "__main__":
    result = basic_benchmark()
    print("Benchmark completed successfully!")
```

## Installation and Usage

### Build with Performance Monitoring

```bash
# Build with performance monitoring enabled
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON -DNLM_PERF_MONITORING_ENABLED=ON ..
make -j$(nproc)
```

### Run Benchmarks

```bash
# Run all benchmarks
./build/nlm --benchmark

# Run specific benchmark
./build/nlm --benchmark --benchmark_type neuron

# Run scaling benchmark
./build/nlm --scaling-benchmark --neuron_counts 1000,10000,100000
```

### Python Usage

```python
import pynlm as nlm

# Create performance monitor
monitor = nlm.PerformanceMonitor()
monitor.enable(True)

# Start benchmark
monitor.start_benchmark("neuron_updates")

# ... perform operations ...

# Stop benchmark
result = monitor.stop_benchmark("neuron_updates")

# Generate report
report = monitor.generate_report()
monitor.save_report("performance_report.txt")
```

### C++ Usage

```cpp
#include "performance/Monitor.hpp"
#include "performance/Benchmark.hpp"

// Get instances
auto& monitor = nlm::PerformanceMonitor::getInstance();
auto& benchmark = nlm::Benchmark::getInstance();

// Configure and run
benchmark.configure(config);
benchmark.runAll();

// Get results
auto history = benchmark.getBenchmarkHistory();
auto scaling = benchmark.getScalingResults();
```

## Troubleshooting

### Common Issues and Solutions

1. **Performance Monitoring Not Working**
   - Check that `NLM_PERF_MONITORING_ENABLED` is defined
   - Verify that performance monitoring is initialized in Brain constructor
   - Ensure that integration is set up correctly

2. **Benchmarks Not Running**
   - Check that `NLM_BENCHMARKS_ENABLED` is defined
   - Verify that benchmark configuration is correct
   - Ensure that benchmarking is enabled in Brain constructor

3. **Python Bindings Not Available**
   - Check that Python bindings are built correctly
   - Verify that performance monitoring classes are registered
   - Ensure that pybind11 dependencies are met

4. **Memory Usage High**
   - Check for memory leaks in performance monitoring code
   - Ensure that snapshots are cleared periodically
   - Verify that benchmark results are managed properly

### Debug Commands

```bash
# Check performance monitoring status
./build/nlm --perf-monitor-status

# Generate performance report
./build/nlm --generate-report perf_report.txt

# Run benchmarks with detailed output
./build/nlm --benchmark --verbose
```

## Compatibility

### Supported Platforms
- Linux x86_64
- macOS (Intel)
- Windows (with appropriate compilers)

### Supported Compilers
- GCC 9.0+
- Clang 10.0+
- MSVC 2019+

### Dependencies
- C++20 or later
- pybind11 (for Python bindings)
- CMake 3.16+
- Standard C++ libraries

## License

This performance monitoring and benchmarking system is distributed under the same license as the main NLM project. Please refer to the main LICENSE file for details.

## Version Information

- Version: 1.0.0
- Date: 2026-09-25
- Authors: NLM Development Team
- Status: Beta Release

For more information and updates, please visit the NLM documentation or contact the development team.
