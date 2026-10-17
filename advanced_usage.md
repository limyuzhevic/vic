# NLM - Advanced Usage Guide

## Overview

This guide provides comprehensive documentation for using NLM (Neural Learning Machine) advanced features including the improved command-line interface, enhanced configuration system, and plugin architecture.

## Part 1: Enhanced Command-Line Interface

### Quick Start

The NLM executable now includes comprehensive command-line options for configuring simulations:

```bash
# Show help
nlm --help

# Run with custom parameters
nlm --config my_config.json --neuron_count 2000 --timestep 0.001 --verbose

# Run specific tests
nlm --basic_test=true --plasticity_test=false --stdp_test=true
```

### Command-Line Options

| Option | Short Form | Description |
|--------|------------|-------------|
| `--help, -h` | `-h` | Show help message and exit |
| `--version` | -v | Show version information and exit |
| `--config <file>` | -c | Load configuration from file |
| `--seed <num>` | -s | Random seed for reproducible simulations |
| `--timestep <sec>` | -t | Simulation timestep in seconds |
| `--neuron_count <num>` | -n | Number of neurons in simulation |
| `--region_count <num>` | -r | Number of neural regions |
| `--connection_prob <f>` | -p | Connection probability (0.0-1.0) |
| `--basic_test <bool>` | -b | Enable/disable basic connectivity test |
| `--plasticity_test <bool>` | -l | Enable/disable plasticity experiment |
| `--stdp_test <bool>` | -p | Enable/disable STDP verification |
| `--output <file>` | -o | Save output to file |
| `--verbose` | -V | Enable verbose logging |

### Command-Line Examples

#### 1. Simple Configuration

```bash
nlm --seed 123 --neuron_count 1000 --timestep 0.001
```

#### 2. Using JSON Configuration File

```bash
nlm --config my_config.json --verbose --basic_test=true
```

#### 3. Advanced Configuration

```bash
nlm \
  --config production_config.json \
  --seed 42 \
  --neuron_count 5000 \
  --region_count 3 \
  --connection_prob 0.15 \
  --stdp_ltp 0.02 \
  --stdp_ltd 0.015 \
  --basic_test=true \
  --plasticity_test=true \
  --stdp_test=false \
  --output results.txt \
  --verbose
```

#### 4. Error Handling

```bash
nlm --config invalid_config.json
# Will show detailed error messages and exit with code 1
```

## Part 2: Enhanced Configuration System

### JSON Configuration Support

The configuration system now supports both JSON and simple key-value formats:

#### JSON Configuration Example

```json
{
  "nlm": {
    "version": "2.0",
    "brain": {
      "neuron_count": 2000,
      "region_count": 2,
      "synapse_density": 0.1,
      "v_thresh": -50.0,
      "v_rest": -70.0
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
        "learning_rate": 0.005
      }
    },
    "neuromodulation": {
      "dopamine": {
        "scale": 1.0,
        "enable": true
      },
      "curiosity": {
        "enable": true,
        "decay_rate": 0.95
      }
    },
    "environment": {
      "width": 20,
      "height": 20,
      "vision_width": 16,
      "vision_height": 16
    }
  }
}
```

#### Simple Configuration Example

```
# configs/simple.cfg
# Brain parameters
brain.neuron_count = 1000
brain.region_count = 1
brain.connection_probability = 0.1

# Plasticity parameters
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.hebbian.enable = true
plasticity.structural.enable = true

# Neuromodulation parameters
neuromod.dopamine.enable = true
neuromod.curiosity.enable = true

# Environment parameters
environment.width = 20
environment.height = 20
environment.vision_width = 16
environment.vision_height = 16

# Development parameters
development.enable = true
development.critical_period = 300.0
```

### Configuration Validation

The configuration system includes automatic validation:

```python
import pynlm

# Create configuration with validation
config = pynlm.createDefaultConfig()

# Add validation rules
config.addValidationRule("brain.neuron_count", pynlm.rangeValidator(1, 10000))
config.addValidationRule("brain.connection_probability", pynlm.rangeValidator(0.0, 1.0))
config.addValidationRule("simulation.timestep", pynlm.positiveValidator())

# Validate configuration
if config.validate():
    brain = pynlm.createBrain(config)
    brain.initialize()
else:
    print("Configuration validation failed!")
    for error in config.getValidationErrors():
        print(f"Error: {error}")
```

### Configuration File Loading

```python
import pynlm

# Load JSON configuration
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")

# Load environment variables
config.loadFromEnvironment("NLM_")

# Load from command line
config.loadFromArgs(sys.argv)

# Merge with defaults
config.merge(pynlm.getDefaultConfig())

# Save configuration
config.saveToFile("final_config.json")
```

## Part 3: Plugin Architecture

### Overview

The plugin system allows you to extend NLM functionality with custom modules:

- **Component Plugins**: Neural network components
- **Behavior Plugins**: Custom behaviors and decision rules
- **Memory Plugins**: Alternative memory systems
- **Utility Plugins**: Logging, randomness, visualization

### Built-in Plugins

```python
import pynlm

# Load all built-in plugins
plugin_manager = pynlm.getPluginManager()

# Core plugins
plugin_manager.loadPlugin("core_brain")
plugin_manager.loadPlugin("memory_system")
plugin_manager.loadPlugin("neural_dynamics")
plugin_manager.loadPlugin("neuromodulation")

# Utility plugins
plugin_manager.loadPlugin("logger")
plugin_manager.loadPlugin("random_generator")
plugin_manager.loadPlugin("visualization")

# Behavior plugins
plugin_manager.loadPlugin("exploration_behavior")
plugin_manager.loadPlugin("learning_behavior")

# Memory plugins
plugin_manager.loadPlugin("working_memory")
plugin_manager.loadPlugin("episodic_memory")
```

### Plugin Configuration

```python
import pynlm

# Configure plugins
plugin_manager.configurePlugin("core_brain", {
    "neuron_count": 2000,
    "region_count": 2
})

plugin_manager.configurePlugin("memory_system", {
    "working_memory.capacity": 5000,
    "episodic_memory.persistence": true
})

plugin_manager.configurePlugin("exploration_behavior", {
    "exploration_rate": 0.3,
    "curiosity_threshold": 0.5
})

# Initialize all plugins
plugin_manager.initializeAllPlugins({
    "core_brain": {"neuron_count": 2000},
    "memory_system": {"capacity": 10000},
    "exploration_behavior": {"exploration_rate": 0.3}
})
```

### Custom Plugin Example

```cpp
// my_custom_plugin.hpp
#pragma once

#include "PluginSystem.hpp"

class CustomBehaviorPlugin : public nlm::BehaviorPlugin {
public:
    std::string getName() const override { return "custom_behavior"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Custom behavior plugin example"; }
    nlm::PluginType getType() const override { return nlm::PluginType::BEHAVIOR; }
    
    std::vector<std::string> getDependencies() const override {
        return {"core_brain", "neuromodulation"};
    }
    
    std::vector<std::string> getCapabilities() const override {
        return {"custom_decision", "adaptive_learning", "contextual_awareness"};
    }
    
    void executeBehavior(const std::map<std::string, std::string>& context) override {
        // Custom behavior implementation
        // Access brain via dependency injection
        // Make decisions based on context
    }
};

// Plugin factory function
extern "C" {
    std::unique_ptr<nlm::Plugin> createPlugin() {
        return std::make_unique<CustomBehaviorPlugin>();
    }
}
```

### Plugin Management

```python
import pynlm

plugin_manager = pynlm.getPluginManager()

# Check if plugin is loaded
if plugin_manager.isPluginLoaded("my_custom_plugin"):
    print("Plugin is loaded")

# Get plugin information
info = plugin_manager.getPluginInfo("my_custom_plugin")
print(info)

# Get plugin status
status = plugin_manager.getAllPluginStatus()
print(status)

# Get system information
system_info = plugin_manager.getSystemInfo()
print(system_info)

# Save/Load plugin state
plugin_manager.savePluginState("plugin_state.json")
plugin_manager.loadPluginState("plugin_state.json")
```

## Part 4: Error Handling and Robustness

### Enhanced Error Handling

```python
import pynlm

# Create brain with error handling
try:
    config = pynlm.createDefaultConfig()
    
    # Validate configuration
    if not config.validate():
        raise pynlm.ConfigurationError("Invalid configuration")
    
    # Create brain
    brain = pynlm.createBrain(config)
    
    # Initialize with error handling
    if not brain.initialize():
        raise pynlm.BrainInitializationError("Failed to initialize brain")
    
    # Run simulation with error handling
    for step in range(1000):
        try:
            brain.step(step)
        except pynlm.SimulationError as e:
            print(f"Simulation error at step {step}: {e}")
            break
        except Exception as e:
            print(f"Unexpected error: {e}")
            continue
    
    print("Simulation completed successfully")
    
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.BrainInitializationError as e:
    print(f"Brain initialization error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Error Types

The NLM system provides specific error types for different failure scenarios:

| Error Type | Description |
|------------|-------------|
| `ConfigurationError` | Invalid configuration parameters |
| `BrainInitializationError` | Failed to initialize brain |
| `SimulationError` | Error during simulation |
| `PluginError` | Plugin loading or execution error |
| `FileError` | File I/O error |

### Error Recovery

```python
import pynlm

def robustSimulation(config, steps):
    """Run simulation with automatic error recovery."""
    brain = pynlm.createBrain(config)
    
    if not brain.initialize():
        # Attempt to recover
        brain = pynlm.createBrain(config)
        brain.initialize()
        if not brain.initialize():
            raise pynlm.BrainInitializationError("Recovery failed")
    
    successful_steps = 0
    
    for step in range(steps):
        try:
            brain.step(step)
            successful_steps += 1
        except pynlm.SimulationError:
            # Attempt recovery
            brain.reset()
            brain.initialize()
            # Log error
        except Exception as e:
            # Log unexpected error
            print(f"Unexpected error at step {step}: {e}")
    
    return successful_steps
```

## Part 5: Advanced Configuration Examples

### Production Configuration

```json
{
  "nlm": {
    "version": "2.0",
    "brain": {
      "neuron_count": 5000,
      "region_count": 3,
      "synapse_density": 0.05,
      "connection_probability": 0.1,
      "initial_weight_mean": 0.5,
      "initial_weight_std": 0.1
    },
    "performance": {
      "enable_simd": true,
      "enable_parallel": true,
      "max_memory_mb": 2048
    },
    "logging": {
      "level": "info",
      "file": "nlm.log",
      "rotate": true,
      "max_size_mb": 100
    }
  }
}
```

### Development Configuration

```json
{
  "nlm": {
    "version": "2.0",
    "brain": {
      "neuron_count": 100,
      "region_count": 1,
      "synapse_density": 0.2
    },
    "development": {
      "enable_plasticity": true,
      "enable_curiosity": true,
      "enable_structural_plasticity": true
    },
    "testing": {
      "enable_debug_mode": true,
      "enable_profiling": true,
      "log_step_details": true
    }
  }
}
```

## Part 6: Performance Optimization

### Command-Line Optimization

```bash
# Enable optimizations for production
nlm \
  --config prod_config.json \
  --neuron_count 10000 \
  --region_count 5 \
  --timestep 0.0001 \
  --stdp_ltp 0.005 \
  --stdp_ltd 0.008 \
  --output optimized_results.json
```

### Python Optimization

```python
import pynlm

# Create optimized configuration
config = pynlm.createDefaultConfig()

# Set optimization parameters
config.set("brain.neuron_count", 10000)
config.set("brain.region_count", 5)
config.set("simulation.timestep", 0.0001)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.stdp.tau", 10.0)
config.set("performance.enable_simd", True)
config.set("performance.enable_parallel", True)

# Create brain with optimized settings
brain = pynlm.createBrain(config)
brain.initialize()

# Run performance-critical simulation
for step in range(5000):
    brain.step(step)
```

## Part 7: Troubleshooting

### Common Issues and Solutions

| Symptom | Cause | Solution |
|---------|-------|----------|
| Simulation too slow | Too many neurons | Reduce neuron_count, increase timestep |
| Brain not learning | Plasticity disabled | Enable plasticity, check configuration |
| Memory errors | Out of memory | Reduce neuron_count, increase memory limits |
| Plugin loading failed | Missing dependencies | Install required plugins, check version compatibility |
| Configuration validation failed | Invalid parameters | Check configuration file format, use validation tools |

### Debug Information

```python
import pynlm

# Enable debug mode
config = pynlm.createDefaultConfig()
config.set("logging.level", "debug")
config.set("logging.enable_profiling", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Run with debug output
for step in range(100):
    brain.step(step)
    
    # Print debug information every 10 steps
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        print(f"Memory usage: {brain.getMemoryUsage():.2f} MB")
        print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
```

## Part 8: Migration Guide

### From Previous Versions

#### Configuration Migration

**Old format:**
```ini
brain.neuron_count = 1000
brain.region_count = 1
brain.connection_probability = 0.1
```

**New format (JSON):**
```json
{
  "nlm": {
    "brain": {
      "neuron_count": 1000,
      "region_count": 1,
      "connection_probability": 0.1
    }
  }
}
```

**New format (Simple):**
```
brain.neuron_count = 1000
brain.region_count = 1
brain.connection_probability = 0.1
```

#### Command-Line Migration

**Old:**
```bash
nlm --neurons 1000 --regions 1
```

**New:**
```bash
nlm --neuron_count 1000 --region_count 1
```

#### Plugin Migration

**Old:**
```cpp
// Load plugins manually
loadPlugin("memory")
loadPlugin("behavior")
```

**New:**
```cpp
// Use plugin manager
plugin_manager = getPluginManager()
plugin_manager.loadPlugin("memory")
plugin_manager.loadPlugin("behavior")
```

## Conclusion

NLM now provides a comprehensive set of advanced features:

1. **Enhanced Command-Line Interface** with comprehensive options and help
2. **Robust Configuration System** with JSON support and validation
3. **Plugin Architecture** for extensible functionality
4. **Advanced Error Handling** with specific error types and recovery
5. **Performance Optimization** options for production use
6. **Comprehensive Documentation** with examples and troubleshooting

These improvements make NLM more powerful, easier to use, and suitable for both beginners and advanced users. The system is now more robust, maintainable, and extensible than ever before.