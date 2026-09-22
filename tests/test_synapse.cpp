// Synapse Tests - Modern Google Test Version
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include "NLMTestFramework.hpp"

namespace nlm {
namespace test {

class SynapseTests : public SynapseTestFixture {
public:
    void TestSynapseCreation() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        EXPECT_EQ(synapse.getId(), nlm::SynapseId(1));
        EXPECT_EQ(synapse.getSourceNeuron(), nlm::NeuronId(1));
        EXPECT_EQ(synapse.getDestinationNeuron(), nlm::NeuronId(2));
    }
    
    void TestSynapseWeight() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        EXPECT_FLOAT_EQ(synapse.getWeight(), 0.0f);
        
        synapse.setWeight(0.5f);
        EXPECT_FLOAT_EQ(synapse.getWeight(), 0.5f);
        
        synapse.addToWeight(0.2f);
        EXPECT_FLOAT_EQ(synapse.getWeight(), 0.7f);
        
        synapse.addToWeight(-0.3f);
        EXPECT_FLOAT_EQ(synapse.getWeight(), 0.4f);
    }
    
    void TestSynapseWeightBounds() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        // Test upper bound
        synapse.setWeight(10.0f);
        EXPECT_LE(synapse.getWeight(), 1.0f);  // Should be clamped
        
        // Test lower bound
        synapse.setWeight(-10.0f);
        EXPECT_GE(synapse.getWeight(), -1.0f);  // Should be clamped
    }
    
    void TestSynapseType() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        synapse.setType(nlm::SynapseType::Excitatory);
        EXPECT_EQ(synapse.getType(), nlm::SynapseType::Excitatory);
        EXPECT_TRUE(synapse.isExcitatory());
        EXPECT_FALSE(synapse.isInhibitory());
        
        synapse.setType(nlm::SynapseType::Inhibitory);
        EXPECT_EQ(synapse.getType(), nlm::SynapseType::Inhibitory);
        EXPECT_TRUE(synapse.isInhibitory());
        EXPECT_FALSE(synapse.isExcitatory());
    }
    
    void TestSynapseDelay() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        EXPECT_EQ(synapse.getDelay(), 1);  // Default
        
        synapse.setDelay(5);
        EXPECT_EQ(synapse.getDelay(), 5);
    }
    
    void TestSynapseSpikeHistory() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        EXPECT_VECTOR_SIZE(synapse.getPreSpikeHistory(), 0);
        EXPECT_VECTOR_SIZE(synapse.getPostSpikeHistory(), 0);
        
        synapse.recordPreSpike(0.1);
        synapse.recordPreSpike(0.2);
        synapse.recordPostSpike(0.15);
        
        EXPECT_EQ(synapse.getPreSpikeHistory().size(), 2);
        EXPECT_EQ(synapse.getPostSpikeHistory().size(), 1);
        
        synapse.clearHistory();
        EXPECT_VECTOR_SIZE(synapse.getPreSpikeHistory(), 0);
        EXPECT_VECTOR_SIZE(synapse.getPostSpikeHistory(), 0);
    }
    
    void TestSynapseEligibilityTrace() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        EXPECT_FLOAT_EQ(synapse.getEligibilityTrace(), 0.0f);
        
        synapse.setEligibilityTrace(0.5f);
        EXPECT_FLOAT_EQ(synapse.getEligibilityTrace(), 0.5f);
        
        // Test decay
        synapse.decayEligibilityTrace(0.1f);
        EXPECT_LT(synapse.getEligibilityTrace(), 0.5f);
    }
    
    void TestSynapseEfficacy() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        EXPECT_FLOAT_EQ(synapse.getEfficacy(), 1.0f);  // Default
        
        synapse.setEfficacy(0.5f);
        EXPECT_FLOAT_EQ(synapse.getEfficacy(), 0.5f);
        
        // Test bounds
        synapse.setEfficacy(5.0f);
        EXPECT_LE(synapse.getEfficacy(), 2.0f);  // Clamped
    }
    
    void TestSynapseRandomInitialization() {
        nlm::RandomGenerator rng(42);
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.initializeRandom(rng);
        
        // Weight should be positive for excitatory
        EXPECT_GT(synapse.getWeight(), 0.0f);
        
        // Delay should be in valid range
        EXPECT_GE(synapse.getDelay(), 1);
        EXPECT_LE(synapse.getDelay(), 5);
    }
    
    void TestSynapseReset() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        synapse.setWeight(0.5f);
        synapse.recordPreSpike(0.1);
        synapse.recordPostSpike(0.2);
        synapse.setEligibilityTrace(0.3f);
        
        synapse.reset();
        
        EXPECT_FLOAT_EQ(synapse.getWeight(), 0.0f);
        EXPECT_VECTOR_SIZE(synapse.getPreSpikeHistory(), 0);
        EXPECT_VECTOR_SIZE(synapse.getPostSpikeHistory(), 0);
        EXPECT_FLOAT_EQ(synapse.getEligibilityTrace(), 0.0f);
    }
    
    void TestSynapsePlasticityFlags() {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        EXPECT_FALSE(synapse.getPlasticityFlags().hebbian);
        EXPECT_FALSE(synapse.getPlasticityFlags().stdp);
        
        synapse.enablePlasticity(true, true, false);
        
        EXPECT_TRUE(synapse.getPlasticityFlags().hebbian);
        EXPECT_TRUE(synapse.getPlasticityFlags().stdp);
        EXPECT_FALSE(synapse.getPlasticityFlags().reward_modulated);
    }
};

TEST_F(SynapseTests, TestSynapseCreation) { TestSynapseCreation(); }
TEST_F(SynapseTests, TestSynapseWeight) { TestSynapseWeight(); }
TEST_F(SynapseTests, TestSynapseWeightBounds) { TestSynapseWeightBounds(); }
TEST_F(SynapseTests, TestSynapseType) { TestSynapseType(); }
TEST_F(SynapseTests, TestSynapseDelay) { TestSynapseDelay(); }
TEST_F(SynapseTests, TestSynapseSpikeHistory) { TestSynapseSpikeHistory(); }
TEST_F(SynapseTests, TestSynapseEligibilityTrace) { TestSynapseEligibilityTrace(); }
TEST_F(SynapseTests, TestSynapseEfficacy) { TestSynapseEfficacy(); }
TEST_F(SynapseTests, TestSynapseRandomInitialization) { TestSynapseRandomInitialization(); }
TEST_F(SynapseTests, TestSynapseReset) { TestSynapseReset(); }
TEST_F(SynapseTests, TestSynapsePlasticityFlags) { TestSynapsePlasticityFlags(); }

} // namespace test
} // namespace nlm
