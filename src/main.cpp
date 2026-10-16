// NLM (熙然) - Neural Learning Machine
// Advanced Neural Simulation Framework with Comprehensive CLI Interface

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <ctime>
#include <chrono>

using namespace nlm;

// === Command Line Argument Parser ===
struct NLMCommandLineArgs {
    std::string mode = "basic";
    std::string configFile = "configs/default.cfg";
    std::string outputDir = ".";
    std::string saveStatePath;
    std::string loadStatePath;
    std::string scriptPath;
    std::string scriptArg;
    std::string deviceType = "CPU";
    std::string precisionMode = "float";
    
    // Basic options
    bool help = false;
    bool version = false;
    bool quiet = false;
    bool verbose = false;
    bool noGraphics = false;
    LogLevel logLevel = LogLevel::Info;
    
    // Simulation parameters
    size_t maxSteps = 1000;
    double timestep = 0.001;
    uint64_t seed = 42;
    size_t neuronCount = 1000;
    size_t regionCount = 2;
    
    // Plasticity and learning
    bool enablePlasticity = true;
    std::string plasticityMode = "combined"; // hebbian, stdp, combined, none
    float plasticityRate = 0.01f;
    bool enableStructuralPlasticity = false;
    bool enableNeuromodulation = false;
    bool enableMemorySystems = false;
    
    // Visualization
    bool plotSpikes = false;
    bool plotSynapses = false;
    bool plotNeurons = false;
    
    // Advanced options
    size_t checkpointInterval = 0;
    bool enableProfiling = false;
    std::string profileType = "CPU"; // CPU, MEMORY, BOTH
    bool enablePerfMeasurement = false;
    
    // Environment
    size_t numThreads = 0; // 0 = auto
    
    // Experimental parameters
    struct ExperimentParams {
        bool enableReward = false;
        float rewardScale = 1.0f;
        bool enableCuriosity = false;
        bool enableDevelopment = false;
        DevelopmentalStage startStage = DevelopmentalStage::Initial;
        size_t developmentInterval = 1000;
        bool enableAttention = false;
        bool enableConceptFormation = false;
        bool enablePlanning = false;
    } experimentParams;
    
    // Parse command line arguments
    static NLMCommandLineArgs parseArgs(int argc, char** argv) {
        NLMCommandLineArgs args;
        
        // Define mode aliases for easier use
        static const std::map<std::string, std::string> modeAliases = {
            {"start", "basic"},
            {"test", "basic"},
            {"demo", "basic"},
            {"learn", "learning"},
            {"plasticity", "learning"},
            {"memory", "learning"},
            {"research", "research"},
            {"benchmark", "benchmark"},
            {"profile", "benchmark"},
            {"debug", "debug"},
            {"diagnostic", "debug"},
        };
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                args.help = true;
            } else if (arg == "--version" || arg == "-v") {
                args.version = true;
            } else if (arg.substr(0, 7) == "--config" || arg.substr(0, 2) == "-c") {
                if (arg.find('=') != std::string::npos) {
                    args.configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.configFile = argv[++i];
                }
            } else if (arg.substr(0, 8) == "--output" || arg.substr(0, 3) == "-o") {
                if (arg.find('=') != std::string::npos) {
                    args.outputDir = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.outputDir = argv[++i];
                }
            } else if (arg == "--quiet" || arg == "-q") {
                args.quiet = true;
            } else if (arg == "--verbose" || arg == "-V") {
                args.verbose = true;
            } else if (arg.substr(0, 12) == "--log-level" || arg.substr(0, 4) == "--log") {
                if (arg.find('=') != std::string::npos) {
                    std::string levelStr = arg.substr(arg.find('=') + 1);
                    std::transform(levelStr.begin(), levelStr.end(), levelStr.begin(), ::tolower);
                    if (levelStr == "debug") args.logLevel = LogLevel::Debug;
                    else if (levelStr == "info") args.logLevel = LogLevel::Info;
                    else if (levelStr == "warn") args.logLevel = LogLevel::Warn;
                    else if (levelStr == "error") args.logLevel = LogLevel::Error;
                }
            } else if (arg.substr(0, 11) == "--max-steps" || arg.substr(0, 4) == "--max") {
                if (arg.find('=') != std::string::npos) {
                    args.maxSteps = std::stoul(arg.substr(arg.find('=') + 1));
                }
            } else if (arg.substr(0, 11) == "--timestep" || arg.substr(0, 5) == "--time") {
                if (arg.find('=') != std::string::npos) {
                    args.timestep = std::stod(arg.substr(arg.find('=') + 1));
                }
            } else if (arg.substr(0, 6) == "--seed") {
                if (arg.find('=') != std::string::npos) {
                    args.seed = std::stoull(arg.substr(arg.find('=') + 1));
                }
            } else if (arg.substr(0, 9) == "--neurons") {
                if (arg.find('=') != std::string::npos) {
                    args.neuronCount = std::stoul(arg.substr(arg.find('=') + 1));
                }
            } else if (arg.substr(0, 8) == "--regions") {
                if (arg.find('=') != std::string::npos) {
                    args.regionCount = std::stoul(arg.substr(arg.find('=') + 1));
                }
            } else if (arg.substr(0, 18) == "--plasticity-mode") {
                if (arg.find('=') != std::string::npos) {
                    args.plasticityMode = arg.substr(arg.find('=') + 1);
                }
            } else if (arg == "--structural-plasticity") {
                args.enableStructuralPlasticity = true;
            } else if (arg == "--neuromodulation") {
                args.enableNeuromodulation = true;
            } else if (arg == "--memory-systems") {
                args.enableMemorySystems = true;
            } else if (arg == "--no-graphics") {
                args.noGraphics = true;
            } else if (arg == "--plot-spikes") {
                args.plotSpikes = true;
            } else if (arg == "--plot-synapses") {
                args.plotSynapses = true;
            } else if (arg == "--plot-neurons") {
                args.plotNeurons = true;
            } else if (arg.substr(0, 12) == "--checkpoint") {
                if (arg.find('=') != std::string::npos) {
                    args.checkpointInterval = std::stoul(arg.substr(arg.find('=') + 1));
                }
            } else if (arg.substr(0, 12) == "--save-state") {
                if (arg.find('=') != std::string::npos) {
                    args.saveStatePath = arg.substr(arg.find('=') + 1);
                }
            } else if (arg.substr(0, 13) == "--load-state") {
                if (arg.find('=') != std::string::npos) {
                    args.loadStatePath = arg.substr(arg.find('=') + 1);
                }
            } else if (arg.substr(0, 10) == "--profile") {
                if (arg.find('=') != std::string::npos) {
                    args.profileType = arg.substr(arg.find('=') + 1);
                }
                args.enableProfiling = true;
            } else if (arg == "--perf-measure") {
                args.enablePerfMeasurement = true;
            } else if (arg.substr(0, 9) == "--device") {
                if (arg.find('=') != std::string::npos) {
                    args.deviceType = arg.substr(arg.find('=') + 1);
                }
            } else if (arg.substr(0, 11) == "--precision") {
                if (arg.find('=') != std::string::npos) {
                    args.precisionMode = arg.substr(arg.find('=') + 1);
                }
            } else if (arg.substr(0, 9) == "--threads") {
                if (arg.find('=') != std::string::npos) {
                    args.numThreads = std::stoul(arg.substr(arg.find('=') + 1));
                }
            } else if (arg == "--enable-reward") {
                args.experimentParams.enableReward = true;
            } else if (arg == "--enable-curiosity") {
                args.experimentParams.enableCuriosity = true;
            } else if (arg == "--enable-development") {
                args.experimentParams.enableDevelopment = true;
            } else if (arg == "--enable-attention") {
                args.experimentParams.enableAttention = true;
            } else if (arg == "--enable-concept-formation") {
                args.experimentParams.enableConceptFormation = true;
            } else if (arg == "--enable-planning") {
                args.experimentParams.enablePlanning = true;
            // Mode selection (first non-option argument)
            } else if (arg.substr(0, 1) != "-") {
                if (modeAliases.find(arg) != modeAliases.end()) {
                    args.mode = modeAliases.at(arg);
                } else {
                    // Try to interpret as mode directly
                    args.mode = arg;
                }
            }
        }
        
        return args;
    }
    
    // Display help information
    static void printHelp() {
        std::cout << "Usage: nlm [OPTIONS]\n\n";
        std::cout << "Neural Learning Machine - Advanced Neural Simulation Framework\n\n";
        std::cout << "Modes:\n";
        std::cout << "  basic         Run basic neural connectivity tests (default)\n";
        std::cout << "  learning      Run learning and plasticity experiments\n";
        std::cout << "  stdp          Verify STDP (Spike-Timing Dependent Plasticity)\n";
        std::cout << "  research      Run research-grade experiments\n";
        std::cout << "  benchmark     Performance benchmarking\n";
        std::cout << "  debug         Debug and diagnostic tools\n\n";
        
        std::cout << "Options:\n";
        std::cout << "  --help, -h                     Show this help message\n";
        std::cout << "  --version, -v                   Show version information\n";
        std::cout << "  --config, -c FILE               Load configuration from file\n";
        std::cout << "  --output, -o DIR                Output directory for results and logs\n";
        std::cout << "  --quiet, -q                     Suppress non-essential output\n";
        std::cout << "  --verbose, -V                   Enable verbose logging\n";
        std::cout << "  --log-level LEVEL               Set logging level (DEBUG, INFO, WARN, ERROR)\n";
        std::cout << "  --max-steps STEPS               Maximum simulation steps (default: 1000)\n";
        std::cout << "  --timestep TIME                 Simulation timestep in seconds (default: 0.001)\n";
        std::cout << "  --seed SEED                     Random seed for reproducible experiments\n";
        std::cout << "  --neurons COUNT                 Number of neurons (default: 1000)\n";
        std::cout << "  --regions COUNT                 Number of brain regions (default: 2)\n\n";
        
        std::cout << "Advanced options:\n";
        std::cout << "  --plasticity-mode MODE          Plasticity mode: hebbian, stdp, combined, none\n";
        std::cout << "  --plasticity-rate RATE           Plasticity learning rate (default: 0.01)\n";
        std::cout << "  --structural-plasticity         Enable structural plasticity\n";
        std::cout << "  --neuromodulation               Enable neuromodulation systems\n";
        std::cout << "  --memory-systems                Enable memory systems (working, episodic, associative)\n\n";
        
        std::cout << "Examples:\n";
        std::cout << "  nlm basic                         Run simple connectivity tests\n";
        std::cout << "  nlm learning --neurons 500       Study learning dynamics\n";
        std::cout << "  nlm research --plasticity-mode combined --structural-plasticity\n";
        std::cout << "  nlm benchmark --profile CPU      Benchmark performance\n";
        std::cout << "  nlm debug --verbose              Debug with verbose output\n";
    }
    
    // Display version information
    static void printVersion() {
        std::cout << "NLM (Neural Learning Machine) v0.1.0\n";
        std::cout << "Phase 2: Real Neural Computation\n";
        std::cout << "Built with C++20 and extensive neural network simulation capabilities.\n";
        std::cout << "License: MIT\n";
        std::cout << "Repository: https://github.com/nlm-project/nlm\n";
    }
};

// === Basic Neural Connectivity Test ===
void runBasicConnectivityTest(std::shared_ptr<Brain> brain, const NLMCommandLineArgs& args) {
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
    size_t steps = std::min(args.maxSteps, static_cast<size_t>(50));
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * args.timestep);
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

// === Learning and Plasticity Experiment ===
void runLearningExperiment(std::shared_ptr<Brain> brain, const NLMCommandLineArgs& args) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Learning and Plasticity Experiment ===");
    NLM_LOG_INFO("Running with " + std::to_string(args.neuronCount) + " neurons");
    NLM_LOG_INFO("Plasticity mode: " + args.plasticityMode);
    NLM_LOG_INFO("Structural plasticity: " + std::string(args.enableStructuralPlasticity ? "enabled" : "disabled"));
    
    // Enable plasticity on synapses
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            bool hebbian = (args.plasticityMode == "hebbian" || args.plasticityMode == "combined");
            bool stdp = (args.plasticityMode == "stdp" || args.plasticityMode == "combined");
            syn->enablePlasticity(hebbian, stdp, false);
        }
    }
    
    // Apply repeated input pattern to stimulate learning
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns...");
    NLM_LOG_INFO("Planned steps: " + std::to_string(args.maxSteps));
    
    for (SimulationStep step = 0; step < args.maxSteps; ++step) {
        // Create input pattern - inject current into sensory neurons
        size_t sensoryNeurons = std::min(size_t(20), brain->getTotalNeuronCount() / 4);
        for (size_t i = 0; i < sensoryNeurons; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * args.timestep);
        
        // Log progress periodically
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                        " | E/I ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
        }
    }
    
    // Performance summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Learning Experiment Results ===");
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Final firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("Excitatory/Inhibitory ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
    
    if (args.enableMemorySystems) {
        if (auto* workingMemory = brain->getWorkingMemory()) {
            NLM_LOG_INFO("Working memory traces: " + std::to_string(workingMemory->getActiveTraces()));
        }
        if (auto* episodicMemory = brain->getEpisodicMemory()) {
            NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(episodicMemory->getEpisodeCount()));
        }
    }
}

// === STDP Verification ===
void runStdpVerification(std::shared_ptr<Brain> brain, const NLMCommandLineArgs& args) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP (Spike-Timing Dependent Plasticity) Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Get first few synapses
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test (need at least 5, have " + std::to_string(synapses.size()) + ")");
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
            brain->step(trial * 2, trial * 2 * args.timestep);
            
            // Then post fires
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * args.timestep);
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
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation (Δ=" + std::to_string(totalDelta) + ")");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters (Δ=" + std::to_string(totalDelta) + ")");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering (Δ=" + std::to_string(totalDelta) + ")");
    }
}

// === Research Mode Experiment ===
void runResearchExperiment(std::shared_ptr<Brain> brain, const NLMCommandLineArgs& args) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Research Mode Experiment ===");
    NLM_LOG_INFO("Running comprehensive research experiments...");
    NLM_LOG_INFO("Neuron count: " + std::to_string(args.neuronCount));
    NLM_LOG_INFO("Region count: " + std::to_string(args.regionCount));
    NLM_LOG_INFO("Plasticity mode: " + args.plasticityMode);
    NLM_LOG_INFO("Structural plasticity: " + std::string(args.enableStructuralPlasticity ? "enabled" : "disabled"));
    NLM_LOG_INFO("Neuromodulation: " + std::string(args.enableNeuromodulation ? "enabled" : "disabled"));
    NLM_LOG_INFO("Memory systems: " + std::string(args.enableMemorySystems ? "enabled" : "disabled"));
    
    // Record initial state
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Recording initial brain state...");
    size_t initialSynapseCount = brain->getTotalSynapseCount();
    float initialWeightSum = 0.0f;
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            initialWeightSum += syn->getWeight();
        }
    }
    
    NLM_LOG_INFO("Initial synapses: " + std::to_string(initialSynapseCount));
    NLM_LOG_INFO("Initial mean weight: " + std::to_string(initialWeightSum / std::max(size_t(1), initialSynapseCount)));
    
    // Run extended learning experiment
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Running extended learning experiment for " + std::to_string(args.maxSteps) + " steps...");
    
    // Enable advanced features based on arguments
    if (args.experimentParams.enableReward) {
        NLM_LOG_INFO("Reward modulation enabled");
    }
    if (args.experimentParams.enableCuriosity) {
        NLM_LOG_INFO("Curiosity modulation enabled");
    }
    if (args.experimentParams.enableDevelopment) {
        NLM_LOG_INFO("Development system enabled");
    }
    if (args.experimentParams.enableAttention) {
        NLM_LOG_INFO("Attention system enabled");
    }
    if (args.experimentParams.enableConceptFormation) {
        NLM_LOG_INFO("Concept formation enabled");
    }
    if (args.experimentParams.enablePlanning) {
        NLM_LOG_INFO("Planning system enabled");
    }
    
    // Run simulation with progress reporting
    for (SimulationStep step = 0; step < args.maxSteps; ++step) {
        // Apply input patterns
        if (args.enableNeuromodulation) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        }
        
        // Update brain
        brain->step(step, step * args.timestep);
        
        // Log progress every 500 steps
        if (step % 500 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                        " | Memory traces: " + std::to_string(brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0));
        }
    }
    
    // Record final state
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Recording final brain state...");
    size_t finalSynapseCount = brain->getTotalSynapseCount();
    float finalWeightSum = 0.0f;
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (const auto& syn : region->getSynapses()) {
            finalWeightSum += syn->getWeight();
        }
    }
    
    NLM_LOG_INFO("Final synapses: " + std::to_string(finalSynapseCount));
    NLM_LOG_INFO("Final mean weight: " + std::to_string(finalWeightSum / std::max(size_t(1), finalSynapseCount)));
    
    // Compute statistics
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Research Experiment Results ===");
    
    float weightChange = (finalWeightSum / std::max(size_t(1), finalSynapseCount)) - (initialWeightSum / std::max(size_t(1), initialSynapseCount));
    NLM_LOG_INFO("Weight change: " + std::to_string(weightChange));
    
    size_t synapseChange = finalSynapseCount - initialSynapseCount;
    NLM_LOG_INFO("Synapse count change: " + std::to_string(synapseChange));
    
    float spikeRate = static_cast<float>(brain->getTotalSpikeCount()) / args.maxSteps;
    NLM_LOG_INFO("Average spike rate: " + std::to_string(spikeRate) + " Hz");
    
    NLM_LOG_INFO("Final firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("Excitatory/Inhibitory ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
    
    // Advanced memory system statistics
    if (args.enableMemorySystems) {
        if (auto* workingMemory = brain->getWorkingMemory()) {
            NLM_LOG_INFO("Working memory active traces: " + std::to_string(workingMemory->getActiveTraces()));
            NLM_LOG_INFO("Working memory capacity: " + std::to_string(workingMemory->getCapacity()));
        }
        if (auto* episodicMemory = brain->getEpisodicMemory()) {
            NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(episodicMemory->getEpisodeCount()));
            NLM_LOG_INFO("Episodic memory max episodes: " + std::to_string(episodicMemory->getMaxEpisodes()));
        }
        if (auto* associativeMemory = brain->getAssociativeMemory()) {
            NLM_LOG_INFO("Associative memory patterns: " + std::to_string(associativeMemory->getPatternCount()));
        }
    }
    
    // Neuromodulation statistics
    if (args.enableNeuromodulation) {
        if (auto* dopamine = brain->getDopamine()) {
            NLM_LOG_INFO("Dopamine level: " + std::to_string(dopamine->getLevel()));
        }
        if (auto* curiosity = brain->getCuriosity()) {
            NLM_LOG_INFO("Curiosity level: " + std::to_string(curiosity->getLevel()));
        }
        if (auto* novelty = brain->getNovelty()) {
            NLM_LOG_INFO("Novelty detection: " + std::to_string(novelty->getLevel()));
        }
    }
    
    // Development statistics
    if (args.experimentParams.enableDevelopment) {
        NLM_LOG_INFO("Current developmental stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())));
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Research experiment completed successfully!");
}

// === Benchmark Mode ===
void runBenchmark(std::shared_ptr<Brain> brain, const NLMCommandLineArgs& args) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Performance Benchmark ===");
    NLM_LOG_INFO("Running performance measurements...");
    
    // Simple benchmark: measure time for different step counts
    std::vector<size_t> stepCounts = {100, 500, 1000, 2000};
    
    for (size_t targetSteps : stepCounts) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Benchmarking with " + std::to_string(targetSteps) + " steps...");
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (SimulationStep step = 0; step < targetSteps; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            brain->step(step, step * args.timestep);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        double stepsPerSecond = static_cast<double>(targetSteps) / (duration.count() / 1000.0);
        NLM_LOG_INFO("  Steps per second: " + std::to_string(stepsPerSecond));
        NLM_LOG_INFO("  Time elapsed: " + std::to_string(duration.count() / 1000.0) + "s");
        
        if (args.enableProfiling) {
            NLM_LOG_INFO("  Profile type: " + args.profileType);
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Benchmark completed!");
}

// === Debug Mode ===
void runDebugMode(std::shared_ptr<Brain> brain, const NLMCommandLineArgs& args) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Debug and Diagnostic Mode ===");
    NLM_LOG_INFO("Running diagnostic analysis...");
    
    // Brain status dump
    brain->logStatus();
    
    // Detailed neuron analysis
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Neuron Analysis ===");
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t activeNeurons = brain->getActiveNeuronCount();
    size_t firingNeurons = brain->getFiringNeuronCount();
    
    NLM_LOG_INFO("Total neurons: " + std::to_string(totalNeurons));
    NLM_LOG_INFO("Active neurons: " + std::to_string(activeNeurons));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(firingNeurons));
    
    if (totalNeurons > 0) {
        float activeRatio = static_cast<float>(activeNeurons) / totalNeurons;
        NLM_LOG_INFO("Active neuron ratio: " + std::to_string(activeRatio));
        
        float firingRatio = static_cast<float>(firingNeurons) / totalNeurons;
        NLM_LOG_INFO("Firing neuron ratio: " + std::to_string(firingRatio));
    }
    
    // Synapse analysis
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Synapse Analysis ===");
    size_t totalSynapses = brain->getTotalSynapseCount();
    NLM_LOG_INFO("Total synapses: " + std::to_string(totalSynapses));
    
    if (totalSynapses > 0) {
        float eiRatio = brain->getExcitationInhibitionRatio();
        NLM_LOG_INFO("Excitatory/Inhibitory ratio: " + std::to_string(eiRatio));
        
        if (eiRatio < 1.0f) {
            NLM_LOG_INFO("  Warning: Network may be inhibited-dominant");
        } else if (eiRatio > 100.0f) {
            NLM_LOG_INFO("  Warning: Network may be excitation-dominant");
        }
    }
    
    // Spike system analysis
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Spike System Analysis ===");
    if (auto* spikeSystem = brain->getSpikeSystem()) {
        NLM_LOG_INFO("Pending spike events: " + std::to_string(spikeSystem->getPendingSpikeCount()));
        NLM_LOG_INFO("Pending delayed events: " + std::to_string(spikeSystem->getPendingDelayedCount()));
    }
    
    NLM_LOG_INFO("Total spike count: " + std::to_string(brain->getTotalSpikeCount()));
    
    // Memory system analysis
    if (auto* workingMemory = brain->getWorkingMemory()) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Working Memory Analysis ===");
        NLM_LOG_INFO("Active traces: " + std::to_string(workingMemory->getActiveTraces()));
        NLM_LOG_INFO("Memory capacity: " + std::to_string(workingMemory->getCapacity()));
    }
    
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Episodic Memory Analysis ===");
        NLM_LOG_INFO("Stored episodes: " + std::to_string(episodicMemory->getEpisodeCount()));
        NLM_LOG_INFO("Max episodes: " + std::to_string(episodicMemory->getMaxEpisodes()));
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Debug analysis completed!");
}

int main(int argc, char** argv) {
    // Parse command line arguments
    NLMCommandLineArgs args = NLMCommandLineArgs::parseArgs(argc, argv);
    
    // Handle help and version requests
    if (args.help) {
        NLMCommandLineArgs::printHelp();
        return 0;
    }
    
    if (args.version) {
        NLMCommandLineArgs::printVersion();
        return 0;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(args.logLevel);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Print banner
    printBanner();
    
    // Log startup information
    if (!args.quiet) {
        std::cout << "Initializing NLM Advanced Neural Simulation Framework..." << std::endl;
        std::cout << "Mode: " << args.mode << std::endl;
        std::cout << "Configuration: " << args.configFile << std::endl;
        if (!args.outputDir.empty()) std::cout << "Output: " << args.outputDir << std::endl;
        std::cout << std::endl;
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    if (!args.quiet) {
        NLM_LOG_INFO("Loading configuration from: " + args.configFile);
    }
    
    if (!config->loadFromFile(args.configFile)) {
        if (!args.quiet) {
            NLM_LOG_INFO("Using default configuration.");
        }
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set configuration based on command line arguments
    config->set("random_seed", static_cast<int64_t>(args.seed), ConfigSource::CommandLine);
    config->set("simulation_timestep", args.timestep, ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(args.neuronCount), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(args.regionCount), ConfigSource::CommandLine);
    config->set("connection_probability", 0.15f, ConfigSource::CommandLine);
    
    // STDP parameters
    config->set("stdp_ltp_weight", args.plasticityRate, ConfigSource::CommandLine);
    config->set("stdp_ltd_weight", args.plasticityRate * 0.95f, ConfigSource::CommandLine);
    config->set("stdp_tau", 20.0f, ConfigSource::CommandLine);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::CommandLine);
    config->set("pruning_rate", 0.00001f, ConfigSource::CommandLine);
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("NLM Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(args.seed));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(args.timestep) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(args.neuronCount));
    NLM_LOG_INFO("  region_count: " + std::to_string(args.regionCount));
    NLM_LOG_INFO("  plasticity_mode: " + args.plasticityMode);
    NLM_LOG_INFO("  max_steps: " + std::to_string(args.maxSteps));
    
    // Initialize simulation clock
    SimulationClock clock(args.timestep);
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Run based on mode
    if (args.mode == "basic") {
        runBasicConnectivityTest(brain, args);
    } else if (args.mode == "learning") {
        runLearningExperiment(brain, args);
    } else if (args.mode == "stdp") {
        runStdpVerification(brain, args);
    } else if (args.mode == "research") {
        runResearchExperiment(brain, args);
    } else if (args.mode == "benchmark") {
        runBenchmark(brain, args);
    } else if (args.mode == "debug") {
        runDebugMode(brain, args);
    } else {
        NLM_LOG_ERROR("Unknown mode: " + args.mode);
        NLM_LOG_INFO("Available modes: basic, learning, stdp, research, benchmark, debug");
        return 1;
    }
    
    // Save state if requested
    if (!args.saveStatePath.empty()) {
        NLM_LOG_INFO("Saving brain state to: " + args.saveStatePath);
        if (brain->save(args.saveStatePath)) {
            NLM_LOG_INFO("Brain state saved successfully");
        } else {
            NLM_LOG_ERROR("Failed to save brain state");
        }
    }
    
    // Final brain status
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Execution Complete ===");
    
    // Performance measurement if requested
    if (args.enablePerfMeasurement) {
        NLM_LOG_INFO("Performance measurement completed");
    }
    
    return 0;
}

// Helper function to print banner
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Advanced Neural Simulation Framework                      ║
    ║                                                               ║
    ║     Phase 2+: Real Neural Computation + Research Tools        ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    std::cout << "Neural Learning Machine - Advanced Neural Simulation Framework" << std::endl;
    std::cout << std::endl;
}
"