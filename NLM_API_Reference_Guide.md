# NLM Project - Advanced API Reference Guide
## Brain Class API Documentation
This is a comprehensive API reference guide for the NLM project.

## Config Class API Documentation

### Overview
The `Config` class provides type-safe configuration management with validation and runtime configuration capabilities.

### Key Features

- Type-safe configuration values
- Runtime configuration updates
- Configuration validation
- Source tracking (default, file, command line, runtime)
- JSON serialization support


## AgentBrain Class API Documentation

### Overview
The `AgentBrain` class connects the NLM brain to the world, handling sensory transduction and motor decoding to enable embodied intelligence.

### Key Responsibilities

- Sensory input processing
- Motor output decoding
- Neuromodulation integration
- Development system coordination
- Reward-based learning


## SimpleWorld Class API Documentation

### Overview
The `SimpleWorld` class provides a 2D simulation environment for NLM agent development, offering sensory experience without semantic labels.

### Key Features

- Physical environment simulation
- Agent-environment interaction
- Sensory perception generation
- Object management
- Energy dynamics
- Time-based simulation

## Usage Examples

### Complete Example: Brain Configuration and Initialization

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuronCount", 1000);
    config->set("brain.regionCount", 5);
    config->set("simulation.dt", 0.1);
    config->set("simulation.maxTime", 100.0);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Brain initialization failed");
    }
    
    // Create SimpleWorld
    nlm::SimpleWorld world;
    world.configure(100.0f, 100.0f, 64, 64);
    world.setAgentStart(50.0f, 50.0f);
    
    // Run simulation
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.1);
        
        // Get sensory input
        auto percept = world.getSensoryPercept();
        brain->receiveSensoryInput(percept);
        
        // Generate action
        auto action = brain->produceAction();
        
        // Apply action to world
        world.applyMotorCommand(*action, step * 0.1);
    }
    
    return 0;
}
```

### Configuration Management Best Practices

```cpp
// Load configuration from JSON
nlm::Config config;
if (config.loadFromFile("config.json")) {
    // Validate critical settings
    if (config.has("brain.neuronCount")) {
        int neuronCount = config.getOr("brain.neuronCount", 1000);
        if (neuronCount < 100) {
            throw std::invalid_argument("Neuron count too small");
        }
    }
    
    // Set runtime overrides
    config.set("simulation.maxTime", 200.0, nlm::ConfigSource::Runtime);
}
```

### Memory System Usage

```cpp
// Store learning experience
auto workingMemory = brain->getWorkingMemory();
workingMemory->storePattern(sensoryPattern, 0.8f);

// Create episodic memory
auto episodicMemory = brain->getEpisodicMemory();
 episodicMemory->createEpisode(
    currentStep,
    sensoryInput,
    action,
    reward,
    predictionError
);
```

### Neuromodulation Integration

```cpp
// Process reward signal
auto dopamine = brain->getDopamine();
dopamine->update(reward, expectedReward);
brain->applyNeuromodulation(*dopamine);

// Monitor curiosity
auto curiosity = brain->getCuriosity();
if (curiosity->getExplorationDrive() > curiosityThreshold) {
    // Trigger exploration behavior
    brain->injectCurrentToNeurons(NeuronType::Exploration, 2.0f);
}
```

### Development System Management

```cpp
// Monitor developmental stage
auto devSystem = brain->getDevelopmentSystem();
auto currentStage = brain->getDevelopmentalStage();

// Trigger developmental phase
brain->develop();

// Set specific developmental goal
brain->setDevelopmentalStage(nlm::DevelopmentalStage::Adolescent);
```

## Configuration Validation Examples

### Valid Configuration

```cpp
// Create and validate configuration
nlm::Config config;

// Valid settings
config.set_with_validation("brain.neuronCount", 1000);
config.set_with_validation("brain.regionCount", 5);
config.set_with_validation("simulation.dt", 0.1);
config.set_with_validation("neuromodulation.level", 0.7);
config.set_with_validation("world.width", 100.0f);
```

### Invalid Configuration Handling

```cpp
// This will throw an exception
try {
    config.set_with_validation("brain.neuronCount", "invalid_string");
} catch (const nlm::ConfigurationError& e) {
    std::cerr << "Configuration error: " << e.what() << std::endl;
    // Use default value
    config.set("brain.neuronCount", 1000, nlm::ConfigSource::Runtime);
}
```

## Advanced Configuration Patterns

### 1. Configuration Class Pattern

```cpp
class BrainConfigurator {
public:
    static std::shared_ptr<nlm::Config> createDefaultConfig() {
        auto config = std::make_shared<nlm::Config>();
        
        // Set all default values
        config->set("brain.neuronCount", 1000);
        config->set("brain.regionCount", 5);
        config->set("brain.connectionProbability", 0.15);
        config->set("brain.excitationInhibitionRatio", 1.0f);
        
        // Load user overrides
        config->loadFromFile("user_config.json");
        
        // Apply command line overrides
        config->loadFromArgs(argc, argv);
        
        return config;
    }
};
```

### 2. Runtime Configuration Manager

```cpp
class RuntimeConfigManager {
private:
    std::shared_ptr<nlm::Config> config_;
    
public:
    RuntimeConfigManager(std::shared_ptr<nlm::Config> config) 
        : config_(config) {}
    
    void updateConfig(const std::string& key, double value) {
        config_->set(key, value, nlm::ConfigSource::Runtime);
        
        // Apply configuration changes
        applyChanges();
    }
    
    template<typename T>
    T getConfig(const std::string& key, const T& defaultValue) {
        return config_->getOr(key, defaultValue);
    }
    
private:
    void applyChanges() {
        // Reinitialize affected systems
        // Update neuromodulation parameters
        // Adjust simulation settings
    }
};
```

## Performance Optimization Tips

### 1. Batch Configuration Updates

```cpp
// Batch configuration updates for efficiency
std::vector<std::pair<std::string, nlm::ConfigValue>> updates = {
    {"brain.neuronCount", 1000},
    {"brain.regionCount", 5},
    {"simulation.dt", 0.1}
};

for (const auto& update : updates) {
    config->set(update.first, update.second);
}
```

### 2. Configuration Caching

```cpp
class CachedConfig {
private:
    std::shared_ptr<nlm::Config> config_;
    mutable std::unordered_map<std::string, nlm::ConfigValue> cache_;
    
public:
    T get(const std::string& key) const {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return std::get<T>(it->second);
        }
        
        std::optional<T> value = config_->get<T>(key);
        if (value) {
            cache_[key] = *value;
            return *value;
        }
        
        throw std::runtime_error("Configuration key not found");
    }
};
```

## Common Error Patterns and Solutions

### 1. Configuration Key Errors

**Problem**: Using non-existent configuration keys.

**Solution**:
```cpp
if (!config->has("critical.setting")) {
    config->set("critical.setting", defaultValue, nlm::ConfigSource::Runtime);
}
```

### 2. Type Mismatch Errors

**Problem**: Assigning wrong type to configuration values.

**Solution**:
```cpp
try {
    config->set_with_validation("brain.neuronCount", "1000");
} catch (const nlm::ConfigurationError& e) {
    config->set("brain.neuronCount", 1000, nlm::ConfigSource::Runtime);
}
```

### 3. Simulation Initialization Errors

**Problem**: Attempting to run simulation before initialization.

**Solution**:
```cpp
if (brain && brain->isInitialized()) {
    brain->step(currentStep);
} else {
    throw std::runtime_error("Brain not initialized");
}
```

## Conclusion

This API reference guide provides comprehensive documentation for the NLM project's core classes. The NLM framework offers a complete neural computing solution with:

- **Realistic neural simulation** with spiking dynamics
- **Advanced memory systems** for learning and adaptation
- **Neuromodulation mechanisms** for reward-based learning
- **Development systems** for autonomous growth
- **Embodied intelligence** through the AgentBrain and SimpleWorld integration

The API is designed for both simplicity and advanced customization, providing Pythonic bindings for quick adoption while maintaining full C++ power for complex scenarios.

---

*This guide covers NLM version 1.0.0. For the latest documentation, see the official NLM documentation repository.*
