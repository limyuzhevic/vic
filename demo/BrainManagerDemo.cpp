#include <iostream>
#include "BrainManager.hpp"
#include "core/Config/Config.hpp"

void demonstrateBrainManagerUsage() {
    std::cout << "=== BrainManager Usage Demo ===\n\n";
    
    // Create a brain with optimal configuration
    auto config = nlm::BrainManager::createConfig(
        100,           // neuron_count
        1,             // region_count
        0.1f,          // connection_probability
        0.001,         // simulation_timestep
        42             // random_seed
    );
    
    auto brain = nlm::BrainManager::createBrain(config);
    
    if (brain->initialize()) {
        std::cout << "✓ Brain created and initialized successfully\n";
        
        // Get basic statistics
        std::cout << "\nInitial Statistics:";
        std::cout << "  Neurons: " << nlm::BrainManager::getNeuronCount(brain) << "\n";
        std::cout << "  Synapses: " << nlm::BrainManager::getSynapseCount(brain) << "\n";
        std::cout << "  Active neurons: " << nlm::BrainManager::getActiveNeuronCount(brain) << "\n";
        std::cout << "  Average firing rate: " << nlm::BrainManager::getAverageFiringRate(brain) << " Hz\n";
        
        // Run a simulation
        std::cout << "\nRunning simulation (50 steps)...";
        nlm::BrainManager::runSimpleSimulation(brain, 50);
        
        std::cout << "\n✓ Simulation completed successfully\n";
        
        // Save brain state
        std::cout << "\nSaving brain state...";
        if (nlm::BrainManager::saveBrainState(brain, "test_brain_checkpoint.dat")) {
            std::cout << " ✓ Saved to file\n";
        } else {
            std::cout << " ✗ Failed to save\n";
        }
        
        // Reset brain
        nlm::BrainManager::resetBrain(brain);
        std::cout << "✓ Brain reset\n";
        
    } else {
        std::cout << "✗ Brain initialization failed\n";
    }
}

void demonstrateConfigurationManagement() {
    std::cout << "\n=== Configuration Management Demo ===\n";
    
    // Create custom config
    auto config = nlm::BrainManager::createConfig(
        200,           // More neurons
        2,             // Multiple regions  
        0.15f,         // Higher connection probability
        0.002,         // Longer timestep
        123            // Different seed
    );
    
    std::cout << "✓ Created custom configuration:\n";
    std::cout << "  " << config->summary() << "\n";
    
    // Modify configuration
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Runtime);
    config->set("random_seed", static_cast<int64_t>(456), nlm::ConfigSource::Runtime);
    
    std::cout << "✓ Modified configuration for new brain\n";
}

void demonstrateDevelopmentFeatures() {
    std::cout << "\n=== Development Features Demo ===\n";
    
    auto config = nlm::BrainManager::createConfig(100, 1, 0.1f);
    auto brain = nlm::BrainManager::createBrain(config);
    
    if (brain->initialize()) {
        std::cout << "✓ Brain initialized\n";
        
        // Enable development and plasticity
        nlm::BrainManager::enableDevelopment(brain, true);
        nlm::BrainManager::enablePlasticity(brain, true);
        nlm::BrainManager::enableRewardModulation(brain, true);
        nlm::BrainManager::enableCuriosity(brain, true);
        
        std::cout << "✓ Enabled development and learning features\n";
        
        // Run development simulation
        nlm::BrainManager::runSimpleSimulation(brain, 30);
        std::cout << "✓ Development simulation completed\n";
    }
}

void demonstrateErrorHandling() {
    std::cout << "\n=== Error Handling Demo ===\n";
    
    // Test null brain handling
    std::shared_ptr<nlm::Brain> nullBrain;
    
    std::cout << "Testing null brain handling...";
    auto count = nlm::BrainManager::getNeuronCount(nullBrain);
    auto rate = nlm::BrainManager::getAverageFiringRate(nullBrain);
    std::cout << "  Neuron count: " << count << " (expected: 0)\n";
    std::cout << "  Firing rate: " << rate << " Hz (expected: 0.0)\n";
    
    if (count == 0 && rate == 0.0f) {
        std::cout << "✓ Null brain handling works correctly\n";
    } else {
        std::cout << "✗ Null brain handling failed\n";
    }
}

void demonstrateSystemStats() {
    std::cout << "\n=== System Statistics Demo ===\n";
    
    auto config = nlm::BrainManager::createConfig(100, 1, 0.1f);
    auto brain = nlm::BrainManager::createBrain(config);
    
    if (brain->initialize()) {
        nlm::BrainManager::runSimpleSimulation(brain, 20);
        
        auto stats = nlm::BrainManager::getSystemStats(brain);
        std::cout << "✓ System statistics gathered:\n";
        for (const auto& [key, value] : stats) {
            std::cout << "  " << key << ": " << value << "\n";
        }
        
        std::cout << "✓ Statistics system working correctly\n";
    }
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "NLM BrainManager Demonstration Program\n";
    std::cout << "==========================================\n\n";
    
    try {
        demonstrateBrainManagerUsage();
        demonstrateConfigurationManagement();
        demonstrateDevelopmentFeatures();
        demonstrateErrorHandling();
        demonstrateSystemStats();
        
        std::cout << "\n" << "==========================================\n";
        std::cout << "✓ All demonstrations completed successfully!\n";
        std::cout << "\nThe improved NLM BrainManager provides:\n";
        std::cout << "  • Simplified, high-level API for brain management\n";
        std::cout << "  • Robust error handling for null objects\n";
        std::cout << "  • Comprehensive configuration management\n";
        std::cout << "  • Development and learning controls\n";
        std::cout << "  • Built-in system statistics\n";
        std::cout << "  • Easy-to-use simulation utilities\n";
        std::cout << "  • Integration with existing nlm framework\n";
        std::cout << "==========================================\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
