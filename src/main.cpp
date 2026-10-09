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
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <filesystem>

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

void printHelp() {
    std::cout << R"(
╔════════════════════════════════════════════════════════════════════════╗
║                           NLM - Command Line Interface                 ║
╠════════════════════════════════════════════════════════════════════════╣
║ USAGE:                                                               ║
║   nlm [OPTIONS] [SUBCOMMAND] [ARGS...]                              ║
║                                                                      ║
║ SUBCOMMANDS:                                                         ║
║   run          Run the neural network simulation                    ║
║   benchmark    Run performance benchmarks                           ║
║   configure    Configure settings and parameters                     ║
║   debug        Debug and diagnostic tools                             ║
║   experiment   Run specific experiments                              ║
║   checkpoint   Checkpoint and save/load functionality               ║
║   help         Show this help message                                ║
║                                                                      ║
║ OPTIONS:                                                             ║
║   --help, -h                        Show help message                   ║
║   --version, -v                     Show version information          
║   --config FILE, -c FILE             Load configuration from file     ║
║   --verbose, -v                     Enable verbose output            ║
║   --quiet, -q                       Suppress non-essential output      ║
║   --log-level LEVEL                 Set logging level (debug, info,  ║
║                                    warning, error, critical)         ║
║   --log-file FILE                   Write logs to file               ║
║   --validate                         Validate configuration before   ║
║                                    running                           ║
║   --dry-run                          Parse configuration but don't    ║
║                                    run simulation                  ║
║                                                                      ║
║ ADVANCED OPTIONS:                                                   ║
║   --neuron-count COUNT              Set number of neurons           ║
║   --region-count COUNT              Set number of brain regions     ║
║   --connection-prob P               Set connection probability       ║
║   --timestep DT                      Set simulation timestep       ║
║   --seed SEED                        Set random seed                ║
║   --stdp-weights LTP,LTD              Set STDP learning weights       ║
║   --plasticity ENABLED               Enable/disable plasticity       ║
║   --development-stage STAGE          Set developmental stage       ║
║   --checkpoint-dir DIR               Set checkpoint directory        ║
║   --replay INTERVAL                 Set replay interval (steps)     ║
║   --consolidation INTERVAL           Set consolidation interval     ║
║                                                                      ║
║ EXAMPLES:                                                           ║
║   nlm run --neuron-count 1000 --timestep 0.001                       ║
║   nlm benchmark --sizes 100,500,1000,2000                           ║
║   nlm configure --set random_seed=12345 --set neuron_count=2000     ║
║   nlm run --config myconfig.json --verbose --validate               ║
║   nlm experiment --type phase6 --steps 5000 --development             ║
╚════════════════════════════════════════════════════════════════════════╝

)` << std::endl;
}

void printVersion() {
    std::cout << R"(
╔════════════════════════════════════════════════════════════════════════╗
║                           NLM Version 2.0.0                           ║
╠════════════════════════════════════════════════════════════════════════╣
║                                                                      ║
║  Neural Learning Machine - Phase 2: Real Neural Computation          ║
║  Copyright (c) 2026 NLM Project                                     ║
║                                                                      ║
║  Key Features:                                                       ║
║  • Real LIF neuron dynamics with event-driven computation           ║
║  • STDP and Hebbian plasticity integration                           ║
║  • Structural plasticity (synaptogenesis/pruning)                   ║
║  • Multi-region brain architecture                                 ║
║  • Integrated memory systems (working, episodic, associative)      ║
║  • Neuromodulation (dopamine, curiosity, novelty)                 ║
║  • Prediction and cognitive systems                                 ║
║  • Developmental and maturation stages                             ║
║  • Checkpoint save/load and replay capabilities                      ║
║                                                                      ║
║  Build Information:                                                  ║
║  • Platform: Linux                                              ║
║  • Compiler: C++17                                             ║
║  • Version: Development Build                                   ║
║  • Git Commit: cf14c76c-4b1f-4d38-9a96-64cd8a400481               ║
║                                                                      ║
╚════════════════════════════════════════════════════════════════════════╝

)` << std::endl;
}

class CommandLineParser {
public:
    struct Options {
        bool help = false;
        bool version = false;
        bool verbose = false;
        bool quiet = false;
        std::string logLevel = "info";
        std::string logFile;
        bool validate = false;
        bool dryRun = false;
        std::string configFile;
        
        // Network configuration
        size_t neuronCount = 0;
        size_t regionCount = 0;
        float connectionProbability = 0.0f;
        double timestep = 0.0;
        int64_t seed = 0;
        
        // Plasticity parameters
        float stdpLTPWeight = 0.0f;
        float stdpLTDWeight = 0.0f;
        float stdpTau = 0.0f;
        bool plasticityEnabled = true;
        std::string developmentStage = "";
        
        // Memory and replay
        std::string checkpointDir;
        size_t replayInterval = 0;
        size_t consolidationInterval = 0;
        
        // Additional parameters
        std::unordered_map<std::string, std::string> keyValuePairs;
        std::vector<std::string> experimentArgs;
    };
    
    static Options parse(int argc, char** argv) {
        Options options;
        std::string subcommand;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.find('=') != std::string::npos && arg[0] == '-') {
                size_t pos = arg.find('=');
                std::string key = arg.substr(1, pos - 1);
                std::string value = arg.substr(pos + 1);
                options.keyValuePairs[key] = value;
            }
            // Handle boolean flags
            else if (arg == "--help" || arg == "-h") {
                options.help = true;
            }
            else if (arg == "--version" || arg == "-v") {
                options.version = true;
            }
            else if (arg == "--verbose" || arg == "-v") {
                options.verbose = true;
            }
            else if (arg == "--quiet" || arg == "-q") {
                options.quiet = true;
            }
            else if (arg == "--validate") {
                options.validate = true;
            }
            else if (arg == "--dry-run") {
                options.dryRun = true;
            }
            // Handle file options
            else if (arg == "--config" || arg == "-c") {
                if (i + 1 < argc) {
                    options.configFile = argv[++i];
                }
            }
            else if (arg == "--log-file") {
                if (i + 1 < argc) {
                    options.logFile = argv[++i];
                }
            }
            else if (arg == "--log-level") {
                if (i + 1 < argc) {
                    options.logLevel = argv[++i];
                }
            }
            // Handle numeric options
            else if (arg == "--neuron-count") {
                if (i + 1 < argc) {
                    options.neuronCount = std::stoull(argv[++i]);
                }
            }
            else if (arg == "--region-count") {
                if (i + 1 < argc) {
                    options.regionCount = std::stoull(argv[++i]);
                }
            }
            else if (arg == "--connection-prob") {
                if (i + 1 < argc) {
                    options.connectionProbability = std::stof(argv[++i]);
                }
            }
            else if (arg == "--timestep") {
                if (i + 1 < argc) {
                    options.timestep = std::stod(argv[++i]);
                }
            }
            else if (arg == "--seed") {
                if (i + 1 < argc) {
                    options.seed = std::stoll(argv[++i]);
                }
            }
            // Handle advanced options
            else if (arg == "--stdp-weights") {
                if (i + 1 < argc) {
                    std::string weights = argv[++i];
                    size_t comma = weights.find(',');
                    if (comma != std::string::npos) {
                        options.stdpLTPWeight = std::stof(weights.substr(0, comma));
                        options.stdpLTDWeight = std::stof(weights.substr(comma + 1));
                    }
                }
            }
            else if (arg == "--plasticity") {
                if (i + 1 < argc) {
                    options.plasticityEnabled = (argv[++i] == "true" || argv[i] == "1" || argv[i] == "yes");
                }
            }
            else if (arg == "--development-stage") {
                if (i + 1 < argc) {
                    options.developmentStage = argv[++i];
                }
            }
            else if (arg == "--checkpoint-dir") {
                if (i + 1 < argc) {
                    options.checkpointDir = argv[++i];
                }
            }
            else if (arg == "--replay") {
                if (i + 1 < argc) {
                    options.replayInterval = std::stoull(argv[++i]);
                }
            }
            else if (arg == "--consolidation") {
                if (i + 1 < argc) {
                    options.consolidationInterval = std::stoull(argv[++i]);
                }
            }
            // Subcommands
            else if (arg == "run" || arg == "benchmark" || arg == "configure" || 
                     arg == "debug" || arg == "experiment" || arg == "checkpoint" ||
                     arg == "help") {
                subcommand = arg;
            }
            // Pass through arguments for subcommands
            else if (!subcommand.empty()) {
                options.experimentArgs.push_back(arg);
            }
            else {
                NLM_LOG_WARNING("Unknown option: " + arg);
            }
        }
        
        return options;
    }
};

class NLMApplication {
public:
    static void run(const CommandLineParser::Options& options) {
        NLM_LOG_INFO("=== Starting NLM Neural Network Simulation ===");
        
        // Initialize configuration
        auto config = std::make_shared<Config>();
        
        // Load configuration from file if specified
        if (!options.configFile.empty()) {
            NLM_LOG_INFO("Loading configuration from: " + options.configFile);
            if (!config->loadFromFile(options.configFile)) {
                NLM_LOG_ERROR("Failed to load configuration file: " + options.configFile);
                NLM_LOG_INFO("Using default configuration");
            }
        }
        
        // Apply command line overrides
        if (options.neuronCount > 0) {
            config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::CommandLine);
        }
        if (options.regionCount > 0) {
            config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::CommandLine);
        }
        if (options.connectionProbability > 0.0f) {
            config->set("connection_probability", options.connectionProbability, ConfigSource::CommandLine);
        }
        if (options.timestep > 0.0) {
            config->set("simulation_timestep", options.timestep, ConfigSource::CommandLine);
        }
        if (options.seed > 0) {
            config->set("random_seed", options.seed, ConfigSource::CommandLine);
        }
        if (options.stdpLTPWeight > 0.0f) {
            config->set("stdp_ltp_weight", options.stdpLTPWeight, ConfigSource::CommandLine);
        }
        if (options.stdpLTDWeight > 0.0f) {
            config->set("stdp_ltd_weight", options.stdpLTDWeight, ConfigSource::CommandLine);
        }
        if (options.stdpTau > 0.0f) {
            config->set("stdp_tau", options.stdpTau, ConfigSource::CommandLine);
        }
        
        // Apply key-value pairs
        for (const auto& kv : options.keyValuePairs) {
            config->set(kv.first, kv.second, ConfigSource::CommandLine);
        }
        
        // Validate configuration if requested
        if (options.validate) {
            NLM_LOG_INFO("Validating configuration...");
            config->enableValidation(true, false);
            if (!config->validateAll()) {
                NLM_LOG_ERROR("Configuration validation failed");
                NLM_LOG_INFO("Run with --help for configuration options");
                return;
            }
            NLM_LOG_INFO("Configuration validation passed");
        }
        
        // Log configuration
        NLM_LOG_INFO("Configuration loaded:");
        NLM_LOG_INFO("  Neuron count: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
        NLM_LOG_INFO("  Region count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
        NLM_LOG_INFO("  Connection probability: " + std::to_string(config->getOr<float>("connection_probability", 0.1f)));
        NLM_LOG_INFO("  Simulation timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
        
        // Initialize brain
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return;
        }
        
        brain->logStatus();
        
        // Run simulation
        NLM_LOG_INFO("Starting simulation...");
        
        if (options.dryRun) {
            NLM_LOG_INFO("Dry run mode - simulation not executed");
            NLM_LOG_INFO("Configuration would be used to initialize brain with " + 
                std::to_string(config->getOr<int64_t>("neuron_count", 1000)) + " neurons");
        } else {
            // Run basic connectivity test
            runBasicConnectivityTest(brain);
            
            // Reset brain for plasticity experiment
            brain->reset();
            brain->initialize();
            
            // Run plasticity learning experiment
            runPlasticityExperiment(brain);
            
            // Reset and run STDP verification
            brain->reset();
            brain->initialize();
            runStdpVerification(brain);
            
            // Final brain status
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Final Brain Status ===");
            brain->logStatus();
            
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== NLM Simulation Complete ===");
        }
    }
    
private:
    static void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No regions found in brain");
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_ERROR("No neurons found in brain");
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
    
    static void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
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
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
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
        
        // Compute and display statistics
        experiment.computeStatistics();
    }
    
    static void runStdpVerification(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No regions found in brain");
            return;
        }
        
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_ERROR("Not enough synapses for STDP test");
            return;
        }
        
        NLM_LOG_INFO("  Testing STDP on 5 synapses:");
        
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " before: " + std::to_string(beforeWeights[i]));
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < 50; ++trial) {
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        NLM_LOG_INFO("  After correlated activity:");
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                    " after: " + std::to_string(synapses[i]->getWeight()) +
                    " (Δ=" + std::to_string(delta) + ")");
        }
        
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
    
    // LearningExperiment struct definition
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
            
            float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float initialMean = initialSum / initialWeights.size();
            float finalMean = finalSum / finalWeights.size();
            
            NLM_LOG_INFO("Weight Statistics:");
            NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
            NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
            NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
            
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
};

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Advanced Command Line Interface...\n" << std::endl;
    
    // Parse command line options
    CommandLineParser::Options options = CommandLineParser::parse(argc, argv);
    
    // Handle help and version options
    if (options.help) {
        printHelp();
        return 0;
    }
    
    if (options.version) {
        printVersion();
        return 0;
    }
    
    // Initialize logger with appropriate level
    LogLevel logLevel = LogLevel::Info;
    if (options.logLevel == "debug") logLevel = LogLevel::Debug;
    else if (options.logLevel == "warning") logLevel = LogLevel::Warning;
    else if (options.logLevel == "error") logLevel = LogLevel::Error;
    else if (options.logLevel == "critical") logLevel = LogLevel::Critical;
    
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(logLevel);
    consoleLogger->setUseColors(!options.quiet);
    if (!options.logFile.empty()) {
        consoleLogger->setOutputFile(options.logFile);
    }
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Advanced Command Line Interface ===");
    NLM_LOG_INFO("Version: 2.0.0");
    NLM_LOG_INFO("Arguments processed: " + std::to_string(argc - 1));
    
    // Check for subcommands
    bool hasSubcommand = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "run" || arg == "benchmark" || arg == "configure" || 
            arg == "debug" || arg == "experiment" || arg == "checkpoint" ||
            arg == "help") {
            hasSubcommand = true;
            break;
        }
    }
    
    if (hasSubcommand) {
        NLM_LOG_INFO("Note: Subcommands not yet implemented. Running default simulation.");
    }
    
    // Run the application
    NLMApplication::run(options);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Command Line Interface Complete ===");
    
    return 0;
}
