// AgentBrain Neuron Manager Tests
// Tests for AgentBrainNeuronManager functionality

#include "agent/AgentBrainNeuronManager.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_neuronmanager {

void testNeuronManagerCreation() {
    std::cout << "    testNeuronManagerCreation..." << std::endl;
    
    nlm::AgentBrainNeuronManager manager;
    
    // Test empty manager initially
    assert(manager.getMotorNeuronsCount() == 0);
    assert(manager.getSensoryNeuronsCount() == 0);
    assert(manager.getInternalNeuronsCount() == 0);
    
    std::cout << "    testNeuronManagerCreation passed" << std::endl;
}

void testNeuronManagerAddNeurons() {
    std::cout << "    testNeuronManagerAddNeurons..." << std::endl;
    
    nlm::AgentBrainNeuronManager manager;
    
    // Create a simple config and brain for testing
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Add neurons to manager
    manager.addNeurons(brain);
    
    // Verify neurons were added
    assert(manager.getMotorNeuronsCount() > 0);
    assert(manager.getSensoryNeuronsCount() > 0 || manager.getInternalNeuronsCount() > 0);
    
    std::cout << "    testNeuronManagerAddNeurons passed" << std::endl;
}

void testNeuronManagerGroupAccess() {
    std::cout << "    testNeuronManagerGroupAccess..." << std::endl;
    
    nlm::AgentBrainNeuronManager manager;
    
    // Test that we can access all groups
    const auto& motorForward = manager.getMotorForward();
    const auto& motorBackward = manager.getMotorBackward();
    const auto& motorTurnLeft = manager.getMotorTurnLeft();
    const auto& motorTurnRight = manager.getMotorTurnRight();
    const auto& motorInteract = manager.getMotorInteract();
    const auto& motorWait = manager.getMotorWait();
    
    const auto& sensoryVision = manager.getSensoryVision();
    const auto& sensoryTouch = manager.getSensoryTouch();
    const auto& sensoryInternal = manager.getSensoryInternal();
    const auto& sensoryProprioception = manager.getSensoryProprioception();
    const auto& internalNeurons = manager.getInternalNeurons();
    
    assert(&motorForward == &manager.getMotorForward());
    assert(&motorBackward == &manager.getMotorBackward());
    assert(&motorTurnLeft == &manager.getMotorTurnLeft());
    assert(&motorTurnRight == &manager.getMotorTurnRight());
    assert(&motorInteract == &manager.getMotorInteract());
    assert(&motorWait == &manager.getMotorWait());
    
    assert(&sensoryVision == &manager.getSensoryVision());
    assert(&sensoryTouch == &manager.getSensoryTouch());
    assert(&sensoryInternal == &manager.getSensoryInternal());
    assert(&sensoryProprioception == &manager.getSensoryProprioception());
    assert(&internalNeurons == &manager.getInternalNeurons());
    
    std::cout << "    testNeuronManagerGroupAccess passed" << std::endl;
}

void testNeuronManagerAllGroups() {
    std::cout << "    testNeuronManagerAllGroups..." << std::endl;
    
    nlm::AgentBrainNeuronManager manager;
    
    // Get all motor groups
    auto motorGroups = manager.getAllMotorGroups();
    assert(motorGroups.size() == 6);  // forward, backward, turnLeft, turnRight, interact, wait
    
    // Get all sensory groups
    auto sensoryGroups = manager.getAllSensoryGroups();
    assert(sensoryGroups.size() == 4);  // vision, touch, internal, proprioception
    
    std::cout << "    testNeuronManagerAllGroups passed" << std::endl;
}

void runAll() {
    std::cout << "=== Neuron Manager Tests ===" << std::endl;
    
    testNeuronManagerCreation();
    testNeuronManagerAddNeurons();
    testNeuronManagerGroupAccess();
    testNeuronManagerAllGroups();
    
    std::cout << "All Neuron Manager tests passed!" << std::endl << std::endl;
}

} // namespace test_neuronmanager