# NLM Agent - Enhanced Documentation

This document provides comprehensive information about the NLM (Neural Learning Machine) agent, including its new features, configuration options, command-line interface, and advanced capabilities.

## Overview

NLM is an experimental artificial developmental brain simulation that implements real spiking neural computation. This document covers the enhanced version with advanced configuration, command-line interface, logging, and debugging capabilities.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Command-Line Interface](#command-line-interface)
3. [Configuration System](#configuration-system)
4. [Enhanced Logging](#enhanced-logging)
5. [Performance Monitoring](#performance-monitoring)
6. [Advanced Features](#advanced-features)
7. [Troubleshooting](#troubleshooting)
8. [API Reference](#api-reference)
9. [Examples](#examples)

## Getting Started

### Installation

The NLM agent is built using CMake and requires C++17 compatible compiler. To build and install:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
./nlm_agent
```

### Basic Usage

Run the basic NLM Phase 2 experiment:

```bash
./nlm_agent
```

For help and version information:

```bash
./nlm_agent --help
./nlm_agent -h
./nlm_agent --version
./nlm_agent -v
```

## Command-Line Interface

The enhanced NLM agent provides a comprehensive command-line interface for configuration and control.

### Core Commands

#### `--help` / `-h`
Display help information for all available commands.

#### `--version` / `-v`
Show version and build information.

### Configuration Commands

#### `--neuron <count>`
Set the number of neurons in the network.

**Example:**
```bash
./nlm_agent --neuron 1000
```

#### `--region <count>`
Set the number of neural regions.

**Example:**
```bash
./nlm_agent --region 2
```

#### `--connections <probability>`
Set the probability of synaptic connections between neurons (0.0-1.0).

**Example:**
```bash
./nlm_agent --connections 0.2
```

#### `--curiosity <threshold> [max_chance]`
Set curiosity threshold and exploration chance.

**Example:**
```bash
./nlm_agent --curiosity 0.4 0.5
```

#### `--plasticity [plasticity_factor] [learning_rate]`
Set plasticity base factor and neuromodulation learning rate.

**Example:**
```bash
./nlm_agent --plasticity 0.6 0.1
```

### Feature Toggle Commands

#### `--reward [true|false]`
Enable or disable reward-based neuromodulation.

#### `--development [true|false]`
Enable or disable developmental processes.

#### `--performance [enabled] [timestep] [max_steps]`
Enable performance monitoring and set timestep/max steps.

#### `--debug [true|false]`
Enable or disable debug logging and verbose output.

### Configuration File Commands

#### `--config <file>`
Load configuration from a file (JSON, YAML, or simple key=value format).

**Example:**
```bash
./nlm_agent --config configs/myconfig.json
```

#### `export <filename>`
Export current configuration to a file.

**Example:**
```bash
./nlm_agent export config.json
```

#### `import <filename>`
Import configuration from a file.

**Example:**
```bash
./nlm_agent import settings.cfg
```

#### `reset`
Reset configuration to default values.

**Example:**
```bash
./nlm_agent reset
```

### Advanced Commands

#### `--benchmark [filename]`
Run performance benchmark and optionally save results.

**Example:**
```bash
./nlm_agent --benchmark results.json
```

#### `--list`
List all available commands with their descriptions.

**Example:**
```bash
./nlm_agent list
```

## Configuration System

The enhanced configuration system supports multiple formats and provides robust validation.

### Supported Formats

1. **JSON** (`.json`)
   - Modern, human-readable format
   - Native data types (objects, arrays, primitives)
   - Schema validation support

2. **YAML** (`.yaml`, `.yml`)
   - Human-friendly, readable format
   - Rich type system
   - Nested structures

3. **Simple Format** (`.cfg`)
   - Legacy key=value format
   - Comment support
   - Quote handling

### Configuration Structure

```json
{
    "neural_network": {
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.15
    },
    "agent_behavior": {
        "curiosity_threshold": 0.3,
        "exploration_chance_max": 0.3,
        "enable_reward_modulation": true,
        "enable_development": true
    },
    "performance": {
        "simulation_timestep": 0.001,
        "max_steps": 10000,
        "enable_performance_monitoring": false
    },
    "advanced": {
        "output_format": "json",
        "validate_config": false,
        "enable_debug_mode": false
    }
}
```

### Configuration Validation

The system validates configuration values:

- Neuron count must be positive
- Region count must be positive
- Connection probability must be between 0.0 and 1.0
- Curiosity threshold must be between 0.0 and 1.0
- Simulation timestep must be positive
- Max steps must be positive

### Command-Line Override

Command-line arguments override configuration file values:

```bash
./nlm_agent --config configs/base.json --neuron 1000 --connections 0.2
```

In this example, `--neuron 1000` and `--connections 0.2` override the corresponding values in the JSON file.

## Enhanced Logging

The enhanced logging system provides detailed runtime monitoring and debugging capabilities.

### Log Levels

1. **Trace** (-1): Detailed fine-grained information for debugging
2. **Debug** (0): General debugging information
3. **Info** (1): General information messages
4. **Notice** (2): Important events
5. **Warning** (3): Warnings
6. **Error** (4): Error conditions
7. **Critical** (5): Critical errors
8. **Alert** (6): Action must be taken immediately
9. **Emergency** (7): System is unusable

### Log Output Format

Each log entry includes:

- Timestamp
- Log level
- Context (if specified)
- Source file and line number
- Function name
- Message
- Duration (for performance tracking)

**Example:**
```
[INFO] 2024-01-15 10:30:45 [neuro-simulation] src/brain/Brain.cpp:123 step - Starting neural simulation (duration: 0.500s)
```

### Memory Logging

The memory logger captures log entries in memory for:

- Searching and filtering
- Analysis
- Debugging
- Replay

**Usage:**
```bash
# Configure memory logger with maximum entries
--log-entries 5000

# Search for specific messages
--search "spike" --file "brain.cpp"

# Export logs to file
--export-logs debug.log
```

### Performance Logging

Performance metrics are automatically tracked for:

- Operation timing
- Average durations
- Performance statistics
- Benchmark analysis

**Example Output:**
```
[PERFORMANCE] plasticity experiment took 1500.250ms (duration: 1500.250ms)
[PERF_STAT] Total operations: 42, Average duration: 35.721ms
```

## Performance Monitoring

The enhanced performance monitoring provides comprehensive runtime metrics.

### Monitored Metrics

1. **Operation Timing**: Duration of individual operations
2. **Average Calculations**: Mean and standard deviation
3. **Resource Usage**: Memory, CPU, I/O
4. **Custom Metrics**: User-defined performance indicators

### Performance Statistics

```bash
./nlm_agent --performance true --benchmark benchmark.json
```

Output includes:

- Total operations
- Average duration per operation
- Performance bottlenecks
- System resource utilization

## Advanced Features

### AgentBrain Configuration

The AgentBrain class now supports extensive configuration through the configuration system:

```cpp
// Constructor with configuration parameters
AgentBrain(std::shared_ptr<Brain> brain,
           float curiosityThreshold = AgentBrainConfig::CURIOUS_THRESHOLD,
           float explorationChanceMax = AgentBrainConfig::EXPLORATION_CHANCE_MAX,
           float motorActivationThreshold = AgentBrainConfig::MOTOR_ACTIVATION_THRESHOLD,
           float neuromodulationLearningRate = AgentBrainConfig::REWARD_LEARNING_RATE,
           float plasticityBaseFactor = AgentBrainConfig::PLASTICITY_BASE_FACTOR,
           float noveltyDecayRate = AgentBrainConfig::NOVELTY_DECAY_RATE);
```

### Configuration Constants

All magic numbers have been replaced with named constants:

```cpp
namespace AgentBrainConfig {
    constexpr float CURIOUS_THRESHOLD = 0.3f;
    constexpr float EXPLORATION_THRESHOLD = 0.5f;
    constexpr float EXPLORATION_CHANCE_MAX = 0.3f;
    constexpr float MOTOR_ACTIVATION_THRESHOLD = 0.5f;
    constexpr float REWARD_LEARNING_RATE = 0.05f;
    constexpr float PLASTICITY_BASE_FACTOR = 0.5f;
    constexpr float PLASTICITY_MODIFIER_MIN = 0.1f;
    constexpr float PLASTICITY_MODIFIER_MAX = 2.0f;
    // ... more constants
}
```

### Extensibility Framework

The enhanced NLM agent provides an extensibility framework for:

- Custom agents
- New commands
- Advanced configurations
- Plugin architectures

### Error Handling and Recovery

Enhanced error handling includes:

- Configuration validation
- Graceful degradation
- Recovery mechanisms
- Detailed error messages

## Troubleshooting

### Common Issues and Solutions

#### Issue: Configuration file not found

**Solution:** Ensure the configuration file exists or use default configuration:

```bash
# Create config directory and file
mkdir -p configs
echo "neuron_count = 500" > configs/default.cfg

# Run with explicit config file
./nlm_agent --config configs/default.cfg
```

#### Issue: High memory usage

**Solution:** Reduce memory logger entries or increase system resources:

```bash
# Limit memory logger entries
./nlm_agent --log-entries 1000

# Disable memory logging
./nlm_agent --memory-logging false
```

#### Issue: Performance bottlenecks

**Solution:** Adjust configuration for better performance:

```bash
# Reduce neuron count
./nlm_agent --neuron 100

# Increase simulation timestep
./nlm_agent --performance true 0.01 5000
```

#### Issue: Log output not appearing

**Solution:** Check logging configuration:

```bash
# Enable debug mode
./nlm_agent --debug true

# Set log level to debug
./nlm_agent --log-level debug
```

### Debugging Techniques

1. **Enable Debug Mode:** `--debug true`
2. **Increase Log Verbosity:** `--log-level trace`
3. **Enable Performance Tracking:** `--performance true`
4. **Export Logs:** `--export-logs debug.log`
5. **Use Memory Logger:** Enable for analysis and replay

## API Reference

### Key Classes and Methods

#### Logger Class

```cpp
class Logger {
public:
    static Logger& getGlobal();
    void addLogger(std::shared_ptr<ILogger> logger);
    void setLevel(LogLevel level);
    void info(const std::string& message);
    void error(const std::string& message);
    // ... more methods
};
```

#### CommandLineInterface Class

```cpp
class CommandLineInterface {
public:
    bool parse(int argc, char** argv);
    const AgentConfig& getConfig() const;
    void printHelp(const std::string& command = "");
    bool validateConfig() const;
    // ... more methods
};
```

#### AgentBrain Class

```cpp
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain, ...);
    void enableRewardModulation(bool enable);
    void enableDevelopment(bool enable);
    float getCuriosityLevel() const;
    DevelopmentalStage getDevelopmentalStage() const;
    // ... more methods
};
```

#### Config Class

```cpp
class Config {
public:
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    // ... more methods
};
```

### Configuration File Format

#### JSON Example

```json
{
    "neural_network": {
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.15
    },
    "agent_behavior": {
        "curiosity_threshold": 0.3,
        "enable_reward_modulation": true
    }
}
```

#### YAML Example

```yaml
neural_network:
  neuron_count: 500
  region_count: 1
  connection_probability: 0.15

agent_behavior:
  curiosity_threshold: 0.3
  enable_reward_modulation: true
```

#### Simple Format Example

```
# NLM Agent Configuration
# Generated on: 2024-01-15

neural_network.neuron_count = 500
neural_network.region_count = 1
neural_network.connection_probability = 0.15

agent_behavior.curiosity_threshold = 0.3
agent_behavior.enable_reward_modulation = true
```

## Examples

### Basic Usage Example

```bash
# Run with default configuration
./nlm_agent

# Run with custom configuration
./nlm_agent --config configs/production.json

# Run with performance monitoring
./nlm_agent --performance true --benchmark benchmark.json

# Export configuration
./nlm_agent export myconfig.json

# Import configuration
./nlm_agent import saved_config.yaml
```

### Advanced Usage Example

```bash
# Complex configuration with multiple overrides
./nlm_agent \
    --config configs/base.json \
    --neuron 2000 \
    --connections 0.25 \
    --curiosity 0.4 0.6 \
    --plasticity 0.7 0.15 \
    --development true \
    --reward true \
    --debug true \
    --performance true 0.002 20000 \
    --benchmark results.json
```

### Programmatic Usage Example

```cpp
#include "agent/CommandLineInterface.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"

int main(int argc, char** argv[]) {
    // Parse command line arguments
    nlm::CommandLineInterface cli;
    if (!cli.parse(argc, argv)) {
        std::cerr << "Error: Command line argument parsing failed" << std::endl;
        return 1;
    }
    
    const auto& config = cli.getConfig();
    
    // Use configuration
    NLM_LOG_INFO("Neuron Count: " + std::to_string(config.neuronCount));
    
    // Load additional configuration
    auto brainConfig = std::make_shared<nlm::Config>();
    brainConfig->loadFromFile(config.configFile);
    
    // ... continue with simulation setup
    
    return 0;
}
```

## References

1. NLM Documentation: https://nlm.readthedocs.io
2. GitHub Repository: https://github.com/nlm-project/nlm
3. Issue Tracker: https://github.com/nlm-project/nlm/issues
4. Contributing Guidelines: https://github.com/nlm-project/nlm/blob/main/CONTRIBUTING.md

## Acknowledgments

Special thanks to all contributors who have helped improve the NLM agent. The enhanced features were developed through collaboration with the NLM research community.

## License

This project is licensed under the MIT License. See LICENSE file for details.
