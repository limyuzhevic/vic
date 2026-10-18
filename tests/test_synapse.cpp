// Synapse Tests with improved error handling and comprehensive coverage
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <stdexcept>

namespace test_synapse {

void testSynapseCreation() {
    try {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        assert(synapse.getId() == nlm::SynapseId(1));
        assert(synapse.getSourceNeuron() == nlm::NeuronId(1));
        assert(synapse.getDestinationNeuron() == nlm::NeuronId(2));
        
        std::cout << "    testSynapseCreation passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "    testSynapseCreation FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testSynapseWeight() {
    try {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        assert(synapse.getWeight() == 0.0f);
        
        synapse.setWeight(0.5f);
        assert(synapse.getWeight() == 0.5f);
        
        synapse.addToWeight(0.2f);
        assert(synapse.getWeight() == 0.7f);
        
        synapse.addToWeight(-0.3f);
        assert(synapse.getWeight() == 0.4f);
        
        std::cout << "    testSynapseWeight passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "    testSynapseWeight FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testSynapseWeightBounds() {
    try {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        // Test upper bound
        synapse.setWeight(10.0f);
        assert(synapse.getWeight() <= 1.0f);  // Should be clamped
        
        // Test lower bound
        synapse.setWeight(-10.0f);
        assert(synapse.getWeight() >= -1.0f);  // Should be clamped
        
        std::cout << "    testSynapseWeightBounds passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "    testSynapseWeightBounds FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testSynapseType() {
    try {
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
    } catch (const std::exception& e) {
        std::cout << "    testSynapseType FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testSynapseDelay() {
    try {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        assert(synapse.getDelay() == 1);  // Default
        
        synapse.setDelay(5);
        assert(synapse.getDelay() == 5);
        
        std::cout << "    testSynapseDelay passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "    testSynapseDelay FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testSynapseSpikeHistory() {
    try {
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
    } catch (const std::exception& e) {
        std::cout << "    testSynapseSpikeHistory FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testSynapseEligibilityTrace() {
    try {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        assert(synapse.getEligibilityTrace() == 0.0f);
        
        synapse.setEligibilityTrace(0.5f);
        assert(synapse.getEligibilityTrace() == 0.5f);
        
        // Test decay
        synapse.decayEligibilityTrace(0.1f);
        assert(synapse.getEligibilityTrace() < 0.5f);
        
        std::cout << "    testSynapseEligibilityTrace passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "    testSynapseEligibilityTrace FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testSynapseEfficacy() {
    try {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        assert(synapse.getEfficacy() == 1.0f);  // Default
        
        synapse.setEfficacy(0.5f);
        assert(synapse.getEfficacy() == 0.5f);
        
        // Test bounds
        synapse.setEfficacy(5.0f);
        assert(synapse.getEfficacy() <= 2.0f);  // Clamped
        
        std::cout << "    testSynapseEfficacy passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "    testSynapseEfficacy FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testSynapseRandomInitialization() {
    try {
        nlm::RandomGenerator rng(42);
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.initializeRandom(rng);
        
        // Weight should be positive for excitatory
        assert(synapse.getWeight() > 0.0f);
        
        // Delay should be in valid range
        assert(synapse.getDelay() >= 1 && synapse.getDelay() <= 5);
        
        std::cout << "    testSynapseRandomInitialization passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "    testSynapseRandomInitialization FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testSynapseReset() {
    try {
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
    } catch (const std::exception& e) {
        std::cout << "    testSynapseReset FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testSynapsePlasticityFlags() {
    try {
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        
        assert(!synapse.getPlasticityFlags().hebbian);
        assert(!synapse.getPlasticityFlags().stdp);
        
        synapse.enablePlasticity(true, true, false);
        
        assert(synapse.getPlasticityFlags().hebbian);
        assert(synapse.getPlasticityFlags().stdp);
        assert(!synapse.getPlasticityFlags().reward_modulated);
        
        std::cout << "    testSynapsePlasticityFlags passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "    testSynapsePlasticityFlags FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testSynapseEdgeCases() {
    try {
        // Test with null neuron IDs (should be handled gracefully)
        nlm::Synapse synapse(nlm::INVALID_NEURON_ID, nlm::INVALID_NEURON_ID, nlm::INVALID_NEURON_ID);
        
        // Should not crash
        assert(synapse.getWeight() == 0.0f);
        
        // Test very small weights
        synapse.setWeight(0.000001f);
        assert(synapse.getWeight() > 0.0f);
        
        // Test very large delays
        synapse.setDelay(100);
        assert(synapse.getDelay() == 100);
        
        std::cout << "    testSynapseEdgeCases passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "    testSynapseEdgeCases FAILED: " << e.what() << std::endl;
        throw;
    }
}

void testSynapseMemoryManagement() {
    try {
        // Test that multiple synapse objects don't interfere
        nlm::Synapse synapse1(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        nlm::Synapse synapse2(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
        
        // Each should have independent state
        synapse1.setWeight(0.5f);
        synapse2.setWeight(0.8f);
        
        assert(synapse1.getWeight() == 0.5f);
        assert(synapse2.getWeight() == 0.8f);
        
        std::cout << "    testSynapseMemoryManagement passed" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "    testSynapseMemoryManagement FAILED: " << e.what() << std::endl;
        throw;
    }
}

void runAll() {
    std::cout << "Running Synapse tests..." << std::endl;
    
    try {
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
        testSynapseEdgeCases();
        testSynapseMemoryManagement();
        
        std::cout << "All Synapse tests PASSED!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Synapse tests FAILED: " << e.what() << std::endl;
        throw;
    }
}

} // namespace test_synapse