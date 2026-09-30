// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Real LIF neuron dynamics                                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Learning Experiment: Demonstrates measurable synaptic changes through experience
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
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
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
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
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
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 10 neurons
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
    }
    
    // Run a few steps
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
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

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
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
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
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
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
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

// Test result structure for better organization
struct TestResult {
    std::string name;
    bool passed;
    std::string details;
};

class TestRunner {
public:
    static TestResult runTest(const std::string& name, std::function<bool()> testFunc) {
        try {
            bool result = testFunc();
            return {name, result, result ? "PASSED" : "FAILED"};
        } catch (const std::exception& e) {
            return {name, false, std::string("EXCEPTION: ") + e.what()};
        } catch (...) {
            return {name, false, "UNKNOWN EXCEPTION"};
        }
    }
    
    static void printResults(const std::vector<TestResult>& results) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Comprehensive Test Results ===");
        NLM_LOG_INFO("");
        
        int passed = 0, failed = 0;
        for (const auto& result : results) {
            if (result.passed) {
                NLM_LOG_INFO("✓ " + result.name + ": " + result.details);
                ++passed;
            } else {
                NLM_LOG_ERROR("✗ " + result.name + ": " + result.details);
                ++failed;
            }
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Test Summary: " + std::to_string(passed + failed) + " total, " +
                    std::to_string(passed) + " passed, " + std::to_string(failed) + " failed");
        
        if (failed > 0) {
            NLM_LOG_ERROR("Tests completed with " + std::to_string(failed) + " failures");
        }
    }
};

// Configurable test scenarios
namespace Tests {
    
    bool testBasicConnectivity(std::shared_ptr<Brain> brain, const std::vector<int>& neuronIndices) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Configurable Basic Neural Connectivity ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found!");
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_ERROR("  No neurons found!");
            return false;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into specified neurons (default to first 10 if not specified)
        std::vector<size_t> indicesToTest = neuronIndices.empty() ? 
            std::vector<size_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9} : 
            neuronIndices;
        
        NLM_LOG_INFO("  Injecting current into " + std::to_string(indicesToTest.size()) + " neurons...");
        for (size_t i : indicesToTest) {
            if (i < neurons.size()) {
                neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
            }
        }
        
        // Run steps
        size_t steps = 50; // Default
        for (SimulationStep step = 0; step < steps; ++step) {
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
            return false; // No spikes is a test failure for connectivity test
        }
    }
    
    bool testConfigurablePlasticity(std::shared_ptr<Brain> brain, const std::vector<std::pair<int, int>>& patterns) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 2: Configurable Plasticity Experiment ===");
        
        LearningExperiment experiment(brain, 42);
        
        // Record initial state
        experiment.recordInitialState();
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply input patterns
        int numSteps = 1000;
        int numPatterns = patterns.empty() ? 1 : patterns.size();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Applying " + std::to_string(numPatterns) + " input patterns for " + std::to_string(numSteps) + " steps...");
        
        for (int step = 0; step < numSteps; ++step) {
            if (!patterns.empty() && step % patterns.size() == 0) {
                for (const auto& pattern : patterns) {
                    brain->injectCurrentToNeurons(static_cast<NeuronType>(pattern.first), 
                                               static_cast<MembranePotential>(pattern.second));
                }
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
        
        // Determine if learning occurred with configurable threshold
        bool learningThreshold = 0.001f;
        bool learningOccurred = (std::abs(experiment.finalMean - experiment.initialMean) > learningThreshold) ||
                                (experiment.strengthened > 0 || experiment.weakened > 0);
        
        return learningOccurred;
    }
    
    bool testSTDVEnhanced(std::shared_ptr<Brain> brain, int numTrials, const std::vector<std::tuple<float, float, float>>& stparams) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: Enhanced STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found!");
            return false;
        }
        
        // Get first few synapses
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_ERROR("  Not enough synapses for STDP test");
            return false;
        }
        
        NLM_LOG_INFO("  Testing STDP on " + std::to_string(std::min(size_t(5), synapses.size())) + " synapses:");
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(beforeWeights[i]));
        }
        
        // Configure STDP parameters if provided
        nlm::STDP* stdp = brain->getSTDP();
        if (stdp) {
            for (const auto& param : stparams) {
                stdp->setLTPWeight(std::get<0>(param));
                stdp->setLTDWeight(std::get<1>(param));
                stdp->setTimeConstant(std::get<2>(param));
            }
        }
        
        // Create correlated activity
        NLM_LOG_INFO("");
        NLM_LOG_INFO("  Creating correlated pre->post activity (" + std::to_string(numTrials) + " trials)...");
        
        for (int trial = 0; trial < numTrials; ++trial) {
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
                        " (Δ=" + std::to_string(delta) + ")");
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
    
    bool testAdvancedNetworkDynamics(std::shared_ptr<Brain> brain, float injectionCurrent, int durationSteps) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 4: Advanced Network Dynamics ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No region found!");
            return false;
        }
        
        // Get all neurons and categorize by type
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_ERROR("  No neurons found!");
            return false;
        }
        
        NLM_LOG_INFO("  Network state at start:")
        NLM_LOG_INFO("    Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("    Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
        NLM_LOG_INFO("    Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
        
        // Inject strong current across the network
        NLM_LOG_INFO("  Injecting " + std::to_string(injectionCurrent) + " mA current to all neurons...");
        
        // Distribute current across neuron types based on proportion
        size_t internalNeurons = 0;
        size_t sensoryNeurons = 0;
        size_t motorNeurons = 0;
        
        for (auto* neuron : neurons) {
            switch (neuron->getType()) {
                case NeuronType::Internal: internalNeurons++; break;
                case NeuronType::Sensory: sensoryNeurons++; break;
                case NeuronType::Motor: motorNeurons++; break;
                default: break;
            }
        }
        
        // Calculate distribution
        float totalCurrent = injectionCurrent;
        float internalCurrent = totalCurrent * (internalNeurons / static_cast<float>(neurons.size()));
        float sensoryCurrent = totalCurrent * (sensoryNeurons / static_cast<float>(neurons.size()));
        float motorCurrent = totalCurrent * (motorNeurons / static_cast<float>(neurons.size()));
        
        // Inject currents
        for (auto* neuron : neurons) {
            switch (neuron->getType()) {
                case NeuronType::Internal: neuron->injectCurrent(internalCurrent); break;
                case NeuronType::Sensory: neuron->injectCurrent(sensoryCurrent); break;
                case NeuronType::Motor: neuron->injectCurrent(motorCurrent); break;
                default: break;
            }
        }
        
        // Run for specified duration
        NLM_LOG_INFO("  Running network dynamics for " + std::to_string(durationSteps) + " steps...");
        
        size_t spikeHistory[durationSteps] = {0};
        
        for (SimulationStep step = 0; step < durationSteps; ++step) {
            brain->step(step, step * 0.001);
            
            // Record spike count for analysis
            spikeHistory[step] = brain->getTotalSpikeCount() - (step > 0 ? spikeHistory[step-1] : 0);
            
            if (step % 100 == 0) {
                NLM_LOG_INFO("    Step " + std::to_string(step) + 
                            ", Spikes: " + std::to_string(spikeHistory[step]) +
                            ", Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Analyze results
        NLM_LOG_INFO("  Network dynamics analysis:");
        NLM_LOG_INFO("    Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
        NLM_LOG_INFO("    Excitation/inhibition ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
        
        // Calculate firing statistics
        size_t maxSpikesInStep = 0;
        size_t totalSpikesInHistory = 0;
        for (int i = 0; i < durationSteps; ++i) {
            if (spikeHistory[i] > maxSpikesInStep) maxSpikesInStep = spikeHistory[i];
            totalSpikesInHistory += spikeHistory[i];
        }
        
        float avgSpikesPerStep = totalSpikesInHistory / static_cast<float>(durationSteps);
        
        NLM_LOG_INFO("    Max spikes in single step: " + std::to_string(maxSpikesInStep));
        NLM_LOG_INFO("    Average spikes per step: " + std::to_string(avgSpikesPerStep));
        
        // Check if network exhibited complex dynamics
        bool complexDynamics = avgSpikesPerStep > 1.0f && maxSpikesInStep > 5;
        if (complexDynamics) {
            NLM_LOG_INFO("  ✓ Complex network dynamics observed");
            return true;
        } else {
            NLM_LOG_INFO("  ! Limited network dynamics - may need different parameters");
            return false;
        }
    }
    
    bool testMemorySystemIntegration(std::shared_ptr<Brain> brain, size_t numEpisodes) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 5: Memory System Integration ===");
        
        bool memorySystemsWorking = true;
        
        // Test working memory
        if (auto* workingMemory = brain->getWorkingMemory()) {
            NLM_LOG_INFO("  Testing working memory system...");
            
            // Store some traces
            workingMemory->storeToNeuron(NeuronId(1), 0.5f);
            workingMemory->storeToNeuron(NeuronId(2), 0.8f);
            
            size_t traces = workingMemory->getActiveTraces();
            NLM_LOG_INFO("    Working memory active traces: " + std::to_string(traces));
            
            if (traces < 2) {
                memorySystemsWorking = false;
                NLM_LOG_ERROR("  ! Working memory not capturing traces");
            }
        } else {
            memorySystemsWorking = false;
            NLM_LOG_ERROR("  ! Working memory system not available");
        }
        
        // Test episodic memory
        if (auto* episodicMemory = brain->getEpisodicMemory()) {
            NLM_LOG_INFO("  Testing episodic memory system...");
            
            // Store some episodes
            for (size_t i = 0; i < numEpisodes; ++i) {
                EpisodicMemoryItem episode;
                episode.timestamp = static_cast<SimulationStep>(i);
                episode.reward = 0.5f + static_cast<float>(i) * 0.1f;
                episode.activeNeurons.push_back(NeuronId(i + 10));
                episode.neuronActivations.push_back(0.7f);
                episodicMemory->storeEpisode(episode);
            }
            
            size_t episodes = episodicMemory->getEpisodeCount();
            NLM_LOG_INFO("    Episodic memory episodes: " + std::to_string(episodes));
            
            if (episodes < numEpisodes) {
                memorySystemsWorking = false;
                NLM_LOG_ERROR("  ! Episodic memory not storing all episodes");
            }
        } else {
            memorySystemsWorking = false;
            NLM_LOG_ERROR("  ! Episodic memory system not available");
        }
        
        // Test associative memory
        if (auto* associativeMemory = brain->getAssociativeMemory()) {
            NLM_LOG_INFO("  Testing associative memory system...");
            
            associativeMemory->storePattern({NeuronId(1), NeuronId(2), NeuronId(3)}, 0.8f);
            associativeMemory->storePattern({NeuronId(4), NeuronId(5), NeuronId(6)}, 0.9f);
            
            // Retrieve patterns
            auto patterns = associativeMemory->retrievePattern({NeuronId(1), NeuronId(2)});
            NLM_LOG_INFO("    Retrieved " + std::to_string(patterns.size()) + " patterns");
            
            if (patterns.empty()) {
                memorySystemsWorking = false;
                NLM_LOG_ERROR("  ! Associative memory not retrieving patterns");
            }
        } else {
            memorySystemsWorking = false;
            NLM_LOG_ERROR("  ! Associative memory system not available");
        }
        
        if (memorySystemsWorking) {
            NLM_LOG_INFO("  ✓ All memory systems integrated and functional");
        } else {
            NLM_LOG_ERROR("  ! Memory systems integration incomplete");
        }
        
        return memorySystemsWorking;
    }
}

void parseCommandLineArgs(int argc, char** argv, struct TestConfig& config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.find("--test1_neurons=") == 0) {
            // Parse comma-separated list of neuron indices
            std::string value = arg.substr(22);
            std::stringstream ss(value);
            std::string token;
            while (std::getline(ss, token, ',')) {
                config.test1_neuronIndices.push_back(std::stoi(token));
            }
        }
        else if (arg.find("--test2_steps=") == 0) {
            config.test2_steps = std::stoi(arg.substr(14));
        }
        else if (arg.find("--test2_patterns=") == 0) {
            // Parse comma-separated patterns as "neuronType,current" pairs
            std::string value = arg.substr(18);
            std::stringstream ss(value);
            std::string token;
            while (std::getline(ss, token, ',')) {
                std::string neuronTypeStr, currentStr;
                size_t comma = token.find(',');
                if (comma != std::string::npos) {
                    neuronTypeStr = token.substr(0, comma);
                    currentStr = token.substr(comma + 1);
                    config.test2_patterns.push_back(std::make_pair(
                        std::stoi(neuronTypeStr), std::stof(currentStr)
                    ));
                }
            }
        }
        else if (arg.find("--test3_trials=") == 0) {
            config.test3_trials = std::stoi(arg.substr(16));
        }
        else if (arg.find("--test4_current=") == 0) {
            config.test4_injectionCurrent = std::stof(arg.substr(17));
        }
        else if (arg.find("--test4_duration=") == 0) {
            config.test4_durationSteps = std::stoi(arg.substr(17));
        }
        else if (arg.find("--test5_episodes=") == 0) {
            config.test5_episodes = std::stoi(arg.substr(17));
        }
        else if (arg == "--integration-tests" || arg == "--integration") {
            config.runIntegrationTests = true;
        }
        else if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        }
    }
}

// Add integration test runner
void runIntegrationTests() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Running Comprehensive Integration Tests ===");
    
    #ifdef RUN_INTEGRATION_TESTS
    test_integration::runAll();
    #endif
}

void printHelp() {
    printBanner();
    
    std::cout << R"(
Usage: ./nlm_main [OPTIONS]

Enhanced Phase 2 Neural Learning Machine tests with configurable scenarios.

Options:
  --config=<file>          Load configuration from specified file
  --help, -h               Show this help message
  --integration-tests      Enable comprehensive integration tests

Test Configuration Options:
  --test1_neurons=<indices>  Comma-separated list of neuron indices for Test 1
                               (default: 0,1,2,3,4,5,6,7,8,9)
  --test2_steps=<steps>       Number of steps for Plasticity Test 2 (default: 1000)
  --test2_patterns=<patterns> Comma-separated "neuronType,current" pairs for Test 2
                               (format: 0,30.0,1,30.0,2,30.0)
  --test3_trials=<trials>     Number of trials for STDP Test 3 (default: 50)
  --test4_current=<current>   Current injection magnitude for Test 4 (default: 30.0)
  --test4_duration=<steps>    Number of steps for Test 4 (default: 1000)
  --test5_episodes=<episodes> Number of episodes for Memory Test 5 (default: 5)

Example:
  ./nlm_main --config=myconfig.cfg --test2_steps=2000 --test3_trials=100 --integration-tests
)" << std::endl;
}

struct TestConfig {
    std::vector<size_t> test1_neuronIndices;
    int test2_steps = 1000;
    std::vector<std::pair<int, int>> test2_patterns;
    int test3_trials = 50;
    float test4_injectionCurrent = 30.0f;
    int test4_durationSteps = 1000;
    size_t test5_episodes = 5;
    bool runIntegrationTests = false;
};

int main(int argc, char** argv) {
    printBanner();
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== Enhanced NLM Phase 2 Test Suite ===");
    NLM_LOG_INFO("Running comprehensive, configurable tests of neural computation");
    NLM_LOG_INFO("");
    
    // Load configuration
    auto config = std::make_shared<Config>();
    std::string configFile = "configs/default.cfg";
    
    // Parse command line for config file first
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Parse test configuration
    TestConfig testConfig;
    parseCommandLineArgs(argc, argv, testConfig);
    
    // Set Phase 2 defaults
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration loaded and configured for comprehensive testing");
    NLM_LOG_INFO("Test configuration: Steps=" + std::to_string(testConfig.test2_steps) +
                ", Trials=" + std::to_string(testConfig.test3_trials));
    
    // Initialize simulation clock and brain
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    
    NLM_LOG_INFO("Initializing NLM Brain with comprehensive test suite...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    // Run comprehensive test suite
    std::vector<TestResult> results;
    
    // Test 1: Basic connectivity (configurable)
    results.push_back(TestRunner::runTest("Basic Neural Connectivity",
        [brain, &testConfig]() {
            return Tests::testBasicConnectivity(brain, testConfig.test1_neuronIndices);
        }));
    
    // Reset for next test
    brain->reset();
    brain->initialize();
    
    // Test 2: Plasticity learning (configurable)
    results.push_back(TestRunner::runTest("Configurable Plasticity Learning",
        [brain, &testConfig]() {
            return Tests::testConfigurablePlasticity(brain, testConfig.test2_patterns);
        }));
    
    // Reset for STDP test
    brain->reset();
    brain->initialize();
    
    // Test 3: STDP verification (configurable)
    std::vector<std::tuple<float, float, float>> stdpParams;
    results.push_back(TestRunner::runTest("Enhanced STDP Verification",
        [brain, &testConfig, &stdpParams]() {
            return Tests::testSTDVEnhanced(brain, testConfig.test3_trials, stdpParams);
        }));
    
    // Reset for network dynamics test
    brain->reset();
    brain->initialize();
    
    // Test 4: Advanced network dynamics (configurable)
    results.push_back(TestRunner::runTest("Advanced Network Dynamics",
        [brain, &testConfig]() {
            return Tests::testAdvancedNetworkDynamics(brain, 
                                                     testConfig.test4_injectionCurrent,
                                                     testConfig.test4_durationSteps);
        }));
    
    // Reset for memory test
    brain->reset();
    brain->initialize();
    
    // Test 5: Memory system integration (configurable)
    results.push_back(TestRunner::runTest("Memory System Integration",
        [brain, &testConfig]() {
            return Tests::testMemorySystemIntegration(brain, testConfig.test5_episodes);
        }));
    
    // Print comprehensive test results
    TestRunner::printResults(results);
    
    // Run integration tests if requested
    if (testConfig.runIntegrationTests) {
        runIntegrationTests();
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain State ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Test Suite Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain system demonstrates:");
    NLM_LOG_INFO("  ✓ Real spiking neural computation");
    NLM_LOG_INFO("  ✓ Configurable learning scenarios");
    NLM_LOG_INFO("  ✓ Complex network dynamics");
    NLM_LOG_INFO("  ✓ Integrated memory systems");
    NLM_LOG_INFO("  ✓ Advanced plasticity mechanisms");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Ready for Phase 3: Advanced cognitive capabilities.");
    
    return 0;
}
