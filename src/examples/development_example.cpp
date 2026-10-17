// NLM - Development Example
// Demonstrates brain development, maturation, and aging effects on learning and behavior

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <iomanip>

int main() {
    std::cout << "=== NLM Development Example ===" << std::endl;
    std::cout << "Demonstrates brain development stages, maturation, and aging effects on neural plasticity and learning capabilities." << std::endl;
    
    // Initialize logger for debugging
    initializeLogger("development_example.log", LogLevel::Info);
    
    try {
        // Create configuration optimized for development demonstration
        nlm::Config config = nlm::Config::createDefaultConfig();
        
        // Configure for developmental stages demonstration
        config.set("brain.neuron_count", 500);
        config.set("brain.synapse_density", 0.12);
        config.set("development.initial_plasticity", 2.0);    // High initial plasticity
        config.set("development.maturation_rate", 0.005);       // Maturation rate
        config.set("development.critical_period", 80.0);       // Critical period duration
        config.set("development.plasticity_decay", 0.998);   // Plasticity decay rate
        config.set("development.synaptic_pruning", true);     // Enable synaptic pruning
        config.set("plasticity.stdp.enable", true);
        config.set("plasticity.hebbian.enable", true);
        
        std::cout << "Configuration created with development focus:" << std::endl;
        std::cout << "  - Initial plasticity: " << config.get<double>("development.initial_plasticity") << std::endl;
        std::cout << "  - Critical period: " << config.get<double>("development.critical_period") << " steps" << std::endl;
        std::cout << "  - Plasticity decay rate: " << config.get<double>("development.plasticity_decay") << std::endl;
        std::cout << "  - Synaptic pruning: " << (config.get<bool>("development.synaptic_pruning") ? "Enabled" : "Disabled") << std::endl;
        
        // Create brain with developmental configuration
        nlm::Brain brain(config);
        brain.initialize();
        
        std::cout << "Brain created successfully with developmental systems." << std::endl;
        
        // Run simulation with developmental stages
        std::cout << "\nStarting developmental stage simulation..." << std::endl;
        std::cout << "Step, Developmental Stage, Plasticity Rate, Excitability, Learning Rate, "
                  << "Firing Neurons, Synapse Count" << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < 400; ++step) {
            // Get current developmental state
            auto dev_state = brain.getDevelopmentalState();
            
            // Update development
            brain.updateDevelopment(0.1);
            
            // Run brain step with developmental effects
            brain.step(step);
            
            // Get learning statistics
            double learning_rate = brain.getPlasticityRate();
            double excitability = brain.getExcitability();
            
            // Log developmental progress every 20 steps
            if (step % 20 == 0) {
                int developmental_stage = brain.getDevelopmentalStage();
                std::string stage_name;
                
                switch (developmental_stage) {
                    case 0: stage_name = "Initial"; break;
                    case 1: stage_name = "Critical Period"; break;
                    case 2: stage_name = "Maturation"; break;
                    case 3: stage_name = "Adult"; break;
                    case 4: stage_name = "Aging"; break;
                    default: stage_name = "Unknown"; break;
                }
                
                std::cout << step << ", "
                          << stage_name << ", "
                          << learning_rate << ", "
                          << excitability << ", "
                          << (learning_rate * 0.001) << ", "  // Approximate learning rate
                          << brain.getFiringNeuronCount() << ", "
                          << brain.getTotalSynapseCount() << std::endl;
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "\nDevelopment simulation completed in " << duration.count() << " ms." << std::endl;
        std::cout << "Average steps per second: " << (400.0 / (duration.count() / 1000.0)) << std::endl;
        
        // Print final developmental statistics
        std::cout << "\nFinal Developmental Statistics:" << std::endl;
        std::cout << "  Final developmental stage: " << brain.getDevelopmentalStage() << std::endl;
        std::cout << "  Maturity level: " << brain.getMaturationProgress() * 100.0 << "%" << std::endl;
        std::cout << "  Final plasticity rate: " << brain.getPlasticityRate() << std::endl;
        std::cout << "  Final excitability: " << brain.getExcitability() << std::endl;
        std::cout << "  Final firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  Final synapse count: " << brain.getTotalSynapseCount() << std::endl;
        
        // Demonstrate critical period learning
        std::cout << "\nDemonstrating critical period learning effects..." << std::endl;
        
        // Create new brain for critical period demonstration
        nlm::Brain brain2(config);
        brain2.initialize();
        
        // Simulate learning during critical period
        double critical_period_reward = 0.0;
        int critical_period_steps = 0;
        
        for (int step = 0; step < 60; ++step) {
            // Get developmental stage
            int developmental_stage = brain2.getDevelopmentalStage();
            
            // Simulate experience-based learning
            double experience_value = (std::rand() % 100) / 100.0; // 0-1.0
            
            // Critical period learning is enhanced
            double learning_effectiveness = (developmental_stage == 1) ? 2.0 : 1.0;
            double reward = experience_value * learning_effectiveness;
            critical_period_reward += reward;
            critical_period_steps++;
            
            // Apply reward
            brain2.applyReward(reward, 0.0);
            
            // Update development
            brain2.updateDevelopment(0.1);
            
            // Run simulation
            brain2.step(step);
            
            if (step % 20 == 0) {
                std::cout << "Critical period step " << step 
                          << ": Learning effectiveness " << learning_effectiveness 
                          << ", Reward " << reward 
                          << ", Total: " << critical_period_reward << std::endl;
            }
        }
        
        std::cout << "\nCritical period learning completed:" << std::endl;
        std::cout << "  Total reward: " << critical_period_reward << std::endl;
        std::cout << "  Average learning effectiveness: " << (critical_period_reward / critical_period_steps) << std::endl;
        std::cout << "  Final developmental stage: " << brain2.getDevelopmentalStage() << std::endl;
        
        // Demonstrate aging effects
        std::cout << "\nDemonstrating aging effects..." << std::endl;
        
        // Create new brain for aging demonstration
        nlm::Config aging_config = config;
        aging_config.set("development.initial_plasticity", 1.5);
        aging_config.set("development.maturation_rate", 0.008);
        aging_config.set("development.plasticity_decay", 0.999);
        
        nlm::Brain brain3(aging_config);
        brain3.initialize();
        
        // Simulate lifelong learning with aging
        double aging_learning_reward = 0.0;
        double aging_plasticity_rate = 0.0;
        
        for (int step = 0; step < 200; ++step) {
            // Get current plasticity rate
            aging_plasticity_rate = brain3.getPlasticityRate();
            
            // Simulate learning with age-dependent plasticity
            double age_effectiveness = aging_plasticity_rate;
            double age_reward = age_effectiveness * 0.5;
            aging_learning_reward += age_reward;
            
            // Apply reward
            brain3.applyReward(age_reward, 0.0);
            
            // Update development
            brain3.updateDevelopment(0.1);
            
            // Run simulation
            brain3.step(step);
            
            if (step % 50 == 0) {
                std::cout << "Aging step " << step 
                          << ": Plasticity rate " << aging_plasticity_rate 
                          << ", Learning reward " << age_reward 
                          << ", Total: " << aging_learning_reward << std::endl;
            }
        }
        
        std::cout << "\nAging effects completed:" << std::endl;
        std::cout << "  Total learning reward: " << aging_learning_reward << std::endl;
        std::cout << "  Final plasticity rate: " << aging_plasticity_rate << std::endl;
        std::cout << "  Plasticity decay over time: " << (config.get<double>("development.initial_plasticity") - aging_plasticity_rate) << std::endl;
        
        // Save final state
        brain.save("development_final.bin");
        std::cout << "\nDevelopment state saved to 'development_final.bin'" << std::endl;
        
        // Demonstrate synaptic pruning
        std::cout << "\nDemonstrating synaptic pruning effects..." << std::endl;
        
        // Create new brain and load state
        nlm::Brain brain4(config);
        brain4.load("development_final.bin");
        
        // Track synapse count over time
        std::vector<int> synapse_counts;
        
        for (int step = 0; step < 150; ++step) {
            // Apply random experiences that trigger pruning
            if (step % 30 == 0) {
                // Simulate negative experiences leading to pruning
                brain4.applyPunishment(0.5); // Punish incorrect behavior
            }
            
            // Run simulation
            brain4.step(step);
            brain4.updateDevelopment(0.1);
            
            // Record synapse count
            synapse_counts.push_back(brain4.getTotalSynapseCount());
            
            if (step % 50 == 0) {
                std::cout << "Pruning step " << step 
                          << ": Synapse count " << brain4.getTotalSynapseCount() 
                          << ", Plasticity rate " << brain4.getPlasticityRate() << std::endl;
            }
        }
        
        // Analyze pruning effects
        if (synapse_counts.size() > 1) {
            int initial_synapses = synapse_counts[0];
            int final_synapses = synapse_counts.back();
            double pruning_efficiency = (initial_synapses - final_synapses) / (double)initial_synapses * 100.0;
            
            std::cout << "\nSynaptic pruning analysis:" << std::endl;
            std::cout << "  Initial synapse count: " << initial_synapses << std::endl;
            std::cout << "  Final synapse count: " << final_synapses << std::endl;
            std::cout << "  Pruning efficiency: " << pruning_efficiency << "%" << std::endl;
            std::cout << "  Synapse reduction: " << (initial_synapses - final_synapses) << " synapses" << std::endl;
        }
        
        std::cout << "\n=== Development Example Completed Successfully ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Unexpected exception in Development Example: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
