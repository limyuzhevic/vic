// Hebbian Test Implementation
// Comprehensive tests for Hebbian plasticity rule

#include "plasticity/Hebbian.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

namespace nlm {
namespace test {

class HebbianTest : public BaseTest {
public:
    HebbianTest() : testName("Hebbian Tests") {}
    
    void run() override {
        std::cout << "Running Hebbian tests..." << std::endl;
        
        testHebbianCreation();
        testHebbianConfiguration();
        testHebbianCorrelatedSpikes();
        testHebbianAntiCorrelatedSpikes();
        testHebbianEmptySpikes();
        testHebbianWeightBounds();
        testHebbianInhibitorySynaptic();
        testHebbianRandomizedLearning();
        testHebbianNameValidation();
        
        std::cout << "Hebbian tests completed successfully!" << std::endl;
    }
    
private:
    void testHebbianCreation() {
        nlm::Hebbian hebbian;
        
        assert(hebbian.getLearningRate() > 0.0f);
        assert(hebbian.getMaxWeight() > 0.0f);
        assert(hebbian.getMaxWeight() > hebbian.getMinWeight());
        
        std::cout << "  ✓ testHebbianCreation passed" << std::endl;
    }
    
    void testHebbianConfiguration() {
        nlm::Hebbian hebbian;
        
        hebbian.setLearningRate(0.05f);
        hebbian.setMaxWeight(2.0f);
        
        assert(hebbian.getLearningRate() == 0.05f);
        assert(hebbian.getMaxWeight() == 2.0f);
        
        std::cout << "  ✓ testHebbianConfiguration passed" << std::endl;
    }
    
    void testHebbianCorrelatedSpikes() {
        nlm::Hebbian hebbian;
        hebbian.setLearningRate(0.1f);
        
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        
        float initialWeight = synapse.getWeight();
        
        std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0, 30.0, 40.0};
        std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0, 35.0, 45.0};
        
        hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
        
        float finalWeight = synapse.getWeight();
        
        assert(finalWeight > initialWeight);
        
        std::cout << "  ✓ testHebbianCorrelatedSpikes passed (Δ=" 
                  << (finalWeight - initialWeight) << ")" << std::endl;
    }
    
    void testHebbianAntiCorrelatedSpikes() {
        nlm::Hebbian hebbian;
        hebbian.setLearningRate(0.1f);
        
        nlm::Synapse synapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        
        float initialWeight = synapse.getWeight();
        
        std::vector<nlm::Timestamp> preSpikes = {0.0, 20.0, 40.0};
        std::vector<nlm::Timestamp> postSpikes = {10.0, 30.0, 50.0};
        
        hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
        
        float finalWeight = synapse.getWeight();
        
        assert(finalWeight >= 0.0f);
        
        std::cout << "  ✓ testHebbianAntiCorrelatedSpikes passed (final=" 
                  << finalWeight << ")" << std::endl;
    }
    
    void testHebbianEmptySpikes() {
        nlm::Hebbian hebbian;
        
        nlm::Synapse synapse(nlm::SynapseId(3), nlm::NeuronId(5), nlm::NeuronId(6));
        float initialWeight = synapse.getWeight();
        
        std::vector<nlm::Timestamp> preSpikes;
        std::vector<nlm::Timestamp> postSpikes = {1.0, 2.0};
        
        hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
        assert(synapse.getWeight() == initialWeight);
        
        preSpikes = {1.0, 2.0};
        postSpikes = {};
        hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
        assert(synapse.getWeight() == initialWeight);
        
        std::cout << "  ✓ testHebbianEmptySpikes passed" << std::endl;
    }
    
    void testHebbianWeightBounds() {
        nlm::Hebbian hebbian;
        hebbian.setMaxWeight(1.0f);
        hebbian.setLearningRate(1.0f);
        
        nlm::Synapse synapse(nlm::SynapseId(4), nlm::NeuronId(7), nlm::NeuronId(8));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.1f);
        
        std::vector<nlm::Timestamp> preSpikes;
        std::vector<nlm::Timestamp> postSpikes;
        
        for (int i = 0; i < 50; ++i) {
            preSpikes.push_back(i * 10.0f);
            postSpikes.push_back(i * 10.0f + 5.0f);
        }
        
        hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
        
        assert(synapse.getWeight() <= 1.0f);
        
        std::cout << "  ✓ testHebbianWeightBounds passed (weight=" 
                  << synapse.getWeight() << ")" << std::endl;
    }
    
    void testHebbianInhibitorySynaptic() {
        nlm::Hebbian hebbian;
        hebbian.setLearningRate(0.05f);
        
        nlm::Synapse synapse(nlm::SynapseId(5), nlm::NeuronId(9), nlm::NeuronId(10));
        synapse.setType(nlm::SynapseType::Inhibitory);
        synapse.setWeight(-0.5f);
        
        float initialWeight = synapse.getWeight();
        
        std::vector<nlm::Timestamp> preSpikes = {0.0, 20.0};
        std::vector<nlm::Timestamp> postSpikes = {5.0, 25.0};
        
        hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
        
        float finalWeight = synapse.getWeight();
        
        assert(finalWeight > initialWeight);
        
        std::cout << "  ✓ testHebbianInhibitorySynaptic passed (Δ=" 
                  << (finalWeight - initialWeight) << ")" << std::endl;
    }
    
    void testHebbianRandomizedLearning() {
        nlm::RandomGenerator rng(42);
        nlm::Hebbian hebbian;
        hebbian.setLearningRate(0.01f);
        
        nlm::Synapse synapse(nlm::SynapseId(6), nlm::NeuronId(11), nlm::NeuronId(12));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.3f);
        
        std::vector<nlm::Timestamp> preSpikes = rng.generateSpikeTrain(100.0f, 5.0f);
        std::vector<nlm::Timestamp> postSpikes = rng.generateSpikeTrain(100.0f, 5.0f);
        
        hebbian.update(&synapse, preSpikes, postSpikes, 0.001);
        
        assert(synapse.getWeight() >= -1.0f && synapse.getWeight() <= 1.0f);
        
        std::cout << "  ✓ testHebbianRandomizedLearning passed (final=" 
                  << synapse.getWeight() << ")" << std::endl;
    }
    
    void testHebbianNameValidation() {
        nlm::Hebbian hebbian;
        
        assert(hebbian.getName() != nullptr);
        assert(std::string(hebbian.getName()) == "Hebbian");
        
        hebbian.setLearningRate(0.5f);
        hebbian.setMaxWeight(10.0f);
        
        assert(std::string(hebbian.getName()) == "Hebbian");
        
        std::cout << "  ✓ testHebbianNameValidation passed" << std::endl;
    }
};

}} // namespace nlm::test
