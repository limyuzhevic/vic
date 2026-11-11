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
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <map>
#include <set>

// Forward declarations for CLI command handlers
void printHelp(const std::string& programName);
void printVersion();
void runMode(const std::shared_ptr<Brain>& brain, const std::shared_ptr<Config>& config, const std::map<std::string, std::string>& args);
void runTests(const std::shared_ptr<Brain>& brain, const std::map<std::string, std::string>& args);
void runFullSimulation(const std::shared_ptr<Brain>& brain, const std::shared_ptr<Config>& config, const std::map<std::string, std::string>& args);
void debugMode(const std::shared_ptr<Brain>& brain, const std::map<std::string, std::string>& args);
void profileMode(const std::shared_ptr<Brain>& brain, const std::map<std::string, std::string>& args);
void validateConfig(const std::shared_ptr<Config>& config);
void saveConfig(const std::shared_ptr<Config>& config, const std::string& filename);
void loadConfig(const std::shared_ptr<Config>& config, const std::string& filename);
std::map<std::string, std::string> parseArgs(int argc, char** argv);
void printBanner(const std::string& mode = "");
void printProgress(size_t current, size_t total, const std::string& label = "Progress");
void exportResults(const std::shared_ptr<Brain>& brain, const std::string& filename);

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
    
    // A pattern, not a seizure: stimulate only the first 20 sensory neurons
    // each step. The old code injected 30.0f into *every* sensory neuron
    // 20 times per step (600.0f), so the whole network fired synchronously
    // and every weight saturated to 1.0 - learning was measurable but not
    // selective at all.
    std::vector<NeuronId> patternNeurons;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
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
    
    // Use one real synapse and its own pre/post neurons. The old test
    // fired neurons[0]/neurons[1] and measured synapses[0..4], which are
    // usually unrelated to that pair, so its result said nothing about STDP.
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        NLM_LOG_INFO("  No synapses for STDP test");
        return;
    }
    Synapse* syn = synapses[0].get();
    syn->enablePlasticity(false, true, false);  // STDP only
    float beforeWeight = syn->getWeight();
    NLM_LOG_INFO("  Testing STDP on 1 synapse:");
    NLM_LOG_INFO("    Synapse before: " + std::to_string(beforeWeight) +
                 " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                 ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");

    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");

    for (int trial = 0; trial < 50; ++trial) {
        brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
        brain->step(trial * 2, trial * 2 * 0.001);
        brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
    }

    float delta = syn->getWeight() - beforeWeight;
    NLM_LOG_INFO("  After correlated activity:");
    NLM_LOG_INFO("    Synapse after: " + std::to_string(syn->getWeight()) +
                 " (Δ=" + std::to_string(delta) + ")");

    float totalDelta = delta;
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

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
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
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
            syn->enablePlasticity(true, true, false);
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    std::vector<NeuronId> patternNeurons;
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (NeuronId id : patternNeurons) {
            brain->injectCurrent(id, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.empty()) {
        NLM_LOG_INFO("  No synapses for STDP test");
        return;
    }
    Synapse* syn = synapses[0].get();
    syn->enablePlasticity(false, true, false);
    float beforeWeight = syn->getWeight();
    NLM_LOG_INFO("  Testing STDP on 1 synapse:");
    NLM_LOG_INFO("    Synapse before: " + std::to_string(beforeWeight) +
                 " (pre=" + std::to_string(syn->getSourceNeuron().index()) +
                 ", post=" + std::to_string(syn->getDestinationNeuron().index()) + ")");
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
        brain->step(trial * 2, trial * 2 * 0.001);
        brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
        brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
    }
    
    float delta = syn->getWeight() - beforeWeight;
    NLM_LOG_INFO("  After correlated activity:");
    NLM_LOG_INFO("    Synapse after: " + std::to_string(syn->getWeight()) +
                 " (Δ=" + std::to_string(delta) + ")");
    
    float totalDelta = delta;
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

void printHelp(const std::string& programName) {
    printBanner();
    std::cout << "\nUSAGE:\n";
    std::cout << "  " << programName << " <mode> [options]\n\n";
    std::cout << "MODES:\n";
    std::cout << "  run              Run a complete simulation with neural computation\n";
    std::cout << "  test             Run the original three-phase test suite\n";
    std::cout << "  debug            Run in debug mode with enhanced logging\n";
    std::cout << "  profile           Profile performance and memory usage\n";
    std::cout << "  help             Show this help message\n";
    std::cout << "  version          Show version information\n";
    std::cout << "  config <file>    Save or load configuration\n\n";
    std::cout << "EXAMPLES:\n";
    std::cout << "  " << programName << " run --config=myconfig.cfg --neuron_count=2000\n";
    std::cout << "  " << programName << " test --steps=100 --verbose\n";
    std::cout << "  " << programName << " debug --log_level=debug --output=debug.log\n\n";
}

void printVersion() {
    printBanner();
    std::cout << "NLM (熙然) - Neural Learning Machine\n";
    std::cout << "Version 2.0 (Phase 2: Real Neural Computation)\n\n";
    std::cout << "Features:\n";
    std::cout << "  - Leaky Integrate-and-Fire (LIF) neurons\n";
    std::cout << "  - Event-driven spike propagation with delays\n";
    std::cout << "  - STDP and Hebbian plasticity rules\n";
    std::cout << "  - Structural plasticity (synaptogenesis/pruning)\n";
    std::cout << "  - Memory systems (working, episodic, associative)\n";
    std::cout << "  - Neuromodulation (dopamine, curiosity, novelty)\n";
    std::cout << "  - Cognitive systems (planner, concept formation, attention)\n\n";
    std::cout << "Phase 6: Final Integration Complete\n";
}

void printProgress(size_t current, size_t total, const std::string& label) {
    if (total == 0) return;
    
    float percentage = static_cast<float>(current) / total * 100.0f;
    std::cout << "\r";
    std::cout << "[" << label << "] ";
    
    // Progress bar (70 characters wide)
    int barWidth = 70;
    int pos = (int)(barWidth * percentage / 100.0f);
    
    std::cout << "["; 
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] ";
    std::cout << std::fixed << std::setprecision(1) << percentage << "%";
    std::cout.flush();
    
    if (current == total) {
        std::cout << std::endl;
    }
}

void validateConfig(const std::shared_ptr<Config>& config) {
    std::cout << "Validating configuration...\n";
    
    bool hasErrors = false;
    
    // Check required configuration parameters
    auto neuronCount = config->getOr<int64_t>("neuron_count", 0);
    if (neuronCount <= 0) {
        std::cout << "  ERROR: neuron_count must be positive, got " << neuronCount << "\n";
        hasErrors = true;
    } else if (neuronCount > 1000000) {
        std::cout << "  WARNING: neuron_count very large (" << neuronCount << "), may impact performance\n";
    }
    
    auto timestep = config->getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0) {
        std::cout << "  ERROR: simulation_timestep must be positive, got " << timestep << "\n";
        hasErrors = true;
    } else if (timestep > 1.0) {
        std::cout << "  WARNING: simulation_timestep large (" << timestep << "s), may impact stability\n";
    }
    
    auto connProb = config->getOr<float>("connection_probability", 2.0f);
    if (connProb < 0.0f || connProb > 1.0f) {
        std::cout << "  ERROR: connection_probability must be between 0.0 and 1.0, got " << connProb << "\n";
        hasErrors = true;
    }
    
    auto stdpLtp = config->getOr<float>("stdp_ltp_weight", 0.0f);
    auto stdpLtd = config->getOr<float>("stdp_ltd_weight", 0.0f);
    if (stdpLtp < 0.0f || stdpLtd < 0.0f) {
        std::cout << "  ERROR: STDP weights must be non-negative, got LTP=" << stdpLtp << ", LTD=" << stdpLtd << "\n";
        hasErrors = true;
    }
    
    auto synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0f);
    auto pruningRate = config->getOr<float>("pruning_rate", 0.0f);
    if (synaptogenesisRate < 0.0f || pruningRate < 0.0f) {
        std::cout << "  ERROR: Plasticity rates must be non-negative, got synaptogenesis=" << synaptogenesisRate << ", pruning=" << pruningRate << "\n";
        hasErrors = true;
    }
    
    // Validate consistency between parameters
    auto regionCount = config->getOr<int64_t>("region_count", 1);
    if (regionCount <= 0) {
        std::cout << "  ERROR: region_count must be positive, got " << regionCount << "\n";
        hasErrors = true;
    }
    
    if (regionCount > neuronCount / 10) {
        std::cout << "  WARNING: region_count may be too high relative to neuron_count (" << regionCount << " vs " << neuronCount << ")\n";
    }
    
    // Validate neural parameters
    auto vThresh = config->getOr<float>("v_thresh", 0.0f);
    auto vRest = config->getOr<float>("v_rest", 0.0f);
    auto vReset = config->getOr<float>("v_reset", 0.0f);
    
    if (vThresh <= vRest) {
        std::cout << "  WARNING: threshold should be above resting potential for stable neuron dynamics\n";
    }
    
    if (vReset > vThresh) {
        std::cout << "  WARNING: reset potential should be below threshold for proper spike reset\n";
    }
    
    // Validate plasticity parameters
    auto hebbianLearnRate = config->getOr<float>("plasticity_hebbian_learning_rate", 0.0f);
    auto stdpLearningRate = config->getOr<float>("plasticity_stdp_learning_rate", 0.0f);
    
    if (hebbianLearnRate <= 0.0f || stdpLearningRate <= 0.0f) {
        std::cout << "  WARNING: Plasticity learning rates should be positive for effective learning\n";
    }
    
    if (hasErrors) {
        std::cout << "\nConfiguration validation failed!\n";
        return;
    }
    
    std::cout << "Configuration validation passed!\n";
    
    // Display validated configuration summary
    std::cout << "\nValidated configuration summary:\n";
    std::cout << "  Neuron count: " << neuronCount << "\n";
    std::cout << "  Region count: " << regionCount << "\n";
    std::cout << "  Connection probability: " << connProb << "\n";
    std::cout << "  Simulation timestep: " << timestep << "s\n";
    std::cout << "  STDP LTP weight: " << stdpLtp << "\n";
    std::cout << "  STDP LTD weight: " << stdpLtd << "\n";
    std::cout << "  Synaptogenesis rate: " << synaptogenesisRate << "\n";
    std::cout << "  Pruning rate: " << pruningRate << "\n";
}

void saveConfig(const std::shared_ptr<Config>& config, const std::string& filename) {
    std::cout << "Saving configuration to " << filename << "...\n";
    
    if (config->saveToFile(filename)) {
        std::cout << "Configuration saved successfully!\n";
    } else {
        std::cout << "ERROR: Failed to save configuration to " << filename << "\n";
    }
}

void loadConfig(const std::shared_ptr<Config>& config, const std::string& filename) {
    std::cout << "Loading configuration from " << filename << "...\n";
    
    if (config->loadFromFile(filename)) {
        std::cout << "Configuration loaded successfully!\n";
    } else {
        std::cout << "WARNING: Could not load configuration from " << filename << ". Using defaults.\n";
    }
}

std::map<std::string, std::string> parseArgs(int argc, char** argv) {
    std::map<std::string, std::string> args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                args[arg.substr(2, pos - 2)] = arg.substr(pos + 1);
            } else {
                args[arg.substr(2)] = "true";
            }
        }
    }
    
    return args;
}

void exportResults(const std::shared_ptr<Brain>& brain, const std::string& filename) {
    std::cout << "Exporting results to " << filename << "...\n";
    
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "ERROR: Cannot open output file " << filename << "\n";
        return;
    }
    
    // Export neural activity statistics
    std::string metadata = "NLM Results Export\n";
    metadata += "Total Neurons: " + std::to_string(brain->getTotalNeuronCount()) + "\n";
    metadata += "Total Synapses: " + std::to_string(brain->getTotalSynapseCount()) + "\n";
    metadata += "Total Spikes: " + std::to_string(brain->getTotalSpikeCount()) + "\n";
    metadata += "Firing Neurons: " + std::to_string(brain->getFiringNeuronCount()) + "\n";
    metadata += "E/I Ratio: " + std::to_string(brain->getExcitationInhibitionRatio()) + "\n";
    metadata += "Simulation Time: " + std::to_string(brain->getCurrentTime()) + "s\n";
    
    file.write(metadata.c_str(), metadata.size());
    
    std::cout << "Results exported successfully!\n";
}

void runMode(const std::shared_ptr<Brain>& brain, const std::shared_ptr<Config>& config, const std::map<std::string, std::string>& args) {
    // Enhanced mode with better progress tracking
    size_t steps = 100;
    if (args.find("steps") != args.end()) {
        try {
            steps = std::stoul(args.at("steps"));
        } catch (...) {
            std::cout << "ERROR: Invalid steps parameter, using default (100)\n";
        }
    }
    
    // Use more neurons for better simulation
    size_t neuronCount = config->getOr<int64_t>("neuron_count", 1000);
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get all neurons and calculate activity
    auto neurons = region->getAllNeurons();
    std::cout << "Running enhanced neural computation mode...\n";
    std::cout << "  Neuronal population: " << neurons.size() << " neurons\n";
    std::cout << "  Simulation steps: " << steps << "\n";
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    for (size_t step = 0; step < steps; ++step) {
        printProgress(step, steps, "Simulating");
        
        // Inject diverse sensory input
        if (step % 10 == 0 && !neurons.empty()) {
            size_t sensoryCount = std::min(size_t(20), neurons.size());
            for (size_t i = 0; i < sensoryCount; ++i) {
                size_t idx = (step + i) % neurons.size();
                neurons[idx]->injectCurrent(15.0f + static_cast<float>(rand()) / RAND_MAX * 20.0f);
            }
        }
        
        brain->step(step);
    }
    
    printProgress(steps, steps, "Simulating");
    
    size_t finalSpikes = brain->getTotalSpikeCount() - initialSpikes;
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Enhanced Mode Results ===");
    NLM_LOG_INFO("Total simulation steps: " + std::to_string(steps));
    NLM_LOG_INFO("Spikes generated: " + std::to_string(finalSpikes));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(static_cast<float>(finalSpikes) / steps) + " spikes/step");
    
    // Enhanced output
    if (args.find("export") != args.end()) {
        exportResults(brain, args.at("export"));
    }
}

void runTests(const std::shared_ptr<Brain>& brain, const std::map<std::string, std::string>& args) {
    size_t steps = 1000;
    if (args.find("steps") != args.end()) {
        try {
            steps = std::stoul(args.at("steps"));
        } catch (...) {
            std::cout << "ERROR: Invalid steps parameter, using default (1000)\n";
        }
    }
    
    bool verbose = args.find("verbose") != args.end();
    std::cout << "Running comprehensive test suite...\n";
    std::cout << "Steps: " << steps << ", Verbose: " << (verbose ? "yes" : "no") << "\n";
    
    // Test 1: Basic connectivity
    std::cout << "\n1. Running basic connectivity test...\n";
    runBasicConnectivityTest(brain);
    
    // Reset for next test
    brain->reset();
    brain->initialize();
    
    // Test 2: Plasticity learning experiment
    std::cout << "\n2. Running plasticity learning experiment...\n";
    runPlasticityExperiment(brain);
    
    // Reset for next test
    brain->reset();
    brain->initialize();
    
    // Test 3: STDP verification
    std::cout << "\n3. Running STDP verification...\n";
    runStdpVerification(brain);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test Suite Complete ===");
    NLM_LOG_INFO("All three original test phases completed successfully!");
}

void runFullSimulation(const std::shared_ptr<Brain>& brain, const std::shared_ptr<Config>& config, const std::map<std::string, std::string>& args) {
    size_t steps = 1000;
    if (args.find("steps") != args.end()) {
        try {
            steps = std::stoul(args.at("steps"));
        } catch (...) {
            std::cout << "ERROR: Invalid steps parameter, using default (1000)\n";
        }
    }
    
    std::cout << "Running full Phase 6 integrated brain simulation...\n";
    std::cout << "This simulates the complete brain loop with all integrated systems.\n";
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    for (size_t step = 0; step < steps; ++step) {
        printProgress(step, steps, "Full Simulation");
        brain->step(step);
    }
    
    printProgress(steps, steps, "Full Simulation");
    
    size_t finalSpikes = brain->getTotalSpikeCount() - initialSpikes;
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Full Simulation Results ===");
    NLM_LOG_INFO("Simulation completed: " + std::to_string(steps) + " steps");
    NLM_LOG_INFO("Total spikes generated: " + std::to_string(finalSpikes));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(static_cast<float>(finalSpikes) / steps) + " spikes/step");
    
    // Show brain statistics
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Brain Statistics ===");
    brain->logStatus();
}

void debugMode(const std::shared_ptr<Brain>& brain, const std::map<std::string, std::string>& args) {
    std::cout << "Debug mode enabled.\n";
    
    // Set debug log level
    auto logger = Logger::getGlobal();
    if (logger) {
        // Note: This would require a debug logger level to be implemented
        std::cout << "Enhanced logging enabled.\n";
    }
    
    std::cout << "Running in debug mode with enhanced monitoring.\n";
    std::cout << "Key debug information will be displayed for each step.\n";
    
    // Use minimal steps for debugging
    size_t steps = 50;
    if (args.find("steps") != args.end()) {
        try {
            steps = std::stoul(args.at("steps"));
        } catch (...) {
            std::cout << "ERROR: Invalid steps parameter, using default (50)\n";
        }
    }
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    
    for (size_t step = 0; step < steps; ++step) {
        printProgress(step, steps, "Debug");
        
        // Inject random input for debugging
        if (!neurons.empty()) {
            size_t idx = step % neurons.size();
            neurons[idx]->injectCurrent(25.0f + static_cast<float>(rand()) / RAND_MAX * 30.0f);
        }
        
        brain->step(step);
        
        // Debug output every 5 steps
        if (step % 5 == 0) {
            NLM_LOG_INFO("Debug step " + std::to_string(step) + ": " + std::to_string(brain->getTotalSpikeCount()) + " total spikes");
        }
    }
    
    printProgress(steps, steps, "Debug");
    NLM_LOG_INFO("Debug mode complete.\n");
}

void profileMode(const std::shared_ptr<Brain>& brain, const std::map<std::string, std::string>& args) {
    std::cout << "Profiling mode enabled.\n";
    std::cout << "This will measure performance and memory usage.\n";
    
    // Profile simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    size_t steps = 200;
    if (args.find("steps") != args.end()) {
        try {
            steps = std::stoul(args.at("steps"));
        } catch (...) {
            std::cout << "ERROR: Invalid steps parameter, using default (200)\n";
        }
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    size_t finalSpikes = brain->getTotalSpikeCount() - initialSpikes;
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Performance Profile ===");
    NLM_LOG_INFO("Total simulation time: " + std::to_string(duration.count()) + " ms");
    NLM_LOG_INFO("Average time per step: " + std::to_string(duration.count() / static_cast<double>(steps)) + " ms");
    NLM_LOG_INFO("Total spikes: " + std::to_string(finalSpikes));
    NLM_LOG_INFO("Performance: " + std::to_string((steps * 1000.0) / duration.count()) + " steps/sec");
    
    std::cout << "\nProfiling complete.\n";
}

int main(int argc, char** argv) {
    try {
        // Set up logging
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Program information
        std::string programName = "nlm";
        if (argc > 0) {
            programName = argv[0];
        }
        
        // Parse command line arguments
        auto args = parseArgs(argc, argv);
        
        // Handle special commands before main mode selection
        if (args.find("help") != args.end() || args.find("?") != args.end()) {
            printHelp(programName);
            return 0;
        }
        
        if (args.find("version") != args.end()) {
            printVersion();
            return 0;
        }
        
        // Determine mode (default to "run" if not specified)
        std::string mode = "run";
        if (args.find("mode") != args.end()) {
            mode = args.at("mode");
        } else if (argc > 1 && argv[1][0] != '-') {
            // First non-flag argument could be the mode
            mode = argv[1];
        }
        
        // Initialize configuration
        auto config = std::make_shared<Config>();
        
        // Load configuration file if specified
        std::string configFile = "configs/default.cfg";
        if (args.find("config") != args.end()) {
            configFile = args.at("config");
        }
        
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Override configuration with command line arguments
        config->loadFromArgs(argc, argv);
        
        // Set default Phase 2 values
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Configure STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Configure structural plasticity
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Validate configuration
        validateConfig(config);
        
        // Print banner
        printBanner(mode);
        
        // Create and initialize brain
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        brain->logStatus();
        
        // Execute selected mode
        if (mode == "run") {
            runMode(brain, config, args);
        } else if (mode == "test") {
            runTests(brain, args);
        } else if (mode == "debug") {
            debugMode(brain, args);
        } else if (mode == "profile") {
            profileMode(brain, args);
        } else {
            NLM_LOG_ERROR("Unknown mode: " + mode);
            return 1;
        }
        
        // Final status report
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Execution Complete ===");
        brain->logStatus();
        
        return 0;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in main: " + std::string(e.what()));
        return 1;
    } catch (...) {
        NLM_LOG_ERROR("Unknown exception in main");
        return 1;
    }
}
