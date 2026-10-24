# NLM (熙然) - Advanced User Guide

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain system at Phase 6 (Final Integration). This document provides advanced users with information on extending, configuring, and working with the integrated brain systems.

## Installation and Build

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with multiple threads
make -j4
```

### Available Executables

- `nlm` - Basic neural computation demo (Phase 2)
- `nlm_phase3_demo` - Phase 3 learning experiments
- `nlm_phase4_demo` - Phase 4 cognitive experiments
- `nlm_phase6_demo` - Phase 6 integration demo (new!)

### Running Phase 6 Integration Demo

```bash
# Basic usage - shows integration verification
./nlm_phase6_demo

# With custom configuration
./nlm_phase6_demo --neurons=5000 --steps=5000 --region_count=2 --connection_prob=0.05

# With checkpointing disabled
./nlm_phase6_demo --disable_checkpointing

# With replay disabled
./nlm_phase6_demo --disable_replay

# With development disabled
./nlm_phase6_demo --disable_development

# Quick integration verification only
./nlm_phase6_demo --quick

# Verbose output
./nlm_phase6_demo --verbose
```

## Plugin Architecture

NLM includes a powerful plugin system for extending brain functionality. The plugin system allows you to add custom neural dynamics, plasticity rules, memory systems, cognition modules, and neuromodulation effects.

### Plugin System Overview

The plugin system consists of:

1. **NLMPlugin** - Base class for all plugins
2. **PluginRegistry** - Manages plugin loading and unloading
3. **PluginFactory** - Creates plugin instances
4. **Built-in Plugins** - Example plugins for extending functionality

### Plugin Development

#### Creating a Custom Plugin

Create a new C++ file in the `src/plugin/` directory or in your own project:

```cpp
#include "plugin/NLMPluginSystem.hpp"

class CustomPlugin : public nlm::NLMPlugin {
public:
    std::string getName() const override { return "CustomPlugin"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    bool initialize(nlm::Brain* brain) override {
        if (!NLMPlugin::initialize(brain)) return false;
        
        // Initialize custom functionality
        // Add custom neural dynamics, plasticity rules, etc.
        
        return true;
    }
    
    void processStep(nlm::SimulationStep step, nlm::Timestamp time) override {
        // Custom processing logic
    }
};

// Plugin factory function
extern "C" std::shared_ptr<nlm::NLMPlugin> create_custom_plugin() {
    return std::make_shared<CustomPlugin>();
}
```

#### Building Plugins

To build a plugin as a shared library:

```bash
# Build with CMake
add_library(custom_plugin SHARED custom_plugin.cpp)
target_link_libraries(custom_plugin nlm_core)

# For dynamic loading
# Add to CMakeLists.txt:
set_target_properties(custom_plugin PROPERTIES PREFIX "" SUFFIX ".so")
```

### Using the Plugin System

#### Loading Plugins at Runtime

```cpp
#include "plugin/NLMPluginSystem.hpp"

int main() {
    nlm::PluginRegistry registry;
    
    // Load a built-in plugin
    if (registry.loadPlugin("MemoryEnhancement")) {
        std::cout << "Memory enhancement plugin loaded successfully" << std::endl;
    }
    
    // Load a custom plugin from shared library
    if (registry.loadPlugin("./libcustom_plugin.so")) {
        std::cout << "Custom plugin loaded successfully" << std::endl;
    }
    
    // Get and initialize plugins
    auto memoryPlugin = registry.getPlugin("MemoryEnhancement");
    auto customPlugin = registry.getPlugin("CustomPlugin");
    
    // Initialize plugins with brain instance
    nlm::Brain brain(config);
    registry.initializeAllPlugins(&brain);
    
    // Process simulation steps
    for (nlm::SimulationStep step = 0; step < 10000; ++step) {
        registry.processStepAllPlugins(step, step * 0.001);
        
        // Or use the integration coordinator
        if (auto* coordinator = brain.getIntegrationCoordinator()) {
            coordinator->processStep(step, step * 0.001);
        }
    }
    
    // Cleanup
    registry.shutdownAllPlugins();
    
    return 0;
}
```

#### Built-in Plugins

NLM includes several built-in enhancement plugins:

1. **MemoryEnhancement** - Increases memory capacity and duration
2. **PredictionEnhancement** - Improves prediction accuracy and speed
3. **CognitionEnhancement** - Enhances planning and concept formation
4. **NeuromodulationEnhancement** - Strengthens neuromodulation effects

To use built-in plugins:

```cpp
#include "plugin/NLMPluginSystem.hpp"

int main() {
    nlm::PluginRegistry registry;
    
    // Add built-in plugins to factory
    nlm::PluginFactory factory;
    factory.registerPlugin("MemoryEnhancement", 
                          nlm::BuiltInPlugins::createMemoryEnhancementPlugin);
    factory.registerPlugin("PredictionEnhancement",
                          nlm::BuiltInPlugins::createPredictionEnhancementPlugin);
    factory.registerPlugin("CognitionEnhancement",
                          nlm::BuiltInPlugins::createCognitionEnhancementPlugin);
    factory.registerPlugin("NeuromodulationEnhancement",
                          nlm::BuiltInPlugins::createNeuromodulationEnhancementPlugin);
    
    // Create and load plugins
    auto memoryPlugin = factory.createPlugin("MemoryEnhancement");
    auto predictionPlugin = factory.createPlugin("PredictionEnhancement");
    
    // ... rest of plugin usage code
}
```

## Configuration System

NLM uses a flexible configuration system based on key-value pairs. The configuration supports:

- Integer values
- Floating-point values
- String values
- Boolean values

### Configuration Files

Create configuration files in the `configs/` directory:

```ini
# Example configuration file
# configs/custom.cfg

# Brain configuration
neuron_count = 5000
region_count = 2
connection_probability = 0.1
simulation_timestep = 0.001

# Memory configuration
working_memory_capacity = 500
episodic_memory_max_episodes = 5000

# Prediction configuration
prediction_update_interval = 50
prediction_accuracy_threshold = 0.8

# Development configuration
development_enabled = true
development_stage = CriticalPeriod

# Checkpoint configuration
checkpoint_enabled = true
checkpoint_path = ./checkpoints/custom_checkpoint
checkpoint_interval = 1000

# Logging configuration
log_level = INFO
log_to_file = true
log_filename = nlm_custom.log
```

### Command Line Configuration

Override configuration values using command line arguments:

```bash
# Override specific values
./nlm_phase6_demo --neurons=3000 --steps=3000 --connection_prob=0.05

# Load custom configuration file
./nlm_phase6_demo --config=./configs/my_config.cfg

# Override config file values
./nlm_phase6_demo --config=./configs/base_config.cfg --neurons=2000
```

## Integration Configuration

The BrainIntegrationCoordinator allows you to configure integration parameters:

```cpp
#include "brain/BrainIntegrationCoordinator.hpp"

int main() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 5000);
    config->set("region_count", 2);
    
    nlm::Brain brain(config);
    
    // Get integration coordinator
    auto* coordinator = brain.getIntegrationCoordinator();
    if (coordinator) {
        // Configure integration for learning mode
        coordinator->configureForLearningMode();
        
        // Or configure for specific parameters
        coordinator->setWorkingMemoryCapacity(1000);
        coordinator->setEpisodicMemoryMaxEpisodes(5000);
        coordinator->setPredictionUpdateInterval(25);
        coordinator->setPlanningDepth(8);
        coordinator->setAttentionParameters(0.3f, 2.0f);
    }
    
    return 0;
}
```

## Performance Configuration

NLM includes performance optimization features:

### Memory Pools

NLM uses memory pools for efficient allocation:

- Neural memory pools for neuron states
- Synapse memory pools for synaptic connections
- Event queue memory pools for spike events

### SIMD Vectorization

Neural computations use SIMD instructions for parallel processing:

- Vectorized neuron dynamics calculations
- Parallel synaptic current integration
- SIMD-based spike detection

### Parallel Processing

Multi-threading capabilities:

- Parallel region processing
- Concurrent event handling
- Parallel plasticity updates

To enable performance features:

```cpp
// Configure performance in BrainIntegrationCoordinator
auto* coordinator = brain.getIntegrationCoordinator();
coordinator->enablePerformanceIntegration(true);
```

## Advanced Usage Examples

### 1. Custom Brain Configuration

```cpp
#include "brain/Brain.hpp"
#include "brain/BrainIntegrationCoordinator.hpp"

nlm::Phase6Config phase6Config;
phase6Config.neuronCount = 10000;
phase6Config.maxSteps = 20000;
phase6Config.regionCount = 4;
phase6Config.connectionProbability = 0.05;
phase6Config.enableCheckpointing = true;
phase6Config.enableReplay = true;
phase6Config.enableDevelopment = true;

// Create brain with Phase 6 configuration
auto brain = std::make_shared<nlm::Brain>(config);
brain->initialize();

// Configure integration coordinator
auto* coordinator = brain->getIntegrationCoordinator();
coordinator->configureForDevelopmentMode();
```

### 2. Multi-Stage Simulation

```cpp
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

int main() {
    // Run Phase 2: Real Neural Computation
    std::cout << "=== Running Phase 2: Real Neural Computation ===" << std::endl;
    // ... Phase 2 demo code ...
    
    // Run Phase 6: Final Integration
    std::cout << "\n=== Running Phase 6: Final Integration ===" << std::endl;
    nlm::Phase6IntegratedExperiment phase6Experiment;
    nlm::Phase6Config phase6Config;
    phase6Config.neuronCount = 5000;
    phase6Config.maxSteps = 10000;
    
    auto result = phase6Experiment.run(phase6Config);
    
    // Analyze results
    std::cout << "Integration Results:" << std::endl;
    std::cout << "  Memory Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Prediction System: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "  Development System: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    
    return 0;
}
```

### 3. Custom Experiment Framework

```cpp
#include "experiments/Experiment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

class CustomExperiment : public nlm::Experiment {
public:
    CustomExperiment(std::shared_ptr<nlm::Brain> brain) : brain_(brain) {}
    
    nlm::ExperimentResult run() override {
        nlm::ExperimentResult result;
        
        // Custom experiment logic
        for (nlm::SimulationStep step = 0; step < 5000; ++step) {
            // Custom processing
            brain_->step(step, step * 0.001);
            
            // Collect metrics
            result.totalReward += brain_->getAverageFiringRate();
        }
        
        result.totalSteps = 5000;
        result.avgFiringRate = result.totalReward / 5000;
        result.experimentName = "CustomExperience";
        
        return result;
    }
    
private:
    std::shared_ptr<nlm::Brain> brain_;
};

int main() {
    // Create brain and experiment
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    CustomExperiment customExp(brain);
    auto result = customExp.run();
    
    std::cout << "Custom experiment completed:" << std::endl;
    std::cout << "  Name: " << result.experimentName << std::endl;
    std::cout << "  Total steps: " << result.totalSteps << std::endl;
    std::cout << "  Average firing rate: " << result.avgFiringRate << std::endl;
    
    return 0;
}
```

## Troubleshooting

### Common Issues and Solutions

**1. Plugin Loading Errors**

```bash
# Error: libcustom_plugin.so: cannot open shared object file
# Solution: Ensure the plugin is in the library search path
export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH
```

**2. Configuration File Not Found**

```bash
# Error: Using default configuration (configs/default.cfg not found)
# Solution: Create the configs directory or provide --config argument
mkdir -p configs
# Then copy or create your config file
```

**3. Integration Coordinator Not Available**

```cpp
// Problem: Integration coordinator returns nullptr
// Solution: Ensure Brain::Impl initializes integrationCoordinator
if (!brain_->getIntegrationCoordinator()) {
    std::cerr << "Integration coordinator not available!" << std::endl;
    return 1;
}
```

**4. Memory System Disconnected**

```cpp
// Problem: brain->getWorkingMemory() returns nullptr
// Solution: Check integration status
auto* coordinator = brain_->getIntegrationCoordinator();
if (coordinator && !coordinator->testMemoryIntegration()) {
    std::cerr << "Memory integration failed!" << std::endl;
    // Fix integration issues
}
```

## Further Reading

- **NLM Documentation**: Complete documentation available in the `docs/` directory
- **Scientific Papers**: See `docs/SCIENCE.md` for theoretical background
- **Architecture**: See `docs/ARCHITECTURE.md` for detailed system design
- **Experiments**: See `docs/EXPERIMENTS.md` for experiment methodologies

## License

MIT License - See LICENSE file for details.