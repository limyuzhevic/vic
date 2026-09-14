// Types Tests
#include "core/Types/Types.hpp"
#include "TestFramework.h"
#include <cassert>
#include <iostream>

namespace test_types {

class TestNeuronId : public TestBase {
public:
    std::string getName() const override {
        return "testNeuronId";
    }
    
    void run() override {
        nlm::NeuronId id1(1);
        nlm::NeuronId id2(2);
        nlm::NeuronId id3(1);
        
        assert(id1 == id3);
        assert(id1 != id2);
        assert(id1 < id2);
        assert(id2 > id1);
        assert(id1.index() == 1);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestSynapseId : public TestBase {
public:
    std::string getName() const override {
        return "testSynapseId";
    }
    
    void run() override {
        nlm::SynapseId id1(100);
        nlm::SynapseId id2(200);
        
        assert(id1 != id2);
        assert(id1 < id2);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestRegionId : public TestBase {
public:
    std::string getName() const override {
        return "testRegionId";
    }
    
    void run() override {
        nlm::RegionId id1(1);
        nlm::RegionId id2(2);
        
        assert(id1 != id2);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestPopulationId : public TestBase {
public:
    std::string getName() const override {
        return "testPopulationId";
    }
    
    void run() override {
        nlm::PopulationId id1(1);
        nlm::PopulationId id2(2);
        
        assert(id1 != id2);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestNeuronType : public TestBase {
public:
    std::string getName() const override {
        return "testNeuronType";
    }
    
    void run() override {
        assert(nlm::NeuronType::Excitatory != nlm::NeuronType::Inhibitory);
        assert(nlm::NeuronType::Sensory != nlm::NeuronType::Motor);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestSynapseType : public TestBase {
public:
    std::string getName() const override {
        return "testSynapseType";
    }
    
    void run() override {
        assert(nlm::SynapseType::Excitatory != nlm::SynapseType::Inhibitory);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestPlasticityFlags : public TestBase {
public:
    std::string getName() const override {
        return "testPlasticityFlags";
    }
    
    void run() override {
        nlm::PlasticityFlags flags;
        assert(!flags.hebbian);
        assert(!flags.stdp);
        assert(!flags.reward_modulated);
        
        flags.hebbian = true;
        assert(flags.hebbian);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestSpikeEvent : public TestBase {
public:
    std::string getName() const override {
        return "testSpikeEvent";
    }
    
    void run() override {
        nlm::SpikeEvent event;
        event.source_neuron = nlm::NeuronId(1);
        event.timestamp = 0.5;
        event.step = 500;
        
        assert(event.source_neuron.index() == 1);
        assert(event.timestamp == 0.5);
        assert(event.step == 500);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

// Function to register all types tests
void registerTypesTests(TestRunner& runner) {
    runner.registerTest<TestNeuronId>();
    runner.registerTest<TestSynapseId>();
    runner.registerTest<TestRegionId>();
    runner.registerTest<TestPopulationId>();
    runner.registerTest<TestNeuronType>();
    runner.registerTest<TestSynapseType>();
    runner.registerTest<TestPlasticityFlags>();
    runner.registerTest<TestSpikeEvent>();
}

// Keep backward compatibility
void runAll() {
    std::cout << "Running Types tests..." << std::endl;
    TestRunner runner;
    registerTypesTests(runner);
    runner.runAll();
}

} // namespace test_types

void testSynapseId() {
    nlm::SynapseId id1(100);
    nlm::SynapseId id2(200);
    
    assert(id1 != id2);
    assert(id1 < id2);
    
    std::cout << "    testSynapseId passed" << std::endl;
}

void testRegionId() {
    nlm::RegionId id1(1);
    nlm::RegionId id2(2);
    
    assert(id1 != id2);
    
    std::cout << "    testRegionId passed" << std::endl;
}

void testPopulationId() {
    nlm::PopulationId id1(1);
    nlm::PopulationId id2(2);
    
    assert(id1 != id2);
    
    std::cout << "    testPopulationId passed" << std::endl;
}

void testNeuronType() {
    assert(nlm::NeuronType::Excitatory != nlm::NeuronType::Inhibitory);
    assert(nlm::NeuronType::Sensory != nlm::NeuronType::Motor);
    
    std::cout << "    testNeuronType passed" << std::endl;
}

void testSynapseType() {
    assert(nlm::SynapseType::Excitatory != nlm::SynapseType::Inhibitory);
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testPlasticityFlags() {
    nlm::PlasticityFlags flags;
    assert(!flags.hebbian);
    assert(!flags.stdp);
    assert(!flags.reward_modulated);
    
    flags.hebbian = true;
    assert(flags.hebbian);
    
    std::cout << "    testPlasticityFlags passed" << std::endl;
}

void testSpikeEvent() {
    nlm::SpikeEvent event;
    event.source_neuron = nlm::NeuronId(1);
    event.timestamp = 0.5;
    event.step = 500;
    
    assert(event.source_neuron.index() == 1);
    assert(event.timestamp == 0.5);
    assert(event.step == 500);
    
    std::cout << "    testSpikeEvent passed" << std::endl;
}

void runAll() {
    testNeuronId();
    testSynapseId();
    testRegionId();
    testPopulationId();
    testNeuronType();
    testSynapseType();
    testPlasticityFlags();
    testSpikeEvent();
}

} // namespace test_types
