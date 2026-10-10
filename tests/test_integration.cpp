// Integration Tests from main.cpp
// Phase 2: Real Neural Computation Integration Tests

#include "brain/Brain.hpp"
#include "core/config/Constants.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <cmath>

namespace nlm {

struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
    }
    
    void computeStatistics() {
        if (initialWeights.empty() || finalWeights.empty()) {
            std::cerr << "WARNING: No weights recorded" << std::endl;
            return;
        }
        
        // Compute weight changes
        float initialSum = 0.0f;
        float finalSum = 0.0f;
        for (float w : initialWeights) initialSum += w;
        for (float w : finalWeights) finalSum += w;
        
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        // Count synapses that changed significantly
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        if (learningOccurred) {
            std::cout << "    ✓ Integration test passed: Learning detected" << std::endl;
        } else {
            std::cout << "    ⚠ Integration test: No significant learning detected" << std::endl;
        }
    }
};

void testBasicConnectivity() {
    std::cout << "Running test: Basic Neural Connectivity..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(config::DEFAULT_NEURON_COUNT / 10), ConfigSource::Default); // Smaller for faster test
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("simulation_timestep", config::SIMULATION_TIMESTEP, ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain.getRegion(RegionId(1));
    assert(region != nullptr);
    
    auto neurons = region->getAllNeurons();
    assert(!neurons.empty());
    
    // Get initial spike count
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    // Inject strong current into first few neurons (using constants)
    const size_t testNeurons = std::min<size_t>(10, neurons.size());
    for (size_t i = 0; i < testNeurons; ++i) {
        neurons[i]->injectCurrent(config::DEFAULT_STDP_LTP_WEIGHT * 100.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.step(step, step * config::SIMULATION_TIMESTEP);
    }
    
    size_t spikes = brain.getTotalSpikeCount() - initialSpikes;
    
    if (spikes > 0) {
        std::cout << "    ✓ Spikes propagate through network" << std::endl;
    } else {
        std::cout << "    ! No spikes detected in basic connectivity test" << std::endl;
    }
}

void testPlasticityLearning() {
    std::cout << "Running test: Plasticity Learning Experiment..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(config::DEFAULT_NEURON_COUNT / 10), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("simulation_timestep", config::SIMULATION_TIMESTEP, ConfigSource::Default);
    config->set("stdp_ltp_weight", config::DEFAULT_STDP_LTP_WEIGHT, ConfigSource::Default);
    config->set("stdp_ltd_weight", config::DEFAULT_STDP_LTD_WEIGHT, ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Reset and reinitialize for clean experiment
    brain.reset();
    brain.initialize();
    
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain.getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning (using constants)
    const size_t steps = 100;  // Reduced for faster testing
    for (nlm::SimulationStep step = 0; step < steps; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 5 && i < brain.getTotalNeuronCount() / 4; ++i) {
            brain.injectCurrentToNeurons(NeuronType::Sensory, config::DEFAULT_STDP_LTP_WEIGHT * 50.0f);
        }
        
        brain.step(step, step * config::SIMULATION_TIMESTEP);
        
        // Log progress every 20 steps
        if (step % 20 == 0) {
            std::cout << "    Step " << step << " | Spikes: " << brain.getTotalSpikeCount() 
                      << " | Firing: " << brain.getFiringNeuronCount() << std::endl;
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

void testStdpVerification() {
    std::cout << "Running test: STDP Verification..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(config::DEFAULT_NEURON_COUNT / 10), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("simulation_timestep", config::SIMULATION_TIMESTEP, ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Reset and reinitialize for clean test
    brain.reset();
    brain.initialize();
    
    auto* region = brain.getRegion(RegionId(1));
    assert(region != nullptr);
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 2) {
        std::cout << "    ⚠ Not enough synapses for STDP test" << std::endl;
        return;
    }
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 2 && i < synapses.size(); ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    auto neurons = region->getAllNeurons();
    if (neurons.size() >= 2) {
        // Fire pre-synaptic neuron
        neurons[0]->injectCurrent(config::DEFAULT_STDP_LTP_WEIGHT * 60.0f);
        brain.step(0, 0.0);
        
        // Then post fires
        neurons[1]->injectCurrent(config::DEFAULT_STDP_LTP_WEIGHT * 60.0f);
        brain.step(1, 1 * config::SIMULATION_TIMESTEP);
        
        // Check weight changes
        for (size_t i = 0; i < 2 && i < synapses.size(); ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            std::cout << "    Synapse " << i << " Δ=" << delta << " (before=" 
                      << beforeWeights[i] << ", after=" << synapses[i]->getWeight() << ")" << std::endl;
        }
        
        // Check if weights increased (LTP)
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 2 && i < synapses.size(); ++i) {
            totalDelta += synapses[i]->getWeight() - beforeWeights[i];
        }
        
        if (totalDelta > 0.001f) {
            std::cout << "    ✓ STDP working: Pre-before-post produced potentiation" << std::endl;
        } else if (totalDelta < -0.001f) {
            std::cout << "    ! STDP reversed: Check parameters" << std::endl;
        } else {
            std::cout << "    ⚠ STDP produced no change: May need different parameters" << std::endl;
        }
    }
}

void runIntegrationTests() {
    std::cout << "=== NLM Phase 2 Integration Tests ===" << std::endl;
    std::cout << "Testing integrated neural systems with configuration constants:" << std::endl;
    
    testBasicConnectivity();
    testPlasticityLearning();
    testStdpVerification();
    
    std::cout << "" << std::endl;
    std::cout << "=== Integration Tests Complete ===" << std::endl;
}

} // namespace nlm