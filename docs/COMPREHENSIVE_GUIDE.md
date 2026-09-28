# NLM - Comprehensive C++ Usage Guide

**NLM (熙然)** is an experimental artificial developmental brain system. This guide provides comprehensive examples for using NLM in C++.

## Overview

NLM consists of three main components:

1. **Brain** - The neural substrate with neurons, synapses, and internal systems
2. **World** - The environment the brain interacts with
3. **Agent** - The interface connecting brain to world (sensory processing, motor decoding)

## Installation and Building

See README.md for build instructions:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Basic Examples

### Example 1: Silent Brain (Core Neural Computation)

This example demonstrates the brain's basic neural computation capabilities without environmental interaction.

```cpp
#include <iostream>
#include "nlm/core/Config/Config.hpp"
#include "nlm/brain/Brain.hpp"
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/world/SimpleWorld.hpp"

int main() {
    std::cout << "=== NLM Basic Example: Silent Brain ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    std::cout << "Brain initialized with " << brain->getTotalNeuronCount() 
              << " neurons and " << brain->getTotalSynapseCount() << " synapses" << std::endl;
    
    // Run simulation
    std::cout << "Running 1000 simulation steps..." << std::endl;
    for (int step = 0; step < 1000; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Show statistics
    std::cout << "\n=== Final Statistics ===" << std::endl;
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    
    // Save brain state for later use
    brain->save("silent_brain_checkpoint.nlm");
    
    std::cout << "\n✓ Silent brain example completed successfully!" << std::endl;
    return 0;
}
```

### Example 2: Brain in a Simple World (Complete Agent Loop)

This example demonstrates the complete agent-environment interaction loop.

```cpp
#include <iostream>
#include "nlm/core/Config/Config.hpp"
#include "nlm/brain/Brain.hpp"
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/world/SimpleWorld.hpp"

int main() {
    std::cout << "=== NLM Complete Agent Example ===" << std::endl;
    
    // Create configuration with agent-friendly settings
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 1);
    
    // Create brain, world, and agent
    auto brain = std::make_shared<nlm::Brain>(config);
    auto world = std::make_shared<nlm::SimpleWorld>();
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    // Configure world
    world->configure(20, 20, 8, 8);  // 20x20 world, 8x8 vision
    world->reset();
    
    // Initialize all components
    brain->initialize();
    agent->initialize(*world);
    
    // Enable learning features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    std::cout << "Starting agent simulation..." << std::endl;
    
    // Main simulation loop
    int steps = 0;
    int positive_rewards = 0;
    
    for (steps = 0; steps < 1000; ++steps) {
        // 1. Update world (environment dynamics)
        world->update(0.1);
        
        // 2. Get sensory input from world
        auto percept = world->getSensoryPercept();
        
        // 3. Process sensory input (brain receives input)
        agent->processSensoryInput(percept);
        
        // 4. Brain computes and makes decisions
        brain->step(steps, steps * 0.1);
        
        // 5. Agent decodes motor command from brain
        auto action = agent->decodeMotorCommand();
        
        // 6. Apply action in world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // 7. Apply reward modulation (learning)
        float reward = world->getCurrentReward();
        agent->applyRewardModulation(reward, agent->getPredictionError());
        
        // 8. Update development (brain maturation)
        agent->updateDevelopment(0.1);
        
        // Log progress periodically
        if (steps % 100 == 0) {
            std::cout << "Step " << steps << ": "
                      << "Action: " << static_cast<int>(action) << ", "
                      << "Reward: " << reward << ", "
                      << "Curiosity: " << agent->getCuriosityLevel() << ", "
                      << "Brain stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
            
            if (reward > 0) positive_rewards++;
        }
    }
    
    std::cout << "\n=== Simulation Results ===" << std::endl;
    std::cout << "Total steps: " << steps << std::endl;
    std::cout << "Positive rewards: " << positive_rewards << " (" 
              << (positive_rewards * 100.0 / steps) << "%)" << std::endl;
    std::cout << "Final curiosity level: " << agent->getCuriosityLevel() << std::endl;
    std::cout << "Final developmental stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
    
    // Save final brain state
    brain->save("agent_brain_final.nlm");
    
    std::cout << "\n✓ Agent simulation completed successfully!" << std::endl;
    return 0;
}
```

### Example 3: Learning Experiment with Plasticity

This example demonstrates how to set up and run a learning experiment with synaptic plasticity.

```cpp
#include <iostream>
#include "nlm/core/Config/Config.hpp"
#include "nlm/brain/Brain.hpp"
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/world/SimpleWorld.hpp"

int main() {
    std::cout << "=== NLM Learning Experiment ===" << std::endl;
    
    // Create brain with learning-focused configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 200);
    config->set("region_count", 1);
    config->set("connection_probability", 0.3f);
    
    // Enable plasticity in configuration
    config->set("stdp_ltp_weight", 0.05f);
    config->set("stdp_ltd_weight", 0.045f);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    brain->initialize();
    agent->enableRewardModulation(true);
    
    std::cout << "Recording initial brain state..." << std::endl;
    brain->save("before_learning.nlm");
    
    // Get initial statistics
    std::cout << "Initial spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Initial synapses: " << brain->getTotalSynapseCount() << std::endl;
    
    // Create learning pattern: reward for exploration
    std::cout << "Running learning phase (2000 steps)..." << std::endl;
    
    for (int step = 0; step < 2000; ++step) {
        // Exploration behavior
        if (step % 100 == 0) {
            std::cout << "Exploration step " << step << std::endl;
        }
        
        // Inject varying input patterns
        float inputPattern = 0.5f * sin(step * 0.01f) + 0.5f * cos(step * 0.02f);
        
        // Process through agent with reward
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->configure(10, 10, 4, 4);
        world->reset();
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step, step * 0.01);
        
        // Reward based on exploration (curiosity-driven)
        float curiosityReward = agent->getCuriosityLevel() * 10.0f;
        agent->applyRewardModulation(curiosityReward, 0.0f);
        
        // Occasionally reward novel actions
        if (step % 47 == 0) { // Prime number for variety
            agent->applyRewardModulation(5.0f, curiosityReward);
        }
    }
    
    std::cout << "\nLearning phase completed!" << std::endl;
    brain->save("after_learning.nlm");
    
    std::cout << "Final spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Final synapses: " << brain->getTotalSynapseCount() << std::endl;
    
    std::cout << "\n✓ Learning experiment completed successfully!" << std::endl;
    return 0;
}
```

## Advanced Features

### Advanced 1: Custom Configuration

```cpp
#include "nlm/core/Config/Config.hpp"

void setupAdvancedConfig(nlm::Config& config) {
    // Neural parameters
    config->set("neuron_count", 10000);
    config->set("region_count", 4);
    config->set("connection_probability", 0.1f);
    
    // Plasticity parameters
    config->set("stdp_ltp_weight", 0.1f);
    config->set("stdp_ltd_weight", 0.09f);
    config->set("stdp_tau", 30.0f);
    
    // Structural plasticity
    config->set("synaptogenesis_rate", 0.0005f);
    config->set("pruning_rate", 0.00005f);
    
    // Memory systems
    config->set("working_memory_capacity", 100);
    config->set("episodic_memory_capacity", 1000);
    
    // Development
    config->set("development_enabled", true);
    config->set("critical_period_duration", 300.0);
    
    // Neuromodulation
    config->set("dopamine_sensitivity", 1.0f);
    config->set("curiosity_threshold", 0.3f);
    
    // Performance
    config->set("use_memory_pool", true);
    config->set("use_soma_optimization", true);
    config->set("thread_count", 8);
}
```

### Advanced 2: Multi-Agent Simulation

```cpp
#include <vector>
#include "nlm/core/Config/Config.hpp"
#include "nlm/brain/Brain.hpp"
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/world/SimpleWorld.hpp"

int main() {
    std::cout << "=== Multi-Agent Competition ===" << std::endl;
    
    // Create shared world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(50, 50, 16, 16);
    world->reset();
    
    // Create multiple agents with different configurations
    std::vector<std::pair<std::string, std::shared_ptr<nlm::AgentBrain>>> agents;
    
    // Agent 1: Conservative (slow, steady learning)
    {
        auto config1 = std::make_shared<nlm::Config>();
        config1->set("neuron_count", 300);
        config1->set("connection_probability", 0.05f);
        auto brain1 = std::make_shared<nlm::Brain>(config1);
        auto agent1 = std::make_shared<nlm::AgentBrain>(brain1);
        brain1->initialize();
        agent1->initialize(*world);
        agent1->enableRewardModulation(true);
        agent1->enableDevelopment(true);
        agents.push_back({"Conservative", agent1});
    }
    
    // Agent 2: Aggressive (fast, exploratory)
    {
        auto config2 = std::make_shared<nlm::Config>();
        config2->set("neuron_count", 500);
        config2->set("connection_probability", 0.2f);
        auto brain2 = std::make_shared<nlm::Brain>(config2);
        auto agent2 = std::make_shared<nlm::AgentBrain>(brain2);
        brain2->initialize();
        agent2->initialize(*world);
        agent2->enableRewardModulation(true);
        agent2->enableCuriosity(true);
        agent2->enableStructuralPlasticity(true);
        agents.push_back({"Aggressive", agent2});
    }
    
    // Agent 3: Balanced (medium settings)
    {
        auto config3 = std::make_shared<nlm::Config>();
        config3->set("neuron_count", 400);
        config3->set("connection_probability", 0.15f);
        auto brain3 = std::make_shared<nlm::Brain>(config3);
        auto agent3 = std::make_shared<nlm::AgentBrain>(brain3);
        brain3->initialize();
        agent3->initialize(*world);
        agent3->enableRewardModulation(true);
        agent3->enableCuriosity(true);
        agent3->enableDevelopment(true);
        agents.push_back({"Balanced", agent3});
    }
    
    std::cout << "Starting multi-agent simulation..." << std::endl;
    
    // Competition loop
    for (int step = 0; step < 500; ++step) {
        world->update(0.1);
        
        // Each agent acts independently
        for (size_t i = 0; i < agents.size(); ++i) {
            auto& [name, agent] = agents[i];
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            agent->getBrain()->step(step, step * 0.1);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Individual learning
            float reward = world->getCurrentReward() * (1.0f + i * 0.1f); // Later agents get slight advantage
            agent->applyRewardModulation(reward, agent->getPredictionError());
            agent->updateDevelopment(0.1);
        }
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": World state updated" << std::endl;
        }
    }
    
    // Compare final performance
    std::cout << "\n=== Competition Results ===" << std::endl;
    for (auto& [name, agent] : agents) {
        auto& brain = agent->getBrain();
        std::cout << name << ": "
                  << "Spikes: " << brain->getTotalSpikeCount() << ", "
                  << "Synapses: " << brain->getTotalSynapseCount() << ", "
                  << "Curiosity: " << agent->getCuriosityLevel() << ", "
                  << "Development: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
    }
    
    std::cout << "\n✓ Multi-agent simulation completed!" << std::endl;
    return 0;
}
```

## Command Line Usage

### Basic Usage
```bash
# Run the basic demo
./nlm

# Run with custom configuration
./nlm --config configs/my_config.cfg

# Save configuration for later use
./nlm --save_config my_config.cfg
```

### Advanced Command Line Options

```bash
# Performance options
--thread_count N          Number of threads for parallel processing
--use_memory_pool         Enable memory pool optimization
--simd_type AVX2|SSE|OFF  SIMD instruction set to use

# Learning options
--enable_curiosity        Enable curiosity-driven exploration
--enable_development      Enable brain development stages
--plasticity_rate FLOAT   Base plasticity rate (0.0-2.0)

# Environment options
--world_width INT         World width in cells
--world_height INT        World height in cells
--vision_width INT        Agent vision width
--vision_height INT       Agent vision height

# Logging options
--log_level DEBUG|INFO|WARNING|ERROR
--log_file FILE           Log output to file in addition to console
--benchmark              Enable performance benchmarking

# Experiment options
--experiment_type EXP_TYPE  Type of experiment (plasticity, learning, etc.)
--experiment_steps INT      Number of steps for experiment
--experiment_save FILE      Save experiment results to file
```

## Configuration File Format

Configuration files use a simple key-value format:

```ini
# NLM Configuration File

[neural]
neuron_count = 1000
region_count = 2
connection_probability = 0.15

[plasticity]
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

[memory]
working_memory_capacity = 100
episodic_memory_capacity = 1000

[development]
enabled = true
critical_period_duration = 300.0

[neuromodulation]
dopamine_sensitivity = 1.0
curiosity_threshold = 0.3

[performance]
use_memory_pool = true
use_soma_optimization = true
thread_count = 4
```

## Performance Tips

1. **Memory Optimization**: Enable memory pools for large brains (>10,000 neurons)
2. **Parallel Processing**: Use multiple threads for simulation with `thread_count`
3. **SIMD Optimization**: Use AVX2 for best performance
4. **Configuration Files**: Save optimized configurations for reproducible experiments
5. **Incremental Saving**: Save brain state periodically during long simulations

## Troubleshooting

**"Brain not spiking"**
- Check that neurons are receiving current input
- Verify membrane potential and threshold parameters
- Ensure simulation timestep is appropriate

**"Memory issues with large brains"**
- Enable memory pool optimization
- Reduce connection probability
- Use `--use_memory_pool` and `--simd_type AVX2`

**"Performance too slow"**
- Enable parallel processing with `--thread_count`
- Use memory pool optimization
- Reduce neuron count or increase timestep

## Next Steps

1. Read `docs/ARCHITECTURE.md` for detailed technical information
2. Explore `docs/SCIENCE.md` for scientific background
3. Experiment with different configuration combinations
4. Look at `src/experiments/` for ready-made experiment implementations
5. Check `python/bindings.cpp` for Python interface if needed

## Quick Reference

| Feature | Function | Example |
|---------|----------|---------|
| Create brain | `Brain(config)` | `auto brain = std::make_shared<Brain>(config);` |
| Initialize | `brain->initialize()` | `brain->initialize();` |
| Simulate step | `brain->step(step, time)` | `brain->step(step, step * 0.001);` |
| Create agent | `AgentBrain(brain)` | `auto agent = std::make_shared<AgentBrain>(brain);` |
| Process input | `agent->processSensoryInput(percept)` | `agent->processSensoryInput(percept);` |
| Get action | `agent->decodeMotorCommand()` | `auto action = agent->decodeMotorCommand();` |
| Apply reward | `agent->applyRewardModulation(reward, predicted)` | `agent->applyRewardModulation(reward, predicted);` |
| Save state | `brain->save(filename)` | `brain->save("brain_state.nlm");` |

That's it! You're now ready to use NLM in C++. Start with the basic examples and gradually explore the advanced features.
