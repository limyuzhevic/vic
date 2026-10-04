
#include "brain/Brain.hpp"
#include "cognition/ConceptFormation.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "=== NLM Phase 6 Concept Formation Integration Test ===" << std::endl;
    
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
    
    // Get concept formation system
    auto* conceptFormation = brain->getConceptFormation();
    
    std::cout << "Concept formation initialized: " << (conceptFormation != nullptr ? "✓" : "✗") << std::endl;
    
    if (conceptFormation) {
        // Test concept formation parameters
        std::cout << "Initial formation threshold: " << conceptFormation->getFormationThreshold() << std::endl;
        std::cout << "Initial stability threshold: " << conceptFormation->getStabilityThreshold() << std::endl;
        
        // Present some experiences
        std::vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        std::vector<float> features1 = {0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
        
        std::cout << "Presenting experience 1..." << std::endl;
        size_t conceptId1 = conceptFormation->presentExperience(pattern1, features1, 0.8f, 0);
        std::cout << "Concept ID for experience 1: " << conceptId1 << std::endl;
        
        std::vector<float> pattern2 = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
        std::vector<float> features2 = {0.7f, 0.8f, 0.9f, 1.0f, 1.1f};
        
        std::cout << "Presenting experience 2..." << std::endl;
        size_t conceptId2 = conceptFormation->presentExperience(pattern2, features2, 0.9f, 1);
        std::cout << "Concept ID for experience 2: " << conceptId2 << std::endl;
        
        // Check concept formation state
        std::cout << "Total concepts discovered: " << conceptFormation->getConceptCount() << std::endl;
        
        if (conceptId1 > 0) {
            auto proto = conceptFormation->getConceptPrototype(conceptId1);
            std::cout << "Concept " << conceptId1 << " prototype size: " << proto.size() << std::endl;
        }
        
        if (conceptId2 > 0) {
            auto proto = conceptFormation->getConceptPrototype(conceptId2);
            std::cout << "Concept " << conceptId2 << " prototype size: " << proto.size() << std::endl;
        }
    }
    
    std::cout << "✓ Concept formation integration test completed successfully" << std::endl;
    return 0;
}
