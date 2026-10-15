// Test runner implementation for NLM Phase 2 tests
#include "TestRunner.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <functional>

namespace nlm {

struct NLMTestRunner::Impl {
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::string results;
    bool testsPassed;
    std::unordered_map<std::string, std::function<bool(NLMTestRunner&)>> testFunctions;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg), testsPassed(true) {
        initializeTests();
    }
    
    void initializeTests() {
        testFunctions["connectivity"] = [this](NLMTestRunner& runner) {
            return runConnectivityTest();
        };
        
        testFunctions["plasticity"] = [this](NLMTestRunner& runner) {
            return runPlasticityTest();
        };
        
        testFunctions["stdp"] = [this](NLMTestRunner& runner) {
            return runStdpTest();
        };
        
        testFunctions["all"] = [this](NLMTestRunner& runner) {
            return runAllTests();
        };
    }
    
    bool runConnectivityTest() {
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        auto region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found!");
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_ERROR("No neurons found!");
            return false;
        }
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("  ✓ Spikes propagate through network");
            return true;
        } else {
            NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                            " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                            " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
            return false;
        }
    }
    
    bool runPlasticityTest() {
        NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
        
        // Reset brain for fresh test
        brain->reset();
        brain->initialize();
        
        // Create learning experiment
        LearningExperiment experiment(brain, 42);
        
        // Record initial state
        experiment.recordInitialState();
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern to stimulate learning
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
        
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
        
        // Record final state
        experiment.recordFinalState();
        
        // Compute and display statistics
        experiment.computeStatistics();
        
        return true;
    }
    
    bool runStdpTest() {
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found!");
            return false;
        }
        
        // Get first few synapses
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_ERROR("Not enough synapses for STDP test");
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
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (\\u0394=" + std::to_string(delta) + ")");
            totalDelta += delta;
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
    
    bool runAllTests() {
        bool allPassed = true;
        
        // Run connectivity test
        if (!runConnectivityTest()) {
            allPassed = false;
        }
        
        // Reset for plasticity test
        brain->reset();
        brain->initialize();
        
        // Run plasticity test
        if (!runPlasticityTest()) {
            allPassed = false;
        }
        
        // Reset for STDP test
        brain->reset();
        brain->initialize();
        
        // Run STDP test
        if (!runStdpTest()) {
            allPassed = false;
        }
        
        return allPassed;
    }
};

NLMTestRunner::NLMTestRunner(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>(config)) {
    // Initialize brain
    pImpl->brain = std::make_shared<Brain>(config);
    if (!pImpl->brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        pImpl->testsPassed = false;
    }
}

NLMTestRunner::~NLMTestRunner() = default;

bool NLMTestRunner::runTest(const std::string& testName) {
    if (pImpl->testFunctions.find(testName) == pImpl->testFunctions.end()) {
        NLM_LOG_ERROR("Unknown test: " + testName);
        return false;
    }
    
    bool result = pImpl->testFunctions[testName](*this);
    pImpl->testsPassed = pImpl->testsPassed && result;
    
    std::stringstream ss;
    ss << "Test \"" << testName << "\" " << (result ? "PASSED" : "FAILED");
    pImpl->results += ss.str() + "\n";
    
    return result;
}

bool NLMTestRunner::runAllTests() {
    bool result = pImpl->runAllTests();
    pImpl->testsPassed = pImpl->testsPassed && result;
    
    std::stringstream ss;
    ss << "All tests " << (result ? "PASSED" : "FAILED");
    pImpl->results += ss.str() + "\n";
    
    return result;
}

const std::string& NLMTestRunner::getResults() const {
    return pImpl->results;
}

bool NLMTestRunner::hasTestsPassed() const {
    return pImpl->testsPassed;
}

void NLMTestRunner::setupDefaultConfig() {
    // Set default values for Phase 2
    pImpl->config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    pImpl->config->set("simulation_timestep", 0.001, ConfigSource::Default);
    pImpl->config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    pImpl->config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    pImpl->config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    pImpl->config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    pImpl->config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    pImpl->config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    pImpl->config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    pImpl->config->set("pruning_rate", 0.00001f, ConfigSource::Default);
}

void NLMTestRunner::overrideWithArgs(int argc, char** argv) {
    pImpl->config->loadFromArgs(argc, argv);
}
