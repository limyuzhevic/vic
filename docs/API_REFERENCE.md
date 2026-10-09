# NLM API Documentation

This document provides comprehensive API documentation for the NLM (熙然) Neural Learning Machine.

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

## Core Architecture

### Main Components

#### 1. Brain Class (`Brain.hpp`)
The main simulation class that coordinates all neural components.

**Key Methods:**
- `Brain(std::shared_ptr<Config> config)` - Constructor with configuration
- `initialize()` - Initialize brain with given neuron count and regions
- `step(SimulationStep step)` - Run one simulation step
- `save(const std::string& filename)` - Save brain state to checkpoint
- `load(const std::string& filename)` - Load brain state from checkpoint

**Key Properties:**
- `getTotalNeuronCount()` - Total number of neurons
- `getTotalSynapseCount()` - Total number of synapses
- `getFiringNeuronCount()` - Currently firing neurons
- `getAverageFiringRate()` - Average firing rate across all neurons

#### 2. Neuron Class (`Neuron.hpp`)
Individual neuron implementing LIF dynamics.

**Key Methods:**
- `stepLIF(Timestamp currentTime, TimestepDuration dt)` - Update neuron membrane potential
- `recordSpike(Timestamp timestamp)` - Record spike event
- `injectCurrent(MembranePotential current)` - Inject external current
- `receiveExcitatoryInput(MembranePotential amplitude)` - Receive excitatory input
- `receiveInhibitoryInput(MembranePotential amplitude)` - Receive inhibitory input

**Key Properties:**
- `getState()` - Get current neuron state
- `getMembranePotential()` - Get membrane potential
- `getThreshold()` - Get firing threshold
- `isFiring()` - Check if neuron is currently firing
- `isRefractory()` - Check if neuron is in refractory period

#### 3. Config Class (`Config.hpp`)
Configuration management system with JSON support.

**Key Methods:**
- `loadFromFile(const std::string& filepath)` - Load configuration from JSON file
- `loadFromArgs(int argc, char** argv)` - Load configuration from command line arguments
- `get<T>(const std::string& key)` - Get configuration value
- `set(const std::string& key, const ConfigValue& value)` - Set configuration value
- `has(const std::string& key)` - Check if configuration key exists

**Common Configuration Keys:**
- `brain.neuron_count` - Number of neurons
- `brain.synapse_density` - Synapse density (0.0-1.0)
- `world.width` - World width
- `world.height` - World height
- `world.vision_width` - Vision sensor width
- `world.vision_height` - Vision sensor height

#### 4. SimpleWorld Class (`SimpleWorld.hpp`)
Environment simulation with agent interaction.

**Key Methods:**
- `configure(int width, int height, int visionWidth, int visionHeight)` - Configure world dimensions
- `update(double timestep)` - Update world simulation
- `setAgentStart(double x, double y)` - Set agent starting position
- `getSensoryPercept()` - Get sensory percept from world
- `applyMotorCommand(MotorCommand cmd, double timestamp)` - Apply motor command to world

### Advanced API Components

#### 5. AgentBrain Class (`AgentBrain.hpp`)
Interface between NLM brain and world.

**Key Methods:**
- `processSensoryInput(const SensoryPercept& percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command from neural activity
- `applyRewardModulation(float reward, float predictedReward)` - Apply reward-based learning
- `updateDevelopment(double timestep)` - Update developmental stage

#### 6. CheckpointManager Class (`CheckpointManager.hpp`)
Enhanced checkpoint management with compression support.

**Key Methods:**
- `save(SimulationStep step, Timestamp timestamp, ...)` - Save checkpoint
- `load(SimulationStep targetStep, ...)` - Load checkpoint
- `getAvailableCheckpoints()` - Get list of available checkpoints
- `configure(...)` - Configure checkpoint system

#### 7. PluginManager Class (`PluginManager.hpp`)
Plugin system for extending NLM functionality.

**Key Methods:**
- `registerPlugin(std::shared_ptr<PluginInterface> plugin)` - Register a plugin
- `initializePlugins(std::shared_ptr<Config> config)` - Initialize all plugins
- `getPlugin<T>()` - Get plugin of specific type
- `loadPluginsFromDirectory(const std::string& directory)` - Load plugins from directory

## Command Line Interface

### Usage

```bash
nlm [OPTIONS] [POSITIONAL_ARGS]...
```

### Options

#### Standard Options
- `--help, -h` - Show help message and exit
- `--version` - Show version information and exit
- `--verbose, -v` - Enable verbose output
- `--debug, -d` - Enable debug output
- `--profile, -p` - Enable performance profiling
- `--benchmark` - Run performance benchmark and exit

#### Configuration Options
- `--config=FILE` - Load configuration from FILE
- `--output=FILE` - Save configuration to FILE
- `--key=value` - Set configuration key to value
- `-key value` - Set configuration key to value (short form)

### Examples

#### Basic Usage
```bash
# Run with default configuration
nlm

# Load custom configuration
nlm --config=my_config.json

# Set specific configuration values
nlm brain.neuron_count=2000 world.width=50 world.height=50

# Enable debugging
nlm --debug --verbose

# Run benchmark
nlm --benchmark
```

#### Advanced Configuration
```bash
# Create custom configuration file
nlm --config=my_config.json brain.synapse_density=0.2 world.vision_width=16 world.vision_height=16 performance.target_fps=500

# Load configuration and set additional options
nlm --config=my_config.json --verbose --profile brain.neuron_count=5000

# Save current configuration
nlm --output=custom_config.json
```

## Configuration Guide

### JSON Configuration Format

```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "connection_probability": 0.05,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0
    },
    "world": {
        "width": 20,
        "height": 20,
        "vision_width": 8,
        "vision_height": 8,
        "simulation_timestep": 0.1
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
        }
    },
    "performance": {
        "target_fps": 1000.0,
        "neuron_pool_size": 10000,
        "synapse_pool_size": 50000
    }
}
```

### Command Line Configuration

Command line arguments override values in configuration files:

```bash
# Set brain neuron count to 2000 (overrides config file)
nlm brain.neuron_count=2000

# Set world dimensions
nlm world.width=50 world.height=30

# Enable specific plasticity mechanisms
nlm plasticity.stdp.enable=true plasticity.hebbian.enable=false
```

### Configuration Validation

The configuration system validates:

1. **Type Validation**: Ensures values match expected types (int, double, string, bool)
2. **Range Validation**: Checks values are within valid ranges
3. **Dependency Validation**: Ensures required configuration keys are present
4. **File Path Validation**: Validates file paths for loading/saving

## Examples and Tutorials

### Example 1: Basic Neural Network Simulation

```cpp
#include "Brain.hpp"
#include "SimpleWorld.hpp"
#include "AgentBrain.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", 1000);
    config->set("brain.synapse_density", 0.1);
    config->set("world.width", 20);
    config->set("world.height", 20);
    config->set("world.vision_width", 8);
    config->set("world.vision_height", 8);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    // Create agent brain interface
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(*world);
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Decode motor command
        auto motor_cmd = agent->decodeMotorCommand();
        
        // Apply motor command to world
        world->applyMotorCommand(motor_cmd, world->getSimulationTime());
        
        // Apply reward modulation
        double reward = 0.0; // Calculate from world state
        agent->applyRewardModulation(reward, 0.0);
        
        // Update development
        agent->updateDevelopment(0.1);
        
        // Print statistics periodically
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                     << "Firing neurons: " << brain->getFiringNeuronCount() << ", "
                     << "Curiosity: " << agent->getCuriosityLevel() << ", "
                     << "Novelty: " << agent->getNoveltyLevel() << ", "
                     << "Dev Stage: " << static_cast<int>(agent->getDevelopmentalStage())
                     << std::endl;
        }
    }
    
    return 0;
}
```

### Example 2: Using Checkpoints

```cpp
#include "Brain.hpp"
#include <iostream>

int main() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize brain
    brain->initialize();
    
    // Run simulation for a few steps
    for (int step = 0; step < 500; ++step) {
        brain->step(step);
        
        // Save checkpoint every 100 steps
        if (step % 100 == 0) {
            std::string filename = "checkpoint_step_" + std::to_string(step) + ".bin";
            if (brain->save(filename)) {
                std::cout << "Checkpoint saved: " << filename << std::endl;
            }
        }
    }
    
    // Load checkpoint from step 200
    std::string checkpointFile = "checkpoint_step_200.bin";
    if (brain->load(checkpointFile)) {
        std::cout << "Checkpoint loaded: " << checkpointFile << std::endl;
        
        // Continue simulation from loaded state
        for (int step = 500; step < 1000; ++step) {
            brain->step(step);
        }
    }
    
    return 0;
}
```

### Example 3: Plugin System Usage

```cpp
#include "PluginManager.hpp"
#include "Config.hpp"
#include <iostream>

// Example custom plugin
class ExampleNeuronModelPlugin : public nlm::PluginInterface {
public:
    std::string getName() const override { return "example_neuron_model"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Example neuron model plugin"; }
    bool initialize(std::shared_ptr<nlm::Config> config) override { return true; }
    void shutdown() override {}
    bool isEnabled() const override { return enabled_; }
    void setEnabled(bool enabled) override { enabled_ = enabled; }
    
    std::unique_ptr<nlm::Neuron> createNeuron(nlm::NeuronId id) override {
        // Create custom neuron implementation
        return std::make_unique<nlm::Neuron>(id);
    }
    
    nlm::NeuronType getSupportedNeuronType() const override {
        return nlm::NeuronType::Internal;
    }
    
    bool supportsDynamics(const std::string& dynamicsType) const override {
        return dynamicsType == "lif" || dynamicsType == "adaptive_lif";
    }
};

int main() {
    // Create plugin manager
    auto pluginManager = std::make_shared<nlm::PluginManager>();
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Create and register example plugin
    auto plugin = std::make_shared<ExampleNeuronModelPlugin>();
    if (pluginManager->registerPlugin(plugin)) {
        std::cout << "Plugin registered: " << plugin->getName() << std::endl;
    }
    
    // Enable the plugin
    pluginManager->enablePlugin("example_neuron_model");
    
    // Initialize all plugins
    if (pluginManager->initializePlugins(config)) {
        std::cout << "Plugins initialized successfully" << std::endl;
        
        // Get plugin instance
        auto neuronModelPlugin = pluginManager->getPlugin<nlm::NeuronModelPlugin>();
        if (neuronModelPlugin) {
            std::cout << "Retrieved neuron model plugin: " << neuronModelPlugin->getName() << std::endl;
        }
    }
    
    // Shutdown plugins
    pluginManager->shutdownPlugins();
    
    return 0;
}
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: "Configuration key not found"
**Problem**: Attempting to get a configuration value that doesn't exist.

**Solution**: Use `getOr<T>(key, defaultValue)` method or check if key exists with `has(key)`.

#### Issue 2: "Failed to open config file"
**Problem**: Cannot load configuration from file.

**Solution**: Ensure file exists and has proper permissions. Use absolute path if relative path doesn't work.

#### Issue 3: "Invalid configuration value"
**Problem**: Configuration value is of wrong type or out of range.

**Solution**: Check configuration validation rules. Use correct data types (int, double, string, bool).

#### Issue 4: "Plugin not found"
**Problem**: Attempting to use a plugin that hasn't been loaded.

**Solution**: Ensure plugin is registered and enabled. Use proper plugin discovery and loading.

#### Issue 5: "Memory allocation failed"
**Problem**: Running out of memory during simulation.

**Solution**: 
- Reduce simulation parameters (neuron count, world size)
- Use checkpoint/restart to save memory
- Enable memory pooling in configuration

### Performance Optimization Tips

1. **Use appropriate simulation timestep**: Smaller timesteps increase accuracy but reduce performance.
2. **Configure memory pools**: Set appropriate pool sizes in configuration.
3. **Enable compression**: Use checkpoint compression for large simulations.
4. **Parallel processing**: Use multiple threads for simulation steps.
5. **Limit checkpoint frequency**: Don't save checkpoints too frequently for large simulations.

### Debugging Tips

1. **Enable verbose logging**: Use `--verbose` or `--debug` flags.
2. **Use checkpoint debugging**: Enable checkpoint debugging in configuration.
3. **Monitor system resources**: Watch CPU and memory usage during simulation.
4. **Use performance profiling**: Enable profiling with `--profile` flag.
5. **Check error logs**: Review application logs for detailed error information.

## Best Practices

### Code Quality

1. **Always initialize before use**: Call `initialize()` on brain before simulation.
2. **Use RAII**: Leverage smart pointers and resource management.
3. **Validate inputs**: Always validate configuration and input parameters.
4. **Handle exceptions**: Use try-catch blocks for critical operations.
5. **Follow naming conventions**: Use consistent naming for variables and functions.

### Simulation Best Practices

1. **Start with small configurations**: Begin with minimal simulation to test functionality.
2. **Use checkpoints**: Save checkpoints periodically for recovery.
3. **Monitor system resources**: Watch memory and CPU usage.
4. **Log important events**: Log key simulation events for debugging.
5. **Use appropriate timestep**: Choose timestep based on required accuracy and performance.

### Plugin Development Best Practices

1. **Implement PluginInterface**: Always implement the base PluginInterface.
2. **Use proper error handling**: Handle errors gracefully and provide meaningful error messages.
3. **Validate configuration**: Validate plugin configuration in initialize() method.
4. **Thread safety**: Ensure plugins are thread-safe if used in multi-threaded environments.
5. **Resource management**: Properly manage resources in plugin initialization and shutdown.

## Migration Guide

### From Previous Versions

#### Configuration Changes
- Configuration system now supports JSON format
- Old key=value format still supported
- New configuration keys for advanced features

#### API Changes
- Some methods have been refactored for better encapsulation
- New plugin system added for extensibility
- Enhanced checkpoint manager with compression

#### Backward Compatibility
- Most existing code should work without changes
- Some deprecated methods still available with warnings
- New features are opt-in via configuration

## Reference

### Type Definitions

```cpp
typedef int SimulationStep;
typedef double Timestamp;
typedef double TimestepDuration;
typedef double MembranePotential;
typedef double SynapticWeight;
typedef float FiringRate;
```

### Constants

```cpp
namespace nlm::Constants {
    constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    constexpr float EXPLORATION_CURiosity_THRESHOLD = 0.5f;
    constexpr float CURIOSITY_MAX_EXPLORATION_CHANCE = 0.3f;
    // ... more constants
}
```

### Error Codes

```cpp
enum class ErrorCode {
    SUCCESS = 0,
    INVALID_ARGUMENTS = 1,
    CONFIG_FILE_NOT_FOUND = 2,
    CONFIG_PARSE_ERROR = 3,
    MEMORY_ALLOCATION_FAILED = 4,
    PLUGIN_NOT_FOUND = 5,
    PLUGIN_INITIALIZATION_FAILED = 6,
    CHECKPOINT_SAVE_FAILED = 7,
    CHECKPOINT_LOAD_FAILED = 8,
    SIMULATION_ERROR = 9
};
```

## Acknowledgments

This project builds on extensive research in computational neuroscience and artificial intelligence. Special thanks to the research team and contributors who have developed and tested this neural network simulation framework.

## License

MIT License

See LICENSE file for more information.