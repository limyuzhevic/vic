#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    // Create brain configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create brain
    nlm::Brain brain(config);
    
    // Initialize brain
    bool success = brain.initialize();
    if (!success) {
        std::cerr << "Brain initialization failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully!" << std::endl;
    
    // Check if concept formation system exists
    nlm::ConceptFormation* conceptFormation = brain.getConceptFormation();
    if (conceptFormation) {
        std::cout << "ConceptFormation system is available!" << std::endl;
        std::cout << "Initial concept count: " << conceptFormation->getConceptCount() << std::endl;
    } else {
        std::cout << "ConceptFormation system is not available!" << std::endl;
        return 1;
    }
    
    // Run a few steps to test concept formation integration
    std::cout << "\nRunning brain steps to test concept formation integration..." << std::endl;
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Check concept formation after each step
        if (step % 5 == 0) {
            std::cout << "Step " << step << ": Concept count = " << conceptFormation->getConceptCount() << std::endl;
        }
    }
    
    std::cout << "\nConcept formation integration test completed successfully!" << std::endl;
    return 0;
}
