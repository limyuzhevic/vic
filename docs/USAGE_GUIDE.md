# NLM Usage Guide

## Overview
This comprehensive usage guide covers the enhanced NLM (熙然) neural learning machine system, including the new configuration system, unified utility classes, and best practices for using the improved codebase.

## Quick Start

### Basic NLM Usage

The NLM system is designed to be used as a closed-loop neural simulation framework. Here's how to get started:

#### Phase 6 Demo (Integration Test)

```bash
# Navigate to the build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4

# Run the Phase 6 integration demo
./nlm_phase6_demo
```

### Configuration System Usage

#### Basic Configuration Loading

```cpp
#include "core/Config/Config.hpp"

int main() {
    // Create configuration instance
    Config config;
    
    // Load from JSON file
    if (!config.loadFromFile("config.json")) {
        std::cerr << "Failed to load configuration from config.json" << std::endl;
        return 1;
    }
    
    // Access configuration values with type safety
    std::string logLevel = config.getStringOr("log_level", "INFO");
    int neuronCount = config.getIntOr("neuron_count", 1000);
    double timestep = config.getDoubleOr("simulation_timestep", 0.001);
    
    std::cout << "Log Level: " << logLevel << std::endl;
    std::cout << "Neuron Count: " << neuronCount << std::endl;
    std::cout << "Timestep: " << timestep << std::endl;
    
    return 0;
}
```

#### Using the Configuration CLI

```bash
# Load configuration from file
nlm-config --config config.json

# Validate configuration
nlm-config --validate --config user.cfg

# Export configuration to JSON
nlm-config --export --format json > output.json

# Apply configuration template
nlm-config --template cpu_config

# Show configuration history
nlm-config --show-history

# Create backup
nlm-config --backup
```

### NeuralActivityCalculator Usage

```cpp
#include "core/utilities/NeuralActivityCalculator.hpp"

// Calculate neural activity using deviation strategy
NeuralActivityCalculator calculator(NeuralActivityCalculator::Strategy::DEVIATION);
calculator.setThreshold(0.5);
calculator.setNormalization(true);

double activity = calculator.calculate(previousVoltage, currentVoltage);
```

### SpikeDetector Usage

```cpp
#include "core/utilities/SpikeDetector.hpp"

// Create spike detector with threshold
SpikeDetector detector;
detector.setThreshold(20.0);  // mV
detector.setRefractoryPeriod(5.0);  // ms
detector.setDerivativeThreshold(5.0);

// Detect spikes in voltage trace
double voltage = getCurrentVoltage();
if (detector.detectSpike(voltage)) {
    onSpikeDetected();
}
```

### ErrorHandler Usage

```cpp
#include "core/utilities/ErrorHandler.hpp"

// Configure error handler
ErrorHandler errorHandler;
errorHandler.setLogLevel(ErrorHandler::Severity::WARNING);
errorHandler.setCallback([](const ErrorHandler::Error& error) {
    std::cerr << "Error: " << error.message << std::endl;
});

// Report errors safely
if (someCondition) {
    errorHandler.report(ErrorHandler::Severity::ERROR, "Calculation failed: " + details);
}
```

### NeuralMath Usage

```cpp
#include "core/utilities/NeuralMath.hpp"

// Use neural activation functions
double sigmoidInput = NeuralMath::sigmoid(2.0);
double tanhInput = NeuralMath::tanh(-1.5);
double reluInput = NeuralMath::relu(-0.5);

// Use normalization
double normalized = NeuralMath::normalize(value, min, max);
double denormalized = NeuralMath::denormalize(normalized, min, max);
```

## Configuration File Formats

### JSON Configuration

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "log_level": "INFO",
    "log_to_file": false,
    "visualization_enabled": false,
    "templates": {
        "cpu_performance": {
            "parallel_processing": true,
            "thread_count": 4
        }
    }
}
```

### YAML Configuration

```yaml
# Neural Network Configuration Template
random_seed: 42
simulation_timestep: 0.001
neuron_count: 1000
region_count: 2
connection_probability: 0.1

stdp:
    ltp_weight: 0.01
    ltd_weight: 0.012
    time_constant: 20.0

logging:
    level: INFO
    to_file: false
    filename: nlm.log

visualization:
    enabled: false
    update_rate: 30.0
```

### Legacy Configuration Format

```
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
log_level = INFO
log_to_file = false
visualization_enabled = false
```

## Configuration Templates

### Basic Configuration

```bash
# Apply basic template
nlm-config --template basic
```

### CPU Performance Template

```bash
# Apply CPU performance template
nlm-config --template cpu_performance
```

### Memory-Intensive Template

```bash
# Apply memory-intensive template
nlm-config --template memory_intensive
```

### Production Template

```bash
# Apply production template
nlm-config --template production
```

## Integration Examples

### Example 1: Full System Integration

```cpp
#include "core/Config/Config.hpp"
#include "core/utilities/NeuralActivityCalculator.hpp"
#include "core/utilities/SpikeDetector.hpp"
#include "core/utilities/ErrorHandler.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"

int main() {
    // Initialize configuration system
    Config config;
    config.loadFromFile("config.json");
    
    // Initialize error handling
    ErrorHandler errorHandler;
    errorHandler.setLogLevel(ErrorHandler::Severity::WARNING);
    
    // Create neural activity calculator
    NeuralActivityCalculator activityCalculator(
        NeuralActivityCalculator::Strategy::DEVIATION
    );
    activityCalculator.setThreshold(0.5);
    
    // Create spike detector
    SpikeDetector spikeDetector;
    spikeDetector.setThreshold(20.0);
    spikeDetector.setRefractoryPeriod(5.0);
    
    // Initialize agent brain
    AgentBrain agentBrain(config, activityCalculator, spikeDetector, errorHandler);
    
    // Run simulation loop
    while (simulationRunning) {
        // Process sensory input
        agentBrain.processSensoryInput(getSensorData());
        
        // Calculate neural activity
        double activity = activityCalculator.calculate(
            previousVoltage, currentVoltage
        );
        
        // Detect spikes
        if (spikeDetector.detectSpike(getCurrentVoltage())) {
            onSpikeEvent();
        }
        
        // Update brain state
        agentBrain.updateBrainState(deltaTime);
        
        // Check for errors
        if (errorHandler.hasErrors()) {
            handleErrors(errorHandler.getErrors());
        }
    }
    
    return 0;
}
```

### Example 2: Neural Activity Analysis

```cpp
#include "core/utilities/NeuralActivityCalculator.hpp"
#include "core/utilities/SpikeDetector.hpp"
#include "brain/Neuron.hpp"

void analyzeNeuralActivity(const std::vector<double>& voltageTrace) {
    // Create activity calculator with variance strategy
    NeuralActivityCalculator calculator(NeuralActivityCalculator::Strategy::VARIANCE);
    calculator.setWindowSize(100);
    calculator.setNormalization(true);
    
    // Create spike detector
    SpikeDetector detector;
    detector.setThreshold(15.0);
    detector.setDerivativeThreshold(2.0);
    
    std::vector<double> activities;
    std::vector<double> spikeTimes;
    
    // Process voltage trace
    for (size_t i = 0; i < voltageTrace.size(); ++i) {
        double voltage = voltageTrace[i];
        
        // Calculate neural activity
        if (i > 10) {
            double activity = calculator.calculate(
                voltageTrace[i-5], voltageTrace[i-10]
            );
            activities.push_back(activity);
        }
        
        // Detect spikes
        if (detector.detectSpike(voltage)) {
            spikeTimes.push_back(i * 0.001); // Convert to seconds
        }
    }
    
    // Analyze results
    double meanActivity = NeuralMath::mean(activities);
    double stdDevActivity = NeuralMath::stdDev(activities);
    
    std::cout << "Mean Activity: " << meanActivity << std::endl;
    std::cout << "Activity Std Dev: " << stdDevActivity << std::endl;
    std::cout << "Spike Count: " << spikeTimes.size() << std::endl;
    std::cout << "Average ISI: " << calculateAverageISI(spikeTimes) << " s" << std::endl;
}
```

### Example 3: Configuration Management

```cpp
#include "core/Config/Config.hpp"
#include "core/utilities/ErrorHandler.hpp"

void demonstrateConfigurationManagement() {
    ErrorHandler errorHandler;
    
    // Create configuration instance
    Config config;
    
    // Load and validate configuration
    if (!config.loadFromFile("production_config.yaml")) {
        errorHandler.report(ErrorHandler::Severity::ERROR, "Failed to load production config");
        return;
    }
    
    if (!config.validate()) {
        errorHandler.report(ErrorHandler::Severity::ERROR, "Configuration validation failed");
        return;
    }
    
    // Create backup before making changes
    config.createBackup("pre-modification");
    
    // Modify configuration
    config.setValue("simulation_timestep", 0.0005);
    config.setValue("log_level", "WARNING");
    
    // Validate changes
    if (!config.validate()) {
        errorHandler.report(ErrorHandler::Severity::ERROR, "Modified configuration invalid");
        // Restore from backup
        config.restoreFromBackup("pre-modification");
        return;
    }
    
    // Export updated configuration
    config.exportToFile("updated_config.json", Config::Format::JSON);
    
    // Show change history
    auto history = config.getHistory();
    for (const auto& entry : history) {
        std::cout << "Change: " << entry.timestamp << " - " << entry.description << std::endl;
    }
}
```

## Performance Tips

### 1. Configuration Performance
- **Reuse Configuration Objects**: Reuse `Config` instances instead of creating new ones
- **Pre-validate**: Validate configuration before use to catch errors early
- **Cache Frequently Accessed Values**: Cache frequently accessed configuration values

### 2. Neural Calculations
- **Pre-allocate Arrays**: Pre-allocate arrays for neural calculations
- **Use Efficient Algorithms**: Use `NeuralMath` for optimized neural operations
- **Vectorize Operations**: Use SIMD-enabled functions when possible

### 3. Error Handling
- **Batch Error Reports**: Batch error reports to reduce overhead
- **Set Appropriate Log Levels**: Set appropriate log levels to reduce noise
- **Use Efficient Callbacks**: Use efficient error callbacks

### 4. Spike Detection
- **Set Appropriate Thresholds**: Set appropriate thresholds to reduce false positives
- **Use Refractory Period**: Use refractory periods to reduce redundant detections
- **Optimize Detection Strategy**: Choose appropriate detection strategy for your use case

## Common Issues and Solutions

### Issue 1: Configuration Validation Failures
**Problem**: Configuration validation fails due to missing required keys.

**Solution**:
```cpp
// Check which keys are missing
std::vector<std::string> missingKeys = config.getMissingRequiredKeys();
for (const auto& key : missingKeys) {
    std::cout << "Missing required key: " << key << std::endl;
}
```

### Issue 2: Memory Usage Issues
**Problem**: High memory usage during neural simulation.

**Solution**:
```cpp
// Use memory-efficient approaches
NeuralActivityCalculator::Config calcConfig;
calcConfig.useMemoryPool = true;
calcConfig.enableCaching = true;

// For spike detection
detector.setMemoryEfficient(true);
```

### Issue 3: Performance Degradation
**Problem**: Performance degradation over time.

**Solution**:
```cpp
// Use efficient utility classes
NeuralActivityCalculator efficientCalculator;
NeuralMath::sigmoidOptimized(sigmoidInput);
```

## Troubleshooting

### Configuration System Issues
- **Error Loading JSON**: Check JSON syntax and file path
- **Validation Failures**: Check schema definitions and required keys
- **Missing Values**: Use safe getters with default values

### Neural Activity Issues
- **Low Activity**: Check calculator parameters and normalization
- **High Activity**: Check threshold settings and noise filtering
- **Spurious Spikes**: Check spike detector thresholds and refractory periods

### Memory Issues
- **Memory Leaks**: Check for proper cleanup of utility objects
- **High Memory Usage**: Use memory-efficient configurations
- **Fragmentation**: Use memory pools where possible

## Conclusion

The NLM system with its enhanced configuration system and unified utility classes provides a powerful and maintainable framework for neural simulation. By following these usage guidelines and best practices, you can effectively utilize the new features and ensure optimal performance of your neural simulations.

For more detailed information, refer to the other documentation files in this guide, particularly the advanced usage guide, error handling reference, and plugin development guide.