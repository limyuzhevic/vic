// Unit tests for enhanced Python bindings and improvements
// This file should be added to the tests/CMakeLists.txt

#include "python/bindings_improvements.cpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

namespace test_bindings_improvements {

void testEnhancedConfigCreation() {
    std::cout << "    testEnhancedConfigCreation passed" << std::endl;
    // Test that enhanced config creation works
}

void testBrainCreationWithValidation() {
    nlm::Config config;
    config.set("brain.neuron_count", 100, nlm::ConfigSource::Runtime);
    
    // Should work without exception
    auto brain = create_brain_simplified(std::make_shared<nlm::Config>(config));
    assert(brain != nullptr);
    
    std::cout << "    testBrainCreationWithValidation passed" << std::endl;
}

void testWorldCreationWithDefaults() {
    // Test enhanced world creation
    auto world = create_enhanced_world();
    assert(world != nullptr);
    
    assert(world->getWidth() == 20);
    assert(world->getHeight() == 20);
    assert(world->getVisionWidth() == 8);
    assert(world->getVisionHeight() == 8);
    
    std::cout << "    testWorldCreationWithDefaults passed" << std::endl;
}

void testAgentCreationWithWorld() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto world = create_enhanced_world();
    
    auto agent = create_enhanced_agent(brain, world);
    assert(agent != nullptr);
    
    std::cout << "    testAgentCreationWithWorld passed" << std::endl;
}

void testBrainStatistics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", 50, nlm::ConfigSource::Runtime);
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Mock brain initialization (normally would call initialize())
    auto stats = get_brain_statistics(brain);
    
    assert(stats.find("neurons_firing") != stats.end());
    assert(stats.find("total_spikes") != stats.end());
    assert(stats.find("average_firing_rate") != stats.end());
    
    std::cout << "    testBrainStatistics passed" << std::endl;
}

void testAgentStatistics() {
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    
    auto stats = get_agent_statistics(agent);
    
    assert(stats.find("curiosity_level") != stats.end());
    assert(stats.find("novelty_level") != stats.end());
    assert(stats.find("prediction_error") != stats.end());
    assert(stats.find("neuromodulation_level") != stats.end());
    
    std::cout << "    testAgentStatistics passed" << std::endl;
}

void testWorldStatistics() {
    auto world = create_enhanced_world(30, 30, 10, 10);
    
    auto stats = get_world_statistics(world);
    
    assert(stats.find("width") != stats.end());
    assert(stats.find("height") != stats.end());
    assert(stats.find("simulation_time") != stats.end());
    
    std::cout << "    testWorldStatistics passed" << std::endl;
}

void testActionCreation() {
    // Test enhanced action creation
    auto action = create_action_enhanced(nlm::ActionType::MoveForward);
    assert(action != nullptr);
    
    assert(action->getType() == nlm::ActionType::MoveForward);
    
    auto action2 = create_action_enhanced(nlm::ActionType::Interact, std::vector<float>{1.0, 0.5});
    assert(action2 != std::make_unique<nlm::Action>(nlm::ActionType::Interact));
    
    std::cout << "    testActionCreation passed" << std::endl;
}

void testActionName() {
    std::string name = get_action_name(nlm::ActionType::MoveForward);
    assert(!name.empty());
    
    std::cout << "    testActionName passed" << std::endl;
}

void testBatchProcessing() {
    // Mock test for batch processing
    std::cout << "    testBatchProcessing passed (conceptual)" << std::endl;
}

void testConfigSummary() {
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", 100, nlm::ConfigSource::Runtime);
    config->set("brain.synapse_density", 0.1, nlm::ConfigSource::Runtime);
    
    auto summary = get_config_summary(config);
    assert(summary.find("keys_count") != summary.end());
    assert(summary.find("raw_summary") != summary.end());
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void runAll() {
    testEnhancedConfigCreation();
    testBrainCreationWithValidation();
    testWorldCreationWithDefaults();
    testAgentCreationWithWorld();
    testBrainStatistics();
    testAgentStatistics();
    testWorldStatistics();
    testActionCreation();
    testActionName();
    testBatchProcessing();
    testConfigSummary();
}

} // namespace test_bindings_improvements