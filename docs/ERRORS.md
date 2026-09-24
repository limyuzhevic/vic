# NLM Error Reference Guide: Troubleshooting and Error Codes

# Welcome to NLM Error Reference Guide

This comprehensive guide documents all known errors in NLM, their meanings, causes, and solutions. It serves as both a troubleshooting resource and a programming reference for error handling.

## Table of Contents

1. [Error Code System](#error-code-system)
2. [Critical Errors](#critical-errors)
3. [Warning Errors](#warning-errors)
4. [Error Categories](#error-categories)
5. [Common Error Scenarios](#common-error-scenarios)
6. [Error Prevention](#error-prevention)
7. [Recovery Procedures](#recovery-procedures)

# Error Code System

## Error Code Format

NLM uses a standardized error code system:

```
ERR_<MODULE>_<ERROR_TYPE>_<SEVERITY>_<UNIQUE_ID>
```

### Components:

- **ERR**: Error prefix
- **MODULE**: Component name (e.g., BRAIN, CONFIG, LOGGER, AGENT)
- **ERROR_TYPE**: Type of error (INIT, LOAD, VALIDATE, OPERATE)
- **SEVERITY**: CRITICAL, ERROR, WARNING, INFO
- **UNIQUE_ID**: Numeric identifier for the specific error

### Examples:

- `ERR_BRAIN_INIT_001` - Brain initialization failed
- `ERR_CONFIG_LOAD_001` - Configuration file not found
- `ERR_LOGGER_CREATE_002` - Log file creation failed
- `ERR_AGENT_OPERATE_001` - Agent action execution failed

## Error Reporting Levels

| Level | Symbol | Description | Action Required |
|-------|--------|-------------|-----------------|
| Critical | `ERR_*_CRITICAL_*` | System cannot continue | Immediate action needed |
| Error | `ERR_*_ERROR_*` | Operation failed, may continue | Fix required |
| Warning | `ERR_*_WARNING_*` | Potential issues detected | Consider action |
| Info | `ERR_*_INFO_*` | Informational messages | No action needed |

# Critical Errors

## ERR_BRAIN_INIT_001

**Message**: "Failed to initialize brain with error code BRAIN_INIT_001"

**Description**: Brain initialization failed completely

**Causes**:
- Invalid configuration parameters
- Insufficient system resources
- Corrupted brain state
- Missing required dependencies

**Recovery Steps**:
1. Check configuration file for valid parameters
2. Verify system has minimum required RAM (512MB+)
3. Check available disk space (at least 100MB)
4. Run with smaller network configuration
5. Check for existing corrupted files and remove them

**Prevention**:
- Validate configuration before initialization
- Monitor system resources during long operations
- Use smaller network sizes for testing

## ERR_CONFIG_LOAD_001

**Message**: "Failed to load configuration file: <filename>"

**Description**: Configuration file cannot be found or read

**Causes**:
- File does not exist
- Insufficient file permissions
- File is corrupted or empty
- Path is incorrect

**Recovery Steps**:
1. Check if the file exists at the specified path
2. Verify file permissions (readable by current user)
3. Check file contents for valid configuration format
4. Create a new configuration file from example
5. Use default configuration if file-based config fails

**Example Recovery Command**:
```bash
# Check file existence and permissions
ls -la configs/default.cfg

# Create from example if missing
./nlm --create-config configs/default.cfg
```

## ERR_LOGGER_CREATE_001

**Message**: "Cannot create log file: <filename>"

**Description**: Log file creation failed

**Causes**:
- Insufficient disk space
- Invalid file path
- Permission denied
- Directory not writable

**Recovery Steps**:
1. Check available disk space
2. Verify directory is writable
3. Use absolute path instead of relative path
4. Disable file logging and use console only
5. Create directory if it doesn't exist

**Prevention**:
- Monitor disk space regularly
- Use absolute paths for log files
- Provide appropriate file permissions

## ERR_AGENT_OPERATE_001

**Message**: "Agent action execution failed"

**Description**: Agent cannot execute the requested action

**Causes**:
- Invalid action parameters
- Missing sensory input
- Network not properly initialized
- Action exceeds world boundaries

**Recovery Steps**:
1. Validate action parameters
2. Ensure agent has received sensory input
3. Check if agent is properly initialized
4. Verify action is within world boundaries
5. Reset agent and try again

**Example Recovery**:
```cpp
// Check if agent can perform action
if (!agent.canPerformAction(action)) {
    NLM_LOG_ERROR("Agent cannot perform action: ERR_AGENT_OPERATE_001");
    agent.reset(); // Reset agent state
    agent.initialize(world); // Reinitialize
}
```

# Warning Errors

## ERR_BRAIN_WARNING_001

**Message**: "Large network size detected: <neuron_count> neurons"

**Description**: Network is large for current system configuration

**Causes**:
- Too many neurons for available memory
- High computational requirements
- Long simulation times

**Recovery Steps**:
1. Reduce neuron count for testing
2. Increase system resources
3. Use smaller timesteps
4. Disable intensive features

**Prevention**:
- Set reasonable neuron count limits
- Monitor resource usage
- Use performance profiling

## ERR_CONFIG_WARNING_001

**Message**: "Configuration parameter '<key>' has non-optimal value: <value>"

**Description**: Configuration parameter is not optimized for performance

**Causes**:
- Parameter values are outside recommended ranges
- Incompatible parameter combinations
- Suboptimal for current use case

**Recovery Steps**:
1. Check parameter documentation for recommended ranges
2. Adjust parameters based on use case
3. Run configuration optimizer
4. Use example configurations

**Example Optimal Values**:
```
# For testing
neuron_count = 100-500
simulation_timestep = 0.001
connection_probability = 0.1-0.2

# For production
neuron_count = 1000-5000
simulation_timestep = 0.0001
connection_probability = 0.05-0.15
```

## ERR_LOGGER_WARNING_002

**Message**: "Log level too verbose for production: <level>"

**Description**: Log level is not suitable for production environment

**Causes**:
- Too much debug information
- Performance impact from verbose logging
- Information leakage

**Recovery Steps**:
1. Reduce log level to INFO or WARNING
2. Disable debug logging in production
3. Use structured logging format
4. Implement log rotation

# Error Categories

## Configuration Errors (ERR_CONFIG_*)

### ERR_CONFIG_FILE_001
- **Message**: "Cannot write configuration file: <filename>"
- **Solution**: Check write permissions, use absolute path

### ERR_CONFIG_FORMAT_001
- **Message**: "Invalid configuration format in file: <filename>"
- **Solution**: Check file syntax, use valid key=value format

### ERR_CONFIG_VALIDATE_001
- **Message**: "Invalid configuration parameter: <parameter>"
- **Solution**: Check parameter type, use valid values

## Brain Errors (ERR_BRAIN_*)

### ERR_BRAIN_MEMORY_001
- **Message**: "Insufficient memory for brain initialization"
- **Solution**: Reduce neuron count, increase system memory

### ERR_BRAIN_CONNECT_001
- **Message**: "Failed to create synaptic connections"
- **Solution**: Check connection probability, adjust parameters

### ERR_BRAIN_PLASTICITY_001
- **Message**: "Plasticity rule failed to apply"
- **Solution**: Check plasticity parameters, verify activity

## Logger Errors (ERR_LOGGER_*)

### ERR_LOGGER_STREAM_001
- **Message**: "Log stream write failed"
- **Solution**: Check disk space, fix file permissions

### ERR_LOGGER_LEVEL_001
- **Message**: "Invalid log level specified"
- **Solution**: Use valid LogLevel enum values

## Agent Errors (ERR_AGENT_*)

### ERR_AGENT_SENSOR_001
- **Message**: "Sensory input processing failed"
- **Solution**: Check sensor data format, validate input

### ERR_AGENT_MOTOR_001
- **Message**: "Motor command execution failed"
- **Solution**: Validate action parameters, check world state

# Common Error Scenarios

## Scenario 1: "Brain initialization failed with 0 neurons"

**Error**: `ERR_BRAIN_INIT_001`

**Troubleshooting**:
```bash
# Check configuration
./nlm --config configs/small_test.cfg

# Or run with command line arguments
./nlm --neuron_count 100 --region_count 1 --connection_probability 0.1
```

**Expected Resolution**: Network initializes with 100 neurons successfully.

## Scenario 2: "Cannot read configuration file: configs/my_config.cfg"

**Error**: `ERR_CONFIG_LOAD_001`

**Troubleshooting**:
```bash
# Check file exists
ls -la configs/my_config.cfg

# View file contents
cat configs/my_config.cfg

# Create minimal config
./nlm --create-config configs/my_config.cfg
```

**Expected Resolution**: File exists and has valid configuration.

## Scenario 3: "Agent stuck at position (0, 0)"

**Error**: `ERR_AGENT_OPERATE_001`

**Troubleshooting**:
```cpp
// Check if agent has sensory input
if (!agent.hasPercept()) {
    NLM_LOG_ERROR("Agent has no sensory input: ERR_AGENT_OPERATE_001");
    agent.reset();
    agent.initialize(world);
}
```

**Expected Resolution**: Agent resets and reinitializes with fresh sensory processing.

## Scenario 4: "Simulation too slow, taking hours for 100 steps"

**Error**: `ERR_PERF_WARNING_001`

**Troubleshooting**:
```cpp
// Check configuration
if (config->getOr<double>("simulation_timestep", 0.001) > 0.01) {
    NLM_LOG_WARNING("Large timestep detected: " + std::to_string(timestep));
    config->set("simulation_timestep", 0.001);
}
```

**Expected Resolution**: Reduce timestep and neuron count for faster execution.

# Error Prevention

## Configuration Validation

```cpp
bool validateConfiguration(const Config& config) {
    bool isValid = true;
    
    // Check neuron count
    int neuronCount = config.getOr<int>("neuron_count", 0);
    if (neuronCount < 1) {
        NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuronCount) + 
                     " - ERR_CONFIG_VALIDATE_001");
        isValid = false;
    } else if (neuronCount > 10000) {
        NLM_LOG_WARNING("Large neuron_count may cause performance issues");
    }
    
    // Check timestep
    double timestep = config.getOr<double>("simulation_timestep", 0.001);
    if (timestep <= 0.0) {
        NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(timestep) +
                     " - ERR_CONFIG_VALIDATE_002");
        isValid = false;
    }
    
    // Check connection probability
    float connProb = config.getOr<float>("connection_probability", 0.0);
    if (connProb < 0.0 || connProb > 1.0) {
        NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(connProb) +
                     " - ERR_CONFIG_VALIDATE_003");
        isValid = false;
    }
    
    return isValid;
}
```

## Resource Monitoring

```cpp
void monitorResources() {
    // Check memory usage
    long availableMemory = getAvailableMemory();
    if (availableMemory < 512 * 1024 * 1024) { // 512MB
        NLM_LOG_WARNING("Low memory available: " + 
                       std::to_string(availableMemory / (1024*1024)) + "MB" +
                       " - ERR_RESOURCE_WARNING_001");
    }
    
    // Check disk space
    long freeDisk = getFreeDiskSpace();
    if (freeDisk < 100 * 1024 * 1024) { // 100MB
        NLM_LOG_WARNING("Low disk space: " + 
                       std::to_string(freeDisk / (1024*1024)) + "MB" +
                       " - ERR_STORAGE_WARNING_001");
    }
}
```

## Error Handling Pattern

```cpp
try {
    // Perform operation
    performCriticalOperation();
} catch (const std::exception& e) {
    NLM_LOG_ERROR("Operation failed: " + std::string(e.what()) +
                 " - ERR_OPERATION_ERROR_001");
    
    // Attempt recovery
    if (!attemptRecovery()) {
        NLM_LOG_CRITICAL("Recovery failed, system cannot continue" +
                         " - ERR_RECOVERY_CRITICAL_001");
        return 1; // Exit with error code
    }
}
```

# Recovery Procedures

## Immediate Recovery

### Brain Initialization Recovery

```cpp
bool initializeWithRecovery(const Config& config) {
    int attempts = 0;
    const int maxAttempts = 3;
    
    while (attempts < maxAttempts) {
        try {
            auto brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                return true; // Success
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Attempt " + std::to_string(attempts + 1) + 
                           ": " + std::string(e.what()));
            
            // Adjust configuration for next attempt
            attempts++;
            if (attempts < maxAttempts) {
                adjustConfigurationForRecovery(config);
            }
        }
    }
    
    NLM_LOG_ERROR("Brain initialization failed after all attempts" +
                 " - ERR_BRAIN_INIT_FAILED_001");
    return false;
}
```

### Configuration Recovery

```cpp
bool loadWithRecovery(const std::string& filename) {
    // Try primary file
    if (loadConfiguration(filename)) {
        return true;
    }
    
    // Try backup files
    std::vector<std::string> backupFiles = {
        "configs/backup.cfg",
        "configs/default.cfg",
        "configs/example.cfg"
    };
    
    for (const auto& backup : backupFiles) {
        NLM_LOG_INFO("Trying backup configuration: " + backup);
        if (loadConfiguration(backup)) {
            NLM_LOG_INFO("Recovered using backup: " + backup +
                        " - ERR_CONFIG_RECOVERY_001");
            return true;
        }
    }
    
    NLM_LOG_ERROR("All configuration files failed to load" +
                 " - ERR_CONFIG_ALL_FAILED_001");
    return false;
}
```

## Long-term Recovery

### Performance Recovery

```cpp
void performPerformanceRecovery() {
    NLM_LOG_WARNING("Performance degradation detected, performing recovery..." +
                   " - ERR_PERF_WARNING_002");
    
    // Gather performance metrics
    auto metrics = collectPerformanceMetrics();
    
    // Analyze bottlenecks
    if (metrics.cpuUsage > 90.0f) {
        NLM_LOG_INFO("CPU bottleneck detected, reducing network size");
        // Reduce neuron count
    } else if (metrics.memoryUsage > 80.0f) {
        NLM_LOG_INFO("Memory bottleneck detected, optimizing memory usage");
        // Optimize memory allocation
    } else if (metrics.diskIO > 95.0f) {
        NLM_LOG_INFO("I/O bottleneck detected, using in-memory cache");
        // Implement caching
    }
    
    // Restart with optimized configuration
    NLM_LOG_INFO("Restarting with optimized configuration");
    // This would typically involve restarting the application
}
```

# Error Code Reference Table

| Error Code | Description | File | Function | Status |
|------------|-------------|------|----------|--------|
| ERR_BRAIN_INIT_001 | Brain initialization failed | src/brain/Brain.cpp | initialize() | Critical |
| ERR_BRAIN_MEMORY_001 | Insufficient memory for brain | src/brain/Brain.cpp | initialize() | Critical |
| ERR_CONFIG_LOAD_001 | Configuration file not found | src/core/Config/Config.cpp | loadFromFile() | Critical |
| ERR_LOGGER_CREATE_001 | Log file creation failed | src/core/Logger/Logger.cpp | setOutputFile() | Critical |
| ERR_AGENT_OPERATE_001 | Agent action execution failed | src/agent/AgentBrain.cpp | decodeMotorCommand() | Error |
| ERR_PERF_WARNING_001 | Performance degradation detected | src/performance/Metrics.cpp | collectMetrics() | Warning |

# Next Steps

1. **Read the TUTORIALS.md** for step-by-step guidance
2. **Run the examples** in the examples/ directory to understand usage patterns
3. **Test error recovery** by intentionally causing errors and watching the recovery mechanisms
4. **Check the ERROR CODES** in the source code for the latest updates
5. **Report issues** with error codes to help improve the error handling system

---

*For more examples and detailed error information, refer to the EXAMPLES.md file.*
