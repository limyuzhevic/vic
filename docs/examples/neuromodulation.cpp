/**
 * NLM Neuromodulation Example
 * 
 * This example demonstrates the neuromodulation system in NLM,
 * including dopamine, curiosity, and novelty detection mechanisms.
 * It shows how neuromodulators affect learning and behavior.
 */

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/NoveltyDetector.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>

using namespace nlm;

struct NeuromodulationStats {
    float dopamineLevel = 0.0f;
    float curiosityLevel = 0.0f;
    float noveltyLevel = 0.0f;
    float predictionError = 0.0f;
    int explorationCount = 0;
    float totalReward = 0.0f;
};

NeuromodulationStats getNeuromodulationStats(AgentBrain* agent, float& dopamine, float& curiosity, float& novelty) {
    NeuromodulationStats stats;
    stats.dopamineLevel = dopamine;
    stats.curiosityLevel = curiosity;
    stats.noveltyLevel = novelty;
    stats.predictionError = agent->getPredictionError();
    stats.explorationCount = 0; // Would be tracked in real implementation
    return stats;
}

int main() {
    std::cout << "NLM Neuromodulation Example" << std::endl;
    std::cout << "==========================\n";
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Warning);
    
    printSection("Setup");
    std::cout << "Configuring neuromodulation systems...\n";
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(800));
    config->set("region_count", size_t(1));
    config->set("enable_neuromodulation", true);
    config->set("enable_dopamine", true);
    config->set("enable_curiosity", true);
    config->set("enable_novelty_detection", true);
    
    printSection("Create Components");
    std::cout << "Setting up brain, world, and agent...\n";
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(15, 15, 8, 8);
    world->reset();
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(world.get());
    
    printSection("Enable Neuromodulators");
    std::cout << "Enabling neuromodulation systems...\n";
    
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableNoveltyDetection(true);
    agent->enableDopamineModulation(true);
    
    printSection("Run Neuromodulation Experiment");
    std::cout << "Demonstrating neuromodulation effects...\n";
    std::cout << std::string(80, '-') << "\n";
    
    int totalSteps = 300;
    std::unordered_map<int, float> stateHistory;
    
    for (int step = 0; step < totalSteps; ++step) {
        // Update world
        world->update(0.1f);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Check for novelty
        std::vector<float> sensoryFeatures;
        if (!percept.getInternal().empty()) {
            sensoryFeatures = percept.getInternal();
        }
        
        // Process through agent
        agent->processSensoryInput(percept);
        
        // Get neuromodulation levels
        float dopamine = agent->getNeuromodulationLevel(N NeuromodulatorType::Dopamine);
        float curiosity = agent->getCuriosityLevel();
        float novelty = agent->getNoveltyLevel();
        
        // Record state for novelty detection
        stateHistory[step] = novelty;
        
        // Run brain
        brain->step(step);
        
        // Get action
        auto action = agent->decodeMotorCommand();
        
        // Apply action
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward and modulation
        float reward = 0.0f;
        
        // Resource reward
        if (percept.getResources().size() > 0) {
            reward += 1.0f;
        }
        
        // Danger penalty
        if (percept.getHazards().size() > 0) {
            reward -= 0.5f;
        }
        
        agent->applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent->updateDevelopment(0.1f);
        
        // Print progress
        if (step % 50 == 0 || step == totalSteps - 1) {
            std::cout << "\nStep " << step << ":\n";
            std::cout << "  Neuromodulators - Dopamine: " << std::fixed << std::setprecision(3) << dopamine
                      << "  Curiosity: " << curiosity
                      << "  Novelty: " << novelty << "\n";
            std::cout << "  Prediction error: " << agent->getPredictionError() << "\n";
            std::cout << "  Reward: " << reward << "\n";
        }
    }
    
    printSection("Final Neuromodulation Statistics");
    std::cout << std::string(80, '-') << "\n";
    
    float finalDopamine = agent->getNeuromodulationLevel(N NeuromodulatorType::Dopamine);
    float finalCuriosity = agent->getCuriosityLevel();
    float finalNovelty = agent->getNoveltyLevel();
    float finalPredictionError = agent->getPredictionError();
    
    std::cout << "Neuromodulation system status:\n";
    std::cout << "  Final dopamine level: " << finalDopamine << "\n";
    std::cout << "  Final curiosity level: " << finalCuriosity << "\n";
    std::cout << "  Final novelty level: " << finalNovelty << "\n";
    std::cout << "  Final prediction error: " << finalPredictionError << "\n\n";
    
    // Analyze patterns
    std::cout << "Neuromodulation patterns:\n";
    
    int highNoveltySteps = 0;
    int highCuriositySteps = 0;
    int highDopamineSteps = 0;
    
    for (const auto& pair : stateHistory) {
        if (pair.second > 0.7f) highNoveltySteps++;
    }
    
    if (finalCuriosity > 0.7f) highCuriositySteps = 1;
    if (finalDopamine > 0.7f) highDopamineSteps = 1;
    
    std::cout << "  High novelty occurrences: " << highNoveltySteps << "\n";
    std::cout << "  High curiosity periods: " << highCuriositySteps << "\n";
    std::cout << "  High dopamine states: " << highDopamineSteps << "\n\n";
    
    printSection("Neuromodulation Summary");
    std::cout << "\nNeuromodulation mechanisms demonstrated:\n";
    std::cout << "  • Dopamine modulation of learning and motivation\n";
    std::cout << "  • Curiosity-driven exploration behavior\n";
    std::cout << "  • Novelty detection and adaptation\n";
    std::cout << "  • Prediction error signaling for learning\n";
    std::cout << "  • Reward prediction and prediction error\n";
    std::cout << "  • Development stage effects on neuromodulation\n";
    
    return 0;
}
