# NLM Extensible Architecture System

This document describes the extensible architecture system for the Neural Learning Machine (NLM) project.

## Overview

The NLM extensible architecture system transforms the project from a fixed research prototype into a fully extensible neural learning machine platform that supports:

- Plugin-based extensibility for neural models and plasticity rules
- Advanced API for custom implementations
- Neural Architecture Definition Language (NADL)
- Advanced configuration system
- Custom optimization hooks
- Extensible experiment framework
- Neural Architecture Builder

## Plugin System

The plugin system allows users to define custom neuron models, plasticity rules, and synaptic dynamics through a plugin interface.

### Core Plugin Components

1. **PluginManager.hpp/.cpp** - Manages plugin lifecycle, loading, and unloading
2. **PluginInterface.hpp** - Base interface for all plugins
3. **NeuronModelPlugin.hpp/.cpp** - Plugin interface for custom neuron models
4. **PlasticityRulePlugin.hpp/.cpp** - Plugin interface for custom plasticity rules
5. **SynapticDynamicsPlugin.hpp/.cpp** - Plugin interface for custom synaptic dynamics

### Plugin Interface

```cpp
class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize(const Config& config) = 0;
    virtual void update(double timestep) = 0;
    virtual void shutdown() = 0;
};
```

### Example Plugin Implementation

```cpp
class CustomNeuronModelPlugin : public INeuronModelPlugin {
public:
    std::string getName() const override { return "CustomNeuronModel"; }
    std::string getNeuronModelName() const override { return "MyCustomModel"; }
    
    bool initialize(const Config& config) override {
        // Initialize with configuration parameters
        return true;
    }
    
    float computeThreshold(const NeuronState& state) override {
        // Custom threshold computation
        return state.threshold * (1.0 + state.adaptationVariable);
    }
    
    MembranePotential updateDynamics(NeuronState& state, double dt) override {
        // Custom neuron dynamics
        // Implement custom update rules
        return state.membranePotential;
    }
    
    // Additional required methods...
};
```

## Advanced API

The advanced API provides extensible interfaces for custom implementations.

### Core Components

1. **ExtensibleBrain.hpp/.cpp** - Extensible brain interface
2. **CustomMemorySystem.hpp/.cpp** - Custom memory system implementation
3. **AdvancedNeuromodulator.hpp/.cpp** - Advanced neuromodulator implementation
4. **NeuralArchitecture.hpp/.cpp** - Neural architecture management

### Key Features

- Dynamic plugin integration
- Custom memory systems with eviction policies
- Advanced neuromodulation with custom signal generators
- Neural architecture compilation and execution

## Configuration System

The configuration system provides environment-based configuration, runtime modifications, and inheritance.

### Core Components

1. **AdvancedConfig.hpp/.cpp** - Advanced configuration management
2. **ConfigValidator.hpp/.cpp** - Configuration validation
3. **ConfigInheritance.hpp/.cpp** - Configuration inheritance and merging

### Key Features

- Environment variable support
- Runtime configuration updates
- Schema validation
- Configuration inheritance
- Hot reloading

## Architecture Definition Language (NADL)

NADL provides JSON/YAML-based network architecture specification with protocol buffers support.

### Core Components

1. **ArchitectureCompiler.hpp/.cpp** - Architecture compilation
2. **ArchitectureParser.hpp/.cpp** - Architecture parsing
3. **Architecture.hpp/.cpp** - Architecture management

### Key Features

- Multiple format support (YAML, JSON, Text)
- Architecture validation and optimization
- Template architecture support
- Custom parser extensions

## Optimization Hooks

Optimization hooks provide SIMD optimization, parallel processing, and custom memory management.

### Core Components

1. **OptimizationHooks.hpp/.cpp** - Main optimization hooks
2. **CustomParallelism.hpp/.cpp** - Custom parallelism implementations
3. **CustomMemoryManager.hpp/.cpp** - Custom memory management

### Built-in Implementations

- **OpenMPParallelism** - OpenMP-based parallelism
- **ThreadPoolParallelism** - Thread pool parallelism
- **TBBParallelism** - Intel TBB parallelism
- **PoolMemoryManager** - Fixed-size memory pool
- **ArenaMemoryManager** - Arena-based memory management

## Experiment Framework

The experiment framework provides extensible experiment protocols and hyperparameter optimization.

### Core Components

1. **AdvancedExperiment.hpp/.cpp** - Advanced experiment management
2. **CustomProtocol.hpp/.cpp** - Custom protocol interface
3. **HyperparameterOptimizer.hpp/.cpp** - Hyperparameter optimization

### Built-in Protocols

- **ScalingBenchmarkProtocol** - Scaling benchmarks
- **PhaseTrainingProtocol** - Phase training protocols

### Built-in Optimizers

- **RandomSearchOptimizer** - Random search optimization
- **BayesianOptimizer** - Bayesian optimization
- **GeneticAlgorithmOptimizer** - Genetic algorithm optimization

## Neural Architecture Builder

The neural architecture builder provides scripted network construction and topological pattern generation.

### Core Components

- **DefaultNeuralArchitectureBuilder** - Default architecture builder implementation

### Key Features

- Random topology generation
- Feedforward and recurrent topology construction
- Modular topology support
- Connection pattern specifications
- Structural plasticity rules as code

## Building and Installation

### Prerequisites

- CMake 3.16 or higher
- C++20 compiler
- YAML-CPP library (for YAML support)
- JSON library (for JSON support)

### Building

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Installation

```bash
cmake --install . --prefix /usr/local
```

## Plugin Development

### Creating a Custom Neuron Model Plugin

1. Implement `INeuronModelPlugin` interface
2. Implement all required methods:
   - `getName()`, `getVersion()`
   - `initialize(const Config&)`
   - `update(double)`
   - `shutdown()`
   - `computeThreshold()`
   - `updateDynamics()`
   - `computeAdaptation()`
   - `initializeNeuron()`
3. Register plugin with `PluginManager`
4. Build as shared library (.so/.dylib/.dll)
5. Place in plugin directory and load

### Creating a Custom Architecture

1. Define regions, populations, and connections
2. Use `ArchitectureCompiler` to compile to code
3. Save as YAML/JSON template for reuse
4. Load in application code

## Usage Examples

### Loading Custom Plugins

```cpp
#include "plugins/PluginManager.hpp"

int main() {
    nlm::PluginManager pluginManager;
    
    // Load plugins from directory
    bool success = pluginManager.loadPluginFromDirectory("./plugins");
    
    // Get specific plugin
    auto neuronModelPlugin = pluginManager.getNeuronModelPlugin("CustomNeuronModel");
    
    // Enable plugin
    pluginManager.enablePlugin("CustomNeuronModel");
    
    return 0;
}
```

### Using Advanced Configuration

```cpp
#include "config/AdvancedConfig.hpp"

int main() {
    nlm::AdvancedConfig config;
    
    // Load from file
    config.loadFromFile("config.yaml");
    
    // Load from environment
    config.loadFromEnvironment();
    
    // Get configuration value
    auto value = config.getOr<int>("neural.network.layers", 3);
    
    // Set configuration at runtime
    config.set("neural.learning_rate", 0.01);
    
    return 0;
}
```

### Using Neural Architecture

```cpp
#include "arch/NeuralArchitecture.hpp"

int main() {
    nlm::NeuralArchitecture arch;
    
    // Add regions
    nlm::RegionSpec region1("InputLayer");
    region1.type = "input";
    arch.addRegion(region1);
    
    nlm::RegionSpec region2("HiddenLayer");
    region2.type = "hidden";
    arch.addRegion(region2);
    
    // Add populations
    nlm::PopulationSpec pop1("InputPopulation", "Excitatory", 100);
    region1.populations.push_back(pop1);
    
    // Add connections
    nlm::ConnectionSpec conn("InputLayer", "HiddenLayer");
    conn.connectionProbability = 0.8;
    arch.addConnection(conn);
    
    // Validate architecture
    if (arch.validate()) {
        // Compile architecture
        arch.compile("generated_brain.cpp");
    }
    
    return 0;
}
```

## Configuration Schema

The following configuration parameters are supported:

### Plugin Configuration

```yaml
plugins:
  neuron_models:
    - name: "CustomNeuronModel"
      path: "./plugins/libneuronmodel.so"
  plasticity_rules:
    - name: "CustomPlasticity"
      path: "./plugins/libplasticity.so"
  synaptic_dynamics:
    - name: "CustomDynamics"
      path: "./plugins/libdynamics.so"
```

### Advanced Configuration

```yaml
neural:
  network:
    layers: 3
    neurons_per_layer: 100
    connectivity: "fully_connected"
  learning:
    learning_rate: 0.01
    decay_rate: 0.9999
    optimizer: "adam"
  memory:
    type: "hierarchical"
    capacity: 1000000
    eviction_policy: "lru"
```

### Architecture Definition

```yaml
architecture:
  name: "MyNetwork"
  version: "1.0.0"
  description: "A custom neural network"
  regions:
    - name: "InputLayer"
      type: "input"
      neurons: 100
    - name: "HiddenLayer"
      type: "hidden"
      neurons: 50
      activation: "relu"
  connections:
    - source: "InputLayer"
      target: "HiddenLayer"
      type: "feedforward"
      weight: 1.0
```

## Testing

Run tests using CMake:

```bash
cd build
make test
```

Or run individual tests:

```bash
cd build
./test_runner
```

## Performance

The extensible architecture system is designed for high performance:

- Plugin loading overhead is minimal
- Runtime type information is cached
- Parallel processing is optimized
- Memory management is efficient
- Configuration validation is fast

## Limitations

- Some plugin operations require runtime type information
- YAML/JSON parsing has overhead
- Parallel processing requires careful implementation
- Memory management needs tuning

## Future Enhancements

- Native code generation for plugins
- Dynamic plugin unloading
- Advanced error recovery
- Distributed plugin loading
- Template-based architecture generation

## Conclusion

The NLM extensible architecture system provides a comprehensive framework for building custom neural network applications. It supports plugin-based extensibility, advanced configuration management, and flexible architecture definition, making it suitable for both research and production use.
