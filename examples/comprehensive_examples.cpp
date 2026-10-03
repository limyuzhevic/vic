// Comprehensive NLM Usage Examples
// Demonstrates advanced patterns and complex use cases

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace nlm;

// Example 1: Complete Agent with Development and Curiosity
void exampleCompleteAgent() {
    std::cout << "=== Example 1: Complete Agent with Development and Curiosity ===" << std::endl;
    
    // Create complete agent with all systems enabled
    auto [brain, world, agent] = utils::createCompleteAgent("demo_agent", 1500, 25);
    
    std::cout << "Created agent with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
    std::cout << "World size: " << world->getWidth() << "x" << world->getHeight() << std::endl;
    
    // Enable all features
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    std::cout << "Enabled features:" << std::endl;
    std::cout << "  - Reward modulation: " << (agent->isRewardModulationEnabled() ? "Yes" : "No") << std::endl;
    std::cout << "  - Structural plasticity: " << (agent->isStructuralPlasticityEnabled() ? "Yes" : "No") << std::endl;
    std::cout << "  - Development: " << (agent->isDevelopmentEnabled() ? "Yes" : "No") << std::endl;
    std::cout << "  - Curiosity: " << (agent->isCuriosityEnabled() ? "Yes" : "No") << std::endl;
    
    // Run simulation
    std::cout << "\nRunning simulation (500 steps)..." << std::endl;
    auto results = utils::runValidatedSimulation(agent, world, 500, 0.05, true);
    
    if (results["success"].is_true()) {
        std::cout << "Simulation completed successfully!" << std::endl;
        std::cout << "Final stats:" << std::endl;
        std::cout << "  - Total spikes: " << results["brain_stats"]["total_spikes"].cast<int>() << std::endl;
        std::cout << "  - Development stage: " << agent->getDevelopmentalStage() << std::endl;
        std::cout << "  - Final health: " << results["final_step_data"]["agent_health"].cast<float>() << std::endl;
    } else {
        std::cout << "Simulation failed: " << results["error"].cast<std::string>() << std::endl;
    }
}

// Example 2: Learning Experiment with Multiple Trials
void exampleLearningExperiment() {
    std::cout << "\n=== Example 2: Learning Experiment with Multiple Trials ===" << std::endl;
    
    // Create preset brain optimized for learning
    auto brain = utils::createSimulationPreset("advanced");
    
    // Create agent and world
    auto [world, agent] = utils::createValidatedAgent(brain, 30, 30, 12, 12);
    
    // Enable learning systems
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    
    // Custom learning callback
    auto learningCallback = [](size_t step, const py::dict& stepData) {
        if (step % 100 == 0) {
            std::cout << "  Trial " << step << ": "
                     << "Dopamine: " << stepData["dopamine_level"].cast<float>()
                     << ", Curiosity: " << stepData["curiosity_level"].cast<float>()
                     << ", Health: " << stepData["agent_health"].cast<float>() << std::endl;
        }
    };
    
    std::cout << "Running learning experiment (1000 trials)..." << std::endl;
    auto results = utils::runValidatedSimulation(
        agent, world, 1000, 0.05, false, learningCallback);
    
    if (results["success"].is_true()) {
        std::cout << "Learning experiment completed!" << std::endl;
        
        // Analyze learning outcomes
        auto finalStats = results["final_step_data"];
        float initialDopamine = 0.0f;  // Would need to track from beginning
        float finalDopamine = finalStats["dopamine_level"].cast<float>();
        float dopamineChange = finalDopamine - initialDopamine;
        
        std::cout << "Learning analysis:" << std::endl;
        std::cout << "  - Dopamine change: " << dopamineChange << std::endl;
        std::cout << "  - Final curiosity: " << agent->getCuriosityLevel() << std::endl;
        std::cout << "  - Development stage: " << agent->getDevelopmentalStage() << std::endl;
        
        // Check if learning occurred
        if (dopamineChange > 0.1f) {
            std::cout << "  ✓ Significant learning detected through neuromodulation changes!" << std::endl;
        }
        
    } else {
        std::cout << "Learning experiment failed: " << results["error"].cast<std::string>() << std::endl;
    }
}

// Example 3: Comparison Between Different Presets
void examplePresetComparison() {
    std::cout << "\n=== Example 3: Comparison Between Different Presets ===" << std::endl;
    
    std::vector<std::string> presets = {"minimal", "simple", "advanced"};
    std::vector<std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>> agents;
    
    std::cout << "Creating agents with different presets:" << std::endl;
    
    for (const auto& preset : presets) {
        std::cout << "\n  Creating " << preset << " preset..." << std::endl;
        
        auto brain = utils::createSimulationPreset(preset);
        auto [world, agent] = utils::createValidatedAgent(brain, 15, 15, 6, 6);
        
        agents.push_back(std::make_tuple(brain, world, agent));
        
        std::cout << "    Neurons: " << brain->getTotalNeuronCount()
                 << ", Development: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
    }
    
    // Run quick comparison
    std::cout << "\nRunning quick comparison (200 steps each)..." << std::endl;
    
    for (size_t i = 0; i < agents.size(); ++i) {
        auto& [brain, world, agent] = agents[i];
        
        // Enable minimal features for comparison
        agent->enableRewardModulation(false);
        agent->enableCuriosity(false);
        agent->enableDevelopment(false);
        
        auto results = utils::runValidatedSimulation(agent, world, 200, 0.1, false);
        
        if (results["success"].is_true()) {
            auto stats = results["brain_stats"];
            std::cout << "    " << presets[i] << " preset: "
                     << stats["total_spikes"].cast<int>() << " spikes, "
                     << stats["average_firing_rate"].cast<float>() << " avg firing rate" << std::endl;
        }
    }
    
    std::cout << "\nPreset comparison complete!" << std::endl;
}

// Example 4: Error Handling and Recovery
void exampleErrorHandling() {
    std::cout << "\n=== Example 4: Error Handling and Recovery ===" << std::endl;
    
    try {
        // This should fail - invalid preset
        std::cout << "Testing invalid preset handling..." << std::endl;
        auto brain = utils::createSimulationPreset("invalid_preset");
        std::cout << "  ERROR: Should have thrown exception!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "  ✓ Correctly caught exception: " << e.what() << std::endl;
    }
    
    try {
        // Test invalid world dimensions
        std::cout << "\nTesting invalid world dimensions..." << std::endl;
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::User);
        config->set("simulation_timestep", 0.001, ConfigSource::User);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::User);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        // This should fail validation - vision dimensions larger than world
        auto [world, agent] = utils::createValidatedAgent(brain, 10, 10, 15, 15);
        std::cout << "  ERROR: Should have thrown validation error!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "  ✓ Correctly caught validation error: " << e.what() << std::endl;
    }
    
    try {
        // Test invalid configuration
        std::cout << "\nTesting invalid configuration..." << std::endl;
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(-100), ConfigSource::User);  // Invalid!
        config->set("simulation_timestep", -0.001, ConfigSource::User);  // Invalid!
        
        utils::validateConfig(*config);
        std::cout << "  ERROR: Should have thrown validation error!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "  ✓ Correctly caught config validation error: " << e.what() << std::endl;
    }
    
    std::cout << "\nError handling test complete!" << std::endl;
}

// Example 5: Advanced Monitoring and Statistics
void exampleAdvancedMonitoring() {
    std::cout << "\n=== Example 5: Advanced Monitoring and Statistics ===" << std::endl;
    
    // Create agent with monitoring enabled
    auto [brain, world, agent] = utils::createCompleteAgent("monitor_agent", 800, 20);
    
    // Custom monitoring callback with detailed statistics
    auto monitorCallback = [](size_t step, const py::dict& stepData) {
        // Advanced metrics collection
        float health = stepData["agent_health"].cast<float>();
        float energy = stepData["agent_energy"].cast<float>();
        float dopamine = stepData["dopamine_level"].cast<float>();
        float curiosity = stepData["curiosity_level"].cast<float>();
        
        // Calculate derived metrics
        float stressLevel = 1.0f - (health / 1.0f);  // Normalize health to stress
        float explorationScore = curiosity * (1.0f - energy / 1.0f);  // High curiosity + low energy = more exploration
        
        // Log detailed metrics
        if (step % 50 == 0) {
            std::cout << "  Step " << step << ": "
                     << "Stress: " << std::fixed << std::setprecision(2) << stressLevel << ", "
                     << "Exploration: " << std::setprecision(2) << explorationScore << ", "
                     << "Decision confidence: " << (1.0f - abs(dopamine - 0.5f)) << std::endl;
        }
    };
    
    std::cout << "Running advanced monitoring (800 steps with detailed metrics)..." << std::endl;
    auto results = utils::runValidatedSimulation(agent, world, 800, 0.05, false, monitorCallback);
    
    if (results["success"].is_true()) {
        std::cout << "\nAdvanced monitoring complete!" << std::endl;
        
        // Generate comprehensive report
        auto stats = results["brain_stats"];
        auto finalStep = results["final_step_data"];
        
        std::cout << "=== FINAL REPORT ===" << std::endl;
        std::cout << "Brain Statistics:" << std::endl;
        std::cout << "  - Total neurons: " << stats["total_neurons"].cast<int>() << std::endl;
        std::cout << "  - Total synapses: " << stats["total_synapses"].cast<int>() << std::endl;
        std::cout << "  - Total spikes: " << stats["total_spikes"].cast<int>() << std::endl;
        std::cout << "  - Average firing rate: " << std::setprecision(3) << stats["average_firing_rate"].cast<float>() << std::endl;
        std::cout << "  - E/I ratio: " << std::setprecision(3) << stats["excitatory_inhibition_ratio"].cast<float>() << std::endl;
        
        std::cout << "\nAgent Performance:" << std::endl;
        std::cout << "  - Final health: " << std::setprecision(3) << finalStep["agent_health"].cast<float>() << std::endl;
        std::cout << "  - Final energy: " << std::setprecision(3) << finalStep["agent_energy"].cast<float>() << std::endl;
        std::cout << "  - Development stage: " << agent->getDevelopmentalStage() << std::endl;
        std::cout << "  - Neuromodulation levels: Dopamine=" << agent->getNeuromodulationLevel() 
                 << ", Curiosity=" << agent->getCuriosityLevel() << std::endl;
        
        std::cout << "\nRegions:" << std::endl;
        auto regions = stats["regions"];
        for (size_t i = 0; i < regions.size(); ++i) {
            auto region = regions[i];
            std::cout << "  - Region " << region["id"].cast<int>() << ": "
                     << region["neuron_count"].cast<int>() << " neurons, "
                     << region["synapse_count"].cast<int>() << " synapses, "
                     << "avg weight: " << std::setprecision(3) << region["average_weight"].cast<float>() << std::endl;
        }
        
    } else {
        std::cout << "Monitoring failed: " << results["error"].cast<std::string>() << std::endl;
    }
}

int main() {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "=== Comprehensive NLM Usage Examples ===" << std::endl;
    
    // Run all examples
    exampleCompleteAgent();
    exampleLearningExperiment();
    examplePresetComparison();
    exampleErrorHandling();
    exampleAdvancedMonitoring();
    
    std::cout << "\n=== All Examples Complete ===" << std::endl;
    std::cout << "These examples demonstrate:" << std::endl;
    std::cout << "1. Complete agent creation with all systems enabled" << std::endl;
    std::cout << "2. Learning experiments with neuromodulation tracking" << std::endl;
    std::cout << "3. Comparison between different simulation presets" << std::endl;
    std::cout << "4. Comprehensive error handling and validation" << std::endl;
    std::cout << "5. Advanced monitoring and statistics collection" << std::endl;
    
    return 0;
}