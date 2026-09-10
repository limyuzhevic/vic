#include "BrainTest.hpp"

namespace nlm {

void BrainTest::runBasicConnectivityTest() {
    std::cout << "\n=== Test 1: Basic Neural Connectivity ===";
    
    auto* region = brain_->getRegion(RegionId(1));
    if (!region) {
        std::cout << "  ERROR: No region found!";
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        std::cout << "  No neurons found!";
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain_->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    std::cout << "  Injecting current into 10 neurons...";
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(Constants::DEFAULT_INJECTION_CURRENT);
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < Constants::CONNECTIVITY_TEST_STEPS; ++step) {
        brain_->step(step, step * Constants::DEFAULT_TIME_STEP);
    }
    
    size_t spikes = brain_->getTotalSpikeCount() - initialSpikes;
    std::cout << "  Spikes generated: " + std::to_string(spikes);
    
    if (spikes > Constants::CONNECTIVITY_THRESHOLD) {
        std::cout << "  ✓ Spikes propagate through network";
        results_.connectivityTestPassed = true;
        results_.totalSpikes += spikes;
    } else {
        std::cout << "  ! No significant spikes - checking neuron parameters...";
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            std::cout << "    Neuron " + std::to_string(i) + 
                         " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                         " thresh=" + std::to_string(neurons[i]->getThreshold());
        }
    }
}

void BrainTest::runPlasticityExperiment() {
    std::cout << "\n=== Test 2: Plasticity Learning Experiment ===";
    
    LearningExperiment experiment(brain_, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain_->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    std::cout << "\nApplying repeated input patterns (" + std::to_string(Constants::PLASTICITY_EXPERIMENT_STEPS) + " steps)...";
    
    for (SimulationStep step = 0; step < Constants::PLASTICITY_EXPERIMENT_STEPS; ++step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain_->getTotalNeuronCount() / 4; ++i) {
            brain_->injectCurrentToNeurons(NeuronType::Sensory, Constants::PLASTICITY_INPUT_CURRENT);
        }
        
        brain_->step(step, step * Constants::DEFAULT_TIME_STEP);
        
        // Log progress every 200 steps
        if (step % 200 == 0) {
            std::cout << "  Step " + std::to_string(step) + 
                         " | Spikes: " + std::to_string(brain_->getTotalSpikeCount()) +
                         " | Firing: " + std::to_string(brain_->getFiringNeuronCount());
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics
    experiment.computeStatistics();
    
    // Check if learning occurred
    if (std::abs(experiment.finalMean - experiment.initialMean) > Constants::LEARNING_WEIGHT_CHANGE_THRESHOLD ||
        experiment.strengthened > 0 || experiment.weakened > 0) {
        std::cout << "  ✓ LEARNING DETECTED: Synaptic weights changed through experience";
        results_.plasticityTestPassed = true;
        results_.weightChange += std::abs(experiment.finalMean - experiment.initialMean);
    } else {
        std::cout << "  ✗ NO LEARNING: Weights did not change significantly";
    }
}

void BrainTest::runStdpVerification() {
    std::cout << "\n=== Test 3: STDP Verification ===";
    
    auto* region = brain_->getRegion(RegionId(1));
    if (!region) {
        std::cout << "  ERROR: No region found!";
        return;
    }
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        std::cout << "  Not enough synapses for STDP test";
        return;
    }
    
    std::cout << "  Testing STDP on 5 synapses:";
    
    // Record initial weights
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5 && i < synapses.size(); ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        std::cout << "    Synapse " + std::to_string(i) + 
                     " before: " + std::to_string(beforeWeights[i]);
    }
    
    std::cout << "\n  Creating correlated pre->post activity (potentiation)...";
    
    for (int trial = 0; trial < Constants::STDP_VERIFICATION_STEPS; ++trial) {
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
            preNeuron->injectCurrent(Constants::STDP_INPUT_CURRENT);
            brain_->step(trial * 2, trial * 2 * Constants::DEFAULT_TIME_STEP);
            
            // Then post fires
            postNeuron->injectCurrent(Constants::STDP_INPUT_CURRENT);
            brain_->step(trial * 2 + 1, (trial * 2 + 1) * Constants::DEFAULT_TIME_STEP);
        }
    }
    
    // Record after weights
    std::cout << "  After correlated activity:";
    float totalDelta = 0.0f;
    for (size_t i = 0; i < beforeWeights.size(); ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
        std::cout << "    Synapse " + std::to_string(i) + 
                     " after: " + std::to_string(synapses[i]->getWeight()) +
                     " (Δ=" + std::to_string(delta) + ")";
    }
    
    std::cout << "\n";
    if (totalDelta > Constants::STDP_POTENTIATION_THRESHOLD) {
        std::cout << "  ✓ STDP WORKING: Pre-before-post produced potentiation";
        results_.stdpTestPassed = true;
    } else if (totalDelta < -Constants::STDP_POTENTIATION_THRESHOLD) {
        std::cout << "  ! STDP reversed: Check parameters";
    } else {
        std::cout << "  ! No change: STDP may not be triggering";
    }
}

void BrainTest::runAllTests() {
    // Reset brain before each test sequence
    brain_->reset();
    brain_->initialize();
    
    std::cout << "\n";
    std::cout << "==================================================";
    std::cout << "NLM Brain Test Suite (Phase 2 - Real Neural Computation)";
    std::cout << "==================================================";
    std::cout << "\n";
    
    // Run Test 1: Basic connectivity
    runBasicConnectivityTest();
    
    // Reset and run Test 2: Plasticity
    brain_->reset();
    brain_->initialize();
    runPlasticityExperiment();
    
    // Reset and run Test 3: STDP
    brain_->reset();
    brain_->initialize();
    runStdpVerification();
    
    std::cout << "\n";
    std::cout << "==================================================";
    std::cout << "Test Summary:";
    std::cout << "==================================================";
    std::cout << "  Basic Connectivity: " << (results_.connectivityTestPassed ? "PASSED" : "FAILED") << "\n";
    std::cout << "  Plasticity Learning: " << (results_.plasticityTestPassed ? "PASSED" : "FAILED") << "\n";
    std::cout << "  STDP Verification: " << (results_.stdpTestPassed ? "PASSED" : "FAILED") << "\n";
    std::cout << "  Total Spikes Generated: " + std::to_string(results_.totalSpikes) << "\n";
    std::cout << "  Total Weight Change: " + std::to_string(results_.weightChange) << "\n";
    std::cout << "==================================================\n";
}

void LearningExperiment::recordInitialState() {
    initialSynapseCount = brain->getTotalSynapseCount();
    initialWeights.clear();
    
    // Record initial weights from first region
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    std::cout << "Initial state recorded:";
    std::cout << "  Synapses: " + std::to_string(initialSynapseCount);
    if (!initialWeights.empty()) {
        float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float mean = sum / initialWeights.size();
        initialMean = mean;
        std::cout << "  Mean weight: " + std::to_string(mean);
    }
}

void LearningExperiment::recordFinalState() {
    finalWeights.clear();
    
    // Record final weights from first region
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights.push_back(syn->getWeight());
        }
    }
    
    mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
    
    std::cout << "Final state recorded:";
    std::cout << "  Total spikes: " + std::to_string(brain->getTotalSpikeCount());
    if (!finalWeights.empty()) {
        float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float mean = sum / finalWeights.size();
        finalMean = mean;
        std::cout << "  Mean weight: " + std::to_string(mean);
    }
}

void LearningExperiment::computeStatistics() {
    std::cout << "\n=== Learning Experiment Results ===\n";
    
    if (initialWeights.empty() || finalWeights.empty()) {
        std::cout << "ERROR: No weights recorded";
        return;
    }
    
    // Compute weight changes
    float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
    float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
    initialMean = initialSum / initialWeights.size();
    finalMean = finalSum / finalWeights.size();
    
    std::cout << "Weight Statistics:";
    std::cout << "  Initial mean weight: " + std::to_string(initialMean);
    std::cout << "  Final mean weight: " + std::to_string(finalMean);
    std::cout << "  Change: " + std::to_string(finalMean - initialMean);
    
    // Count synapses that changed significantly
    size_t strengthened = 0;
    size_t weakened = 0;
    size_t unchanged = 0;
    
    size_t minSize = std::min(initialWeights.size(), finalWeights.size());
    for (size_t i = 0; i < minSize; ++i) {
        float delta = finalWeights[i] - initialWeights[i];
        if (delta > Constants::LEARNING_WEIGHT_CHANGE_THRESHOLD) ++strengthened;
        else if (delta < -Constants::LEARNING_WEIGHT_CHANGE_THRESHOLD) ++weakened;
        else ++unchanged;
    }
    
    std::cout << "\nSynaptic Changes:";
    std::cout << "  Strengthened: " + std::to_string(strengthened);
    std::cout << "  Weakened: " + std::to_string(weakened);
    std::cout << "  Unchanged: " + std::to_string(unchanged);
    
    std::cout << "\nSpike Activity:";
    std::cout << "  Total spikes: " + std::to_string(brain->getTotalSpikeCount());
    std::cout << "  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size());
    
    // Determine if learning occurred
    bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                            (strengthened > 0 || weakened > 0);
    
    std::cout << "\n";
    if (learningOccurred) {
        std::cout << "✓ LEARNING DETECTED: Synaptic weights changed through experience";
    } else {
        std::cout << "✗ NO LEARNING: Weights did not change significantly";
    }
}

} // namespace nlm