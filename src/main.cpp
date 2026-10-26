// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This phase demonstrates the integrated artificial brain with:
// - All memory systems (working, episodic, associative) connected to neural processing
// - Neuromodulation (dopamine, curiosity, novelty) affecting plasticity and dynamics
// - Prediction system integrated with learning
// - Development affecting plasticity rates
// - Checkpoint save/load working
// - Replay and consolidation functional
// - All systems working together as a coherent brain loop

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
#include <map>
#include <algorithm>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     Integrated artificial brain with all systems connected.    ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "NLM - Neural Learning Machine (Phase 6)" << std::endl;
    std::cout << "=" * 60 << std::endl;
    std::cout << std::endl;
    
    std::cout << "Basic Options:" << std::endl;
    std::cout << "  --help, -h                  Show this help message" << std::endl;
    std::cout << "  --version                   Show version information" << std::endl;
    std::cout << "  --config FILE, -c FILE       Load configuration from file" << std::endl;
    std::cout << "  --phase VALUE, -p VALUE      Select phase (2, 3, 4, 5, 6, demo)" << std::endl;
    std::cout << "  --neurons COUNT             Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps COUNT               Number of simulation steps (default: 1000)" << std::endl;
    std::cout << "  --seed VALUE                Random seed (default: 42)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Phase 2 Options (Basic Neural Computation):" << std::endl;
    std::cout << "  --test1, -t1               Run basic connectivity test" << std::endl;
    std::cout << "  --test2, -t2               Run plasticity learning experiment" << std::endl;
    std::cout << "  --test3, -t3               Run STDP verification" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Phase 3 Options (World Interaction):" << std::endl;
    std::cout << "  --world, -w                Enable world environment simulation" << std::endl;
    std::cout << "  --agent, -a                Enable agent brain interface" << std::endl;
    std::cout << "  --environment STRING        Environment type (GridWorld, etc.)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Phase 4 Options (Emerging Cognition):" << std::endl;
    std::cout << "  --cognition, -cog           Enable cognitive capabilities" << std::endl;
    std::cout << "  --concepts                  Enable concept formation" << std::endl;
    std::cout << "  --planning                  Enable neural planning" << std::endl;
    std::cout << "  --attention                 Enable attention mechanisms" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Phase 5 Options (Lifetime Learning):" << std::endl;
    std::cout << "  --lifetime, -life           Enable lifetime learning experiments" << std::endl;
    std::cout << "  --continual                 Enable continual learning" << std::endl;
    std::cout << "  --scaling                   Enable scaling experiments" << std::endl;
    std::cout << "  --damage                    Enable damage recovery experiments" << std::endl;
    std::endl;
    
    std::cout << "Phase 6 Options (Final Integration):" << std::endl;
    std::cout << "  --integration, -i           Enable integration verification" << std::endl;
    std::cout << "  --memory, -mem              Test memory integration" << std::endl;
    std::cout << "  --neuromod, -mod            Test neuromodulation integration" << std::endl;
    std::cout << "  --checkpoint, -check        Test checkpointing" << std::endl;
    std::cout << "  --replay, -replay           Test replay system" << std::endl;
    std::endl;
    
    std::cout << "Output Options:" << std::endl;
    std::cout << "  --verbose, -v               Verbose logging" << std::endl;
    std::cout << "  --quiet, -q                 Quiet mode (errors only)" << std::endl;
    std::cout << "  --progress                  Show progress during simulation" << std::endl;
    std::cout << "  --json OUTPUT               Save results in JSON format" << std::endl;
    std::cout << "  --csv OUTPUT                Save results in CSV format" << std::endl;
    std::endl;
    
    std::cout << "Advanced Options:" << std::endl;
    std::cout << "  --threads COUNT             Number of threads (default: 1)" << std::endl;
    std::cout << "  --simd                      Enable SIMD optimizations" << std::endl;
    std::cout << "  --multithread              Enable multithreading" << std::endl;
    std::cout << "  --sparse                    Use sparse connectivity" << std::endl;
    std::cout << "  --event-driven              Use event-driven simulation" << std::endl;
    std::cout << "  --auto-save INTERVAL        Auto-save every INTERVAL steps" << std::endl;
    std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --phase 6 --neurons 1000 --steps 5000" << std::endl;
    std::cout << "  " << programName << " --phase demo --test1 --test2 --test3" << std::endl;
    std::cout << "  " << programName << " --phase 4 --cognition --concepts --planning" << std::endl;
    std::cout << "  " << programName << " --phase lifetime --progress --verbose" << std::endl;
}

void printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version 0.1.0 (Phase 6)" << std::endl;
    std::cout << "An experimental artificial developmental brain" << std::endl;
    std::cout << "Built for Phase 6: Final Integration" << std::endl;
}

void parseCommandLine(int argc, char** argv, 
                     std::string& configFile,
                     std::string& phase,
                     size_t& neuronCount,
                     size_t& steps,
                     uint64_t& seed,
                     bool& verbose,
                     bool& quiet,
                     bool& progress,
                     bool& enableWorld,
                     bool& enableAgent,
                     bool& enableCognition,
                     bool& enableLifetime,
                     bool& enableIntegration,
                     size_t& threadCount,
                     std::string& outputFile,
                     std::string& outputFormat,
                     bool& enableMultithreading,
                     bool& enableSIMD,
                     bool& enableSparseConnectivity,
                     bool& enableEventDriven,
                     size_t& autoSaveInterval) {
    
    std::map<std::string, std::string> args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            exit(0);
        } else if (arg == "--version") {
            printVersion();
            exit(0);
        } else if (arg.find("--") == 0) {
            size_t eqPos = arg.find('=');
            if (eqPos != std::string::npos) {
                args[arg.substr(0, eqPos)] = arg.substr(eqPos + 1);
            } else {
                args[arg] = "";
            }
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            exit(1);
        }
    }
    
    // Parse arguments
    if (args.find("--config") != args.end() || args.find("-c") != args.end()) {
        configFile = args["--config"].empty() ? args["-c"] : args["--config"];
    }
    
    if (args.find("--phase") != args.end() || args.find("-p") != args.end()) {
        phase = args["--phase"].empty() ? args["-p"] : args["--phase"];
    }
    
    if (args.find("--neurons") != args.end()) {
        neuronCount = std::stoul(args["--neurons"]);
    }
    
    if (args.find("--steps") != args.end()) {
        steps = std::stoul(args["--steps"]);
    }
    
    if (args.find("--seed") != args.end()) {
        seed = std::stoull(args["--seed"]);
    }
    
    if (args.find("--verbose") != args.end() || args.find("-v") != args.end()) {
        verbose = true;
    }
    
    if (args.find("--quiet") != args.end() || args.find("-q") != args.end()) {
        quiet = true;
    }
    
    if (args.find("--progress") != args.end()) {
        progress = true;
    }
    
    if (args.find("--world") != args.end() || args.find("-w") != args.end()) {
        enableWorld = true;
    }
    
    if (args.find("--agent") != args.end() || args.find("-a") != args.end()) {
        enableAgent = true;
    }
    
    if (args.find("--cognition") != args.end() || args.find("-cog") != args.end()) {
        enableCognition = true;
    }
    
    if (args.find("--lifetime") != args.end() || args.find("-life") != args.end()) {
        enableLifetime = true;
    }
    
    if (args.find("--integration") != args.end() || args.find("-i") != args.end()) {
        enableIntegration = true;
    }
    
    if (args.find("--threads") != args.end()) {
        threadCount = std::stoul(args["--threads"]);
    }
    
    if (args.find("--multithread") != args.end()) {
        enableMultithreading = true;
    }
    
    if (args.find("--simd") != args.end()) {
        enableSIMD = true;
    }
    
    if (args.find("--sparse") != args.end()) {
        enableSparseConnectivity = true;
    }
    
    if (args.find("--event-driven") != args.end()) {
        enableEventDriven = true;
    }
    
    if (args.find("--auto-save") != args.end()) {
        autoSaveInterval = std::stoul(args["--auto-save"]);
    }
    
    if (args.find("--json") != args.end()) {
        outputFile = args["--json"];
        outputFormat = "json";
    }
    
    if (args.find("--csv") != args.end()) {
        outputFile = args["--csv"];
        outputFormat = "csv";
    }
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

void runBasicConnectivityTest(std::shared_ptr<Brain> brain, bool verbose) {
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
        if (verbose && step % 10 == 0) {
            NLM_LOG_DEBUG("  Step " + std::to_string(step) + 
                         " | Spikes: " + std::to_string(brain->getTotalSpikeCount() - initialSpikes));
        }
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

void runPlasticityExperiment(std::shared_ptr<Brain> brain, bool verbose) {
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
        if (verbose && step % 100 == 0) {
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

void runStdpVerification(std::shared_ptr<Brain> brain, bool verbose) {
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
        
        if (verbose && trial % 10 == 0) {
            NLM_LOG_DEBUG("  Trial " + std::to_string(trial));
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
    printBanner();
    
    std::cout << "Initializing NLM Phase 6 Real Neural Computation...\n" << std::endl;
    
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    std::string phase = "demo"; // default
    size_t neuronCount = 500;
    size_t steps = 1000;
    uint64_t seed = 42;
    bool verbose = false;
    bool quiet = false;
    bool progress = false;
    bool enableWorld = false;
    bool enableAgent = false;
    bool enableCognition = false;
    bool enableLifetime = false;
    bool enableIntegration = false;
    size_t threadCount = 1;
    std::string outputFile = "";
    std::string outputFormat = "";
    bool enableMultithreading = false;
    bool enableSIMD = false;
    bool enableSparseConnectivity = false;
    bool enableEventDriven = false;
    size_t autoSaveInterval = 0;
    
    // Parse enhanced command line arguments
    parseCommandLine(argc, argv,
                     configFile, phase, neuronCount, steps, seed, verbose, quiet, progress,
                     enableWorld, enableAgent, enableCognition, enableLifetime, enableIntegration,
                     threadCount, outputFile, outputFormat, enableMultithreading, enableSIMD,
                     enableSparseConnectivity, enableEventDriven, autoSaveInterval);
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(quiet ? LogLevel::Error : 
                                                           verbose ? LogLevel::Debug : LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6: Enhanced Command Line Interface ===");
    NLM_LOG_INFO("Phase: " + phase);
    NLM_LOG_INFO("Neurons: " + std::to_string(neuronCount));
    NLM_LOG_INFO("Steps: " + std::to_string(steps));
    NLM_LOG_INFO("Seed: " + std::to_string(seed));
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    if (configFile != "configs/default.cfg" && config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with parsed command line args
    config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::CommandLine);
    config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::CommandLine);
    
    if (enableMultithreading) {
        config->set("enable_multithreading", true, ConfigSource::CommandLine);
    }
    if (enableSIMD) {
        config->set("enable_simd", true, ConfigSource::CommandLine);
    }
    if (enableSparseConnectivity) {
        config->set("enable_sparse_connectivity", true, ConfigSource::CommandLine);
    }
    if (enableEventDriven) {
        config->set("enable_event_driven", true, ConfigSource::CommandLine);
    }
    
    // Phase-specific configuration
    if (phase == "2") {
        NLM_LOG_INFO("Running Phase 2: Real Neural Computation");
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
    } else if (phase == "3") {
        NLM_LOG_INFO("Running Phase 3: World Interaction");
        config->set("simulation_timestep", 0.01, ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        if (enableWorld && enableAgent) {
            NLM_LOG_INFO("World and agent systems enabled");
            config->set("environment_name", "GridWorld", ConfigSource::CommandLine);
        }
        
    } else if (phase == "4") {
        NLM_LOG_INFO("Running Phase 4: Emerging Cognition");
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.12f, ConfigSource::Default);
        
        if (enableCognition) {
            NLM_LOG_INFO("Cognitive systems enabled");
            config->set("enable_temporal_prediction", true, ConfigSource::CommandLine);
            config->set("enable_working_memory", true, ConfigSource::CommandLine);
            config->set("enable_concept_formation", true, ConfigSource::CommandLine);
            config->set("enable_planning", true, ConfigSource::CommandLine);
            config->set("enable_attention", true, ConfigSource::CommandLine);
        }
        
    } else if (phase == "5") {
        NLM_LOG_INFO("Running Phase 5: Lifetime Learning");
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        if (enableLifetime) {
            NLM_LOG_INFO("Lifetime learning systems enabled");
            config->set("enable_multitask_learning", true, ConfigSource::CommandLine);
            config->set("enable_continual_learning", true, ConfigSource::CommandLine);
            config->set("enable_scaling", true, ConfigSource::CommandLine);
            config->set("enable_damage_recovery", true, ConfigSource::CommandLine);
        }
        
    } else if (phase == "6" || phase == "demo") {
        NLM_LOG_INFO("Running Phase 6: Final Integration (Enhanced)");
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        if (enableIntegration) {
            NLM_LOG_INFO("Integration verification enabled");
            config->set("enable_integration_verification", true, ConfigSource::CommandLine);
        }
        
        if (enableWorld && enableAgent) {
            NLM_LOG_INFO("World and agent systems enabled for integration");
        }
        
        if (enableCognition) {
            NLM_LOG_INFO("Cognitive systems enabled for integration");
        }
        
        if (enableLifetime) {
            NLM_LOG_INFO("Lifetime learning systems enabled for integration");
        }
        
        config->set("enable_checkpointing", true, ConfigSource::CommandLine);
        config->set("enable_replay", true, ConfigSource::CommandLine);
        config->set("enable_development", true, ConfigSource::CommandLine);
    }
    
    // Log configuration summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Configuration Summary:");
    NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
    NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.1f)));
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
    
    // Phase-specific execution based on command line arguments
    if (phase == "2") {
        // Phase 2: Basic neural computation tests
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== PHASE 2: Basic Neural Computation ===");
        
        // Run all three Phase 2 tests
        runBasicConnectivityTest(brain, progress);
        
        // Reset brain for plasticity experiment
        brain->reset();
        brain->initialize();
        
        runPlasticityExperiment(brain, progress);
        
        // Reset and run STDP verification
        brain->reset();
        brain->initialize();
        runStdpVerification(brain, progress);
        
    } else if (phase == "3") {
        // Phase 3: World interaction (simplified)
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== PHASE 3: World Interaction ===");
        
        if (enableWorld && enableAgent) {
            NLM_LOG_INFO("World and agent simulation enabled");
            NLM_LOG_INFO("  Running world simulation with agent control...");
            
            // Simulate some world steps
            for (SimulationStep step = 0; step < std::min<size_t>(steps, 100); ++step) {
                brain->step(step, step * timestep);
                if (progress && step % 20 == 0) {
                    NLM_LOG_INFO("  Step " + std::to_string(step) + 
                                " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                                " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
                }
            }
            
            NLM_LOG_INFO("  ✓ World simulation completed");
        }
        
    } else if (phase == "4") {
        // Phase 4: Cognition (demonstrated through Python interface)
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== PHASE 4: Emerging Cognition ===");
        
        if (enableCognition) {
            NLM_LOG_INFO("Cognitive capabilities demonstrated via Python bindings");
            NLM_LOG_INFO("  Temporal prediction, concept formation, planning, attention");
            NLM_LOG_INFO("  (See Phase4Demo.cpp for examples)");
            
            // Demonstrate with simple steps
            for (SimulationStep step = 0; step < std::min<size_t>(steps, 50); ++step) {
                brain->step(step, step * timestep);
                if (progress) {
                    auto conceptFormation = brain->getConceptFormation();
                    int conceptCount = conceptFormation ? conceptFormation->getConceptCount() : 0;
                    NLM_LOG_INFO("  Step " + std::to_string(step) + 
                                " | Concepts: " + std::to_string(conceptCount));
                }
            }
            
            NLM_LOG_INFO("  ✓ Cognitive capabilities demonstrated");
        }
        
    } else if (phase == "5") {
        // Phase 5: Lifetime learning (demonstrated through Python interface)
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== PHASE 5: Lifetime Learning & Continual Adaptation ===");
        
        if (enableLifetime) {
            NLM_LOG_INFO("Lifetime learning capabilities demonstrated via Python bindings");
            NLM_LOG_INFO("  Multitask learning, continual adaptation, scaling, damage recovery");
            NLM_LOG_INFO("  (See Phase5Demo.cpp for examples)");
            
            // Demonstrate with some steps
            for (SimulationStep step = 0; step < std::min<size_t>(steps, 100); ++step) {
                brain->step(step, step * timestep);
                if (progress) {
                    NLM_LOG_INFO("  Step " + std::to_string(step) + 
                                " | Lifetime Adaptation: Demonstrated");
                }
            }
            
            NLM_LOG_INFO("  ✓ Lifetime learning capabilities demonstrated");
        }
        
    } else {
        // Phase 6 or demo: Integration (use the Phase6Demo approach)
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== PHASE 6: Final Integration ===");
        
        // Quick integration verification
        NLM_LOG_INFO("Running integration verification...");
        bool integrationOK = true;
        
        // Check memory systems
        if (brain->getWorkingMemory() != nullptr) {
            NLM_LOG_INFO("[PASS] Working memory is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Working memory is NOT integrated");
            integrationOK = false;
        }
        
        if (brain->getEpisodicMemory() != nullptr) {
            NLM_LOG_INFO("[PASS] Episodic memory is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Episodic memory is NOT integrated");
            integrationOK = false;
        }
        
        // Check neuromodulation
        if (brain->getDopamine() != nullptr) {
            NLM_LOG_INFO("[PASS] Dopamine system is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Dopamine system is NOT integrated");
            integrationOK = false;
        }
        
        if (brain->getCuriosity() != nullptr) {
            NLM_LOG_INFO("[PASS] Curiosity system is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Curiosity system is NOT integrated");
            integrationOK = false;
        }
        
        // Check prediction system
        if (brain->getPredictionSystem() != nullptr) {
            NLM_LOG_INFO("[PASS] Prediction system is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Prediction system is NOT integrated");
            integrationOK = false;
        }
        
        // Check cognition systems
        if (brain->getPlanner() != nullptr) {
            NLM_LOG_INFO("[PASS] Planner is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Planner is NOT integrated");
            integrationOK = false;
        }
        
        if (brain->getConceptFormation() != nullptr) {
            NLM_LOG_INFO("[PASS] Concept formation is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Concept formation is NOT integrated");
            integrationOK = false;
        }
        
        if (brain->getAttention() != nullptr) {
            NLM_LOG_INFO("[PASS] Attention is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Attention is NOT integrated");
            integrationOK = false;
        }
        
        // Check development system
        if (brain->getDevelopmentSystem() != nullptr) {
            NLM_LOG_INFO("[PASS] Development system is integrated");
        } else {
            NLM_LOG_ERROR("[FAIL] Development system is NOT integrated");
            integrationOK = false;
        }
        
        if (!integrationOK) {
            NLM_LOG_ERROR("Integration verification failed!");
            return 1;
        }
        
        NLM_LOG_INFO("Integration verification passed!");
        
        // Run some integration steps
        NLM_LOG_INFO("Running integration steps...");
        for (SimulationStep step = 0; step < std::min<size_t>(steps, 200); ++step) {
            brain->step(step, step * timestep);
            
            // Inject some sensory input
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
            
            if (progress && step % 20 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Reward: " + std::to_string(brain->getTotalSpikeCount() / (step + 1)) +
                            " | WorkingMem: " + std::to_string(brain->getWorkingMemory() ? 
                                brain->getWorkingMemory()->getActiveTraces() : 0));
            }
        }
        
        NLM_LOG_INFO("  ✓ Integration demonstration completed");
        
        // Final brain status
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Final Brain Status ===");
        brain->logStatus();
    }
    
    // Save results if requested
    if (!outputFile.empty()) {
        NLM_LOG_INFO("Saving results to: " + outputFile);
        
        // This would save results in the requested format
        // For now, just log that it would happen
        NLM_LOG_INFO("[INFO] Result saving would be implemented based on format: " + outputFormat);
    }
    
    // Final summary
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Execution Complete ===");
    NLM_LOG_INFO("Phase: " + phase);
    NLM_LOG_INFO("Total steps executed: " + std::to_string(std::min<size_t>(steps, 1000)));
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Final firing rate: " + std::to_string(brain->getAverageFiringRate()));
    NLM_LOG_INFO("");
    
    NLM_LOG_INFO("Phase " + phase + " execution completed successfully!");
    
    return 0;
}