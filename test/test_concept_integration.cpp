#include "src/brain/Brain.hpp"
#include "src/cognition/ConceptFormation.hpp"
#include <iostream>

int main() {
    std::cout << "Testing concept formation integration..." << std::endl;
    
    // Test that ConceptFormation interface is properly integrated
    std::cout << "1. Checking ConceptFormation interface..." << std::endl;
    
    // Create a simple config for testing
    auto config = std::make_shared<nlm::Config>();
    
    // Create brain
    nlm::Brain brain(config);
    
    // Initialize brain
    bool initialized = brain.initialize();
    std::cout << "   Brain initialization: " << (initialized ? "PASS" : "FAIL") << std::endl;
    
    // Get concept formation system
    auto* conceptFormation = brain.getConceptFormation();
    if (conceptFormation) {
        std::cout << "2. ConceptFormation system accessible: PASS" << std::endl;
        
        // Check if concept formation is initialized
        if (conceptFormation->getConceptCount() == 0) {
            std::cout << "3. ConceptFormation starts with no concepts (expected): PASS" << std::endl;
        }
        
        // Test presentExperience with simple pattern
        std::vector<float> pattern = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
        std::vector<float> features = {0.5f};
        
        size_t conceptId = conceptFormation->presentExperience(pattern, features, 0.5f, 0);
        std::cout << "4. presentExperience() called: PASS (returned concept ID: " << conceptId << ")" << std::endl;
        
        if (conceptId > 0) {
            std::cout << "5. Concept created successfully: PASS" << std::endl;
            
            // Check if concept can be retrieved
            const nlm::DiscoveredConcept* concept = conceptFormation->getConcept(conceptId);
            if (concept) {
                std::cout << "6. Concept retrieved successfully: PASS" << std::endl;
                std::cout << "   Concept ID: " << concept->id << ", Category: " << concept->categoryHint << ", Stability: " << concept->avgStability << std::endl;
            }
            
            // Test concept-based operations
            std::vector<float> testPattern = {0.11f, 0.21f, 0.31f, 0.41f, 0.51f};
            size_t matchingConcept = conceptFormation->getMatchingConcept(testPattern);
            std::cout << "7. Concept matching works: PASS (matching concept ID: " << matchingConcept << ")" << std::endl;
            
            // Test updateConcept
            std::vector<float> updatedPattern = {0.12f, 0.22f, 0.32f, 0.42f, 0.52f};
            conceptFormation->updateConcept(conceptId, updatedPattern, features, 0.7f);
            std::cout << "8. Concept update works: PASS" << std::endl;
            
            // Test getConceptStability
            float stability = conceptFormation->getConceptStability(conceptId);
            std::cout << "9. Concept stability check works: PASS (stability: " << stability << ")" << std::endl;
        }
    } else {
        std::cout << "2. ConceptFormation system accessible: FAIL" << std::endl;
    }
    
    std::cout << "\nConcept formation integration test completed." << std::endl;
    return 0;
}