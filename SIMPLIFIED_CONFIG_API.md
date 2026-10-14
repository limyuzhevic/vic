# Simplified Configuration API for NLM

## Overview

This document describes the new simplified configuration API for the NLM (Neural Learning Machine) codebase. The new API replaces the complex `ConfigValue` variant approach with an easy-to-use interface while maintaining backward compatibility.

## What Was Changed

### 1. **Original Complex API** (`src/core/Config.hpp`)

The original `Config` class had:
- Complex `ConfigValue` variant type supporting many types
- Template-based `get<T>()` methods
- Hard to use and understand

### 2. **New Simplified API** (`src/core/ConfigSimplified.hpp`)

The new `ConfigSimplified` class provides:
- Simple `setBool()`, `setInt()`, `setDouble()`, `setString()` methods
- Direct `getBool()`, `getInt()`, `getDouble()`, `getString()` methods
- Easy-to-use convenience methods
- Configuration factory methods for common use cases

### 3. **Backward Compatibility**

The original `Config` class has been updated with convenience methods while keeping the original complex API for advanced users.

## New Simplified API Usage

### Basic Usage

```cpp
#include "ConfigSimplified.hpp"

// Create a configuration
ConfigSimplified config = ConfigSimplified::createBrainConfig();

// Set values using simple types
config.setBool("neuron_debug", false);
config.setInt("neuron_count", 1000);
config.setDouble("simulation_timestep", 0.001);
config.setString("brain_name", "MyBrain");

// Get values using simple types
bool debug = config.getBool("neuron_debug", false);
int neurons = config.getInt("neuron_count", 1000);
double timestep = config.getDouble("simulation_timestep", 0.001);
std::string name = config.getString("brain_name", "Default");
```

### Configuration Templates

```cpp
// Common brain configurations
Config config = Config::createBrainConfig();        // Default brain configuration
Config config = Config::createAdvancedConfig();   // Advanced configuration for experts
Config config = Config::createDevelopmentConfig(); // Development-focused configuration

// Easy configuration creation
Config config = Config::createDefaultConfig();     // Alias for createBrainConfig()
```

### Validation and Configuration

```cpp
// Validate configuration
if (config.validate()) {
    std::cout << "Configuration is valid!" << std::endl;
} else {
    std::cout << "Configuration validation failed!" << std::endl;
}

// Merge configurations
Config config1 = Config::createBrainConfig();
Config config2 = Config::createAdvancedConfig();
config1.merge(config2);  // Merge config2 into config1
```

## Configuration Templates

### Brain Configuration (`createBrainConfig()`)

Basic configuration suitable for most use cases:

```cpp
{
    "neuron_debug": false,
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "working_memory_capacity": 100,
    "episodic_memory_max_episodes": 1000,
    "enable_reward_modulation": true,
    "enable_curiosity": true,
    "enable_novelty_detection": true,
    "enable_prediction_error": true,
    "enable_development": true,
    "development_stage_duration": 1000
}
```

### Advanced Configuration (`createAdvancedConfig()`)

Advanced configuration for experienced users:

```cpp
{
    "neuron_debug": true,
    "neuron_count": 5000,
    "region_count": 4,
    "connection_probability": 0.15,
    "simulation_timestep": 0.0005,
    "random_seed": 12345,
    "stdp_ltp_weight": 0.05,
    "stdp_ltd_weight": 0.04,
    "stdp_tau": 25.0,
    "synaptogenesis_rate": 0.0005,
    "pruning_rate": 0.00005,
    "working_memory_capacity": 500,
    "episodic_memory_max_episodes": 5000,
    "enable_reward_modulation": true,
    "enable_curiosity": true,
    "enable_novelty_detection": true,
    "enable_prediction_error": true,
    "enable_development": true,
    "development_stage_duration": 5000,
    "use_synaptic_efficacy": true,
    "eligibility_trace_decay": 0.001,
    "max_synapses_per_neuron": 500,
    "max_total_synapses": 10000000
}
```

### Development Configuration (`createDevelopmentConfig()`)

Development-focused configuration for research:

```cpp
{
    "neuron_debug": false,
    "neuron_count": 500,
    "region_count": 2,
    "connection_probability": 0.2,
    "simulation_timestep": 0.002,
    "random_seed": 999,
    "stdp_ltp_weight": 0.03,
    "stdp_ltd_weight": 0.025,
    "stdp_tau": 30.0,
    "synaptogenesis_rate": 0.0002,
    "pruning_rate": 0.00002,
    "working_memory_capacity": 200,
    "episodic_memory_max_episodes": 2000,
    "enable_reward_modulation": true,
    "enable_curiosity": true,
    "enable_novelty_detection": true,
    "enable_prediction_error": true,
    "enable_development": true,
    "development_stage_duration": 2000,
    "enable_critical_period": true,
    "enable_maturation": true,
    "development_plasticity_mod": 1.5,
    "use_developmental_stages": true
}
```

## Migration Guide

### From Original API to New API

**Original API (hard to use):**
```cpp
Config config;
config.set("neuron_count", static_cast<ConfigValue>(1000), ConfigSource::Runtime);
config.set("simulation_timestep", static_cast<ConfigValue>(0.001), ConfigSource::Runtime);
bool debug = config.get<bool>("neuron_debug").value_or(false);
```

**New Simplified API (easy to use):**
```cpp
ConfigSimplified config = ConfigSimplified::createBrainConfig();
config.setInt("neuron_count", 1000);
config.setDouble("simulation_timestep", 0.001);
bool debug = config.getBool("neuron_debug", false);
```

### Backward Compatibility

The original `Config` class still supports all original methods:

```cpp
// Original complex API still works
Config config;
config.set("neuron_count", 1000, ConfigSource::Runtime);
config.set("simulation_timestep", 0.001, ConfigSource::Runtime);
bool debug = config.get<bool>("neuron_debug").value_or(false);
```

## Benefits of the New API

### 1. **Simplicity**
- No complex variant types
- No template magic
- Straightforward method signatures

### 2. **Type Safety**
- Each type has its own method
- No risk of type errors
- Clear and predictable API

### 3. **Convenience**
- Factory methods for common configurations
- Easy configuration validation
- Simple configuration merging

### 4. **Backward Compatibility**
- Original API still available
- Existing code continues to work
- No breaking changes

### 5. **Performance**
- Simple map-based implementation
- Reduced overhead from variant types
- Better cache locality

## Files Modified

### 1. **`src/core/ConfigSimplified.hpp`**
- New simplified configuration class
- Easy-to-use API methods
- Configuration factory methods
- Validation methods

### 2. **`src/core/ConfigSimplified.cpp`**
- Implementation of simplified configuration
- Configuration templates
- Validation logic
- Merge operations

### 3. **`src/core/Config.hpp`**
- Updated original Config class with convenience methods
- Maintained backward compatibility
- Added factory methods

## Integration with Existing Code

### Step 1: Replace Original Config with Simplified Config

```cpp
// Old code (original API)
#include "Config.hpp"
Config config;

// New code (simplified API)
#include "ConfigSimplified.hpp"
ConfigSimplified config = ConfigSimplified::createBrainConfig();
```

### Step 2: Update Configuration Usage

```cpp
// Old code (complex access)
bool debug = config.get<bool>("neuron_debug").value_or(false);
int neurons = config.get<int>("neuron_count").value_or(1000);

// New code (simple access)
bool debug = config.getBool("neuron_debug", false);
int neurons = config.getInt("neuron_count", 1000);
```

### Step 3: Use Configuration Templates

```cpp
// Create a configuration with recommended settings
Config config = Config::createBrainConfig();

// Customize as needed
config.setInt("neuron_count", 2000);
config.setDouble("connection_probability", 0.15);
```

## Testing and Validation

### Configuration Validation

The new API includes configuration validation:

```cpp
if (config.validate()) {
    std::cout << "Configuration is valid!" << std::endl;
} else {
    std::cout << "Configuration validation failed!" << std::endl;
}
```

### Required Keys

The validation checks for required keys:

```cpp
// Required keys for valid configuration
"neuron_count", "region_count", "connection_probability", 
"simulation_timestep", "random_seed"
```

## Conclusion

The new simplified configuration API provides:

1. **Easy-to-use interface** with simple method names
2. **Type safety** with dedicated methods for each type
3. **Convenience factory methods** for common use cases
4. **Configuration validation** and merging
5. **Backward compatibility** with the original API
6. **Better performance** with simpler data structures

This makes it much easier to configure NLM brains while maintaining flexibility for advanced users who need the original complex API.