// NLM - Neuromodulation Example
// Demonstrates dopamine, curiosity, and novelty effects on learning and behavior

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>

int main() {
    std::cout << "=== NLM Neuromodulation Example ===" << std::endl;
    std::cout << "Demonstrates how neuromodulators (dopamine, curiosity, novelty) affect learning, memory, and behavior." << std::endl;
    
    // Initialize logger for debugging
    initializeLogger("neuromodulation_example.log", LogLevel::Info);
    
    try {
        // Create configuration optimized for neuromodulation
        nlm::Config config = nlm::Config::createDefaultConfig();
        
        // Configure for neuromodulation demonstration
        config.set("brain.neuron_count", 700);
        config.set("brain.synapse_density", 0.18);
        config.set("neuromod.dopamine.scale", 1.5);
        config.set("neuromod.dopamine.baseline", 0.2);
        config.set("neuromod.curiosity.enable", true);
        config.set("neuromod.curiosity.sensitivity", 1.2);
        config.set("neuromod.curiosity.threshold", 0.08);
        config.set("neuromod.novelty.enable", true);
        config.set("neuromod.novelty.detection_method", "prediction_error");
        config.set("neuromod.novelty.weight", 0.8);
        config.set("development.initial_plasticity", 1.0);
        
        std::cout << "Configuration created with neuromodulation focus:" << std::endl;
        std::cout << "  - Dopamine scale: " << config.get<double>("neuromod.dopamine.scale") << std::endl;
        std::cout << "  - Curiosity enabled: " << (config.get<bool>("neuromod.curiosity.enable") ? "Yes" : "No") << std::endl;
        std::cout << "  - Novelty detection: " << config.get<std::string>("neuromod.novelty.detection_method") << std::endl;
        
        // Create brain with neuromodulation configuration
        nlm::Brain brain(config);
        brain.initialize();
        
        std::cout << "Brain created successfully with neuromodulation systems." << std::endl;
        
        // Run simulation with neuromodulation effects
        std::cout << "\nStarting neuromodulation simulation..." << std::endl;
        std::cout << "Step, Novelty, Dopamine, Curiosity, Prediction Error, "
                  << "Firing Neurons, Structural Changes, Developmental Stage" << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        double total_dopamine = 0.0;
        double total_curiosity = 0.0;
        double total_novelty = 0.0;
        double total_prediction_error = 0.0;
        
        for (int step = 0; step < 300; ++step) {
            // Generate novelty (random input changes)
            double novelty = (std::rand() % 100) / 1000.0; // 0-0.1
            
            // Apply novelty to neuromodulation
            brain.applyNovelty(novelty);
            
            // Update prediction system
            std::vector<double> sensory_input = {novelty, 0.5, 0.3, 0.1};
            brain.updatePrediction(sensory_input);
            
            // Get current neuromodulation levels
            double dopamine = brain.getDopamineLevel();
            double curiosity = brain.getCuriosityLevel();
            double prediction_error = brain.getPredictionError();
            
            // Update cumulative statistics
            total_dopamine += dopamine;
            total_curiosity += curiosity;
            total_novelty += novelty;
            total_prediction_error += prediction_error;
            
            // Run brain step (neuromodulators affect plasticity)
            brain.step(step);
            
            // Record development
            brain.updateDevelopment(0.1);
            
            // Log statistics periodically
            if (step % 50 == 0) {
                double structural_changes = brain.getStructuralPlasticityChanges();
                
                std::cout << step << ", "
                          << novelty << ", "
                          << dopamine << ", "
                          << curiosity << ", "
                          << prediction_error << ", "
                          << brain.getFiringNeuronCount() << ", "
                          << structural_changes << ", "
                          << brain.getDevelopmentalStage() << std::endl;
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "\nNeuromodulation simulation completed in " << duration.count() << " ms." << std::endl;
        std::cout << "Average steps per second: " << (300.0 / (duration.count() / 1000.0)) << std::endl;
        
        // Print final neuromodulation statistics
        std::cout << "\nFinal Neuromodulation Statistics:" << std::endl;
        std::cout << "  Average dopamine level: " << (total_dopamine / 300.0) << std::endl;
        std::cout << "  Average curiosity level: " << (total_curiosity / 300.0) << std::endl;
        std::cout << "  Average novelty level: " << (total_novelty / 300.0) << std::endl;
        std::cout << "  Average prediction error: " << (total_prediction_error / 300.0) << std::endl;
        std::cout << "  Final brain firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  Final development stage: " << brain.getDevelopmentalStage() << std::endl;
        std::cout << "  Final plasticity rate: " << brain.getPlasticityRate() << std::endl;
        
        // Demonstrate curiosity-driven exploration
        std::cout << "\nDemonstrating curiosity-driven exploration..." << std::endl;
        
        // Create new brain and load state
        nlm::Brain brain2(config);
        brain2.load("neuromodulation_final.bin"); // Load from demo run
        
        // Simulate exploration with curiosity
        double exploration_reward = 0.0;
        int exploration_steps = 0;
        
        for (int step = 0; step < 150; ++step) {
            // High novelty triggers curiosity
            double novelty = (std::rand() % 100) / 1000.0;
            
            if (novelty > 0.08) { // Above curiosity threshold
                brain2.applyCuriosity(novelty);
                exploration_steps++;
                
                // Get curiosity-driven reward
                double curiosity_reward = brain2.getCuriosityLevel() * 0.1;
                exploration_reward += curiosity_reward;
                
                std::cout << "Step " << step << ": Curiosity triggered! Novelty: " 
                          << novelty << ", Reward: " << curiosity_reward 
                          << ", Total exploration reward: " << exploration_reward << std::endl;
            }
            
            // Run simulation
            brain2.step(step);
            brain2.updateDevelopment(0.1);
        }
        
        std::cout << "\nCuriosity-driven exploration completed:" << std::endl;
        std::cout << "  Exploration steps: " << exploration_steps << std::endl;
        std::cout << "  Total curiosity reward: " << exploration_reward << std::endl;
        std::cout << "  Average curiosity per exploration step: " 
                  << (exploration_steps > 0 ? exploration_reward / exploration_steps : 0.0) << std::endl;
        
        // Demonstrate reward prediction error learning
        std::cout << "\nDemonstrating reward prediction error learning..." << std::endl;
        
        // Create new brain for RPE demonstration
        nlm::Brain brain3(config);
        brain3.initialize();
        
        double prediction_error_total = 0.0;
        int rpe_steps = 0;
        
        for (int step = 0; step < 100; ++step) {
            // Generate reward prediction error
            double actual_reward = (std::rand() % 100) / 100.0; // 0-1.0
            double predicted_reward = 0.5; // Constant prediction for demonstration
            
            double rpe = std::abs(actual_reward - predicted_reward);
            prediction_error_total += rpe;
            rpe_steps++;
            
            // Apply reward with prediction error
            brain3.applyReward(actual_reward, predicted_reward);
            
            // Update neuromodulators
            brain3.updateNeuromodulators(0.1);
            
            // Run simulation
            brain3.step(step);
            brain3.updateDevelopment(0.1);
            
            if (step % 25 == 0) {
                std::cout << "Step " << step << ": RPE = " << rpe 
                          << ", Actual: " << actual_reward 
                          << ", Predicted: " << predicted_reward << std::endl;
            }
        }
        
        std::cout << "\nReward prediction error learning completed:" << std::endl;
        std::cout << "  Average RPE: " << (prediction_error_total / rpe_steps) << std::endl;
        std::cout << "  Final dopamine level: " << brain3.getDopamineLevel() << std::endl;
        
        // Save final state
        brain.save("neuromodulation_final.bin");
        std::cout << "\nNeuromodulation state saved to 'neuromodulation_final.bin'" << std::endl;
        
        std::cout << "\n=== Neuromodulation Example Completed Successfully ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Unexpected exception in Neuromodulation Example: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
