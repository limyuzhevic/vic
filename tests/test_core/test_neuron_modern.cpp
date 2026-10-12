#include "framework/test_base.hpp"
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cmath>

class NeuronTestSuite : public TestSuite {
public:
    NeuronTestSuite() : TestSuite() {}
    
    void runTests() override {
        std::cout << "Running Neuron Test Suite..." << std::endl;
        
        testNeuronCreation();
        testNeuronType();
        testMembranePotential();
        testFiringState();
        testRefractoryDecrement();
        testCurrentInjection();
        testSynapticInput();
        testSpikeHistory();
        testRandomInitialization();
        testReset();
        
        std::cout << "Neuron Test Suite completed." << std::endl;
    }
    
    std::string getName() const override {
        return "NeuronTestSuite";
    }
    
private:
    void testNeuronCreation() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        ASSERT_EQ(neuron.getId().getId(), 1);
        ASSERT_EQ(static_cast<int>(neuron.getType()), static_cast<int>(nlm::NeuronType::Internal));
        
        std::cout << "    testNeuronCreation passed" << std::endl;
    }
    
    void testNeuronType() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.setType(nlm::NeuronType::Excitatory);
        ASSERT_EQ(static_cast<int>(neuron.getType()), static_cast<int>(nlm::NeuronType::Excitatory));
        
        neuron.setType(nlm::NeuronType::Inhibitory);
        ASSERT_EQ(static_cast<int>(neuron.getType()), static_cast<int>(nlm::NeuronType::Inhibitory));
        
        std::cout << "    testNeuronType passed" << std::endl;
    }
    
    void testMembranePotential() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        ASSERT_EQ(neuron.getMembranePotential(), -70.0f);
        
        neuron.setMembranePotential(-65.0f);
        ASSERT_EQ(neuron.getMembranePotential(), -65.0f);
        
        neuron.addToMembranePotential(5.0f);
        ASSERT_EQ(neuron.getMembranePotential(), -60.0f);
        
        std::cout << "    testMembranePotential passed" << std::endl;
    }
    
    void testFiringState() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        ASSERT_FALSE(neuron.isFiring());
        ASSERT_FALSE(neuron.isRefractory());
        
        neuron.setFiringState(nlm::FiringState::Active);
        ASSERT_TRUE(neuron.isFiring());
        
        neuron.setRefractoryPeriod(10);
        ASSERT_TRUE(neuron.isRefractory());
        ASSERT_FALSE(neuron.isFiring());
        
        std::cout << "    testFiringState passed" << std::endl;
    }
    
    void testRefractoryDecrement() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.setRefractoryPeriod(5);
        ASSERT_TRUE(neuron.isRefractory());
        
        neuron.decrementRefractory();
        ASSERT_TRUE(neuron.isRefractory());
        
        for (int i = 0; i < 4; ++i) {
            neuron.decrementRefractory();
        }
        ASSERT_FALSE(neuron.isRefractory());
        
        std::cout << "    testRefractoryDecrement passed" << std::endl;
    }
    
    void testCurrentInjection() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        ASSERT_EQ(neuron.getTotalCurrent(), 0.0f);
        
        neuron.injectCurrent(5.0f);
        ASSERT_EQ(neuron.getTotalCurrent(), 5.0f);
        
        neuron.clearTotalCurrent();
        ASSERT_EQ(neuron.getTotalCurrent(), 0.0f);
        
        std::cout << "    testCurrentInjection passed" << std::endl;
    }
    
    void testSynapticInput() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.receiveExcitatoryInput(5.0f);
        ASSERT_EQ(neuron.getTotalCurrent(), 5.0f);
        
        neuron.clearTotalCurrent();
        neuron.receiveInhibitoryInput(3.0f);
        ASSERT_EQ(neuron.getTotalCurrent(), -3.0f);
        
        std::cout << "    testSynapticInput passed" << std::endl;
    }
    
    void testSpikeHistory() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        ASSERT_EQ(neuron.getSpikeHistory().size(), 0);
        
        neuron.recordSpike(0.1);
        neuron.recordSpike(0.2);
        neuron.recordSpike(0.3);
        
        ASSERT_EQ(neuron.getSpikeHistory().size(), 3);
        
        neuron.clearSpikeHistory();
        ASSERT_EQ(neuron.getSpikeHistory().size(), 0);
        
        std::cout << "    testSpikeHistory passed" << std::endl;
    }
    
    void testRandomInitialization() {
        nlm::RandomGenerator rng(42);
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.initializeRandom(rng);
        
        ASSERT_TRUE(neuron.getMembranePotential() < -60.0f && neuron.getMembranePotential() > -80.0f);
        ASSERT_TRUE(neuron.getThreshold() < -50.0f && neuron.getThreshold() > -60.0f);
        
        std::cout << "    testRandomInitialization passed" << std::endl;
    }
    
    void testReset() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.setMembranePotential(-60.0f);
        neuron.injectCurrent(5.0f);
        neuron.recordSpike(0.1);
        
        neuron.reset();
        
        ASSERT_EQ(neuron.getMembranePotential(), -70.0f);
        ASSERT_EQ(neuron.getTotalCurrent(), 0.0f);
        ASSERT_EQ(neuron.getSpikeHistory().size(), 0);
        
        std::cout << "    testReset passed" << std::endl;
    }
};

extern "C" {
    TestSuite* createNeuronTestSuite() {
        return new NeuronTestSuite();
    }
}