// STDP Tests
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Random/Random.hpp"
#include "core/Exception.hpp"
#include <iostream>
#include <cmath>

namespace test_stdp {

void testSTDPCreation() {
    nlm::STDP stdp;
    
    if (stdp.getLTPWeight() != 0.01f) {
        throw nlm::NLMTestException("STDP creation failed: LTP weight should be 0.01f, got " + 
                                   std::to_string(stdp.getLTPWeight()));
    }
    if (stdp.getLTDWeight() != 0.012f) {
        throw nlm::NLMTestException("STDP creation failed: LTD weight should be 0.012f, got " + 
                                   std::to_string(stdp.getLTDWeight()));
    }
    if (stdp.getTimeConstant() != 20.0f) {
        throw nlm::NLMTestException("STDP creation failed: time constant should be 20.0f, got " + 
                                   std::to_string(stdp.getTimeConstant()));
    }
    
    std::cout << "    testSTDPCreation passed" << std::endl;
}

void testSTDPConfiguration() {
    nlm::STDP stdp;
    
    stdp.setLTPWeight(0.05f);
    stdp.setLTDWeight(0.06f);
    stdp.setTimeConstant(30.0f);
    
    if (stdp.getLTPWeight() != 0.05f) {
        throw nlm::NLMTestException("STDP configuration failed: LTP weight should be 0.05f, got " + 
                                   std::to_string(stdp.getLTPWeight()));
    }
    if (stdp.getLTDWeight() != 0.06f) {
        throw nlm::NLMTestException("STDP configuration failed: LTD weight should be 0.06f, got " + 
                                   std::to_string(stdp.getLTDWeight()));
    }
    if (stdp.getTimeConstant() != 30.0f) {
        throw nlm::NLMTestException("STDP configuration failed: time constant should be 30.0f, got " + 
                                   std::to_string(stdp.getTimeConstant()));
    }
    
    std::cout << "    testSTDPConfiguration passed" << std::endl;
}

void testSTDPConfigure() {
    nlm::STDP stdp;
    
    stdp.configure(0.03f, 0.035f, 25.0f);
    
    if (stdp.getLTPWeight() != 0.03f) {
        throw nlm::NLMTestException("STDP configure failed: LTP weight should be 0.03f, got " + 
                                   std::to_string(stdp.getLTPWeight()));
    }
    if (stdp.getLTDWeight() != 0.035f) {
        throw nlm::NLMTestException("STDP configure failed: LTD weight should be 0.035f, got " + 
                                   std::to_string(stdp.getLTDWeight()));
    }
    if (stdp.getTimeConstant() != 25.0f) {
        throw nlm::NLMTestException("STDP configure failed: time constant should be 25.0f, got " + 
                                   std::to_string(stdp.getTimeConstant()));
    }
    
    std::cout << "    testSTDPConfigure passed" << std::endl;
}

void testSTDPName() {
    nlm::STDP stdp;
    
    const char* name = stdp.getName();
    if (!name) {
        throw nlm::NLMTestException("STDP name failed: name is null");
    }
    if (std::string(name) != "STDP") {
        throw nlm::NLMTestException("STDP name failed: expected 'STDP', got '" + 
                                   std::string(name) + "'");
    }
    
    std::cout << "    testSTDPName passed" << std::endl;
}

void testSTDPPotentiation() {
    nlm::STDP stdp;
    nlm::RandomGenerator rng(42);
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Pre fires before post (positive dt) - should potentiate
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};  // ms
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // ms (10ms after pre)
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have increased due to pre-before-post
    if (finalWeight <= initialWeight) {
        throw nlm::NLMTestException("STDP potentiation failed: weight should increase, " + 
                                   "got " + std::to_string(finalWeight) + " (initial: " + 
                                   std::to_string(initialWeight) + ")");
    }
    
    std::cout << "    testSTDPPotentiation passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testSTDPDepression() {
    nlm::STDP stdp;
    
    // Create a synapse
    nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Post fires before pre (negative dt) - should depress
    std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0, 30.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // 5ms before pre
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    float finalWeight = synapse.getWeight();
    
    // Weight should have decreased due to post-before-pre
    if (finalWeight >= initialWeight) {
        throw nlm::NLMTestException("STDP depression failed: weight should decrease, " + 
                                   "got " + std::to_string(finalWeight) + " (initial: " + 
                                   std::to_string(initialWeight) + ")");
    }
    
    std::cout << "    testSTDPDepression passed (Δ=" 
              << (finalWeight - initialWeight) << ")" << std::endl;
}

void testSTDPEmptySpikes() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
    float initialWeight = synapse.getWeight();
    
    // Empty pre spikes - should not change
    std::vector<nlm::Timestamp> preSpikes;
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    if (synapse.getWeight() != initialWeight) {
        throw nlm::NLMTestException("STDP empty spikes failed: weight should not change with empty pre spikes, " + 
                                   "got " + std::to_string(synapse.getWeight()) + " (initial: " + 
                                   std::to_string(initialWeight) + ")");
    }
    
    // Empty post spikes - should not change
    preSpikes = {1.0, 2.0};
    postSpikes = {};
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    if (synapse.getWeight() != initialWeight) {
        throw nlm::NLMTestException("STDP empty spikes failed: weight should not change with empty post spikes, " + 
                                   "got " + std::to_string(synapse.getWeight()) + " (initial: " + 
                                   std::to_string(initialWeight) + ")");
    }
    
    std::cout << "    testSTDPEmptySpikes passed" << std::endl;
}

void testSTDPWeightBounds() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.99f);  // Near max
    
    // Very strong potentiation
    std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
    std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001);
    
    // Weight should be clamped to max
    if (synapse.getWeight() > 1.0f) {
        throw nlm::NLMTestException("STDP weight bounds failed: weight should not exceed 1.0, " + 
                                   "got " + std::to_string(synapse.getWeight()));
    }
    
    std::cout << "    testSTDPWeightBounds passed" << std::endl;
}

void runAll() {
    std::cout << "Running STDP tests..." << std::endl;
    testSTDPCreation();
    testSTDPConfiguration();
    testSTDPConfigure();
    testSTDPName();
    testSTDPPotentiation();
    testSTDPDepression();
    testSTDPEmptySpikes();
    testSTDPWeightBounds();
}

} // namespace test_stdp
