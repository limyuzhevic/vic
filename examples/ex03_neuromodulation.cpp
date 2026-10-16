// ex03_neuromodulation.cpp - Neuromodulation and Learning

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

int main() {
    // Initialize logging
    nlm::Logger::getInstance().setLevel(nlm::LogLevel::INFO);
    
    std::cout << "=== Example 3: Neuromodulation and Learning ===" << std::endl;
    
    // Create configuration with neuromodulation
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 150);
    config->set("brain.neuromodulation.dopamine.enabled", true);
    config->set("brain.neuromodulation.dopamine.baselineLevel", 0.1f);
    config->set("brain.neuromodulation.dopamine.gain", 5.0f);
    config->set("brain.neuromodulation.curiosity.enabled", true);
    config->set("brain.neuromodulation.curiosity.baselineLevel", 0.1f);
    config->set("brain.neuromodulation.curiosity.noveltyGain", 3.0f);
    config->set("brain.neuromodulation.novelty.enabled", true);
    config->set("brain.neuromodulation.novelty.decayRate", 0.1f);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Neuromodulation brain initialized!" << std::endl;
    
    // Get neuromodulation systems
    auto dopamine = brain->getDopamine();
    auto curiosity = brain->getCuriosity();
    auto novelty = brain->getNovelty();
    
    std::cout << "\n--- Neuromodulation Simulation ---" << std::endl;
    
    // Simulation loop
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        double currentTime = step * 0.01; // 10ms steps
        
        // Generate simulated sensory input with novelty
        std::vector<float> sensoryInput;
        for (int i = 0; i < 50; ++i) {
            // Create interesting sensory patterns
            float value = 0.5f + 0.5f * std::sin(currentTime * 2.0f + i * 0.1f);
            
            // Add occasional novel inputs
            if (step % 50 == 0 && i % 7 == 0) {
                value = 1.0f - value; // Novel pattern
            }
            
            sensoryInput.push_back(value);
        }
        
        // Inject current into sensory neurons
        brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 2.0f);
        
        // Step brain
        brain->step(step);
        
        // Monitor neuromodulation levels
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": "
                     << "Dopamine: " << dopamine->getLevel() << ", "
                     << "Curiosity: " << curiosity->getLevel() << ", "
                     << "Novelty: " << novelty->getLevel() << std::endl;
        }
        
        // Apply reward modulation at key points
        if (step % 40 == 0) {
            float reward = (step % 80 == 0) ? 1.0f : 0.5f; // Varying reward
            float predictionError = 0.0f; // Simplified for demo
            
            // Apply neuromodulation based on reward
            auto agent = std::make_shared<nlm::AgentBrain>(brain);
            agent->applyRewardModulation(reward, predictionError);
            
            std::cout << "  Applied reward: " << reward << " (Prediction error: " 
                     << predictionError << ")" << std::endl;
        }
        
        // Update novelty detection
        novelty->update(0.01);
        
        // Update curiosity based on novelty
        curiosity->update(0.01);
        
        // Update dopamine based on prediction error
        dopamine->update(0.01);
        
        // Small delay for visualization
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Demonstrate curiosity-driven exploration
    std::cout << "\n--- Curiosity-Driven Exploration ---" << std::endl;
    auto curiosityLevel = curiosity->getLevel();
    std::cout << "Final curiosity level: " << curiosityLevel << std::endl;
    
    if (curiosityLevel > 0.5f) {
        std::cout << "High curiosity detected - increased exploratory behavior!" << std::endl;
    }
    
    // Demonstrate novelty processing
    std::cout << "\n--- Novelty Detection ---" << std::endl;
    float finalNovelty = novelty->getLevel();
    std::cout << "Final novelty level: " << finalNovelty << std::endl;
    
    if (finalNovelty > 0.3f) {
        std::cout << "Novelty above threshold - triggers exploration and learning!" << std::endl;
    }
    
    // Demonstrate prediction error
    std::cout << "\n--- Prediction Error Processing ---" << std::endl;
    auto predictionErrorSignal = brain->getPredictionErrorSignal();
    if (predictionErrorSignal) {
        float error = predictionErrorSignal->getLevel();
        std::cout << "Prediction error level: " << error << std::endl;
        
        if (std::abs(error) > 0.2f) {
            std::cout << "Significant prediction error - driving plasticity!" << std::endl;
        }
    }
    
    std::cout << "\n=== Example 3 Complete ===" << std::endl;
    std::cout << "Neuromodulation simulation completed successfully!" << std::endl;
    return 0;
}