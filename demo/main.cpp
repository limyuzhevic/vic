#include <iostream>
#include "src/BrainManager.hpp"
#include "src/core/Config/Config.hpp"

void demonstrateSimpleBrain() {
    std::cout << "=== Simple Brain Demo ===\n\n";
    
    // Create a simple brain using BrainManager
    auto brain = nlm::BrainManager::createBrain(
        nlm::BrainManager::createConfig(
            100,           // 100 neurons
            1,             // 1 region
            0.1f,          // 10% connection probability
            0.001,         // 1ms timestep
            42             // seed
        )
    );
    
    std::cout << "Brain created successfully!\n";
    std::cout << "Brain status:\n" << brain->getConfig()->summary() << "\n";
    
    // Initialize the brain
    if (brain->initialize()) {
        std::cout << "Brain initialized successfully!\n\n";
        
        // Get basic stats
        std::cout << "Initial stats:\n";
        std::cout << "  Neurons: " << nlm::BrainManager::getNeuronCount(brain) << "\n";
        std::cout << "  Synapses: " << nlm::BrainManager::getSynapseCount(brain) << "\n";
        std::cout << "  Active neurons: " << nlm::BrainManager::getActiveNeuronCount(brain) << "\n";
        std::cout << "  Average firing rate: " << nlm::BrainManager::getAverageFiringRate(brain) << "\n\n";
        
        // Run a simple simulation with random sensory input
        std::cout << "Running simple simulation (50 steps)...\n";
        nlm::BrainManager::runSimpleSimulation(brain, 50);
        
        // Check final stats
        std::cout << "\nFinal stats:\n";
        std::cout << "  Neurons: " << nlm::BrainManager::getNeuronCount(brain) << "\n";
        std::cout << "  Synapses: " << nlm::BrainManager::getSynapseCount(brain) << "\n";
        std::cout << "  Active neurons: " << nlm::BrainManager::getActiveNeuronCount(brain) << "\n";
        std::cout << "  Average firing rate: " << nlm::BrainManager::getAverageFiringRate(brain) << "\n";
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << "\n";
        
        // Reset brain
        nlm::BrainManager::resetBrain(brain);
        std::cout << "\nBrain reset.\n";
        
    } else {
        std::cout << "Brain initialization failed!\n";
    }
}

void demonstrateConfigManagement() {
    std::cout << "=== Configuration Management Demo ===\n\n";
    
    // Create config using BrainManager
    auto config = nlm::BrainManager::createConfig(
        500,           // 500 neurons
        2,             // 2 regions
        0.15f,         // 15% connection probability
        0.001,         // 1ms timestep
        123            // different seed
    );
    
    std::cout << "Created config with " << nlm::BrainManager::getNeuronCount(
        nlm::BrainManager::createBrain(config)) << " neurons\n";
    
    // Save config to file
    if (nlm::BrainManager::saveConfigToFile(config, "demo_config.cfg")) {
        std::cout << "Saved config to 'demo_config.cfg'\n";
    }
    
    // Load config from file
    auto loadedConfig = nlm::BrainManager::loadConfigFromFile("demo_config.cfg");
    if (loadedConfig) {
        std::cout << "Loaded config from file:\n" << loadedConfig->summary() << "\n";
    }
    
    // Modify config
    loadedConfig->set("random_seed", static_cast<int64_t>(999), nlm::ConfigSource::Runtime);
    loadedConfig->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Runtime);
    
    std::cout << "Modified config:\n" << loadedConfig->summary() << "\n";
}

void demonstrateDevelopmentFeatures() {
    std::cout << "=== Development Features Demo ===\n\n";
    
    auto config = nlm::BrainManager::createConfig(100, 1, 0.1f);
    auto brain = nlm::BrainManager::createBrain(config);
    
    if (brain->initialize()) {
        std::cout << "Brain initialized with development features\n";
        
        // Enable development
        nlm::BrainManager::enableDevelopment(brain, true);
        
        // Run simulation with development
        std::cout << "Running simulation with development (20 steps)...\n";
        nlm::BrainManager::runSimpleSimulation(brain, 20);
        
        std::cout << "Development simulation complete.\n";
    }
}

void demonstrateErrorHandling() {
    std::cout << "=== Error Handling Demo ===\n\n";
    
    // Test with null brain (should handle gracefully)
    std::cout << "Testing null brain handling...\n";
    std::shared_ptr<nlm::Brain> nullBrain;
    
    std::cout << "Null brain neuron count: " << nlm::BrainManager::getNeuronCount(nullBrain) << "\n";
    std::cout << "Null brain firing rate: " << nlm::BrainManager::getAverageFiringRate(nullBrain) << "\n";
    
    // Test invalid file operations
    std::cout << "\nTesting invalid file operations...\n";
    auto config = nlm::BrainManager::createConfig(100, 1, 0.1f);
    bool saveResult = nlm::BrainManager::saveConfigToFile(config, "/invalid/path/config.json");
    std::cout << "Save to invalid path: " << (saveResult ? "succeeded" : "failed (expected)") << "\n";
}

int main() {
    std::cout << "NLM BrainManager Demonstration\n";
    std::cout << "==============================\n\n";
    
    try {
        demonstrateConfigManagement();
        std::cout << "\n" << std::string(50, '-') << "\n\n";
        
        demonstrateSimpleBrain();
        std::cout << "\n" << std::string(50, '-') << "\n\n";
        
        demonstrateDevelopmentFeatures();
        std::cout << "\n" << std::string(50, '-') << "\n\n";
        
        demonstrateErrorHandling();
        std::cout << "\n" << std::string(50, '-') << "\n\n";
        
        std::cout << "All demonstrations completed successfully!\n";
        std::cout << "The improved NLM BrainManager provides:\n";
        std::cout << "  - Simplified brain creation and initialization\n";
        std::cout << "  - Easy configuration management\n";
        std::cout << "  - Development and learning controls\n";
        std::cout << "  - Robust error handling\n";
        std::cout << "  - Clear, user-friendly API\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
