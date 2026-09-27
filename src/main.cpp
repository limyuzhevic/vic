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
#include "cli/CLIEntryPoint.hpp"

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
            NLM_LOG_INFO("  Weight change: " + std::to_string(mean - (initialWeights.empty() ? 0.0f : (std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f) / initialWeights.size())))));
        }
    }
};

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Test 1: Basic Connectivity ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_ERROR("  Region not found!");
        return;
    }
    
    NLM_LOG_INFO("  Testing connectivity with " + std::to_string(region->getAllNeurons().size()) + " neurons");
    
    // Check for any connections
    size_t connected = 0;
    for (const auto& syn : region->getSynapses()) {
        if (syn->getWeight() > 0.0f) {
            connected++;
        }
    }
    
    NLM_LOG_INFO("  Connected synapses: " + std::to_string(connected));
    if (connected > 0) {
        NLM_LOG_INFO("  ✓ Basic connectivity established");
    } else {
        NLM_LOG_INFO("  ! No connections found - network needs initialization");
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    brain->reset();
    brain->initialize();
    
    LearningExperiment experiment(brain, 42);
    experiment.recordInitialState();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating uncorrelated pre->post activity (baseline)...");
    
    // Create uncorrelated activity for baseline
    for (int trial = 0; trial < 20; ++trial) {
        auto neurons = brain->getRegion(RegionId(1))->getAllNeurons();
        if (neurons.size() >= 2) {
            // Fire pre-synaptic neuron
            neurons[0]->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Sometimes post fires, sometimes not
            if (trial % 2 == 0) {
                neurons[1]->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
    }
    
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    // Create correlated activity to trigger LTP
    for (int trial = 0; trial < 50; ++trial) {
        auto neurons = brain->getRegion(RegionId(1))->getAllNeurons();
        if (neurons.size() >= 2) {
            // Pre fires first
            neurons[0]->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            // Then post fires (immediately after)
            neurons[1]->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    experiment.recordFinalState();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Experiment Results:");
    NLM_LOG_INFO("    Initial synapses: " + std::to_string(experiment.initialSynapseCount));
    NLM_LOG_INFO("    Final spike count: " + std::to_string(brain->getTotalSpikeCount()));
    if (!experiment.finalWeights.empty() && !experiment.initialWeights.empty()) {
        NLM_LOG_INFO("    Initial mean weight: " + std::to_string(std::accumulate(experiment.initialWeights.begin(), experiment.initialWeights.end(), 0.0f) / experiment.initialWeights.size()));
        NLM_LOG_INFO("    Final mean weight: " + std::to_string(std::accumulate(experiment.finalWeights.begin(), experiment.finalWeights.end(), 0.0f) / experiment.finalWeights.size()));
    }
    
    float totalDelta = 0.0f;
    if (!experiment.finalWeights.empty() && !experiment.initialWeights.empty() && experiment.finalWeights.size() == experiment.initialWeights.size()) {
        for (size_t i = 0; i < experiment.finalWeights.size(); ++i) {
            totalDelta += experiment.finalWeights[i] - experiment.initialWeights[i];
        }
    }
    
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ PLASTICITY WORKING: Correlated activity produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! PLASTICITY REVERSED: Check parameters");
    } else {
        NLM_LOG_INFO("  ! NO CHANGE: Plasticity may not be triggering");
    }
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    brain->reset();n    brain->initialize();
    
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

int main(int argc, char** argv) {
    try {
        // Use the new CLI framework
        return nlm::cli::CLIEntryPoint::run(argc, argv);
    } catch (const std::exception& e) {
        ErrorReporter::getGlobal().logError("Fatal error: " + std::string(e.what()));
        return 1;
    }
}
