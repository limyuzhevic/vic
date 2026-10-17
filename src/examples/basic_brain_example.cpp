// NLM - Basic Brain Example
// Demonstrates core brain functionality without complex interaction

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <chrono>

int main() {
    std::cout << "=== NLM Basic Brain Example ===" << std::endl;
    std::cout << "Demonstrates core brain simulation without environmental interaction." << std::endl;
    
    // Initialize logger for debugging
    initializeLogger("basic_brain_example.log", LogLevel::Info);
    
    try {
        // Create default configuration
        nlm::Config config = nlm::Config::createDefaultConfig();
        
        // Customize configuration for basic demonstration
        config.set("brain.neuron_count", 500);
        config.set("brain.synapse_density", 0.1);
        config.set("brain.v_thresh", -50.0);
        config.set("brain.v_rest", -70.0);
        config.set("plasticity.stdp.enable", true);
        config.set("plasticity.hebbian.enable", true);
        config.set("development.initial_plasticity", 1.0);
        
        std::cout << "Configuration created with " << config.get<int>("brain.neuron_count") 
                  << " neurons and " << config.get<float>("brain.synapse_density") * 100 
                  << "% synapse density." << std::endl;
        
        // Create brain with configuration
        nlm::Brain brain(config);
        
        std::cout << "Brain created successfully." << std::endl;
        
        // Initialize brain
        brain.initialize();
        
        std::cout << "Brain initialized with " << brain.getTotalNeuronCount() 
                  << " neurons and " << brain.getTotalSynapseCount() 
                  << " synapses." << std::endl;
        
        // Run simulation with detailed statistics
        std::cout << "\nStarting simulation..." << std::endl;
        std::cout << "Step, Firing Neurons, Average Firing Rate (Hz), Membrane Potential Mean (mV)" << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < 200; ++step) {
            // Run brain step
            brain.step(step);
            
            // Record statistics every 10 steps
            if (step % 10 == 0) {
                double firing_rate = brain.getAverageFiringRate();
                double membrane_potential_mean = brain.getMembranePotentialMean();
                
                std::cout << step << ", " 
                          << brain.getFiringNeuronCount() << ", "
                          << firing_rate << ", "
                          << membrane_potential_mean << std::endl;
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "\nSimulation completed in " << duration.count() << " ms." << std::endl;
        std::cout << "Average steps per second: " << (200.0 / (duration.count() / 1000.0)) << std::endl;
        
        // Print final statistics
        std::cout << "\nFinal Brain Statistics:" << std::endl;
        std::cout << "  Total neurons: " << brain.getTotalNeuronCount() << std::endl;
        std::cout << "  Total synapses: " << brain.getTotalSynapseCount() << std::endl;
        std::cout << "  Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
        std::cout << "  Average firing rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  Excitation-inhibition ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
        std::cout << "  Development stage: " << brain.getDevelopmentalStage() << std::endl;
        std::cout << "  Plasticity rate: " << brain.getPlasticityRate() << std::endl;
        std::cout << "  Excitability: " << brain.getExcitability() << std::endl;
        
        // Save final brain state
        brain.save("basic_brain_final.bin");
        std::cout << "\nBrain state saved to 'basic_brain_final.bin'" << std::endl;
        
        // Demonstrate loading from checkpoint
        std::cout << "\nDemonstrating checkpoint save/load..." << std::endl;
        
        // Create a new brain and load the checkpoint
        nlm::Brain brain2(config);
        brain2.load("basic_brain_final.bin");
        
        std::cout << "Loaded brain state successfully." << std::endl;
        std::cout << "Loaded brain has " << brain2.getTotalNeuronCount() << " neurons" << std::endl;
        
        std::cout << "\n=== Basic Brain Example Completed Successfully ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Unexpected exception in Basic Brain Example: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
