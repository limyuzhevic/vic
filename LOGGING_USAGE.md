# Installation and Usage Instructions

## Quick Start

1. **Clone the repository**
   ```bash
   git clone <your-repo>
   cd <your-repo>
   ```

2. **Build the project**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)
   ```

3. **Run tests**
   ```bash
   ./your_executable --run-logging-tests
   ```

## Running the Enhanced Logging System

### Enable Structured Logging

Add these lines to your main application:

```cpp
#include "src/core/Logger/Logger.hpp"
#include "src/core/Logger/DebugUtils.hpp"
#include "src/core/Logger/Monitor.hpp"

int main() {
    // Initialize logging systems
    auto& logger = nlm::StructuredLogger::getInstance();
    auto& debugUtils = nlm::DebugUtils::getInstance();
    auto& monitor = nlm::RealTimeMonitor::getInstance();
    
    // Configure logging
    logger.setMinimumLevel(nlm::LogLevel::Debug);
    logger.setMinimumCategory(nlm::LogCategory::System, true);
    logger.setMinimumCategory(nlm::LogCategory::Performance, true);
    logger.setLogFile("./logs/application.log");
    
    // Start real-time monitoring
    monitor.startMonitoring(1.0);  // 1 second intervals
    
    // Example usage
    LOG_INFO("Application started", "System");
    LOG_TRACE("Debug information", "Debug", {{ "debug_id", "12345" }});
    LOG_PERFORMANCE("startup", 150.5, {{ "component", "initialization" }});
    
    // Your application code here
    
    // Cleanup
    monitor.stopMonitoring();
    logger.flushLogs();
    
    return 0;
}
```

### Python Debugging API

```python
from python.debug_api import NLMDebugAPI, get_debug_api

# Get global debug API instance
debug_api = get_debug_api()

# Log structured messages
debug_api.log_info("Application started")
debug_api.log_debug("Debug information", category="Debug", metadata={"debug_id": "12345"})
debug_api.performance("startup", 150.5, category="Performance", tags={"component": "initialization"})

# Start performance benchmarks
result = debug_api.run_benchmark(
    "neural_network_inference",
    lambda: your_neural_network_function(),
    iterations=10,
    metadata={"model": "deep_net", "input_size": "1024"}
)

# Create visualizations
debug_api.create_memory_visualization("./visualizations/memory_report.html")
debug_api.create_performance_visualization("inference_time", "./visualizations/perf_report.html")
debug_api.create_trace_visualization("./visualizations/trace_report.html")

# Export debug data
debug_api.export_debug_data("./debug_export")
```

### Advanced Usage Examples

#### 1. Correlation Tracking for Distributed Systems

```cpp
// Start a correlation trace for a distributed operation
std::string correlationId = nlm::StructuredLogger::getInstance().generateCorrelationId();
nlm::StructuredLogger::getInstance().startCorrelationTrace(
    correlationId, 
    "data_processing_pipeline",
    {{ "source", "sensor_array_1" }, { "destination", "neural_network" }}
);

// Log multiple operations with the same correlation ID
LOG_INFO("Data preprocessing started", "System", correlationId);
LOG_INFO("Feature extraction completed", "Neural", correlationId, {{ "features", "128" }});
LOG_INFO("Model inference completed", "Neural", correlationId, {{ "confidence", "0.95" }});

// End the trace
nlm::StructuredLogger::getInstance().endCorrelationTrace(correlationId, true, "All processing completed");
```

#### 2. Memory Debugging for Debugging Memory Issues

```cpp
// Track memory allocations for debugging
void* myData = malloc(1024);
std::string allocId = nlm::DebugUtils::getInstance().trackMemoryAllocation(
    reinterpret_cast<intptr_t>(myData),
    1024,
    "neural_network_memory",
    {{ "neuron_id", "neuron_42" }, { "memory_type", "working_memory" }}
);

// Deallocate and track
delete myData;
nlm::DebugUtils::getInstance().trackMemoryDeallocation(allocId);

// Trigger memory cleanup if needed
nlm::DebugUtils::getInstance().cleanupMemory("high_usage_scenario");
```

#### 3. Performance Benchmarking

```cpp
// Benchmark neural network processing times
auto& debugUtils = nlm::DebugUtils::getInstance();

// Start a performance scope
PERFORMANCE_SCOPE("neuron_step_processing");

// Your neural network step code here
for (auto* neuron : neurons) {
    neuron->step(current_time, timestep);
}

// End the scope and log results
LOG_PERFORMANCE_SCOPE_END("neuron_step_processing", {{ "neuron_count", "1000" }});

// Run comprehensive benchmark
auto benchmark = debugUtils.runBenchmark(
    "full_brain_simulation",
    [&]() {
        // Simulate a full brain step
        brain.step(current_step, current_time);
    },
    iterations=100,
    metadata={
        {"brain_type", "integrated_phase6"},
        {"neuron_count", "5000"},
        {"synapse_count", "20000"}
    }
);
```

#### 4. Error Recovery with Context

```cpp
// Enhanced error recovery with context
bool processSensorData(const std::vector<float>& data) {
    try {
        // Simulate sensor processing
        if (data.empty()) {
            throw std::runtime_error("Empty sensor data");
        }
        
        // Process data with error recovery
        auto result = nlm::DebugUtils::getInstance().attemptRecovery(
            [&]() {
                // Your processing code here
                return processNeuralNetwork(data);
            },
            "sensor_processing",
            3,  // max retries
            0.1,  // retry delay
            {
                {"sensor_id", "sensor_001"},
                {"data_size", std::to_string(data.size())},
                {"processing_mode", "real_time"}
            }
        );
        
        return result;
        
    } catch (const std::exception& e) {
        // Log error with context
        nlm::DebugUtils::getInstance().logErrorContext(
            "sensor_processing",
            std::string("Sensor processing failed: ") + e.what(),
            {{ "sensor_id", "sensor_001" }, { "timestamp", std::to_string(current_time) }}
        );
        
        return false;
    }
}
```

#### 5. Real-Time Monitoring and Alerting

```cpp
// Configure monitoring and alerting
auto& monitor = nlm::RealTimeMonitor::getInstance();

// Add system resource monitoring
monitor.addSystemResourceMonitor([]() {
    nlm::SystemResourceMetrics metrics;
    metrics.cpuUsage = psutil.cpu_usage();
    auto memory = psutil.virtual_memory();
    metrics.memoryUsage = memory.percent;
    metrics.timestamp = std::chrono::system_clock::now();
    metrics.hostname = "brain_server";
    return metrics;
});

// Add simulation monitoring
monitor.addSimulationMonitor([]() {
    nlm::SimulationMetrics metrics;
    metrics.simulationId = "brain_simulation_001";
    metrics.stepNumber = current_simulation_step;
    metrics.simulationTime = simulation_elapsed_time;
    metrics.totalNeurons = brain->getTotalNeuronCount();
    metrics.activeNeurons = brain->getActiveNeuronCount();
    metrics.timestamp = std::chrono::system_clock::now();
    return metrics;
});

// Add performance monitoring
monitor.addPerformanceMetric({
    "name": "neuron_firing_rate",
    "value": brain->getAverageFiringRate(),
    "unit": "Hz",
    "timestamp": std::chrono::system_clock::now(),
    "tags": {{ "brain_region", "cortex" }}
});

// Add threshold alerts
monitor.addThresholdAlert(
    "system_cpu_usage",
    90.0,  // threshold percentage
    "warning",
    [](const std::string& metric, double value) {
        LOG_WARNING("High CPU usage detected", "System", {{ "metric", metric }, { "value", std::to_string(value) }});
        // Trigger cooling measures
        brain->reduceProcessingLoad();
    }
);

// Start monitoring
monitor.startMonitoring(1.0);  // 1 second intervals

// Your application code
while (running) {
    // Process brain step
    brain->step(current_step);
    
    // Check for alerts
    monitor.checkThresholds();
}

// Stop monitoring
monitor.stopMonitoring();
```

### Python Debug API Advanced Usage

```python
import asyncio
from python.debug_api import get_debug_api

async def run_simulation_with_debugging():
    debug_api = get_debug_api()
    
    # Start real-time monitoring
    debug_api.start_real_time_monitoring(interval=0.5)
    
    # Run simulation steps
    for step in range(1000):
        # Log simulation step
        debug_api.log_info(f"Simulation step {step}")
        
        # Process neural network step
        await process_neural_network_step(step)
        
        # Log performance metrics
        debug_api.performance("simulation_step", step_time, tags={
            "step": str(step),
            "neurons": str(len(neural_network.neurons))
        })
        
        # Check for errors and log them
        if error_detected:
            debug_api.log_error("Neural network instability", category="Error", metadata={
                "step": step,
                "error_type": "instability",
                "severity": "warning"
            })
    
    # Create comprehensive visualizations
    debug_api.create_memory_visualization("./visualizations/memory_usage.html")
    debug_api.create_performance_visualization("simulation_step", "./visualizations/performance.html")
    debug_api.create_trace_visualization("./visualizations/trace_analysis.html")
    
    # Export debug data
    debug_api.export_debug_data("./simulation_debug_export")
    
    # Stop monitoring
    debug_api.stop_real_time_monitoring()

# Run the simulation
asyncio.run(run_simulation_with_debugging())
```

## Configuration Files

### Debug Configuration

Create `debug_config.json`:

```json
{
    "logging": {
        "minimum_level": "Debug",
        "categories": {
            "System": true,
            "Simulation": true,
            "Neural": true,
            "Memory": true,
            "Performance": true,
            "Network": true,
            "GUI": true
        },
        "log_file": "./logs/debug.log",
        "max_file_size_mb": 10,
        "max_backup_files": 5
    },
    "monitoring": {
        "system_sample_interval": 1.0,
        "simulation_sample_interval": 0.5,
        "performance_sample_interval": 0.1,
        "enable_alerts": true,
        "alert_thresholds": {
            "cpu_usage": 90.0,
            "memory_usage": 85.0,
            "simulation_time": 100.0
        }
    },
    "debugging": {
        "enable_memory_debugging": true,
        "enable_cpu_profiling": true,
        "enable_network_debugging": true,
        "enable_gui_debugging": true,
        "max_memory_allocations": 1000,
        "max_cpu_profiles": 500
    }
}
```

### Build Configuration

Add to your CMakeLists.txt:

```cmake
# Enable logging and debugging features
option(ENABLE_STRUCTURED_LOGGING "Enable structured logging system" ON)
option(ENABLE_DEBUG_UTILS "Enable advanced debugging utilities" ON)
option(ENABLE_REALTIME_MONITORING "Enable real-time monitoring" ON)
option(ENABLE_PYTHON_DEBUG_API "Enable Python debugging API" ON)

# Link against logging libraries
if(ENABLE_STRUCTURED_LOGGING)
    target_link_libraries(your_executable nlm_logger)
endif()

if(ENABLE_DEBUG_UTILS)
    target_link_libraries(your_executable nlm_debug_utils)
endif()

if(ENABLE_REALTIME_MONITORING)
    target_link_libraries(your_executable nlm_monitor)
endif()

if(ENABLE_PYTHON_DEBUG_API)
    python3_add_target(python_bindings
        bindings.cpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/python
    )
    target_link_libraries(python_bindings your_executable)
endif()
```

## Common Issues and Solutions

### Issue 1: No log output appears

**Solution:**
1. Check that logging is enabled in your code
2. Verify log level settings
3. Ensure log files have write permissions
4. Check for log filtering rules

### Issue 2: Memory debugging causes performance degradation

**Solution:**
1. Disable memory debugging in production
2. Use custom allocators instead of tracking every allocation
3. Configure memory debugging with lower verbosity
4. Limit the number of tracked allocations

### Issue 3: Real-time monitoring uses too much CPU

**Solution:**
1. Increase sampling intervals
2. Disable non-essential monitoring
3. Use background threads for monitoring
4. Configure CPU and memory limits

### Issue 4: Python debug API not working

**Solution:**
1. Ensure Python bindings are properly built
2. Check that Python development libraries are installed
3. Verify module imports and paths
4. Run with verbose logging to debug issues

## Performance Tips

1. **Log Level Management:** Use appropriate log levels to reduce overhead
2. **Batch Logging:** Combine multiple log messages when possible
3. **Conditional Logging:** Only log when debugging or in development
4. **Asynchronous Logging:** Use thread-safe logging for production
5. **Structured Log Size:** Keep metadata and metrics sizes reasonable

## Testing and Validation

Run the comprehensive test suite:

```bash
./your_executable --test-logging
```

This will run:
- Structured logging functionality tests
- Performance metrics logging tests
- Real-time monitoring tests
- Debugging tools tests
- Error recovery tests
- Visualization tools tests
- Correlation tracking tests

All tests should pass successfully with the enhanced logging system.
