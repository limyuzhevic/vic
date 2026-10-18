# Performance Monitoring and Debugging Guide

# Overview

This document provides comprehensive information about performance monitoring, debugging, and troubleshooting features in the enhanced NLM system.

## Key Improvements

### 1. Comprehensive Performance Monitoring

The enhanced system includes comprehensive performance monitoring capabilities:

```python
import pynlm

# Create brain with monitoring enabled
config = pynlm.createEnhancedConfig()\
    .set("performance.enable_monitoring", True)\
    .set("performance.log_rate_limit", 10000)\
    .set("performance.memory_limit_mb", 1024)

brain = pynlm.createEnhancedBrain(config)
brain.initialize()
```

### 2. Advanced Logging

```python
import pynlm

# Configure enhanced logging
logger = pynlm.Logger.getGlobal()
logger.setLevel(pynlm.LogLevel.Info)
logger.setUseColors(True)

# Set up file logging with automatic directory creation
logger.setOutputFile("/var/log/nlm.log")
```

### 3. Performance Statistics

```python
import pynlm

# Get performance statistics
stats = brain.getPerformanceStats()
print(f"Log rate: {stats.logRate} logs/sec")
print(f"Memory usage: {stats.memoryUsage} MB")
print(f"CPU usage: {stats.cpuUsage}%")
print(f"Active neurons: {stats.activeNeuronCount}")
print(f"Total spikes: {stats.totalSpikeCount}")
print(f"Average firing rate: {stats.averageFiringRate}")
```

## Performance Monitoring Features

### 1. Log Rate Monitoring

```python
import pynlm

# Monitor log rate
logger = pynlm.Logger.getGlobal()

# Get current log rate
log_rate = logger.getLogRate()
print(f"Current log rate: {log_rate} logs/sec")

# Check if log rate exceeds threshold
if log_rate > logger.getLogRateLimit():
    print("Warning: Log rate exceeded threshold")

# Reset log rate
logger.resetLogRate()
```

### 2. Memory Monitoring

```python
import pynlm

# Monitor memory usage
memory_usage = brain.getMemoryUsage()
print(f"Memory usage: {memory_usage} MB")

# Check memory threshold
memory_limit = brain.getMemoryLimit()
if memory_usage > memory_limit:
    print(f"Warning: Memory usage exceeded limit ({memory_usage} > {memory_limit} MB)")

# Get memory statistics
memory_stats = brain.getMemoryStats()
print(f"Heap memory: {memory_stats.heapMemory} MB")
print(f"Stack memory: {memory_stats.stackMemory} MB")
print(f"Object count: {memory_stats.objectCount}")
```

### 3. CPU Monitoring

```python
import pynlm

# Monitor CPU usage
cpu_usage = brain.getCPUUsage()
print(f"CPU usage: {cpu_usage}%")

# Check CPU threshold
cpu_limit = brain.getCPULimit()
if cpu_usage > cpu_limit:
    print(f"Warning: CPU usage exceeded limit ({cpu_usage} > {cpu_limit}%)")
```

### 4. System Resource Monitoring

```python
import pynlm

# Get system resource statistics
system_stats = brain.getSystemStats()
print(f"CPU cores: {system_stats.cpuCores}")
print(f"Memory total: {system_stats.memoryTotal} MB")
print(f"Memory available: {system_stats.memoryAvailable} MB")
print(f"Disk space: {system_stats.diskSpace} MB")
print(f"Network connections: {system_stats.networkConnections}")
```

## Debugging Features

### 1. Enhanced Error Messages

```python
import pynlm

# Get detailed error information
try:
    brain.initialize()
except pynlm.SimulationError as e:
    error_info = brain.getLastErrorInfo()
    print(f"Error type: {error_info.errorType}")
    print(f"Error message: {error_info.errorMessage}")
    print(f"Error code: {error_info.errorCode}")
    print(f"Stack trace: {error_info.stackTrace}")
    print(f"Context: {error_info.context}")
    print(f"Timestamp: {error_info.timestamp}")
    print(f"Additional data: {error_info.additionalData}")
```

### 2. Trace Logging

```python
import pynlm

# Enable detailed trace logging
brain.enableTraceLogging(True)
brain.setTraceLevel(pynlm.TraceLevel.Verbose)

# Run simulation with tracing
brain.step(0)
brain.step(1)
brain.step(2)

# Get trace information
trace_info = brain.getTraceInfo()
print(f"Trace entries: {len(trace_info.entries)}")
print(f"Trace size: {trace_info.size} bytes")
print(f"Trace duration: {trace_info.duration} seconds")

# Save trace to file
brain.saveTrace("simulation_trace.log")
```

### 3. Memory Leak Detection

```python
import pynlm

# Enable memory leak detection
brain.enableMemoryLeakDetection(True)

# Run simulation
brain.initialize()
for i in range(1000):
    brain.step(i)

# Check for memory leaks
leak_info = brain.getMemoryLeakInfo()
if leak_info.hasLeaks:
    print(f"Memory leak detected: {leak_info.leakSize} bytes")
    print(f"Leak source: {leak_info.leakSource}")
    print(f"Leak count: {leak_info.leakCount}")
    print(f"Recommended action: {leak_info.recommendedAction}")
else:
    print("No memory leaks detected")
```

### 4. Performance Profiling

```python
import pynlm

# Enable performance profiling
brain.enablePerformanceProfiling(True)

# Run simulation with profiling
brain.initialize()
start_time = brain.getCurrentTime()

for i in range(1000):
    brain.step(i)

end_time = brain.getCurrentTime()

# Get performance statistics
perf_stats = brain.getPerformanceStats()
print(f"Simulation duration: {perf_stats.duration} seconds")
print(f"Steps per second: {perf_stats.stepsPerSecond}")
print(f"Memory usage: {perf_stats.memoryUsage} MB")
print(f"CPU usage: {perf_stats.cpuUsage}%")

# Get detailed performance breakdown
perf_breakdown = brain.getPerformanceBreakdown()
for operation, stats in perf_breakdown.items():
    print(f"{operation}: {stats.count} calls, {stats.totalTime} seconds, {stats.averageTime} seconds/call")
```

## Debugging Tools

### 1. Debug Console

```python
import pynlm

# Create debug console
debug_console = pynlm.DebugConsole()
debug_console.setLogLevel(pynlm.LogLevel.Debug)
debug_console.setOutputFile("debug_console.log")

# Connect to brain
brain.setDebugConsole(debug_console)

# Run simulation with debugging
brain.initialize()
brain.step(0)
brain.step(1)
brain.step(2)

# Get debug information
debug_info = debug_console.getDebugInfo()
print(f"Debug entries: {len(debug_info.entries)}")
print(f"Debug level: {debug_info.level}")
print(f"Debug timestamp: {debug_info.timestamp}")
```

### 2. Memory Inspector

```python
import pynlm

# Create memory inspector
memory_inspector = pynlm.MemoryInspector()

# Attach to brain
brain.setMemoryInspector(memory_inspector)

# Run simulation
brain.initialize()
for i in range(100):
    brain.step(i)

# Inspect memory
memory_report = memory_inspector.getMemoryReport()
print(f"Memory usage: {memory_report.memoryUsage} MB")
print(f"Object count: {memory_report.objectCount}")
print(f"Object distribution: {memory_report.objectDistribution}")

# Get memory statistics
memory_stats = memory_inspector.getMemoryStats()
for category, stats in memory_stats.items():
    print(f"{category}: {stats.count} objects, {stats.size} bytes")
```

### 3. Performance Monitor

```python
import pynlm

# Create performance monitor
performance_monitor = pynlm.PerformanceMonitor()
performance_monitor.setSamplingInterval(0.1)  # 100ms

# Attach to brain
brain.setPerformanceMonitor(performance_monitor)

# Run simulation with monitoring
brain.initialize()
for i in range(1000):
    brain.step(i)

# Get performance report
perf_report = performance_monitor.getPerformanceReport()
print(f"Duration: {perf_report.duration} seconds")
print(f"Samples: {perf_report.samples}")
print(f"Average CPU usage: {perf_report.averageCPUUsage}%")
print(f"Average memory usage: {perf_report.averageMemoryUsage} MB")
print(f"Average log rate: {perf_report.averageLogRate} logs/sec")

# Get detailed performance data
perf_data = performance_monitor.getPerformanceData()
for timestamp, data in perf_data.items():
    print(f"{timestamp}: CPU={data.cpuUsage}%, Memory={data.memoryUsage}MB, Logs={data.logRate}")
```

## Troubleshooting Guide

### 1. Common Issues

#### Issue: High Memory Usage

```python
# Problem: Memory usage is too high
# Solution: Optimize memory usage

def optimizeMemoryUsage(brain):
    # Enable memory pools
    brain.enableMemoryPools(True)
    
    # Clear unused memory
    brain.forceGarbageCollection()
    
    # Reduce memory allocation
    brain.setMemoryAllocationLimit(512)  # 512MB limit
    
    return brain
```

#### Issue: Slow Simulation Speed

```python
# Problem: Simulation is too slow
# Solution: Optimize performance

def optimizeSimulationSpeed(brain):
    # Enable performance optimizations
    brain.enableSIMD(true)
    brain.enableVectorization(true)
    
    # Reduce logging overhead
    logger = pynlm.Logger.getGlobal()
    logger.setLogLevel(pynlm.LogLevel.Warning)
    
    # Use memory pools
    brain.enableMemoryPools(True)
    
    return brain
```

#### Issue: Log File Too Large

```python
# Problem: Log file is too large
# Solution: Enable log rotation

def configureLogRotation(brain):
    # Enable log rotation
    logger = pynlm.Logger.getGlobal()
    logger.enableLogRotation(10)  # Rotate after 10MB
    
    # Set log file size limit
    logger.setLogFileSizeLimit(10 * 1024 * 1024)  # 10MB
    
    return brain
```

### 2. Debugging Techniques

#### Technique 1: Isolate Problematic Components

```python
import pynlm

# Test individual components
def testComponent(component_name, test_function):
    try:
        print(f"Testing {component_name}...")
        result = test_function()
        print(f"{component_name} test passed")
        return result
    except Exception as e:
        print(f"{component_name} test failed: {e}")
        return None

# Test brain initialization
testBrain = lambda: pynlm.createEnhancedBrain(pynlm.createEnhancedConfig())

# Test world simulation
brain = testBrain()
if brain:
    world = pynlm.createEnhancedWorld()
    world_test = lambda: world.initialize()
    testComponent("World", world_test)

# Test agent functionality
if brain:
    agent = pynlm.createEnhancedAgent(brain)
    agent_test = lambda: agent.initialize(world)
    testComponent("Agent", agent_test)
```

#### Technique 2: Use Debug Output

```python
import pynlm

# Enable debug output
logger = pynlm.Logger.getGlobal()
logger.setLevel(pynlm.LogLevel.Debug)

# Create brain with debug configuration
config = pynlm.createEnhancedConfig()\
    .set("performance.enable_monitoring", True)\
    .set("brain.neuron_count", 100)  # Small brain for testing

brain = pynlm.createEnhancedBrain(config)

# Run simulation with debug output
print("Starting simulation...")
brain.initialize()
print("Brain initialized")

for i in range(5):
    print(f"Step {i}...")
    brain.step(i)
    print(f"Step {i} completed")

print("Simulation completed")
```

#### Technique 3: Use Memory Analysis

```python
import pynlm
import psutil
import os

# Monitor memory usage during simulation
def monitorMemoryDuringSimulation(brain, duration):
    process = psutil.Process(os.getpid())
    
    print("Starting memory monitoring...")
    initial_memory = process.memory_info().rss / 1024 / 1024  # MB
    
    brain.initialize()
    
    for i in range(duration):
        brain.step(i)
        
        current_memory = process.memory_info().rss / 1024 / 1024  # MB
        memory_increase = current_memory - initial_memory
        
        print(f"Step {i}: Memory usage = {current_memory:.2f} MB (increase: {memory_increase:.2f} MB)")
        
        # Check for memory issues
        if memory_increase > 100:  # More than 100MB increase
            print(f"Warning: High memory increase detected: {memory_increase:.2f} MB")
        
        # Force garbage collection every 100 steps
        if i % 100 == 0:
            import gc
            gc.collect()
            current_memory = process.memory_info().rss / 1024 / 1024
            print(f"After garbage collection: {current_memory:.2f} MB")
    
    final_memory = process.memory_info().rss / 1024 / 1024
    total_increase = final_memory - initial_memory
    
    print(f"Final memory usage: {final_memory:.2f} MB")
    print(f"Total memory increase: {total_increase:.2f} MB")
    
    if total_increase > 200:  # More than 200MB total increase
        print("Warning: Significant memory increase detected - possible memory leak")
    
    return total_increase

# Usage
brain = pynlm.createEnhancedBrain(pynlm.createEnhancedConfig())
monitorMemoryDuringSimulation(brain, 100)
```

### 3. Common Error Messages

#### Error: "Configuration file not found"

```python
# Problem: Configuration file not found
# Solution: Check file path and permissions

def loadConfigurationWithFallback(config_path):
    try:
        config = pynlm.createEnhancedConfig(config_path)
        print(f"Successfully loaded configuration from {config_path}")
        return config
    except pynlm.ConfigurationError:
        print(f"Configuration file {config_path} not found")
        print("Using default configuration")
        return pynlm.createEnhancedConfig()
```

#### Error: "Memory allocation failed"

```python
# Problem: Memory allocation failed
# Solution: Optimize memory usage

def optimizeForMemory(brain):
    # Reduce memory allocation
    brain.setMemoryAllocationLimit(256)  # 256MB limit
    
    # Enable memory pools
    brain.enableMemoryPools(True)
    
    # Clear unused memory
    brain.forceGarbageCollection()
    
    return brain
```

#### Error: "Log file write failed"

```python
# Problem: Log file write failed
# Solution: Check file permissions

def setupLoggingWithFallback():
    try:
        logger = pynlm.Logger.getGlobal()
        logger.setOutputFile("/var/log/nlm.log")
        print("Successfully configured log file")
    except Exception as e:
        print(f"Failed to configure log file: {e}")
        print("Using console logging only")
        # Continue with default logging
```

## Performance Optimization

### 1. Memory Optimization

```python
import pynlm

# Optimize memory usage
def optimizeMemory(brain):
    # Enable memory pools
    brain.enableMemoryPools(True)
    
    # Set memory limits
    brain.setMemoryLimit(512)  # 512MB limit
    
    # Enable garbage collection
    brain.enableAutomaticGarbageCollection(True)
    
    # Use memory-efficient data structures
    brain.useEfficientDataStructures(True)
    
    return brain

# Usage
brain = optimizeMemory(brain)
```

### 2. CPU Optimization

```python
import pynlm

# Optimize CPU usage
def optimizeCPU(brain):
    # Enable SIMD instructions
    brain.enableSIMD(true)
    
    # Enable vectorization
    brain.enableVectorization(true)
    
    # Reduce context switching
    brain.setThreadCount(1)
    
    # Optimize for single-threaded performance
    brain.optimizeForSingleThread()
    
    return brain

# Usage
brain = optimizeCPU(brain)
```

### 3. I/O Optimization

```python
import pynlm

# Optimize I/O operations
def optimizeIO(brain):
    # Enable buffered I/O
    brain.enableBufferedIO(true)
    
    # Reduce disk I/O
    brain.setSyncInterval(60)  # Sync every 60 seconds
    
    # Use asynchronous I/O
    brain.enableAsyncIO(true)
    
    return brain

# Usage
brain = optimizeIO(brain)
```

## Conclusion

Performance monitoring and debugging are essential for maintaining and optimizing the NLM system. The enhanced system provides comprehensive tools for monitoring, debugging, and optimizing performance, including:

1. **Performance Monitoring**: Real-time monitoring of system resources
2. **Error Handling**: Comprehensive error handling with detailed diagnostics
3. **Memory Management**: Advanced memory management and leak detection
4. **Performance Optimization**: Tools for optimizing CPU, memory, and I/O performance
5. **Debugging**: Comprehensive debugging tools and techniques

These improvements make the NLM system more reliable, efficient, and user-friendly, enabling users to easily identify and resolve performance issues.

## Additional Resources

### Debugging Tools

- **debug_console.py**: Debug console for interactive debugging
- **memory_inspector.py**: Memory inspector for memory analysis
- **performance_monitor.py**: Performance monitor for performance analysis
- **trace_analyzer.py**: Trace analyzer for detailed performance analysis

### Related Documentation

- **ENHANCED_ERROR_HANDLING.md**: Error handling documentation
- **CONFIG_MANAGEMENT.md**: Configuration management documentation
- **ENHANCED_DOCUMENTATION.md**: Enhanced documentation

---

*This document covers the performance monitoring and debugging features and improvements made to NLM. For specific questions or additional information, please refer to the individual documentation files.*