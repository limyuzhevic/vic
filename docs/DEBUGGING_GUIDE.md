# Debugging and Troubleshooting Guide

# Overview

This document provides comprehensive information about debugging and troubleshooting features in the enhanced NLM system, including common issues, debugging techniques, and troubleshooting guides.

## Key Improvements

### 1. Enhanced Debugging Tools

The enhanced system includes comprehensive debugging capabilities:

```python
import pynlm

# Create debug console with advanced features
debug_console = pynlm.DebugConsole()
debug_console.setLogLevel(pynlm.LogLevel.Debug)
debug_console.setOutputFile("debug_console.log")
debug_console.setMaxLogSize(10 * 1024 * 1024)  # 10MB

# Enable detailed tracing
brain.enableTraceLogging(True)
brain.setTraceLevel(pynlm.TraceLevel.Verbose)
brain.setTraceOutputFormat(pynlm.TraceFormat.JSON)

# Enable memory inspection
memory_inspector = pynlm.MemoryInspector()
brain.setMemoryInspector(memory_inspector)

# Enable performance monitoring
performance_monitor = pynlm.PerformanceMonitor()
performance_monitor.setSamplingInterval(0.1)  # 100ms
brain.setPerformanceMonitor(performance_monitor)
```

### 2. Comprehensive Error Reporting

```python
import pynlm

# Enable comprehensive error reporting
brain.enableErrorReporting(True)
brain.setErrorReportingLevel(pynlm.ErrorReportingLevel.Detailed)

# Get detailed error information
try:
    brain.initialize()
except pynlm.SimulationError as e:
    error_report = brain.getErrorReport()
    print(f"Error ID: {error_report.errorId}")
    print(f"Error Type: {error_report.errorType}")
    print(f"Error Message: {error_report.errorMessage}")
    print(f"Error Code: {error_report.errorCode}")
    print(f"Stack Trace: {error_report.stackTrace}")
    print(f"Context: {error_report.context}")
    print(f"Timestamp: {error_report.timestamp}")
    print(f"Additional Data: {error_report.additionalData}")
    print(f"Suggestions: {error_report.suggestions}")
```

### 3. Advanced Debugging Features

```python
import pynlm

# Create advanced debugging tools
class AdvancedDebugger:
    def __init__(self, brain):
        self.brain = brain
        self.debug_log = []
        self.memory_tracker = pynlm.MemoryTracker()
        self.performance_analyzer = pynlm.PerformanceAnalyzer()
    
    def startDebugging(self):
        """Start comprehensive debugging"""
        print("Starting advanced debugging...")
        
        # Enable all debugging features
        self.brain.enableTraceLogging(True)
        self.brain.enableMemoryLeakDetection(True)
        self.brain.enablePerformanceProfiling(True)
        
        # Initialize performance analyzer
        self.performance_analyzer.startProfiling()
        
        # Start memory tracking
        self.memory_tracker.startTracking()
    
    def stopDebugging(self):
        """Stop comprehensive debugging"""
        print("Stopping advanced debugging...")
        
        # Stop performance analyzer
        perf_stats = self.performance_analyzer.stopProfiling()
        
        # Stop memory tracking
        memory_stats = self.memory_tracker.stopTracking()
        
        # Generate debug report
        debug_report = self.generateDebugReport(perf_stats, memory_stats)
        
        return debug_report
    
    def generateDebugReport(self, perf_stats, memory_stats):
        """Generate comprehensive debug report"""
        report = {
            'timestamp': self.brain.getCurrentTime(),
            'performance': {
                'duration': perf_stats.duration,
                'steps_per_second': perf_stats.stepsPerSecond,
                'memory_usage': perf_stats.memoryUsage,
                'cpu_usage': perf_stats.cpuUsage,
                'log_rate': perf_stats.logRate
            },
            'memory': {
                'initial_memory': memory_stats.initialMemory,
                'peak_memory': memory_stats.peakMemory,
                'final_memory': memory_stats.finalMemory,
                'memory_increase': memory_stats.memoryIncrease,
                'leaks_detected': memory_stats.leaksDetected
            },
            'errors': self.brain.getRecentErrors(),
            'warnings': self.brain.getRecentWarnings(),
            'trace_entries': len(self.brain.getTraceEntries()),
            'suggestions': self.generateSuggestions(perf_stats, memory_stats)
        }
        
        return report
    
    def generateSuggestions(self, perf_stats, memory_stats):
        """Generate debugging suggestions"""
        suggestions = []
        
        # Performance suggestions
        if perf_stats.cpuUsage > 80:
            suggestions.append("High CPU usage detected. Consider optimizing simulation loop.")
        
        if perf_stats.memoryUsage > 512:
            suggestions.append("High memory usage detected. Consider optimizing memory allocation.")
        
        if perf_stats.logRate > 1000:
            suggestions.append("High log rate detected. Consider reducing log verbosity.")
        
        # Memory leak suggestions
        if memory_stats.leaksDetected:
            suggestions.append("Memory leaks detected. Check for unclosed resources.")
        
        if memory_stats.memoryIncrease > 200:
            suggestions.append("Significant memory increase detected. Check for memory leaks.")
        
        return suggestions

# Usage
advanced_debugger = AdvancedDebugger(brain)
advanced_debugger.startDebugging()

# Run simulation with debugging
brain.initialize()
for i in range(1000):
    brain.step(i)

# Stop debugging and get report
debug_report = advanced_debugger.stopDebugging()
print(f"Debug report generated: {debug_report}")
```

## Common Debugging Scenarios

### 1. Simulation Not Running

#### Problem
The simulation is not running or is stuck.

#### Symptoms
- No output during simulation
- Simulation appears to hang
- Long execution time for simple operations

#### Debugging Steps

```python
import pynlm
import time

def debugSimulationNotRunning(brain):
    print("Debugging simulation not running...")
    
    # Check if brain is initialized
    print("Checking brain initialization...")
    try:
        if not brain.isInitialized():
            print("Brain is not initialized")
            brain.initialize()
            print("Brain initialized successfully")
        else:
            print("Brain is already initialized")
    except pynlm.SimulationError as e:
        print(f"Brain initialization failed: {e}")
        return False
    
    # Check if simulation step is working
    print("Testing simulation step...")
    try:
        start_time = time.time()
        brain.step(0)
        end_time = time.time()
        
        if end_time - start_time > 5.0:
            print("Simulation step is too slow")
            return False
        else:
            print(f"Simulation step completed in {end_time - start_time:.2f} seconds")
    except pynlm.SimulationError as e:
        print(f"Simulation step failed: {e}")
        return False
    
    # Check brain state
    print("Checking brain state...")
    try:
        neuron_count = brain.getTotalNeuronCount()
        print(f"Total neurons: {neuron_count}")
        
        firing_neurons = brain.getFiringNeuronCount()
        print(f"Firing neurons: {firing_neurons}")
        
        if firing_neurons == 0:
            print("Warning: No neurons are firing. Check neuron configuration.")
    except pynlm.SimulationError as e:
        print(f"Brain state check failed: {e}")
        return False
    
    return True

# Usage
brain = pynlm.createEnhancedBrain(pynlm.createEnhancedConfig())
if debugSimulationNotRunning(brain):
    print("Simulation is working correctly")
else:
    print("Simulation has issues that need to be resolved")
```

### 2. Memory Issues

#### Problem
Memory usage is too high or memory leaks are detected.

#### Symptoms
- High memory usage
- Memory leaks detected
- System becomes unresponsive after running

#### Debugging Steps

```python
import pynlm
import psutil
import os

def debugMemoryIssues(brain):
    print("Debugging memory issues...")
    
    # Monitor memory usage
    process = psutil.Process(os.getpid())
    
    print("Initial memory usage:")
    initial_memory = process.memory_info().rss / 1024 / 1024  # MB
    print(f"  {initial_memory:.2f} MB")
    
    # Run simulation with memory tracking
    brain.initialize()
    
    memory_samples = []
    for i in range(100):
        brain.step(i)
        
        if i % 10 == 0:
            current_memory = process.memory_info().rss / 1024 / 1024
            memory_samples.append(current_memory)
            print(f"Step {i}: {current_memory:.2f} MB")
    
    # Analyze memory usage
    final_memory = process.memory_info().rss / 1024 / 1024
    memory_increase = final_memory - initial_memory
    
    print(f"\nMemory analysis:")
    print(f"  Initial memory: {initial_memory:.2f} MB")
    print(f"  Final memory: {final_memory:.2f} MB")
    print(f"  Memory increase: {memory_increase:.2f} MB")
    
    # Check for memory leaks
    if len(memory_samples) > 1:
        memory_trend = memory_samples[-1] - memory_samples[0]
        print(f"  Memory trend: {memory_trend:.2f} MB")
        
        if memory_trend > 50:
            print("  Warning: High memory increase - possible memory leak")
            
            # Check memory usage over time
            print("  Memory usage over time:")
            for i, mem in enumerate(memory_samples):
                print(f"    Step {i*10}: {mem:.2f} MB")
        
        else:
            print("  Memory usage is stable")
    
    # Force garbage collection
    import gc
    gc.collect()
    
    # Check memory after garbage collection
    after_gc_memory = process.memory_info().rss / 1024 / 1024
    print(f"\nAfter garbage collection:")
    print(f"  Memory usage: {after_gc_memory:.2f} MB")
    
    memory_after_gc = after_gc_memory - initial_memory
    
    if memory_after_gc > 30:
        print(f"  Warning: Memory still high after garbage collection: {memory_after_gc:.2f} MB")
        print("  Possible memory leak or large data structures")
    
    return memory_increase

# Usage
brain = pynlm.createEnhancedBrain(pynlm.createEnhancedConfig())
debugMemoryIssues(brain)
```

### 3. Performance Issues

#### Problem
Simulation performance is too slow or inefficient.

#### Symptoms
- Slow simulation execution
- High CPU usage
- Low steps per second

#### Debugging Steps

```python
import pynlm
import time

def debugPerformanceIssues(brain):
    print("Debugging performance issues...")
    
    # Benchmark simulation performance
    brain.initialize()
    
    # Measure simulation speed
    start_time = time.time()
    steps_completed = 0
    
    for i in range(1000):
        brain.step(i)
        steps_completed += 1
        
        # Measure performance every 100 steps
        if i % 100 == 0:
            current_time = time.time()
            steps_per_second = steps_completed / (current_time - start_time)
            print(f"Step {i}: {steps_per_second:.2f} steps/sec")
    
    # Calculate overall performance
    end_time = time.time()
    total_time = end_time - start_time
    average_steps_per_second = steps_completed / total_time
    
    print(f"\nPerformance analysis:")
    print(f"  Total steps: {steps_completed}")
    print(f"  Total time: {total_time:.2f} seconds")
    print(f"  Average steps per second: {average_steps_per_second:.2f}")
    
    # Check performance against expected values
    expected_steps_per_second = 1000  # Typical expectation
    if average_steps_per_second < expected_steps_per_second * 0.1:
        print(f"  Warning: Performance is {average_steps_per_second/expected_steps_per_second*100:.1f}% of expected")
        print("  Consider optimizing simulation performance")
    
    # Get detailed performance statistics
    perf_stats = brain.getPerformanceStats()
    print(f"\nDetailed performance statistics:")
    print(f"  Duration: {perf_stats.duration:.2f} seconds")
    print(f"  Steps per second: {perf_stats.stepsPerSecond:.2f}")
    print(f"  Memory usage: {perf_stats.memoryUsage:.2f} MB")
    print(f"  CPU usage: {perf_stats.cpuUsage:.2f}%")
    print(f"  Log rate: {perf_stats.logRate:.2f} logs/sec")
    
    return average_steps_per_second

# Usage
brain = pynlm.createEnhancedBrain(pynlm.createEnhancedConfig())
debugPerformanceIssues(brain)
```

### 4. Configuration Issues

#### Problem
Configuration is invalid or causing issues.

#### Symptoms
- Configuration loading errors
- Invalid parameter values
- Simulation configuration mismatches

#### Debugging Steps

```python
import pynlm

def debugConfigurationIssues(brain):
    print("Debugging configuration issues...")
    
    # Check configuration validity
    config = brain.getConfig()
    
    print("Checking configuration validity...")
    
    # Required keys
    required_keys = [
        "brain.neuron_count",
        "brain.connection_probability",
        "plasticity.enable"
    ]
    
    missing_keys = []
    for key in required_keys:
        if not config.has(key):
            missing_keys.append(key)
    
    if missing_keys:
        print(f"  Missing configuration keys: {missing_keys}")
    else:
        print("  All required configuration keys are present")
    
    # Validate key values
    print("\nValidating configuration values...")
    
    try:
        neuron_count = config.getOr<int>("brain.neuron_count", 0)
        print(f"  brain.neuron_count: {neuron_count}")
        
        if not (100 <= neuron_count <= 10000):
            print("  Error: brain.neuron_count must be between 100 and 10000")
        
        connection_probability = config.getOr<double>("brain.connection_probability", 0.0)
        print(f"  brain.connection_probability: {connection_probability}")
        
        if not (0.0 <= connection_probability <= 1.0):
            print("  Error: brain.connection_probability must be between 0.0 and 1.0")
        
        plasticity_enabled = config.getOr<bool>("plasticity.enable", false)
        print(f"  plasticity.enable: {plasticity_enabled}")
        
    except pynlm.ConfigurationError as e:
        print(f"  Configuration error: {e}")
    
    # Check for common configuration issues
    print("\nChecking for common configuration issues...")
    
    if config.has("brain.neuron_count"):
        neuron_count = config.getOr<int>("brain.neuron_count", 0)
        if neuron_count == 0:
            print("  Warning: brain.neuron_count is 0 - may cause simulation issues")
    
    if config.has("brain.connection_probability"):
        connection_probability = config.getOr<double>("brain.connection_probability", 0.0)
        if connection_probability == 0.0:
            print("  Warning: brain.connection_probability is 0.0 - may cause sparse connectivity")
        elif connection_probability > 0.5:
            print("  Warning: brain.connection_probability is high - may cause too many connections")
    
    return len(missing_keys) == 0

# Usage
brain = pynlm.createEnhancedBrain(pynlm.createEnhancedConfig())
debugConfigurationIssues(brain)
```

## Debugging Tools and Techniques

### 1. Debug Console

```python
import pynlm

# Create and configure debug console
debug_console = pynlm.DebugConsole()
debug_console.setLogLevel(pynlm.LogLevel.Debug)
debug_console.setOutputFile("debug.log")
debug_console.setMaxLogSize(10 * 1024 * 1024)  # 10MB

# Connect to brain
brain.setDebugConsole(debug_console)

# Use debug console
print("Debug console created and configured")
print(f"Log level: {debug_console.getLogLevel()}")
print(f"Output file: {debug_console.getOutputFile()}")
print(f"Max log size: {debug_console.getMaxLogSize()} bytes")
```

### 2. Memory Inspector

```python
import pynlm

# Create memory inspector
memory_inspector = pynlm.MemoryInspector()

# Attach to brain
brain.setMemoryInspector(memory_inspector)

# Start memory inspection
memory_inspector.startTracking()

# Run simulation with memory inspection
brain.initialize()
for i in range(1000):
    brain.step(i)

# Stop memory inspection
memory_stats = memory_inspector.stopTracking()

# Analyze memory statistics
print("Memory statistics:")
print(f"  Initial memory: {memory_stats.initialMemory} MB")
print(f"  Peak memory: {memory_stats.peakMemory} MB")
print(f"  Final memory: {memory_stats.finalMemory} MB")
print(f"  Memory increase: {memory_stats.memoryIncrease} MB")
print(f"  Leaks detected: {memory_stats.leaksDetected}")

# Get detailed memory information
memory_details = memory_inspector.getMemoryDetails()
print("\nMemory details:")
for category, details in memory_details.items():
    print(f"  {category}:")
    print(f"    Count: {details.count}")
    print(f"    Size: {details.size} bytes")
    print(f"    Percentage: {details.percentage:.2f}%")
```

### 3. Performance Monitor

```python
import pynlm

# Create performance monitor
performance_monitor = pynlm.PerformanceMonitor()
performance_monitor.setSamplingInterval(0.1)  # 100ms

# Attach to brain
brain.setPerformanceMonitor(performance_monitor)

# Start performance monitoring
performance_monitor.startMonitoring()

# Run simulation with performance monitoring
brain.initialize()
for i in range(1000):
    brain.step(i)

# Stop performance monitoring
perf_stats = performance_monitor.stopMonitoring()

# Analyze performance statistics
print("Performance statistics:")
print(f"  Duration: {perf_stats.duration:.2f} seconds")
print(f"  Steps per second: {perf_stats.stepsPerSecond:.2f}")
print(f"  Memory usage: {perf_stats.memoryUsage:.2f} MB")
print(f"  CPU usage: {perf_stats.cpuUsage:.2f}%")
print(f"  Log rate: {perf_stats.logRate:.2f} logs/sec")

# Get detailed performance data
perf_data = performance_monitor.getPerformanceData()
print("\nPerformance data:")
for timestamp, data in perf_data.items():
    print(f"  {timestamp}:")
    print(f"    CPU: {data.cpuUsage:.2f}%")
    print(f"    Memory: {data.memoryUsage:.2f} MB")
    print(f"    Logs: {data.logRate:.2f} logs/sec")
```

### 4. Trace Analyzer

```python
import pynlm

# Create trace analyzer
trace_analyzer = pynlm.TraceAnalyzer()

# Attach to brain
brain.setTraceAnalyzer(trace_analyzer)

# Start trace analysis
brain.enableTraceLogging(True)
brain.setTraceLevel(pynlm.TraceLevel.Verbose)

# Run simulation with tracing
brain.initialize()
for i in range(100):
    brain.step(i)

# Stop trace analysis
trace_stats = trace_analyzer.stopAnalysis()

# Analyze trace statistics
print("Trace statistics:")
print(f"  Total entries: {trace_stats.totalEntries}")
print(f"  Trace size: {trace_stats.traceSize} bytes")
print(f"  Trace duration: {trace_stats.traceDuration:.2f} seconds")
print(f"  Average entry size: {trace_stats.averageEntrySize:.2f} bytes")

# Get detailed trace information
trace_details = trace_analyzer.getTraceDetails()
print("\nTrace details:")
for i, entry in enumerate(trace_details):
    if i < 10:  # Limit output
        print(f"  Entry {i}:")
        print(f"    Time: {entry.timestamp:.6f}")
        print(f"    Type: {entry.type}")
        print(f"    Data: {entry.data}")
    else:
        break
```

## Troubleshooting Checklist

### Pre-Simulation Checks

1. **Configuration Validation**
   - Check if configuration file exists
   - Validate configuration keys
   - Check parameter values

2. **System Resources**
   - Check available memory
   - Check disk space
   - Check network connectivity

3. **Brain Initialization**
   - Verify brain configuration
   - Check neuron count
   - Validate connection probability

### During Simulation Checks

1. **Performance Monitoring**
   - Monitor CPU usage
   - Monitor memory usage
   - Monitor log rate

2. **Error Detection**
   - Check for simulation errors
   - Monitor for exceptions
   - Log errors and warnings

3. **State Validation**
   - Check neuron states
   - Verify synapse states
   - Validate memory usage

### Post-Simulation Checks

1. **Result Validation**
   - Check simulation results
   - Validate performance metrics
   - Verify memory usage

2. **Error Analysis**
   - Analyze errors and warnings
   - Check for patterns
   - Identify root causes

3. **Optimization**
   - Optimize configuration
   - Adjust performance parameters
   - Improve error handling

## Conclusion

Comprehensive debugging and troubleshooting are essential for maintaining and optimizing the NLM system. The enhanced system provides extensive tools for debugging, including:

1. **Advanced Debugging Tools**: Debug console, memory inspector, performance monitor, trace analyzer
2. **Error Reporting**: Detailed error messages with comprehensive context
3. **Performance Monitoring**: Real-time monitoring of system resources
4. **Memory Management**: Advanced memory management and leak detection
5. **Troubleshooting**: Step-by-step debugging and troubleshooting guides

These improvements make the NLM system more reliable, maintainable, and user-friendly, enabling users to easily identify and resolve issues.

## Additional Resources

### Debugging Tools

- **debug_console.py**: Debug console for interactive debugging
- **memory_inspector.py**: Memory inspector for memory analysis
- **performance_monitor.py**: Performance monitor for performance analysis
- **trace_analyzer.py**: Trace analyzer for detailed performance analysis

### Related Documentation

- **ENHANCED_ERROR_HANDLING.md**: Error handling documentation
- **CONFIG_MANAGEMENT.md**: Configuration management documentation
- **PERFORMANCE_MONITORING.md**: Performance monitoring documentation
- **ENHANCED_DOCUMENTATION.md**: Enhanced documentation

---

*This document covers the debugging and troubleshooting features and improvements made to NLM. For specific questions or additional information, please refer to the individual documentation files.*