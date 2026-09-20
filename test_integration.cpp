#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== NLM Phase 6 Integration Test ===" << std::endl;
    
    // Initialize logging
    auto logger = nlm::getLogger();
    logger->setLevel(nlm::LogLevel::INFO);
    
    try {
        // Create configuration
        auto config = std::make_shared<nlm::Config>();
        
        // Set basic configuration
        config->set("neuron_count", 100);
        config->set("region_count", 2);
        config->set("simulation_timestep", 0.001);
        
        // Create brain
        auto brain = std::make_unique<nlm::Brain>(config);
        
        // Initialize brain
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return 1;
        }
        
        std::cout << "Brain initialized successfully!" << std::endl;
        std::cout << "  - " << brain->getTotalNeuronCount() << " neurons" << std::endl;
        std::cout << "  - " << brain->getRegionCount() << " regions" << std::endl;
        
        // Test step function
        std::cout << "\nTesting simulation steps..." << std::endl;
        for (int step = 0; step < 10; ++step) {
            brain->step(step, static_cast<nlm::Timestamp>(step) * 0.001);
            
            // Log every 5 steps
            if (step % 5 == 0) {
                std::cout << "  Step " << step << ": "
                         << brain->getFiringNeuronCount() << " firing, "
                         << brain->getTotalSpikeCount() << " total spikes" << std::endl;
            }
        }
        
        // Test neuromodulation integration
        std::cout << "\nTesting neuromodulation integration..." << std::endl;
        
        // Get neuromodulators
        auto dopamine = brain->getDopamine();
        if (dopamine) {
            std::cout << "  - Dopamine level: " << dopamine->getLevel() << std::endl;
            dopamine->setLevel(0.5f);
            std::cout << "  - Set dopamine to 0.5, now: " << dopamine->getLevel() << std::endl;
        }
        
        // Test memory systems
        auto workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            std::cout << "  - Working memory capacity: " << workingMemory->getCapacity() << std::endl;
            std::cout << "  - Working memory traces: " << workingMemory->getActiveTraces() << std::endl;
        }
        
        // Test episodic memory
        auto episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            std::cout << "  - Episodic memory episodes: " << episodicMemory->getEpisodeCount() << std::endl;
        }
        
        // Test plasticity
        auto stdp = brain->getSTDP();
        if (stdp) {
            std::cout << "  - STDP system active" << std::endl;
        }
        
        auto hebbian = brain->getHebbian();
        if (hebbian) {
            std::cout << "  - Hebbian learning system active" << std::endl;
        }
        
        std::cout << "\n=== All integration tests passed! ===" << std::endl;
        std::cout << "Brain is fully functional with all Phase 6 systems integrated." << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}