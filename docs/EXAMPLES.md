# NLM Examples: Practical Usage Demonstrations

# Welcome to NLM Examples

This guide provides practical examples that demonstrate NLM's key features and capabilities. Each example is designed to be runnable and showcases specific aspects of the neural learning machine.

## Table of Contents

1. [Basic Neuron Simulation](#basic-neuron-simulation)
2. [Configuration Examples](#configuration-examples)
3. [Plasticity Examples](#plasticity-examples)
4. [Agent Examples](#agent-examples)
5. [Integration Examples](#integration-examples)
6. [Troubleshooting Examples](#troubleshooting-examples)

# Basic Neuron Simulation

## Example 1: Simple Network

```cpp
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"

int main() {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== Basic Neuron Simulation ===");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("connection_probability", 0.2);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain with error code BRAIN_INIT_001");
        return 1;
    }
    
    for (uint64_t step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    NLM_LOG_INFO("Simulation completed successfully!");
    brain->logStatus();
    return 0;
}
```

## Example 2: Configuration File

Create `configs/example.cfg`:

```
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 500
region_count = 2
connection_probability = 0.15
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.018
log_level = INFO
```

## Example 3: STDP Learning

```cpp
// Enable only STDP plasticity
if (auto* region = brain->getRegion(RegionId(1))) {
    for (auto& synapse : region->getSynapses()) {
        synapse->enablePlasticity(false, true, false);
    }
}

// Apply correlated pre-post activity
for (int trial = 0; trial < 50; ++trial) {
    neurons[0]->injectCurrent(60.0f);  // Pre fires first
    brain->step(trial * 2, trial * 2 * 0.001);
    neurons[1]->injectCurrent(60.0f);  // Post fires second
    brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
}
```

# Configuration Examples

## Example 4: Performance Configuration

```cpp
auto config = std::make_shared<Config>();
config->set("neuron_count", 500);
config->set("region_count", 2);
config->set("connection_probability", 0.1);
config->set("simulation_timestep", 0.002);
config->set("stdp_ltp_weight", 0.015);
config->set("stdp_ltd_weight", 0.018);
```

## Example 5: Large Network Configuration

```cpp
auto config = std::make_shared<Config>();
config->set("neuron_count", 5000);
config->set("region_count", 3);
config->set("connection_probability", 0.05);
config->set("random_seed", 12345);
```

# Plasticity Examples

## Example 6: Hebbian Plasticity

```cpp
if (auto* region = brain->getRegion(RegionId(1))) {
    for (auto& synapse : region->getSynapses()) {
        synapse->enablePlasticity(true, false, false);
    }
}
```

## Example 7: Combined Plasticity

```cpp
if (auto* region = brain->getRegion(RegionId(1))) {
    for (auto& synapse : region->getSynapses()) {
        synapse->enablePlasticity(true, true, true);
    }
}
```

# Agent Examples

## Example 8: Basic Agent

```cpp
AgentBrain agent(brain);
agent.initialize(world);
agent.enableRewardModulation(true);
agent.enableCuriosity(true);
```

## Example 9: Navigation Agent

```cpp
for (int step = 0; step < 200; ++step) {
    SensoryPercept percept = world.observe(brain->getRegion(RegionId(1)).get());
    agent.processSensoryInput(percept);
    brain->step(step, step * 0.001);
    MotorCommand cmd = agent.decodeMotorCommand();
    world.applyAction(brain->getRegion(RegionId(1)).get(), cmd);
}
```

# Integration Examples

## Example 10: Multi-Agent System

```cpp
std::vector<std::shared_ptr<Brain>> brains;
std::vector<std::shared_ptr<AgentBrain>> agents;

for (int i = 0; i < 5; ++i) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100 + i * 20);
    auto brain = std::make_shared<Brain>(config);
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(world);
    agents.push_back(agent);
    brains.push_back(brain);
}
```

# Troubleshooting Examples

## Example 11: Error Recovery

```cpp
bool initializeWithRecovery(const Config& config) {
    int attempts = 0;
    while (attempts < 3) {
        try {
            auto brain = std::make_shared<Brain>(config);
            if (brain->initialize()) {
                return true;
            }
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Attempt " + std::to_string(++attempts) + ": " + 
                           std::string(e.what()));
        }
    }
    return false;
}
```

# Compilation Instructions

```bash
mkdir build && cd build
cmake ..
make
./nlm
```

---

*For more detailed tutorials and error information, refer to the TUTORIALS.md and ERRORS.md files.*
