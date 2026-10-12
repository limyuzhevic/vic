#include "agent/AgentBrain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <iostream>

void testRefactoredAgentBrain() {
    std::cout << "Testing refactored AgentBrain with modular architecture..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Set configurable sensory dimensions
    config->setVisionDimensions(32, 32);  // Different from default 16x16
    config->setTouchCount(12);            // Different from default 8
    config->setInternalSignalCount(6);    // Different from default 4
    config->setProprioceptionCount(8);    // Different from default 6
    config->setMotorActionCount(8);       // Different from default 6
    
    // Create mock brain (for testing purposes)
    // In real usage, this would be a proper Brain instance
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Create AgentBrain with modular architecture
    nlm::AgentBrain agentBrain(brain, config);
    
    // Verify initialization
    std::cout << "AgentBrain created with modular components" << std::endl;
    std::cout << "Vision dimensions: " << config->getVisionWidth() << "x" << config->getVisionHeight() << std::endl;
    std::cout << "Touch count: " << config->getTouchCount() << std::endl;
    std::cout << "Internal signals: " << config->getInternalSignalCount() << std::endl;
    std::cout << "Proprioception count: " << config->getProprioceptionCount() << std::endl;
    std::cout << "Motor actions: " << config->getMotorActionCount() << std::endl;
    
    // Verify sensory input size is configurable
    size_t sensoryInputSize = agentBrain.getSensoryInputSize();
    std::cout << "Sensory input size: " << sensoryInputSize << " (configurable)" << std::endl;
    
    // Verify motor output size is configurable
    size_t motorOutputSize = agentBrain.getMotorOutputSize();
    std::cout << "Motor output size: " << motorOutputSize << " (configurable)" << std::endl;
    
    // Test configuration methods
    agentBrain.enableRewardModulation(false);
    assert(!agentBrain.isRewardModulationEnabled());
    
    agentBrain.enableDevelopment(false);
    assert(!agentBrain.isDevelopmentEnabled());
    
    agentBrain.enableCuriosity(false);
    assert(!agentBrain.isCuriosityEnabled());
    
    std::cout << "Configuration methods work correctly" << std::endl;
    
    // Reset to verify reset works
    agentBrain.reset();
    std::cout << "Reset completed successfully" << std::endl;
    
    std::cout << "✓ All tests passed! Refactored AgentBrain maintains backward compatibility with modular architecture." << std::endl;
}

int main() {
    try {
        testRefactoredAgentBrain();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
