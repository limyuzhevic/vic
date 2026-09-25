// Test class implementations for NLM Phase 2
// Extracted from main.cpp for better organization

#include "test/TestClasses.hpp"
#include "core/Logger/Logger.hpp"
#include <numeric>
#include <cmath>
#include <algorithm>

namespace nlm {

ConnectivityTester::ConnectivityTester(std::shared_ptr<Brain> brain) : brain_(brain) {}

void ConnectivityTester::runBasicConnectivityTest() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain_->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain_->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain_->step(step, step * 0.001);
    }
    
    size_t spikes = brain_->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

PlasticityTester::PlasticityTester(std::shared_ptr<Brain> brain) : brain_(brain) {}

void PlasticityTester::runPlasticityExperiment() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain_);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain_->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain_->getTotalNeuronCount() / 4; ++i) {
            brain_->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain_->step(step, step * 0.001);
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain_->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain_->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
}

STDPTester::STDPTester(std::shared_ptr<Brain> brain) : brain_(brain) {}

void STDPTester::runStdpVerification() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain_->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
    }
    
    // Create correlated activity: fire pre then post to trigger LTP
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        // Fire pre-synaptic neuron
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain_->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain_->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
    }
    
    // Check if weights increased (LTP)
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

LearningExperiment::LearningExperiment(std::shared_ptr<Brain> brain) : brain_(brain) {}

void LearningExperiment::recordInitialState() {
    results_.initialSynapseCount = brain_->getTotalSynapseCount();
    initialWeights_.clear();
    
    // Record initial weights from first region
    if (auto* region = brain_->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights_.push_back(syn->getWeight());
        }
    }
    
    NLM_LOG_INFO("Initial state recorded:");
    NLM_LOG_INFO("  Synapses: " + std::to_string(results_.initialSynapseCount));
    if (!initialWeights_.empty()) {
        float sum = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f);
        float mean = sum / initialWeights_.size();
        results_.initialMeanWeight = mean;
        NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
    }
}

void LearningExperiment::recordFinalState() {
    finalWeights_.clear();
    
    // Record final weights from first region
    if (auto* region = brain_->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights_.push_back(syn->getWeight());
        }
    }
    
    results_.mostActiveNeurons = brain_->getSpikeSystem()->getMostActiveNeurons(10);
    
    NLM_LOG_INFO("Final state recorded:");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain_->getTotalSpikeCount()));
    if (!finalWeights_.empty()) {
        float sum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
        float mean = sum / finalWeights_.size();
        results_.finalMeanWeight = mean;
        NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
    }
}

void LearningExperiment::computeStatistics() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Learning Experiment Results ===");
    NLM_LOG_INFO("");
    
    if (initialWeights_.empty() || finalWeights_.empty()) {
        NLM_LOG_INFO("ERROR: No weights recorded");
        return;
    }
    
    // Compute weight changes
    float initialSum = std::accumulate(initialWeights_.begin(), initialWeights_.end(), 0.0f);
    float finalSum = std::accumulate(finalWeights_.begin(), finalWeights_.end(), 0.0f);
    results_.initialMeanWeight = initialSum / initialWeights_.size();
    results_.finalMeanWeight = finalSum / finalWeights_.size();
    
    NLM_LOG_INFO("Weight Statistics:");
    NLM_LOG_INFO("  Initial mean weight: " + std::to_string(results_.initialMeanWeight));
    NLM_LOG_INFO("  Final mean weight: " + std::to_string(results_.finalMeanWeight));
    NLM_LOG_INFO("  Change: " + std::to_string(results_.finalMeanWeight - results_.initialMeanWeight));
    
    // Count synapses that changed significantly
    size_t strengthened = 0;
    size_t weakened = 0;
    size_t unchanged = 0;
    
    size_t minSize = std::min(initialWeights_.size(), finalWeights_.size());
    for (size_t i = 0; i < minSize; ++i) {
        float delta = finalWeights_[i] - initialWeights_[i];
        if (delta > 0.01f) ++strengthened;
        else if (delta < -0.01f) ++weakened;
        else ++unchanged;
    }
    
    results_.strengthened = strengthened;
    results_.weakened = weakened;
    results_.unchanged = unchanged;
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Synaptic Changes:");
    NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
    NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
    NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Spike Activity:");
    results_.totalSpikes = brain_->getTotalSpikeCount();
    NLM_LOG_INFO("  Total spikes: " + std::to_string(results_.totalSpikes));
    NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(results_.mostActiveNeurons.size()));
    
    // Determine if learning occurred
    results_.learningOccurred = (std::abs(results_.finalMeanWeight - results_.initialMeanWeight) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
    
    NLM_LOG_INFO("");
    if (results_.learningOccurred) {
        NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
    } else {
        NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
    }
}

} // namespace nlm