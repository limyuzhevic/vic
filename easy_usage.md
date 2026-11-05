# NLM Advanced Logging and Debugging

This document provides an overview of the advanced logging and debugging capabilities in the NLM (Neural Learning Machine) project.

## Overview

The NLM logging system has been significantly enhanced to provide comprehensive monitoring and debugging tools for advanced users. The new system includes:

- **Performance timing utilities** for measuring operation durations
- **Event tracing** for tracking complex workflows and debugging interactions
- **Memory profiling** for monitoring memory usage and allocations
- **Structured logging** with correlation IDs and detailed context information
- **Multiple output formats** (console, JSON, structured)

## Key Components

### 1. Timer Class

The Timer class provides automatic performance measurement of code operations.

**Usage:**
```cpp
// Time a specific operation
auto result = Logger::getGlobal().timeOperation("Neural processing", LogLevel::Info, []() {
    // Your code here
    return 42;
});

// Manual timing with correlation ID
Timer timer("Database query", LogLevel::Info, "request-123");
// ... your code ...
// Timer automatically logs on destruction
timer.stop();
timer.resume();
```

**Features:**
- Automatic timing with pause/resume support
- Correlation ID for log correlation
- Structured logging output
- Can be used with any callable function

### 2. EventTracer Class

The EventTracer class provides structured event tracing for complex workflows.

**Usage:**
```cpp
// Create an event tracer
auto tracer = Logger::getGlobal().traceEvent("Data processing workflow", "workflow-456");

// Add events
tracer.addEvent("Input received", "500MB of data");
tracer.addCheckpoint("Parsing started");
tracer.addEvent("Parsing completed", "200 files processed");
tracer.addCheckpoint("Analysis started");
tracer.addEvent("Analysis completed", "95% confidence");

// Set context for the entire trace
tracer.setContext("user_id", "user789");
tracer.setContext("priority", "high");
```

**Features:**
- Hierarchical event tracking
- Checkpoints and progress monitoring
- Context propagation across events
- Structured event information

### 3. MemoryProfiler Class

The MemoryProfiler class provides memory tracking and profiling capabilities.

**Usage:**
```cpp
// Create a memory profiler
auto profiler = Logger::getGlobal().profileMemory("Neural network training", "mem-prof-789");

// Track allocations
profiler.trackAllocation(1024, "neuron initialization");
profiler.trackAllocation(2048, "synapse creation");

// Track object creation/destruction
profiler.trackObjectCreation("Neuron", 1000);
profiler.trackObjectDestruction("Synapse", 500);

// Track deallocations
profiler.trackDeallocation(1024);
```

**Features:**
- Allocation/deallocation tracking
- Object lifecycle monitoring
- Peak memory usage tracking
- Detailed memory reports

### 4. Enhanced Logger Class

The Logger class now supports advanced logging capabilities:

**Key Features:**
- **Correlation IDs**: Track related log messages across different components
- **Structured Data**: Add custom key-value pairs to log entries
- **Performance Logging**: Built-in timing and memory logging
- **Multiple Formats**: Console, JSON, and structured output
- **Filtering**: Advanced filtering based on level, correlation ID, or custom criteria

**Example:**
```cpp
Logger::getGlobal().setLevel(LogLevel::Info);

// Performance logging
Logger::getGlobal().logPerformance("Neural processing", 125.5, "req-123");

// Memory allocation logging
Logger::getGlobal().logMemoryAllocation(4096, "buffer allocation", "req-123");

// Event logging
Logger::getGlobal().logEvent("User authentication", "Success", "auth-456");
```

## Quick Start Guide

### 1. Basic Usage

```cpp
#include "Logger.hpp"

int main() {
    // Initialize global logger
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    // Add console logger (automatically added by default)
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    consoleLogger->setLevel(LogLevel::Info);
    Logger::getGlobal().addLogger(consoleLogger);
    
    // Simple logging
    NLM_LOG_INFO("Application started");
    
    // Performance timing
    auto timer = Logger::getGlobal().timeOperation("Database query", []() {
        // Your code here
        return queryResults;
    });
    
    return 0;
}
```

### 2. Performance Monitoring

```cpp
// Time a specific operation
auto result = Logger::getGlobal().timeOperation("Data processing", LogLevel::Info, []() {
    // Your data processing code here
    return processedData;
});

// Manual timer with correlation ID
Timer timer("Database transaction", LogLevel::Info, "tx-123");
// ... your code ...
// Timer automatically logs on destruction
```

### 3. Event Tracing

```cpp
// Create an event trace
auto tracer = Logger::getGlobal().traceEvent("User authentication workflow", "auth-456");
tracer.setContext("ip_address", "192.168.1.100");
tracer.setContext("user_agent", "Mozilla/5.0");

// Add events throughout the workflow
tracer.addEvent("Login request received");
tracer.addCheckpoint("Credential validation");
tracer.addEvent("Authentication successful");
```

### 4. Memory Profiling

```cpp
// Create memory profiler
auto profiler = Logger::getGlobal().profileMemory("Neural network training", "mem-prof-789");

// Track allocations
profiler.trackAllocation(1024, "neuron weights");
profiler.trackAllocation(2048, "synapse matrix");

// Track object lifecycle
profiler.trackObjectCreation("Neuron", 1000);
profiler.trackObjectCreation("Synapse", 5000);
```

## Configuration

### Log Levels

- **Debug** (0): Detailed debugging information
- **Info** (1): General information messages
- **Warning** (2): Warning messages
- **Error** (3): Error messages
- **Critical** (4): Critical error messages

### Setting Log Level

```cpp
Logger::getGlobal().setLevel(LogLevel::Info);
```

### Adding Loggers

```cpp
// Add console logger
auto consoleLogger = std::make_shared<ConsoleLogger>();
consoleLogger->setLevel(LogLevel::Info);
consoleLogger->setUseColors(true);
Logger::getGlobal().addLogger(consoleLogger);

// Add file logger
consoleLogger->setOutputFile("nlm.log");
```

## Examples

### Full Pipeline Example

This example demonstrates all advanced logging features together:

```cpp
void processUserRequest(const std::string& userId) {
    // Create correlation ID for the entire request
    std::string correlationId = "req-" + userId + "-" + std::to_string(std::time(nullptr));
    
    // Performance timing for the entire pipeline
    auto pipelineTimer = Logger::getGlobal().timeOperation("User request pipeline", correlationId);
    
    // Event tracing for the request lifecycle
    auto tracer = Logger::getGlobal().traceEvent("Request processing", correlationId);
    tracer.setContext("user_id", userId);
    tracer.setContext("request_type", "API_CALL");
    
    // Memory profiling for the request
    auto profiler = Logger::getGlobal().profileMemory("Request processing", correlationId);
    
    try {
        // Log request start
        tracer.addEvent("Request received");
        
        // Simulate authentication
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        tracer.addCheckpoint("Authentication completed");
        
        // Simulate data processing
        profiler.trackAllocation(4096, "request buffer");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        tracer.addEvent("Data processing completed");
        
        // Simulate database operations
        profiler.trackAllocation(2048, "query results");
        std::this_thread::sleep_for(std::chrono::milliseconds(75));
        tracer.addCheckpoint("Database operations");
        
        // Simulate response generation
        profiler.trackDeallocation(4096);
        tracer.addEvent("Response generated");
        
        // Log performance summary
        Logger::getGlobal().logPerformance("Request processing", pipelineTimer.getDurationMs(), correlationId);
        
    } catch (const std::exception& e) {
        // Log error
        Logger::getGlobal().logError(std::string("Request processing failed: ") + e.what());
        tracer.addEvent("Error: " + std::string(e.what()));
        throw;
    }
}
```

## Integration with Existing Code

The enhanced logging system is fully backward compatible with existing code. The existing logging macros continue to work:

```cpp
NLM_LOG_INFO("This still works");
NLM_LOG_ERROR("This also works");
```

New code can use the enhanced features while existing code continues to function without modification.

## Benefits

1. **Better Debugging**: Easy correlation of log messages across different components
2. **Performance Insights**: Automatic timing and performance monitoring
3. **Memory Management**: Comprehensive memory tracking and profiling
4. **Workflow Tracing**: Structured event tracing for complex operations
5. **Production Ready**: Multiple output formats and filtering capabilities
6. **Developer Friendly**: Simple API for advanced debugging features

## Next Steps

1. **Enable advanced logging**: Set appropriate log levels for your use case
2. **Add correlation IDs**: Use correlation IDs to track related log messages
3. **Implement timing**: Add timing to critical operations
4. **Profile memory**: Monitor memory usage in performance-critical sections
5. **Trace events**: Use event tracing for complex workflows and debugging

The enhanced logging system provides powerful tools for debugging, monitoring, and understanding the behavior of your NLM applications, making it easier to develop, test, and maintain complex neural network applications.