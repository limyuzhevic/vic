// STDP Test Implementation
// Comprehensive tests for STDP plasticity rule

#include "plasticity/STDP.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace nlm {
namespace test {

class STDPTest : public BaseTest {
public:
    STDPTest() : testName("STDP Tests") {}
    
    void run() override {
        std::cout << "Running STDP tests..." << std::endl;
        
        testSTDPCreation();
        testSTDPConfiguration();
        testSTDPPotentiation();
        testSTDPDepression();
        testSTDPEmptySpikes();
        testSTDPWeightBounds();
        testSTDPParameterValidation();
        testSTDPRandomized();
        testSTDPDeterministic();
        
        std::cout << "STDP tests completed successfully!" << std::endl;
    }
    
private:
    void testSTDPCreation() {
        nlm::STDP stdp;
        
        assert(stdp.getLTPWeight() == 0.01f);
        assert(stdp.getLTDWeight() == 0.012f);
        assert(stdp.getTimeConstant() == 20.0f);
        
        std::cout << "  ✓ testSTDPCreation passed" << std::endl;
    }
    
    void testSTDPConfiguration() {
        nlm::STDP stdp;
        
        stdp.setLTPWeight(0.05f);
        stdp.setLTDWeight(0.06f);
        stdp.setTimeConstant(30.0f);
        
        assert(stdp.getLTPWeight() == 0.05f);
        assert(stdp.getLTDWeight() == 0.06f);
        assert(stdp.getTimeConstant() == 30.0f);
        
        std::cout << "  ✓ testSTDPConfiguration passed" << std::endl;
    }
    
    void testSTDPPotentiation() {
        nlm::STDP stdp;
        nlm::RandomGenerator rng(42);
        
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        
        float initialWeight = synapse.getWeight();
        
        std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
        std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
        
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        
        float finalWeight = synapse.getWeight();
        
        assert(finalWeight > initialWeight);
        
        std::cout << "  ✓ testSTDPPotentiation passed (Δ=" 
                  << (finalWeight - initialWeight) << ")" << std::endl;
    }
    
    void testSTDPDepression() {
        nlm::STDP stdp;
        
        nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        
        float initialWeight = synapse.getWeight();
        
        std::vector<nlm::Timestamp> preSpikes = {10.0, 20.0, 30.0};
        std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
        
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        
        float finalWeight = synapse.getWeight();
        
        assert(finalWeight < initialWeight);
        
        std::cout << "  ✓ testSTDPDepression passed (Δ=" 
                  << (finalWeight - initialWeight) << ")" << std::endl;
    }
    
    void testSTDPEmptySpikes() {
        nlm::STDP stdp;
        
        nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
        float initialWeight = synapse.getWeight();
        
        std::vector<nlm::Timestamp> preSpikes;
        std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
        
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        assert(synapse.getWeight() == initialWeight);
        
        preSpikes = {1.0, 2.0};
        postSpikes = {};
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        assert(synapse.getWeight() == initialWeight);
        
        std::cout << "  ✓ testSTDPEmptySpikes passed" << std::endl;
    }
    
    void testSTDPWeightBounds() {
        nlm::STDP stdp;
        
        nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.99f);
        
        std::vector<nlm::Timestamp> preSpikes = {0.0, 1.0, 2.0, 3.0, 4.0};
        std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0, 3.0, 4.0, 5.0};
        
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        
        assert(synapse.getWeight() <= 1.0f);
        
        std::cout << "  ✓ testSTDPWeightBounds passed (weight=" 
                  << synapse.getWeight() << ")" << std::endl;
    }
    
    void testSTDPParameterValidation() {
        nlm::STDP stdp;
        
        // Test parameter bounds
        assert(stdp.getLTPWeight() >= 0.0f);
        assert(stdp.getLTDWeight() >= 0.0f);
        assert(stdp.getTimeConstant() > 0.0f);
        
        // Test parameter updates
        stdp.setLTPWeight(0.1f);
        stdp.setLTDWeight(0.12f);
        stdp.setTimeConstant(15.0f);
        
        assert(stdp.getLTPWeight() == 0.1f);
        assert(stdp.getLTDWeight() == 0.12f);
        assert(stdp.getTimeConstant() == 15.0f);
        
        std::cout << "  ✓ testSTDPParameterValidation passed" << std::endl;
    }
    
    void testSTDPRandomized() {
        nlm::RandomGenerator rng(42);
        nlm::STDP stdp;
        
        nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        
        std::vector<nlm::Timestamp> preSpikes = rng.generateSpikeTrain(50.0f, 10.0f);
        std::vector<nlm::Timestamp> postSpikes = rng.generateSpikeTrain(50.0f, 10.0f);
        
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        
        assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
        
        std::cout << "  ✓ testSTDPRandomized passed (weight=" 
                  << synapse.getWeight() << ")" << std::endl;
    }
    
    void testSTDPDeterministic() {
        nlm::STDP stdp1;
        nlm::STDP stdp2;
        
        nlm::Synapse synapse1(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
        nlm::Synapse synapse2(nlm::SynapseId(7), nlm::NeuronId(13), nlm::NeuronId(14));
        
        std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
        std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
        
        // Run with same parameters
        stdp1.update(&synapse1, preSpikes, postSpikes, 0.001);
        stdp2.update(&synapse2, preSpikes, postSpikes, 0.001);
        
        float weightChange1 = synapse1.getWeight() - 0.5f;
        float weightChange2 = synapse2.getWeight() - 0.5f;
        
        assert(std::abs(weightChange1 - weightChange2) < 0.0001f);
        
        std::cout << "  ✓ testSTDPDeterministic passed" << std::endl;
    }
};

}} // namespace nlm::test
