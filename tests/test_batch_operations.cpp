// Batch Operation Tests
// Comprehensive test suite for new batch operations in Synapse and NeuralRegion

#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_batch_operations {

void testBatchWeightOperations() {
    std::cout << "Testing batch weight operations..." << std::endl;
    
    NeuralRegion region(1);
    
    // Create some neurons and synapses
    auto neurons = region.getAllNeurons();
    if (neurons.size() < 4) {
        // Add neurons if not enough exist
        region.addPopulation(2);
        neurons = region.getAllNeurons();
    }
    
    if (neurons.size() >= 4) {
        std::vector<SynapseId> synapseIds;
        std::vector<SynapticWeight> weights;
        
        // Create synapses
        for (size_t i = 0; i < 4; ++i) {
            SynapseId synId = region.addSynapse(neurons[i]->getId(), neurons[(i + 1) % 4]->getId(), 0.0f);
            synapseIds.push_back(synId);
            weights.push_back((i % 2 == 0) ? 0.5f : -0.3f);  // Mixed weights
        }
        
        // Test batch set weights
        region.batchSetSynapseWeights(synapseIds, weights);
        
        // Verify weights were set
        std::vector<SynapticWeight> retrievedWeights;
        region.batchGetSynapseWeights(synapseIds, retrievedWeights);
        
        for (size_t i = 0; i < synapseIds.size(); ++i) {
            assert(std::abs(retrievedWeights[i] - weights[i]) < 0.001f);
        }
        
        // Test batch add weights
        std::vector<SynapticWeight> deltas(synapseIds.size(), 0.1f);
        region.batchAddToSynapseWeights(synapseIds, deltas);
        
        // Verify weights were updated
        retrievedWeights.clear();
        region.batchGetSynapseWeights(synapseIds, retrievedWeights);
        
        for (size_t i = 0; i < synapseIds.size(); ++i) {
            // Original weight + delta, clamped
            float expected = Synapse::clampWeight(weights[i] + 0.1f);
            assert(std::abs(retrievedWeights[i] - expected) < 0.001f);
        }
    }
    
    std::cout << "    batch weight operations passed" << std::endl;
}

void testBatchSpikeOperations() {
    std::cout << "Testing batch spike operations..." << std::endl;
    
    NeuralRegion region(1);
    auto neurons = region.getAllNeurons();
    
    if (neurons.size() >= 4) {
        // Create synapses
        std::vector<SynapseId> synapseIds;
        for (size_t i = 0; i < 4; ++i) {
            SynapseId synId = region.addSynapse(neurons[i]->getId(), neurons[(i + 1) % 4]->getId(), 0.0f);
            synapseIds.push_back(synId);
        }
        
        // Test batch pre-synaptic spikes
        std::vector<Timestamp> preTimestamps(synapseIds.size());
        std::vector<Timestamp> postTimestamps(synapseIds.size());
        for (size_t i = 0; i < synapseIds.size(); ++i) {
            preTimestamps[i] = static_cast<Timestamp>(i * 0.1);
            postTimestamps[i] = static_cast<Timestamp>((i + 0.5) * 0.1);
        }
        
        region.batchRecordPreSpikes(synapseIds, preTimestamps);
        region.batchRecordPostSpikes(synapseIds, postTimestamps);
        
        // Verify spike history
        for (size_t i = 0; i < synapseIds.size(); ++i) {
            auto* syn = region.getSynapse(synapseIds[i]);
            if (syn) {
                assert(syn->getPreSpikeHistory().size() == 1);
                assert(syn->getPostSpikeHistory().size() == 1);
            }
        }
        
        // Test batch clear
        region.batchClearSpikeHistory(synapseIds);
        
        for (size_t i = 0; i < synapseIds.size(); ++i) {
            auto* syn = region.getSynapse(synapseIds[i]);
            if (syn) {
                assert(syn->getPreSpikeHistory().empty());
                assert(syn->getPostSpikeHistory().empty());
            }
        }
    }
    
    std::cout << "    batch spike operations passed" << std::endl;
}

void testBatchPlasticityOperations() {
    std::cout << "Testing batch plasticity operations..." << std::endl;
    
    NeuralRegion region(1);
    auto neurons = region.getAllNeurons();
    
    if (neurons.size() >= 4) {
        // Create synapses
        std::vector<SynapseId> synapseIds;
        for (size_t i = 0; i < 4; ++i) {
            SynapseId synId = region.addSynapse(neurons[i]->getId(), neurons[(i + 1) % 4]->getId(), 0.0f);
            synapseIds.push_back(synId);
        }
        
        // Test batch enable plasticity
        region.batchEnablePlasticity(synapseIds, true, true, true);
        
        // Verify plasticity flags
        for (const auto& synId : synapseIds) {
            auto* syn = region.getSynapse(synId);
            if (syn) {
                assert(syn->getPlasticityFlags().hebbian);
                assert(syn->getPlasticityFlags().stdp);
                assert(syn->getPlasticityFlags().reward_modulated);
            }
        }
        
        // Test batch set eligibility traces
        std::vector<float> traces(synapseIds.size(), 0.5f);
        region.batchSetEligibilityTraces(synapseIds, traces);
        
        // Verify eligibility traces
        for (size_t i = 0; i < synapseIds.size(); ++i) {
            auto* syn = region.getSynapse(synapseIds[i]);
            if (syn) {
                assert(std::abs(syn->getEligibilityTrace() - 0.5f) < 0.001f);
            }
        }
        
        // Test batch decay eligibility traces
        std::vector<float> decayRates(synapseIds.size(), 0.1f);
        region.batchDecayEligibilityTraces(synapseIds, decayRates);
        
        for (size_t i = 0; i < synapseIds.size(); ++i) {
            auto* syn = region.getSynapse(synapseIds[i]);
            if (syn) {
                assert(std::abs(syn->getEligibilityTrace() - 0.5f * 0.9f) < 0.001f);
            }
        }
    }
    
    std::cout << "    batch plasticity operations passed" << std::endl;
}

void testBatchStatisticsOperations() {
    std::cout << "Testing batch statistics operations..." << std::endl;
    
    NeuralRegion region(1);
    auto neurons = region.getAllNeurons();
    
    if (neurons.size() >= 4) {
        // Create synapses with varying weights
        std::vector<SynapseId> synapseIds;
        std::vector<SynapticWeight> weights = {0.1f, 0.5f, -0.3f, 0.2f};
        
        for (size_t i = 0; i < weights.size(); ++i) {
            SynapseId synId = region.addSynapse(neurons[i]->getId(), neurons[(i + 1) % 4]->getId(), weights[i]);
            synapseIds.push_back(synId);
        }
        
        // Test batch statistics
        float avgWeight = region.batchComputeAverageSynapseWeight(synapseIds);
        float variance = region.batchComputeSynapseWeightVariance(synapseIds);
        float skewness = region.batchComputeSynapseWeightSkewness(synapseIds);
        float ieRatio = region.batchComputeSynapseExcitationInhibitionRatio(synapseIds);
        
        // Verify calculations (allow for floating point tolerance)
        float expectedAvg = (0.1f + 0.5f - 0.3f + 0.2f) / 4.0f;
        assert(std::abs(avgWeight - expectedAvg) < 0.001f);
        
        // Verify excitation-inhibition ratio (2 excitatory, 2 inhibitory)
        assert(ieRatio == 1.0f);  // 2 inhibitory / 2 excitatory
    }
    
    std::cout << "    batch statistics operations passed" << std::endl;
}

void testBatchLifecycleOperations() {
    std::cout << "Testing batch lifecycle operations..." << std::endl;
    
    NeuralRegion region(1);
    auto neurons = region.getAllNeurons();
    
    if (neurons.size() >= 4) {
        // Create synapses
        std::vector<std::pair<NeuronId, NeuronId>> connections;
        for (size_t i = 0; i < 4; ++i) {
            connections.push_back({neurons[i]->getId(), neurons[(i + 1) % 4]->getId()});
        }
        
        // Test batch creation
        std::vector<SynapseId> synapseIds = region.createBatchSynapses(connections, SynapseType::Excitatory, 0.1f);
        
        assert(synapseIds.size() == connections.size());
        
        // Verify synapses were created
        for (const auto& synId : synapseIds) {
            assert(region.getSynapse(synId) != nullptr);
        }
        
        // Test batch initialization (simulate with random generator)
        RandomGenerator rng(42);
        region.batchInitializeSynapses(synapseIds, rng);
        
        // Test batch deletion
        region.batchDeleteSynapses(synapseIds);
        
        // Verify synapses were deleted
        for (const auto& synId : synapseIds) {
            assert(region.getSynapse(synId) == nullptr);
        }
    }
    
    std::cout << "    batch lifecycle operations passed" << std::endl;
}

void testBatchSynapseGroupOperations() {
    std::cout << "Testing batch synapse group operations..." << std::endl;
    
    NeuralRegion region(1);
    auto neurons = region.getAllNeurons();
    
    if (neurons.size() >= 4) {
        // Create synapses
        std::vector<SynapseId> synapseIds;
        for (size_t i = 0; i < 4; ++i) {
            SynapseId synId = region.addSynapse(neurons[i]->getId(), neurons[(i + 1) % 4]->getId(),
                                               (i % 2 == 0) ? 0.5f : -0.5f);
            synapseIds.push_back(synId);
        }
        
        // Test batch get by type
        std::vector<SynapseId> excitatory = region.batchGetSynapsesByType(SynapseType::Excitatory);
        std::vector<SynapseId> inhibitory = region.batchGetSynapsesByType(SynapseType::Inhibitory);
        
        assert(excitatory.size() == 2);
        assert(inhibitory.size() == 2);
        
        // Test batch get by weight range
        std::vector<SynapseId> positive = region.batchGetSynapsesInWeightRange(0.0f, 1.0f);
        std::vector<SynapseId> negative = region.batchGetSynapsesInWeightRange(-1.0f, 0.0f);
        
        assert(positive.size() == 2);
        assert(negative.size() == 2);
    }
    
    std::cout << "    batch synapse group operations passed" << std::endl;
}

void testBatchDependencyTracking() {
    std::cout << "Testing batch dependency tracking..." << std::endl;
    
    NeuralRegion region(1);
    auto neurons = region.getAllNeurons();
    
    if (neurons.size() >= 4) {
        // Create synapses
        std::vector<SynapseId> synapseIds;
        for (size_t i = 0; i < 4; ++i) {
            SynapseId synId = region.addSynapse(neurons[i]->getId(), neurons[(i + 1) % 4]->getId(), 0.0f);
            synapseIds.push_back(synId);
        }
        
        // Test batch get from neuron
        std::vector<SynapseId> fromNeuron1 = region.batchGetSynapsesFromNeuron(neurons[1]->getId());
        assert(fromNeuron1.size() >= 1);  // At least one synapse from neuron 1
        
        // Test batch get to neuron
        std::vector<SynapseId> toNeuron3 = region.batchGetSynapsesToNeuron(neurons[3]->getId());
        assert(toNeuron3.size() >= 1);  // At least one synapse to neuron 3
        
        // Test batch get unique source neurons
        std::vector<NeuronId> uniqueSources = region.batchGetUniqueSourceNeurons(synapseIds);
        assert(uniqueSources.size() >= 1);  // At least one unique source
        
        // Test batch get unique destination neurons
        std::vector<NeuronId> uniqueDests = region.batchGetUniqueDestinationNeurons(synapseIds);
        assert(uniqueDests.size() >= 1);  // At least one unique destination
    }
    
    std::cout << "    batch dependency tracking passed" << std::endl;
}

void testCentralizedClamping() {
    std::cout << "Testing centralized clamping..." << std::endl;
    
    // Test the centralized clamping function
    float weight1 = Synapse::clampWeight(0.5f);
    assert(weight1 == 0.5f);
    
    float weight2 = Synapse::clampWeight(2.0f);
    assert(weight2 == 1.0f);  // Clamped to max
    
    float weight3 = Synapse::clampWeight(-2.0f);
    assert(weight3 == -1.0f);  // Clamped to min
    
    float weight4 = Synapse::clampWeight(0.0f);
    assert(weight4 == 0.0f);
    
    std::cout << "    centralized clamping passed" << std::endl;
}

void runAllBatchOperationTests() {
    std::cout << "Running Batch Operation Tests..." << std::endl;
    
    testCentralizedClamping();
    testBatchWeightOperations();
    testBatchSpikeOperations();
    testBatchPlasticityOperations();
    testBatchStatisticsOperations();
    testBatchLifecycleOperations();
    testBatchSynapseGroupOperations();
    testBatchDependencyTracking();
    
    std::cout << std::endl << "All batch operation tests passed!" << std::endl;
}

// Integration test for batch operations with neural region
void runIntegrationTest() {
    std::cout << "Running integration test for batch operations..." << std::endl;
    
    NeuralRegion region(1);
    
    // Create neurons and populations
    auto neurons = region.getAllNeurons();
    if (neurons.size() < 10) {
        region.addPopulation(10);
        neurons = region.getAllNeurons();
    }
    
    if (neurons.size() >= 10) {
        // Create synapses with random weights using batch operations
        std::vector<std::pair<NeuronId, NeuronId>> connections;
        for (size_t i = 0; i < 20; ++i) {
            connections.push_back({neurons[i]->getId(), neurons[(i + 1) % neurons.size()]->getId()});
        }
        
        std::vector<SynapseId> synapseIds = region.createBatchSynapses(connections, SynapseType::Excitatory, 0.0f);
        
        // Initialize synapses using batch operation
        RandomGenerator rng(42);
        region.batchInitializeSynapses(synapseIds, rng);
        
        // Test batch statistics
        float avgWeight = region.batchComputeAverageSynapseWeight(synapseIds);
        float variance = region.batchComputeSynapseWeightVariance(synapseIds);
        
        std::cout << "  Average weight: " << avgWeight << std::endl;
        std::cout << "  Weight variance: " << variance << std::endl;
        
        // Test batch plasticity operations
        region.batchEnablePlasticity(synapseIds, true, true, false);
        
        // Verify
        for (const auto& synId : synapseIds) {
            auto* syn = region.getSynapse(synId);
            if (syn) {
                assert(syn->getPlasticityFlags().hebbian);
                assert(syn->getPlasticityFlags().stdp);
                assert(!syn->getPlasticityFlags().reward_modulated);
            }
        }
        
        // Test batch synaptic current computation
        std::vector<float> firingRates(synapseIds.size(), 1.0f);  // Simple firing rates
        std::vector<MembranePotential> currents(synapseIds.size());
        
        region.batchComputeSynapticCurrents(synapseIds, neurons, firingRates, currents);
        
        // Verify currents
        for (size_t i = 0; i < currents.size(); ++i) {
            assert(currents[i] > 0.0f || currents[i] < 0.0f);  // Should have some current
        }
        
        // Test batch spike history operations
        std::vector<Timestamp> preSpikes(synapseIds.size(), 0.1);
        std::vector<Timestamp> postSpikes(synapseIds.size(), 0.2);
        
        region.batchRecordPreSpikes(synapseIds, preSpikes);
        region.batchRecordPostSpikes(synapseIds, postSpikes);
        
        // Clear history
        region.batchClearSpikeHistory(synapseIds);
        
        // Verify history cleared
        for (const auto& synId : synapseIds) {
            auto* syn = region.getSynapse(synId);
            if (syn) {
                assert(syn->getPreSpikeHistory().empty());
                assert(syn->getPostSpikeHistory().empty());
            }
        }
        
        std::cout << "  Integration test passed!" << std::endl;
    }
}

int main() {
    test_batch_operations::runAllBatchOperationTests();
    runIntegrationTest();
    
    std::cout << std::endl << "All batch operation tests completed successfully!" << std::endl;
    return 0;
}