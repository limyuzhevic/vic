// Plasticity Suite Integration Tests
// End-to-end testing of plasticity components within brain simulation

#include "brain/Brain.hpp"
#include "plasticity/PlasticityRule.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>

namespace test_plasticity_suite {

void testBrainWithPlasticity() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    config->set("learning_rate", 0.01f, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run simulation with plasticity
    for (nlm::SimulationStep step = 0; step < 20; ++step) {
        brain.step(step);
        
        // Simulate spike generation (simplified)
        auto neurons = brain.getAllNeurons();
        for (auto* neuron : neurons) {
            if (step % 5 == 0) {  // Some neurons spike
                neuron->spike();
            }
        }
        
        brain.step(step);
    }
    
    // Verify brain still in valid state
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testBrainWithPlasticity passed" << std::endl;
}

void testPlasticityRuleLearningDynamics() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Get some synapses
    auto allSynapses = brain.getAllSynapses();
    assert(!allSynapses.empty());
    
    // Record initial weights
    std::vector<float> initialWeights;
    for (auto* synapse : allSynapses) {
        if (synapse->isPlastic() && synapse->getType() == nlm::SynapseType::Excitatory) {
            initialWeights.push_back(synapse->getWeight());
        }
    }
    
    assert(!initialWeights.empty());
    
    // Run learning simulation
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Simulate correlated activity
        if (step % 10 == 0) {
            auto neurons = brain.getAllNeurons();
            std::vector<nlm::NeuronId> preNeurons, postNeurons;
            
            // Select pre and post neurons
            if (neurons.size() >= 2) {
                preNeurons.push_back(neurons[0]->getId());
                postNeurons.push_back(neurons[1]->getId());
                
                // Create spike pairs with STDP-like timing
                std::vector<nlm::Timestamp> preSpikes = {static_cast<nlm::Timestamp>(step * 10.0f)};
                std::vector<nlm::Timestamp> postSpikes = {static_cast<nlm::Timestamp>(step * 10.0f + 5.0f)}; // Post after pre
                
                // Find synapses between these neurons
                for (auto* synapse : allSynapses) {
                    if (synapse->getSourceNeuron() == preNeurons[0] && 
                        synapse->getDestinationNeuron() == postNeurons[0]) {
                        
                        // Apply STDP using the synapse's plasticity rule
                        auto rule = synapse->getPlasticityRule();
                        if (rule && std::string(rule->getName()) == "STDP") {
                            static_cast<nlm::STDP*>(rule)->update(synapse, preSpikes, postSpikes, 0.001f);
                        }
                    }
                }
            }
        }
        
        brain.step(step);
    }
    
    // Verify plasticity occurred
    float maxWeightChange = 0.0f;
    for (size_t i = 0; i < initialWeights.size(); ++i) {
        float currentWeight = allSynapses[i]->getWeight();
        float change = std::abs(currentWeight - initialWeights[i]);
        maxWeightChange = std::max(maxWeightChange, change);
    }
    
    // Weight should have changed due to plasticity
    assert(maxWeightChange > 1e-6f);
    
    std::cout << "    testPlasticityRuleLearningDynamics passed (max Δ=" 
              << maxWeightChange << ")" << std::endl;
}

void testPlasticityRuleHebbianCorrelation() {
    nlm::Hebbian hebbian;
    
    // Create test synapses
    std::vector<nlm::Synapse*> testSynapses;
    for (int i = 0; i < 10; ++i) {
        nlm::Synapse* synapse = new nlm::Synapse(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1));
        synapse->setType(nlm::SynapseType::Excitatory);
        synapse->setWeight(0.5f);
        testSynapses.push_back(synapse);
    }
    
    // Test with varying correlation levels
    std::vector<std::pair<std::vector<nlm::Timestamp>, std::vector<nlm::Timestamp>>> spikePatterns = {
        // High correlation (synchronous)
        {{100.0f, 200.0f}, {100.0f, 200.0f}},
        // Medium correlation (50ms offset)
        {{100.0f, 200.0f}, {150.0f, 250.0f}},
        // Low correlation (outside window)
        {{100.0f, 200.0f}, {500.0f, 600.0f}},
        // Empty spikes
        {{}, {100.0f, 200.0f}},
        // Single spikes
        {{100.0f}, {100.0f}},
    };
    
    for (size_t i = 0; i < spikePatterns.size(); ++i) {
        float initialWeight = testSynapses[i % testSynapses.size()]->getWeight();
        
        hebbian.update(testSynapses[i % testSynapses.size()], 
                      spikePatterns[i].first, 
                      spikePatterns[i].second, 
                      0.001f);
        
        float finalWeight = testSynapses[i % testSynapses.size()]->getWeight();
        
        // Weight change should be proportional to correlation
        if (spikePatterns[i].first.empty() || spikePatterns[i].second.empty()) {
            // Empty spikes should not change weight
            assert(finalWeight == initialWeight);
        } else {
            // Weight should change for non-empty spikes
            assert(finalWeight != initialWeight);
        }
    }
    
    // Cleanup
    for (auto* synapse : testSynapses) {
        delete synapse;
    }
    
    std::cout << "    testPlasticityRuleHebbianCorrelation passed" << std::endl;
}

void testPlasticityRulePerformanceValidation() {
    nlm::STDP stdp;
    nlm::Hebbian hebbian;
    
    const size_t largeSpikeCount = 1000;
    const size_t smallSpikeCount = 10;
    
    // Create test spikes
    std::vector<nlm::Timestamp> largePreSpikes;
    std::vector<nlm::Timestamp> largePostSpikes;
    std::vector<nlm::Timestamp> smallPreSpikes;
    std::vector<nlm::Timestamp> smallPostSpikes;
    
    for (size_t i = 0; i < largeSpikeCount; ++i) {
        largePreSpikes.push_back(static_cast<nlm::Timestamp>(i * 10.0f));
        largePostSpikes.push_back(static_cast<nlm::Timestamp>(i * 10.0f + 5.0f));
    }
    
    for (size_t i = 0; i < smallSpikeCount; ++i) {
        smallPreSpikes.push_back(static_cast<nlm::Timestamp>(i * 10.0f));
        smallPostSpikes.push_back(static_cast<nlm::Timestamp>(i * 10.0f + 5.0f));
    }
    
    nlm::Synapse largeSynapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    largeSynapse.setType(nlm::SynapseType::Excitatory);
    largeSynapse.setWeight(0.5f);
    
    nlm::Synapse smallSynapse(nlm::SynapseId(2), nlm::NeuronId(3), nlm::NeuronId(4));
    smallSynapse.setType(nlm::SynapseType::Excitatory);
    smallSynapse.setWeight(0.5f);
    
    // Test STDP performance with large spike counts
    stdp.update(&largeSynapse, largePreSpikes, largePostSpikes, 0.001f);
    assert(largeSynapse.getWeight() >= 0.5f);  // Should potentiate
    
    // Test Hebbian performance with large spike counts
    hebbian.update(&largeSynapse, largePreSpikes, largePostSpikes, 0.001f);
    
    // Test with small spike counts
    stdp.update(&smallSynapse, smallPreSpikes, smallPostSpikes, 0.001f);
    hebbian.update(&smallSynapse, smallPreSpikes, smallPostSpikes, 0.001f);
    
    std::cout << "    testPlasticityRulePerformanceValidation passed" << std::endl;
}

void testPlasticityRuleTimeWindowAccuracy() {
    nlm::STDP stdp;
    
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    
    float initialWeight = synapse.getWeight();
    
    // Test with spikes within time window (within 3τ = 60ms)
    std::vector<nlm::Timestamp> preSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> postSpikes = {5.0, 15.0, 25.0};  // +5ms offsets
    
    stdp.update(&synapse, preSpikes, postSpikes, 0.001f);
    float weightAfterWindow = synapse.getWeight();
    
    // Test with spikes outside cutoff (60ms threshold)
    std::vector<nlm::Timestamp> farPreSpikes = {0.0, 10.0, 20.0};
    std::vector<nlm::Timestamp> farPostSpikes = {70.0, 80.0, 90.0};  // >60ms offsets
    
    // Reset for fair comparison
    synapse.setWeight(0.5f);
    stdp.update(&synapse, farPreSpikes, farPostSpikes, 0.001f);
    float weightAfterCutoff = synapse.getWeight();
    
    // Weight change should be significantly smaller for far spikes
    float deltaWindow = std::abs(weightAfterWindow - initialWeight);
    float deltaCutoff = std::abs(weightAfterCutoff - initialWeight);
    
    // Delta window should be larger than delta cutoff (exponential decay)
    assert(deltaWindow > deltaCutoff);
    
    std::cout << "    testPlasticityRuleTimeWindowAccuracy passed" << std::endl;
}

void runAll() {
    std::cout << "Running PlasticitySuite integration tests..." << std::endl;
    testBrainWithPlasticity();
    testPlasticityRuleLearningDynamics();
    testPlasticityRuleHebbianCorrelation();
    testPlasticityRulePerformanceValidation();
    testPlasticityRuleTimeWindowAccuracy();
}

} // namespace test_plasticity_suite
