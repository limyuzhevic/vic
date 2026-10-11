# Front Matter

title: "Advanced NLM Usage Guide"
description: "Complete guide to using NLM's advanced CLI and plugin system for power users"
author: "NLM Development Team"
date: "2026-10-11"
version: "2.0.0"
last_updated: "2026-10-11"

# Advanced NLM Usage Guide

## Overview

This guide provides comprehensive documentation for advanced users who want to leverage NLM's (Neural Learning Machine) advanced command-line interface (CLI) and plugin system. It extends beyond basic usage to provide power users with complete control, customization, and extensibility options.

## Table of Contents

1. [Introduction to Advanced CLI](#introduction-to-advanced-cli)
2. [Plugin System Architecture](#plugin-system-architecture)
3. [Plugin Management Commands](#plugin-management-commands)
4. [Brain State Management](#brain-state-management)
5. [Experiment Configuration](#experiment-configuration)
6. [Performance Monitoring](#performance-monitoring)
7. [Batch Processing and Scripting](#batch-processing-and-scripting)
8. [Example Plugins](#example-plugins)
9. [Configuration Reference](#configuration-reference)
10. [Troubleshooting](#troubleshooting)
11. [API Reference](#api-reference)
12. [Contributing Plugins](#contributing-plugins)

## Introduction to Advanced CLI

The advanced CLI provides a comprehensive interface for controlling NLM sessions, managing brain states, running experiments, and extending functionality through plugins.

### Key Features

- **Interactive Mode**: Real-time command execution with auto-completion
- **Batch Mode**: Execute command scripts from files
- **Plugin Management**: Load, unload, and manage plugins dynamically
- **Experiment Control**: Run complex experiments with custom configurations
- **Performance Monitoring**: Profile and optimize NLM performance
- **Brain State Persistence**: Save and load brain states for reproducibility
- **Custom Configuration**: Extend configuration system with custom parameters

### CLI Usage Modes

#### Interactive Mode

Start an interactive NLM session:

```bash
./nlm_advanced_cli
```

Type commands at the prompt:

```
> save my_brain_state.json
> load my_brain_state.json
> run plasticity_experiment --steps 1000 --learning-rate 0.05
> plugins
> help
```

#### Batch Mode

Run commands from a script file:

```bash
./nlm_advanced_cli batch my_script.txt
```

Batch script format:
```
# NLM Advanced CLI Script
# This is a comment

# Save current brain state
save session_1_state.json

# Run experiment with custom parameters
run plasticity_experiment --steps 1000 --learning-rate 0.05

# Show system status
plugins
stats

# Load saved state
load session_1_state.json
```

## Plugin System Architecture

### Plugin Interface

The plugin system uses a standard interface for extensibility:

#### Core Plugin Types

1. **Plasticity Plugins**: Custom synaptic modification rules
2. **Neuromodulator Plugins**: Custom chemical signals
3. **Sensory Plugins**: Custom sensory processing
4. **Action Plugins**: Custom motor control
5. **Visualization Plugins**: Custom output formats
6. **Development Plugins**: Custom developmental processes

#### Plugin Lifecycle

Each plugin follows a standard lifecycle:

1. **Initialization**: Plugin is loaded and configured
2. **Registration**: Plugin registers with the plugin manager
3. **Active**: Plugin receives messages and processes data
4. **Shutdown**: Plugin cleans up and unregisters
5. **Unload**: Plugin library is unloaded from memory

### Plugin Manager

The PluginManager class handles:

- Dynamic loading and unloading of plugin libraries
- Plugin dependency resolution and validation
- Plugin metadata and versioning
- Communication between plugins
- Plugin state tracking

### Example Plugin Types

#### Custom Plasticity Rule Plugin

```cpp
class CustomPlasticityPlugin : public Plugin {
public:
    bool initialize(std::shared_ptr<PluginManager> manager, 
                   std::shared_ptr<Config> config) override;
    
    void update(TimestepDuration dt) override;
    
    bool onPluginMessage(const std::string& message, 
                        const std::vector<std::string>& data) override;
    
    // Implement custom plasticity rule logic
    void computeCustomPlasticity(Synapse* synapse, 
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt);
};
```

#### Custom Neuromodulator Plugin

```cpp
class CustomNeuromodulatorPlugin : public Plugin {
public:
    void update(TimestepDuration dt) override;
    
    // Implement neuromodulation effects
    float getPlasticityFactor() const override;
    float getDopamineLevel() const;
    float getNoveltyLevel() const;
    
    // Handle reward signals
    void signalReward(float reward);
    void signalPredictionError(float error);
};
```

#### Custom Sensory Processing Plugin

```cpp
class CustomSensoryPlugin : public Plugin {
public:
    void update(TimestepDuration dt) override;
    
    // Implement sensory processing
    void processVisionInput(const std::vector<float>& data, float timestamp);
    void processAudioInput(const std::vector<float>& data, float timestamp);
    std::vector<float> extractFeatures(const std::vector<float>& data);
};
```

## Plugin Management Commands

### Core Plugin Commands

#### `plugins`

List all loaded plugins with their information:

```bash
> plugins
```

Output:
```
Loaded Plugins:
===============
1. CustomPlasticityPlugin (v1.2.0)
   Author: NLM Development Team
   Description: Custom plasticity rule implementation...
   Status: Active
   Dependencies: brain, synapse, neuron

2. CustomNeuromodulatorPlugin (v1.3.0)
   Author: NLM Development Team
   Description: Advanced neuromodulator system...
   Status: Active
   Dependencies: brain, sensory, action, reward

3. CustomSensoryPlugin (v1.1.0)
   Author: NLM Development Team
   Description: Advanced sensory processing...
   Status: Active
   Dependencies: brain, sensory, attention, motor
```

#### `load-plugin <plugin_name> [path]`

Load a plugin dynamically:

```bash
> load-plugin CustomPlasticityPlugin ./plugins/libCustomPlasticityPlugin.so
> load-plugin CustomNeuromodulatorPlugin
```

Parameters:
- `plugin_name`: Name of the plugin to load
- `path`: Optional path to plugin library (defaults to `./plugins/<plugin_name>.so`)

#### `unload-plugin <plugin_name>`

Unload a loaded plugin:

```bash
> unload-plugin CustomPlasticityPlugin
```

#### `plugin-info <plugin_name>`

Get detailed information about a specific plugin:

```bash
> plugin-info CustomNeuromodulatorPlugin
```

Output includes plugin metadata, configuration, and current state.

### Plugin Development Commands

#### `plugin-help`

Get help information for plugin development:

```bash
> plugin-help
```

Lists available plugin types and provides development guidance.

#### `plugin-validate <plugin_name>`

Validate a plugin library:

```bash
> plugin-validate CustomPlasticityPlugin
```

Checks for required functions, proper formatting, and dependencies.

## Brain State Management

### Save Brain State

Save the current brain state to a file:

```bash
> save my_session_state.json
> save session_backup.json --compress
```

Parameters:
- `filename`: Output file name
- `--compress`: Enable compression (reduces file size)
- `--format json`: Output format (json, binary, or both)

### Load Brain State

Load a previously saved brain state:

```bash
> load session_backup.json
> load my_session_state.json --verify
```

Parameters:
- `filename`: Input file name
- `--verify`: Verify file integrity before loading
- `--merge`: Merge with current brain state instead of replacing

### Reset Brain State

Reset the brain to its initial state:

```bash
> reset
> reset --clear-synapses  # Also clear synaptic connections
> reset --preserve-memory  # Keep memory states
```

### Brain State Information

Get information about the current brain state:

```bash
> brain-info
> brain-stats --verbose
```

Shows neuron count, synapse count, spike statistics, and memory usage.

## Experiment Configuration

### Running Experiments

Run predefined experiments with custom parameters:

```bash
> run plasticity --steps 1000 --learning-rate 0.05 --decay-rate 0.001
> run novelty --threshold 0.8 --window 100
> run curiosity --exploration-factor 2.0
```

Available experiments:
- `plasticity`: Synaptic modification experiments
- `novelty`: Novelty detection and response
- `curiosity`: Exploration and curiosity-driven learning
- `prediction`: Prediction error and learning
- `social`: Social learning and imitation

### Experiment Configuration File

Create a configuration file for complex experiments:

```bash
declarative_custom_plasticity_config.yaml
```

```yaml
title: "Complex Plasticity Experiment"
parameters:
  steps: 1000
  learning_rate: 0.05
  decay_rate: 0.001
  enable_stdp: true
  enable_hebbian: true
  enable_structural: false

conditions:
  input_pattern: "alternating"
  reward_schedule: "sparse"
  noise_level: 0.1

plugins:
  - CustomPlasticityPlugin
  - CustomNeuromodulatorPlugin
  ```

Run with configuration:

```bash
> run plasticity --config experiment_config.yaml
```

### Experiment Monitoring

Monitor running experiments:

```bash
> monitor plasticity --output metrics.json --interval 0.1
> experiment-info plasticity
```

Parameters:
- `--output`: File to save metrics
- `--interval`: Monitoring interval in seconds
- `--verbose`: Show detailed progress

## Performance Monitoring

### Performance Profiling

Start performance profiling:

```bash
> profile start --output profile_data.json
> profile stop
```

Track performance metrics:

```bash
> stats --cpu --memory --network
> performance-report --format html
```

### Performance Metrics

Available metrics:

#### Neural Performance
- **Firing Rate**: Average spikes per second per neuron
- **Synapse Activity**: Number of active synapses
- **Network Synchronization**: Measure of coordinated activity

#### Computational Performance
- **CPU Usage**: Processor utilization
- **Memory Usage**: RAM consumption
- **I/O Operations**: File and network operations

#### Learning Metrics
- **Plasticity Rate**: Speed of synaptic modification
- **Convergence**: How quickly learning stabilizes
- **Stability**: Resistance to catastrophic forgetting

### Performance Analysis

Generate performance analysis reports:

```bash
> analyze performance --time-range 24h --output summary.pdf
> compare-performance old_config new_config
```

## Batch Processing and Scripting

### Batch Script Examples

#### Simple Batch Script

```bash
# batch_script.txt
# Save initial state
save initial_state.json

# Run learning experiment
run plasticity --steps 500

# Show progress
stats --verbose

# Load saved state
load initial_state.json

# Run exploration experiment
run curiosity --exploration-factor 1.5

# Save final state
save final_state.json
```

#### Automated Experiment Pipeline

```bash
# pipeline.txt
#!/usr/bin/env bash

# Setup environment
load-plugin CustomPlasticityPlugin
load-plugin CustomNeuromodulatorPlugin

# Run parameter sweep
for steps in 100 500 1000; do
    for rate in 0.01 0.05 0.1; do
        run plasticity --steps $steps --learning-rate $rate
        save results_$(date +%s).json
    done
done

# Analyze results
analyze-results --pattern "results_*.json" --output analysis.csv

# Cleanup
unload-plugin CustomPlasticityPlugin
unload-plugin CustomNeuromodulatorPlugin
```

### Batch Command Syntax

Batch commands support:

1. **Simple commands**: `save brain_state.json`
2. **Commands with arguments**: `run plasticity --steps 1000`
3. **Conditional execution**: `if stats --spikes > 1000 then save high_activity.json`
4. **Loops**: `for i in 1 10 100 do run plasticity --steps $i done`
5. **Variable assignment**: `steps=500 run plasticity --steps $steps`

## Example Plugins

### 1. CustomPlasticityPlugin

Implements an adaptive plasticity rule that adjusts learning rates based on network activity:

- **Type**: Plasticity Plugin
- **Location**: `src/plugins/CustomPlasticityPlugin.cpp`
- **Features**:
  - Activity-dependent learning rates
  - Custom STDP variant with sigmoidal modulation
  - Hebbian component for co-activation
  - Integration with neuromodulators

### 2. CustomNeuromodulatorPlugin

Implements an advanced neuromodulator system with adaptive gain control:

- **Type**: Neuromodulator Plugin
- **Location**: `src/plugins/CustomNeuromodulatorPlugin.cpp`
- **Features**:
  - Adaptive dopamine regulation
  - Novelty detection and gain modulation
  - Curiosity-driven exploration
  - Action value learning

### 3. CustomSensoryPlugin

Implements advanced sensory processing with selective attention:

- **Type**: Sensory Plugin
- **Location**: `src/plugins/CustomSensoryPlugin.cpp`
- **Features**:
  - Multimodal sensory fusion
  - Feature extraction and analysis
  - Selective attention mechanism
  - Adaptive thresholding

## Configuration Reference

### Core Configuration

#### Basic Settings
```yaml
nlm:
  general:
    neuron_count: 1000
    region_count: 2
    random_seed: 42
    simulation_timestep: 0.001
  
  plugins:
    directory: "./plugins"
    auto_load: ["CustomPlasticityPlugin"]
```

#### CLI Configuration
```yaml
nlm:
  cli:
    interactive_mode: true
    auto_complete: true
    command_history: true
    help_format: "detailed"
    completion_delay: 0.1
```

#### Performance Configuration
```yaml
nlm:
  performance:
    profiling_enabled: true
    monitoring_interval: 1.0
    max_memory_mb: 2048
    cpu_affinity: [0, 1]
```

### Plugin Configuration

#### Plasticity Plugin Config
```yaml
plugins:
  CustomPlasticityPlugin:
    learning_rate: 0.01
    decay_rate: 0.001
    enable_stdp: true
    enable_hebbian: true
    stdp_tau: 20.0
```

#### Neuromodulator Plugin Config
```yaml
plugins:
  CustomNeuromodulatorPlugin:
    baseline_dopamine: 1.0
    novelty_gain: 1.0
    curiosity_boost: 1.5
    prediction_error_sensitivity: 1.0
```

#### Sensory Plugin Config
```yaml
plugins:
  CustomSensoryPlugin:
    baseline_gain: 1.0
    attention_modulation: 2.0
    feature_extraction: true
    multimodal_fusion: true
    adaptive_thresholding: true
```

## Troubleshooting

### Common Issues and Solutions

#### Plugin Loading Errors

**Symptom**: "Failed to load plugin: CustomPlasticityPlugin"

**Possible Causes**:
1. Plugin library not found
2. Missing dependencies
3. Corrupted plugin file
4. Incompatible architecture

**Solutions**:
1. Check if plugin file exists: `./plugins/CustomPlasticityPlugin.so`
2. Verify dependencies: `ldd ./plugins/CustomPlasticityPlugin.so`
3. Recompile plugin if corrupted
4. Ensure plugin was built for the same architecture

#### Command Not Found

**Symptom**: "Unknown command: xyz"

**Solutions**:
1. Check spelling
2. Use `help` for available commands
3. Check for typos
4. Verify CLI version compatibility

#### Brain State Corruption

**Symptom**: Brain loading fails or produces unexpected behavior

**Solutions**:
1. Verify file integrity with `--verify` flag
2. Try loading from backup
3. Check file format compatibility
4. Validate plugin compatibility

### Error Messages

| Error Code | Description | Solution |
|------------|-------------|----------|
| PLUGIN_LOAD_FAILED | Plugin failed to load | Check file path and dependencies |
| BRAIN_SAVE_FAILED | Failed to save brain state | Check disk space and permissions |
| EXPERIMENT_NOT_FOUND | Experiment doesn't exist | Use `help experiments` to list available |
| PLUGIN_DEPENDENCY_MISSING | Required plugin not loaded | Load required plugins first |

## API Reference

### Core Classes

#### `nlm::cli::PluginManager`

**Methods**:
- `loadPlugin(const std::string& pluginPath)`
- `unloadPlugin(const std::string& pluginName)`
- `isPluginLoaded(const std::string& pluginName)`
- `executePluginFunction(...)`

#### `nlm::cli::NLMSession`

**Methods**:
- `runCommand(const std::string& commandLine)`
- `runBatchFile(const std::string& filename)`
- `saveBrainState(const std::string& filename)`
- `loadBrainState(const std::string& filename)`

#### `nlm::cli::CommandLineParser`

**Methods**:
- `parse(int argc, char** argv)`
- `registerCommand(const Command& cmd)`
- `executeCommand(const std::string& cmdName, ...)`

### Plugin Interface

#### `nlm::cli::Plugin`

**Virtual Methods**:
- `virtual const std::string& getName() const = 0`
- `virtual const std::string& getVersion() const = 0`
- `virtual bool initialize(...) = 0`
- `virtual void update(TimestepDuration dt) = 0`
- `virtual bool onPluginMessage(...) = 0`

## Contributing Plugins

### Plugin Development Guidelines

1. **Plugin Naming**: Use clear, descriptive names (e.g., `CustomPlasticityPlugin`)
2. **Versioning**: Follow semantic versioning (MAJOR.MINOR.PATCH)
3. **Dependencies**: Clearly specify required plugins
4. **Error Handling**: Implement robust error handling
5. **Documentation**: Include comprehensive documentation

### Creating a New Plugin

1. **Create Plugin Class**: Inherit from `nlm::cli::Plugin`
2. **Implement Required Methods**: `initialize`, `shutdown`, `update`, `onPluginMessage`
3. **Register Factory Function**: Export `createPlugin()` function
4. **Build Plugin**: Add to CMakeLists.txt
5. **Test Plugin**: Verify integration with NLM
6. **Package Plugin**: Create plugin distribution package

### Example Plugin Template

```cpp
// Example: MyCustomPlugin.cpp
#include "nlm/cli/Plugin.hpp"
#include <string>

class MyCustomPlugin : public nlm::cli::Plugin {
public:
    const std::string& getName() const override {
        static const std::string name = "MyCustomPlugin";
        return name;
    }
    
    const std::string& getVersion() const override {
        static const std::string version = "1.0.0";
        return version;
    }
    
    // ... implement other required methods ...
};

extern "C" {
    std::shared_ptr<nlm::cli::Plugin> createPlugin() {
        return std::make_shared<MyCustomPlugin>();
    }
}
```

### Plugin Submission

1. **Test Thoroughly**: Ensure plugin works with all NLM configurations
2. **Performance Testing**: Verify no performance degradation
3. **Documentation**: Complete documentation and examples
4. **License**: Include appropriate license
5. **Submit Issue**: Create GitHub issue with plugin details

## Conclusion

The advanced NLM CLI and plugin system provides a powerful framework for:

- **Extending NLM functionality** through custom plugins
- **Automating complex experiments** with batch processing
- **Monitoring and optimizing performance** with advanced profiling
- **Managing brain states** for reproducibility and persistence
- **Customizing neural computation** with plugin-specific configurations

This system enables power users to build sophisticated NLM applications tailored to their specific research needs while maintaining backward compatibility with existing NLM installations.

For more information or to contribute plugins, visit the NLM GitHub repository.
