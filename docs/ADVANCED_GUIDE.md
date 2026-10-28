# Advanced NLM User Guide
# Comprehensive Documentation for Neural Learning Machine

## Overview

This guide provides advanced documentation for the NLM (Neural Learning Machine) project, covering the latest improvements in:

- Command-line interface with enhanced configuration management
- Advanced memory systems with multiple eviction policies
- Configuration validation and JSON/YAML support
- Performance optimizations and best practices
- Advanced usage patterns and examples

## Table of Contents

1. [Getting Started with Advanced NLM](#getting-started-with-advanced-nlm)
2. [Command-Line Interface Advanced Features](#command-line-interface-advanced-features)
3. [Enhanced Configuration System](#enhanced-configuration-system)
4. [Advanced Memory Systems](#advanced-memory-systems)
   - [Working Memory](#working-memory)
   - [Episodic Memory](#episodic-memory)
   - [Neural Working Memory](#neural-working-memory)
   - [Neural Episodic Memory](#neural-episodic-memory)
5. [Configuration Management](#configuration-management)
6. [Performance Optimization](#performance-optimization)
7. [Integration and Extension](#integration-and-extension)
8. [Troubleshooting](#troubleshooting)
9. [API Reference](#api-reference)

## Getting Started with Advanced NLM

### Prerequisites

- C++ compiler with C++20 support
- CMake 3.16 or higher
- (Optional) JSON and YAML libraries for advanced configuration

### Basic Setup

```bash
# Clone the repository
clone https://github.com/nlm-project/nlm.git

# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with optimizations
make -j$(nproc)

# Run basic tests
./nlm_test
```

### First Run with CLI

```bash
# Initialize project with improved configuration system
nlm config init

# Run basic simulation
nlm run --config configs/default.cfg --steps 1000

# Run with validation
nlm run --config production.cfg --validate-only
```

## Command-Line Interface Advanced Features

### Enhanced CLI Commands

#### Configuration Management

```bash
# Create default configuration
nlm config init

# List all configuration files
nlm config list

# Show specific configuration values
nlm config show neuron_count
nlm config show connection_probability

# Generate configuration schema
nlm run --generate-schema --config docs/config_schema.json

# Export configuration schema
nlm run --export-schema --config schemas/config_schema.json

# Import configuration schema
nlm run --import-schema --config custom_schema.json
```

#### Running Simulations

```bash
# Basic simulation with custom configuration
nlm run --config my_config.cfg --steps 5000 --verbose

# Run with experiment
nlm run --config test.cfg --experiment connectivity
nlm run --config test.cfg --experiment plasticity
nlm run --config test.cfg --experiment stdp
nlm run --config test.cfg --experiment integration
nlm run --config test.cfg --experiment demo

# Run with validation and output
nlm run --config production.cfg --validate-only --output results.json

# Run Phase 6 integration demo
nlm run --experiment integration

# Run Phase 6 demo with custom settings
nlm run --experiment demo --config demo_config.cfg
```

#### Advanced Configuration Options

```bash
# Validate configuration only
nlm run --config production.cfg --validate-only

# Generate configuration schema
nlm run --generate-schema --config docs/config_schema.json

# Export configuration schema
nlm run --export-schema --config schemas/config_schema.json

# Import configuration schema
nlm run --import-schema --config custom_schema.json

# Show system status
nlm status

# Show help
nlm help
```

### CLI Help Examples

```bash
# Basic help
nlm help

# Configuration help
nlm config --help

# Experiment help
nlm experiment --help

# Version information
nlm --version
```

## Enhanced Configuration System

### Overview

The enhanced configuration system provides:

- **JSON/YAML Support**: Native configuration file format
- **Schema Validation**: Type checking and validation rules
- **Inheritance and Merging**: Combine multiple configuration sources
- **Runtime Overrides**: Command-line and runtime configuration
- **Validation Errors**: Detailed error messages for configuration issues

### JSON Configuration

Example JSON configuration file:

```json
{
  "nlm": {
    "version": "2.0.0",
    "configuration": {
      "random_seed": 42,
      "simulation_timestep": 0.001,
      "neuron_count": 500,
      "region_count": 1,
      "connection_probability": 0.15,
      "plasticity_learning_rate": 0.01,
      "stdp_ltp_weight": 0.02,
      "stdp_ltd_weight": 0.015,
      "stdp_time_constant": 20.0,
      "synaptogenesis_rate": 0.0001,
      "pruning_rate": 0.00001,
      "dopamine_baseline": 0.1,
      "reward_discount_factor": 0.99,
      "environment_name": "GridWorld",
      "environment_width": 10,
      "environment_height": 10,
      "log_level": "INFO",
      "log_to_file": false,
      "log_filename": "nlm.log",
      "max_simulation_steps": 10000,
      "simulation_time_limit": 0.0,
      "visualization_enabled": false,
      "visualization_update_rate": 30.0
    }
  }
}
```

### Configuration Validation

The system validates configuration values against schema rules:

```bash
# Validate configuration file
nlm run --config production.cfg --validate-only

# Output example:
Validating configuration...
Configuration file exists and is readable: production.cfg
JSON syntax is valid
Configuration validation passed!

# Invalid configuration example
{"neuron_count": -5, "connection_probability": 1.5}
```

### Configuration Schema

The configuration system includes built-in schema validation:

```bash
# Generate schema for documentation
nlm run --generate-schema --config docs/config_schema.json

# Example schema entry:
{
  "key": "neuron_count",
  "type": "int64",
  "description": "Number of neurons in the brain",
  "required": true,
  "default": 500,
  "allowed_values": [],
  "validator": "range_1_100000"
}
```

## Advanced Memory Systems

### Working Memory

Enhanced working memory with multiple eviction policies:

#### Eviction Policies

1. **FIFO (First In, First Out)**: Standard queue behavior
2. **LRU (Least Recently Used)**: Evict least recently accessed items
3. **LFU (Least Frequently Used)**: Evict least frequently accessed items
4. **HEBBIAN**: Evict least important items (importance-based)
5. **ADAPTIVE**: Combine importance and access frequency

#### Usage Example

```cpp
#include "core/Memory/Memory_Improved.hpp"

// Create working memory with LRU eviction
auto workingMemory = std::make_shared<nlm::WorkingMemory>(
    1000,  // capacity
    nlm::WorkingMemory::EvictionPolicy::LRU,
    nlm::WorkingMemory::DecayModel::EXPONENTIAL,
    0.001f  // decay rate
);

// Store items
workingMemory->store(neuronId, activationValue, importance);

// Retrieve items
auto value = workingMemory->retrieve(neuronId);

// Get statistics
std::cout << "Capacity: " << workingMemory->getCapacity() << std::endl;
std::cout << "Size: " << workingMemory->getCurrentSize() << std::endl;
std::cout << "Occupancy: " << workingMemory->getOccupancyRatio() * 100 << "%" << std::endl;
```

#### Memory Management Features

- **Decay Models**: Exponential, linear, contextual
- **Importance Tracking**: Configurable importance for eviction
- **Access Statistics**: Track access patterns
- **Batch Operations**: Efficient bulk operations
- **State Management**: Save/load memory state

### Episodic Memory

Enhanced episodic memory with temporal indexing and semantic tagging:

#### Key Features

1. **Temporal Indexing**: Efficient time-based retrieval
2. **Semantic Tagging**: Keyword-based search
3. **Replay Mechanisms**: Neural replay for memory consolidation
4. **Consolidation**: Automatic memory consolidation
5. **Compression**: Memory compression for efficiency

#### Usage Example

```cpp
#include "core/Memory/Memory_Improved.hpp"

// Create episodic memory
auto episodicMemory = std::make_shared<nlm::EpisodicMemory>(
    10000,  // max episodes
    nlm::EpisodicMemory::EncodingStrategy::CONNECTED,
    nlm::EpisodicMemory::ConsolidationStrategy::TRIGGERED,
    0.7f  // consolidation threshold
);

// Store episode
nlm::EpisodicMemoryItem episode;
episode.description = "Mouse pressed button 3";
episode.timestamp = std::chrono::system_clock::now();
episode.coherence = 0.8f;
episode.predictedOutcome = 0.5f;
episode.actualOutcome = 0.6f;

episodicMemory->storeEpisode(episode);

// Retrieve by time
auto recentEpisodes = episodicMemory->getRecentEpisodes(10);

// Retrieve by semantic tag
auto taggedEpisodes = episodicMemory->retrieveEpisodesByTag("food");

// Replay for consolidation
episodicMemory->initiateReplay(0, 1.0f, true);
```

#### Memory Analytics

```cpp
auto stats = episodicMemory->getStats();
std::cout << "Total Episodes: " << stats.totalEpisodes << std::endl;
std::cout << "Max Capacity: " << stats.maxCapacity << std::endl;
std::cout << "Average Coherence: " << stats.avgCoherence << std::endl;
std::cout << "Compression Ratio: " << stats.compressionRatio << std::endl;
```

### Neural Working Memory

Enhanced working memory with neural dynamics and competition:

#### Competition Models

1. **E-I Competition**: Excitatory-inhibitory neural competition
2. **Top-K Selection**: Select top k items based on activation
3. **Gamma Model**: Global inhibition for balanced selection
4. **Renormalization**: Normalize activation values

#### Maintenance Dynamics

1. **Active Maintenance**: Continuous maintenance of active items
2. **Passive Maintenance**: Decay-based maintenance
3. **Spontaneous Activity**: Spontaneous activity-driven maintenance
4. **Noise-Driven**: Maintenance through stochastic processes

### Neural Episodic Memory

Enhanced episodic memory with self-model integration:

#### Self-Model Features

- **Core Identity**: Track self-representation
- **Social Awareness**: Track social interactions
- **Goal Progress**: Track goal achievement
- **Memory Integration**: Integrate with neural processing

#### Timeline Tracking

- **Event Recording**: Record all timeline events
- **Temporal Relationships**: Track temporal relationships
- **Significance Tracking**: Track event significance
- **Coherence Analysis**: Analyze memory coherence

## Configuration Management

### Configuration File Formats

#### .cfg Files (Simple Format)

```cfg
# Simple key=value format
random_seed = 42
simulation_timestep = 0.001
neuron_count = 500
region_count = 1
connection_probability = 0.15
```

#### .json Files (JSON Format)

```json
{
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "neuron_count": 500,
  "region_count": 1,
  "connection_probability": 0.15
}
```

#### .yaml Files (YAML Format)

```yaml
random_seed: 42
simulation_timestep: 0.001
neuron_count: 500
region_count: 1
connection_probability: 0.15
```

### Configuration Merge Strategies

1. **Command Line Overrides**: Command-line arguments override file settings
2. **Schema Validation**: Validate against configuration schema
3. **Default Values**: Apply default values for missing settings
4. **Inheritance**: Inherit settings from parent configurations

## Performance Optimization

### Memory Optimization

1. **Memory Pools**: Use memory pools for efficient allocation
2. **Cache-Friendly Data Structures**: Optimize for CPU cache
3. **Batch Operations**: Minimize function call overhead
4. **Compression**: Compress memory when possible

### CPU Optimization

1. **SIMD Instructions**: Use SIMD for vector operations
2. **Parallel Processing**: Parallelize independent operations
3. **Cache Optimization**: Optimize memory access patterns
4. **Branch Prediction**: Optimize conditional branches

### I/O Optimization

1. **Buffered I/O**: Use buffered I/O for file operations
2. **Asynchronous I/O**: Use asynchronous I/O for large files
3. **Compression**: Compress large configuration files
4. **Streaming**: Stream large data files

## Integration and Extension

### Extending Memory Systems

```cpp
// Extend working memory with custom eviction policy
class CustomEvictionPolicy : public WorkingMemory::EvictionPolicy {
public:
    void evict(WorkingMemory& memory) override {
        // Custom eviction logic
    }
};

// Create working memory with custom policy
auto customMemory = std::make_shared<nlm::WorkingMemory>(
    1000,
    CustomEvictionPolicy(),
    nlm::WorkingMemory::DecayModel::EXPONENTIAL,
    0.001f
);
```

### Integrating with Other Systems

```cpp
// Integrate memory systems with neuromodulation
void integrateWithNeuromodulation(MemorySystem& memorySystem, Neuromodulator& neuromodulator) {
    memorySystem.connectToNeuromodulation(std::make_shared<Neuromodulator>(neuromodulator));
}

// Integrate memory systems with development
void integrateWithDevelopment(MemorySystem& memorySystem, DevelopmentSystem& development) {
    memorySystem.connectToDevelopment(std::make_shared<DevelopmentSystem>(development));
}

// Integrate memory systems with plasticity
void integrateWithPlasticity(MemorySystem& memorySystem, PlasticitySystem& plasticity) {
    memorySystem.connectToPlasticity(std::make_shared<PlasticitySystem>(plasticity));
}
```

## Troubleshooting

### Common Issues

#### Configuration File Not Found

```bash
# Error: Configuration file not found: configs/default.cfg
# Solution: Run 'nlm config init' to create default configuration
nlm config init
```

#### Invalid JSON Configuration

```bash
# Error: Invalid JSON syntax in configuration file
# Solution: Check JSON syntax with a JSON validator
# Use: python3 -m json.tool config.json
```

#### Memory Usage Exceeds Capacity

```cpp
// Error: Working memory capacity exceeded
// Solution: Increase capacity or implement custom eviction policy
auto memory = std::make_shared<nlm::WorkingMemory>(2000);
```

#### Performance Issues

```cpp
// Error: Simulation too slow
// Solution: Optimize configuration or increase step size
memory->setEvictionPolicy(nlm::WorkingMemory::EvictionPolicy::LRU);
memory->setDecayModel(nlm::WorkingMemory::DecayModel::NONE);
```

### Diagnostic Commands

```bash
# Show system status
nlm status

# Validate configuration
nlm run --config test.cfg --validate-only

# Show configuration summary
nlm config show

# Check for available binaries
nlm status | grep "Available Binaries"
```

## API Reference

### Core Classes

#### nlm::WorkingMemory
- `store(NeuronId, float, float)`: Store item with importance
- `retrieve(NeuronId)`: Retrieve item
- `contains(NeuronId)`: Check if item exists
- `getValue(NeuronId)`: Get current value
- `remove(NeuronId)`: Remove item
- `clear()`: Clear all items
- `getCapacity()`: Get capacity
- `getCurrentSize()`: Get current size
- `getOccupancyRatio()`: Get occupancy ratio

#### nlm::EpisodicMemory
- `storeEpisode(EpisodicMemoryItem)`: Store episode
- `retrieveEpisode(size_t)`: Retrieve episode
- `retrieveEpisodesByTime(...)`: Retrieve by time range
- `retrieveEpisodesByTag(...)`: Retrieve by semantic tag
- `getRecentEpisodes(size_t)`: Get recent episodes
- `searchEpisodes(string)`: Search episodes
- `getEpisodeCount()`: Get episode count
- `getMaxEpisodes()`: Get max episodes
- `setMaxEpisodes(size_t)`: Set max episodes

#### nlm::MemorySystem
- `initialize()`: Initialize memory systems
- `connectAll()`: Connect all systems
- `update(float)`: Update all systems
- `getWorkingMemory()`: Get working memory
- `getEpisodicMemory()`: Get episodic memory
- `getStats()`: Get system statistics

### Configuration Classes

#### nlm::Config
- `loadFromFile(string)`: Load from file
- `loadFromArgs(int, char**)`: Load from command line
- `loadFromJson(string)`: Load from JSON string
- `saveToFile(string)`: Save to file
- `get<T>(string)`: Get value
- `getOr<T>(string, T)`: Get value with default
- `set(string, ConfigValue)`: Set value
- `has(string)`: Check if key exists
- `remove(string)`: Remove key
- `clear()`: Clear all
- `summary()`: Get summary
- `validate()`: Validate configuration

#### nlm::ConfigSchema
- `addEntry(string, string, string, bool, ConfigValue, vector<string>)`: Add schema entry
- `addValidator(string, function)`: Add validator
- `validate(string, ConfigValue)`: Validate value
- `validateAll(map)`: Validate all
- `getEntry(string)`: Get schema entry
- `getKeys()`: Get keys
- `hasKey(string)`: Check if key exists
- `toJson()`: Convert to JSON
- `loadFromJson(json)`: Load from JSON
- `loadFromFile(string)`: Load from file

## Conclusion

The enhanced NLM project provides a comprehensive neural simulation framework with:

1. **Advanced Configuration Management**: JSON/YAML support with validation
2. **Enhanced Memory Systems**: Multiple memory types with sophisticated management
3. **Improved Command-Line Interface**: Rich CLI with extensive options
4. **Performance Optimizations**: Optimized for speed and efficiency
5. **Extensive Documentation**: Comprehensive guides and examples
6. **Integration Support**: Easy integration with other systems
7. **Robust Testing**: Comprehensive test coverage

These improvements make NLM more powerful, easier to use, and better suited for advanced neural simulation research and applications.

## References

- [NLM Documentation](https://nlm.readthedocs.io)
- [NLM GitHub Repository](https://github.com/nlm-project/nlm)
- [C++ Standards Documentation](https://isocpp.org/cppstandard)
- [JSON Specification](https://json.org)
- [YAML Specification](https://yaml.org)