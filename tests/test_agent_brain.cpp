// Agent Brain Tests
// Tests for AgentBrain functionality including sensory processing, motor decoding, and agent-brain interface

#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "sensory/InternalSignals.hpp"
#include "motor/MotorSystem.hpp"
#include "TestInfrastructure.hpp"
#include "TestUtilities.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>

namespace test_agent_brain {

void testAgentBrainCreation() {
    nlm::AgentBrain agentBrain;
    
    // Test basic properties
    auto percept = agentBrain.getPercept();
    assert(percept != nullptr);
    
    auto config = agentBrain.getConfig();
    assert(config != nullptr);
    
    std::cout << "    testAgentBrainCreation passed" << std::endl;
}

void testAgentBrainSensoryProcessing() {
    nlm::AgentBrain agentBrain;
    
    // Create test sensory percepts
    nlm::SensoryPercept visualPercept;
    visualPercept.type = "vision";
    visualPercept.data = "object_detected:5,0.8"; // object:distance,confidence
    visualPercept.timestamp = 100;
    
    nlm::SensoryPercept audioPercept;
    audioPercept.type = "audio";
    audioPercept.data = "sound:alert"; // sound type
    audioPercept.timestamp = 101;
    
    nlm::SensoryPercept internalPercept;
    internalPercept.type = "internal";
    internalPercept.data = "hunger:0.3,thirst:0.7"; // need:level
    internalPercept.timestamp = 102;
    
    // Process sensory percepts
    agentBrain.receivePercept(visualPercept);
    agentBrain.receivePercept(audioPercept);
    agentBrain.receivePercept(internalPercept);
    
    // Check that percepts are stored
    auto currentPercept = agentBrain.getPercept();
    assert(currentPercept != nullptr);
    assert(currentPercept->type == "internal"); // Should have updated with latest
    
    std::cout << "    testAgentBrainSensoryProcessing passed" << std::endl;
}

void testAgentBrainMotorDecoding() {
    nlm::AgentBrain agentBrain;
    
    // Create test motor action
    nlm::Action action;
    action.type = "move";
    action.parameters = "10,5,0"; // x,y,z coordinates
    action.timestamp = 100;
    action.source = "brain";
    
    // Apply action through motor decoding
    agentBrain.applyAction(action);
    
    // Verify action was processed
    auto currentPercept = agentBrain.getPercept();
    assert(currentPercept != nullptr);
    assert(currentPercept->type == "internal"); // May update based on action
    
    std::cout << "    testAgentBrainMotorDecoding passed" << std::endl;
}

void testAgentBrainIntegrationWithBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Create agent brain
    nlm::AgentBrain agentBrain;
    
    // Set brain reference
    agentBrain.setBrain(&brain);
    assert(agentBrain.getBrain() == &brain);
    
    // Create test percepts
    nlm::SensoryPercept visualPercept;
    visualPercept.type = "vision";
    visualPercept.data = "object_detected:5,0.9";
    visualPercept.timestamp = 100;
    
    // Process percept through agent brain
    agentBrain.receivePercept(visualPercept);
    
    // Brain should have received sensory input
    auto* brainPercept = agentBrain.getPercept();
    assert(brainPercept != nullptr);
    
    std::cout << "    testAgentBrainIntegrationWithBrain passed" << std::endl;
}

void testAgentBrainPerceptStorage() {
    nlm::AgentBrain agentBrain;
    
    // Store multiple percepts
    std::vector<nlm::SensoryPercept> percepts;
    
    for (int i = 0; i < 10; ++i) {
        nlm::SensoryPercept percept;
        percept.type = "test_sensor";
        percept.data = "value:" + std::to_string(i);
        percept.timestamp = i * 10;
        
        agentBrain.receivePercept(percept);
        percepts.push_back(percept);
    }
    
    // Verify current percept is latest
    auto currentPercept = agentBrain.getPercept();
    assert(currentPercept != nullptr);
    assert(currentPercept->type == "test_sensor");
    assert(currentPercept->data == "value:9");
    assert(currentPercept->timestamp == 90);
    
    std::cout << "    testAgentBrainPerceptStorage passed" << std::endl;
}

void testAgentBrainPerceptProcessing() {
    nlm::AgentBrain agentBrain;
    
    // Create percept with structured data
    nlm::SensoryPercept percept;
    percept.type = "complex";
    percept.data = "temp:25,pressure:101.3,humidity:60"; // temp,pressure,humidity
    percept.timestamp = 100;
    
    // Process percept
    agentBrain.receivePercept(percept);
    
    // Percept should be accessible
    auto processedPercept = agentBrain.getPercept();
    assert(processedPercept != nullptr);
    assert(processedPercept->type == "complex");
    assert(processedPercept->data == "temp:25,pressure:101.3,humidity:60");
    
    std::cout << "    testAgentBrainPerceptProcessing passed" << std::endl;
}

void testAgentBrainActionOutput() {
    nlm::AgentBrain agentBrain;
    
    // Create and apply actions
    std::vector<nlm::Action> actions;
    
    for (int i = 0; i < 5; ++i) {
        nlm::Action action;
        action.type = "move_to_point";
        action.parameters = std::to_string(i * 10) + "," + std::to_string(i * 5) + ",0";
        action.timestamp = i * 20;
        action.source = "agent";
        
        agentBrain.applyAction(action);
        actions.push_back(action);
    }
    
    // Percept should reflect the actions
    auto currentPercept = agentBrain.getPercept();
    assert(currentPercept != nullptr);
    
    std::cout << "    testAgentBrainActionOutput passed" << std::endl;
}

void testAgentBrainIntegrationWithMultiplePercepts() {
    nlm::AgentBrain agentBrain;
    
    // Create multiple sensor types
    nlm::SensoryPercept visionPercept;
    visionPercept.type = "vision";
    visionPercept.data = "object:5,confidence:0.9";
    visionPercept.timestamp = 100;
    
    nlm::SensoryPercept audioPercept;
    audioPercept.type = "audio";
    audioPercept.data = "sound:alert_level_3";
    audioPercept.timestamp = 101;
    
    nlm::SensoryPercept internalPercept;
    internalPercept.type = "internal";
    internalPercept.data = "energy:0.3,satiety:0.8";
    internalPercept.timestamp = 102;
    
    // Process all percepts
    agentBrain.receivePercept(visionPercept);
    agentBrain.receivePercept(audioPercept);
    agentBrain.receivePercept(internalPercept);
    
    // Latest percept should be internal
    auto currentPercept = agentBrain.getPercept();
    assert(currentPercept != nullptr);
    assert(currentPercept->type == "internal");
    assert(currentPercept->data == "energy:0.3,satiety:0.8");
    
    std::cout << "    testAgentBrainIntegrationWithMultiplePercepts passed" << std::endl;
}

void testAgentBrainPerceptPriority() {
    nlm::AgentBrain agentBrain;
    
    // Test that newer percepts override older ones
    nlm::SensoryPercept oldPercept;
    oldPercept.type = "sensor1";
    oldPercept.data = "old_data";
    oldPercept.timestamp = 50;
    
    agentBrain.receivePercept(oldPercept);
    
    nlm::SensoryPercept newPercept;
    newPercept.type = "sensor2";
    newPercept.data = "new_data";
    newPercept.timestamp = 100;
    
    agentBrain.receivePercept(newPercept);
    
    // Current percept should be new one
    auto currentPercept = agentBrain.getPercept();
    assert(currentPercept != nullptr);
    assert(currentPercept->type == "sensor2");
    assert(currentPercept->data == "new_data");
    
    std::cout << "    testAgentBrainPerceptPriority passed" << std::endl;
}

void testAgentBrainStress() {
    nlm::AgentBrain agentBrain;
    
    // Rapid percept and action processing
    for (int i = 0; i < 100; ++i) {
        // Create and process percept
        nlm::SensoryPercept percept;
        percept.type = "stress_sensor";
        percept.data = "iteration:" + std::to_string(i);
        percept.timestamp = i;
        
        agentBrain.receivePercept(percept);
        
        // Create and process action
        nlm::Action action;
        action.type = "stress_action";
        action.parameters = std::to_string(i);
        action.timestamp = i + 10;
        action.source = "stress_test";
        
        agentBrain.applyAction(action);
        
        // Verify system is still responsive
        auto currentPercept = agentBrain.getPercept();
        assert(currentPercept != nullptr);
    }
    
    std::cout << "    testAgentBrainStress passed" << std::endl;
}

void runAll() {
    nlm_test::TestRunner runner(true);
    
    runner.runTest("Agent Brain Creation", testAgentBrainCreation);
    runner.runTest("Agent Brain Sensory Processing", testAgentBrainSensoryProcessing);
    runner.runTest("Agent Brain Motor Decoding", testAgentBrainMotorDecoding);
    runner.runTest("Agent Brain Integration With Brain", testAgentBrainIntegrationWithBrain);
    runner.runTest("Agent Brain Percept Storage", testAgentBrainPerceptStorage);
    runner.runTest("Agent Brain Percept Processing", testAgentBrainPerceptProcessing);
    runner.runTest("Agent Brain Action Output", testAgentBrainActionOutput);
    runner.runTest("Agent Brain Integration With Multiple Percepts", testAgentBrainIntegrationWithMultiplePercepts);
    runner.runTest("Agent Brain Percept Priority", testAgentBrainPerceptPriority);
    runner.runTest("Agent Brain Stress Test", testAgentBrainStress);
    
    runner.runAll();
}

} // namespace test_agent_brain
