# Advanced User Guide for NLM
# Comprehensive Documentation for Power Users

This document provides comprehensive documentation for advanced users of the NLM (Neural Learning Machine) framework. It covers the advanced features, plugin system, configuration options, and provides examples for power users who want to customize and extend the neural simulation framework.

## Table of Contents

1. [Advanced Configuration System](#advanced-configuration-system)
2. [Plugin Architecture](#plugin-architecture)
3. [Python API Enhancements](#python-api-enhancements)
4. [Performance Optimization](#performance-optimization)
5. [Visualization and Monitoring](#visualization-and-monitoring)
6. [Advanced Commands and Scripts](#advanced-commands-and-scripts)
7. [Plugin Development Examples](#plugin-development-examples)
8. [Performance Benchmarking](#performance-benchmarking)
9. [Distributed Computing](#distributed-computing)
10. [Extensibility and Customization](#extensibility-and-customization)

## Advanced Configuration System

### Overview

The NLM now includes an advanced configuration system that supports multiple file formats including JSON, YAML, and the original text format. This provides power users with flexible configuration options and powerful command-line tools.

### Key Features

- **Multiple Format Support**: JSON, YAML, and text-based configuration files
- **Advanced Command Interface**: Powerful commands for loading, saving, exporting, and validating configurations
- **Type Safety**: Strong type checking with automatic type conversion
- **Schema Validation**: Configuration schema validation for robust error checking
- **Nested Configuration**: Support for hierarchical configuration structures

### Configuration Commands

```bash
# Load configuration from file
nlm load config.json

# Save configuration to file
nlm save output.json

# Export to different format
nlm export output.yaml yaml

# Validate configuration
nlm validate

# Merge configurations
nlm merge base_config.json override.json

# Show configuration information
nlm info
```

### Example Configuration Files

**JSON Configuration**:
```json
{
  "brain": {
    "type": "Adaptive",
    "neuron_count": 5000,
    "plasticity": {
      "stdp": {
        "learning_rate": 0.01,
        "time_constant": 20.0
      }
    }
  },
  "memory": {
    "working_memory_size": 1000,
    "episodic_capacity": 10000
  }
}
```

**YAML Configuration**:
```yaml
# Advanced NLM Configuration
brain:
  type: "Adaptive"
  neuron_count: 5000
  connectivity: "spatially_clustered"
  
plasticity:
  stdp:
    learning_rate: 0.01
    time_constant: 20.0
    ltp_weight: 0.012
    ltd_weight: 0.01
  
memory:
  working_memory_size: 1000
  episodic_capacity: 10000
  
neuromodulation:
  dopamine_enabled: true
  curiosity_enabled: true
  novelty_enabled: true
```

## Plugin Architecture

### Overview

NLM's plugin architecture allows power users to create custom neuron models, plasticity rules, neuromodulators, sensors, actions, memory systems, and cognitive systems. This provides unprecedented flexibility for customization and research.

### Plugin Types

1. **Neuron Models**: Custom neuron dynamics (Izhikevich, Hodgkin-Huxley, etc.)
2. **Plasticity Rules**: Custom learning rules (STDP variants, Hebbian, etc.)
3. **Neuromodulators**: Custom neuromodulation systems (dopamine, curiosity, etc.)
4. **Sensors**: Custom sensory input processing
5. **Actions**: Custom motor output generation
6. **Memory Systems**: Custom memory architectures
7. **Cognitive Systems**: Custom cognitive processing

### Plugin Development

#### Neuron Model Plugin Example

```cpp
// Izhikevich Neuron Model Plugin
class IzhikevichNeuronModelPlugin : public NeuronModelPlugin {
public:
    IzhikevichNeuronModelPlugin() {
        metadata.name = "izhi2007";
        metadata.version = "1.0.0";
        metadata.description = "Izhikevich 2007 adaptive exponential integrate-and-fire model";
        metadata.type = PluginType::NeuronModel;
    }
    
    std::unique_ptr<Neuron> createNeuron(NeuronId id) override {
        auto neuron = std::make_unique<Neuron>(id);
        // Configure Izhikevich parameters
        neuron->setType(NeuronType::Internal);
        neuron->setMembranePotential(-70.0f);
        neuron->setThreshold(-50.0f);
        return neuron;
    }
};
```

#### Plasticity Rule Plugin Example

```cpp
// Simple STDP Plasticity Rule
class SimpleSTDPPlasticityRulePlugin : public PlasticityRulePlugin {
public:
    std::string getRuleName() const override {
        return "Simple STDP";
    }
    
    void updateSynapticWeight(float& weight, 
                             const std::vector<Timestamp>& preSpikeHistory,
                             const std::vector<Timestamp>& postSpikeHistory,
                             float learningRate,
                             float timeConstant) override {
        // Implement STDP learning rule
        if (postSpikeHistory.empty() || preSpikeHistory.empty()) {
            return;
        }
        
        Timestamp lastPreSpike = preSpikeHistory.back();
        Timestamp lastPostSpike = postSpikeHistory.back();
        float dt = static_cast<float>(lastPostSpike - lastPreSpike);
        
        float weightChange = 0.0f;
        if (dt > 0) {
            weightChange = learningRate * std::exp(-dt / timeConstant);
        } else {
            weightChange = -learningRate * std::exp(dt / timeConstant);
        }
        
        weight += weightChange;
        weight = std::clamp(weight, -1.0f, 1.0f);
    }
};
```

### Plugin Management

#### Command Line Interface

```bash
# List available plugins
nlm plugins list

# Load a plugin
nlm plugins load my_custom_plugin.so

# Unload a plugin
nlm plugins unload plugin_name

# Enable/disable a plugin
nlm plugins enable plugin_name
nlm plugins disable plugin_name

# Get plugin information
nlm plugins info plugin_name
```

#### Python API for Plugins

```python
import pynlm

# Create a custom configuration
config = pynlm.createDefaultConfig()

# Enable advanced features
config.set("enable_plugins", True)
config.set("plugin_directory", "/path/to/plugins")

# Load custom neuron model
brain = pynlm.createBrain(config)
brain.initialize()

# Use custom plasticity rule
brain.getSTDP().setLTPWeight(0.02)
brain.getSTDP().setLTDWeight(0.015)
```

## Python API Enhancements

### Advanced Brain Operations

The Python API now includes advanced operations for power users:

```python
import pynlm
import time

# Create brain with advanced configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 10000)
config.set("enable_advanced_learning", True)
config.set("enable_structural_plasticity", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Batch processing
start_time = time.time()
brain.batchStep(10000, 0.001)
end_time = time.time()
print(f"Processed 10000 steps in {end_time - start_time:.2f} seconds")

# Performance monitoring
metrics = brain.getPerformanceMetrics()
print(f"Simulation speed: {metrics.simulationSpeed} steps/sec")
print(f"Spike efficiency: {metrics.spikeEfficiency} spikes/step")

# Advanced learning statistics
stats = brain.getLearningStats()
print(f"STDP weight change: {stats.stdpWeightChange}")
print(f"Learning efficiency: {stats.learningEfficiency}")

# Complete simulation loop with world
world = pynlm.createSimpleWorld()
world.configure(100, 100, 20, 20)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Run simulation
brain.runSimulation(world, 5000, 0.1)

print("Simulation completed successfully!")
```

### Python Plugin System

```python
import pynlm

# Create advanced configuration
config = pynlm.createAdvancedConfig()
config.loadFromFile("plugins_config.json")

# Load custom neuron models
brain = pynlm.createBrain(config)

# Get available plugins
plugins = pynlm.getAvailablePlugins()
for plugin in plugins:
    if plugin.getType() == "neuron_model":
        print(f"Found neuron model: {plugin.getName()}")

# Create neuron using custom model
neuron = brain.createNeuronWithPlugin("my_custom_neuron")

# Apply custom plasticity rule
brain.applyPlasticityRule("custom_stdp", pre_spikes, post_spikes, 0.01, 20.0)
```

## Performance Optimization

### Memory Pool Optimizations

NLM includes memory pool optimizations for large-scale simulations:

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("enable_memory_pools", True)
config.set("pool_size", 100000)
config.set("enable_vectorization", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable SIMD optimizations
brain.enableSIMD(true)

# Monitor memory usage
memory_info = brain.getMemoryInfo()
print(f"Active neurons: {memory_info.activeNeurons}")
print(f"Memory efficiency: {memory_info.efficiency}")
```

### Performance Benchmarking

```python
import pynlm
import time
import pynlm.benchmark as benchmark

config = pynlm.createDefaultConfig()
config.set("neuron_count", 50000)

brain = pynlm.createBrain(config)
brain.initialize()

# Create benchmark suite
bench = benchmark.PerformanceBenchmark(brain)

# Run benchmarks
results = bench.runAllTests()

# Get performance summary
print(benchmark.getPerformanceSummary(results))
```

## Visualization and Monitoring

### Advanced Visualization

```python
import pynlm

config = pynlm.createDefaultConfig()
config.set("enable_visualization", True)
config.set("visualization_mode", "all")
config.set("update_rate", 60)

brain = pynlm.createBrain(config)

# Create visualization interface
viz = pynlm.createVisualization()

# Configure visualization
viz.configure({
    "show_neurons": True,
    "show_synapses": True,
    "show_spikes": True,
    "show_labels": False,
    "animate": True
})

# Set visualization mode
viz.setVisualizationMode(nlm.VisualizationMode.All)

# Update visualization during simulation
for i in range(1000):
    brain.step(i)
    viz.update()
    if i % 100 == 0:
        viz.render()

# Export visualization data
viz.exportData("simulation_data.json", "json")
```

### Performance Monitoring

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Start performance monitoring
brain.startPerformanceMonitoring()

# Run simulation while monitoring
start_time = time.time()
for i in range(10000):
    brain.step(i)
    
    if i % 1000 == 0:
        metrics = brain.getPerformanceMetrics()
        print(f"Step {i}: {metrics.simulationSpeed:.2f} steps/sec")

end_time = time.time()
total_time = end_time - start_time

# Get performance report
report = brain.getPerformanceReport()
print(benchmark.formatReport(report))
```

## Advanced Commands and Scripts

### Command Line Interface

```bash
# NLM Advanced Command Line Interface

# Help
nlm help
nlm help <command_name>

# List all commands
nlm commands

# Execute command with arguments
nlm <command> <arg1> <arg2> ...

# Configuration commands
nlm load config.json
nlm save current_config.json
nlm export simulation_data.csv csv
nlm validate config.json
nlm merge base.json override.json
nlm info

# Plugin commands
nlm plugins list
nlm plugins load plugin.so
nlm plugins unload plugin_name
nlm plugins enable plugin_name

# Simulation commands
nlm run simulation_config.json
nlm benchmark config.json
nlm profile config.json

# Export commands
nlm export data.json json
nlm export data.yaml yaml
nlm export data.txt text
```

### Advanced Scripts

```python
# Advanced simulation script
#!/usr/bin/env python3
import pynlm
import argparse
import json
import time

def main():
    parser = argparse.ArgumentParser(description="Advanced NLM Simulation")
    parser.add_argument("--config", required=True, help="Configuration file")
    parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    parser.add_argument("--output", help="Output file for results")
    parser.add_argument("--benchmark", action="store_true", help="Run performance benchmarks")
    parser.add_argument("--visualize", action="store_true", help="Enable visualization")
    
    args = parser.parse_args()
    
    # Load configuration
    config = pynlm.createAdvancedConfig()
    config.loadFromFile(args.config)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable visualization if requested
    if args.visualize:
        viz = pynlm.createVisualization()
        viz.initialize()
    
    # Run benchmark if requested
    if args.benchmark:
        import pynlm.benchmark
        benchmark = pynlm.benchmark.PerformanceBenchmark(brain)
    
    # Run simulation
    start_time = time.time()
    for i in range(args.steps):
        brain.step(i)
        
        if i % 100 == 0 and args.visualize:
            viz.update()
            viz.render()
    
    end_time = time.time()
    
    # Save results
    if args.output:
        results = {
            "total_time": end_time - start_time,
            "steps_performed": args.steps,
            "final_spike_count": brain.getTotalSpikeCount(),
            "average_firing_rate": brain.getAverageFiringRate()
        }
        
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2)
    
    print(f"Simulation completed in {end_time - start_time:.2f} seconds")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")

if __name__ == "__main__":
    main()
```

## Plugin Development Examples

### Complete Plugin Example

```cpp
// ExamplePlugin.hpp
#pragma once

#include "plugins/PluginSystem.hpp"

namespace nlm {
class ExamplePlugin : public PluginInterface {
public:
    ExamplePlugin();
    ~ExamplePlugin() override;
    
    const PluginMetadata& getMetadata() const override;
    bool initialize() override;
    void shutdown() override;
    bool isInitialized() const override;
    const PluginCapabilities& getCapabilities() const override;
    
    // Additional plugin-specific methods
    void customFunction();
};
}

// ExamplePlugin.cpp
#include "ExamplePlugin.hpp"

namespace nlm {
ExamplePlugin::ExamplePlugin() {
    metadata.name = "example_plugin";
    metadata.version = "1.0.0";
    metadata.author = "NLM Developers";
    metadata.description = "Example plugin demonstrating plugin development";
    metadata.license = "MIT";
    metadata.type = PluginType::NeuronModel;
}

ExamplePlugin::~ExamplePlugin() = default;

const PluginMetadata& ExamplePlugin::getMetadata() const {
    return metadata;
}

bool ExamplePlugin::initialize() {
    initialized = true;
    return true;
}

void ExamplePlugin::shutdown() {
    initialized = false;
}

bool ExamplePlugin::isInitialized() const {
    return initialized;
}

const PluginCapabilities& ExamplePlugin::getCapabilities() const {
    return capabilities;
}

void ExamplePlugin::customFunction() {
    // Plugin-specific functionality
}
}
```

### Building Custom Plugins

```bash
# Build a custom plugin
g++ -std=c++20 -I./include -I./plugins -fPIC -shared custom_plugin.cpp -o libcustom_plugin.so

# Load the plugin
nlm plugins load libcustom_plugin.so

# Verify the plugin was loaded
nlm plugins list
```

## Performance Benchmarking

### Benchmarking Framework

```python
import pynlm.benchmark as benchmark
import pynlm

# Create benchmark configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 10000)
config.set("simulation_timestep", 0.001)

# Create benchmark suite
brain = pynlm.createBrain(config)
benchmark_suite = benchmark.PerformanceBenchmark(brain)

# Add test cases
benchmark_suite.addTest("plasticity", test_plasticity_performance)
benchmark_suite.addTest("memory", test_memory_usage)
benchmark_suite.addTest("simulation", test_simulation_speed)

# Run benchmarks
results = benchmark_suite.runAllTests()

# Generate report
report = benchmark_suite.getReport()
print(benchmark.formatReport(report))

# Export results
benchmark_suite.exportResults("benchmark_results.json")
```

### Performance Metrics

```python
# Performance metrics
class PerformanceMetrics:
    def __init__(self):
        self.simulation_speed = 0.0  # steps per second
        self.spike_efficiency = 0.0  # spikes per millisecond
        self.energy_consumption = 0.0  # arbitrary energy units
        self.plasticity_rate = 0.0  # plasticity events per step
        self.learning_rate = 0.0  # weight change magnitude
        self.memory_utilization = 0.0  # memory usage percentage
        self.total_spikes = 0
        self.active_neurons = 0
        self.average_adaptation = 0.0
        self.synaptic_efficiency = 0.0

# Benchmarking utilities
class BenchmarkSuite:
    def __init__(self, brain):
        self.brain = brain
        self.tests = []
    
    def addTest(self, name, test_function):
        self.tests.append((name, test_function))
    
    def runAllTests(self):
        results = {}
        for name, test_function in self.tests:
            results[name] = test_function()
        return results
```

## Distributed Computing

### Parallel Simulation

```python
import pynlm
import multiprocessing as mp
import time

def runSimulation(config, steps, worker_id):
    """Run simulation in a separate process"""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    start_time = time.time()
    for i in range(steps):
        brain.step(i)
    
    end_time = time.time()
    
    return {
        "worker_id": worker_id,
        "steps_performed": steps,
        "time_elapsed": end_time - start_time,
        "spike_count": brain.getTotalSpikeCount(),
        "final_firing_rate": brain.getAverageFiringRate()
    }

def main():
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 5000)
    
    # Number of workers
    num_workers = 4
    steps_per_worker = 1000
    
    # Create processes
    processes = []
    for i in range(num_workers):
        p = mp.Process(target=runSimulation, args=(config, steps_per_worker, i))
        processes.append(p)
        p.start()
    
    # Wait for all processes to complete
    results = []
    for p in processes:
        p.join()
        results.append(p.get())
    
    # Aggregate results
    total_steps = sum(r["steps_performed"] for r in results)
    total_time = sum(r["time_elapsed"] for r in results)
    total_spikes = sum(r["spike_count"] for r in results)
    
    print(f"Distributed simulation completed:")
    print(f"  Total steps: {total_steps}")
    print(f"  Total time: {total_time:.2f} seconds")
    print(f"  Average speed: {total_steps/total_time:.2f} steps/sec")
    print(f"  Total spikes: {total_spikes}")

if __name__ == "__main__":
    main()
```

## Extensibility and Customization

### Extending NLM

The NLM framework is designed to be highly extensible. Power users can:

1. **Create custom neuron models** using the plugin system
2. **Implement custom plasticity rules** for novel learning behaviors
3. **Extend sensory and motor systems** for domain-specific applications
4. **Add custom memory architectures** for specialized memory requirements
5. **Develop custom cognitive systems** for advanced reasoning capabilities
6. **Integrate external data sources** for real-world applications
7. **Create custom visualization tools** for specialized analysis
8. **Implement distributed computing** for large-scale simulations

### Customization Best Practices

1. **Use the plugin system** for modular extensions
2. **Follow the existing API patterns** for consistency
3. **Implement proper error handling** and validation
4. **Provide comprehensive documentation** for custom components
5. **Use configuration files** for flexible customization
6. **Implement performance monitoring** for optimization
7. **Support serialization/deserialization** for checkpointing
8. **Provide clear interfaces** between components

### Future Extensibility

The NLM framework is designed with future extensibility in mind:

- **Plugin system** for dynamic component loading
- **Configuration system** for flexible customization
- **Performance monitoring** for optimization insights
- **Visualization system** for real-time analysis
- **Distributed computing** for large-scale simulations
- **Advanced Python API** for scripting and automation
- **Comprehensive testing** for robust development
- **Documentation and examples** for user guidance

## Conclusion

The NLM framework provides power users with advanced tools for neural simulation and research. This document covers the advanced configuration system, plugin architecture, Python API enhancements, performance optimization, visualization and monitoring, advanced commands and scripts, plugin development examples, performance benchmarking, distributed computing, and extensibility and customization.

Power users can leverage these features to:

- Create custom neural models and learning rules
- Optimize performance for large-scale simulations
- Analyze and visualize complex neural dynamics
- Automate complex simulation workflows
- Integrate NLM with external tools and data sources
- Extend NLM for specialized research applications

The NLM framework continues to evolve with new features and capabilities, providing a robust foundation for neural simulation research and applications.
