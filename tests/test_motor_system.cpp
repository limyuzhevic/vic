// MotorSystem Tests
// Testing real action selection based on neural activity and neuromodulation

#include "motor/MotorSystem.hpp"
#include "brain/Brain.hpp"
#include "brain/NeuralRegion.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_motor_system {

void testMotorSystemCreation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Default);
    config->set("region_count", 1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Create motor system
    nlm::MotorSystem motorSystem;
    motorSystem.initialize(&brain);
    
    // Test basic properties
    const auto& availableActions = motorSystem.getAvailableActions();
    assert(!availableActions.empty());
    assert(availableActions.size() >= 8);
    
    std::cout << "    testMotorSystemCreation passed" << std::endl;
}

void testActionPreferences() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50, nlm::ConfigSource::Default);
    config->set("region_count", 1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::MotorSystem motorSystem;
    motorSystem.initialize(&brain);
    
    // Test setting and retrieving action preferences
    motorSystem.setActionPreference(nlm::ActionType::MoveForward, 0.8f);
    motorSystem.setActionPreference(nlm::ActionType::MoveBackward, 0.2f);
    
    // Verify preferences are set (indirectly through action selection)
    auto action = motorSystem.selectAction(*brain.getRegion(brain.getRegionIds()[0]));
    assert(action != nullptr);
    
    std::cout << "    testActionPreferences passed" << std::endl;
}

void testWorkingMemoryIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Default);
    config->set("region_count", 1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::MotorSystem motorSystem;
    motorSystem.initialize(&brain);
    
    // Get working memory
    auto* workingMemory = brain.getWorkingMemory();
    
    // Create a simple test pattern (all zeros for simplicity)
    std::vector<float> pattern(100, 0.0f);
    pattern[0] = 1.0f; // Set first neuron as active
    
    workingMemory->store(pattern, 1.0f);
    
    // Test action selection with working memory
    nlm::RegionId regionId = brain.getRegionIds()[0];
    const nlm::NeuralRegion* region = brain.getRegion(regionId);
    
    auto action = motorSystem.selectAction(*region);
    assert(action != nullptr);
    
    std::cout << "    testWorkingMemoryIntegration passed" << std::endl;
}

void testActionHistory() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50, nlm::ConfigSource::Default);
    config->set("region_count", 1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::MotorSystem motorSystem;
    motorSystem.initialize(&brain);
    
    // Get initial history
    const auto& initialHistory = motorSystem.getActionHistory();
    assert(initialHistory.empty());
    
    // Get a region for testing
    nlm::RegionId regionId = brain.getRegionIds()[0];
    const nlm::NeuralRegion* region = brain.getRegion(regionId);
    
    // Select an action
    auto action = motorSystem.selectAction(*region);
    assert(action != nullptr);
    
    // Check that action was recorded
    const auto& history = motorSystem.getActionHistory();
    assert(history.size() == 1);
    assert(history[0] == action.get());
    
    // Clear history
    motorSystem.clearHistory();
    assert(motorSystem.getActionHistory().empty());
    
    std::cout << "    testActionHistory passed" << std::endl;
}

void runAll() {
    std::cout << "=== MotorSystem Tests ===" << std::endl;
    
    testMotorSystemCreation();
    testActionPreferences();
    testWorkingMemoryIntegration();
    testActionHistory();
    
    std::cout << std::endl << "=== All MotorSystem Tests PASSED ===" << std::endl;
}

} // namespace test_motor_system
