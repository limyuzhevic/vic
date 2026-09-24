# NLM User Guide: Tutorials

# Welcome to NLM Tutorials

This guide provides step-by-step tutorials to help you get started with NLM (Neural Learning Machine) and master its various features.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Basic Neuron Simulation](#basic-neuron-simulation)
3. [Learning Experiments](#learning-experiments)
4. [World Interaction Examples](#world-interaction-examples)
5. [Advanced Usage](#advanced-usage)
6. [Performance Optimization Tips](#performance-optimization-tips)
7. [Common Pitfalls and Solutions](#common-pitfalls-and-solutions)

# Getting Started

## Installation

NLM is a C++ project that requires compilation. The project uses CMake for build configuration.

### Prerequisites

- C++17 compatible compiler
- CMake 3.10 or higher
- Build tools (make/ninja)

### Basic Build

1. Clone the repository
2. Create a build directory:

```bash
mkdir build && cd build
```

3. Configure with CMake:

```bash
cmake ..
```

4. Build the project:

```bash
cmake --build . --config Release
```

### Running NLM

After building, you can run the main NLM program with:

```bash
./nlm
```

or specify a custom configuration file:

```bash
./nlm --config my_config.cfg
```

## Basic Neuron Simulation

This tutorial demonstrates how to create and run a basic neuron simulation.

### Example 1: Simple LIF Neuron

```cpp
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"

int main() {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.2);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain! Check configuration parameters.");
        return 1;
    }
    
    // Run simulation for 1000 steps
    for (uint64_t step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    NLM_LOG_INFO("Simulation completed successfully!");
    brain->logStatus();
    return 0;
}
```

### Example 2: Custom Configuration

Create a configuration file `custom.cfg`:

```
# Custom configuration example
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 500
region_count = 2
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018
log_level = INFO
log_to_file = true
log_filename = nlm_custom.log
```

Run with this configuration:

```bash
./nlm --config custom.cfg
```

## Learning Experiments

### Exploring Plasticity

This tutorial shows how to set up and run plasticity experiments.

```cpp
void runPlasticityLearningExperiment() {
    NLM_LOG_INFO("=== Plasticity Learning Experiment ===");
    
    // Create configuration optimized for learning
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200);
    config->set("region_count", 1);
    config->set("connection_probability", 0.3);
    config->set("stdp_ltp_weight", 0.03);
    config->set("stdp_ltd_weight", 0.025);
    config->set("synaptogenesis_rate", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Enable plasticity
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& synapse : region->getSynapses()) {
            synapse->enablePlasticity(true, true, true);
        }
    }
    
    // Apply learning stimuli
    for (int trial = 0; trial < 500; ++trial) {
        // Simulate correlated pre-post activity
        brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        brain->step(trial, trial * 0.001);
    }
    
    // Analyze results
    brain->logStatus();
}
```

## World Interaction Examples

### Agent in a Simple World

This example shows how to create an agent that interacts with an environment.

```cpp
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"

void runWorldInteractionExample() {
    NLM_LOG_INFO("=== World Interaction Example ===");
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create world
    SimpleWorld world;
    world.initialize(20, 20);
    
    // Create agent
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    
    NLM_LOG_INFO("Agent initialized in world");
    
    // Run interaction loop
    for (int step = 0; step < 200; ++step) {
        // Get observation from world
        SensoryPercept percept = world.observe(brain->getRegion(RegionId(1)).get());
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain processing
        brain->step(step, step * 0.001);
        
        // Get action
        MotorCommand cmd = agent.decodeMotorCommand();
        
        // Apply to world
        world.applyAction(brain->getRegion(RegionId(1)).get(), cmd);
        
        // Log progress
        if (step % 50 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Position: " + std::to_string(world.getAgentX()) + ", " +
                        std::to_string(world.getAgentY()));
        }
    }
    
    NLM_LOG_INFO("World interaction completed!");
}
```

## Advanced Usage

### Configuration Validation

Learn how to validate configuration parameters for optimal performance:

```cpp
bool validateConfiguration(const Config& config) {
    bool isValid = true;
    
    // Check neuron count
    int neuronCount = config.getOr<int>("neuron_count", 100);
    if (neuronCount < 10) {
        NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuronCount) +
                     ". Minimum 10 neurons required.");
        isValid = false;
    } else if (neuronCount > 10000) {
        NLM_LOG_WARNING("Large neuron_count (" + std::to_string(neuronCount) +
                       ") may impact performance. Consider reducing for faster testing.");
    }
    
    // Check timestep
    double timestep = config.getOr<double>("simulation_timestep", 0.001);
    if (timestep <= 0.0) {
        NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(timestep) +
                     ". Must be positive.");
        isValid = false;
    } else if (timestep > 0.1) {
        NLM_LOG_WARNING("Large timestep (" + std::to_string(timestep) +
                       ") may cause simulation instability.");
    }
    
    // Check connection probability
    float connProb = config.getOr<float>("connection_probability", 0.1);
    if (connProb < 0.0 || connProb > 1.0) {
        NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(connProb) +
                     ". Must be between 0.0 and 1.0.");
        isValid = false;
    }
    
    return isValid;
}
```

### Error Recovery Example

How to implement error recovery in your NLM applications:

```cpp
class NLMApplication {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    int maxRetries_ = 3;
    
public:
    bool initializeWithRetry() {
        for (int attempt = 1; attempt <= maxRetries_; ++attempt) {
            try {
                // Attempt initialization
                if (initializeBrain()) {
                    NLM_LOG_INFO("Successfully initialized on attempt " + 
                               std::to_string(attempt));
                    return true;
                }
                
                // Adjust parameters for next attempt
                if (attempt < maxRetries_) {
                    adjustParametersForRetry();
                }
                
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Initialization attempt " + std::to_string(attempt) +
                               " failed: " + std::string(e.what()));
                if (attempt == maxRetries_) {
                    NLM_LOG_ERROR("Failed to initialize after " +
                                 std::to_string(maxRetries_) + " attempts. " +
                                 "Check system resources and configuration.");
                    return false;
                }
            }
        }
        return false;
    }
    
private:
    bool initializeBrain() {
        // Implementation here
        return brain_->initialize();
    }
    
    void adjustParametersForRetry() {
        // Reduce complexity, increase stability
        config_->set("neuron_count", config_->getOr<int>("neuron_count", 100) / 2);
        config_->set("connection_probability", 0.05);
        NLM_LOG_INFO("Adjusted parameters for retry: smaller network, simpler connectivity");
    }
};
```

## Performance Optimization Tips

1. **Neuron Count**: For testing, use 100-500 neurons. For production, consider 1000+ neurons.

2. **Timestep Size**: Use 0.001 for accuracy, 0.01 for faster execution.

3. **Log Level**: Set to `ERROR` for production, `INFO` for development, `DEBUG` for troubleshooting.

4. **Memory Management**: Clean up brain and config objects when no longer needed.

5. **Batch Operations**: Process multiple steps between logging to improve performance.

## Common Pitfalls and Solutions

### Issue 1: "No neurons found!"

**Problem**: Brain initialization fails because no neurons were created.

**Solution**:
```cpp
// Check configuration parameters
int neuronCount = config->getOr<int>("neuron_count", 0);
if (neuronCount < 1) {
    NLM_LOG_ERROR("neuron_count must be at least 1. Current value: " +
                 std::to_string(neuronCount));
    return 1;
}

float connProb = config->getOr<float>("connection_probability", 0.0);
if (connProb <= 0.0) {
    NLM_LOG_ERROR("connection_probability must be positive. Current value: " +
                 std::to_string(connProb));
    return 1;
}
```

### Issue 2: Memory Issues with Large Networks

**Problem**: Application crashes due to insufficient memory.

**Solution**:
- Reduce neuron count
- Use smaller timestep
- Disable development features
- Monitor memory usage

### Issue 3: Simulation Instability

**Problem**: Neurons fire erratically or the simulation diverges.

**Solution**:
```cpp
// Check timestep
if (timestep > 0.01) {
    NLM_LOG_WARNING("Large timestep may cause instability. Consider reducing to 0.001.");
}

// Check STDP parameters
if (stdp_ltp_weight <= 0.0 || stdp_ltd_weight <= 0.0) {
    NLM_LOG_ERROR("STDP weights must be positive.");
}
```

### Issue 4: File I/O Errors

**Problem**: Configuration files cannot be read or written.

**Solution**:
```cpp
// Check file permissions
std::ifstream file("configs/default.cfg");
if (!file.is_open()) {
    NLM_LOG_ERROR("Cannot read configuration file: configs/default.cfg");
    NLM_LOG_ERROR("Check file permissions and existence.");
    NLM_LOG_ERROR("Create the file or run with default configuration.");
    return 1;
}
```

## Next Steps

1. Try the basic neuron simulation tutorial
2. Experiment with different configuration parameters
3. Explore the examples in the examples/ directory
4. Consult the error reference guide for troubleshooting
5. Join the NLM community for advanced usage tips

---

*For more detailed information, refer to the EXAMPLES.md and ERRORS.md files.*
