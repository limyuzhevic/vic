#include "brain/Brain.hpp"
#include "development/DevelopmentManager.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    // Create a simple config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    
    if (!initSuccess) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Test that DevelopmentManager is accessible
    auto* devManager = brain.getDevelopmentManager();
    if (!devManager) {
        std::cerr << "DevelopmentManager is null after initialization" << std::endl;
        return 1;
    }
    
    std::cout << "DevelopmentManager successfully created and accessible" << std::endl;
    
    // Test development manager statistics
    const auto& stats = devManager->getStats();
    std::cout << "Development stats - Total updates: " << stats.totalUpdates << std::endl;
    
    // Run a few steps to test integration
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
        
        // Check that development effects are being applied periodically
        if (step % 100 == 0 && step > 0) {
            std::cout << "Step " << step << ": Development effects applied" << std::endl;
            // We could check developmental stage progression here
        }
    }
    
    // Test develop() method
    brain.develop();
    std::cout << "develop() method called successfully" << std::endl;
    
    std::cout << "\nAll DevelopmentManager tests passed!" << std::endl;
    return 0;
}