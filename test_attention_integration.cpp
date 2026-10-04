
#include "brain/Brain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "cognition/AttentionalSelection.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Phase 6 Attention Integration Test ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Get attention system
    auto* attention = brain->getAttention();
    
    std::cout << "Attention system initialized: " << (attention != nullptr ? "✓" : "✗") << std::endl;
    
    if (attention) {
        // Test attention parameters
        std::cout << "Initial inhibition strength: " << attention->getInhibitionStrength() << std::endl;
        std::cout << "Initial excitation strength: " << attention->getExcitationStrength() << std::endl;
        
        // Modify attention parameters
        attention->setInhibitionStrength(0.6f);
        attention->setExcitationStrength(1.2f);
        
        std::cout << "Updated inhibition strength: " << attention->getInhibitionStrength() << std::endl;
        std::cout << "Updated excitation strength: " << attention->getExcitationStrength() << std::endl;
    }
    
    // Test attention integration with working memory
    if (attention) {
        std::cout << "Testing attention integration with working memory..." << std::endl;
        
        // Run a few steps to test integration
        for (int step = 0; step < 5; ++step) {
            brain->step(step);
            
            // Check if attention is being used
            const auto& winners = attention->getWinners();
            std::cout << "Step " << step << ": Attention winners count: " << winners.size() << std::endl;
        }
    }
    
    std::cout << "✓ Attention integration test completed successfully" << std::endl;
    return 0;
}
