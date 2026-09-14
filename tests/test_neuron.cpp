// Neuron Tests
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include "TestFramework.h"
#include <cassert>
#include <iostream>

namespace test_neuron {

class TestNeuronCreation : public TestBase {
public:
    std::string getName() const override {
        return "testNeuronCreation";
    }
    
    void run() override {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        assert(neuron.getId() == nlm::NeuronId(1));
        assert(neuron.getType() == nlm::NeuronType::Internal);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestNeuronType : public TestBase {
public:
    std::string getName() const override {
        return "testNeuronType";
    }
    
    void run() override {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.setType(nlm::NeuronType::Excitatory);
        assert(neuron.getType() == nlm::NeuronType::Excitatory);
        
        neuron.setType(nlm::NeuronType::Inhibitory);
        assert(neuron.getType() == nlm::NeuronType::Inhibitory);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestMembranePotential : public TestBase {
public:
    std::string getName() const override {
        return "testMembranePotential";
    }
    
    void run() override {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        assert(neuron.getMembranePotential() == -70.0f);  // Default value
        
        neuron.setMembranePotential(-65.0f);
        assert(neuron.getMembranePotential() == -65.0f);
        
        neuron.addToMembranePotential(5.0f);
        assert(neuron.getMembranePotential() == -60.0f);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestFiringState : public TestBase {
public:
    std::string getName() const override {
        return "testFiringState";
    }
    
    void run() override {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        assert(!neuron.isFiring());
        assert(!neuron.isRefractory());
        
        neuron.setFiringState(nlm::FiringState::Active);
        assert(neuron.isFiring());
        
        neuron.setRefractoryPeriod(10);
        assert(neuron.isRefractory());
        assert(!neuron.isFiring());
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestRefractoryDecrement : public TestBase {
public:
    std::string getName() const override {
        return "testRefractoryDecrement";
    }
    
    void run() override {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.setRefractoryPeriod(5);
        assert(neuron.isRefractory());
        
        neuron.decrementRefractory();
        assert(neuron.isRefractory());
        
        for (int i = 0; i < 4; ++i) {
            neuron.decrementRefractory();
        }
        assert(!neuron.isRefractory());
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestCurrentInjection : public TestBase {
public:
    std::string getName() const override {
        return "testCurrentInjection";
    }
    
    void run() override {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        assert(neuron.getTotalCurrent() == 0.0f);
        
        neuron.injectCurrent(5.0f);
        assert(neuron.getTotalCurrent() == 5.0f);
        
        neuron.clearTotalCurrent();
        assert(neuron.getTotalCurrent() == 0.0f);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestSynapticInput : public TestBase {
public:
    std::string getName() const override {
        return "testSynapticInput";
    }
    
    void run() override {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.receiveExcitatoryInput(5.0f);
        assert(neuron.getTotalCurrent() == 5.0f);
        
        neuron.clearTotalCurrent();
        neuron.receiveInhibitoryInput(3.0f);
        assert(neuron.getTotalCurrent() == -3.0f);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestSpikeHistory : public TestBase {
public:
    std::string getName() const override {
        return "testSpikeHistory";
    }
    
    void run() override {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        assert(neuron.getSpikeHistory().empty());
        
        neuron.recordSpike(0.1);
        neuron.recordSpike(0.2);
        neuron.recordSpike(0.3);
        
        assert(neuron.getSpikeHistory().size() == 3);
        
        neuron.clearSpikeHistory();
        assert(neuron.getSpikeHistory().empty());
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestRandomInitialization : public TestBase {
public:
    std::string getName() const override {
        return "testRandomInitialization";
    }
    
    void run() override {
        nlm::RandomGenerator rng(42);
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.initializeRandom(rng);
        
        // Just verify it doesn't crash and values are in reasonable range
        assert(neuron.getMembranePotential() < -60.0f && neuron.getMembranePotential() > -80.0f);
        assert(neuron.getThreshold() < -50.0f && neuron.getThreshold() > -60.0f);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestReset : public TestBase {
public:
    std::string getName() const override {
        return "testReset";
    }
    
    void run() override {
        nlm::Neuron neuron(nlm::NeuronId(1));
        
        neuron.setMembranePotential(-60.0f);
        neuron.injectCurrent(5.0f);
        neuron.recordSpike(0.1);
        
        neuron.reset();
        
        assert(neuron.getMembranePotential() == -70.0f);
        assert(neuron.getTotalCurrent() == 0.0f);
        assert(neuron.getSpikeHistory().empty());
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

// Function to register all neuron tests
void registerNeuronTests(TestRunner& runner) {
    runner.registerTest<TestNeuronCreation>();
    runner.registerTest<TestNeuronType>();
    runner.registerTest<TestMembranePotential>();
    runner.registerTest<TestFiringState>();
    runner.registerTest<TestRefractoryDecrement>();
    runner.registerTest<TestCurrentInjection>();
    runner.registerTest<TestSynapticInput>();
    runner.registerTest<TestSpikeHistory>();
    runner.registerTest<TestRandomInitialization>();
    runner.registerTest<TestReset>();
}

// Keep backward compatibility
void runAll() {
    std::cout << "Running Neuron tests..." << std::endl;
    TestRunner runner;
    registerNeuronTests(runner);
    runner.runAll();
}

} // namespace test_neuron

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

} // namespace test_neuron
