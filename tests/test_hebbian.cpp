// Hebbian Plasticity Tests
// Comprehensive testing of Hebbian learning mechanisms

#include "brain/Synapse.hpp"
#include "plasticity/Hebbian.hpp"
#include "core/Random/Random.hpp"
#include "core/Exception.hpp"
#include <iostream>
#include <cmath>

namespace test_hebbian {

void testHebbianCreation() {
    nlm::Hebbian hebbian;
    
    if (hebbian.getLearningRate() != 0.01f) {
        throw nlm::NLMTestException("Hebbian creation failed: learning rate should be 0.01f, got " + 
                                   std::to_string(hebbian.getLearningRate()));
    }
    if (hebbian.getMaxWeight() != 1.0f) {
        throw nlm::NLMTestException("Hebbian creation failed: max weight should be 1.0f, got " + 
                                   std::to_string(hebbian.getMaxWeight()));
    }
    if (hebbian.getMinWeight() != -1.0f) {
        throw nlm::NLMTestException("Hebbian creation failed: min weight should be -1.0f, got " + 
                                   std::to_string(hebbian.getMinWeight()));
    }
    
    if (std::string(hebbian.getName()) != "Hebbian") {
        throw nlm::NLMTestException("Hebbian creation failed: name should be 'Hebbian', got '" + 
                                   std::string(hebbian.getName()) + "'");
    }
    
    std::cout << "    testHebbianCreation passed" << std::endl;
}

void testHebbianConfiguration() {
    nlm::Hebbian hebbian;
    
    hebbian.setLearningRate(0.05f);
    hebbian.setMaxWeight(2.0f);
    hebbian.setMinWeight(-2.0f);
    
    if (hebbian.getLearningRate() != 0.05f) {
        throw nlm::NLMTestException("Hebbian configuration failed: learning rate should be 0.05f, got " + 
                                   std::to_string(hebbian.getLearningRate()));
    }
    if (hebbian.getMaxWeight() != 2.0f) {
        throw nlm::NLMTestException("Hebbian configuration failed: max weight should be 2.0f, got " + 
                                   std::to_string(hebbian.getMaxWeight()));
    }
    if (hebbian.getMinWeight() != -2.0f) {
        throw nlm::NLMTestException("Hebbian configuration failed: min weight should be -2.0f, got " + 
                                   std::to_string(hebbian.getMinWeight()));
    }
    
    std::cout << "    testHebbianConfiguration passed" << std::endl;
}

void testHebbianName() {
    nlm::Hebbian hebbian;
    
    const char* name = hebbian.getName();
    if (!name) {
        throw nlm::NLMTestException("Hebbian name failed: name is null");
    }
    if (std::string(name) != "Hebbian") {
        throw nlm::NLMTestException("Hebbian name failed: expected 'Hebbian', got '" + 
                                   std::string(name) + "'");
    }
    
    std::cout << "    testHebbianName passed" << std::endl;
}

void testHebbianPotentiation() {
    nlm::Hebbian hebbian;
    hebbian.setLearningRate(0.01f);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post (positive correlation) - should potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // ms (5ms after pre)
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to pre-before-post correlation
    if (finalWeight <= initialWeight) {
        throw nlm::NLMTestException("Hebbian potentiation failed: weight should increase, " + 
                                   "got " + std::to_string(finalWeight) + " (initial: " + 
                                   std::to_string(initialWeight) + ")");
    }
    
    std::cout << "    testHebbianPotentiation passed (Δ=" << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianDepression() {
    nlm::Hebbian hebbian;
    hebbian.setLearningRate(0.01f);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Post fires before pre (negative correlation) - should depress
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // 5ms before pre
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased due to post-before-pre correlation
    if (finalWeight >= initialWeight) {
        throw nlm::NLMTestException("Hebbian depression failed: weight should decrease, " + 
                                   "got " + std::to_string(finalWeight) + " (initial: " + 
                                   std::to_string(initialWeight) + ")");
    }
    
    std::cout << "    testHebbianDepression passed (Δ=" << (finalWeight - initialWeight) << ")" << std::endl;
}

void testHebbianEmptySpikes() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes - should not change
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    if (synapse.getWeight() != initialWeight) {
        throw nlm::NLMTestException("Hebbian empty spikes failed: weight should not change with empty pre spikes, " + 
                                   "got " + std::to_string(synapse.getWeight()) + " (initial: " + 
                                   std::to_string(initialWeight) + ")");
    }
    
    // Empty post spikes - should not change
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    if (synapse.getWeight() != initialWeight) {
        throw nlm::NLMTestException("Hebbian empty spikes failed: weight should not change with empty post spikes, " + 
                                   "got " + std::to_string(synapse.getWeight()) + " (initial: " + 
                                   std::to_string(initialWeight) + ")");
    }
    
    std::cout << "    testHebbianEmptySpikes passed" << std::endl;
}

void testHebbianNullSynapse() {
    nlm::Hebbian hebbian;
    
    // Null synapse - should not crash
    hebbian.update(nullptr, {1.0, 2.0}, {3.0, 4.0}, 0.001);
    
    std::cout << "    testHebbianNullSynapse passed" << std::endl;
}

void testHebbianWeightBounds() {
    nlm::Hebbian hebbian;
    hebbian.setMaxWeight(1.0f);
    hebbian.setMinWeight(-1.0f);
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    if (synapse.getWeight() > 1.0f) {
        throw nlm::NLMTestException("Hebbian weight bounds failed: weight should not exceed 1.0, " + 
                                   "got " + std::to_string(synapse.getWeight()));
    }
    
    std::cout << "    testHebbianWeightBounds passed" << std::endl;
}

void testHebbianExtremeCorrelation() {
    nlm::Hebbian hebbian;
    hebbian.setLearningRate(0.1f); // Higher learning rate for stronger effect
    
    nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.0f);  // Start at minimum
    
    // Many correlated spikes - strong potentiation
    std::vector<nlm::Timestamp> preSpikes(100, 0.0);  // 100 spikes at time 0
    std::vector<nlm::Timestamp> postSpikes(100, 5.0); // 100 spikes at time 5
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased significantly
    if (finalWeight <= 0.0f) {
        throw nlm::NLMTestException("Hebbian extreme correlation failed: weight should increase with many correlated spikes, " + 
                                   "got " + std::to_string(finalWeight));\n    }
    
    std::cout << "    testHebbianExtremeCorrelation passed (final weight: " << finalWeight << ")" << std::endl;
}

void testHebbianTimeWindow() {
    nlm::Hebbian hebbian;
    
    nlm::Synapse synapse(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    float initialWeight = synapse.getWeight();
    
    // Spikes within correlation window (100ms)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 50.0};
    std::vector<nlm::Timestamp> postSpikes = {20.0, 70.0};  // Within ±100ms window
    
    hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
    float weightWithinWindow = synapse.getWeight();
    
    // Reset and test with spikes outside window
    synapse.setWeight(initialWeight);
    std::vector<nlm::Timestamp> preSpikes2 = {0.0};
    std::vector<nlm::Timestamp> postSpikes2 = {200.0};  // Outside correlation window
    
    hebbian.update(&synapse, preSpikes2, postSpikes2, 0.001);
    float weightOutsideWindow = synapse.getWeight();
    
    // Weight should be higher with spikes within window
    if (weightWithinWindow <= weightOutsideWindow) {
        throw nlm::NLMTestException("Hebbian time window failed: weight should be higher with spikes within correlation window, " + 
                                   "within: " + std::to_string(weightWithinWindow) + ", outside: " + 
                                   std::to_string(weightOutsideWindow));
    }
    
    std::cout << "    testHebbianTimeWindow passed (within: " << weightWithinWindow 
              << ", outside: " << weightOutsideWindow << ")" << std::endl;
}

void runAll() {
    std::cout << "Running Hebbian Plasticity tests..." << std::endl;
    testHebbianCreation();
    testHebbianConfiguration();
    testHebbianName();
    testHebbianPotentiation();
    testHebbianDepression();
    testHebbianEmptySpikes();
    testHebbianNullSynapse();
    testHebbianWeightBounds();
    testHebbianExtremeCorrelation();
    testHebbianTimeWindow();
}

} // namespace test_hebbian
