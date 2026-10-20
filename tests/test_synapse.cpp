// Synapse Tests - Enhanced with comprehensive validation
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "core/Random/Random.hpp"
#include "TestFramework.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <unordered_map>

namespace test_synapse {

// Synapse validation test classes
class SynapsePlasticityValidationTest : public test_framework::TestBase {
private:
    nlm::SynapseType type_;
    float initialWeight_;
public:
    SynapsePlasticityValidationTest(const std::string& name, nlm::SynapseType type, float weight)
        : TestBase(name, test_framework::TestCategory::Unit), type_(type), initialWeight_(weight) {}
    
    void run() override {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse.setType(type_);
        synapse.setWeight(initialWeight_);
        
        // Validate initial state
        assert(synapse.getType() == type_);
        assert(synapse.getWeight() == initialWeight_);
        
        // Test STDP plasticity based on spike timing
        nlm::STDP stdp;
        
        // Pre-before-post scenario (potentiation)
        std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
        std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};
        
        float weightBefore = synapse.getWeight();
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        float weightAfter = synapse.getWeight();
        
        // Weight should have increased for pre-before-post
        if (type_ == nlm::SynapseType::Excitatory) {
            assert(weightAfter >= weightBefore);  // May stay same if already max
        }
        
        // Test post-before-pre scenario (depression)
        std::fill(preSpikes.begin(), preSpikes.end(), 10.0);
        std::fill(postSpikes.begin(), postSpikes.end(), 5.0);
        
        float weightBefore2 = synapse.getWeight();
        stdp.update(&synapse, preSpikes, postSpikes, 0.001);
        float weightAfter2 = synapse.getWeight();
        
        // Weight should have decreased for post-before-pre
        if (type_ == nlm::SynapseType::Excitatory) {
            assert(weightAfter2 <= weightBefore2);  // May stay same if already min
        }
        
        std::cout << "    Synapse plasticity validation test for " << getName() << " passed" << std::endl;
    }
};

class SynapseWeightBoundsValidationTest : public test_framework::TestBase {
public:
    SynapseWeightBoundsValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Unit) {}
    
    void run() override {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse.setType(nlm::SynapseType::Excitatory);
        
        // Test upper bound (1.0 for excitatory, -1.0 for inhibitory)
        synapse.setWeight(10.0f);
        assert(synapse.getWeight() <= 1.0f);
        
        // Test lower bound
        synapse.setWeight(-10.0f);
        assert(synapse.getWeight() >= -1.0f);
        
        // Test inhibitory synapse bounds
        synapse.setType(nlm::SynapseType::Inhibitory);
        synapse.setWeight(10.0f);
        assert(synapse.getWeight() >= -1.0f);
        
        synapse.setWeight(-10.0f);
        assert(synapse.getWeight() <= 1.0f);
        
        std::cout << "    Synapse weight bounds validation test for " << getName() << " passed" << std::endl;
    }
};

class SynapseEfficacyValidationTest : public test_framework::TestBase {
public:
    SynapseEfficacyValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Unit) {}
    
    void run() override {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        // Test default efficacy
        assert(synapse.getEfficacy() == 1.0f);
        
        // Test setting efficacy
        synapse.setEfficacy(0.5f);
        assert(synapse.getEfficacy() == 0.5f);
        
        // Test efficacy bounds (0.0 to 2.0)
        synapse.setEfficacy(5.0f);
        assert(synapse.getEfficacy() <= 2.0f);
        
        synapse.setEfficacy(-1.0f);
        assert(synapse.getEfficacy() >= 0.0f);
        
        std::cout << "    Synapse efficacy validation test for " << getName() << " passed" << std::endl;
    }
};

class SynapseRandomInitializationValidationTest : public test_framework::TestBase {
public:
    SynapseRandomInitializationValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Unit) {}
    
    void run() override {
        nlm::RandomGenerator rng(42);
        
        // Test excitatory synapse random initialization
        nlm::Synapse excSynapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        excSynapse.setType(nlm::SynapseType::Excitatory);
        excSynapse.initializeRandom(rng);
        
        // Excitatory synapses should have positive weights
        assert(excSynapse.getWeight() > 0.0f);
        assert(excSynapse.getWeight() <= 1.0f);
        
        // Test inhibitory synapse random initialization
        nlm::Synapse inhSynapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
        inhSynapse.setType(nlm::SynapseType::Inhibitory);
        inhSynapse.initializeRandom(rng);
        
        // Inhibitory synapses should have negative weights
        assert(inhSynapse.getWeight() < 0.0f);
        assert(inhSynapse.getWeight() >= -1.0f);
        
        // Test delay range (should be between 1 and 5)
        assert(inhSynapse.getDelay() >= 1 && inhSynapse.getDelay() <= 5);
        
        std::cout << "    Synapse random initialization validation test for " << getName() << " passed" << std::endl;
    }
};

class SynapseEligibilityTraceValidationTest : public test_framework::TestBase {
public:
    SynapseEligibilityTraceValidationTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Unit) {}
    
    void run() override {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        // Test initial trace
        assert(synapse.getEligibilityTrace() == 0.0f);
        
        // Test setting eligibility trace
        synapse.setEligibilityTrace(0.7f);
        assert(synapse.getEligibilityTrace() == 0.7f);
        
        // Test decay
        float traceBefore = synapse.getEligibilityTrace();
        synapse.decayEligibilityTrace(0.1f);
        float traceAfter = synapse.getEligibilityTrace();
        assert(traceAfter < traceBefore);
        
        // Test trace bounds (should remain between 0 and 1)
        synapse.setEligibilityTrace(2.0f);
        assert(synapse.getEligibilityTrace() <= 1.0f);
        
        synapse.setEligibilityTrace(-1.0f);
        assert(synapse.getEligibilityTrace() >= 0.0f);
        
        std::cout << "    Synapse eligibility trace validation test for " << getName() << " passed" << std::endl;
    }
};

// Unit Tests (Original tests)
void testSynapseCreation() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getId() == nlm::SynapseId(1));
    assert(synapse.getSourceNeuron() == nlm::NeuronId(1));
    assert(synapse.getDestinationNeuron() == nlm::NeuronId(2));
    
    std::cout << "    testSynapseCreation passed" << std::endl;
}

void testSynapseWeight() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getWeight() == 0.0f);
    
    synapse.setWeight(0.5f);
    assert(synapse.getWeight() == 0.5f);
    
    synapse.addToWeight(0.2f);
    assert(synapse.getWeight() == 0.7f);
    
    synapse.addToWeight(-0.3f);
    assert(synapse.getWeight() == 0.4f);
    
    std::cout << "    testSynapseWeight passed" << std::endl;
}

void testSynapseWeightBounds() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Test upper bound
    synapse.setWeight(10.0f);
    assert(synapse.getWeight() <= 1.0f);  // Should be clamped
    
    // Test lower bound
    synapse.setWeight(-10.0f);
    assert(synapse.getWeight() >= -1.0f);  // Should be clamped
    
    std::cout << "    testSynapseWeightBounds passed" << std::endl;
}

void testSynapseType() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    assert(synapse.getType() == nlm::SynapseType::Excitatory);
    assert(synapse.isExcitatory());
    assert(!synapse.isInhibitory());
    
    synapse.setType(nlm::SynapseType::Inhibitory);
    assert(synapse.getType() == nlm::SynapseType::Inhibitory);
    assert(synapse.isInhibitory());
    assert(!synapse.isExcitatory());
    
    std::cout << "    testSynapseType passed" << std::endl;
}

void testSynapseDelay() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getDelay() == 1);  // Default
    
    synapse.setDelay(5);
    assert(synapse.getDelay() == 5);
    
    std::cout << "    testSynapseDelay passed" << std::endl;
}

void testSynapseSpikeHistory() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    synapse.recordPreSpike(0.1);
    synapse.recordPreSpike(0.2);
    synapse.recordPostSpike(0.15);
    
    assert(synapse.getPreSpikeHistory().size() == 2);
    assert(synapse.getPostSpikeHistory().size() == 1);
    
    synapse.clearHistory();
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    
    std::cout << "    testSynapseSpikeHistory passed" << std::endl;
}

void testSynapseEligibilityTrace() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getEligibilityTrace() == 0.0f);
    
    synapse.setEligibilityTrace(0.5f);
    assert(synapse.getEligibilityTrace() == 0.5f);
    
    // Test decay
    synapse.decayEligibilityTrace(0.1f);
    assert(synapse.getEligibilityTrace() < 0.5f);
    
    std::cout << "    testSynapseEligibilityTrace passed" << std::endl;
}

void testSynapseEfficacy() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(synapse.getEfficacy() == 1.0f);  // Default
    
    synapse.setEfficacy(0.5f);
    assert(synapse.getEfficacy() == 0.5f);
    
    // Test bounds
    synapse.setEfficacy(5.0f);
    assert(synapse.getEfficacy() <= 2.0f);  // Clamped
    
    std::cout << "    testSynapseEfficacy passed" << std::endl;
}

void testSynapseRandomInitialization() {
    nlm::RandomGenerator rng(42);
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.initializeRandom(rng);
    
    // Weight should be positive for excitatory
    assert(synapse.getWeight() > 0.0f);
    
    // Delay should be in valid range
    assert(synapse.getDelay() >= 1 && synapse.getDelay() <= 5);
    
    std::cout << "    testSynapseRandomInitialization passed" << std::endl;
}

void testSynapseReset() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    synapse.setWeight(0.5f);
    synapse.recordPreSpike(0.1);
    synapse.recordPostSpike(0.2);
    synapse.setEligibilityTrace(0.3f);
    
    synapse.reset();
    
    assert(synapse.getWeight() == 0.0f);
    assert(synapse.getPreSpikeHistory().empty());
    assert(synapse.getPostSpikeHistory().empty());
    assert(synapse.getEligibilityTrace() == 0.0f);
    
    std::cout << "    testSynapseReset passed" << std::endl;
}

void testSynapsePlasticityFlags() {
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    assert(!synapse.getPlasticityFlags().hebbian);
    assert(!synapse.getPlasticityFlags().stdp);
    
    synapse.enablePlasticity(true, true, false);
    
    assert(synapse.getPlasticityFlags().hebbian);
    assert(synapse.getPlasticityFlags().stdp);
    assert(!synapse.getPlasticityFlags().reward_modulated);
    
    std::cout << "    testSynapsePlasticityFlags passed" << std::endl;
}

void runAll() {
    std::cout << "=== Running Enhanced Synapse Tests ===" << std::endl;
    
    // Run original unit tests
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
    
    std::cout << "\n=== Running Synapse Validation Tests ===" << std::endl;
    
    // Run validation tests
    std::vector<std::pair<nlm::SynapseType, float>> synapseConfigs = {
        {nlm::SynapseType::Excitatory, 0.5f},
        {nlm::SynapseType::Inhibitory, -0.3f},
        {nlm::SynapseType::Modulatory, 0.2f}
    };
    
    for (size_t i = 0; i < synapseConfigs.size(); ++i) {
        SynapsePlasticityValidationTest test(
            "plasticity_validation_" + std::to_string(i),
            synapseConfigs[i].first, synapseConfigs[i].second
        );
        test.run();
    }
    
    // Run other validation tests
    SynapseWeightBoundsValidationTest weightBoundsTest("weight_bounds_validation");
    weightBoundsTest.run();
    
    SynapseEfficacyValidationTest efficacyTest("efficacy_validation");
    efficacyTest.run();
    
    SynapseRandomInitializationValidationTest randomInitTest("random_init_validation");
    randomInitTest.run();
    
    SynapseEligibilityTraceValidationTest eligibilityTest("eligibility_trace_validation");
    eligibilityTest.run();
    
    std::cout << "\n=== All Synapse Tests Completed ===" << std::endl;
}

} // namespace test_synapse