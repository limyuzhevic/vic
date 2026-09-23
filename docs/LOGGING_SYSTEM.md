# NLM Logging System Documentation

## Overview

The NLM (Neural Learning Machine) logging system provides a comprehensive, modular logging framework designed for large-scale neural simulations. It supports component-based logging, hierarchical structures, and multiple output formats with performance optimization for production use.

## Key Features

### 1. Component-Based Logging
- **Hierarchical Logger System**: Loggers organized by functional components (e.g., `brain.synaptic`, `brain.neuron`, `agent.bridging`)
- **Configurable Log Levels**: DEBUG, INFO, WARNING, ERROR, FATAL for each component
- **Selective Component Logging**: Enable/disable logging per component independently

### 2. Multiple Output Formats
- **Console Output**: Human-readable formatted messages with timestamps
- **File Output**: Structured log files with rotation and compression support
- **Custom Output**: Extensible output system for integration with external logging infrastructure

### 3. Performance Optimized
- **Buffered Output**: Minimizes I/O operations through batching
- **Zero-Allocation Logging**: Pre-allocated buffers to avoid memory allocation overhead
- **Thread-Safe Design**: Lock-free operations for high-concurrency scenarios
- **Configurable Verbosity**: Dynamic adjustment based on runtime conditions

## Architecture

### Logger Hierarchy

```
NLM Logging Architecture

┌─────────────────────────────────────────────────────────┐
│                    Global Logger                        │
├─────────────────────────────────────────────────────────┤
│  ┌─────────────────┐  ┌─────────────────┐            │
│  │  ComponentLog   │  │   AsyncWriter   │            │
│  │   (Hierarchical)│  │   (Buffered)    │            │
│  └─────────────────┘  └─────────────────┘            │
│          │                      │                   │
│   ┌──────▼──────┐      ┌────────▼────────┐           │
│   │   brain     │      │   world         │           │
│   │   ┌────────┐│      │   ┌────────────┐│           │
│   │   │ neuron  ││      │   │ sensory    ││           │
│   │   │ synaptic││      │   │ motor      ││           │
│   │   │ memory  ││      │   │ reward     ││           │
│   │   └────────┘│      │   └────────────┘│           │
│   └─────────────┘      └─────────────────┘           │
└─────────────────────────────────────────────────────────┘
```

### Core Components

#### 1. ComponentLogger.hpp
**Purpose**: Hierarchical logger with component-based organization

**Key Features**:
- **Component Hierarchy**: `brain.neuron`, `brain.synaptic`, `brain.memory`, etc.
- **Log Level Control**: Independent level setting per component
- **Pattern Matching**: Wildcard support for component selection
- **Parent-Child Relationships**: Inherited log settings

```cpp
// Component hierarchy example
brain.neuron.debug = true    // Enable debug for all neurons
brain.synaptic.info = true  // Enable info for all synapses
agent.*.warning = true       // Enable warning for all agent components
```

#### 2. HierarchicalLogger.hpp
**Purpose**: Manages hierarchical logger relationships and inheritance

**Key Features**:
- **Automatic Hierarchy Creation**: Based on component naming patterns
- **Inheritance Rules**: Child components inherit parent settings unless overridden
- **Circular Dependency Detection**: Prevents logging deadlocks
- **Dynamic Hierarchy Adjustment**: Runtime hierarchy modifications

#### 3. AsyncWriter.hpp
**Purpose**: Asynchronous log output with buffering

**Key Features**:
- **Thread-Pool-Based**: Dedicated threads for I/O operations
- **Circular Buffer**: Efficient memory usage with zero copying
- **Flushing Strategy**: Configurable flush intervals and thresholds
- **Backpressure Handling**: Automatic throttling when output is slow

#### 4. LoggerFactory.hpp
**Purpose**: Centralized logger creation and configuration

**Key Features**:
- **Singleton Pattern**: Single access point for all loggers
- **Configuration-Driven**: Load from config files or runtime settings
- **Component Registry**: Dynamic component discovery
- **Template Specialization**: Common logger patterns predefined

## Configuration

### Logging Configuration Schema

```json
{
  "logging": {
    "enabled": true,
    "level": "INFO",
    "components": {
      "brain": {
        "level": "DEBUG",
        "children": {
          "neuron": { "level": "DEBUG" },
          "synaptic": { "level": "INFO" },
          "memory": { "level": "WARNING" }
        }
      },
      "world": {
        "level": "INFO",
        "children": {
          "sensory": { "level": "DEBUG" },
          "motor": { "level": "DEBUG" }
        }
      },
      "agent": {
        "level": "INFO",
        "children": {
          "bridging": { "level": "INFO" },
          "neuromodulation": { "level": "DEBUG" }
        }
      }
    },
    "outputs": {
      "console": {
        "enabled": true,
        "format": "[%(timestamp)s] [%(level)s] %(component)s: %(message)s",
        "buffer_size": 8192
      },
      "file": {
        "enabled": true,
        "path": "logs/nlm_$(date).log",
        "rotation": "daily",
        "max_size_mb": 100,
        "compression": true
      }
    },
    "performance": {
      "buffer_size": 16384,
      "flush_interval_ms": 100,
      "async_threads": 2
    }
  }
}
```

### Runtime Configuration API

```cpp
#include "core/Logger/LoggerFactory.hpp"

void ConfigureLogging() {
    auto& factory = LoggerFactory::getInstance();
    
    // Configure component hierarchy
    factory.configureComponent("brain.neuron", LogLevel::DEBUG);
    factory.configureComponent("brain.synaptic", LogLevel::INFO);
    factory.configureComponent("world.sensory", LogLevel::DEBUG);
    
    // Configure outputs
    auto& consoleOutput = factory.addConsoleOutput(
        "[%(timestamp)s] [%(level)s] %(component)s: %(message)s"
    );
    consoleOutput.setBufferSize(8192);
    
    auto& fileOutput = factory.addFileOutput(
        "logs/nlm_simulation_$(timestamp).log"
    );
    fileOutput.setRotation("daily");
    fileOutput.setMaxSizeMB(100);
    
    // Apply configuration
    factory.applyConfiguration();
}
```

## Usage Examples

### Example 1: Basic Logging Setup

```cpp
#include "core/Logger/LoggerFactory.hpp"

void setupBasicLogging() {
    LoggerFactory& factory = LoggerFactory::getInstance();
    
    // Set global log level
    factory.setGlobalLogLevel(LogLevel::INFO);
    
    // Configure specific components
    factory.configureComponent("brain", LogLevel::DEBUG);
    factory.configureComponent("world", LogLevel::INFO);
    
    // Add console output
    factory.addConsoleOutput("[%(time)s] %(message)s");
    
    // Apply settings
    factory.applyConfiguration();
}
```

### Example 2: Advanced Component-Based Logging

```cpp
#include "core/Logger/LoggerFactory.hpp"

void setupAdvancedLogging() {
    LoggerFactory& factory = LoggerFactory::getInstance();
    
    // Create component hierarchy
    factory.createComponentHierarchy({
        {"brain.neuron", "brain"},
        {"brain.synaptic", "brain.neuron"},
        {"brain.memory", "brain"},
        {"world.sensory", "world"},
        {"world.motor", "world"},
        {"agent.bridging", "agent"},
        {"agent.neuromodulation", "agent"}
    });
    
    // Configure levels with inheritance
    factory.configureComponent("brain", LogLevel::DEBUG);
    factory.configureComponent("brain.neuron", LogLevel::INFO); // Override brain level
    factory.configureComponent("brain.synaptic", LogLevel::WARNING);
    
    // Setup multiple outputs
    auto& console = factory.addConsoleOutput(
        "[%Y-%m-%d %H:%M:%S] [%(level)5s] [%(component)s] %(message)s"
    );
    console.setBufferSize(16384);
    
    auto& file = factory.addFileOutput("logs/nlm_$(date).log");
    file.setRotation("hourly");
    file.setMaxSizeMB(50);
    file.enableCompression(true);
    
    // Configure performance settings
    factory.setBufferSize(32768);
    factory.setFlushInterval(50); // milliseconds
    factory.setAsyncThreads(4);
    
    factory.applyConfiguration();
}
```

### Example 3: Dynamic Logging Configuration

```cpp
#include "core/Logger/LoggerFactory.hpp"

void configureForProduction() {
    LoggerFactory& factory = LoggerFactory::getInstance();
    
    // Performance-oriented configuration
    factory.setGlobalLogLevel(LogLevel::WARNING);
    factory.setBufferSize(65536);
    factory.setFlushInterval(200);
    factory.setAsyncThreads(2);
    
    // Minimal component logging for production
    factory.configureComponent("brain.neuron", LogLevel::ERROR);
    factory.configureComponent("brain.synaptic", LogLevel::ERROR);
    factory.configureComponent("world", LogLevel::ERROR);
    
    // File-only output with rotation
    auto& fileOutput = factory.addFileOutput("logs/errors_$(date).log");
    fileOutput.setRotation("daily");
    fileOutput.setMaxSizeMB(200);
    
    factory.applyConfiguration();
}

void configureForDevelopment() {
    LoggerFactory& factory = LoggerFactory::getInstance();
    
    // Debug-oriented configuration
    factory.setGlobalLogLevel(LogLevel::DEBUG);
    factory.setBufferSize(8192);
    factory.setFlushInterval(10);
    factory.setAsyncThreads(1);
    
    // Verbose component logging for development
    factory.configureComponent("brain", LogLevel::DEBUG);
    factory.configureComponent("world", LogLevel::DEBUG);
    factory.configureComponent("agent", LogLevel::DEBUG);
    
    // Console output with details
    auto& consoleOutput = factory.addConsoleOutput(
        "[%H:%M:%S.%f] [%(thread)s] [%(level)s] [%(component)s] %(message)s"
    );
    
    // File output for debugging
    auto& debugFile = factory.addFileOutput("logs/debug_$(timestamp).log");
    debugFile.enableCompression(false);
    
    factory.applyConfiguration();
}
```

### Example 4: Custom Log Output

```cpp
#include "core/Logger/LoggerFactory.hpp"
#include "core/Logger/AsyncWriter.hpp"

class CustomOutput : public AsyncWriter {
public:
    CustomOutput(const std::string& format) : AsyncWriter(format) {}
    
    bool write(const LogMessage& message) override {
        // Custom output logic here
        // Example: Send to external monitoring system
        sendToMonitoringSystem(message);
        return true;
    }
    
private:
    void sendToMonitoringSystem(const LogMessage& message) {
        // Implementation for sending logs to monitoring system
        // Could use HTTP, gRPC, or any custom protocol
    }
};

void setupCustomOutput() {
    LoggerFactory& factory = LoggerFactory::getInstance();
    
    // Add custom output
    auto& customOutput = factory.addCustomOutput(
        std::make_unique<CustomOutput>("[%(timestamp)s] CUSTOM: %(message)s")
    );
    
    // Configure other settings
    factory.configureComponent("brain", LogLevel::INFO);
    factory.addConsoleOutput("[%(time)s] %(message)s");
    
    factory.applyConfiguration();
}
```

## Performance Considerations

### Memory Usage

- **Pre-allocated Buffers**: 32KB to 64KB per component
- **Zero-Copy Logging**: Minimizes memory allocations
- **Buffer Pool**: Shared buffers across components

### CPU Impact

- **Minimal Overhead**: < 1% CPU impact at DEBUG level
- **Conditional Logging**: Checks level at entry, skips processing if not needed
- **Vectorized Operations**: SIMD-accelerated string formatting

### I/O Optimization

- **Asynchronous Writing**: Prevents simulation stalls
- **Batched Flushes**: Reduces system call overhead
- **Output Selection**: Choose appropriate outputs based on runtime needs

## Best Practices

### 1. Component Naming Convention
```cpp
// Use dot notation for hierarchy
brain.neuron.stability     // Neuron stability metrics
brain.synaptic.weight     // Synaptic weight changes
brain.memory.episodic     // Episodic memory operations
world.sensory.visual      // Visual input processing
world.motor.motion        // Motor command execution
agent.bridging.translation // Agent-world bridging
agent.neuromodulation.dopamine // Dopamine modulation
```

### 2. Log Level Guidelines
```cpp
// Level assignment guidelines
brain.neuron: DEBUG     // Detailed neuron state information
brain.synaptic: INFO   // Synaptic events and plasticity
brain.memory: WARNING   // Memory errors and consolidation issues
world.sensory: INFO     // Sensory input processing
world.motor: INFO       // Motor command execution
agent.bridging: DEBUG   // Agent-world interaction details
agent.neuromodulation: INFO // Neuromodulation updates
```

### 3. Performance Tuning
```cpp
// Performance tuning strategies
factory.setBufferSize(32768);        // Balance memory and performance
factory.setFlushInterval(100);       // Trade-off between freshness and efficiency
factory.setAsyncThreads(2);          // CPU core count divided by 2
factory.setGlobalLogLevel(LogLevel::INFO); // Reduce verbosity in production
```

### 4. Error Logging
```cpp
// Error logging guidelines
logger.logError("Simulation failed", 
                ErrorContext()
                    .add("step", step)
                    .add("neuron_count", brain.getNeuronCount())
                    .add("simulation_time", time)
                    .add("error_code", error_code)
                    .withStackTrace());
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Missing Log Output
**Symptoms**: No logs appearing despite configuration
**Solution**:
```cpp
// Check configuration
LoggerFactory::getInstance().validateConfiguration();

// Force apply configuration
LoggerFactory::getInstance().applyConfiguration(true);
```

#### Issue 2: Performance Degradation
**Symptoms**: Simulation running slower with logging enabled
**Solution**:
```cpp
// Reduce verbosity
LoggerFactory::getInstance().setGlobalLogLevel(LogLevel::WARNING);

// Increase buffer size
LoggerFactory::getInstance().setBufferSize(65536);

// Reduce async threads
LoggerFactory::getInstance().setAsyncThreads(1);
```

#### Issue 3: Component Not Found
**Symptoms**: Component logging not working for specific components
**Solution**:
```cpp
// Check component exists
if (LoggerFactory::getInstance().hasComponent("brain.neuron")) {
    LoggerFactory::getInstance().configureComponent("brain.neuron", LogLevel::DEBUG);
} else {
    // Create component
    LoggerFactory::getInstance().createComponent("brain.neuron");
}
```

### Debugging Commands

```bash
# Check logging configuration
./nlm --logging-config

# Validate logging setup
./nlm --validate-logging

# Test logging performance
./nlm --logging-benchmark

# Monitor logging memory usage
./nlm --memory-profile --logging
```

## Migration Guide

### From Basic Logging to Modular System

If you're migrating from basic logging to the new modular system:

1. **Update Configuration**: Move from single-level logging to component-based configuration
2. **Adjust Log Files**: Update any custom log parsing to handle hierarchical format
3. **Performance Testing**: Test with both small and large simulations to ensure no performance regressions
4. **Component Organization**: Reorganize log messages to fit new component hierarchy

## Future Enhancements

### Planned Features

1. **Distributed Logging**: Integration with centralized logging infrastructure
2. **Log Analytics**: Built-in log analysis and visualization
3. **Adaptive Logging**: Runtime adjustment based on simulation state
4. **Structured Logging**: JSON format support for machine parsing
5. **Log Sampling**: Intelligent log sampling for large-scale simulations

## Conclusion

The NLM logging system provides a robust, scalable, and performant solution for large-scale neural simulations. Its modular architecture allows for flexible configuration while maintaining high performance, making it suitable for everything from development debugging to production monitoring.
