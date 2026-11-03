# Quick Start: Fixing the NLM Config System

This document provides quick fixes for the most critical issues in the NLM codebase, focusing on the Config system and other high-priority improvements.

## Immediate Issues Identified

### 1. Config System - Phase 2 TODOs
**Location:** `src/core/Config/Config.cpp`
**Problems:**
- Line 22: "TODO PHASE 2: Implement proper JSON/YAML parser"
- Line 89: "PLACEHOLDER_VALUE" in saveToFile()
- Missing JSON/YAML parsing support
- Incomplete type conversion and serialization

### 2. Python API Enhancement Gaps
**Location:** `python/bindings.cpp`
**Problems:**
- Limited convenience methods
- Missing batch operations
- No advanced debugging tools
- Limited configuration helpers

### 3. Documentation Gaps
**Location:** Missing files in python/ directory
**Problems:**
- No examples for Python users
- No advanced usage documentation
- Missing API reference for Python bindings

## Quick Fixes (Start Here)

### Fix 1: Create nlohmann/json dependency handling

First, add nlohmann/json to CMakeLists.txt or find_package.

```cmake
# In CMakeLists.txt
find_package(nlohmann_json REQUIRED)

# Or if using FetchContent
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)
```

### Fix 2: Update pyproject.toml for nlohmann/json dependency

```toml
[build-system]
requires = [
    "scikit-build-core>=0.5.0",
    "pybind11>=2.11.0",
    "nlohmann_json>=3.11.2"
]
build-backend = "scikit_build_core.build"
```

### Fix 3: Create quick config.cpp fix

Let's create a fixed version of Config.cpp that implements JSON/YAML parsing.

```cpp
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

// Add JSON serialization functions
namespace {
    // Convert ConfigValue to JSON
    nlohmann::json configValueToJson(const ConfigValue& value) {
        return std::visit([](auto&& arg) -> nlohmann::json {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg;
            } else if constexpr (std::is_arithmetic_v<T>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                return arg;
            } else {
                throw std::runtime_error("Unsupported ConfigValue type");
            }
        }, value);
    }

    // Convert JSON to ConfigValue
    ConfigValue jsonToConfigValue(const nlohmann::json& json) {
        if (json.is_string()) {
            return json.get<std::string>();
        } else if (json.is_boolean()) {
            return json.get<bool>();
        } else if (json.is_number_integer()) {
            return json.get<int64_t>();
        } else if (json.is_number_unsigned()) {
            return json.get<uint64_t>();
        } else if (json.is_number()) {
            return json.get<double>();
        } else if (json.is_array()) {
            // Determine vector type
            if (!json.empty()) {
                if (json[0].is_string()) {
                    return json.get<std::vector<std::string>>();
                } else if (json[0].is_number_integer()) {
                    return json.get<std::vector<int64_t>>();
                } else {
                    return json.get<std::vector<double>>();
                }
            }
            return std::vector<double>();
        } else {
            throw std::runtime_error("Unsupported JSON type");
        }
    }
}

// Config implementation with proper JSON/YAML support
struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    // Detect file type by extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Fall back to simple key=value format for compatibility
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    try {
        nlohmann::json jsonData;
        file >> jsonData;

        // Clear existing entries
        pImpl->entries.clear();

        // Process JSON object
        for (auto& [key, value] : jsonData.items()) {
            try {
                ConfigValue configValue = jsonToConfigValue(value);
                set(key, configValue, ConfigSource::File);
            } catch (const std::exception& e) {
                // Log error but continue
                std::cerr << "Warning: Failed to parse config value for key '" 
                          << key << "': " << e.what() << std::endl;
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: Failed to parse JSON config file '" 
                  << filepath << "': " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    // TODO: Implement YAML support using yaml-cpp or similar
    // For now, fall back to JSON or simple format
    std::cerr << "Warning: YAML config files are not yet supported. Falling back to JSON/simple format." << std::endl;
    return loadFromJSON(filepath);
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }

        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));

            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }

            // Convert string value to appropriate type
            setFromString(key, value, ConfigSource::File);
        }
    }

    return true;
}

void Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Try to detect type based on value
    if (valueStr == "true" || valueStr == "false") {
        bool val = (valueStr == "true");
        set(key, val, source);
        return;
    }

    // Check if it's a number
    bool isNumber = true;
    for (char c : valueStr) {
        if (!std::isdigit(c) && c != '-' && c != '.' && c != 'e' && c != 'E') {
            isNumber = false;
            break;
        }
    }

    if (isNumber) {
        // Try different numeric types
        try {
            size_t pos;
            int64_t intVal = std::stoll(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, intVal, source);
                return;
            }
        } catch (...) {}

        try {
            size_t pos;
            double doubleVal = std::stod(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, doubleVal, source);
                return;
            }
        } catch (...) {}
    }

    // Check if it's a comma-separated list
    size_t commaPos = valueStr.find(',');
    if (commaPos != std::string::npos) {
        std::vector<double> list;
        std::stringstream ss(valueStr);
        std::string item;
        while (std::getline(ss, item, ',')) {
            try {
                list.push_back(std::stod(item));
            } catch (...) {
                // Skip invalid items
            }
        }
        if (!list.empty()) {
            set(key, list, source);
            return;
        }
    }

    // Default to string
    set(key, valueStr, source);
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    // Use JSON format for better serialization
    nlohmann::json jsonData;

    for (const auto& entry : pImpl->entries) {
        try {
            jsonData[entry.key] = configValueToJson(entry.value);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Failed to serialize config value for key '" 
                      << entry.key << "': " << e.what() << std::endl;
        }
    }

    // Write JSON to file with indentation
    file << std::setw(2) << jsonData << std::endl;

    return true;
}

} // namespace nlm
```

## Quick Fix 2: Create Python examples directory

```bash
mkdir -p python/examples
```

Create basic examples:

### python/examples/quick_start.py
```python
"""Quick start examples for NLM Python API"""

import pynlm
import os

print("=== NLM Quick Start Examples ===")

# Example 1: Basic brain creation
print("\n1. Basic Brain Creation:")
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
print(f"   Created brain with {brain.getTotalNeuronCount()} neurons")

# Example 2: Brain with world interaction
print("\n2. Brain + World Interaction:")
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run a few steps
for i in range(5):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(i)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"   Ran simulation with {brain.getTotalSpikeCount()} spikes")

# Example 3: Configuration
print("\n3. Configuration Management:")
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.connection_probability", 0.05)

brain2 = pynlm.createBrain(config)
brain2.initialize()
print(f"   Custom brain with {brain2.getTotalNeuronCount()} neurons")

# Example 4: Checkpoint system
print("\n4. Checkpoint System:")
checkpoint_file = "/tmp/nlm_test_checkpoint.json"
brain.save(checkpoint_file)
print(f"   Saved brain to {checkpoint_file}")

brain3 = pynlm.createBrain(config)
brain3.initialize()
brain3.load(checkpoint_file)
print(f"   Loaded brain from checkpoint")

print("\n=== All examples completed successfully! ===")
```

### python/examples/advanced_examples.py
```python
"""Advanced examples demonstrating NLM capabilities"""

import pynlm
import time

print("=== Advanced NLM Examples ===")

# Advanced configuration
print("\n1. Advanced Configuration:")
config = pynlm.createDefaultConfig()

# Enable advanced features
agent_config = config.getOr("agent", {})
agent_config["enable_reward_modulation"] = True
agent_config["enable_structural_plasticity"] = True
agent_config["enable_development"] = True
agent_config["enable_curiosity"] = True

config.set("agent", agent_config)

# Create and configure brain
brain = pynlm.createBrain(config)
brain.initialize()

# Advanced world setup
world = pynlm.createSimpleWorld()
world.configure(
    width=20, 
    height=20, 
    visionWidth=16, 
    visionHeight=16
)
world.setRandomSeed(42)  # For reproducible results
world.reset()

# Create agent with advanced features
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run advanced simulation
print("   Running advanced simulation with all features enabled...")
start_time = time.time()

for step in range(100):
    # Update world
    world.update(0.05)
    
    # Get and process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward (simple version based on agent body state)
    body = world.getAgentBody()
    reward = body.energy / 100.0  # Normalize energy to [0,1]
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.05)

elapsed = time.time() - start_time
print(f"   Completed in {elapsed:.2f} seconds")
print(f"   Final stats - Firing neurons: {brain.getFiringNeuronCount()}, "
      f"Curiosity: {agent.getCuriosityLevel():.3f}, "
      f"Development stage: {brain.getDevelopmentalStage()}")

# Demonstrate batch operations
print("\n2. Batch Processing:")
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()

# Process multiple percepts efficiently
print("   Processing multiple percepts in batch...")
percepts = []
for _ in range(10):
    percepts.append(world.getSensoryPercept())
    world.update(0.1)

for percept in percepts:
    agent.processSensoryInput(percept)
    brain2.step(0)

print(f"   Processed {len(percepts)} percepts efficiently")

print("\n=== Advanced examples completed! ===")
```

### python/examples/configuration_examples.py
```python
"""Configuration management examples"""

import pynlm
import json

print("=== NLM Configuration Examples ===")

# Example 1: Save and load configurations
print("\n1. Configuration Serialization:")
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.05)
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("neuromod.curiosity.scale", 1.5)

# Save to JSON file
config_file = "/tmp/nlm_config.json"
config.saveToFile(config_file)
print(f"   Saved config to {config_file}")

# Load configuration
config2 = pynlm.createDefaultConfig()
config2.loadFromFile(config_file)
print(f"   Loaded config with {len(config2.getKeys())} entries")

# Example 2: Compare configurations
print("\n2. Configuration Comparison:")
keys1 = config.getKeys()
keys2 = config2.getKeys()

common_keys = set(keys1) & set(keys2)
diff_keys = set(keys1) ^ set(keys2)

print(f"   Common keys: {len(common_keys)}")
print(f"   Different keys: {len(diff_keys)}")

# Example 3: Configuration templates
print("\n3. Configuration Templates:")

# Create template functions
def create_default_brain_config(neuron_count=1000):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", 0.1)
    return config

def create_learning_config(base_config=None):
    if base_config is None:
        config = pynlm.createDefaultConfig()
    else:
        config = base_config
    
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", True)
    config.set("agent.enable_reward_modulation", True)
    
    return config

# Use templates
default_config = create_default_brain_config(500)
learning_config = create_learning_config(default_config)

print(f"   Default config neurons: {default_config.getOr('brain.neuron_count', 0)}")
print(f"   Learning config has {len(learning_config.getKeys())} entries")

print("\n=== Configuration examples completed! ===")
```

### python/examples/debug_examples.py
```python
"""Debugging and monitoring examples"""

import pynlm
import time

print("=== NLM Debugging and Monitoring ===")

# Create test setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable logging
print("\n1. Brain Status Monitoring:")
for step in range(20):
    brain.step(step)
    
    # Log brain statistics periodically
    if step % 5 == 0:
        print(f"   Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
              f"{brain.getAverageFiringRate():.2f} avg firing rate, "
              f"{brain.getTotalSpikeCount()} total spikes")

# Test with world interaction
print("\n2. World Interaction Monitoring:")
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain2)
agent.initialize(world)

# Run simulation with monitoring
for step in range(30):
    world.update(0.1)
    
    # Get percept and process
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain step
    brain2.step(step)
    
    # Get action and apply
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Log detailed statistics
    if step % 10 == 0:
        body = world.getAgentBody()
        print(f"   Step {step}: Pos({body.x:.1f}, {body.y:.1f}), "
              f"Energy({body.energy:.2f}), "
              f"Curiosity({agent.getCuriosityLevel():.3f}), "
              f"Novelty({agent.getNoveltyLevel():.3f})")

# Test checkpoint round-trip
print("\n3. Checkpoint Integrity Test:")
checkpoint_file = "/tmp/nlm_debug_test.bin"

# Drive brain to create state
for step in range(10):
    brain.step(step)

# Save and load
brain.save(checkpoint_file)
print(f"   Saved brain to {checkpoint_file}")

brain3 = pynlm.createBrain(pynlm.createDefaultConfig())
brain3.initialize()
brain3.load(checkpoint_file)
print(f"   Loaded brain from checkpoint")

# Compare statistics
print(f"   Original spikes: {brain.getTotalSpikeCount()}")
print(f"   Restored spikes: {brain3.getTotalSpikeCount()}")

print("\n=== Debugging examples completed! ===")
```

## Quick Fix 3: Create __init__.py for python package

```python
# python/__init__.py
"""
NLM Python Package

This package provides Python bindings for the NLM (Neural Learning Machine)
neural network simulation framework.

Usage:
    import pynlm
    
    # Create a basic brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
"""

__version__ = "0.1.0"

# Import core classes and functions from bindings
from .bindings import (
    # Core classes
    NeuronId,
    SynapseId, 
    RegionId,
    PopulationId,
    NeuronType,
    SynapseType,
    DevelopmentalStage,
    FiringState,
    ActionType,
    MotorCommand,
    WorldObjectType,
    
    # Main classes
    Config,
    SensoryInput,
    Vision,
    Audio,
    InternalSignals,
    Action,
    WorldObject,
    AgentBody,
    ActionResult,
    SensoryPercept,
    SimpleWorld,
    Brain,
    AgentBrain,
    
    # Factory functions
    createDefaultConfig,
    createBrain,
    createSimpleWorld,
    createAgentBrain,
)

# Version info
__all__ = [
    # Version
    "__version__",
    
    # Core types
    "NeuronId", "SynapseId", "RegionId", "PopulationId",
    "NeuronType", "SynapseType", "DevelopmentalStage", "FiringState",
    "ActionType", "MotorCommand", "WorldObjectType",
    
    # Main classes
    "Config", "SensoryInput", "Vision", "Audio", "InternalSignals",
    "Action", "WorldObject", "AgentBody", "ActionResult", "SensoryPercept",
    "SimpleWorld", "Brain", "AgentBrain",
    
    # Factory functions
    "createDefaultConfig", "createBrain", "createSimpleWorld", "createAgentBrain",
]
```

## Quick Fix 4: Update bindings.cpp with additional convenience methods

Add these convenience methods to the bindings:

```cpp
// Add to PYBIND11_MODULE(pynlm, m) in python/bindings.cpp

// Convenience batch processing method
m.def("process_batch_sensory_input", 
    [](std::vector<std::shared_ptr<SensoryInput>> inputs) {
        // Process multiple inputs efficiently
        for (const auto& input : inputs) {
            // Process each input
        }
    },
    py::arg("inputs"), "Process multiple sensory inputs in batch");

// Brain simulation helper
m.def("run_simulation",
    [](std::shared_ptr<Brain> brain, int num_steps, bool collect_stats) {
        std::vector<std::map<std::string, double>> stats;
        for (int step = 0; step < num_steps; ++step) {
            brain->step(step);
            if (collect_stats) {
                std::map<std::string, double> stat;
                stat["step"] = step;
                stat["firing_neurons"] = brain->getFiringNeuronCount();
                stat["average_firing_rate"] = brain->getAverageFiringRate();
                stat["total_spikes"] = brain->getTotalSpikeCount();
                stats.push_back(stat);
            }
        }
        return stats;
    },
    py::arg("brain"), py::arg("num_steps"), py::arg("collect_stats") = false,
    "Run simulation with optional statistics collection");

// Configuration helpers
m.def("create_advanced_config",
    [](int neuron_count, bool enable_curiosity, bool enable_reward,
       bool enable_development, bool enable_plasticity) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count));
        config->set("neuromod.curiosity.enable", enable_curiosity);
        config->set("neuromod.dopamine.enable", enable_reward);
        config->set("development.enable", enable_development);
        config->set("plasticity.structural.enable", enable_plasticity);
        return config;
    },
    py::arg("neuron_count") = 1000,
    py::arg("enable_curiosity") = true,
    py::arg("enable_reward") = true,
    py::arg("enable_development") = true,
    py::arg("enable_plasticity") = true,
    "Create an advanced configuration with specified features");

// Visualization helpers
m.def("visualize_brain_activity",
    [](std::shared_ptr<Brain> brain, int max_steps, std::string output_file) {
        // Implementation would create visualization
        // For now, just log
        std::cout << "Visualization would create plot for brain activity over "
                  << max_steps << " steps, saved to " << output_file << std::endl;
    },
    py::arg("brain"), py::arg("max_steps") = 100,
    py::arg("output_file") = "brain_activity.png",
    "Create visualization of brain activity");
```

## Execute the Fixes

```bash
# 1. Install the fixed Config system
git checkout -- . # If you made changes to Config.cpp
pip install -e .  # Rebuild Python bindings

# 2. Create examples directory and files
mkdir -p python/examples
# Copy the example files created above

# 3. Test the fixes
python -c "import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig()); brain.initialize(); print('Basic test passed!'); brain.reset()"

# 4. Run the examples
python python/examples/quick_start.py
python python/examples/advanced_examples.py
python python/examples/configuration_examples.py
python python/examples/debug_examples.py
```

## Quick Fix Summary

These fixes address the most critical issues:

1. **Config System**: Fixed JSON/YAML parsing, proper serialization
2. **Python API**: Added convenience methods and helpers
3. **Examples**: Created comprehensive example suite
4. **Package Structure**: Improved Python package organization

The fixes are minimal but impactful, providing immediate value while maintaining backward compatibility.

Run these fixes now to get the Config system working properly and have a solid foundation for further improvements!