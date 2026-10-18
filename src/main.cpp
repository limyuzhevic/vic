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

#include <cstring>
#include <filesystem>

namespace nlm {

// Command line options structure
struct CommandLineOptions {
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    std::string checkpointFile;
    int64_t neuronCount = 500;
    int64_t regionCount = 1;
    double simulationTimestep = 0.001;
    double simulationTime = 0.0;
    int64_t randomSeed = 42;
    float connectionProbability = 0.15f;
    float stdpLTPWeight = 0.02f;
    float stdpLTDWeight = 0.015f;
    float stdpTau = 20.0f;
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    bool enablePlasticity = true;
    bool enableDevelopment = true;
    bool enableCuriosity = true;
    bool enableRewardModulation = true;
    bool enableSTDP = true;
    bool enableHebbian = true;
    std::string mode = "demo";  // demo, benchmark, experiment
    int steps = 100;
    int benchmarkSteps = 1000;
    bool enableLogging = true;
    std::string logLevel = "info";
    bool enableVisualization = false;
    bool showVersion = false;
    bool showHelp = false;
    bool saveCheckpoint = false;
    bool loadCheckpoint = false;
};

// Parse command line arguments
CommandLineOptions parseCommandLine(int argc, char** argv) {
    CommandLineOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.showHelp = true;
        } else if (arg == "--version" || arg == "-v") {
            options.showVersion = true;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                options.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else if (arg == "--mode" && i + 1 < argc) {
            options.mode = argv[++i];
        } else if (arg == "--neuron-count" && i + 1 < argc) {
            options.neuronCount = std::stoll(argv[++i]);
        } else if (arg == "--region-count" && i + 1 < argc) {
            options.regionCount = std::stoll(argv[++i]);
        } else if (arg == "--timestep" && i + 1 < argc) {
            options.simulationTimestep = std::stod(argv[++i]);
        } else if (arg == "--time" && i + 1 < argc) {
            options.simulationTime = std::stod(argv[++i]);
        } else if (arg == "--seed" && i + 1 < argc) {
            options.randomSeed = std::stoll(argv[++i]);
        } else if (arg == "--connection-prob" && i + 1 < argc) {
            options.connectionProbability = std::stof(argv[++i]);
        } else if (arg == "--stdp-ltp" && i + 1 < argc) {
            options.stdpLTPWeight = std::stof(argv[++i]);
        } else if (arg == "--stdp-ltd" && i + 1 < argc) {
            options.stdpLTDWeight = std::stof(argv[++i]);
        } else if (arg == "--stdp-tau" && i + 1 < argc) {
            options.stdpTau = std::stof(argv[++i]);
        } else if (arg == "--synaptogenesis" && i + 1 < argc) {
            options.synaptogenesisRate = std::stof(argv[++i]);
        } else if (arg == "--pruning" && i + 1 < argc) {
            options.pruningRate = std::stof(argv[++i]);
        } else if (arg == "--steps" && i + 1 < argc) {
            options.steps = std::stoi(argv[++i]);
        } else if (arg == "--benchmark-steps" && i + 1 < argc) {
            options.benchmarkSteps = std::stoi(argv[++i]);
        } else if (arg == "--output" && i + 1 < argc) {
            options.outputFile = argv[++i];
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            options.checkpointFile = argv[++i];
            options.saveCheckpoint = true;
        } else if (arg == "--load-checkpoint" && i + 1 < argc) {
            options.checkpointFile = argv[++i];
            options.loadCheckpoint = true;
        } else if (arg == "--enable-plasticity" || arg == "--plasticity") {
            options.enablePlasticity = true;
        } else if (arg == "--disable-plasticity") {
            options.enablePlasticity = false;
        } else if (arg == "--enable-development" || arg == "--development") {
            options.enableDevelopment = true;
        } else if (arg == "--disable-development") {
            options.enableDevelopment = false;
        } else if (arg == "--enable-curiosity" || arg == "--curiosity") {
            options.enableCuriosity = true;
        } else if (arg == "--disable-curiosity") {
            options.enableCuriosity = false;
        } else if (arg == "--enable-reward" || arg == "--reward") {
            options.enableRewardModulation = true;
        } else if (arg == "--disable-reward") {
            options.enableRewardModulation = false;
        } else if (arg == "--enable-stdp") {
            options.enableSTDP = true;
        } else if (arg == "--disable-stdp") {
            options.enableSTDP = false;
        } else if (arg == "--enable-hebbian") {
            options.enableHebbian = true;
        } else if (arg == "--disable-hebbian") {
            options.enableHebbian = false;
        } else if (arg == "--enable-logging" || arg == "--log") {
            options.enableLogging = true;
        } else if (arg == "--disable-logging") {
            options.enableLogging = false;
        } else if (arg == "--log-level" && i + 1 < argc) {
            options.logLevel = argv[++i];
        } else if (arg == "--enable-visualization" || arg == "--viz") {
            options.enableVisualization = true;
        } else if (arg == "--disable-visualization") {
            options.enableVisualization = false;
        }
    }
    
    return options;
}

// Print help information
void printHelp() {
    printBanner();
    
    std::cout << "\nUsage: nlm [options]\n\n";
    std::cout << "Modes:\n";
    std::cout << "  --mode demo        Run demonstration (default)\n";
    std::cout << "  --mode benchmark   Run performance benchmark\n";
    std::cout << "  --mode experiment  Run experimental protocol\n\n";
    
    std::cout << "Configuration options:\n";
    std::cout << "  --config file      Load configuration from file (default: configs/default.cfg)\n";
    std::cout << "  --neuron-count N   Number of neurons (default: 500)\n";
    std::cout << "  --region-count N   Number of brain regions (default: 1)\n";
    std::cout << "  --timestep T       Simulation timestep in seconds (default: 0.001)\n";
    std::cout << "  --time T           Total simulation time in seconds (default: 0.0)\n";
    std::cout << "  --seed S           Random seed (default: 42)\n";
    std::cout << "  --connection-prob P Connection probability (default: 0.15)\n\n";
    
    std::cout << "Plasticity options:\n";
    std::cout << "  --enable-stdp / --disable-stdp   Enable/disable STDP (default: enable)\n";
    std::cout << "  --enable-hebbian / --disable-hebbian Enable/disable Hebbian learning (default: enable)\n";
    std::cout << "  --enable-plasticity / --disable-plasticity Enable/disable all plasticity (default: enable)\n";
    std::cout << "  --stdp-ltp W      STDP LTP weight (default: 0.02)\n";
    std::cout << "  --stdp-ltd W      STDP LTD weight (default: 0.015)\n";
    std::cout << "  --stdp-tau T      STDP time constant (default: 20.0)\n";
    std::cout << "  --synaptogenesis R Synaptogenesis rate (default: 0.0001)\n";
    std::cout << "  --pruning R       Pruning rate (default: 0.00001)\n\n";
    
    std::cout << "Development options:\n";
    std::cout << "  --enable-development / --disable-development Enable/disable development (default: enable)\n";
    std::cout << "  --enable-curiosity / --disable-curiosity   Enable/disable curiosity (default: enable)\n";
    std::cout << "  --enable-reward / --disable-reward        Enable/disable reward modulation (default: enable)\n\n";
    
    std::cout << "Output options:\n";
    std::cout << "  --steps N          Number of simulation steps (default: 100 for demo)\n";
    std::cout << "  --benchmark-steps N Number of steps for benchmark (default: 1000)\n";
    std::cout << "  --output file      Save output to file\n";
    std::cout << "  --checkpoint file  Save brain state to file\n";
    std::cout << "  --load-checkpoint file Load brain state from file\n\n";
    
    std::cout << "Logging options:\n";
    std::cout << "  --enable-logging / --disable-logging Enable/disable logging (default: enable)\n";
    std::cout << "  --log-level L      Set log level (debug, info, warning, error, critical)\n\n";
    
    std::cout << "Visualization options:\n";
    std::cout << "  --enable-visualization / --disable-visualization Enable/disable visualization (default: disable)\n\n";
    
    std::cout << "General options:\n";
    std::cout << "  --help, -h         Show this help message\n";
    std::cout << "  --version, -v      Show version information\n\n";
}

// Print version information
void printVersion() {
    printBanner();
    std::cout << "\nNLM (熙然) - Neural Learning Machine\n";
    std::cout << "Version: Phase 2.3 (Neural Computation)\n";
    std::cout << "Build: C++20 with pybind11 Python bindings\n";
    std::cout << "Features:\n";
    std::cout << "  ✓ Real LIF neuron dynamics\n";
    std::cout << "  ✓ Event-driven spike propagation\n";
    std::cout << "  ✓ STDP and Hebbian plasticity\n";
    std::cout << "  ✓ Structural plasticity\n";
    std::cout << "  ✓ Memory systems (working, episodic, associative)\n";
    std::cout << "  ✓ Neuromodulation (dopamine, curiosity, novelty)\n";
    std::cout << "  ✓ Prediction and planning systems\n";
    std::cout << "  ✓ Development and maturation\n";
    std::cout << "  ✓ Agent-environment interaction\n";
    std::cout << "  ✓ Python API (pynlm)\n";
    std::cout << "\nFor more information: https://github.com/nlm-project/nlm\n";
}

// Run demonstration mode
void runDemo(CommandLineOptions& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== NLM Phase 2 Demo Mode ===");
    NLM_LOG_INFO("Running standard neural computation demonstration...");
    
    // Load configuration with command line overrides
    if (config->loadFromFile(options.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + options.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(1, new char*[1]{const_cast<char*>("--mode")});
    
    // Set options from command line
    config->set("random_seed", options.randomSeed, ConfigSource::CommandLine);
    config->set("simulation_timestep", options.simulationTimestep, ConfigSource::CommandLine);
    config->set("neuron_count", options.neuronCount, ConfigSource::CommandLine);
    config->set("region_count", options.regionCount, ConfigSource::CommandLine);
    config->set("connection_probability", options.connectionProbability, ConfigSource::CommandLine);
    config->set("stdp_ltp_weight", options.stdpLTPWeight, ConfigSource::CommandLine);
    config->set("stdp_ltd_weight", options.stdpLTDWeight, ConfigSource::CommandLine);
    config->set("stdp_tau", options.stdpTau, ConfigSource::CommandLine);
    config->set("synaptogenesis_rate", options.synaptogenesisRate, ConfigSource::CommandLine);
    config->set("pruning_rate", options.pruningRate, ConfigSource::CommandLine);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    brain->logStatus();
    
    // Run three demonstration tests
    runBasicConnectivityTest(brain);
    
    // Reset brain for plasticity experiment
    brain->reset();
    brain->initialize();
    
    runPlasticityExperiment(brain);
    
    // Reset and run STDP verification
    brain->reset();
    brain->initialize();
    runStdpVerification(brain);
    
    // Save checkpoint if requested
    if (options.saveCheckpoint && !options.checkpointFile.empty()) {
        if (brain->save(options.checkpointFile)) {
            NLM_LOG_INFO("Checkpoint saved to: " + options.checkpointFile);
        } else {
            NLM_LOG_ERROR("Failed to save checkpoint to: " + options.checkpointFile);
        }
    }
    
    NLM_LOG_INFO("=== Demo Complete ===");
}

// Run benchmark mode
void runBenchmark(CommandLineOptions& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== NLM Phase 2 Benchmark Mode ===");
    NLM_LOG_INFO("Running performance benchmark...");
    
    // Configure for benchmarking
    config->set("neuron_count", options.neuronCount, ConfigSource::CommandLine);
    config->set("simulation_timestep", options.simulationTimestep, ConfigSource::CommandLine);
    config->set("random_seed", options.randomSeed, ConfigSource::CommandLine);
    
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for benchmark!");
        return;
    }
    
    // Measure performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < options.benchmarkSteps; ++step) {
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        // Inject current periodically to maintain activity
        if (step % 10 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        }
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    NLM_LOG_INFO("=== Benchmark Results ===");
    NLM_LOG_INFO("  Total steps: " + std::to_string(options.benchmarkSteps));
    NLM_LOG_INFO("  Total time: " + std::to_string(duration.count() / 1000.0) + " seconds");
    NLM_LOG_INFO("  Steps per second: " + std::to_string(options.benchmarkSteps / (duration.count() / 1000.0)));
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("  Final brain status:");
    brain->logStatus();
}

// Run experimental mode
void runExperiment(CommandLineOptions& options, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("=== NLM Phase 2 Experiment Mode ===");
    NLM_LOG_INFO("Running experimental protocol...");
    
    // Create brain with experiment configuration
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for experiment!");
        return;
    }
    
    // Run experimental sequence
    for (int step = 0; step < options.steps; ++step) {
        // Create input pattern
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    NLM_LOG_INFO("=== Experiment Complete ===");
    brain->logStatus();
}

int main(int argc, char** argv) {
    // Parse command line options
    CommandLineOptions options = parseCommandLine(argc, argv);
    
    // Check for help/version requests
    if (options.showHelp) {
        printHelp();
        return 0;
    }
    
    if (options.showVersion) {
        printVersion();
        return 0;
    }
    
    // Set up global logger
    auto logger = std::make_shared<Logger>();
    
    if (options.enableLogging) {
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        if (!options.logLevel.empty()) {
            if (options.logLevel == "debug") consoleLogger->setLevel(LogLevel::Debug);
            else if (options.logLevel == "warning") consoleLogger->setLevel(LogLevel::Warning);
            else if (options.logLevel == "error") consoleLogger->setLevel(LogLevel::Error);
            else if (options.logLevel == "critical") consoleLogger->setLevel(LogLevel::Critical);
        }
        logger->addLogger(consoleLogger);
    } else {
        consoleLogger->setLevel(LogLevel::Error);  // Only show errors
    }
    
    Logger::setGlobal(logger);
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    
    // Set default values for Phase 2
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
    
    // Enable/disable subsystems based on options
    config->set("plasticity_stdp_enable", options.enableSTDP, ConfigSource::Runtime);
    config->set("plasticity_hebbian_enable", options.enableHebbian, ConfigSource::Runtime);
    config->set("development_enable", options.enableDevelopment, ConfigSource::Runtime);
    config->set("neuromod_curiosity_enable", options.enableCuriosity, ConfigSource::Runtime);
    config->set("neuromod_dopamine_enable", options.enableRewardModulation, ConfigSource::Runtime);
    
    // Load checkpoint if requested
    if (options.loadCheckpoint && !options.checkpointFile.empty()) {
        // Would need to implement checkpoint loading
        NLM_LOG_INFO("Checkpoint loading not yet implemented for " + options.checkpointFile);
    }
    
    // Run appropriate mode
    if (options.mode == "benchmark") {
        runBenchmark(options, config);
    } else if (options.mode == "experiment") {
        runExperiment(options, config);
    } else {  // demo mode
        runDemo(options, config);
    }
    
    return 0;
}

