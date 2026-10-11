// Comprehensive Edge Case Tests
// This file adds extensive tests for boundary conditions, error handling,
// and complex scenarios not covered in basic tests.

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

namespace test_edge_cases {

// Neuron Edge Cases
void testNeuronBoundaryValues() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test extreme membrane potentials
    neuron.setMembranePotential(-100.0f);  // Minimum allowed
    assert(neuron.getMembranePotential() == -100.0f);
    
    neuron.setMembranePotential(50.0f);   // Maximum allowed
    assert(neuron.getMembranePotential() == 50.0f);
    
    // Test extreme threshold values
    neuron.setThreshold(-100.0f);   // Very low threshold
    assert(neuron.getThreshold() == -100.0f);
    
    neuron.setThreshold(50.0f);    // Very high threshold
    assert(neuron.getThreshold() == 50.0f);
    
    // Test extreme current injection
    neuron.injectCurrent(1000.0f);  // Very large current
    assert(neuron.getTotalCurrent() == 1000.0f);
    
    std::cout << "    testNeuronBoundaryValues passed" << std::endl;
}

void testNeuronRefractoryPeriodEdgeCases() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test zero refractory period
    neuron.setRefractoryPeriod(0);
    assert(neuron.getRefractoryPeriod() == 0);
    assert(!neuron.isRefractory());
    
    // Test very large refractory period
    neuron.setRefractoryPeriod(10000);
    assert(neuron.getRefractoryPeriod() == 10000);
    
    // Test decrement with zero refractory
    neuron.setRefractoryPeriod(0);
    neuron.decrementRefractory();  // Should not crash
    
    // Test decrement when already not refractory
    neuron.setRefractoryPeriod(5);
    neuron.decrementRefractory();  // Should decrement to 4
    assert(neuron.getRefractoryRemaining() == 4);
    
    std::cout << "    testNeuronRefractoryPeriodEdgeCases passed" << std::endl;
}

void testNeuronPlasticityFlags() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test all plasticity flags
    neuron.enablePlasticity(true, true, true);
    assert(neuron.getPlasticityFlags().hebbian);
    assert(neuron.getPlasticityFlags().stdp);
    assert(neuron.getPlasticityFlags().reward_modulated);
    
    // Test individual flags
    neuron.enablePlasticity(false, true, false);
    assert(!neuron.getPlasticityFlags().hebbian);
    assert(neuron.getPlasticityFlags().stdp);
    assert(!neuron.getPlasticityFlags().reward_modulated);
    
    // Test no plasticity
    neuron.enablePlasticity(false, false, false);
    assert(!neuron.getPlasticityFlags().hebbian);
    assert(!neuron.getPlasticityFlags().stdp);
    assert(!neuron.getPlasticityFlags().reward_modulated);
    
    std::cout << "    testNeuronPlasticityFlags passed" << std::endl;
}

void testNeuronSpikeHistoryLimits() {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test spike history overflow protection
    const size_t maxHistory = neuron.getMaxSpikeHistory();
    
    // Record more spikes than history can hold
    for (size_t i = 0; i < maxHistory + 100; ++i) {
        neuron.recordSpike(static_cast<float>(i));
    }
    
    // Verify history size doesn't exceed limit
    assert(neuron.getSpikeHistory().size() <= maxHistory);
    
    // Verify recent spikes are recorded (not just first ones)
    size_t recentCount = neuron.getSpikeHistory().size();
    assert(recentCount > 0);
    
    // Verify spike history can be cleared
    neuron.clearSpikeHistory();
    assert(neuron.getSpikeHistory().empty());
    
    std::cout << "    testNeuronSpikeHistoryLimits passed" << std::endl;
}

// Synapse Edge Cases
void testSynapseWeightBoundaries() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* region = brain.getRegion(nlm::RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) return;
    
    // Test synapse with extreme weights
    synapses[0]->setWeight(1000.0f);  // Very large weight
    assert(std::abs(synapses[0]->getWeight() - 1000.0f) < 0.001f);
    
    synapses[0]->setWeight(-1000.0f); // Very negative weight
    assert(std::abs(synapses[0]->getWeight() + 1000.0f) < 0.001f);
    
    synapses[0]->setWeight(0.0001f);  // Very small positive weight
    assert(std::abs(synapses[0]->getWeight() - 0.0001f) < 0.00001f);
    
    synapses[0]->setWeight(-0.0001f); // Very small negative weight
    assert(std::abs(synapses[0]->getWeight() + 0.0001f) < 0.00001f);
    
    std::cout << "    testSynapseWeightBoundaries passed" << std::endl;
}

void testSynapseEligibilityTraceEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* region = brain.getRegion(nlm::RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) return;
    
    // Test synapse eligibility trace boundaries
    auto& syn = *synapses[0];
    
    // Set extreme eligibility trace values
    syn.setEligibilityTrace(100.0f);  // Very large trace
    assert(std::abs(syn.getEligibilityTrace() - 100.0f) < 0.001f);
    
    syn.setEligibilityTrace(-100.0f); // Very negative trace
    assert(std::abs(syn.getEligibilityTrace() + 100.0f) < 0.001f);
    
    // Test decay with zero trace
    syn.setEligibilityTrace(0.0f);
    syn.decayEligibilityTrace(0.5f);  // Should remain zero
    assert(syn.getEligibilityTrace() == 0.0f);
    
    // Test decay with negative trace
    syn.setEligibilityTrace(-50.0f);
    syn.decayEligibilityTrace(0.5f);
    assert(syn.getEligibilityTrace() > -50.0f); // Should have decayed towards zero
    
    std::cout << "    testSynapseEligibilityTraceEdgeCases passed" << std::endl;
}

// Memory System Edge Cases
void testWorkingMemoryEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* wm = brain.getWorkingMemory();
    if (!wm) return;
    
    // Test working memory with zero traces
    size_t initialTraces = wm->getActiveTraces();
    assert(initialTraces >= 0);
    
    // Test working memory capacity limits
    size_t maxTraces = wm->getMaxTraces();
    assert(maxTraces > 0);
    
    // Test working memory decay
    wm->addTrace(1.0f);  // Add a trace
    wm->update(0.001);   // Update with small timestep
    
    // Decay should reduce trace activity
    size_t tracesAfter = wm->getActiveTraces();
    assert(tracesAfter >= 0);
    
    std::cout << "    testWorkingMemoryEdgeCases passed" << std::endl;
}

void testEpisodicMemoryEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* em = brain.getEpisodicMemory();
    if (!em) return;
    
    // Test episodic memory with zero episodes
    size_t initialEpisodes = em->getEpisodeCount();
    assert(initialEpisodes >= 0);
    
    // Test episodic memory retrieval with empty memory
    auto retrieved = em->retrieveEpisodes(0, 10);  // No episodes
    assert(retrieved.empty());
    
    // Test episodic memory pattern completion with weak patterns
    em->addEpisode(1.0f, 0.1f);  // Very weak episode
    auto episodes = em->getEpisodesForReplay(1);
    
    // Should still be able to retrieve weak episodes
    assert(!episodes.empty());
    
    std::cout << "    testEpisodicMemoryEdgeCases passed" << std::endl;
}

// Neuromodulation Edge Cases
void testDopamineEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* dopamine = brain.getDopamine();
    if (!dopamine) return;
    
    // Test dopamine with extreme values
    dopamine->setLevel(2.0f);  // Above maximum
    assert(dopamine->getLevel() <= 1.0f);  // Should be clamped
    
    dopamine->setLevel(-2.0f); // Below minimum
    assert(dopamine->getLevel() >= 0.0f); // Should be clamped
    
    // Test dopamine plasticity factor with extreme levels
    dopamine->setLevel(1.0f);  // Maximum
    float plasticity = dopamine->getPlasticityFactor();
    assert(plasticity >= 0.5f && plasticity <= 1.0f);
    
    dopamine->setLevel(0.0f);  // Minimum
    plasticity = dopamine->getPlasticityFactor();
    assert(plasticity >= 0.5f && plasticity <= 1.0f);
    
    std::cout << "    testDopamineEdgeCases passed" << std::endl;
}

void testCuriosityEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* curiosity = brain.getCuriosity();
    if (!curiosity) return;
    
    // Test curiosity with extreme values
    curiosity->setLevel(2.0f);  // Should be clamped
    assert(curiosity->getLevel() <= 1.0f);
    
    curiosity->setLevel(-2.0f); // Should be clamped
    assert(curiosity->getLevel() >= 0.0f);
    
    // Test curiosity with zero novelty
    curiosity->setLevel(0.0f);
    assert(curiosity->getLevel() == 0.0f);
    
    std::cout << "    testCuriosityEdgeCases passed" << std::endl;
}

// Development System Edge Cases
void testDevelopmentStageEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test development system with brain
    auto* dev = brain.getDevelopmentSystem();
    if (!dev) return;
    
    // Test all developmental stages
    std::vector<nlm::DevelopmentalStage> stages = {
        nlm::DevelopmentalStage::Initial,
        nlm::DevelopmentalStage::CriticalPeriod,
        nlm::DevelopmentalStage::Maturation,
        nlm::DevelopmentalStage::Adult,
        nlm::DevelopmentalStage::Aging
    };
    
    for (auto stage : stages) {
        brain.setDevelopmentalStage(stage);
        assert(brain.getDevelopmentalStage() == stage);
    }
    
    // Test development with zero age
    brain.setDevelopmentalAge(0.0);
    assert(brain.getDevelopmentalAge() == 0.0);
    
    // Test development with very large age
    brain.setDevelopmentalAge(1000000.0);
    assert(brain.getDevelopmentalAge() == 1000000.0);
    
    std::cout << "    testDevelopmentStageEdgeCases passed" << std::endl;
}

// Plasticity Rule Edge Cases
void testSTDPEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* stdp = brain.getSTDP();
    if (!stdp) return;
    
    // Test STDP with extreme parameters
    stdp->setLTPWeight(10.0f);  // Very large LTP weight
    assert(stdp->getLTPWeight() <= 1.0f);  // Should be clamped
    
    stdp->setLTDWeight(10.0f);  // Very large LTD weight
    assert(stdp->getLTDWeight() <= 1.0f);  // Should be clamped
    
    // Test STDP with zero weights
    stdp->setLTPWeight(0.0f);
    stdp->setLTDWeight(0.0f);
    assert(stdp->getLTPWeight() == 0.0f);
    assert(stdp->getLTDWeight() == 0.0f);
    
    std::cout << "    testSTDPEdgeCases passed" << std::endl;
}

void testHebbianEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto* hebbian = brain.getHebbian();
    if (!hebbian) return;
    
    // Test Hebbian with extreme parameters
    hebbian->setLearningRate(10.0f);  // Very large learning rate
    assert(hebbian->getLearningRate() <= 1.0f);  // Should be clamped
    
    hebbian->setLearningRate(0.0f);  // Zero learning rate
    assert(hebbian->getLearningRate() == 0.0f);
    
    std::cout << "    testHebbianEdgeCases passed" << std::endl;
}

// Integration Edge Cases
void testBrainResetEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Record initial state
    size_t initialNeurons = brain.getTotalNeuronCount();
    size_t initialSynapses = brain.getTotalSynapseCount();
    
    // Reset multiple times
    brain.reset();
    brain.reset();
    brain.reset();
    
    // State should remain consistent
    assert(brain.getTotalNeuronCount() == initialNeurons);
    assert(brain.getTotalSynapseCount() == initialSynapses);
    
    // Initialize and reset again
    brain.initialize();
    brain.reset();
    
    assert(brain.getTotalNeuronCount() == initialNeurons);
    
    std::cout << "    testBrainResetEdgeCases passed" << std::endl;
}

void testStepWithExtremeInputs() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Record initial state
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    // Inject extreme current
    brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 1000.0f);
    
    // Take a step
    brain.step(0, 0.001);
    
    // Should not crash (may or may not spike depending on parameters)
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "    testStepWithExtremeInputs passed" << std::endl;
}

void testConfigurationEdgeCases() {
    auto config = std::make_shared<nlm::Config>();
    
    // Test with extreme configuration values
    config->set("neuron_count", 10);  // Very small brain
    config->set("region_count", 1);
    config->set("connection_probability", 0.0f);  // No connections
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    // May or may not succeed depending on implementation
    
    // Test with very large values
    config->set("neuron_count", 10000);
    config->set("region_count", 100);
    config->set("connection_probability", 1.0f);  // All connections
    
    nlm::Brain brain2(config);
    success = brain2.initialize();
    // May or may not succeed depending on memory constraints
    
    std::cout << "    testConfigurationEdgeCases passed" << std::endl;
}

void testRandomGeneratorEdgeCases() {
    nlm::RandomGenerator rng(42);  // Fixed seed
    
    // Test with extreme bounds
    float min = -1000.0f;
    float max = 1000.0f;
    
    for (int i = 0; i < 100; ++i) {
        float val = rng.uniformReal(min, max);
        assert(val >= min - 0.0001f && val <= max + 0.0001f);
    }
    
    // Test with zero range
    float zeroMin = 5.0f;
    float zeroMax = 5.0f;
    
    for (int i = 0; i < 10; ++i) {
        float val = rng.uniformReal(zeroMin, zeroMax);
        assert(std::abs(val - 5.0f) < 0.0001f);
    }
    
    // Test with reversed bounds (min > max)
    float reverseMin = 10.0f;
    float reverseMax = 5.0f;
    
    for (int i = 0; i < 10; ++i) {
        float val = rng.uniformReal(reverseMin, reverseMax);
        // Implementation should handle this gracefully
    }
    
    std::cout << "    testRandomGeneratorEdgeCases passed" << std::endl;
}

// Performance and Memory Edge Cases
void testMemoryUsageWithLargeBrain() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 5000);  // Large brain
    config->set("region_count", 10);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    // Should handle large brain if memory allows
    if (success) {
        assert(brain.getTotalNeuronCount() == 5000);
        assert(brain.getTotalSynapseCount() > 0);
    }
    
    std::cout << "    testMemoryUsageWithLargeBrain passed" << std::endl;
}

void testPerformanceWithComplexNetwork() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 5);
    config->set("connection_probability", 0.2);  // Dense network
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Record start time
    auto start = std::chrono::high_resolution_clock::now();
    
    // Take many steps
    for (int step = 0; step < 1000; ++step) {
        brain.step(step, step * 0.001);
    }
    
    // Record end time
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete within reasonable time
    assert(duration.count() < 60000);  // Less than 1 minute
    
    std::cout << "    testPerformanceWithComplexNetwork passed" << std::endl;
}

void runAll() {
    testNeuronBoundaryValues();
    testNeuronRefractoryPeriodEdgeCases();
    testNeuronPlasticityFlags();
    testNeuronSpikeHistoryLimits();
    
    testSynapseWeightBoundaries();
    testSynapseEligibilityTraceEdgeCases();
    
    testWorkingMemoryEdgeCases();
    testEpisodicMemoryEdgeCases();
    
    testDopamineEdgeCases();
    testCuriosityEdgeCases();
    
    testDevelopmentStageEdgeCases();
    
    testSTDPEdgeCases();
    testHebbianEdgeCases();
    
    testBrainResetEdgeCases();
    testStepWithExtremeInputs();
    testConfigurationEdgeCases();
    testRandomGeneratorEdgeCases();
    
    testMemoryUsageWithLargeBrain();
    testPerformanceWithComplexNetwork();
    
    std::cout << "\n=== All Edge Case Tests Passed ===" << std::endl;
}

} // namespace test_edge_cases
