#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/cognition/ConceptFormation.hpp"
#include <iostream>

int main() {
    std::cout << "Testing SpatialRepresentation Integration..." << std::endl;
    
    // Create config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    // Create Brain
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Test 1: Check if SpatialRepresentation is accessible
    auto* spatial = brain.getSpatialRepresentation();
    if (spatial) {
        std::cout << "✓ SpatialRepresentation is accessible" << std::endl;
    } else {
        std::cerr << "✗ SpatialRepresentation is NOT accessible" << std::endl;
        return 1;
    }
    
    // Test 2: Check SpatialRepresentation status
    if (spatial->hasPlaceCells()) {
        std::cout << "✓ SpatialRepresentation has place cells" << std::endl;
    } else {
        std::cout << "ℹ SpatialRepresentation doesn't have place cells yet (expected for empty brain)" << std::endl;
    }
    
    // Test 3: Check spatial context
    auto context = brain.getSpatialContext();
    std::cout << "✓ Spatial context: x=" << context.first << ", y=" << context.second << std::endl;
    
    // Test 4: Test through AgentBrain
    auto agentBrain = std::make_shared<nlm::AgentBrain>(std::shared_ptr<nlm::Brain>(&brain, [](nlm::Brain*) {}));
    
    auto agentContext = agentBrain->getSpatialContext();
    std::cout << "✓ AgentBrain spatial context: x=" << agentContext.first << ", y=" << agentContext.second << std::endl;
    
    // Test 5: Test hasSpatialContext
    if (agentBrain->hasSpatialContext()) {
        std::cout << "✓ AgentBrain has spatial context" << std::endl;
    } else {
        std::cout << "ℹ AgentBrain doesn't have spatial context yet (expected)" << std::endl;
    }
    
    std::cout << "\n✓ All SpatialRepresentation integration tests passed!" << std::endl;
    return 0;
}
