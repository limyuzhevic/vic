#include "TestRunner.hpp"
#include "core/Logger/Logger.hpp"
#include <thread>
#include <chrono>

using namespace nlm;

TestRunner::TestRunner(std::shared_ptr<ConfigurationManager> config) 
    : config_(config) {
    testEnabled_["basic_connectivity"] = true;
    testEnabled_["plasticity_experiment"] = true;
    testEnabled_["stdp_verification"] = true;
}

int TestRunner::runAllTests(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot run tests: Brain is null");
        return -1;
    }
    
    NLM_LOG_INFO("Starting test suite with brain of " + std::to_string(brain->getTotalNeuronCount()) + " neurons");
    
    testsRun_ = 0;
    testsPassed_ = 0;
    testsFailed_ = 0;
    
    // Run basic connectivity test
    if (testEnabled_["basic_connectivity"]) {
        testsRun_++;
        if (runBasicConnectivityTest(brain)) {
            testsPassed_++;
            NLM_LOG_INFO("✓ Basic connectivity test PASSED");
        } else {
            testsFailed_++;
            NLM_LOG_ERROR("✗ Basic connectivity test FAILED");
        }
    }
    
    // Reset brain for next test
    if (brain->reset() && brain->initialize()) {
        NLM_LOG_INFO("Brain reset and reinitialized for plasticity experiment");
    }
    
    // Run plasticity experiment
    if (testEnabled_["plasticity_experiment"]) {
        testsRun_++;
        if (runPlasticityExperiment(brain)) {
            testsPassed_++;
            NLM_LOG_INFO("✓ Plasticity experiment test PASSED");
        } else {
            testsFailed_++;
            NLM_LOG_ERROR("✗ Plasticity experiment test FAILED");
        }
    }
    
    // Reset brain for STDP test
    if (brain->reset() && brain->initialize()) {
        NLM_LOG_INFO("Brain reset and reinitialized for STDP verification");
    }
    
    // Run STDP verification
    if (testEnabled_["stdp_verification"]) {
        testsRun_++;
        if (runStdpVerification(brain)) {
            testsPassed_++;
            NLM_LOG_INFO("✓ STDP verification test PASSED");
        } else {
            testsFailed_++;
            NLM_LOG_ERROR("✗ STDP verification test FAILED");
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test Suite Summary ===");
    NLM_LOG_INFO("Tests run: " + std::to_string(testsRun_));
    NLM_LOG_INFO("Tests passed: " + std::to_string(testsPassed_));
    NLM_LOG_INFO("Tests failed: " + std::to_string(testsFailed_));
    
    return testsFailed_ == 0 ? 0 : 1;
}

bool TestRunner::runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("  No brain region found");
        return false;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_ERROR("  No neurons found");
        return false;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    NLM_LOG_INFO("  Running 50 simulation steps...");
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
        return true;
    } else {
        NLM_LOG_WARNING("  ! No spikes detected - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
        return false;  // No spikes means test failed
    }
}

bool TestRunner::runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    // Create learning experiment
    LearningExperiment experiment(brain, 42);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
        
        // Apply repeated input pattern to stimulate learning
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display statistics using LearningExperiment class
    experiment.computeStatistics();
    
    // Check if learning occurred using LearningExperiment method
    if (experiment.didLearningOccur()) {
        NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        return true;
    } else {
        NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        return false;
    }
}

bool TestRunner::runStdpVerification(std::shared_ptr<Brain> brain) {
    if (!brain) return false;
    
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("  No brain region found");
        return false;
    }
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_ERROR("  Not enough synapses for STDP test");
        return false;
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
    
    auto neurons = region->getAllNeurons();
    Neuron* preNeuron = nullptr;
    Neuron* postNeuron = nullptr;
    
    if (neurons.size() >= 2) {
        preNeuron = neurons[0];
        postNeuron = neurons[1];
    }
    
    for (int trial = 0; trial < 50; ++trial) {
        if (preNeuron && postNeuron) {
            // Pre fires first
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        totalDelta += delta;
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
        return true;
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
        return false;
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
        return false;
    }
}

std::string TestRunner::getTestStatistics() const {
    std::ostringstream oss;
    oss << "Test Statistics: " << testsRun_ << " run, "
        << testsPassed_ << " passed, " << testsFailed_ << " failed";
    return oss.str();
}

void TestRunner::setTestEnabled(const std::string& testName, bool enabled) {
    testEnabled_[testName] = enabled;
}

std::vector<std::string> TestRunner::getEnabledTests() const {
    std::vector<std::string> enabled;
    for (const auto& pair : testEnabled_) {
        if (pair.second) {
            enabled.push_back(pair.first);
        }
    }
    return enabled;
}

bool TestRunner::waitForSpikes(std::shared_ptr<Brain> brain, size_t expectedSpikes, size_t timeoutSteps) {
    if (!brain) return false;
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    for (size_t step = 0; step < timeoutSteps; ++step) {
        brain->step(step, step * 0.001);
        if (brain->getTotalSpikeCount() - initialSpikes >= expectedSpikes) {
            return true;
        }
    }
    return false;
}

bool TestRunner::checkConvergence(std::vector<float>& weights, float tolerance) {
    if (weights.size() < 2) return true;
    
    float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
    float mean = sum / weights.size();
    
    float variance = 0.0f;
    for (float w : weights) {
        variance += (w - mean) * (w - mean);
    }
    variance /= weights.size();
    
    return std::sqrt(variance) < tolerance;
}