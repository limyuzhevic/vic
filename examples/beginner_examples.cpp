// NLM C++ Beginner Examples
// This file contains beginner-friendly C++ examples for NLM
// Each example builds on the previous one, teaching core concepts
// 
// To compile these examples, run:
// mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

// Example 1: The Simplest Brain
// ------------------------------------------------------------
// This example shows the absolute minimum code to create and run a brain
void example_1_simplest_brain() {
    std::cout << "=== Example 1: The Simplest Brain ===" << std::endl;
    
    // Step 1: Create a default configuration
    auto config = std::make_shared<nlm::Config>();
    std::cout << "Created default configuration" << std::endl;
    
    // Step 2: Create the brain
    auto brain = std::make_shared<nlm::Brain>(config);
    std::cout << "Created brain" << std::endl;
    
    // Step 3: Initialize the brain
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    std::cout << "Initialized brain" << std::endl;
    
    // Step 4: Run the brain for a few steps
    std::cout << "Running brain simulation..." << std::endl;
    for (int step = 0; step < 10; ++step) {
        brain->step(step);
        if (step % 5 == 0) {
            std::cout << "  Step " << step << ": " 
                      << brain->getFiringNeuronCount() << " neurons firing" << std::endl;
        }
    }
    
    std::cout << "Example 1 complete! Brain had " 
              << brain->getTotalSpikeCount() << " total spikes" << std::endl;
    std::cout << std::endl;
}

// Example 2: Brain with Configuration
// ------------------------------------------------------------
// This example shows how to configure a brain with custom parameters
void example_2_brain_configuration() {
    std::cout << "=== Example 2: Brain with Custom Configuration ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Set some basic configuration parameters
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(12345), nlm::ConfigSource::Default);
    
    std::cout << "Configured brain with:" << std::endl;
    std::cout << "  - " << config->getOr<int64_t>("neuron_count", 1000) << " neurons" << std::endl;
    std::cout << "  - " << config->getOr<int64_t>("region_count", 1) << " regions" << std::endl;
    std::cout << "  - " << config->getOr<float>("connection_probability", 0.1f) << " connection probability" << std::endl;
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Show initial state
    std::cout << "Initial brain status:" << std::endl;
    brain->logStatus();
    std::cout << std::endl;
    
    // Run simulation
    std::cout << "Running 50 steps of simulation..." << std::endl;
    for (int step = 0; step < 50; ++step) {
        brain->step(step);
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
        
        if (step % 10 == 0) {
            std::cout << "  Step " << step << ": "
                      << brain->getFiringNeuronCount() << " firing, "
                      << brain->getTotalSpikeCount() << " total spikes" << std::endl;
        }
    }
    
    std::cout << "Example 2 complete!" << std::endl;
    std::cout << std::endl;
}

// Example 3: Brain Learning (STDP)
// ------------------------------------------------------------
// This example demonstrates STDP learning (neurons that fire together learn to connect)
void example_3_stdp_learning() {
    std::cout << "=== Example 3: STDP Learning Demonstration ===" << std::endl;
    
    // Create configuration for learning
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2f, nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, nlm::ConfigSource::Default);  // Learning rate
    config->set("stdp_ltd_weight", 0.018f, nlm::ConfigSource::Default); // Forgetting rate
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    std::cout << "Initial STDP weights: LTP = " << config->getOr<float>("stdp_ltp_weight", 0.01f)
              << ", LTD = " << config->getOr<float>("stdp_ltd_weight", 0.012f) << std::endl;
    std::cout << std::endl;
    
    // Record initial average weight from first region
    float initialAverageWeight = 0.0f;
    if (auto* region = brain->getRegion(nlm::RegionId(1))) {
        if (!region->getSynapses().empty()) {
            float sum = 0.0f;
            for (const auto& syn : region->getSynapses()) {
                sum += syn->getWeight();
            }
            initialAverageWeight = sum / region->getSynapses().size();
        }
    }
    
    std::cout << "Initial average synaptic weight: " << initialAverageWeight << std::endl;
    std::cout << "Running learning simulation..." << std::endl;
    
    // Run learning simulation
    for (int step = 0; step < 100; ++step) {
        // Inject sensory input to trigger learning
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 30.0f);
        
        // Inject current to some neurons to create correlated activity
        if (step % 10 == 0) {
            brain->injectCurrentToNeurons(nlm::NeuronType::Internal, 15.0f);
        }
        
        brain->step(step);
        
        if (step % 20 == 0) {
            std::cout << "  Step " << step << ": "
                      << brain->getFiringNeuronCount() << " firing, "
                      << brain->getTotalSpikeCount() << " spikes total" << std::endl;
        }
    }
    
    // Record final average weight
    float finalAverageWeight = 0.0f;
    if (auto* region = brain->getRegion(nlm::RegionId(1))) {
        if (!region->getSynapses().empty()) {
            float sum = 0.0f;
            for (const auto& syn : region->getSynapses()) {
                sum += syn->getWeight();
            }
            finalAverageWeight = sum / region->getSynapses().size();
        }
    }
    
    std::cout << "Final average synaptic weight: " << finalAverageWeight << std::endl;
    std::cout << "Weight change: " << (finalAverageWeight - initialAverageWeight) << std::endl;
    std::cout << "Example 3 complete! Learning simulation finished." << std::endl;
    std::cout << std::endl;
}

// Example 4: Brain in a World
// ------------------------------------------------------------
// This example shows how a brain interacts with a simulated world
void example_4_brain_in_world() {
    std::cout << "=== Example 4: Brain in a Simple World ===" << std::endl;
    
    // Setup the brain
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Setup the world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);  // 20x20 world, 8x8 vision
    world->reset();
    world->setAgentStart(10, 10);   // Start agent at position (10, 10)
    
    // Create agent (brain + world interface)
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(*world);
    
    std::cout << "Created environment:" << std::endl;
    std::cout << "  - World size: " << world->getWidth() << "x" << world->getHeight() << std::endl;
    std::cout << "  - Vision: " << world->getSensoryPercept().getVisionWidth() << "x" 
              << world->getSensoryPercept().getVisionHeight() << std::endl;
    std::cout << std::endl;
    
    // Run simulation loop
    std::cout << "Running simulation with agent..." << std::endl;
    for (int step = 0; step < 50; ++step) {
        // Update world
        world->update(0.1);  // 100ms timestep
        
        // Get what the agent sees (vision + other senses)
        auto percept = world->getSensoryPercept();
        
        // Tell the brain what it sees
        agent->processSensoryInput(percept);
        
        // Brain thinks
        brain->step(step);
        
        // Brain decides what to do
        auto action = agent->decodeMotorCommand();
        
        // Do the action in the world
        auto result = world->applyMotorCommand(action, world->getSimulationTime());
        
        // Log step info every 10 steps
        if (step % 10 == 0) {
            std::cout << "  Step " << step << ": "
                      << brain->getFiringNeuronCount() << " neurons firing, "
                      << "action type = " << static_cast<int>(action.getType()) << "\n"
                      << "    Reward: " << result.reward << "\n"
                      << "    Success: " << (result.success ? "Yes" : "No") << "\n"
                      << "    Message: " << result.message << std::endl;
        }
    }
    
    std::cout << "Example 4 complete! Simulation finished." << std::endl;
    std::cout << std::endl;
}

// Example 5: Complete Learning Agent
// ------------------------------------------------------------
// This example shows a complete agent that learns from its experiences
void example_5_complete_learning_agent() {
    std::cout << "=== Example 5: Complete Learning Agent ===" << std::endl;
    
    // Setup brain with learning capabilities
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("environment_name", std::string("GridWorld"), nlm::ConfigSource::Default);
    config->set("environment_width", 15, nlm::ConfigSource::Default);
    config->set("environment_height", 15, nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Setup world and agent
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(15, 15, 6, 6);
    world->reset();
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(*world);
    
    // Enable learning features
    agent->enableRewardModulation(true);        // Learn from rewards
    agent->enableStructuralPlasticity(true);    // Grow new connections
    agent->enableDevelopment(true);             // Brain matures over time
    agent->enableCuriosity(true);               // Explore new things
    
    std::cout << "Learning agent setup complete:" << std::endl;
    std::cout << "  - Reward modulation: " << (agent->isRewardModulationEnabled() ? "ON" : "OFF") << std::endl;
    std::cout << "  - Structural plasticity: " << (agent->isStructuralPlasticityEnabled() ? "ON" : "OFF") << std::endl;
    std::cout << "  - Development: " << (agent->isDevelopmentEnabled() ? "ON" : "OFF") << std::endl;
    std::cout << "  - Curiosity: " << (agent->isCuriosityEnabled() ? "ON" : "OFF") << std::endl;
    std::cout << std::endl;
    
    // Run learning simulation
    std::cout << "Starting learning simulation..." << std::endl;
    float cumulativeReward = 0.0f;
    int episode = 0;
    
    for (int step = 0; step < 200; ++step) {
        // Update world
        world->update(0.1);
        
        // Get perception
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain processes and learns
        brain->step(step);
        
        // Choose and execute action
        auto action = agent->decodeMotorCommand();
        auto result = world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply learning
        agent->applyRewardModulation(result.reward, 0.0f);  // Predicted reward = 0 (basic)
        
        // Update development (brain matures)
        agent->updateDevelopment(0.1);
        
        cumulativeReward += result.reward;
        
        // Log episode every 50 steps
        if (step % 50 == 0 && step > 0) {
            episode++;
            std::cout << "=== Episode " << episode << " ===" << std::endl;
            std::cout << "  Steps: " << step << "/200" << std::endl;
            std::cout << "  Cumulative reward: " << cumulativeReward << std::endl;
            std::cout << "  Current firing rate: " << brain->getAverageFiringRate() << std::endl;
            std::cout << "  Novelty level: " << agent->getNoveltyLevel() << std dojem("    Curiosity level: %f", agent->getCuriosityLevel() << std::endl;
            std::cout << "  Dopamine level: " << agent->getNeuromodulationLevel() << std::endl;
            std::cout << std::endl;
        }
    }
    
    std::cout << "Learning simulation complete!" << std::endl;
    std::cout << "Final statistics:" << std::endl;
    std::cout << "  - Total episodes: " << episode << std::endl;
    std::cout << "  - Total reward: " << cumulativeReward << std::endl;
    std::cout << "  - Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  - Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "Example 5 complete!" << std::endl;
    std::cout << std::endl;
}

// Example 6: Configuration and Validation
// ------------------------------------------------------------
// This example shows how to use configuration validation and error handling
void example_6_configuration_validation() {
    std::cout << "=== Example 6: Configuration Validation ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Add some valid configurations
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    // Try to add an invalid configuration (this should throw an exception)
    std::cout << "Adding valid configurations..." << std::endl;
    try {
        config->set("neuron_count", static_cast<int64_t>(-10), nlm::ConfigSource::Default);
        std::cout << "ERROR: Should have thrown exception for negative neuron count!" << std::endl;
    } catch (const nlm::ConfigValidationError& e) {
        std::cout << "Good: Caught expected exception: " << e.what() << std::endl;
    }
    
    // Add more valid configurations
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, nlm::ConfigSource::Default);
    
    // Show current configuration
    std::cout << "\nCurrent configuration:" << std::endl;
    std::cout << config->summary() << std::endl;
    
    // Validate configuration
    std::cout << "Validating configuration..." << std::endl;
    if (config->validate()) {
        std::cout << "✓ Configuration is valid!" << std::endl;
    } else {
        std::cout << "✗ Configuration has validation errors:" << std::endl;
        for (const auto& error : config->getValidationErrors()) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    // Show available schema keys
    std::cout << "\nAvailable configuration keys in schema:" << std::endl;
    // In a real implementation, we would access the schema through the Config class
    // For this example, we'll show some common keys
    std::vector<std::string> commonKeys = {
        "random_seed", "simulation_timestep", "neuron_count", "region_count",
        "connection_probability", "stdp_ltp_weight", "stdp_ltd_weight"
    };
    
    for (const auto& key : commonKeys) {
        std::cout << "  - " << key << std::endl;
    }
    
    std::cout << "Example 6 complete! Configuration validation demonstrated." << std::endl;
    std::cout << std::endl;
}

// Main function to run all examples
int main() {
    std::cout << "=============================================" << std::endl;
    std::cout << "NLM C++ Beginner Examples" << std::endl;
    std::cout << "This demonstrates NLM with step-by-step examples" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << std::endl;
    
    // Run examples in order
    example_1_simplest_brain();
    example_2_brain_configuration();
    example_3_stdp_learning();
    example_4_brain_in_world();
    example_5_complete_learning_agent();
    example_6_configuration_validation();
    
    std::cout << "=============================================" << std::endl;
    std::cout << "All examples completed successfully!" << std::endl;
    std::cout << "These examples demonstrate the core NLM concepts:" << std::endl;
    std::cout << "  - Creating and initializing brains" << std::endl;
    std::cout << "  - Using configuration files" << std::endl;
    std::cout << "  - STDP learning mechanisms" << std::endl;
    std::cout << "  - Brain-world interaction" << std::endl;
    std::cout << "  - Learning agents with neuromodulation" << std::endl;
    std::cout << "  - Configuration validation and error handling" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    return 0;
}
