// Integration Tests for Refactored NLM Brain
// Tests the integration between Brain class and manager components

#include "brain/Brain.hpp"
#include "brain/BrainInitialization.hpp"
#include "brain/BrainMemoryManager.hpp"
#include "brain/BrainCognitionManager.hpp"
#include "brain/BrainNeuromodulationManager.hpp"
#include "brain/BrainDevelopmentManager.hpp"
#include "brain/BrainPlasticityManager.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_integration {

void testBrainInitializationManagerIntegration() {
    std::cout << "Testing BrainInitialization integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test BrainInitialization manager
    auto brainInitializer = std::make_unique<nlm::BrainInitialization>(config);
    bool success = brainInitializer->initialize(&brain);
    
    assert(success);
    assert(brain.getRegionCount() == 2);
    assert(brain.getTotalNeuronCount() == 200);
    
    std::cout << "    ✓ BrainInitialization integration passed" << std::endl;
}

void testBrainMemoryManagerIntegration() {
    std::cout << "Testing BrainMemoryManager integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test BrainMemoryManager
    auto memoryManager = std::make_unique<nlm::BrainMemoryManager>();
    memoryManager->initialize(&brain);
    
    // Verify memory systems are initialized
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    
    // Test memory updates
    memoryManager->updateWorkingMemory(&brain, 0.001);
    
    std::cout << "    ✓ BrainMemoryManager integration passed" << std::endl;
}

void testBrainCognitionManagerIntegration() {
    std::cout << "Testing BrainCognitionManager integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test BrainCognitionManager
    auto cognitionManager = std::make_unique<nlm::BrainCognitionManager>();
    cognitionManager->initialize(&brain);
    
    // Verify cognition systems are initialized
    assert(brain.getPlanner() != nullptr);
    assert(brain.getConceptFormation() != nullptr);
    assert(brain.getAttention() != nullptr);
    
    // Test cognition updates
    cognitionManager->updatePlanner(&brain, 0.001);
    cognitionManager->updateConceptFormation(&brain);
    cognitionManager->updateAttention(&brain, 0.001);
    
    std::cout << "    ✓ BrainCognitionManager integration passed" << std::endl;
}

void testBrainNeuromodulationManagerIntegration() {
    std::cout << "Testing BrainNeuromodulationManager integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(120), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test BrainNeuromodulationManager
    auto neuromodulationManager = std::make_unique<nlm::BrainNeuromodulationManager>();
    neuromodulationManager->initialize(&brain);
    
    // Verify neuromodulation systems are initialized
    assert(brain.getDopamine() != nullptr);
    assert(brain.getCuriosity() != nullptr);
    assert(brain.getNovelty() != nullptr);
    assert(brain.getPredictionErrorSignal() != nullptr);
    
    // Test neuromodulation updates
    neuromodulationManager->updateNeuromodulators(&brain, 0.001);
    
    // Test neuromodulation effects on neurons
    float neuromodulationLevel = brain.getDopamine() ? brain.getDopamine()->getLevel() : 0.0f;
    neuromodulationManager->applyNeuromodulationToNeurons(&brain, neuromodulationLevel);
    
    // Test neuromodulation effects on plasticity
    neuromodulationManager->applyNeuromodulationToPlasticity(&brain, neuromodulationLevel);
    
    std::cout << "    ✓ BrainNeuromodulationManager integration passed" << std::endl;
}

void testBrainDevelopmentManagerIntegration() {
    std::cout << "Testing BrainDevelopmentManager integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test BrainDevelopmentManager
    auto developmentManager = std::make_unique<nlm::BrainDevelopmentManager>();
    developmentManager->initialize(&brain);
    
    // Verify development system is initialized
    assert(brain.getDevelopmentSystem() != nullptr);
    
    // Test development updates
    nlm::SimulationStep currentStep = 1000;
    nlm::Timestamp currentTime = 1.0;
    nlm::TimestepDuration timestep = 0.001;
    developmentManager->updateDevelopment(&brain, currentStep, currentTime, timestep);
    
    std::cout << "    ✓ BrainDevelopmentManager integration passed" << std::endl;
}

void testBrainPlasticityManagerIntegration() {
    std::cout << "Testing BrainPlasticityManager integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(80), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test BrainPlasticityManager
    auto plasticityManager = std::make_unique<nlm::BrainPlasticityManager>();
    plasticityManager->initialize(&brain);
    
    // Verify plasticity systems are initialized
    assert(brain.getSTDP() != nullptr);
    assert(brain.getHebbian() != nullptr);
    assert(brain.getStructuralPlasticity() != nullptr);
    
    // Test structural plasticity updates
    nlm::SimulationStep currentStep = 100;
    plasticityManager->applyStructuralPlasticity(&brain, currentStep, *brain.getRandomGenerator());
    
    std::cout << "    ✓ BrainPlasticityManager integration passed" << std::endl;
}

void testSimulationLoopManagerIntegration() {
    std::cout << "Testing SimulationLoop integration..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(150), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test that simulation loop works with all managers
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
    }
    
    // Verify state changes over simulation
    assert(brain.getTotalNeuronCount() == 150);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    ✓ SimulationLoop integration passed" << std::endl;
}

void testCompleteIntegration() {
    std::cout << "Testing complete integration of all managers..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(300), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Initialize all managers
    auto brainInitializer = std::make_unique<nlm::BrainInitialization>(config);
    auto memoryManager = std::make_unique<nlm::BrainMemoryManager>();
    auto cognitionManager = std::make_unique<nlm::BrainCognitionManager>();
    auto neuromodulationManager = std::make_unique<nlm::BrainNeuromodulationManager>();
    auto developmentManager = std::make_unique<nlm::BrainDevelopmentManager>();
    auto plasticityManager = std::make_unique<nlm::BrainPlasticityManager>();
    
    assert(brainInitializer->initialize(&brain));
    memoryManager->initialize(&brain);
    cognitionManager->initialize(&brain);
    neuromodulationManager->initialize(&brain);
    developmentManager->initialize(&brain);
    plasticityManager->initialize(&brain);
    
    // Run simulation steps
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
    }
    
    // Verify integration
    assert(brain.getRegionCount() == 3);
    assert(brain.getTotalNeuronCount() == 300);
    assert(brain.getTotalSynapseCount() > 0);
    assert(brain.getTotalSpikeCount() >= 0);
    
    std::cout << "    ✓ Complete integration passed" << std::endl;
}

void runAllIntegrationTests() {
    std::cout << "Running Integration Tests for Refactored NLM Brain" << std::endl;
    std::cout << "====================================================" << std::endl;
    
    testBrainInitializationManagerIntegration();
    testBrainMemoryManagerIntegration();
    testBrainCognitionManagerIntegration();
    testBrainNeuromodulationManagerIntegration();
    testBrainDevelopmentManagerIntegration();
    testBrainPlasticityManagerIntegration();
    testSimulationLoopManagerIntegration();
    testCompleteIntegration();
    
    std::cout << std::endl;
    std::cout << "All integration tests passed! ✓" << std::endl;
    std::cout << "Refactored Brain successfully integrates all manager components." << std::endl;
}

} // namespace test_integration

// Main function for integration tests
int main() {
    test_integration::runAllIntegrationTests();
    return 0;
}
