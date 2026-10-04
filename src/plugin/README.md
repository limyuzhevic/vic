# NLM Plugin Architecture

## Overview

This directory implements a comprehensive plugin architecture for the Neural Machine Learning (NLM) framework, enabling extensibility through dynamic plugin loading, registration, and management.

## Components

### 1. Core Plugin Infrastructure

#### PluginBase.hpp
Base abstract class for all plugins providing:
- Plugin metadata (name, version, description, author)
- Plugin lifecycle management (initialize, shutdown)
- Plugin configuration
- Plugin status (enabled/disabled)
- Validation support

#### PluginManager.hpp/cpp
Central plugin management system for:
- Plugin registration and loading
- Plugin lifecycle management
- Plugin discovery and access
- Configuration management
- Statistics and error handling

#### PluginRegistry.hpp/cpp
Plugin registration and discovery system for:
- Plugin type-based registration
- Plugin metadata storage
- Dependency management
- Plugin validation

#### PluginLoader.hpp/cpp
Dynamic plugin loading system for:
- Loading plugins from files
- Scanning plugin directories
- Plugin manifest parsing
- Plugin validation

#### PluginFactory.hpp/cpp
Factory pattern implementation for:
- Plugin type factory registration
- Plugin creation from types
- Factory management

### 2. Plugin Interfaces

Each interface extends `PluginBase` and provides specialized functionality:

#### NeuronPluginInterface.hpp
For custom neuron models:
- Neuron creation and configuration
- Neuron type management
- Population management
- Dynamic model support

#### SynapsePluginInterface.hpp
For custom synapse dynamics:
- Synapse creation and configuration
- Weight and delay management
- Plasticity integration
- Custom dynamics support

#### PlasticityRulePluginInterface.hpp
For custom plasticity rules:
- Plasticity rule creation
- STDP and Hebbian support
- Learning rate management
- Custom learning algorithms

#### NeuromodulatorPluginInterface.hpp
For custom neuromodulators:
- Neuromodulator creation
- Signal processing
- Reward and prediction error handling
- Custom dynamics support

#### DevelopmentPluginInterface.hpp
For custom development systems:
- Developmental stage management
- Critical period control
- Plasticity modification
- Custom maturation algorithms

#### MemoryPluginInterface.hpp
For custom memory systems:
- Memory system creation
- Encoding and retrieval management
- Capacity and decay control
- Custom memory algorithms

#### SensoryPluginInterface.hpp
For custom sensory processing:
- Sensory modality support
- Transduction and filtering
- Input normalization
- Custom sensory models

#### MotorPluginInterface.hpp
For custom action selection:
- Motor command creation
- Action selection algorithms
- Exploration-exploitation balance
- Custom motor control

#### EnvironmentPluginInterface.hpp
For custom environments:
- Environment creation
- Reward and state management
- Custom physics and dynamics
- Action validation

### 3. Example Plugins

#### CustomNeuronPlugin.hpp/cpp
Example neuron plugin demonstrating:
- Extended neuron model implementation
- Custom configuration
- Population management
- Custom dynamics

#### CustomSynapsePlugin.hpp/cpp
Example synapse plugin demonstrating:
- Extended synapse model implementation
- Custom weight and delay management
- Plasticity integration
- Custom dynamics

#### CustomPlasticityPlugin.hpp/cpp
Example plasticity plugin demonstrating:
- Custom plasticity rules
- STDP and Hebbian extension
- Custom learning algorithms
- Rule configuration

### 4. Integration Files

#### PluginArchitecture.hpp
Complete plugin architecture overview including all interfaces and examples.

## Usage Examples

### Basic Plugin Usage
```cpp
#include "PluginArchitecture.hpp"

void exampleUsage() {
    nlm::PluginRegistry registry;
    nlm::PluginManager manager;

    // Register a custom neuron plugin
    registry.registerPlugin<nlm::NeuronPluginInterface>(
        "MyNeuronPlugin",
        "1.0.0",
        "Custom neuron model",
        "Developer",
        []() { return std::make_unique<nlm::CustomNeuronPlugin>(); }
    );

    // Initialize and enable all plugins
    registry.initializeAllPlugins();
    manager.enableAll();
}
```

### Creating Custom Plugins
```cpp
// 1. Create a plugin class that implements the appropriate interface
class MyCustomPlugin : public nlm::NeuronPluginInterface {
public:
    std::string getName() const override { return "MyCustomPlugin"; }
    // ... implement all pure virtual methods
};

// 2. Register the plugin with the registry
registry.registerPlugin<nlm::NeuronPluginInterface>(
    "MyCustomPlugin",
    "1.0.0",
    "My custom neuron implementation",
    "Developer",
    []() { return std::make_unique<MyCustomPlugin>(); }
);
```

## Plugin Development Guidelines

### 1. Plugin Structure
- Each plugin should implement its respective interface
- Plugins should follow RAII principles
- All operations should be thread-safe
- Error handling should be robust

### 2. Configuration
- Plugins should accept configuration via `Config` objects
- Configuration should be validated before use
- Default configurations should be provided

### 3. Lifecycle
- Plugins should initialize and shutdown cleanly
- Resources should be properly managed
- Plugins should handle errors gracefully

### 4. Dependencies
- Plugins can declare dependencies on other plugins
- Dependency resolution should be supported
- Plugin loading order should be managed

## Testing

Run the plugin architecture tests:
```bash
./test_plugin_architecture
```

## Files

- `src/plugin/PluginBase.hpp` - Base plugin class
- `src/plugin/PluginManager.hpp/cpp` - Plugin manager implementation
- `src/plugin/PluginRegistry.hpp/cpp` - Plugin registry implementation
- `src/plugin/PluginLoader.hpp/cpp` - Plugin loader implementation
- `src/plugin/PluginFactory.hpp/cpp` - Plugin factory implementation
- `src/plugin/interfaces/*.hpp` - All plugin interfaces
- `src/plugin/examples/*.hpp/cpp` - Example plugins
- `src/plugin/PluginArchitecture.hpp` - Complete architecture overview
- `test_plugin_architecture.cpp` - Comprehensive test suite

## Benefits

1. **Extensibility**: Users can add custom components without modifying core code
2. **Flexibility**: Plugin architecture supports diverse research and specialized applications
3. **Maintainability**: Core system remains stable while enabling customization
4. **Testing**: Each plugin can be tested independently
5. **Compatibility**: Plugin system integrates seamlessly with existing NLM framework