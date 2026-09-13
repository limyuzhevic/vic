# NLM Advanced User Guide

This document provides advanced users with guidance on extending and modifying the NLM (Neural Learning Machine) system.

## Overview

NLM is a modular, extensible neural simulation framework with Phase 6 representing the final integrated artificial brain implementation. The system is designed to be easily extended with new neural components, memory systems, and behaviors.

## Directory Structure

```
src/
├── core/          # Core utilities and types
├── brain/         # Neural computation (Phase 2+)
├── sensory/       # Sensory processing
├── motor/         # Motor control
├── environment/   # Environment interaction
├── agent/         # Agent-brain interface
├── world/         # World representation
├── experiments/   # Experimental frameworks
├── visualization/ # Visualization system
└── performance/   # Performance optimizations

README.md                     # Project overview and building
HOW_TO_USE.md                 # Basic usage guide
ADVISE.md                     # This advanced user guide
```

## Extending the Brain

### Creating New Neural Populations

The NLM brain is organized into regions, each containing neural populations. Here's how to add new population types:

```cpp
// In your custom brain implementation

class CustomRegion : public NeuralRegion {
public:
    CustomRegion(RegionId id, const std::string& name) 
        : NeuralRegion(id, name) {}
    
    // Add a new population type
    PopulationId addCustomPopulation(size_t neuronCount, NeuronType type) {
        // Implement your custom logic
        // This could include specialized connectivity patterns
        // Custom neuron behaviors, or learning mechanisms
        return addPopulation(neuronCount, type);
    }
};
```

### Extending Brain Initialization

To add custom initialization logic to the brain:

```cpp
// In your main.cpp or custom application

void customBrainInitialization(nlm::Brain* brain, const nlm::Config& config) {
    // Add custom population types
    auto* region = brain->getRegion(nlm::RegionId(1));
    if (region) {
        // Add custom sensory modality
        auto customSensoryId = region->addPopulation(100, nlm::NeuronType::Sensory);
        
        // Configure custom connectivity
        region->configurePopulationConnectivity(customSensoryId, 0.3f, 0.8f);
    }
    
    // Add custom plasticity rules
    brain->getSTDP()->configure(0.02f, 0.018f, 25.0f);
    brain->getHebbian()->configure(0.1f, 0.9f);
}

int main() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    if (brain->initialize()) {
        customBrainInitialization(brain.get(), *config);
        
        // Your custom logic here
        for (size_t i = 0; i < 1000; ++i) {
            brain->step(i, i * 0.001);
        }
    }
}
```

### Adding Custom Memory Systems

Memory systems are pluggable components. To add a new memory system:

```cpp
// Create a new memory system class
class CustomMemory : public nlm::NeuralWorkingMemory {
public:
    CustomMemory() : NeuralWorkingMemory() {}
    
    void initialize(nlm::Brain* brain) override {
        NeuralWorkingMemory::initialize(brain);
        // Add your custom initialization
        capacity_ = 500;  // Custom capacity
    }
    
    void update(nlm::TimestepDuration dt) override {
        NeuralWorkingMemory::update(dt);
        // Add your custom update logic
        applyCustomConsolidation();
    }
    
private:
    void applyCustomConsolidation() {
        // Implement your custom consolidation algorithm
    }
};

// Integration into brain
void integrateCustomMemory(nlm::Brain* brain) {
    auto customMem = std::make_unique<CustomMemory>();
    brain->setWorkingMemory(std::move(customMem));
}
```

## Configuration and Customization

### Advanced Configuration Options

The NLM configuration system supports complex nested settings. Here's how to use it:

```cpp
// Create a custom configuration
nlm::Config config;

// Set complex nested parameters
config.set("brain.memory.working_memory.capacity", 1000);
config.set("brain.memory.episodic.max_episodes", 5000);
config.set("brain.neuromodulation.dopamine.learning_rate", 0.05f);
config.set("brain.plasticity.stdp.lambda_pre", 0.8f);
config.set("brain.plasticity.stdp.lambda_post", -0.8f);

// Access configuration
auto capacity = config.getOr<size_t>("brain.memory.working_memory.capacity", 100);
```

### Configuration File Format

Create a custom configuration file (`configs/custom.cfg`):

```ini
# NLM Configuration File
[brain]
neuron_count = 2000
region_count = 3
connection_probability = 0.15

[brain.memory.working_memory]
capacity = 500
decay_rate = 0.01

[brain.memory.episodic]
max_episodes = 1000
replay_interval = 50

[brain.neuromodulation.dopamine]
learning_rate = 0.03
initial_level = 0.5

[brain.plasticity.stdp]
ltp_weight = 0.02
ltd_weight = 0.018
tau = 20.0
lambda_pre = 0.8
lambda_post = -0.8

[brain.development]
enable_structural_plasticity = true
initial_plasticity_rate = 0.1
```

## Command Line Interface

The NLM provides an extensive command-line interface for quick customization:

```bash
# Basic usage with custom parameters
./nlm --neurons 2000 --seed 12345 --timestep 0.0001

# With custom configuration file
./nlm --config my_config.cfg --plasticity 0.001

# All options
./nlm --help

# Available options:
# --config <file>        Load configuration from file
# --seed <number>       Random seed (default: 42)
# --neurons <count>      Number of neurons (default: 500)
# --timestep <seconds>   Simulation timestep (default: 0.001)
# --plasticity <rate>    Synaptogenesis rate (default: 0.0001)
# --help, -h             Show help message
```

## Example: Creating a Custom Learning Experiment

Here's a complete example of creating a custom learning experiment:

```cpp
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>

class CustomLearningExperiment {
public:
    struct CustomConfig {
        size_t neuronCount = 1000;
        size_t maxSteps = 5000;
        bool enableCustomLearning = true;
        float learningRate = 0.01f;
    };
    
    struct CustomResult {
        float totalReward = 0.0f;
        float learningProgress = 0.0f;
        size_t successfulEpisodes = 0;
        float adaptationRate = 0.0f;
    };
    
    CustomResult run(const CustomConfig& config) {
        CustomResult result;
        
        // Create configuration
        auto cfg = std::make_shared<nlm::Config>();
        cfg->set("neuron_count", config.neuronCount);
        cfg->set("stdp_ltp_weight", config.learningRate);
        cfg->set("synaptogenesis_rate", 0.00005f);
        
        // Create brain and environment
        auto brain = std::make_shared<nlm::Brain>(cfg);
        nlm::SimpleWorld world;
        nlm::AgentBrain agent(brain);
        
        world.initialize(16, 16);
        agent.initialize(world);
        
        // Custom learning loop
        for (size_t step = 0; step < config.maxSteps; ++step) {
            // Get observation
            auto percept = world.observe(brain->getRegions()[0].get());
            agent.processSensoryInput(percept);
            
            // Custom learning modulation
            if (config.enableCustomLearning && step % 100 == 0) {
                applyCustomLearning(brain.get(), step);
            }
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get action and apply
            auto cmd = agent.decodeMotorCommand();
            world.applyAction(brain->getRegions()[0].get(), cmd);
            
            // Compute and accumulate reward
            float reward = world.computeReward(brain->getRegions()[0].get());
            result.totalReward += reward;
            
            // Update development
            agent.applyRewardModulation(reward, 0.0f);
            agent.updateDevelopment(0.001);
        }
        
        // Compute learning metrics
        result.learningProgress = computeLearningProgress(brain.get());
        result.successfulEpisodes = brain->getEpisodicMemory() ? 
            brain->getEpisodicMemory()->getEpisodeCount() : 0;
        result.adaptationRate = computeAdaptationRate(brain.get());
        
        return result;
    }
    
private:
    void applyCustomLearning(nlm::Brain* brain, size_t step) {
        // Implement your custom learning rules
        // This could include:
        // - Custom neuromodulation patterns
        // - Specialized plasticity rules
        // - Activity-dependent learning
        
        // Example: Enhance synaptic strengthening based on reward
        if (auto* dopamine = brain->getDopamine()) {
            float level = dopamine->getLevel();
            if (level > 0.7f) {
                // Boost learning during high dopamine
                brain->getSTDP()->configure(0.03f, 0.028f, 20.0f);
            }
        }
    }
    
    float computeLearningProgress(nlm::Brain* brain) {
        // Calculate how much the brain has learned
        // This could be based on weight changes, firing rate adaptation, etc.
        return 0.5f;  // Placeholder
    }
    
    float computeAdaptationRate(nlm::Brain* brain) {
        // Measure how quickly the brain adapts to new inputs
        return 0.3f;  // Placeholder
    }
};

// Usage example
int main() {
    CustomLearningExperiment experiment;
    CustomLearningExperiment::CustomConfig config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCustomLearning = true;
    config.learningRate = 0.015f;
    
    std::cout << "=== Custom Learning Experiment ===" << std::endl;
    auto result = experiment.run(config);
    
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Learning progress: " << result.learningProgress << std::endl;
    std::cout << "Successful episodes: " << result.successfulEpisodes << std::endl;
    std::cout << "Adaptation rate: " << result.adaptationRate << std::endl;
    
    return 0;
}
```

## Performance Optimization Guide

### Profiling the NLM

To profile your NLM simulation:

```bash
# Build with profiling support
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j4

# Run with built-in profiling
./nlm --neurons 1000 --timestep 0.0001

# Check performance bottlenecks
./nlm --neurons 500 --timestep 0.0001
```

### Memory Management Best Practices

```cpp
// Use shared_ptr for brain management
auto brain = std::make_shared<nlm::Brain>(config);

// Use smart pointers for all neural objects
auto region = std::make_unique<nlm::NeuralRegion>(id, name);
brain->addRegion(std::move(region));

// Always check for null pointers before dereferencing
if (auto* region = brain->getRegion(nlm::RegionId(1))) {
    // Safe to use region
}
```

## Testing and Debugging

### Running Tests

```bash
# Build tests
cmake -DBUILD_TESTS=ON ..
make -j4

# Run all tests
./nlm_test

# Run specific test
./nlm_test --filter "brain"
```

### Debugging with GDB

```bash
# Launch NLM with gdb
gdb ./nlm

# Set breakpoints
break main.cpp:346

# Run with custom parameters
run --neurons 100 --seed 42
```

## Extensible Components

### Plugin Architecture

The NLM supports a plugin architecture for extending functionality:

```cpp
// Example plugin interface
class NLMPlugin {
public:
    virtual ~NLMPlugin() = default;
    virtual void initialize(nlm::Brain* brain) = 0;
    virtual void update(nlm::Brain* brain, float dt) = 0;
    virtual const std::string& getName() const = 0;
};

// Example plugin implementation
class CustomCuriosityPlugin : public NLMPlugin {
public:
    void initialize(nlm::Brain* brain) override {
        // Store brain reference for later use
        brain_ = brain;
    }
    
    void update(nlm::Brain* brain, float dt) override {
        // Apply custom curiosity-based exploration
        if (brain_->getCuriosity()) {
            brain_->getCuriosity()->setExplorationRate(0.8f);
        }
    }
    
    const std::string& getName() const override {
        static const std::string name = "CustomCuriosityPlugin";
        return name;
    }
    
private:
    nlm::Brain* brain_ = nullptr;
};
```

## Migration Guide

### From Phase 2 to Phase 6

If you're migrating from Phase 2 to Phase 6:

1. **Brain Class**: The brain class has been extended with many new systems
2. **Configuration**: New configuration options are available in the config system
3. **Memory Systems**: Multiple memory systems are now integrated (working, episodic, associative)
4. **Neuromodulation**: Enhanced neuromodulation with dopamine, curiosity, novelty detection
5. **Development**: Full developmental stages and plasticity modulation

### Backward Compatibility

The NLM maintains backward compatibility:

- All Phase 2 functionality is preserved
- Existing tests should still pass
- Command-line interface has been extended, not changed
- Configuration file format is extended, not broken

## Further Reading

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [NLM Scientific Background](docs/SCIENCE.md)
- [NLM Roadmap](docs/ROADMAP.md)
- [NLM Phase 6 Final Audit](docs/PHASE6_FINAL_AUDIT.md)

## Getting Help

### Community Resources

- GitHub Issues: For bug reports and feature requests
- Slack/Discord: For real-time discussion
- Documentation: Comprehensive API documentation in source headers
- Examples: Look in the `examples/` directory for additional use cases

### Support Channels

- **GitHub**: [github.com/nlm-project/nlm](https://github.com/nlm-project/nlm)
- **Documentation**: [nlm.readthedocs.io](https://nlm.readthedocs.io)
- **Forum**: [discourse.nlm-project.org](https://discourse.nlm-project.org)

## License

MIT License. See `LICENSE` file for details.

---

*This guide is continuously updated as new features are added to NLM. Check the repository for the latest version.*
