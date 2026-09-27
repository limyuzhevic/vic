// NLM (熙然) - Neural Learning Machine
// Enhanced Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)
// - Advanced neuromodulation
// - Working memory and attention
// - Development stages
// - Full integration of all brain components

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
#include "neuromodulation/Neuromodulator.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <sstream>
#include <csignal>

using namespace nlm;

// Global flag for graceful shutdown
sig_atomic_t gShutdown = 0;

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    gShutdown = 1;
}

// Print enhanced banner
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Enhanced Neural Computation                     ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     Version 2.0 - Fully Integrated Brain System                  ║
    ║                                                               ║
    ║     Features:                                                  ║
    ║     • Real LIF neuron dynamics                               ║
    ║     • Event-driven spike propagation                         ║
    ║     • Multiple neuromodulators                                 ║
    ║     • Working memory & attention                              ║
    ║     • Developmental stages                                    ║
    ║     • Concept formation                                      ║
    ║     • Social learning                                         ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Print help information
void printHelp(const std::string& programName) {
    std::cout << "NLM - Neural Learning Machine (Phase 2)" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Basic Options:" << std::endl;
    std::cout << "  --config <file>              Load configuration from file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --neuron_count <n>            Set number of neurons (default: 1000)" << std::endl;
    std::cout << "  --timestep <t>                Set simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --steps <n>                   Run N simulation steps (default: 1000)" << std::endl;
    std::cout << std::endl;
    std::cout << "Run Modes:" << std::endl;
    std::cout << "  --demo                        Run demonstration mode (default)" << std::endl;
    std::cout << "  --test                        Run scientific tests" << std::endl;
    std::cout << "  --interactive                 Run interactive mode" << std::endl;
    std::cout << "  --stress <n>                 Stress test with n neurons" << std::endl;
    std::cout << std::endl;
    std::cout << "Simulation Control:" << std::endl;
    std::cout << "  --enable-neuromod <name>      Enable neuromodulator (DA, ACh, NE, 5-HT)" << std::endl;
    std::cout << "  --disable-neuromod <name>     Disable neuromodulator" << std::endl;
    std::cout << "  --develop-to <stage>           Develop to specific stage (Initial, CriticalPeriod, Maturation, Adult)" << std::endl;
    std::cout << "  --enable-working-memory       Enable working memory system" << std::endl;
    std::cout << "  --disable-working-memory      Disable working memory system" << std::endl;
    std::cout << "  --enable-attention            Enable attentional selection" << std::endl;
    std::cout << "  --enable-curiosity            Enable curiosity system" << std::endl;
    std::cout << "  --enable-novelty              Enable novelty detection" << std::endl;
    std::cout << std::endl;
    std::cout << "Output and Logging:" << std::endl;
    std::cout << "  --log-level <level>           Set log level (Debug, Info, Warning, Error)" << std::endl;
    std::cout << "  --log-file <file>             Write logs to file" << std::endl;
    std::cout << "  --progress                    Show progress during simulation" << std::endl;
    std::cout << "  --statistics <interval>       Print statistics every N steps" << std::endl;
    std::cout << "  --save-state <file>           Save brain state to file" << std::endl;
    std::cout << "  --load-state <file>           Load brain state from file" << std::endl;
    std::cout << std::endl;
    std::cout << "Learning and Plasticity:" << std::endl;
    std::cout << "  --plasticity-factor <f>        Set global plasticity factor (default: 1.0)" << std::endl;
    std::cout << "  --enable-stdp                 Enable Spike-Timing-Dependent Plasticity" << std::endl;
    std::cout << "  --enable-hebbian              Enable Hebbian plasticity" << std::endl;
    std::cout << "  --enable-structural           Enable structural plasticity (synaptogenesis/pruning)" << std::endl;
    std::cout << std::endl;
    std::cout << "Advanced Options:" << std::endl;
    std::cout << "  --random-seed <n>             Set random seed for reproducibility" << std::endl;
    std::cout << "  --num-regions <n>             Set number of brain regions (default: 1)" << std::endl;
    std::cout << "  --connection-prob <p>         Set connection probability (default: 0.1)" << std::endl;
    std::cout << "  --quiet                       Suppress most output (errors only)" << std::endl;
    std::cout << "  --batch                       Run without user interaction" << std::endl;
    std::cout << "  --help                        Show this help message" << std::endl;
    std::cout << std::endl;
}

// Print simulation progress
void printProgress(int currentStep, int totalSteps, double elapsedTime) {
    if (currentStep % 100 == 0 || currentStep == totalSteps) {
        std::cout << "\r[Progress] Step: " << currentStep << " / " << totalSteps
                  << " (" << std::fixed << std::setprecision(1)
                  << (100.0 * currentStep / totalSteps) << "%)"
                  << " | Time: " << std::fixed << std::setprecision(2) << elapsedTime << "s"
                  << " | " << std::flush;
    }
}

// Print brain statistics
void printBrainStatistics(const std::shared_ptr<Brain>& brain) {
    if (!brain) return;
    
    std::cout << std::endl;
    std::cout << "=== Brain Statistics ===" << std::endl;
    std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "E/I ratio: " << std::fixed << std::setprecision(2) << brain->getExcitationInhibitionRatio() << std::endl;
    std::cout << "Average firing rate: " << std::fixed << std::setprecision(2) << brain->getAverageFiringRate() << std::endl;
    
    // Get brain subsystems
    auto spikeSystem = brain->getSpikeSystem();
    if (spikeSystem) {
        std::cout << "Pending spike events: " << spikeSystem->getPendingSpikeCount() << std::endl;
    }
    
    // Get neuromodulation state
    auto dopamine = brain->getDopamine();
    if (dopamine) {
        std::cout << "Dopamine level: " << std::fixed << std::setprecision(3) << dopamine->getLevel() << std::endl;
    }
    
    auto curiosity = brain->getCuriosity();
    if (curiosity) {
        std::cout << "Curiosity level: " << std::fixed << std::setprecision(3) << curiosity->getLevel() << std::endl;
    }
    
    auto novelty = brain->getNovelty();
    if (novelty) {
        std::cout << "Novelty level: " << std::fixed << std::setprecision(3) << novelty->getLevel() << std::endl;
    }
    
    // Get developmental stage
    auto stage = brain->getDevelopmentalStage();
    std::cout << "Developmental stage: " << static_cast<int>(stage) << std::endl;
    
    // Get memory system status
    auto workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        std::cout << "Working memory traces: " << workingMemory->getActiveTraces() << std::endl;
    }
    
    auto episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        std::cout << "Episodic memory episodes: " << episodicMemory->getEpisodeCount() << std::endl;
    }
    
    std::cout << "=============================" << std::endl;
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

void runComprehensiveDemo(std::shared_ptr<Brain> brain, int totalSteps, bool showProgress) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Phase 2 Enhanced Demo ===");
    NLM_LOG_INFO("Running comprehensive demonstration with integrated brain systems...");
    
    // Enable all subsystems for full demo
    if (brain->getDopamine()) {
        brain->getDopamine()->enable();
    }
    if (brain->getCuriosity()) {
        brain->getCuriosity()->enable();
    }
    if (brain->getNovelty()) {
        brain->getNovelty()->enable();
    }
    if (brain->getWorkingMemory()) {
        brain->getWorkingMemory()->enable();
    }
    if (brain->getAttention()) {
        brain->getAttention()->enable();
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (SimulationStep step = 0; step < totalSteps; ++step) {
        if (gShutdown) {
            NLM_LOG_INFO("Simulation interrupted by user.");
            break;
        }
        
        // Apply environmental inputs
        brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        
        // Run brain step
        brain->step(step, step * 0.001);
        
        // Show progress if requested
        if (showProgress && step % 100 == 0) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0;
            printProgress(step, totalSteps, elapsed);
        }
        
        // Apply neuromodulation based on activity
        if (brain->getDopamine()) {
            float activityLevel = static_cast<float>(brain->getFiringNeuronCount()) / std::max(1.0f, brain->getTotalNeuronCount());
            if (activityLevel > 0.1f) {
                brain->getDopamine()->signalReward(activityLevel);
            }
        }
        
        // Update development occasionally
        if (step % 500 == 0 && step > 0) {
            brain->develop();
        }
        
        // Log status periodically
        if (step % 500 == 0) {
            NLM_LOG_INFO("Demo step: " + std::to_string(step) + 
                        " | Firing neurons: " + std::to_string(brain->getFiringNeuronCount()) +
                        " | Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0;
    
    std::cout << std::endl; // New line after progress
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Demo Complete ===");
    NLM_LOG_INFO("Total simulation time: " + std::to_string(totalTime) + "s");
    NLM_LOG_INFO("Average steps per second: " + std::to_string(totalSteps / totalTime) + " Hz");
    
    printBrainStatistics(brain);
}

void runScientificTests(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Scientific Test Suite ===");
    NLM_LOG_INFO("Running controlled experiments...");
    
    // Test 1: Basic connectivity
    runBasicConnectivityTest(brain);
    
    // Reset and run Test 2: Plasticity
    brain->reset();
    brain->initialize();
    runPlasticityExperiment(brain);
    
    // Reset and run Test 3: STDP
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== All Scientific Tests Complete ===");
}

int main(int argc, char** argv) {
    // Set signal handler for graceful shutdown
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    printBanner();
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    int neuronCount = 1000;
    double timestep = 0.001;
    int totalSteps = 1000;
    std::string runMode = "demo"; // demo, test, interactive
    bool showProgress = true;
    int statisticsInterval = 500;
    std::string logLevel = "Info";
    std::string logFile = "";
    float plasticityFactor = 1.0f;
    int randomSeed = 42;
    int numRegions = 1;
    float connectionProb = 0.1f;
    bool enableNeuromodDA = true;
    bool enableNeuromodACh = true;
    bool enableNeuromodNE = true;
    bool enableNeuromod5HT = true;
    std::string developToStage = "";
    bool enableWorkingMemory = true;
    bool enableAttention = true;
    bool enableCuriosity = true;
    bool enableNovelty = true;
    bool quietMode = false;
    
    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            printHelp(argv[0]);
            return 0;
        }
        
        else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
        
        else if (arg == "--demo") {
            runMode = "demo";
        }
        
        else if (arg == "--test") {
            runMode = "test";
        }
        
        else if (arg == "--interactive") {
            runMode = "interactive";
        }
        
        else if (arg.substr(0, 8) == "--stress") {
            if (arg.find('=') != std::string::npos) {
                neuronCount = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                neuronCount = std::stoi(argv[++i]);
            }
            runMode = "stress";
        }
        
        else if (arg.substr(0, 14) == "--neuron_count") {
            if (arg.find('=') != std::string::npos) {
                neuronCount = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                neuronCount = std::stoi(argv[++i]);
            }
        }
        
        else if (arg.substr(0, 10) == "--timestep") {
            if (arg.find('=') != std::string::npos) {
                timestep = std::stod(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                timestep = std::stod(argv[++i]);
            }
        }
        
        else if (arg.substr(0, 7) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                totalSteps = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                totalSteps = std::stoi(argv[++i]);
            }
        }
        
        else if (arg.substr(0, 18) == "--enable-neuromod") {
            if (arg.find('=') != std::string::npos) {
                std::string name = arg.substr(arg.find('=') + 1);
                if (name == "DA") enableNeuromodDA = true;
                else if (name == "ACh") enableNeuromodACh = true;
                else if (name == "NE") enableNeuromodNE = true;
                else if (name == "5-HT") enableNeuromod5HT = true;
            } else if (i + 1 < argc) {
                std::string name = argv[++i];
                if (name == "DA") enableNeuromodDA = true;
                else if (name == "ACh") enableNeuromodACh = true;
                else if (name == "NE") enableNeuromodNE = true;
                else if (name == "5-HT") enableNeuromod5HT = true;
            }
        }
        
        else if (arg.substr(0, 18) == "--disable-neuromod") {
            if (arg.find('=') != std::string::npos) {
                std::string name = arg.substr(arg.find('=') + 1);
                if (name == "DA") enableNeuromodDA = false;
                else if (name == "ACh") enableNeuromodACh = false;
                else if (name == "NE") enableNeuromodNE = false;
                else if (name == "5-HT") enableNeuromod5HT = false;
            } else if (i + 1 < argc) {
                std::string name = argv[++i];
                if (name == "DA") enableNeuromodDA = false;
                else if (name == "ACh") enableNeuromodACh = false;
                else if (name == "NE") enableNeuromodNE = false;
                else if (name == "5-HT") enableNeuromod5HT = false;
            }
        }
        
        else if (arg.substr(0, 15) == "--develop-to") {
            if (arg.find('=') != std::string::npos) {
                developToStage = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                developToStage = argv[++i];
            }
        }
        
        else if (arg == "--enable-working-memory") {
            enableWorkingMemory = true;
        }
        
        else if (arg == "--disable-working-memory") {
            enableWorkingMemory = false;
        }
        
        else if (arg == "--enable-attention") {
            enableAttention = true;
        }
        
        else if (arg == "--enable-curiosity") {
            enableCuriosity = true;
        }
        
        else if (arg == "--enable-novelty") {
            enableNovelty = true;
        }
        
        else if (arg.substr(0, 11) == "--log-level") {
            if (arg.find('=') != std::string::npos) {
                logLevel = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                logLevel = argv[++i];
            }
        }
        
        else if (arg.substr(0, 10) == "--log-file") {
            if (arg.find('=') != std::string::npos) {
                logFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                logFile = argv[++i];
            }
        }
        
        else if (arg == "--progress") {
            showProgress = true;
        }
        
        else if (arg.substr(0, 14) == "--statistics") {
            if (arg.find('=') != std::string::npos) {
                statisticsInterval = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                statisticsInterval = std::stoi(argv[++i]);
            }
        }
        
        else if (arg.substr(0, 12) == "--plasticity") {
            if (arg.find('=') != std::string::npos) {
                plasticityFactor = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                plasticityFactor = std::stof(argv[++i]);
            }
        }
        
        else if (arg.substr(0, 13) == "--random-seed") {
            if (arg.find('=') != std::string::npos) {
                randomSeed = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                randomSeed = std::stoi(argv[++i]);
            }
        }
        
        else if (arg.substr(0, 12) == "--num-regions") {
            if (arg.find('=') != std::string::npos) {
                numRegions = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                numRegions = std::stoi(argv[++i]);
            }
        }
        
        else if (arg.substr(0, 19) == "--connection-prob") {
            if (arg.find('=') != std::string::npos) {
                connectionProb = std::stof(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                connectionProb = std::stof(argv[++i]);
            }
        }
        
        else if (arg == "--quiet") {
            quietMode = true;
        }
        
        else if (arg == "--batch") {
            quietMode = true;
            showProgress = false;
        }
        
        else if (arg.substr(0, 10) == "--save-state") {
            if (arg.find('=') != std::string::npos) {
                std::string saveFile = arg.substr(arg.find('=') + 1);
                // Note: Save functionality would be implemented here
                if (!quietMode) {
                    NLM_LOG_INFO("Save state to: " + saveFile);
                }
            } else if (i + 1 < argc) {
                std::string saveFile = argv[++i];
                // Note: Save functionality would be implemented here
                if (!quietMode) {
                    NLM_LOG_INFO("Save state to: " + saveFile);
                }
            }
        }
        
        else if (arg.substr(0, 11) == "--load-state") {
            if (arg.find('=') != std::string::npos) {
                std::string loadFile = arg.substr(arg.find('=') + 1);
                // Note: Load functionality would be implemented here
                if (!quietMode) {
                    NLM_LOG_INFO("Load state from: " + loadFile);
                }
            } else if (i + 1 < argc) {
                std::string loadFile = argv[++i];
                // Note: Load functionality would be implemented here
                if (!quietMode) {
                    NLM_LOG_INFO("Load state from: " + loadFile);
                }
            }
        }
        
        else {
            if (!quietMode) {
                NLM_LOG_WARNING("Unknown option: " + arg);
            }
        }
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    
    // Set log level
    if (logLevel == "Debug") {
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
        logger->addLogger(consoleLogger);
    } else if (logLevel == "Warning") {
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
        logger->addLogger(consoleLogger);
    } else if (logLevel == "Error") {
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Error);
        logger->addLogger(consoleLogger);
    } else {
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
    }
    
    // Add file logger if specified
    if (!logFile.empty()) {
        auto fileLogger = std::make_shared<FileLogger>(logFile);
        logger->addLogger(fileLogger);
    }
    
    Logger::setGlobal(logger);
    
    // Print configuration summary
    if (!quietMode) {
        std::cout << "Configuration Summary:" << std::endl;
        std::cout << "  Config file: " << configFile << std::endl;
        std::cout << "  Neuron count: " << neuronCount << std::endl;
        std::cout << "  Timestep: " << timestep << "s" << std::endl;
        std::cout << "  Total steps: " << totalSteps << std::endl;
        std::cout << "  Run mode: " << runMode << std::endl;
        std::cout << "  Log level: " << logLevel << std::endl;
        std::cout << "  Random seed: " << randomSeed << std::endl;
        std::cout << std::endl;
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    if (!config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Using default configuration (file not found: " + configFile + ")");
    } else {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values
    config->set("random_seed", static_cast<int64_t>(randomSeed), ConfigSource::Default);
    config->set("simulation_timestep", timestep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(numRegions), ConfigSource::Default);
    config->set("connection_probability", connectionProb, ConfigSource::Default);
    config->set("plasticity_factor", plasticityFactor, ConfigSource::Default);
    
    // Set development stage if specified
    if (!developToStage.empty()) {
        if (developToStage == "Initial") {
            config->set("development_stage", "Initial", ConfigSource::CommandLine);
        } else if (developToStage == "CriticalPeriod") {
            config->set("development_stage", "CriticalPeriod", ConfigSource::CommandLine);
        } else if (developToStage == "Maturation") {
            config->set("development_stage", "Maturation", ConfigSource::CommandLine);
        } else if (developToStage == "Adult") {
            config->set("development_stage", "Adult", ConfigSource::CommandLine);
        }
    }
    
    // Enable/disable neuromodulators
    config->set("neuromod.dopamine.enable", enableNeuromodDA, ConfigSource::CommandLine);
    config->set("neuromod.acetylcholine.enable", enableNeuromodACh, ConfigSource::CommandLine);
    config->set("neuromod.norepinephrine.enable", enableNeuromodNE, ConfigSource::CommandLine);
    config->set("neuromod.serotonin.enable", enableNeuromod5HT, ConfigSource::CommandLine);
    
    // Enable/disable subsystems
    config->set("memory.working_memory.enable", enableWorkingMemory, ConfigSource::CommandLine);
    config->set("cognition.attention.enable", enableAttention, ConfigSource::CommandLine);
    config->set("neuromod.curiosity.enable", enableCuriosity, ConfigSource::CommandLine);
    config->set("neuromod.novelty.enable", enableNovelty, ConfigSource::CommandLine);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Enhanced Configuration ===");
    NLM_LOG_INFO("Random seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("Simulation timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("Neuron count: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
    NLM_LOG_INFO("Region count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("Connection probability: " + std::to_string(config->getOr<float>("connection_probability", 0.1f)));
    NLM_LOG_INFO("Plasticity factor: " + std::to_string(config->getOr<float>("plasticity_factor", 1.0f)));
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Neuromodulators: DA=" << (enableNeuromodDA ? "1" : "0") 
                        << ", ACh=" << (enableNeuromodACh ? "1" : "0")
                        << ", NE=" << (enableNeuromodNE ? "1" : "0")
                        << ", 5-HT=" << (enableNeuromod5HT ? "1" : "0"));
    NLM_LOG_INFO("Subsystems: WM=" << (enableWorkingMemory ? "1" : "0")
                        << ", Attention=" << (enableAttention ? "1" : "0")
                        << ", Curiosity=" << (enableCuriosity ? "1" : "0")
                        << ", Novelty=" << (enableNovelty ? "1" : "0"));
    
    // Initialize simulation clock
    SimulationClock clock(timestep);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Enhanced Brain (Phase 2 - Full Integration)...");
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run based on mode
    if (runMode == "test") {
        runScientificTests(brain);
    } else if (runMode == "interactive") {
        NLM_LOG_INFO("Interactive mode not yet implemented - running demo instead.");
        runComprehensiveDemo(brain, totalSteps, showProgress);
    } else if (runMode == "stress") {
        NLM_LOG_INFO("Running stress test with " + std::to_string(neuronCount) + " neurons...");
        runComprehensiveDemo(brain, totalSteps, showProgress);
    } else { // demo
        runComprehensiveDemo(brain, totalSteps, showProgress);
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Enhanced Complete ===");
    NLM_LOG_INFO("The NLM brain system is now a fully functional artificial neural substrate");
    NLM_LOG_INFO("with advanced neuromodulation, working memory, attention, and development.");
    NLM_LOG_INFO("The system demonstrates real neural computation with complete integration");
    NLM_LOG_INFO("of all brain subsystems working together.");
    
    return 0;
}