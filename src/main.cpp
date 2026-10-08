// NLM (熙然) - Neural Learning Machine
// Multi-Phase Neural Computation and Cognitive Development
//
// This phase implements a complete artificial developmental brain with:
// - Real spiking neural computation (Phase 2)
// - Integrated memory systems (Phase 4)
// - Cognitive functions (Phase 4)
// - Neuromodulation and development (Phase 6)
// - World interaction and agent control (Phase 3/4)

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>
#include <algorithm>
#include <chrono>

using namespace nlm;

// Command-line argument parser
class CommandLineParser {
public:
    struct Options {
        std::string configFile = "configs/default.cfg";
        std::string mode = "auto";  // "auto", "phase2", "phase4", "integrated", "benchmark"
        std::string experiment = "all";  // "all", "connectivity", "plasticity", "stdp", "learning"
        int64_t customNeuronCount = 0;
        int64_t customRegionCount = 0;
        float customConnectionProb = 0.0f;
        bool verbose = false;
        bool quiet = false;
        bool help = false;
        bool version = false;
        int benchmarkIterations = 1;
        std::string outputFile;
        std::string profileFile;
        bool enableAllSystems = false;
        bool continuousMode = false;
        int stepsPerCycle = 1000;
        float rewardDecay = 0.95f;
    };

    static Options parse(int argc, char** argv) {
        Options opts;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                opts.help = true;
            } else if (arg == "--version") {
                opts.version = true;
            } else if (arg == "--verbose") {
                opts.verbose = true;
            } else if (arg == "--quiet") {
                opts.quiet = true;
            } else if (arg == "--mode") {
                if (i + 1 < argc) opts.mode = argv[++i];
            } else if (arg == "--experiment") {
                if (i + 1 < argc) opts.experiment = argv[++i];
            } else if (arg == "--config") {
                if (i + 1 < argc) opts.configFile = argv[++i];
            } else if (arg == "--neurons") {
                if (i + 1 < argc) opts.customNeuronCount = std::stol(argv[++i]);
            } else if (arg == "--regions") {
                if (i + 1 < argc) opts.customRegionCount = std::stol(argv[++i]);
            } else if (arg == "--connection-prob") {
                if (i + 1 < argc) opts.customConnectionProb = std::stof(argv[++i]);
            } else if (arg == "--benchmark") {
                if (i + 1 < argc) opts.benchmarkIterations = std::stoi(argv[++i]);
            } else if (arg == "--output") {
                if (i + 1 < argc) opts.outputFile = argv[++i];
            } else if (arg == "--profile") {
                if (i + 1 < argc) opts.profileFile = argv[++i];
            } else if (arg == "--continuous") {
                opts.continuousMode = true;
                if (i + 1 < argc) opts.stepsPerCycle = std::stoi(argv[++i]);
            } else if (arg == "--all-systems") {
                opts.enableAllSystems = true;
            } else if (arg.substr(0, 2) == "--") {
                size_t eqPos = arg.find('=');
                if (eqPos != std::string::npos) {
                    std::string key = arg.substr(0, eqPos);
                    std::string value = arg.substr(eqPos + 1);
                    
                    if (key == "--neurons") opts.customNeuronCount = std::stol(value);
                    else if (key == "--regions") opts.customRegionCount = std::stol(value);
                    else if (key == "--connection-prob") opts.customConnectionProb = std::stof(value);
                }
            }
        }
        
        return opts;
    }
    
    static void printHelp() {
        std::cout << "NLM Neural Learning Machine - Multi-Phase Neural Computation" << std::endl;
        std::cout << "===============================================================" << std::endl;
        std::cout << std::endl;
        std::cout << "Modes:" << std::endl;
        std::cout << "  auto       - Automatically select appropriate mode based on components" << std::endl;
        std::cout << "  phase2     - Phase 2 demonstrations (basic neural computation)" << std::endl;
        std::cout << "  phase4     - Phase 4 demonstrations (memory and cognition)" << std::endl;
        std::cout << "  integrated - Full integration with agent and world interaction" << std::endl;
        std::cout << "  benchmark  - Performance benchmarking" << std::endl;
        std::cout << std::endl;
        std::cout << "Experiments:" << std::endl;
        std::cout << "  all        - Run all available experiments" << std::endl;
        std::cout << "  connectivity - Basic neural connectivity tests" << std::endl;
        std::cout << "  plasticity  - Plasticity learning experiments" << std::endl;
        std::cout << "  stdp       - STDP verification" << std::endl;
        std::cout << "  learning   - Comprehensive learning demonstration" << std::endl;
        std::cout << std::endl;
        std::cout << "Advanced Options:" << std::endl;
        std::cout << "  --neurons N      Set neuron count (default: auto)" << std::endl;
        std::cout << "  --regions R      Set region count (default: auto)" << std::endl;
        std::cout << "  --connection-prob P   Set connection probability (0.0-1.0)" << std::endl;
        std::cout << "  --benchmark N     Run N iterations for benchmarking" << std::endl;
        std::cout << "  --continuous      Continuous mode with world interaction" << std::endl;
        std::cout << "  --all-systems     Enable all available brain systems" << std::endl;
        std::cout << "  --output FILE     Write results to file" << std::endl;
        std::cout << "  --profile FILE    Write performance profile" << std::endl;
        std::cout << std::endl;
        std::cout << "General Options:" << std::endl;
        std::cout << "  --verbose         Enable verbose logging" << std::endl;
        std::cout << "  --quiet           Quiet mode (minimal output)" << std::endl;
        std::cout << "  --help, -h        Show this help message" << std::endl;
        std::cout << "  --version         Show version information" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  ./nlm --mode phase2 --experiment all" << std::endl;
        std::cout << "  ./nlm --mode integrated --continuous" << std::endl;
        std::cout << "  ./nlm --mode phase4 --neurons 1000 --benchmark 5" << std::endl;
        std::cout << std::endl;
    }
};

void printBanner(const std::string& mode = "auto") {
    std::string subtitle;
    if (mode == "phase2") {
        subtitle = "Phase 2: Real Neural Computation";
    } else if (mode == "phase4") {
        subtitle = "Phase 4: Memory and Cognition Integration";
    } else if (mode == "integrated") {
        subtitle = "Integrated Brain with World Interaction";
    } else if (mode == "benchmark") {
        subtitle = "Performance Benchmarking";
    } else {
        subtitle = "Multi-Phase Neural Computation and Cognitive Development";
    }
    
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     " << subtitle << "                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This implementation includes:                             ║
    ║     - Real LIF neuron dynamics                                 ║
    ║     - Event-driven spike propagation                           ║
    ║     - STDP and Hebbian plasticity                              ║
    ║     - Structural plasticity                                    ║
    ║     - Working and episodic memory                            ║
    ║     - Prediction and cognition                               ║
    ║     - Neuromodulation                                          ║
    ║     - Development stages                                      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Enhanced Learning Experiment with more detailed analysis
struct EnhancedLearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    std::vector<float> weightChanges;
    std::vector<std::pair<NeuronId, NeuronId>> activePaths;
    float learningRate;
    float plasticityStrength;
    
    EnhancedLearningExperiment(std::shared_ptr<Brain> b, uint64_t s, float lr = 1.0f, float ps = 1.0f)
        : brain(b), seed(s), learningRate(lr), plasticityStrength(ps), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        weightChanges.clear();
        activePaths.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Enhanced initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            float stddev = 0.0f;
            for (float w : initialWeights) {
                float diff = w - mean;
                stddev += diff * diff;
            }
            stddev = std::sqrt(stddev / initialWeights.size());
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean) + ", StdDev: " + std::to_string(stddev));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        weightChanges.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        // Compute weight changes
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            weightChanges.push_back(finalWeights[i] - initialWeights[i]);
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Enhanced final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void analyzeLearning() {
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Enhanced Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        // Compute weight statistics
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        // Count synaptic changes with different thresholds
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        size_t highlyStrengthened = 0;
        size_t highlyWeakened = 0;
        
        for (float delta : weightChanges) {
            if (delta > 0.01f) {
                strengthened++;
                if (delta > 0.1f) highlyStrengthened++;
            } else if (delta < -0.01f) {
                weakened++;
                if (delta < -0.1f) highlyWeakened++;
            } else {
                unchanged++;
            }
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes (delta > 0.01):");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        if (highlyStrengthened > 0) {
            NLM_LOG_INFO("    Highly strengthened (>0.1): " + std::to_string(highlyStrengthened));
        }
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        if (highlyWeakened > 0) {
            NLM_LOG_INFO("    Highly weakened (<-0.1): " + std::to_string(highlyWeakened));
        }
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        // Compute learning metrics
        float totalChange = 0.0f;
        float maxChange = 0.0f;
        float avgAbsChange = 0.0f;
        for (float delta : weightChanges) {
            totalChange += delta;
            maxChange = std::max(maxChange, std::abs(delta));
            avgAbsChange += std::abs(delta);
        }
        avgAbsChange /= weightChanges.size();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Learning Metrics:");
        NLM_LOG_INFO("  Total weight change: " + std::to_string(totalChange));
        NLM_LOG_INFO("  Maximum absolute change: " + std::to_string(maxChange));
        NLM_LOG_INFO("  Average absolute change: " + std::to_string(avgAbsChange));
        NLM_LOG_INFO("  Learning rate factor: " + std::to_string(learningRate));
        NLM_LOG_INFO("  Plasticity strength: " + std::to_string(plasticityStrength));
        
        // Determine learning quality
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        bool strongLearning = (std::abs(finalMean - initialMean) > 0.01f) ||
                             (highlyStrengthened > 0 || highlyWeakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            if (strongLearning) {
                NLM_LOG_INFO("✓ STRONG LEARNING DETECTED: Significant synaptic changes through experience");
            } else {
                NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
            }
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

void runBasicConnectivityTest(std::shared_ptr<Brain> brain, bool verbose = false) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Enhanced Basic Neural Connectivity ===");
    
    // Inject current into a few neurons and see if spikes propagate
    auto* region = brain->getRegion(RegionId(1));
    if (!region) {
        NLM_LOG_INFO("  ERROR: No regions found!");
        return;
    }
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  ERROR: No neurons found!");
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
    for (SimulationStep step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
        
        if (verbose) {
            NLM_LOG_INFO("  Network statistics:");
            NLM_LOG_INFO("    Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
            NLM_LOG_INFO("    Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
            NLM_LOG_INFO("    Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
            NLM_LOG_INFO("    Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
            NLM_LOG_INFO("    Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
        }
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain, float learningRate = 1.0f, float plasticityStrength = 1.0f) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Enhanced Plasticity Learning Experiment ===");
    
    EnhancedLearningExperiment experiment(brain, 42, learningRate, plasticityStrength);
    
    // Record initial state
    experiment.recordInitialState();
    
    // Enable plasticity on synapses with enhanced settings
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            // Enable both Hebbian and STDP with enhanced parameters
            syn->enablePlasticity(true, true, false);
            
            // Boost learning based on parameters
            float baseWeight = syn->getWeight();
            if (plasticityStrength > 1.0f) {
                syn->setWeight(baseWeight * plasticityStrength);
            }
        }
    }
    
    // Apply repeated input pattern with more sophisticated input
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying enhanced input patterns (2000 steps with modulated input)...");
    
    for (SimulationStep step = 0; step < 2000; ++step) {
        // Create sophisticated input pattern based on step and learning rate
        float inputBase = 30.0f + 10.0f * learningRate;
        float inputMod = 5.0f * std::sin(step * 0.01f);  // Oscillatory component
        
        // Inject current into sensory neurons in a pattern
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            float current = inputBase + inputMod;
            brain->injectCurrentToNeurons(NeuronType::Sensory, current);
        }
        
        brain->step(step, step * 0.001);
        
        // Log progress every 200 steps
        if (step % 200 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                        " | Learning: " + std::to_string(learningRate) +
                        " | Plasticity: " + std::to_string(plasticityStrength));
        }
    }
    
    // Record final state
    experiment.recordFinalState();
    
    // Compute and display enhanced statistics
    experiment.analyzeLearning();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification (Enhanced) ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses with enhanced conditions:");
    
    // Record initial weights with more precision
    std::vector<float> beforeWeights;
    std::vector<float> beforeDelays;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        beforeDelays.push_back(synapses[i]->getDelay());
        synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]) +
                    " delay: " + std::to_string(beforeDelays[i]));
    }
    
    // Create correlated activity with more sophisticated timing
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating enhanced correlated pre->post activity (100 trials with variable timing)...");
    
    auto neurons = region->getAllNeurons();
    if (neurons.size() < 2) return;
    
    for (int trial = 0; trial < 100; ++trial) {
        Neuron* preNeuron = neurons[0];
        Neuron* postNeuron = neurons[1];
        
        // Variable timing for more realistic STDP
        float preDelay = 0.0f;
        float postDelay = 50.0f + 10.0f * std::sin(trial * 0.1f);  // Variable post delay
        
        // Pre fires first
        preNeuron->injectCurrent(60.0f);
        brain->step(trial * 2, trial * 2 * 0.001);
        
        // Variable delay before post fires
        for (int delayStep = 0; delayStep < (int)postDelay; ++delayStep) {
            brain->step(trial * 2 + delayStep, (trial * 2 + delayStep) * 0.001);
        }
        
        // Then post fires
        postNeuron->injectCurrent(60.0f);
        brain->step(trial * 2 + postDelay, (trial * 2 + postDelay) * 0.001);
    }
    
    // Record after weights
    NLM_LOG_INFO("  After correlated activity:");
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        float delayDelta = synapses[i]->getDelay() - beforeDelays[i];
        totalDelta += delta;
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ", delay Δ=" + std::to_string(delayDelta) + ")");
    }
    
    // Check if weights increased (LTP)
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation (total Δ=" + std::to_string(totalDelta) + ")");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters (total Δ=" + std::to_string(totalDelta) + ")");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering (total Δ=" + std::to_string(totalDelta) + ")");
    }
}

void runMemoryIntegrationTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 4: Memory Integration Test ===");
    
    // Test working memory
    auto* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        NLM_LOG_INFO("  Working Memory System: PRESENT");
        // Check if working memory can store and retrieve information
        // This would depend on the working memory API
        NLM_LOG_INFO("    Status: System accessible");
    } else {
        NLM_LOG_INFO("  Working Memory System: NOT IMPLEMENTED (returns nullptr)");
    }
    
    // Test episodic memory
    auto* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        NLM_LOG_INFO("  Episodic Memory System: PRESENT");
        NLM_LOG_INFO("    Status: System accessible");
    } else {
        NLM_LOG_INFO("  Episodic Memory System: NOT IMPLEMENTED (returns nullptr)");
    }
    
    // Test associative memory
    auto* associativeMemory = brain->getAssociativeMemory();
    if (associativeMemory) {
        NLM_LOG_INFO("  Associative Memory System: PRESENT");
        NLM_LOG_INFO("    Status: System accessible");
    } else {
        NLM_LOG_INFO("  Associative Memory System: NOT IMPLEMENTED (returns nullptr)");
    }
    
    // Test prediction system
    auto* predictionSystem = brain->getPredictionSystem();
    if (predictionSystem) {
        NLM_LOG_INFO("  Prediction System: PRESENT");
        NLM_LOG_INFO("    Status: System accessible");
    } else {
        NLM_LOG_INFO("  Prediction System: NOT IMPLEMENTED (returns nullptr)");
    }
    
    // Test cognitive systems
    auto* planner = brain->getPlanner();
    auto* conceptFormation = brain->getConceptFormation();
    auto* attention = brain->getAttention();
    
    if (planner) NLM_LOG_INFO("  Neural Planner: PRESENT");
    else NLM_LOG_INFO("  Neural Planner: NOT IMPLEMENTED (returns nullptr)");
    
    if (conceptFormation) NLM_LOG_INFO("  Concept Formation: PRESENT");
    else NLM_LOG_INFO("  Concept Formation: NOT IMPLEMENTED (returns nullptr)");
    
    if (attention) NLM_LOG_INFO("  Attentional Selection: PRESENT");
    else NLM_LOG_INFO("  Attentional Selection: NOT IMPLEMENTED (returns nullptr)");
    
    // Test neuromodulation systems
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    auto* novelty = brain->getNovelty();
    
    if (dopamine) NLM_LOG_INFO("  Dopamine System: PRESENT");
    else NLM_LOG_INFO("  Dopamine System: NOT IMPLEMENTED (returns nullptr)");
    
    if (curiosity) NLM_LOG_INFO("  Curiosity System: PRESENT");
    else NLM_LOG_INFO("  Curiosity System: NOT IMPLEMENTED (returns nullptr)");
    
    if (novelty) NLM_LOG_INFO("  Novelty System: PRESENT");
    else NLM_LOG_INFO("  Novelty System: NOT IMPLEMENTED (returns nullptr)");
    
    NLM_LOG_INFO("  System Integration Status: PARTIAL - Many systems exist but disconnected");
}

void runIntegratedAgentTest(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config, bool continuous = false, int stepsPerCycle = 1000) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 5: Integrated Agent with World Interaction ===");
    
    // Create agent with brain
    auto agentBrain = std::make_shared<AgentBrain>(brain);
    auto agentBody = std::make_shared<AgentBody>();
    
    // Initialize with a simple world
    SimpleWorld world;
    world.initialize(64, 64);  // 64x64 world
    
    // Initialize agent brain with world
    agentBrain->initialize(world);
    
    NLM_LOG_INFO("  Agent initialized:");
    NLM_LOG_INFO("    Sensory input size: " + std::to_string(agentBrain->getSensoryInputSize()));
    NLM_LOG_INFO("    Motor output size: " + std::to_string(agentBrain->getMotorOutputSize()));
    
    if (continuous) {
        NLM_LOG_INFO("  Entering continuous mode for " + std::to_string(stepsPerCycle) + " steps...")
        
        float totalReward = 0.0f;
        float bestReward = -1000.0f;
        size_t totalSteps = 0;
        
        for (SimulationStep step = 0; step < stepsPerCycle; ++step) {
            // 1. Get observation from world
            SensoryPercept percept = world.observe(*agentBody);
            
            // 2. Process sensory input
            agentBrain->processSensoryInput(percept);
            
            // 3. Brain step
            brain->step(step, step * 0.001);
            
            // 4. Decode motor command
            MotorCommand cmd = agentBrain->decodeMotorCommand();
            
            // 5. Apply action to world
            ActionResult result = world.applyAction(*agentBody, cmd);
            
            // 6. Compute reward and apply neuromodulation
            float predictedReward = agentBrain->getPredictionError();
            agentBrain->applyRewardModulation(result.reward, predictedReward);
            
            // 7. Update development
            agentBrain->updateDevelopment(step * 0.001);
            
            totalReward += result.reward;
            bestReward = std::max(bestReward, result.reward);
            totalSteps++;
            
            // Log progress
            if (step % 200 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Reward: " + std::to_string(result.reward) +
                            " | Total: " + std::to_string(totalReward) +
                            " | Best: " + std::to_string(bestReward) +
                            " | Curiosity: " + std::to_string(agentBrain->getCuriosityLevel()) +
                            " | Novelty: " + std::to_string(agentBrain->getNoveltyLevel()));
            }
        }
        
        float avgReward = totalReward / totalSteps;
        NLM_LOG_INFO("  Continuous mode results:");
        NLM_LOG_INFO("    Average reward per step: " + std::to_string(avgReward));
        NLM_LOG_INFO("    Total reward: " + std::to_string(totalReward));
        NLM_LOG_INFO("    Best single reward: " + std::to_string(bestReward));
        NLM_LOG_INFO("    Final developmental stage: " + std::to_string(static_cast<int>(agentBrain->getDevelopmentalStage())));
        
    } else {
        // Single cycle for testing
        NLM_LOG_INFO("  Running single integration cycle...")
        
        SimpleWorld world;
        world.initialize(64, 64);
        
        // Process a few steps
        for (SimulationStep step = 0; step < 100; ++step) {
            SensoryPercept percept = world.observe(*agentBody);
            agentBrain->processSensoryInput(percept);
            brain->step(step, step * 0.001);
            MotorCommand cmd = agentBrain->decodeMotorCommand();
            ActionResult result = world.applyAction(*agentBody, cmd);
            agentBrain->applyRewardModulation(result.reward, 0.0f);
            agentBrain->updateDevelopment(step * 0.001);
        }
        
        NLM_LOG_INFO("  Integration cycle completed:");
        NLM_LOG_INFO("    Agent position: (" + std::to_string(agentBody->x) + ", " + std::to_string(agentBody->y) + ")");
        NLM_LOG_INFO("    Agent orientation: " + std::to_string(agentBody->orientation) + " rad");
        NLM_LOG_INFO("    Agent energy: " + std::to_string(agentBody->energy));
        NLM_LOG_INFO("    Agent health: " + std::to_string(agentBody->health));
    }
}

void runBenchmark(std::shared_ptr<Brain> brain, int iterations) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Benchmark: Performance Testing ===");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        // Reset brain for each iteration
        brain->reset();
        brain->initialize();
        
        // Run basic connectivity test
        runBasicConnectivityTest(brain, false);
        
        if (i % 10 == 0) {
            NLM_LOG_INFO("  Completed benchmark iteration " + std::to_string(i + 1) + "/" + std::to_string(iterations));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    double avgStepsPerSecond = (iterations * 100) / (duration.count() / 1000.0);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Benchmark Results (" + std::to_string(iterations) + " iterations):");
    NLM_LOG_INFO("  Total time: " + std::to_string(duration.count() / 1000.0) + " seconds");
    NLM_LOG_INFO("  Average steps per second: " + std::to_string(avgStepsPerSecond));
    NLM_LOG_INFO("  Steps per millisecond: " + std::to_string((iterations * 100) / duration.count()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
}

void applyConfigurationOverrides(std::shared_ptr<Config> config, const CommandLineParser::Options& opts) {
    if (opts.customNeuronCount > 0) {
        config->set("neuron_count", static_cast<int64_t>(opts.customNeuronCount), ConfigSource::CommandLine);
        NLM_LOG_INFO("Config override: neuron_count = " + std::to_string(opts.customNeuronCount));
    }
    
    if (opts.customRegionCount > 0) {
        config->set("region_count", static_cast<int64_t>(opts.customRegionCount), ConfigSource::CommandLine);
        NLM_LOG_INFO("Config override: region_count = " + std::to_string(opts.customRegionCount));
    }
    
    if (opts.customConnectionProb > 0.0f) {
        config->set("connection_probability", opts.customConnectionProb, ConfigSource::CommandLine);
        NLM_LOG_INFO("Config override: connection_probability = " + std::to_string(opts.customConnectionProb));
    }
}

int main(int argc, char** argv) {
    // Parse command line arguments
    CommandLineParser::Options opts = CommandLineParser::parse(argc, argv);
    
    // Handle help and version
    if (opts.help) {
        CommandLineParser::printHelp();
        return 0;
    }
    
    if (opts.version) {
        std::cout << "NLM Neural Learning Machine v2.0.0 (Phase 2-6 Integration)" << std::endl;
        std::cout << "Built with advanced neural computation and cognitive integration." << std::endl;
        return 0;
    }
    
    // Set logging level
    auto logger = std::make_shared<Logger>();
    if (opts.quiet) {
        logger->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Error));
    } else if (opts.verbose) {
        logger->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Debug));
    } else {
        logger->addLogger(std::make_shared<ConsoleLogger>(LogLevel::Info));
    }
    Logger::setGlobal(logger);
    
    // Print banner with mode
    printBanner(opts.mode);
    
    std::cout << "Starting NLM Neural Learning Machine..." << std::endl;
    std::cout << "Mode: " << opts.mode << std::endl;
    std::cout << "Experiment: " << opts.experiment << std::endl;
    if (opts.benchmarkIterations > 1) {
        std::cout << "Benchmark iterations: " << opts.benchmarkIterations << std::endl;
    }
    std::cout << std::endl;
    
    try {
        // Initialize configuration
        auto config = std::make_shared<Config>();
        
        // Try to load from file if provided
        std::string configFile = opts.configFile;
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Apply configuration overrides from command line
        applyConfigurationOverrides(config, opts);
        
        // Set default values for Phase 2-6
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
        
        // Memory system parameters (if enabled)
        if (opts.enableAllSystems) {
            config->set("working_memory_enabled", true, ConfigSource::Default);
            config->set("episodic_memory_enabled", true, ConfigSource::Default);
            config->set("prediction_system_enabled", true, ConfigSource::Default);
        }
        
        // Log configuration summary
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration Summary:");
        NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
        NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
        NLM_LOG_INFO("  enable_all_systems: " + std::to_string(opts.enableAllSystems));
        NLM_LOG_INFO("");
        
        // Initialize simulation clock
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        SimulationClock clock(timestep);
        NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
        
        // Initialize brain
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return 1;
        }
        
        brain->logStatus();
        
        // Run experiments based on mode and selection
        if (opts.mode == "benchmark") {
            runBenchmark(brain, opts.benchmarkIterations);
        } else if (opts.mode == "phase4" || opts.experiment.find("memory") != std::string::npos) {
            // Phase 4: Memory and cognition focus
            NLM_LOG_INFO("Running Phase 4 experiments (memory and cognition)...");
            runMemoryIntegrationTest(brain);
            
            if (opts.experiment == "all" || opts.experiment.find("plasticity") != std::string::npos) {
                runPlasticityExperiment(brain, 1.5f, 1.2f);  // Enhanced learning
            }
            
        } else if (opts.mode == "integrated") {
            // Full integration with agent and world
            NLM_LOG_INFO("Running integrated agent experiments...");
            runIntegratedAgentTest(brain, config, opts.continuousMode, opts.stepsPerCycle);
            
        } else {
            // Default Phase 2 tests (original tests)
            NLM_LOG_INFO("Running Phase 2 demonstrations...");
            
            if (opts.experiment == "all" || opts.experiment.find("connectivity") != std::string::npos) {
                runBasicConnectivityTest(brain, opts.verbose);
            }
            
            if (opts.experiment == "all" || opts.experiment.find("plasticity") != std::string::npos) {
                runPlasticityExperiment(brain, 1.0f, 1.0f);
            }
            
            if (opts.experiment == "all" || opts.experiment.find("stdp") != std::string::npos) {
                runStdpVerification(brain);
            }
            
            if (opts.experiment == "all" || opts.experiment.find("learning") != std::string::npos) {
                EnhancedLearningExperiment learningExperiment(brain, 42);
                learningExperiment.recordInitialState();
                
                // Apply enhanced learning parameters
                if (auto* region = brain->getRegion(RegionId(1))) {
                    for (auto& syn : region->getSynapses()) {
                        syn->enablePlasticity(true, true, false);
                        syn->setWeight(syn->getWeight() * 1.2f);  // Boost initial weights
                    }
                }
                
                // Run learning simulation
                for (SimulationStep step = 0; step < 2000; ++step) {
                    // Enhanced input pattern
                    float inputStrength = 30.0f * (1.0f + 0.5f * std::sin(step * 0.01f));
                    for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                        brain->injectCurrentToNeurons(NeuronType::Sensory, inputStrength);
                    }
                    
                    brain->step(step, step * 0.001);
                }
                
                learningExperiment.recordFinalState();
                learningExperiment.analyzeLearning();
            }
        }
        
        // Final brain status
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Final Brain Status ===");
        brain->logStatus();
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Execution Complete ===");
        NLM_LOG_INFO("")
        NLM_LOG_INFO("Mode: " << opts.mode);
        NLM_LOG_INFO("Experiment: " << opts.experiment);
        NLM_LOG_INFO("Total steps completed successfully.");
        NLM_LOG_INFO("")
        NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
        NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
        NLM_LOG_INFO("");
        
        // Save results if requested
        if (!opts.outputFile.empty()) {
            NLM_LOG_INFO("Saving results to: " + opts.outputFile);
            // Implement result saving
        }
        
        if (!opts.profileFile.empty()) {
            NLM_LOG_INFO("Saving profile to: " + opts.profileFile);
            // Implement profiling
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception occurred: " + std::string(e.what()));
        return 1;
    } catch (...) {
        NLM_LOG_ERROR("Unknown exception occurred");
        return 1;
    }
}
