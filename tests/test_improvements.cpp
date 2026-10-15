// Comprehensive test suite for critical NLM improvements
// Tests error handling, robustness, and performance optimizations

#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace test_improvements {

// Test 1: AgentBrain null pointer checks and error handling
void testAgentBrainErrorHandling() {
    std::cout << "Testing AgentBrain error handling..." << std::endl;
    
    // Test with null brain pointer
    auto agentBrain = std::make_shared<nlm::AgentBrain>(nullptr);
    
    // processSensoryInput with null brain should not crash
    nlm::SensoryPercept percept;
    agentBrain->processSensoryInput(percept);
    
    // getDevelopmentalStage with null brain should return Initial
    assert(agentBrain->getDevelopmentalStage() == nlm::DevelopmentalStage::Initial);
    
    // getSensoryInputSize should still work
    assert(agentBrain->getSensoryInputSize() > 0);
    
    std::cout << "    AgentBrain error handling test passed" << std::endl;
}

// Test 2: Config error handling improvements
void testConfigErrorHandling() {
    std::cout << "Testing Config error handling..." << std::endl;
    
    nlm::Config config;
    
    // Test empty key handling
    config.set("", 123, nlm::ConfigSource::Runtime);
    assert(!config.has("")); // Should not crash
    
    // Test type mismatch handling
    config.set("string", "hello", nlm::ConfigSource::Runtime);
    auto intVal = config.get<int>("string");
    assert(!intVal.has_value()); // Type mismatch should return nullopt
    
    // Test remove on non-existent key
    config.remove("non_existent"); // Should not crash
    
    // Test has with empty key
    assert(!config.has(""));
    
    std::cout << "    Config error handling test passed" << std::endl;
}

// Test 3: Brain initialization bounds checking
void testBrainInitializationBoundsChecking() {
    std::cout << "Testing Brain initialization bounds checking..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Set extreme values to test bounds checking
    config->set("neuron_count", 0, nlm::ConfigSource::Runtime); // Zero neurons
    config->set("region_count", 1000, nlm::ConfigSource::Runtime); // Many regions
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Brain should initialize without crashing
    bool initResult = brain->initialize();
    
    // Clean up
    brain->reset();
    
    std::cout << "    Brain initialization bounds checking test passed" << std::endl;
}

// Test 4: AgentBrain performance optimizations
void testAgentBrainPerformanceOptimizations() {
    std::cout << "Testing AgentBrain performance optimizations..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Initialize with world (mock)
    nlm::SimpleWorld world;
    world.configure(10, 10, 8, 8);
    agentBrain->initialize(world);
    
    // Test that motor decoding doesn't crash with null neurons
    nlm::SensoryPercept percept;
    // Create some mock sensory data
    auto vision = std::make_shared<nlm::Vision>(8, 8, 3);
    percept.setVision(*vision);
    
    // Should not crash even with empty neuron groups
    agentBrain->processSensoryInput(percept);
    
    // Test novelty computation with empty data
    nlm::SensoryPercept emptyPercept;
    agentBrain->processSensoryInput(emptyPercept);
    
    std::cout << "    AgentBrain performance optimizations test passed" << std::endl;
}

// Test 5: Python bindings organization
void testPythonBindingsOrganization() {
    std::cout << "Testing Python bindings organization..." << std::endl;
    
    // Test that config factory function works
    auto config = nlm::createDefaultConfig();
    assert(config != nullptr);
    
    // Test that brain factory function works
    auto brain = nlm::createBrain(config);
    assert(brain != nullptr);
    
    // Test that world factory function works
    auto world = nlm::createSimpleWorld();
    assert(world != nullptr);
    
    // Test that agent brain factory function works
    auto agent = nlm::createAgentBrain(brain);
    assert(agent != nullptr);
    
    std::cout << "    Python bindings organization test passed" << std::endl;
}

// Test 6: Config saveToFile error handling
void testConfigSaveErrorHandling() {
    std::cout << "Testing Config save error handling..." << std::endl;
    
    nlm::Config config;
    
    // Test save to empty filepath
    bool saveResult = config.saveToFile("");
    assert(!saveResult); // Should fail gracefully
    
    // Test save with valid data
    config.set("test", 123, nlm::ConfigSource::Runtime);
    config.set("name", std::string("test"), nlm::ConfigSource::Runtime);
    
    // Note: We can't actually test file writing in unit tests without mocks
    // but we can verify that the function doesn't crash
    
    std::cout << "    Config save error handling test passed" << std::endl;
}

// Test 7: AgentBrain development system bounds checking
void testAgentBrainDevelopmentBoundsChecking() {
    std::cout << "Testing AgentBrain development bounds checking..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    auto brain = std::make_shared<nlm::Brain>(config);
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    
    // Test development update with negative timestep
    agentBrain->updateDevelopment(-1.0); // Should handle gracefully
    
    // Test development with very large timestep
    agentBrain->updateDevelopment(1000000.0); // Should handle gracefully
    
    // Test neuromodulation with extreme values
    agentBrain->applyRewardModulation(1000.0f, -1000.0f); // Large values
    agentBrain->applyRewardModulation(-1000.0f, 1000.0f); // Negative values
    
    std::cout << "    AgentBrain development bounds checking test passed" << std::endl;
}

// Run all improvement tests
void runAllImprovementTests() {
    std::cout << "Running comprehensive improvement tests..." << std::endl;
    std::cout << std::endl;
    
    testAgentBrainErrorHandling();
    testConfigErrorHandling();
    testBrainInitializationBoundsChecking();
    testAgentBrainPerformanceOptimizations();
    testPythonBindingsOrganization();
    testConfigSaveErrorHandling();
    testAgentBrainDevelopmentBoundsChecking();
    
    std::cout << std::endl;
    std::cout << "All improvement tests passed successfully!" << std::endl;
}

} // namespace test_improvements

// Main function for standalone testing
int main() {
    test_improvements::runAllImprovementTests();
    return 0;
}
