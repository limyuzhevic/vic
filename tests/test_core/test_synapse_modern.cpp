#include "framework/test_base.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cmath>

class SynapseTestSuite : public TestSuite {
public:
    SynapseTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Synapse Test Suite..." << std::endl;
        
        testSynapseCreation();
        testSynapseWeight();
        testSynapseWeightBounds();
        testSynapseType();
        testSynapseDelay();
        testSynapseSpikeHistory();
        testSynapseEligibilityTrace();
        testSynapseEfficacy();
        testSynapseRandomInitialization();
        testSynapseReset();
        testSynapsePlasticityFlags();
        
        std::cout << "Synapse Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "SynapseTestSuite";
    }
    
private:
    void testSynapseCreation() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        ASSERT_EQ(synapse.getId().getId(), 1);
        ASSERT_EQ(synapse.getSourceNeuron().getId(), 1);
        ASSERT_EQ(synapse.getDestinationNeuron().getId(), 2);
        
        std::cout << "    testSynapseCreation passed" << std::endl;
    }
    
    void testSynapseWeight() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        ASSERT_EQ(synapse.getWeight(), 0.0f);
        
        synapse.setWeight(0.5f);
        ASSERT_EQ(synapse.getWeight(), 0.5f);
        
        synapse.addToWeight(0.2f);
        ASSERT_EQ(synapse.getWeight(), 0.7f);
        
        synapse.addToWeight(-0.3f);
        ASSERT_EQ(synapse.getWeight(), 0.4f);
        
        std::cout << "    testSynapseWeight passed" << std::endl;
    }
    
    void testSynapseWeightBounds() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        synapse.setWeight(10.0f);
        ASSERT_TRUE(synapse.getWeight() <= 1.0f);
        
        synapse.setWeight(-10.0f);
        ASSERT_TRUE(synapse.getWeight() >= -1.0f);
        
        std::cout << "    testSynapseWeightBounds passed" << std::endl;
    }
    
    void testSynapseType() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        synapse.setType(nlm::SynapseType::Excitatory);
        ASSERT_EQ(static_cast<int>(synapse.getType()), static_cast<int>(nlm::SynapseType::Excitatory));
        ASSERT_TRUE(synapse.isExcitatory());
        ASSERT_FALSE(synapse.isInhibitory());
        
        synapse.setType(nlm::SynapseType::Inhibitory);
        ASSERT_EQ(static_cast<int>(synapse.getType()), static_cast<int>(nlm::SynapseType::Inhibitory));
        ASSERT_TRUE(synapse.isInhibitory());
        ASSERT_FALSE(synapse.isExcitatory());
        
        std::cout << "    testSynapseType passed" << std::endl;
    }
    
    void testSynapseDelay() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        ASSERT_EQ(synapse.getDelay(), 1);
        
        synapse.setDelay(5);
        ASSERT_EQ(synapse.getDelay(), 5);
        
        std::cout << "    testSynapseDelay passed" << std::endl;
    }
    
    void testSynapseSpikeHistory() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        ASSERT_EQ(synapse.getPreSpikeHistory().size(), 0);
        ASSERT_EQ(synapse.getPostSpikeHistory().size(), 0);
        
        synapse.recordPreSpike(0.1);
        synapse.recordPreSpike(0.2);
        synapse.recordPostSpike(0.15);
        
        ASSERT_EQ(synapse.getPreSpikeHistory().size(), 2);
        ASSERT_EQ(synapse.getPostSpikeHistory().size(), 1);
        
        synapse.clearHistory();
        ASSERT_EQ(synapse.getPreSpikeHistory().size(), 0);
        ASSERT_EQ(synapse.getPostSpikeHistory().size(), 0);
        
        std::cout << "    testSynapseSpikeHistory passed" << std::endl;
    }
    
    void testSynapseEligibilityTrace() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        ASSERT_EQ(synapse.getEligibilityTrace(), 0.0f);
        
        synapse.setEligibilityTrace(0.5f);
        ASSERT_EQ(synapse.getEligibilityTrace(), 0.5f);
        
        synapse.decayEligibilityTrace(0.1f);
        ASSERT_TRUE(synapse.getEligibilityTrace() < 0.5f);
        
        std::cout << "    testSynapseEligibilityTrace passed" << std::endl;
    }
    
    void testSynapseEfficacy() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        ASSERT_EQ(synapse.getEfficacy(), 1.0f);
        
        synapse.setEfficacy(0.5f);
        ASSERT_EQ(synapse.getEfficacy(), 0.5f);
        
        synapse.setEfficacy(5.0f);
        ASSERT_TRUE(synapse.getEfficacy() <= 2.0f);
        
        std::cout << "    testSynapseEfficacy passed" << std::endl;
    }
    
    void testSynapseRandomInitialization() {
        nlm::RandomGenerator rng(42);
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.initializeRandom(rng);
        
        ASSERT_TRUE(synapse.getWeight() > 0.0f);
        ASSERT_TRUE(synapse.getDelay() >= 1 && synapse.getDelay() <= 5);
        
        std::cout << "    testSynapseRandomInitialization passed" << std::endl;
    }
    
    void testSynapseReset() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        synapse.setWeight(0.5f);
        synapse.recordPreSpike(0.1);
        synapse.recordPostSpike(0.2);
        synapse.setEligibilityTrace(0.3f);
        
        synapse.reset();
        
        ASSERT_EQ(synapse.getWeight(), 0.0f);
        ASSERT_EQ(synapse.getPreSpikeHistory().size(), 0);
        ASSERT_EQ(synapse.getPostSpikeHistory().size(), 0);
        ASSERT_EQ(synapse.getEligibilityTrace(), 0.0f);
        
        std::cout << "    testSynapseReset passed" << std::endl;
    }
    
    void testSynapsePlasticityFlags() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        ASSERT_FALSE(synapse.getPlasticityFlags().hebbian);
        ASSERT_FALSE(synapse.getPlasticityFlags().stdp);
        
        synapse.enablePlasticity(true, true, false);
        
        ASSERT_TRUE(synapse.getPlasticityFlags().hebbian);
        ASSERT_TRUE(synapse.getPlasticityFlags().stdp);
        ASSERT_FALSE(synapse.getPlasticityFlags().reward_modulated);
        
        std::cout << "    testSynapsePlasticityFlags passed" << std::endl;
    }
};

extern "C" {
    TestSuite* createSynapseTestSuite() {
        return new SynapseTestSuite();
    }
}