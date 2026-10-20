// Neuron Tests - Enhanced with parameterization and framework integration
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include "TestFramework.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_neuron {

// Enhanced base test using test_framework
class NeuronTestBase : public test_framework::TestBase {
public:
    NeuronTestBase(const std::string& name, test_framework::TestCategory category)
        : TestBase(name, category) {}
};

// Parameterized test for neuron type variations
class NeuronTypeParameterizedTest : public NeuronTestBase {
private:
    nlm::NeuronType type_;
public:
    NeuronTypeParameterizedTest(const std::string& name, nlm::NeuronType type)
        : NeuronTestBase(name, test_framework::TestCategory::Unit), type_(type) {}
    
    void run() override {
        nlm::Neuron neuron(nlm::NeuronId(1));
        neuron.setType(type_);
        assert(neuron.getType() == type_);
        
        // Additional validation based on type
        if (type_ == nlm::NeuronType::Excitatory) {
            // Excitatory neurons might have specific properties
            neuron.injectCurrent(1.0f);
            assert(neuron.getTotalCurrent() == 1.0f);
        } else if (type_ == nlm::NeuronType::Inhibitory) {
            neuron.receiveInhibitoryInput(1.0f);
            assert(neuron.getTotalCurrent() == -1.0f);
        }
        
        std::cout << "    Parameterized test for " << getName() << " passed" << std::endl;
    }
};

// Test helper function for membrane potential variations
void testMembranePotentialVariations(const float targetPotential) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setMembranePotential(targetPotential);
    assert(neuron.getMembranePotential() == targetPotential);
}

// Unit Tests
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

// Performance benchmark for neuron operations
void benchmarkNeuronCreation(size_t iterations) {
    std::vector<nlm::Neuron> neurons;
    neurons.reserve(iterations);
    
    for (size_t i = 0; i < iterations; ++i) {
        neurons.emplace_back(nlm::NeuronId(static_cast<uint64_t>(i)));
    }
}

// Fuzz testing for membrane potential
void fuzzMembranePotential(float maxDelta, int iterations) {
    nlm::RandomGenerator rng(12345);
    
    for (int i = 0; i < iterations; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(1));
        float original = neuron.getMembranePotential();
        
        // Add random variations
        float variation = rng.nextFloat() * maxDelta * 2.0f - maxDelta;
        neuron.addToMembranePotential(variation);
        
        float result = neuron.getMembranePotential();
        assert(result == original + variation);
    }
}

// Property-based test for neuron reset invariant
void testNeuronResetInvariant() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Set neuron to arbitrary state
    neuron.setMembranePotential(-50.0f);
    neuron.injectCurrent(10.0f);
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    
    // Record state before reset
    float potentialBefore = neuron.getMembranePotential();
    float currentBefore = neuron.getTotalCurrent();
    auto spikesBefore = neuron.getSpikeHistory();
    
    neuron.reset();
    
    // Verify reset invariant: neuron should return to default state
    assert(neuron.getMembranePotential() == -70.0f);
    assert(neuron.getTotalCurrent() == 0.0f);
    assert(neuron.getSpikeHistory().empty());
}

void runAll() {
    std::cout << "=== Running Enhanced Neuron Tests ===" << std::endl;
    
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
    
    // Run performance benchmarks
    std::cout << "\n=== Performance Benchmarks ===" << std::endl;
    
    const size_t iterations = 10000;
    auto start = std::chrono::high_resolution_clock::now();
    benchmarkNeuronCreation(iterations);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Neuron creation benchmark: " 
              << std::chrono::duration<double, std::milli>(end - start).count() 
              << "ms for " << iterations << " neurons" << std::endl;
    
    // Run fuzz tests
    std::cout << "\n=== Fuzz Testing ===" << std::endl;
    fuzzMembranePotential(10.0f, 1000);
    std::cout << "Fuzz test for membrane potential completed" << std::endl;
    
    std::cout << "\n=== Property-Based Testing ===" << std::endl;
    testNeuronResetInvariant();
    std::cout << "Property-based test for reset invariant completed" << std::endl;
    
    std::cout << "\n=== All Neuron Tests Completed ===" << std::endl;
}

} // namespace test_neuron
