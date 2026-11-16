// Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>

namespace test_neuron {

void testNeuronCreation() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getId() == nlm::NeuronId(1));
    assert(neuron.getType() == nlm::NeuronType::Internal);
    
    std::cout << "    testNeuronCreation passed" << std::endl;
}

void testNeuronType() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    assert(neuron.getType() == nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    assert(neuron.getType() == nlm::NeuronType::Inhibitory);
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testMembranePotential() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getMembranePotential() == -70.0f);  // Default value
    
    neuron.setMembranePotential(-65.0f);
    assert(neuron.getMembranePotential() == -65.0f);
    
    neuron.addToMembranePotential(5.0f);
    assert(neuron.getMembranePotential() == -60.0f);
    
    std::cout << "    testMembranePotential passed" << std::endl;
}

void testFiringState() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(!neuron.isFiring());
    assert(!neuron.isRefractory());
    
    neuron.setFiringState(nlm::FiringState::Active);
    assert(neuron.isFiring());
    
    neuron.setRefractoryPeriod(10);
    assert(neuron.isRefractory());
    assert(!neuron.isFiring());
    
    std::cout << "    testFiringState passed" << std::endl;
}

void testRefractoryDecrement() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRefractoryPeriod(5);
    assert(neuron.isRefractory());
    
    neuron.decrementRefractory();
    assert(neuron.isRefractory());
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    assert(!neuron.isRefractory());
    
    std::cout << "    testRefractoryDecrement passed" << std::endl;
}

void testCurrentInjection() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getTotalCurrent() == 0.0f);
    
    neuron.injectCurrent(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    neuron.clearTotalCurrent();
    assert(neuron.getTotalCurrent() == 0.0f);
    
    std::cout << "    testCurrentInjection passed" << std::endl;
}

void testSynapticInput() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.receiveExcitatoryInput(5.0f);
    assert(neuron.getTotalCurrent() == 5.0f);
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    assert(neuron.getTotalCurrent() == -3.0f);
    
    std::cout << "    testSynapticInput passed" << std::endl;
}

void testSpikeHistory() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    assert(neuron.getSpikeHistory().empty());
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    assert(neuron.getSpikeHistory().size() == 3);
    
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testSpikeHistory passed" << std::endl;
}

void testRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.initializeRandom(rng);
    
    // Just verify it doesn't crash and values are in reasonable range
    assert(neuron.getMembranePotential() < -60.0f && neuron.getMembranePotential() > -80.0f);
    assert(neuron.getThreshold() < -50.0f && neuron.getThreshold() > -60.0f);
    
    std::cout << "    testRandomInitialization passed" << std::endl;
}

void testReset() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    
    neuron.reset();
    
    assert(neuron.getMembranePotential() == -70.0f);
    assert(neuron.getTotalCurrent() == 0.0f);
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testReset passed" << std::endl;
}

void runAll() {
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
}

void test_neuron::runAllEnhanced(TestFramework& framework) {
    // Create a standalone framework for neuron tests
    class SimpleTestFramework {
    public:
        void addTest(const std::string& name, std::function<void()> testFunc) {
            std::cout << "    Testing: " << name << "...";
            try {
                testFunc();
                std::cout << " PASSED" << std::endl;
            } catch (const std::exception& e) {
                std::cout << " FAILED: " << e.what() << std::endl;
                throw;
            } catch (...) {
                std::cout << " FAILED: unknown exception" << std::endl;
                throw;
            }
        }
    };
    
    SimpleTestFramework testFramework;
    
    testFramework.addTest("Neuron Creation", []() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        assert(neuron.getId() == nlm::NeuronId(1));
        assert(neuron.getType() == nlm::NeuronType::Internal);
    });
    
    testFramework.addTest("Neuron Type", []() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.setType(nlm::NeuronType::Excitatory);
        assert(neuron.getType() == nlm::NeuronType::Excitatory);
        
        neuron.setType(nlm::NeuronType::Inhibitory);
        assert(neuron.getType() == nlm::NeuronType::Inhibitory);
    });
    
    testFramework.addTest("Membrane Potential", []() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        assert(neuron.getMembranePotential() == -70.0f);
        
        neuron.setMembranePotential(-65.0f);
        assert(neuron.getMembranePotential() == -65.0f);
        
        neuron.addToMembranePotential(5.0f);
        assert(neuron.getMembranePotential() == -60.0f);
    });
    
    testFramework.addTest("Firing State", []() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        assert(!neuron.isFiring());
        assert(!neuron.isRefractory());
        
        neuron.setFiringState(nlm::FiringState::Active);
        assert(neuron.isFiring());
        
        neuron.setRefractoryPeriod(10);
        assert(neuron.isRefractory());
        assert(!neuron.isFiring());
    });
    
    testFramework.addTest("Refractory Decrement", []() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.setRefractoryPeriod(5);
        assert(neuron.isRefractory());
        
        neuron.decrementRefractory();
        assert(neuron.isRefractory());
        
        for (int i = 0; i < 4; ++i) {
            neuron.decrementRefractory();
        }
        assert(!neuron.isRefractory());
    });
    
    testFramework.addTest("Current Injection", []() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.injectCurrent(15.0f);
        assert(neuron.getInjectedCurrent() == 15.0f);
        
        neuron.addToInjectedCurrent(5.0f);
        assert(neuron.getInjectedCurrent() == 20.0f);
    });
    
    testFramework.addTest("Firing Threshold", []() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.setThreshold(0.0f);
        assert(neuron.getThreshold() == 0.0f);
        
        neuron.setThreshold(-50.0f);
        assert(neuron.getThreshold() == -50.0f);
    });
}

} // namespace test_neuron
