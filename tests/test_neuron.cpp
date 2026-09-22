// Neuron Tests - Modern Google Test Version
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include "NLMTestFramework.hpp"

namespace nlm {
namespace test {

class NeuronTests : public NeuronTestFixture {
public:
    void TestNeuronCreation() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        EXPECT_EQ(neuron.getId(), nlm::NeuronId(1));
        EXPECT_EQ(neuron.getType(), nlm::NeuronType::Internal);
    }
    
    void TestNeuronType() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.setType(nlm::NeuronType::Excitatory);
        EXPECT_EQ(neuron.getType(), nlm::NeuronType::Excitatory);
        
        neuron.setType(nlm::NeuronType::Inhibitory);
        EXPECT_EQ(neuron.getType(), nlm::NeuronType::Inhibitory);
    }
    
    void TestMembranePotential() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        EXPECT_FLOAT_EQ(neuron.getMembranePotential(), -70.0f);  // Default value
        
        neuron.setMembranePotential(-65.0f);
        EXPECT_FLOAT_EQ(neuron.getMembranePotential(), -65.0f);
        
        neuron.addToMembranePotential(5.0f);
        EXPECT_FLOAT_EQ(neuron.getMembranePotential(), -60.0f);
    }
    
    void TestFiringState() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        EXPECT_FALSE(neuron.isFiring());
        EXPECT_FALSE(neuron.isRefractory());
        
        neuron.setFiringState(nlm::FiringState::Active);
        EXPECT_TRUE(neuron.isFiring());
        
        neuron.setRefractoryPeriod(10);
        EXPECT_TRUE(neuron.isRefractory());
        EXPECT_FALSE(neuron.isFiring());
    }
    
    void TestRefractoryDecrement() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.setRefractoryPeriod(5);
        EXPECT_TRUE(neuron.isRefractory());
        
        neuron.decrementRefractory();
        EXPECT_TRUE(neuron.isRefractory());
        
        for (int i = 0; i < 4; ++i) {
            neuron.decrementRefractory();
        }
        EXPECT_FALSE(neuron.isRefractory());
    }
    
    void TestCurrentInjection() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), 0.0f);
        
        neuron.injectCurrent(5.0f);
        EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), 5.0f);
        
        neuron.clearTotalCurrent();
        EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), 0.0f);
    }
    
    void TestSynapticInput() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.receiveExcitatoryInput(5.0f);
        EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), 5.0f);
        
        neuron.clearTotalCurrent();
        neuron.receiveInhibitoryInput(3.0f);
        EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), -3.0f);
    }
    
    void TestSpikeHistory() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        EXPECT_VECTOR_SIZE(neuron.getSpikeHistory(), 0);
        
        neuron.recordSpike(0.1);
        neuron.recordSpike(0.2);
        neuron.recordSpike(0.3);
        
        EXPECT_EQ(neuron.getSpikeHistory().size(), 3);
        
        neuron.clearSpikeHistory();
        EXPECT_VECTOR_SIZE(neuron.getSpikeHistory(), 0);
    }
    
    void TestRandomInitialization() {
        nlm::RandomGenerator rng(42);
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.initializeRandom(rng);
        
        // Just verify it doesn't crash and values are in reasonable range
        EXPECT_LT(neuron.getMembranePotential(), -60.0f);
        EXPECT_GT(neuron.getMembranePotential(), -80.0f);
        EXPECT_LT(neuron.getThreshold(), -50.0f);
        EXPECT_GT(neuron.getThreshold(), -60.0f);
    }
    
    void TestReset() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.setMembranePotential(-60.0f);
        neuron.injectCurrent(5.0f);
        neuron.recordSpike(0.1);
        
        neuron.reset();
        
        EXPECT_FLOAT_EQ(neuron.getMembranePotential(), -70.0f);
        EXPECT_FLOAT_EQ(neuron.getTotalCurrent(), 0.0f);
        EXPECT_VECTOR_SIZE(neuron.getSpikeHistory(), 0);
    }
};

TEST_F(NeuronTests, TestNeuronCreation) { TestNeuronCreation(); }
TEST_F(NeuronTests, TestNeuronType) { TestNeuronType(); }
TEST_F(NeuronTests, TestMembranePotential) { TestMembranePotential(); }
TEST_F(NeuronTests, TestFiringState) { TestFiringState(); }
TEST_F(NeuronTests, TestRefractoryDecrement) { TestRefractoryDecrement(); }
TEST_F(NeuronTests, TestCurrentInjection) { TestCurrentInjection(); }
TEST_F(NeuronTests, TestSynapticInput) { TestSynapticInput(); }
TEST_F(NeuronTests, TestSpikeHistory) { TestSpikeHistory(); }
TEST_F(NeuronTests, TestRandomInitialization) { TestRandomInitialization(); }
TEST_F(NeuronTests, TestReset) { TestReset(); }

} // namespace test
} // namespace nlm
