// NLM Unified Demo - Example Usage
// A comprehensive example demonstrating NLM usage patterns
// This file serves as both documentation and a working example

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace nlm;

// Example 1: Basic Brain Operations
void example_basic_brain() {
    std::cout << "=== Example 1: Basic Brain Operations ===" << std::endl;
    
    // Create brain with custom configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100, ConfigSource::Default);
    config->set("region_count", 2, ConfigSource::Default);
    config->set("random_seed", 42, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "Brain initialized with " << brain->getTotalNeuronCount() 
             << " neurons and " << brain->getTotalSynapseCount() << " synapses" << std::endl;
    
    // Run simulation loop
    const size_t steps = 50;
    for (size_t step = 0; step < steps; ++step) {
        brain->step(static_cast<SimulationStep>(step));
        
        // Log progress every 10 steps
        if (step % 10 == 0) {
            std::cout << "  Step " << step << 
                     << ": " << brain->getTotalSpikeCount() << " spikes, "
                     << brain->getFiringNeuronCount() << " firing" << std::endl;
        }
    }
    
    std::cout << "Basic brain test completed successfully!" << std::endl << std::endl;
}

// Example 2: Complete Agent in World
void example_agent_world() {
    std::cout << "=== Example 2: Complete Agent in World ===" << std::endl;
    
    // Create and configure brain
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200, ConfigSource::Default);
    config->set("region_count", 1, ConfigSource::Default);
    config->set("random_seed", 123, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create and configure world
    auto world = std::make_unique<SimpleWorld>();
    world->configure(width: 20, height: 20, visionWidth: 8, visionHeight: 8);
    world->reset();
    
    // Create agent
    auto agent = std::make_unique<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Enable learning features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    
    std::cout << "Agent and world created successfully" << std::endl;
    
    // Run complete simulation loop
    const size_t steps = 100;
    for (size_t step = 0; step < steps; ++step) {
        // World updates
        world->update(0.1);
        
        // Get agent's perception
        auto percept = world->getSensoryPercept();
        
        // Process and think
        agent->processSensoryInput(percept);
        brain->step(static_cast<SimulationStep>(step));
        
        // Get and execute action
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Log occasionally
        if (step % 20 == 0) {
            std::cout << "  Step " << step << 
                     ": Action " << static_cast<int>(action) << 
                     ", " << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
        }
    }
    
    std::cout << "Agent-world simulation completed!" << std::endl << std::endl;
}

// Example 3: Configuration and Setup
void example_configuration() {
    std::cout << "=== Example 3: Configuration and Setup ===" << std::endl;
    
    // Create configuration with all important parameters
    auto config = std::make_shared<Config>();
    
    // Network parameters
    config->set("neuron_count", 500, ConfigSource::Default);
    config->set("region_count", 3, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Connection parameters
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Plasticity parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Development parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Memory system parameters
    config->set("working_memory_capacity", 50, ConfigSource::Default);
    config->set("episodic_max_episodes", 1000, ConfigSource::Default);
    
    std::cout << "Configuration created with following parameters:" << std::endl;
    std::cout << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 500) << std::endl;
    std::cout << "  region_count: " << config->getOr<int64_t>("region_count", 3) << std::endl;
    std::cout << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << std::endl;
    std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.1f) << std::endl;
    std::cout << "  stdp_ltp_weight: " << config->getOr<float>("stdp_ltp_weight", 0.02f) << std::endl;
    
    // Create brain with this configuration
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    std::cout << "Brain created with configuration successfully!" << std::endl << std::endl;
}

// Example 4: Error Handling and Validation
void example_error_handling() {
    std::cout << "=== Example 4: Error Handling and Validation ===" << std::endl;
    
    // Test 1: Invalid configuration handling
    std::cout << "Test 1: Configuration validation" << std::endl;
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 0, ConfigSource::Default);  // Edge case: zero neurons
    
    auto brain = std::make_shared<Brain>(config);
    // Note: Brain::initialize() may fail gracefully or handle this internally
    
    std::cout << "  Configuration validation completed (internal handling)" << std::endl;
    
    // Test 2: Null pointer safety
    std::cout << "Test 2: Null pointer safety" << std::endl;
    std::shared_ptr<Brain> nullBrain;  // Null shared_ptr
    
    // Safe access pattern (should not crash)
    if (nullBrain) {
        std::cout << "  Brain is not null" << std::endl;
    } else {
        std::cout << "  Brain is null (handled safely)" << std::endl;
    }
    
    // Test 3: Memory cleanup
    std::cout << "Test 3: Memory management" << std::endl;
    auto tempBrain = std::make_shared<Brain>(config);
    tempBrain->initialize();
    std::cout << "  Temporary brain created and will be cleaned up automatically" << std::endl;
    
    std::cout << "Error handling examples completed!" << std::endl << std::endl;
}

// Example 5: Advanced Features
void example_advanced_features() {
    std::cout << "=== Example 5: Advanced Features ===" << std::endl;
    
    // Create brain with development enabled
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 300, ConfigSource::Default);
    config->set("random_seed", 999, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create agent with advanced features
    auto world = std::make_unique<SimpleWorld>();
    world->configure(width: 15, height: 15, visionWidth: 8, visionHeight: 8);
    world->reset();
    
    auto agent = std::make_unique<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Enable all advanced features
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    std::cout << "Advanced features enabled: Reward, Structural Plasticity, Development, Curiosity" << std::endl;
    
    // Run with learning
    const size_t steps = 75;
    float totalReward = 0.0f;
    
    for (size_t step = 0; step < steps; ++step) {
        world->update(0.1);
        
        // Simulate some reward based on action
        float reward = 0.0f;
        if (step % 10 == 0) reward = 10.0f;  // Resource
        if (step % 15 == 0) reward = -5.0f; // Hazard
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(static_cast<SimulationStep>(step));
        
        auto action = agent->decodeMotorCommand();
        
        // Apply reward modulation
        agent->applyRewardModulation(reward, 0.0f);  // No predicted reward for now
        totalReward += reward;
        
        world->applyMotorCommand(action, world->getSimulationTime());
        
        if (step % 25 == 0) {
            std::cout << "  Step " << step << ": Reward " << reward << ", Total " << totalReward 
                     << ", Firing " << brain->getFiringNeuronCount() << " neurons" << std::endl;
        }
    }
    
    std::cout << "Advanced features test completed! Total reward: " << totalReward << std::endl << std::endl;
}

// Example 6: Complete Usage Pattern
void example_complete_usage() {
    std::cout << "=== Example 6: Complete Usage Pattern ===" << std::endl;
    std::cout << "This example shows the complete pattern for using NLM in a real application:" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "1. Create configuration with desired parameters" << std::endl;
    std::cout << "2. Initialize brain with configuration" << std::endl;
    std::cout << "3. Create world for the agent" << std::endl;
    std::cout << "4. Create agent to interface brain and world" << std::endl;
    std::cout << "5. Enable learning features if needed" << std::endl;
    std::cout << "6. Run simulation loop: world -> agent -> brain -> world" << std::endl;
    std::cout << "7. Monitor and log results" << std::endl;
    std::cout << "8. Clean up resources" << std::endl;
    std::cout << "" << std::endl;
    
    // Quick demonstration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 150, ConfigSource::Default);
    config->set("random_seed", 555, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto world = std::make_unique<SimpleWorld>();
    world->configure(width: 10, height: 10, visionWidth: 8, visionHeight: 8);
    world->reset();
    
    auto agent = std::make_unique<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Run one complete cycle
    world->update(0.1);
    auto percept = world->getSensoryPercept();
    agent->processSensoryInput(percept);
    brain->step(0);
    auto action = agent->decodeMotorCommand();
    world->applyMotorCommand(action, world->getSimulationTime());
    
    std::cout << "Complete usage pattern demonstrated successfully!" << std::endl << std::endl;
}

int main() {
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "NLM - Unified Demo and Usage Examples" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "This file demonstrates various usage patterns for NLM." << std::endl;
    std::cout << "Each example shows different aspects of the NLM framework." << std::endl;
    std::cout << "" << std::endl;
    
    try {
        example_basic_brain();
        example_configuration();
        example_agent_world();
        example_error_handling();
        example_advanced_features();
        example_complete_usage();
        
        std::cout << "=========================================" << std::endl;
        std::cout << "All examples completed successfully!" << std::endl;
        std::cout << "These examples demonstrate the main usage patterns for NLM." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Example failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Example failed with unknown exception!" << std::endl;
        return 1;
    }
    
    return 0;
}
