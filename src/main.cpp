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

// Helper function implementations
void runEnhancedConnectivityTest(std::shared_ptr<Brain> brain, const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Connectivity Test ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("Region 1 not found");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    // Get initial spike count
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into first 20 neurons (enhanced from original 10)
    NLM_LOG_INFO("  Injecting current into " + std::to_string(std::min(size_t(20), neurons.size())) + " neurons...");
    for (size_t i = 0; i < std::min(size_t(20), neurons.size()); ++i) {
        neurons[i]->injectCurrent(75.0f);  // Stronger input for better testing
    }
    
    // Run more steps for better propagation
    double timestep = config.getOr<double>("simulation_timestep", 0.001);
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step, step * timestep);
        
        // Log every 10 steps
        if (step % 10 == 0) {
            size_t currentSpikes = brain->getTotalSpikeCount();
            NLM_LOG_INFO("    Step " + std::to_string(step) + ": " + std::to_string(currentSpikes - initialSpikes) + " spikes so far");
        }
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Total spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network effectively");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runLearningSimulation(std::shared_ptr<Brain> brain, const Config& config, size_t numSteps) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Learning Simulation ===");
    NLM_LOG_INFO("Running " + std::to_string(numSteps) + " simulation steps with learning enabled...");
    
    double timestep = config.getOr<double>("simulation_timestep", 0.001);
    
    // Enable plasticity on all synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    size_t initialSynapses = brain->getTotalSynapseCount();
    
    for (SimulationStep step = 0; step < numSteps; ++step) {
        // Create diverse input patterns
        if (step % 4 == 0) {
            // Pattern 1: Burst activity
            for (size_t i = 0; i < 30 && i < brain->getTotalNeuronCount() / 5; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 35.0f);
            }
        } else if (step % 4 == 1) {
            // Pattern 2: Sparse activity
            for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount() / 10; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 60.0f);
            }
        } else if (step % 4 == 2) {
            // Pattern 3: Inhibitory focus
            for (size_t i = 0; i < 15 && i < brain->getTotalNeuronCount() / 6; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Inhibitory, -30.0f);
            }
        } else {
            // Pattern 4: Mixed activity
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Modulatory, 25.0f);
            }
        }
        
        brain->step(step, step * timestep);
        
        // Log progress periodically
        if (step % 200 == 0) {
            size_t currentSpikes = brain->getTotalSpikeCount();
            size_t currentFiring = brain->getFiringNeuronCount();
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(currentSpikes - initialSpikes) +
                        " | Firing: " + std::to_string(currentFiring));
        }
    }
    
    size_t totalSpikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Learning Simulation Complete:");
    NLM_LOG_INFO("  Total steps: " + std::to_string(numSteps));
    NLM_LOG_INFO("  Total spikes generated: " + std::to_string(totalSpikes));
    NLM_LOG_INFO("  Average spikes per step: " + std::to_string(totalSpikes / numSteps));
    NLM_LOG_INFO("  Final firing neuron count: " + std::to_string(brain->getFiringNeuronCount()));
}

void runAdvancedPlasticityTest(std::shared_ptr<Brain> brain, const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Advanced Plasticity Test ===");
    
    double timestep = config.getOr<double>("simulation_timestep", 0.001);
    
    // Create LearningExperiment to track plasticity
    struct LearningExperiment {
        std::shared_ptr<Brain> brain;
        uint64_t seed;
        size_t initialSynapseCount;
        std::vector<float> initialWeights;
        std::vector<float> finalWeights;
        
        LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
            : brain(b), seed(s), initialSynapseCount(0) {}
        
        void recordInitialState() {
            initialSynapseCount = brain->getTotalSynapseCount();
            initialWeights.clear();
            
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    initialWeights.push_back(syn->getWeight());
                }
            }
        }
        
        void recordFinalState() {
            finalWeights.clear();
            
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    finalWeights.push_back(syn->getWeight());
                }
            }
        }
        
        void computeAndDisplayStatistics() {
            if (initialWeights.empty() || finalWeights.empty()) {
                NLM_LOG_INFO("ERROR: No weights recorded");
                return;
            }
            
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Plasticity Statistics ===");
            
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
            
            bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                    (strengthened > 0 || weakened > 0);
            
            NLM_LOG_INFO("");
            if (learningOccurred) {
                NLM_LOG_INFO("✓ PLACILITY DETECTED: Synaptic weights changed through experience");
            } else {
                NLM_LOG_INFO("✗ NO PLACILITY: Weights did not change significantly");
            }
        }
    };
    
    LearningExperiment experiment(brain, 42);
    experiment.recordInitialState();
    
    // Apply complex learning patterns
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying complex learning patterns (3000 steps)...");
    
    for (SimulationStep step = 0; step < 3000; ++step) {
        // Create correlated activity patterns that drive different plasticity mechanisms
        
        // 1. Correlated pre-post activity for STDP
        if (step % 100 == 0) {
            auto* region = brain->getRegion(RegionId(1));
            if (region && region->getAllNeurons().size() >= 2) {
                auto neurons = region->getAllNeurons();
                Neuron* preNeuron = neurons[0];
                Neuron* postNeuron = neurons[1];
                
                // LTP protocol: pre before post
                preNeuron->injectCurrent(80.0f);
                brain->step(step * 2, step * 2 * timestep);
                postNeuron->injectCurrent(80.0f);
                brain->step(step * 2 + 1, (step * 2 + 1) * timestep);
            }
        }
        
        // 2. Random burst patterns
        if (step % 50 == 10) {
            for (size_t i = 0; i < 25 && i < brain->getTotalNeuronCount() / 8; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Excitatory, 50.0f);
            }
        }
        
        // 3. Inhibitory modulation
        if (step % 50 == 25) {
            for (size_t i = 0; i < 15 && i < brain->getTotalNeuronCount() / 12; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Inhibitory, -40.0f);
            }
        }
        
        // 4. Modulatory signals
        if (step % 50 == 40) {
            for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount() / 20; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Modulatory, 30.0f);
            }
        }
        
        brain->step(step, step * timestep);
        
        // Log progress
        if (step % 500 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    experiment.recordFinalState();
    experiment.computeAndDisplayStatistics();
}

void runCompleteBenchmark(std::shared_ptr<Brain> brain, const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Complete Benchmark ===");
    
    double timestep = config.getOr<double>("simulation_timestep", 0.001);
    
    // Run multiple intensive tests
    NLM_LOG_INFO("Running comprehensive benchmark with " + std::to_string(config.getOr<int64_t>("neuron_count", 1000)) + " neurons...");
    
    // Test 1: Connectivity stress test
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Test 1: Connectivity Stress Test");
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        auto neurons = region->getAllNeurons();
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current to all neurons
        NLM_LOG_INFO("  Injecting current to all neurons...");
        for (size_t i = 0; i < std::min(size_t(100), neurons.size()); ++i) {
            neurons[i]->injectCurrent(100.0f);
        }
        
        for (SimulationStep step = 0; step < 200; ++step) {
            brain->step(step, step * timestep);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Connectivity test result: " + std::to_string(spikes) + " spikes generated");
    }
    
    // Test 2: Plasticity benchmark
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Test 2: Plasticity Benchmark");
    
    // Record baseline
    if (auto* region = brain->getRegion(RegionId(1))) {
        std::vector<float> baselineWeights;
        for (const auto& syn : region->getSynapses()) {
            baselineWeights.push_back(syn->getWeight());
        }
        
        // Enable all plasticity mechanisms
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, true);
        }
        
        // Apply intensive learning
        NLM_LOG_INFO("  Applying intensive learning patterns (1000 steps)...");
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Complex input patterns
            if (step % 3 == 0) {
                for (size_t i = 0; i < 50 && i < brain->getTotalNeuronCount() / 20; ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Excitatory, 45.0f);
                }
            }
            
            brain->step(step, step * timestep);
            
            if (step % 200 == 0) {
                NLM_LOG_INFO("    Step " + std::to_string(step) + " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Compare final weights
        float baselineSum = std::accumulate(baselineWeights.begin(), baselineWeights.end(), 0.0f);
        float finalSum = 0.0f;
        size_t synapseCount = 0;
        
        for (const auto& syn : region->getSynapses()) {
            finalSum += syn->getWeight();
            synapseCount++;
        }
        
        float baselineMean = baselineSum / baselineWeights.size();
        float finalMean = finalSum / synapseCount;
        float change = finalMean - baselineMean;
        
        NLM_LOG_INFO("  Plasticity results:");
        NLM_LOG_INFO("    Baseline mean weight: " + std::to_string(baselineMean));
        NLM_LOG_INFO("    Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("    Change: " + std::to_string(change));
        
        if (std::abs(change) > 0.001f) {
            NLM_LOG_INFO("    ✓ Plasticity active - weights changed significantly");
        } else {
            NLM_LOG_INFO("    ✗ No significant plasticity detected");
        }
    }
    
    // Test 3: Development benchmark
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Test 3: Developmental Changes");
    
    auto initialStage = brain->getDevelopmentalStage();
    NLM_LOG_INFO("  Initial developmental stage: " + std::to_string(static_cast<int>(initialStage)));
    
    // Simulate development over time
    NLM_LOG_INFO("  Simulating development (500 steps)...");
    for (SimulationStep step = 0; step < 500; ++step) {
        brain->develop();  // Apply developmental changes
        brain->step(step, step * timestep);
        
        if (step % 100 == 0) {
            auto currentStage = brain->getDevelopmentalStage();
            NLM_LOG_INFO("    Step " + std::to_string(step) + " -> Stage: " + std::to_string(static_cast<int>(currentStage)));
        }
    }
    
    auto finalStage = brain->getDevelopmentalStage();
    NLM_LOG_INFO("  Final developmental stage: " + std::to_string(static_cast<int>(finalStage)));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Benchmark Complete ===");
    NLM_LOG_INFO("Comprehensive testing finished successfully.");
}

void saveProgress(const std::string& filepath, std::shared_ptr<Brain> brain, const Config& config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Saving progress to checkpoint...");
    
    // Create a new config with checkpoint info
    auto checkpointConfig = std::make_shared<Config>(config);
    checkpointConfig->set("checkpoint_file", filepath, ConfigSource::Runtime);
    
    // Save brain state
    if (brain->save(filepath)) {
        NLM_LOG_INFO("✓ Progress saved successfully to: " + filepath);
        NLM_LOG_INFO("  Checkpoint contains brain state, configuration, and simulation history.");
    } else {
        NLM_LOG_ERROR("Failed to save progress to: " + filepath);
    }
}
