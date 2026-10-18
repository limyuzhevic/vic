# NLM Enhanced Documentation

# Overview

NLM (熙然) has been significantly enhanced with robust error handling, comprehensive configuration management, and improved logging capabilities. This document provides detailed information about the improvements and new features.

## Key Improvements

### 1. Enhanced Configuration Management

#### JSON and YAML Support

The enhanced Config class now provides full support for both JSON and YAML configuration files, with intelligent auto-detection and validation:

```cpp
// Load from JSON file
bool success = config.loadFromFile("config.json");

// Load from YAML file  
bool success = config.loadFromFile("config.yaml");

// Auto-detection based on file content
config.loadFromFile("settings.conf"); // Automatically detected
```

#### Smart Format Detection

The system intelligently detects file format based on:
- File extension (.json, .yaml, .yml, .txt, etc.)
- File content patterns (JSON object/array, YAML key-value pairs)
- Fallback to simple key=value format for compatibility

#### Enhanced Error Recovery

```cpp
// Graceful error handling with fallback values
if (!config.loadFromFile("production.json")) {
    // Log error and continue with defaults
    NLM_LOG_WARNING("Failed to load production config");
    config.set("brain.neuron_count", 1000);
    config.set("plasticity.enable", true);
}
```

### 2. Robust Logging System

#### Performance Monitoring

```cpp
// Log rate monitoring
std::cout << "Log rate: " << logger->getLogRate() << " logs/sec" << std::endl;
std::cout << "Total logs: " << logger->getTotalLogCount() << std::endl;

// Time-based statistics
auto stats = logger->getPerformanceStats();
std::cout << "Average log rate: " << stats.averageLogRate << " logs/sec" << std::endl;
```

#### File Error Recovery

```cpp
// Automatic directory creation
logger->setOutputFile("/logs/nlm.log");  // Creates /logs directory

// Graceful file write failure handling
try {
    logger->setOutputFile("/app/nlm.log");
} catch (const std::exception& e) {
    NLM_LOG_WARNING(std::string("Cannot write to log file: ") + e.what());
    // Continue with console logging only
}
```

#### Context-Rich Logging

```cpp
// Enhanced log entries with context information
NLM_LOG_INFO("Processing neuron", "src/brain/Neuron.cpp", 142, "stepLIF");
NLM_LOG_WARNING("Low memory available", __FILE__, __LINE__, __func__);
NLM_LOG_ERROR("Failed to load configuration", "src/core/Config.cpp", 89, "loadFromFile");
```

### 3. Comprehensive Error Handling

#### Input Validation

```cpp
// Input validation for all public APIs
config.set("brain.neuron_count", -1);  // Validation rejects invalid values
config.set("plasticity.rate", 1.5f);   // Validation clamps to [0, 1]

// Enhanced error messages with context
NLM_LOG_ERROR("Invalid configuration key: invalid.key", "src/core/Config.cpp", 156, "set");
```

#### Graceful Degradation

```cpp
// System continues operating even when components fail
try {
    brain->initialize();
} catch (const std::exception& e) {
    NLM_LOG_CRITICAL("Brain initialization failed: " + std::string(e.what()));
    // System can continue with degraded functionality
}
```

#### Exception Safety

```cpp
// Exception safety throughout the codebase
try {
    config.loadFromFile("important.conf");
    world->reset();
    brain->step(0);
} catch (const std::bad_alloc&) {
    NLM_LOG_CRITICAL("Memory allocation failed");
    // Cleanup and recovery
} catch (const std::exception& e) {
    NLM_LOG_ERROR("Operation failed: " + std::string(e.what()));
    // Graceful error recovery
}
```

## Configuration Examples

### JSON Configuration

```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1,
    "v_thresh": -55.0,
    "v_rest": -70.0,
    "v_reset": -75.0
  },
  "plasticity": {
    "stdp": {
      "enable": true,
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": {
      "enable": true,
      "learning_rate": 0.01
    },
    "structural": {
      "enable": true,
      "synaptogenesis_rate": 0.0001,
      "pruning_rate": 0.00001
    }
  },
  "neuromodulation": {
    "dopamine": {
      "scale": 1.0
    },
    "curiosity": {
      "enable": true,
      "scale": 1.0
    },
    "novelty": {
      "enable": true,
      "scale": 1.0
    },
    "prediction_error": {
      "enable": true,
      "scale": 1.0
    }
  },
  "memory": {
    "working_memory": {
      "capacity": 1000,
      "decay_rate": 0.01
    },
    "episodic_memory": {
      "max_episodes": 10000,
      "replay_enabled": true
    }
  },
  "performance": {
    "enable_monitoring": true,
    "log_rate_limit": 10000,
    "memory_limit_mb": 1024
  }
}
```

### YAML Configuration

```yaml
brain:
  neuron_count: 1000
  region_count: 1
  connection_probability: 0.1
  v_thresh: -55.0
  v_rest: -70.0
  v_reset: -75.0

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
  hebbian:
    enable: true
    learning_rate: 0.01
  structural:
    enable: true
    synaptogenesis_rate: 0.0001
    pruning_rate: 0.00001

neuromodulation:
  dopamine:
    scale: 1.0
  curiosity:
    enable: true
    scale: 1.0
  novelty:
    enable: true
    scale: 1.0
  prediction_error:
    enable: true
    scale: 1.0

memory:
  working_memory:
    capacity: 1000
    decay_rate: 0.01
  episodic_memory:
    max_episodes: 10000
    replay_enabled: true

performance:
  enable_monitoring: true
  log_rate_limit: 10000
  memory_limit_mb: 1024
```

### Command Line Configuration

```bash
./nlm \
  --brain.neuron_count=2000 \
  --brain.v_thresh=-60.0 \
  --plasticity.stdp.enable=true \
  --plasticity.stdp.learning_rate=0.002 \
  --neuromodulation.curiosity.enable=true \
  --neuromodulation.novelty.enable=true \
  --memory.working_memory.capacity=2000 \
  --performance.enable_monitoring=true
```

## Enhanced Features

### 1. Configuration Validation

The enhanced Config class includes comprehensive validation:

```cpp
// Type validation
config.set("brain.neuron_count", "invalid");  // Validation fails
config.set("brain.neuron_count", 1000);        // Validation passes

// Range validation
config.set("brain.v_thresh", -100.0);  // Valid
config.set("brain.v_thresh", 100.0);   // Clamped to -100.0

// String validation
config.set("brain.description", "Hello World");  // Valid
config.set("brain.description", "Hello\"World"); // Valid (quotes handled)
```

### 2. Enhanced Logging

#### Performance Statistics

```cpp
// Log performance statistics
logger->logPerformanceStats();

// Monitor log file health
logger->checkLogFileHealth();

// Analyze log patterns
logger->analyzeLogPatterns();
```

#### Thread-Safe Operations

```cpp
// All logging operations are thread-safe
std::thread t1([&]() { NLM_LOG_INFO("Thread 1"); });
std::thread t2([&]() { NLM_LOG_INFO("Thread 2"); });
t1.join();
t2.join();  // No data races
```

### 3. Error Recovery

#### File Error Recovery

```cpp
// Automatic file error recovery
logger->setOutputFile("/app/nlm.log");  // Creates directory if needed

// Graceful fallback on write failure
try {
    std::ofstream file("/app/nlm.log");
    file << "Log entry\n";
} catch (...) {
    std::cout << "Cannot write to log file, using console only\n";
}
```

#### System Recovery

```cpp
// System recovery after errors
try {
    config.loadFromFile("critical.conf");
    world->reset();
    brain->initialize();
    for (int step = 0; step < 100; ++step) {
        brain->step(step);
    }
} catch (const std::exception& e) {
    NLM_LOG_CRITICAL("System recovery failed: " + std::string(e.what()));
    // System can continue with default configuration
}
```

## Performance Considerations

### Memory Management

The enhanced system includes better memory management:

```cpp
// Memory usage tracking
logger->trackMemoryUsage();

// Memory limit enforcement
logger->setMemoryLimit(1024);  // 1GB limit

// Memory pool optimization
logger->optimizeMemoryPools();
```

### I/O Optimization

```cpp
// Buffered file I/O
logger->enableFileBuffering(true);

// Asynchronous logging
logger->enableAsyncLogging(true);

// Log rotation
logger->enableLogRotation(10);  // Rotate after 10MB
```

### CPU Optimization

```cpp
// Log rate limiting
logger->setLogRateLimit(1000);  // Max 1000 logs/sec

// Log level optimization
logger->setLogLevel(LogLevel::Info);  // Only Info and above

// Context optimization
logger->enableContextLogging(false);  // Reduce context information
```

## Migration Guide

### From Previous Versions

#### Configuration Migration

```cpp
// Old configuration format
brain.neuron_count = 1000
brain.v_thresh = -55
plasticity.enable = true

// New configuration format (JSON)
{
  "brain": {
    "neuron_count": 1000,
    "v_thresh": -55
  },
  "plasticity": {
    "enable": true
  }
}
```

#### API Changes

```cpp
// Old error handling
if (!config.loadFromFile("config.txt")) {
    std::cout << "Load failed" << std::endl;
}

// New error handling
if (!config.loadFromFile("config.json")) {
    NLM_LOG_ERROR("Configuration load failed: " + config.getLastError());
    // Automatic fallback to defaults
    config.set("brain.neuron_count", 1000);
}
```

### Upgrading Steps

1. **Update Configuration**: Convert existing configuration files to JSON/YAML format
2. **Update Code**: Check for deprecated API usage
3. **Test**: Run enhanced test suite to verify functionality
4. **Monitor**: Use enhanced logging to monitor system performance
5. **Validate**: Validate configuration with enhanced validation

## Testing and Validation

### Unit Tests

```bash
// Run enhanced test suite
cd build
make test
./nlm_test --verbose

// Run configuration tests
./nlm_test --config-tests

// Run logging tests
./nlm_test --logging-tests
```

### Integration Tests

```cpp
// Test configuration loading
TEST(ConfigTests, LoadFromJSON) {
    Config config;
    EXPECT_TRUE(config.loadFromFile("test_config.json"));
    EXPECT_EQ(config.getOr<int>("brain.neuron_count", 0), 1000);
}

// Test error handling
TEST(ConfigTests, ErrorHandling) {
    Config config;
    EXPECT_FALSE(config.loadFromFile("nonexistent.json"));
    EXPECT_FALSE(config.has("nonexistent.key"));
}
```

### Performance Tests

```bash
// Run performance tests
make benchmark
./benchmark --config=json --size=large

// Monitor system performance
./monitor --log-rate --memory --cpu
```

## Troubleshooting

### Common Issues

#### Configuration Loading Failures

```bash
# Issue: Configuration file not found
solution: Check file path and permissions

# Issue: Malformed JSON syntax
solution: Validate JSON syntax with jsonlint

# Issue: Invalid YAML syntax
solution: Validate YAML syntax with yamllint
```

#### Logging Failures

```bash
# Issue: Cannot write to log file
solution: Check disk space and permissions

# Issue: Log file too large
solution: Enable log rotation

# Issue: Memory limit exceeded
solution: Increase memory limit or reduce log volume
```

#### Performance Issues

```bash
# Issue: High CPU usage
solution: Reduce log rate, optimize log levels

# Issue: High memory usage
solution: Enable memory tracking, optimize memory pools

# Issue: Slow file I/O
solution: Use buffered I/O, enable async logging
```

### Debug Commands

```bash
# Enable debug logging
nlm --log-level=DEBUG --log-file=debug.log

# Check system status
nlm --status

# Validate configuration
nlm --validate-config config.json

# Test error recovery
nlm --test-recovery
```

## Future Enhancements

### Planned Features

1. **Advanced Configuration Validation**: Schema-based validation with JSON Schema
2. **Real-Time Monitoring**: Live system monitoring with web interface
3. **Distributed Logging**: Multi-system logging with load balancing
4. **AI-Enhanced Logging**: AI-powered log analysis and anomaly detection
5. **Memory Profiler**: Advanced memory profiling and optimization

### Research Integration

NLM provides several research features:

- **Modular Architecture**: Easy extension with new components
- **Configuration Flexibility**: Support for experimental variations
- **Performance Monitoring**: Comprehensive performance tracking
- **Error Recovery**: Robust handling of experimental failures

## Conclusion

NLM represents a significant advancement in brain simulation research, combining:

- **Robust Error Handling**: Comprehensive error recovery and validation
- **Enhanced Configuration**: Multiple format support and validation
- **Performance Monitoring**: Comprehensive performance tracking
- **Modern Software Engineering**: Clean architecture and best practices
- **Research Support**: Flexible and extensible design

The system is ready for both educational use and advanced research applications, with comprehensive documentation and examples to support users at all levels.

## Additional Resources

### Documentation Files

- **ENHANCED_ERROR_HANDLING.md**: Comprehensive guide to error handling improvements
- **CONFIG_MANAGEMENT.md**: Detailed documentation of enhanced configuration management
- **PERFORMANCE_MONITORING.md**: Guide to performance monitoring and optimization
- **DEBUGGING_GUIDE.md**: Comprehensive debugging and troubleshooting guide

### Test Files

- **test_config.cpp**: Configuration loading and validation tests
- **test_logger.cpp**: Logging system tests
- **test_performance.cpp**: Performance monitoring tests
- **test_error_handling.cpp**: Error recovery and handling tests

### Examples

- **examples/basic_usage.cpp**: Basic usage examples
- **examples/advanced_config.cpp**: Advanced configuration examples
- **examples/performance_monitoring.cpp**: Performance monitoring examples
- **examples/debugging.cpp**: Debugging and troubleshooting examples

---

*This document covers the enhanced features and improvements made to NLM. For specific questions or additional information, please refer to the individual documentation files.*