// Plasticity Tests
// Testing improved Hebbian and STDP implementations

#include "plasticity/PlasticityRule.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/STDP.hpp"
#include "brain/Synapse.hpp"
#include "brain/Neuron.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_plasticity {

void testHebbianRuleCreation() {
    nlm::HebbianRule hebbian;
    assert(hebbian.isEnabled());
    assert(hebbian.getName() != nullptr);
    assert(std::string(hebbian.getName()) == "Hebbian");
    
    // Check default learning rate
    float learningRate = hebbian.getLearningRate();
    assert(learningRate == 0.01f);
    
    std::cout << "    testHebbianRuleCreation passed" << std::endl;
}

void testHebbianRuleParameters() {
    nlm::HebbianRule hebbian;
    
    // Test setting learning rate
    hebbian.setLearningRate(0.05f);
    assert(hebbian.getLearningRate() == 0.05f);
    
    // Test clamping of learning rate
    hebbian.setLearningRate(2.0f);  // Should be clamped to 1.0f
    assert(hebbian.getLearningRate() == 1.0f);
    
    hebbian.setLearningRate(-0.5f);  // Should be clamped to 0.0f
    assert(hebbian.getLearningRate() == 0.0f);
    
    // Test setting max weight
    hebbian.setMaxWeight(5.0f);
    assert(hebbian.getMaxWeight() == 5.0f);
    
    // Test clamping of max weight
    hebbian.setMaxWeight(20.0f);  // Should be clamped to 10.0f
    assert(hebbian.getMaxWeight() == 10.0f);
    
    std::cout << "    testHebbianRuleParameters passed" << std::endl;
}

void testHebbianRuleUpdate() {
    nlm::HebbianRule hebbian;
    
    // Create a mock synapse (in real implementation, this would be a real synapse)
    class MockSynapse {
    public:
        float weight = 0.5f;
        float getWeight() const { return weight; }
        void addToWeight(float delta) { weight += delta; }
    };
    
    MockSynapse synapse;
    
    // Create spike histories
    std::vector<float> preSpikes = {10.0f, 15.0f, 20.0f};
    std::vector<float> postSpikes = {11.0f, 16.0f, 21.0f};
    
    // Update with Hebbian rule
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001f);
    
    // The synapse weight should have changed (positive correlation)
    assert(synapse.getWeight() > 0.5f);
    
    std::cout << "    testHebbianRuleUpdate passed" << std::endl;
}

void testSTDPRuleCreation() {
    nlm::STDP stdp;
    assert(stdp.isEnabled());
    assert(stdp.getName() != nullptr);
    assert(std::string(stddp.getName()) == "STDP");
    
    std::cout << "    testSTDPRuleCreation passed" << std::endl;
}

void testSTDPRuleParameters() {
    nlm::STDP stdp;
    
    // Test setting parameters
    stdp.setLTPWeight(0.02f);
    assert(stdp.getLTPWeight() == 0.02f);
    
    stdp.setLTDWeight(0.015f);
    assert(stdp.getLTDWeight() == 0.015f);
    
    stdp.setTimeConstant(25.0f);
    assert(stdp.getTimeConstant() == 25.0f);
    
    // Test clamping
    stdp.setLTPWeight(2.0f);  // Should be clamped to 1.0f
    assert(stdp.getLTPWeight() == 1.0f);
    
    std::cout << "    testSTDPRuleParameters passed" << std::endl;
}

void testSTDPRuleUpdate() {
    nlm::STDP stdp;
    
    // Create a mock synapse
    class MockSynapse {
    public:
        float weight = 0.5f;
        float getWeight() const { return weight; }
        void addToWeight(float delta) { weight += delta; }
        float getEfficacy() const { return 1.0f; }
        float getEligibilityTrace() const { return 0.0f; }
        void setEligibilityTrace(float trace) { (void)trace; }
    };
    
    MockSynapse synapse;
    
    // Create spike histories with pre-before-post timing (should cause potentiation)
    std::vector<float> preSpikes = {10.0f, 15.0f};  // Pre-synaptic spikes
    std::vector<float> postSpikes = {11.0f, 16.0f}; // Post-synaptic spikes, both 1ms after pre
    
    // Update with STDP rule
    stdp.update(&synapse, preSpikes, postSpikes, 0.001f);
    
    // The synapse weight should have increased (potentiation)
    assert(synapse.getWeight() > 0.5f);
    
    std::cout << "    testSTDPRuleUpdate passed" << std::endl;
}

void testMemoryManagement() {
    // Test move assignment operator memory safety
    nlm::Neuron neuron1(nlm::NeuronId(1));
    nlm::Neuron neuron2(std::move(neuron1));  // Should not crash
    
    // Test move assignment operator memory safety
    nlm::Neuron neuron3(nlm::NeuronId(2));
    neuron3 = std::move(neuron2);  // Should not crash
    
    // Test with null pointer scenarios
    nlm::HebbianRule hebbian1;
    nlm::HebbianRule hebbian2;
    
    // These should not crash even if internal pointers are null
    hebbian1.setLearningRate(0.1f);
    hebbian2.setLearningRate(0.2f);
    
    std::cout << "    testMemoryManagement passed" << std::endl;
}

void testErrorHandling() {
    // Test error handling in config parser
    nlm::Config config;
    
    // Test loading invalid JSON file
    bool loadSuccess = config.loadFromFile("/tmp/invalid.json");
    assert(!loadSuccess);
    
    // Test setting invalid values
    config.set("test_int", "invalid", nlm::ConfigSource::Runtime);
    auto intVal = config.get<int>("test_int");
    assert(!intVal.has_value());
    
    // Test setting valid values
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    auto validInt = config.get<int>("test_int");
    assert(validInt.has_value());
    assert(*validInt == 42);
    
    std::cout << "    testErrorHandling passed" << std::endl;
}

void runAll() {
    testHebbianRuleCreation();
    testHebbianRuleParameters();
    testHebbianRuleUpdate();
    testSTDPRuleCreation();
    testSTDPRuleParameters();
    testSTDPRuleUpdate();
    testMemoryManagement();
    testErrorHandling();
    
    std::cout << std::endl;
    std::cout << "=== All Plasticity Tests PASSED ===" << std::endl;
}

} // namespace test_plasticity