// NLM (熙然) - Neural Learning Machine
// Phase 6: FINAL INTEGRATION
//
// This phase implements the complete integrated artificial brain with:
// - Working & Episodic Memory Systems
// - Neuromodulation (Dopamine, Curiosity, Novelty)
// - Prediction System
// - Cognition (Planner, Concepts, Attention)
// - Development System
// - Checkpoint & Replay

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
#include "experiments/Phase6IntegratedExperiment.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <map>
#include <algorithm>
#include <chrono>

using namespace nlm;

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

void printPhase6Banner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: FINAL INTEGRATION                                ║
    ║                                                               ║
    ║     Complete artificial brain with all systems integrated:     ║
    ║     - Working & Episodic Memory Systems                        ║
    ║     - Neuromodulation (Dopamine, Curiosity, Novelty)          ║
    ║     - Prediction System                                        ║
    ║     - Cognition (Planner, Concepts, Attention)                ║
    ║     - Development System                                       ║
    ║     - Checkpoint & Replay                                     ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

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
    ║     - Leaky Integrate-and-Fire (LIF) neuron dynamics          ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --config <file>          Load configuration from file" << std::endl;
    std::cout << "  --demo-mode <mode>       Run specific demo (phase2, phase6)" << std::endl;
    std::cout << "  --checkpoint <file>      Save checkpoint" << std::endl;
    std::cout << "  --load <file>            Load checkpoint" << std::endl;
    std::cout << "  --memory-stats           Print memory system statistics" << std::endl;
    std::cout << "  --neuromod-stats          Print neuromodulation statistics" << std::endl;
    std::cout << "  --development-stats      Print development statistics" << std::endl;
    std::cout << "  --prediction-stats       Print prediction system statistics" << std::endl;
    std::cout << "  --cognition-stats        Print cognition statistics" << std::endl;
    std::cout << "  --export-neurons <file>  Export neuron network to file" << std::endl;
    std::cout << "  --export-synapses <file> Export synapse connections to file" << std::endl;
    std::cout << "  --graph-export <file>    Export brain state as graph" << std::endl;
    std::cout << "  --analyze-plasticity     Analyze plasticity effects" << std::endl;
    std::cout << "  --replay-episodes <n>    Replay last n episodes" << std::endl;
    std::cout << "  --save-episodes <n>      Save last n episodes" << std::endl;
    std::cout << "  --verify-integration     Verify all systems are connected" << std::endl;
    std::cout << "  --memory-consolidation    Force memory consolidation" << std::endl;
    std::cout << "  --development-update     Update developmental stage" << std::endl;
    std::cout << "  --reward-modulate <r>    Apply reward modulation" << std::endl;
    std::cout << "  --curiosity-level <l>    Set curiosity level" << std::endl;
    std::cout << "  --novelty-threshold <t>  Set novelty threshold" << std::endl;
    std::cout << "  --planning-depth <d>     Set planning depth" << std::endl;
    std::cout << "  --attention-strength <s> Set attention strength" << std::endl;
    std::cout << "  --steps <n>              Run for n steps" << std::endl;
    std::cout << "  --enable-all             Enable all subsystems" << std::endl;
    std::cout << "  --disable-all            Disable all subsystems" << std::endl;
    std::cout << "  --random-seed <seed>      Set random seed for reproducible experiments" << std::endl;
    std::cout << "  --enable-debug-logging   Enable debug logging" << std::endl;
    std::cout << "  --output <file>          Redirect output to file" << std::endl;
}

void parseCommandLineArgs(int argc, char** argv, std::string& configFile, 
                               std::string& demoMode, std::string& checkpointFile, 
                               std::string& loadFile, std::string& outputFile,
                               std::string& exportNeuronsFile, std::string& exportSynapsesFile,
                               std::string& graphExportFile, size_t& steps,
                               bool& enableDebugLogging, bool& verifyIntegration,
                               bool& memoryStats, bool& neuromodStats, bool& developmentStats,
                               bool& predictionStats, bool& cognitionStats,
                               bool& analyzePlasticity, bool& replayEpisodes,
                               bool& saveEpisodes, bool& memoryConsolidation,
                               bool& developmentUpdate, float& rewardModulate,
                               float& curiosityLevel, float& noveltyThreshold,
                               float& planningDepth, float& attentionStrength,
                               bool& enableAll, bool& disableAll, uint64_t& randomSeed) {
    
    // Initialize defaults
    configFile = "configs/default.cfg";
    demoMode = "none";
    checkpointFile = "";
    loadFile = "";
    outputFile = "";
    exportNeuronsFile = "";
    exportSynapsesFile = "";
    graphExportFile = "";
    steps = 1000;
    enableDebugLogging = false;
    verifyIntegration = false;
    memoryStats = false;
    neuromodStats = false;
    developmentStats = false;
    predictionStats = false;
    cognitionStats = false;
    analyzePlasticity = false;
    replayEpisodes = false;
    saveEpisodes = false;
    memoryConsolidation = false;
    developmentUpdate = false;
    rewardModulate = 0.0f;
    curiosityLevel = 0.0f;
    noveltyThreshold = 0.0f;
    planningDepth = 5.0f;
    attentionStrength = 1.0f;
    enableAll = false;
    disableAll = false;
    randomSeed = 42;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            printUsage(argv[0]);
            exit(0);
        } else if (arg == "--demo-mode") {
            if (i + 1 < argc) {
                demoMode = argv[++i];
            }
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg == "--checkpoint") {
            if (i + 1 < argc) {
                checkpointFile = argv[++i];
            }
        } else if (arg == "--load") {
            if (i + 1 < argc) {
                loadFile = argv[++i];
            }
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            }
        } else if (arg == "--export-neurons") {
            if (i + 1 < argc) {
                exportNeuronsFile = argv[++i];
            }
        } else if (arg == "--export-synapses") {
            if (i + 1 < argc) {
                exportSynapsesFile = argv[++i];
            }
        } else if (arg == "--graph-export") {
            if (i + 1 < argc) {
                graphExportFile = argv[++i];
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                steps = std::stoul(argv[++i]);
            }
        } else if (arg == "--enable-debug-logging") {
            enableDebugLogging = true;
        } else if (arg == "--verify-integration") {
            verifyIntegration = true;
        } else if (arg == "--memory-stats") {
            memoryStats = true;
        } else if (arg == "--neuromod-stats") {
            neuromodStats = true;
        } else if (arg == "--development-stats") {
            developmentStats = true;
        } else if (arg == "--prediction-stats") {
            predictionStats = true;
        } else if (arg == "--cognition-stats") {
            cognitionStats = true;
        } else if (arg == "--analyze-plasticity") {
            analyzePlasticity = true;
        } else if (arg == "--replay-episodes") {
            replayEpisodes = true;
        } else if (arg == "--save-episodes") {
            saveEpisodes = true;
        } else if (arg == "--memory-consolidation") {
            memoryConsolidation = true;
        } else if (arg == "--development-update") {
            developmentUpdate = true;
        } else if (arg == "--reward-modulate") {
            if (i + 1 < argc) {
                rewardModulate = std::stof(argv[++i]);
            }
        } else if (arg == "--curiosity-level") {
            if (i + 1 < argc) {
                curiosityLevel = std::stof(argv[++i]);
            }
        } else if (arg == "--novelty-threshold") {
            if (i + 1 < argc) {
                noveltyThreshold = std::stof(argv[++i]);
            }
        } else if (arg == "--planning-depth") {
            if (i + 1 < argc) {
                planningDepth = std::stof(argv[++i]);
            }
        } else if (arg == "--attention-strength") {
            if (i + 1 < argc) {
                attentionStrength = std::stof(argv[++i]);
            }
        } else if (arg == "--enable-all") {
            enableAll = true;
        } else if (arg == "--disable-all") {
            disableAll = true;
        } else if (arg == "--random-seed") {
            if (i + 1 < argc) {
                randomSeed = std::stoul(argv[++i]);
            }
        } else if (arg.find("--") == 0 && i + 1 < argc && arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else {
                configFile = argv[++i];
            }
        }
    }
}

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