# NLM Unified CLI - Documentation

## Overview

The NLM (熙然) - Neural Learning Machine unified CLI provides a comprehensive command-line interface for all phases of the neural network simulation project. It unifies the previously separate Phase 2 and Phase 6 interfaces into a single, powerful command-line tool.

## Key Features

### Unified Interface
- Single entry point for all phases (Phase 2, Phase 3, Phase 4, Phase 6)
- Consistent command-line argument structure across all modes
- Comprehensive help system with detailed usage information

### Enhanced Configuration Support
- JSON configuration file support
- YAML configuration file support
- Simple key=value format (original format)
- Configuration validation and schema checking
- Configuration templates and profiles

### Advanced Error Handling
- Comprehensive error classification (Configuration, Memory, Simulation, Input, IO, Network, Plugin, System, User, Unknown)
- Error recovery mechanisms
- Detailed error reporting with context
- Global error handler management

### Performance Monitoring
- Real-time performance metrics
- CPU, memory, and I/O monitoring
- Simulation efficiency tracking
- Resource limit checking

### Plugin System
- Extensible plugin architecture
- Plugin discovery and loading
- Event-driven plugin communication
- Plugin command execution

### Advanced Scripting
- Multi-language support (Lua, Python, JavaScript)
- Script execution with variable substitution
- Event-driven scripting
- Script debugging and validation

### Extension API
- Third-party extension registration
- Service provision and discovery
- Message-based communication between extensions
- Security and validation framework

## Command Line Interface

### Basic Usage

```bash
# Show help
nlm --help

# Show version
nlm --version

# List available configurations
nlm --list-configs

# Validate configuration file
nlm --validate-config my_config.json
```

### Mode Selection

```bash
# Phase 2: Real Neural Computation Demo
nlm --mode phase2

# Phase 3: Basic World Experiment
nlm --mode phase3 --config my_config.cfg

# Phase 4: Advanced Learning Experiment
nlm --mode phase4 --verbose

# Phase 6: Integration Demo
nlm --mode phase6 --config phase6_config.json
```

### Configuration Options

```bash
# Load configuration from file
--config FILE              Load configuration from FILE

# Enable verbose output
--verbose                 Enable verbose logging

# Suppress output
--quiet, -q               Suppress all output

# Save output to file
--output FILE             Save output to FILE

# Validate configuration
--validate-config FILE     Validate configuration file
```

### Configuration File Formats

#### Simple (key=value)

```
# Comment lines start with # or /
random_seed = 12345
neuron_count = 1000
simulation_timestep = 0.001
connection_probability = 0.1
```

#### JSON

```json
{
  "random_seed": 12345,
  "neuron_count": 1000,
  "simulation_timestep": 0.001,
  "connection_probability": 0.1
}
```

#### YAML

```yaml
random_seed: 12345
neuron_count: 1000
simulation_timestep: 0.001
connection_probability: 0.1
```

## Configuration Management

### Available Configuration Templates

The unified CLI provides several built-in configuration templates:

1. **default**: Basic configuration for standard use
2. **phase2**: Optimized for Phase 2 neural computation demo
3. **phase6**: Configured for Phase 6 integration demo
4. **debug**: Configuration optimized for debugging

### Configuration Validation

The configuration system validates settings against a schema:

- Required settings: `random_seed`, `neuron_count`, `simulation_timestep`
- Type checking for all settings
- Range validation where applicable
- Dependency validation between settings

### Configuration Profiles

Users can save and load configuration profiles for quick access to common settings:

```bash
# Save current configuration as a profile
nlm --mode phase6 --config my_config.cfg  # Saves as "my_config_profile"

# Load a saved profile
nlm --mode phase6 --config my_config_profile
```

## Error Handling

### Error Categories

NLM errors are classified into 9 categories:

1. **Configuration**: Configuration file loading, parsing, and validation errors
2. **Memory**: Memory allocation and management errors
3. **Simulation**: Simulation execution and neural computation errors
4. **Input**: User input and argument errors
5. **IO**: File I/O errors
6. **Network**: Network communication errors
7. **Plugin**: Plugin loading and execution errors
8. **System**: System resource and call errors
9. **User**: User interruption and exit errors

### Error Handling Modes

The unified CLI supports three error handling modes:

1. **Standard Mode**: Shows error message and exits
2. **Verbose Mode**: Shows detailed error information including stack traces
3. **Debug Mode**: Enables additional debugging information

### Error Recovery

The system provides automatic error recovery for non-fatal errors:

- Configuration file errors: Fall back to default configuration
- Memory errors: Attempt cleanup and retry
- Simulation errors: Skip problematic steps and continue

## Performance Monitoring

### Performance Metrics

The performance monitor tracks:

- **CPU Usage**: Current CPU utilization percentage
- **Memory Usage**: Current memory consumption in MB
- **Memory Usage Percent**: Current memory usage as percentage
- **Disk I/O**: Disk read/write operations in MB/s
- **Network I/O**: Network upload/download in MB/s
- **Active Threads**: Number of active threads
- **Total Memory**: Total available memory in MB
- **Simulation Time**: Elapsed simulation time in seconds
- **Real Time**: Elapsed real time in seconds
- **Steps Per Second**: Simulation steps per second
- **Spike Count**: Total number of neural spikes
- **Average Firing Rate**: Average neural firing rate
- **Peak Firing Rate**: Maximum neural firing rate

### Performance Analysis

The performance analysis provides:

- **Average CPU Usage**: Mean CPU utilization over time
- **Average Memory Usage**: Mean memory consumption over time
- **Peak Memory Usage**: Maximum memory consumption
- **Average Steps Per Second**: Mean simulation performance
- **Simulation Efficiency**: Ratio of simulation steps to real time
- **Performance Grade**: Overall performance rating (A-F)
- **Recommendations**: Actionable performance improvement suggestions

## Plugin System

### Plugin Architecture

The plugin system allows third-party developers to extend NLM functionality:

#### Plugin Interface

```cpp
class NLMPlugin {
public:
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::vector<std::string> getCommands() const = 0;
    
    virtual bool initialize(const std::shared_ptr<Config>& config) = 0;
    virtual void shutdown() = 0;
    
    virtual bool handleCommand(const std::string& command,
                              const std::vector<std::string>& args,
                              std::string& output) = 0;
    
    virtual void onSimulationStep(std::shared_ptr<Brain> brain,
                                  double timestep) = 0;
    
    virtual void onMemoryEvent(const std::string& eventType,
                              const std::map<std::string, std::string>& data) = 0;
};
```

#### Plugin Commands

Plugins can define custom commands:

```bash
# Example plugin command
nlm --mode phase6 --plugin myplugin --command mycommand --arg1 value1 --arg2 value2
```

### Plugin Development

#### Basic Plugin Example

```cpp
class MyPlugin : public NLMPlugin {
public:
    std::string getName() const override { return "MyPlugin"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { 
        return "Example plugin for NLM demonstration"; 
    }
    std::vector<std::string> getCommands() const override { 
        return {"help", "info", "stats"}; 
    }
    
    bool initialize(const std::shared_ptr<Config>& config) override {
        // Initialize plugin
        return true;
    }
    
    void shutdown() override {
        // Clean up plugin
    }
    
    bool handleCommand(const std::string& command,
                     const std::vector<std::string>& args,
                     std::string& output) override {
        if (command == "help") {
            output = "Shows help information";
            return true;
        } else if (command == "info") {
            output = "Plugin information and status";
            return true;
        } else if (command == "stats") {
            output = "Plugin statistics and metrics";
            return true;
        }
        return false;
    }
    
    void onSimulationStep(std::shared_ptr<Brain> brain, double timestep) override {
        // Handle simulation step
    }
    
    void onMemoryEvent(const std::string& eventType,
                      const std::map<std::string, std::string>& data) override {
        // Handle memory event
    }
};
```

## Advanced Scripting

### Supported Languages

The scripting system supports multiple languages:

1. **Lua**: Lightweight, embeddable scripting language
2. **Python**: Full-featured programming language
3. **JavaScript**: Web-oriented scripting language
4. **Custom**: User-defined scripting language

### Script Execution

#### Basic Script Execution

```bash
# Execute a Lua script
nlm --mode phase6 --script myscript.lua

# Execute a Python script
nlm --mode phase6 --script myscript.py

# Execute with variables
nlm --mode phase6 --script myscript.lua --var neuron_count=1000
```

#### Script Context

```cpp
struct ScriptContext {
    std::string name;
    Language language;
    std::string code;
    std::map<std::string, std::string> variables;
    std::map<std::string, std::function<std::string(const std::vector<std::string>&)>> functions;
    bool enabled;
};
```

### Script Event Handling

Scripts can register to handle events:

```cpp
void AdvancedScripting::registerEventHandler(
    const std::string& eventType,
    const std::string& scriptName,
    const std::string& functionName
) {
    registeredEvents_[eventType].push_back({scriptName, functionName});
}

void AdvancedScripting::triggerEvent(
    const std::string& eventType,
    const std::map<std::string, std::string>& data
) {
    // Notify registered scripts of the event
}
```

## Extension API

### Extension Registration

Third-party developers can register extensions:

```cpp
void ExtensionAPI::registerExtension(
    const std::string& name,
    const std::string& version,
    const std::string& description,
    const std::function<void()>& init,
    const std::function<void()>& shutdown
) {
    // Register extension
}
```

### Extension Services

Extensions can provide and consume services:

```cpp
template<typename T>
bool ExtensionAPI::provideService(const std::string& name, T service) {
    // Provide a service
}

template<typename T>
std::optional<T> ExtensionAPI::getService(const std::string& name) {
    // Get a service
}
```

### Extension Communication

Extensions communicate through message passing:

```cpp
void ExtensionAPI::sendMessage(
    const std::string& extensionName,
    const std::string& messageType,
    const std::map<std::string, std::string>& data
) {
    // Send message to another extension
}
```

## Development Tools

### Brain State Inspection

Inspect the current state of the brain simulation:

```cpp
void DevelopmentTools::getBrainState(std::shared_ptr<Brain> brain) {
    // Return detailed brain state information
}
```

### Simulation Control

Control simulation execution:

```cpp
void DevelopmentTools::SimulationController::pauseSimulation() {
    controlMode_ = ControlMode::Pause;
}

void DevelopmentTools::SimulationController::resumeSimulation() {
    controlMode_ = ControlMode::Normal;
}

void DevelopmentTools::SimulationController::stepForward() {
    controlMode_ = ControlMode::StepByStep;
}
```

### Debugging and Profiling

Access debugging and profiling tools:

```cpp
void DevelopmentTools::enableDebugMode(bool enable) {
    debugMode_ = enable;
}

DevelopmentTools::Profiler& DevelopmentTools::getProfiler() {
    return profiler_;
}
```

## Performance Monitoring

### Real-time Metrics

Monitor system performance in real-time:

```cpp
PerformanceMonitor::PerformanceMetrics metrics = monitor.getCurrentMetrics();
std::cout << "CPU Usage: " << metrics.cpuUsage << "%" << std::endl;
std::cout << "Memory Usage: " << metrics.memoryUsage << " MB" << std::endl;
std::cout << "Simulation Time: " << metrics.simulationTime << " s" << std::endl;
```

### Performance Analysis

Analyze historical performance data:

```cpp
PerformanceMonitor::PerformanceAnalysis analysis = monitor.analyzePerformance();
std::cout << "Performance Grade: " << analysis.performanceGrade << std::endl;
for (const auto& recommendation : analysis.recommendations) {
    std::cout << "- " << recommendation << std::endl;
}
```

## Configuration Management

### Configuration Templates

Access pre-defined configuration templates:

```cpp
AdvancedConfig::ConfigTemplate template = config.getTemplate("phase6");
config.exportConfig(template.config, "phase6_template.json");
```

### Configuration Validation

Validate configuration against schema:

```cpp
AdvancedConfig::ValidationResult result = config.validateConfig(config);
if (!result.isValid()) {
    for (const auto& error : result.getErrors()) {
        std::cout << "ERROR: " << error << std::endl;
    }
}
for (const auto& warning : result.getWarnings()) {
    std::cout << "WARNING: " << warning << std::endl;
}
```

## API Reference

### Core Classes

#### UnifiedCLI

```cpp
class UnifiedCLI {
public:
    int run(int argc, char** argv);
    void showHelp(const std::string& programName);
    void showVersion();
    int executeMode(const UnifiedCLIOptions& options);
    // ... other methods
};
```

#### Config

```cpp
class Config {
public:
    bool loadFromFile(const std::string& filepath);
    std::string getOr(const std::string& key, const T& defaultValue) const;
    void set(const std::string& key, const std::string& value, ConfigSource source);
    // ... other methods
};
```

#### ErrorHandling

```cpp
namespace nlm {
    class NLMError : public std::runtime_error {
    public:
        NLMError(const std::string& message, NLMErrorCode code, NLMErrorCategory category);
        NLMErrorCode getCode() const;
        NLMErrorCategory getCategory() const;
        std::string formatFullError() const;
    };
    
    class ErrorHandlerManager {
    public:
        void handleError(const NLMError& error);
        bool shouldExit(const NLMError& error);
        RecoveryAction getRecoveryAction(const NLMError& error);
    };
}
```

### Namespace Functions

#### nlm::parseCommandLine

```cpp
UnifiedCLIOptions nlm::parseCommandLine(int argc, char** argv);
```

#### nlm::g_cli

```cpp
inline UnifiedCLI nlm::g_cli;
```

## Migration Guide

### From Legacy CLI

If you're migrating from the legacy separate executables (nlm_phase2 and nlm_phase6_demo), use the following:

```bash
# Old Phase 2 command
./nlm_phase2 --config config.cfg

# New unified command
nlm --mode phase2 --config config.cfg

# Old Phase 6 command
./nlm_phase6_demo

# New unified command
nlm --mode phase6
```

### Configuration Migration

Configuration files in the old simple format are automatically compatible:

```ini
# Old format
random_seed = 12345
neuron_count = 1000
simulation_timestep = 0.001
```

Can be used directly with the new unified CLI.

## Troubleshooting

### Common Issues and Solutions

1. **Configuration File Not Found**
   - Solution: Ensure the configuration file exists and is readable
   - Command: `ls -la my_config.cfg`

2. **Invalid Configuration**
   - Solution: Check configuration file syntax and required settings
   - Command: `nlm --validate-config my_config.cfg`

3. **Performance Issues**
   - Solution: Check system resources and reduce neuron count if needed
   - Command: `nlm --mode phase6 --config debug_config.json`

4. **Plugin Loading Errors**
   - Solution: Check plugin compatibility and file permissions
   - Command: `ls -la plugin_directory/*.so`

## Examples

### Complete Example

```bash
#!/bin/bash

# Create a configuration file
cat > my_config.json << EOF
{
  "random_seed": 12345,
  "neuron_count": 1000,
  "region_count": 1,
  "simulation_timestep": 0.001,
  "connection_probability": 0.1,
  "max_steps": 5000,
  "enable_curiosity": true,
  "enable_development": true,
  "enable_replay": true
}
EOF

# Run Phase 6 integration demo with configuration
nlm --mode phase6 --config my_config.json --verbose --output results.txt

# List plugins
nlm --mode phase6 --plugin list

# Execute plugin command
nlm --mode phase6 --plugin myplugin --command stats
```

### Programming Example

```cpp
#include "UnifiedCLI.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

int main(int argc, char** argv) {
    nlm::UnifiedCLI cli;
    
    // Parse arguments
    nlm::UnifiedCLIOptions options = cli.parseArguments(argc, argv);
    
    // Load configuration
    auto config = std::make_shared<nlm::Config>();
    config->loadFromFile(options.configFile);
    
    // Set additional configuration
    config->set("verbose_logging", options.verbose, nlm::ConfigSource::CommandLine);
    
    // Execute based on mode
    return cli.executeMode(options);
}
```

## Conclusion

The NLM unified CLI provides a comprehensive, extensible, and user-friendly command-line interface for all phases of the neural network simulation project. It includes advanced features such as plugin support, scripting, performance monitoring, and comprehensive error handling, making it suitable for both beginners and advanced users.

The unified architecture simplifies the user experience while providing powerful tools for development, debugging, and performance optimization. Whether you're running simple simulations or complex integration tests, the unified CLI offers a consistent and intuitive interface for all NLM operations.