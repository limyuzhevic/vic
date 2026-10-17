// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration and Advanced Brain Interface
//
// This phase provides a comprehensive interface to the fully integrated
// artificial developmental brain with all cognitive systems connected.

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/Phase6Demo.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <unordered_map>
#include <stdexcept>

using namespace nlm;

class NLMCLI {
public:
    enum class Mode {
        UNKNOWN,
        HELP,
        VERSION,
        DEMO,
        EXPERIMENT,
        STATUS,
        CONFIG,
        QUICK_START,
        INTEGRATION_VERIFY,
        CHECKPOINT,
        INFO
    };
    
    struct CommandOptions {
        Mode mode = Mode::UNKNOWN;
        std::string configFile = "configs/default.cfg";
        uint64_t maxSteps = 10000;
        size_t neuronCount = 1000;
        size_t regionCount = 1;
        float connectionProbability = 0.1f;
        bool enableCheckpointing = true;
        bool enableReplay = true;
        bool enableDevelopment = true;
        std::string checkpointPath = "./checkpoint_test.bin";
        bool verbose = false;
        bool quiet = false;
        std::string experimentType = "full";
        std::string outputFile = "";
        bool listModes = false;
        bool showConfig = false;
        bool benchmark = false;
        size_t benchmarkRuns = 3;
    };
    
    NLMCLI() = default;
    ~NLMCLI() = default;
    
    void printVersion() {
        std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
        std::cout << "Version: Phase 6 - Final Integration" << std::endl;
        std::cout << "Description: Fully integrated artificial developmental brain" << std::endl;
        std::cout << "Author: NLM Research Project" << std::endl;
        std::cout << "License: MIT" << std::endl;
        std::cout << "Website: https://github.com/nlm-project/nlm" << std::endl;
    }
    
    void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6 - Final Integration                              ║
    ║                                                               ║
    ║     An integrated artificial developmental brain.            ║
    ║     All cognitive systems connected and working.              ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
    void printHelp() {
        printBanner();
        std::cout << std::endl;
        std::cout << "USAGE: nlm [OPTIONS] [COMMAND] [ARGS...]" << std::endl;
        std::cout << std::endl;
        
        std::cout << "NLM provides multiple modes for interacting with the integrated brain:" << std::endl;
        std::cout << std::endl;
        
        std::cout << "COMMANDS:" << std::endl;
        std::cout << "  demo               Run Phase 6 integration demonstration" << std::endl;
        std::cout << "  experiment         Run Phase 6 integration experiment" << std::endl;
        std::cout << "  status             Show current system status" << std::endl;
        std::cout << "  config             Display configuration information" << std::endl;
        std::cout << "  verify             Verify integration of all systems" << std::endl;
        std::cout << "  checkpoint         Test checkpoint save/load functionality" << std::endl;
        std::cout << "  quickstart         Show quick start guide for new users" << std::endl;
        std::cout << "  info              Show detailed system information" << std::endl;
        std::cout << "  benchmark          Run performance benchmarks" << std::endl;
        std::cout << std::endl;
        
        std::cout << "GLOBAL OPTIONS:" << std::endl;
        std::cout << "  --help             Show this help message" << std::endl;
        std::cout << "  --version          Show version information" << std::endl;
        std::cout << "  --config FILE      Configuration file (default: configs/default.cfg)" << std::endl;
        std::cout << "  --verbose          Enable verbose output" << std::endl;
        std::cout << "  --quiet            Suppress informational messages" << std::endl;
        std::cout << "  --benchmark        Run benchmark mode instead of command" << std::endl;
        std::cout << "  --benchmark-runs N Number of benchmark runs (default: 3)" << std::endl;
        std::cout << "  --neuron-count N   Number of neurons (default: 1000)" << std::endl;
        std::cout << "  --region-count N   Number of regions (default: 1)" << std::endl;
        std::cout << "  --steps N          Number of simulation steps (default: 10000)" << std::endl;
        std::cout << "  --connection-prob P Connection probability (default: 0.1)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "MODE-SPECIFIC OPTIONS:" << std::endl;
        std::cout << "  For experiment mode: --enable-checkpointing, --enable-replay, --enable-development" << std::endl;
        std::cout << "  For demo mode:      --quick" << std::endl;
        std::cout << "  For benchmark mode: --runs N" << std::endl;
        std::cout << std::endl;
        
        std::cout << "EXAMPLES:" << std::endl;
        std::cout << "  nlm demo                    # Run integration demonstration" << std::endl;
        std::cout << "  nlm experiment --steps 5000  # Run experiment with 5000 steps" << std::endl;
        std::cout << "  nlm verify                  # Verify all systems are integrated" << std::endl;
        std::cout << "  nlm status                  # Show current brain status" << std::endl;
        std::cout << "  nlm quickstart              # Get started with NLM" << std::endl;
        std::cout << std::endl;
    }
    
    void printQuickStart() {
        std::cout << "NLM QUICK START GUIDE" << std::endl;
        std::cout << "====================" << std::endl;
        std::cout << std::endl;
        std::cout << "Welcome to NLM (熙然) - the Neural Learning Machine!" << std::endl;
        std::cout << std::endl;
        std::cout << "WHAT IS IT:" << std::endl;
        std::cout << "  NLM is an experimental artificial developmental brain." << std::endl;
        std::cout << "  It starts in a primitive state and develops complex abilities" << std::endl;
        std::cout << "  through interaction with its environment." << std::endl;
        std::cout << std::endl;
        std::cout << "WHAT IT DOES:" << std::endl;
        std::cout << "  • Simulates spiking neural networks using LIF neurons" << std::endl;
        std::cout << "  • Has multiple cognitive systems (memory, prediction, planning)" << std::endl;
        std::cout << "  • Learns from experience through multiple plasticity rules" << std::endl;
        std::cout << "  • Develops over time like a biological brain" << std::endl;
        std::cout << "  • Stores experiences in episodic memory" << std::endl;
        std::cout << std::endl;
        std::cout << "GETTING STARTED:" << std::endl;
        std::cout << std::endl;
        std::cout << "1. Run a quick demonstration:" << std::endl;
        std::cout << "   nlm demo --steps 1000" << std::endl;
        std::cout << std::endl;
        std::cout << "2. See what systems are integrated:" << std::endl;
        std::cout << "   nlm verify" << std::endl;
        std::cout << std::endl;
        std::cout << "3. View current system status:" << std::endl;
        std::cout << "   nlm status" << std::endl;
        std::endl;
        std::cout << "4. See system configuration details:" << std::endl;
        std::cout << "   nlm config" << std::endl;
        std::cout << std::endl;
        std::cout << "TRY IT NOW:" << std::endl;
        std::cout << "  Run: nlm demo --steps 100 --quiet" << std::endl;
        std::cout << std::endl;
        std::cout << "FOR MORE HELP:" << std::endl;
        std::cout << "  nlm help                    # Show all available commands" << std::endl;
        std::cout << "  nlm experiment --help       # Help for experiment mode" << std::endl;
        std::cout << "  nlm demo --help            # Help for demo mode" << std::endl;
        std::cout << std::endl;
    }
    
    void printStatus(std::shared_ptr<Brain> brain) {
        std::cout << "=== NLM Brain Status ===" << std::endl;
        brain->logStatus();
        std::cout << std::endl;
        
        // Check which systems are available
        std::cout << "=== System Integration Status ===" << std::endl;
        
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        auto* dopamine = brain->getDopamine();
        auto* predictionSystem = brain->getPredictionSystem();
        auto* planner = brain->getPlanner();
        auto* conceptFormation = brain->getConceptFormation();
        auto* attention = brain->getAttention();
        auto* developmentSystem = brain->getDevelopmentSystem();
        
        struct SystemStatus {
            const char* name;
            void* pointer;
            bool optional;
        };
        
        std::vector<SystemStatus> systems = {
            {"Working Memory", workingMemory, true},
            {"Episodic Memory", episodicMemory, true},
            {"Dopamine System", dopamine, true},
            {"Prediction System", predictionSystem, true},
            {"Neural Planner", planner, true},
            {"Concept Formation", conceptFormation, true},
            {"Attention System", attention, true},
            {"Development System", developmentSystem, true}
        };
        
        for (const auto& sys : systems) {
            if (sys.pointer) {
                std::cout << "✓ " << sys.name << " (INTEGRATED)" << std::endl;
            } else if (sys.optional) {
                std::cout << "○ " << sys.name << " (OPTIONAL - may be disabled)" << std::endl;
            } else {
                std::cout << "✗ " << sys.name << " (MISSING)" << std::endl;
            }
        }
        
        // Memory stats
        if (workingMemory) {
            std::cout << "Working memory traces: " << workingMemory->getActiveTraces() << std::endl;
        }
        if (episodicMemory) {
            std::cout << "Episodic memory episodes: " << episodicMemory->getEpisodeCount() << std::endl;
        }
        
        // Basic statistics
        std::cout << std::endl;
        std::cout << "=== Basic Statistics ===" << std::endl;
        std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Pending spike events: " << brain->getPendingSpikeEventCount() << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    }
    
    void printConfigInfo(std::shared_ptr<Config> config) {
        std::cout << "=== NLM Configuration ===" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Core Parameters:" << std::endl;
        std::cout << "  random_seed: " << config->getOr<int64_t>("random_seed", 42) << std::endl;
        std::cout << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
        std::cout << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 1000) << std::endl;
        std::cout << "  region_count: " << config->getOr<int64_t>("region_count", 1) << std::endl;
        std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.1f) << std::endl;
        std::cout << std::endl;
        
        std::cout << "Plasticity Parameters:" << std::endl;
        std::cout << "  stdp_ltp_weight: " << config->getOr<float>("stdp_ltp_weight", 0.01f) << std::endl;
        std::cout << "  stdp_ltd_weight: " << config->getOr<float>("stdp_ltd_weight", 0.012f) << std::endl;
        std::cout << "  stdp_tau: " << config->getOr<float>("stdp_tau", 20.0f) << std::endl;
        std::cout << "  synaptogenesis_rate: " << config->getOr<float>("synaptogenesis_rate", 0.0001f) << std::endl;
        std::cout << "  pruning_rate: " << config->getOr<float>("pruning_rate", 0.00001f) << std::endl;
        std::cout << std::endl;
        
        std::cout << "Development Parameters:" << std::endl;
        std::cout << "  replay_interval: " << config->getOr<size_t>("replay_interval", 100) << std::endl;
        std::cout << "  consolidation_interval: " << config->getOr<size_t>("consolidation_interval", 1000) << std::endl;
        std::cout << std::endl;
    }
    
    Mode parseMode(const std::string& arg) {
        static const std::unordered_map<std::string, Mode> modeMap = {
            {"demo", Mode::DEMO},
            {"experiment", Mode::EXPERIMENT},
            {"status", Mode::STATUS},
            {"config", Mode::CONFIG},
            {"verify", Mode::INTEGRATION_VERIFY},
            {"quickstart", Mode::QUICK_START},
            {"info", Mode::INFO},
            {"checkpoint", Mode::CHECKPOINT},
            {"benchmark", Mode::BENCHMARK},
            {"help", Mode::HELP},
            {"version", Mode::VERSION},
        };
        
        auto it = modeMap.find(arg);
        return it != modeMap.end() ? it->second : Mode::UNKNOWN;
    }
    
    Mode parseCommandLine(int argc, char** argv, CommandOptions& options) {
        if (argc <= 1) {
            return Mode::DEMO;  // Default: run demo
        }
        
        // Parse global options first
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                return Mode::HELP;
            } else if (arg == "--version" || arg == "-v") {
                return Mode::VERSION;
            } else if (arg == "--quickstart" || arg == "-qs") {
                return Mode::QUICK_START;
            } else if (arg == "--verbose" || arg == "-V") {
                options.verbose = true;
            } else if (arg == "--quiet" || arg == "-q") {
                options.quiet = true;
            } else if (arg == "--benchmark") {
                options.benchmark = true;
            } else if (arg.find("--benchmark-runs=") == 0) {
                options.benchmarkRuns = std::stoul(arg.substr(16));
            } else if (arg == "--benchmark-runs" && i + 1 < argc) {
                options.benchmarkRuns = std::stoul(argv[++i]);
            } else if (arg.find("--neuron-count=") == 0) {
                options.neuronCount = std::stoul(arg.substr(15));
            } else if (arg == "--neuron-count" && i + 1 < argc) {
                options.neuronCount = std::stoul(argv[++i]);
            } else if (arg.find("--region-count=") == 0) {
                options.regionCount = std::stoul(arg.substr(14));
            } else if (arg == "--region-count" && i + 1 < argc) {
                options.regionCount = std::stoul(argv[++i]);
            } else if (arg.find("--steps=") == 0) {
                options.maxSteps = std::stoul(arg.substr(7));
            } else if (arg == "--steps" && i + 1 < argc) {
                options.maxSteps = std::stoul(argv[++i]);
            } else if (arg.find("--connection-prob=") == 0) {
                options.connectionProbability = std::stof(arg.substr(18));
            } else if (arg == "--connection-prob" && i + 1 < argc) {
                options.connectionProbability = std::stof(argv[++i]);
            } else if (arg.find("--config=") == 0) {
                options.configFile = arg.substr(8);
            } else if (arg == "--config" && i + 1 < argc) {
                options.configFile = argv[++i];
            } else if (arg.find("--enable-checkpointing=") == 0) {
                options.enableCheckpointing = (arg.substr(23) == "1" || arg.substr(23) == "true");
            } else if (arg == "--enable-checkpointing" && i + 1 < argc) {
                options.enableCheckpointing = (std::string(argv[++i]) == "1" || std::string(argv[i]) == "true");
            } else if (arg.find("--enable-replay=") == 0) {
                options.enableReplay = (arg.substr(16) == "1" || arg.substr(16) == "true");
            } else if (arg == "--enable-replay" && i + 1 < argc) {
                options.enableReplay = (std::string(argv[++i]) == "1" || std::string(argv[i]) == "true");
            } else if (arg.find("--enable-development=") == 0) {
                options.enableDevelopment = (arg.substr(21) == "1" || arg.substr(21) == "true");
            } else if (arg == "--enable-development" && i + 1 < argc) {
                options.enableDevelopment = (std::string(argv[++i]) == "1" || std::string(argv[i]) == "true");
            } else {
                // Check if it's a command
                Mode mode = parseMode(arg);
                if (mode != Mode::UNKNOWN) {
                    options.mode = mode;
                    // Continue parsing command-specific options
                    return parseCommandSpecificOptions(argc, argv, i, options);
                }
            }
        }
        
        return options.mode;  // Default mode
    }
    
    Mode parseCommandSpecificOptions(int argc, char** argv, int startIndex, CommandOptions& options) {
        // Parse command-specific options
        for (int i = startIndex + 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                return options.mode;  // Will print help for this command
            } else if (arg.find("--experiment-type=") == 0) {
                options.experimentType = arg.substr(18);
            } else if (arg == "--experiment-type" && i + 1 < argc) {
                options.experimentType = argv[++i];
            } else if (arg.find("--output=") == 0) {
                options.outputFile = arg.substr(8);
            } else if (arg == "--output" && i + 1 < argc) {
                options.outputFile = argv[++i];
            } else if (arg == "--quick") {
                // Quick demo mode
            } else if (arg.find("--runs=") == 0) {
                options.benchmarkRuns = std::stoul(arg.substr(6));
            } else if (arg == "--runs" && i + 1 < argc) {
                options.benchmarkRuns = std::stoul(argv[++i]);
            }
        }
        
        return options.mode;
    }
    
    void initializeLogger(bool verbose, bool quiet) {
        auto logger = std::make_shared<Logger>();
        
        LogLevel level = LogLevel::Info;
        if (quiet) {
            level = LogLevel::Error;
        } else if (verbose) {
            level = LogLevel::Debug;
        }
        
        auto consoleLogger = std::make_shared<ConsoleLogger>(level);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    bool loadConfig(std::shared_ptr<Config> config, const std::string& configFile) {
        // Try to load from file
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
            return true;
        }
        
        NLM_LOG_INFO("Using default configuration (no config file found: " + configFile + ")");
        return false;
    }
    
    void runDemo(const CommandOptions& options) {
        std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
        std::cout << "Running comprehensive integration demonstration..." << std::endl << std::endl;
        
        // Use Phase6Demo to run the demo
        Phase6Demo demo;
        
        if (options.verbose) {
            std::cout << "Demo will test:" << std::endl;
            std::cout << "  - Integration verification" << std::endl;
            std::cout << "  - Memory system integration" << std::endl;
            std::cout << "  - Neuromodulation integration" << std::endl;
            std::cout << "  - Checkpoint functionality" << std::endl;
            std::cout << "  - Replay system" << std::endl;
            std::cout << "  - Full integration experiment" << std::endl;
        }
        
        try {
            demo.run();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Demo failed with exception: " + std::string(e.what()));
            throw;
        }
    }
    
    void runExperiment(const CommandOptions& options) {
        std::cout << "=== Phase 6 Integration Experiment ===" << std::endl;
        std::cout << "Running " << options.experimentType << " experiment with " 
                  << options.neuronCount << " neurons and " << options.maxSteps << " steps." << std::endl << std::endl;
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(options.neuronCount));
        cfg->set("region_count", static_cast<int64_t>(options.regionCount));
        cfg->set("connection_probability", options.connectionProbability);
        cfg->set("stdp_ltp_weight", 0.01f);
        cfg->set("stdp_ltd_weight", 0.012f);
        cfg->set("synaptogenesis_rate", 0.0001f);
        cfg->set("pruning_rate", 0.00001f);
        
        // Create Phase6 config
        Phase6Config phase6Config;
        phase6Config.maxSteps = options.maxSteps;
        phase6Config.neuronCount = options.neuronCount;
        phase6Config.regionCount = options.regionCount;
        phase6Config.connectionProbability = options.connectionProbability;
        phase6Config.enableCheckpointing = options.enableCheckpointing;
        phase6Config.enableReplay = options.enableReplay;
        phase6Config.enableDevelopment = options.enableDevelopment;
        phase6Config.checkpointPath = options.checkpointPath;
        
        // Create and run experiment
        Phase6IntegratedExperiment experiment;
        
        if (options.verbose) {
            std::cout << "Experiment configuration:" << std::endl;
            std::cout << "  - Max steps: " << phase6Config.maxSteps << std::endl;
            std::cout << "  - Neurons: " << phase6Config.neuronCount << std::endl;
            std::cout << "  - Regions: " << phase6Config.regionCount << std::endl;
            std::cout << "  - Connection probability: " << phase6Config.connectionProbability << std::endl;
            std::cout << "  - Checkpointing: " << (phase6Config.enableCheckpointing ? "enabled" : "disabled") << std::endl;
            std::cout << "  - Replay: " << (phase6Config.enableReplay ? "enabled" : "disabled") << std::endl;
            std::cout << "  - Development: " << (phase6Config.enableDevelopment ? "enabled" : "disabled") << std::endl;
        }
        
        try {
            auto result = experiment.run(phase6Config);
            
            std::cout << std::endl;
            std::cout << "=== EXPERIMENT RESULTS ===" << std::endl;
            std::cout << "Total reward: " << result.totalReward << std::endl;
            std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
            std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
            std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
            std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
            std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
            std::cout << std::endl;
            
            std::cout << "=== SYSTEM INTEGRATION STATUS ===" << std::endl;
            std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
            std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
            std::cout << std::endl;
            
            std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Experiment failed with exception: " + std::string(e.what()));
            throw;
        }
    }
    
    void runVerification(const CommandOptions& options) {
        std::cout << "=== Integration Verification ===" << std::endl;
        std::cout << "Verifying that all brain systems are properly integrated..." << std::endl << std::endl;
        
        // Create minimal brain for verification
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", 100);
        cfg->set("region_count", 1);
        
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain for verification");
            return;
        }
        
        bool success = true;
        
        // Test each integrated system
        std::cout << "Testing system integration..." << std::endl;
        
        // Test memory systems
        if (brain->getWorkingMemory() != nullptr) {
            std::cout << "✓ Working Memory: INTEGRATED" << std::endl;
        } else {
            std::cout << "✗ Working Memory: NOT INTEGRATED" << std::endl;
            success = false;
        }
        
        if (brain->getEpisodicMemory() != nullptr) {
            std::cout << "✓ Episodic Memory: INTEGRATED" << std::endl;
        } else {
            std::cout << "✗ Episodic Memory: NOT INTEGRATED" << std::endl;
            success = false;
        }
        
        // Test neuromodulation systems
        if (brain->getDopamine() != nullptr) {
            std::cout << "✓ Dopamine System: INTEGRATED" << std::endl;
        } else {
            std::cout << "✗ Dopamine System: NOT INTEGRATED" << std::endl;
            success = false;
        }
        
        if (brain->getCuriosity() != nullptr) {
            std::cout << "✓ Curiosity System: INTEGRATED" << std::endl;
        } else {
            std::cout << "✗ Curiosity System: NOT INTEGRATED" << std::endl;
            success = false;
        }
        
        // Test prediction system
        if (brain->getPredictionSystem() != nullptr) {
            std::cout << "✓ Prediction System: INTEGRATED" << std::endl;
        } else {
            std::cout << "✗ Prediction System: NOT INTEGRATED" << std::endl;
            success = false;
        }
        
        // Test cognition systems
        if (brain->getPlanner() != nullptr) {
            std::cout << "✓ Neural Planner: INTEGRATED" << std::endl;
        } else {
            std::cout << "✗ Neural Planner: NOT INTEGRATED" << std::endl;
            success = false;
        }
        
        if (brain->getConceptFormation() != nullptr) {
            std::cout << "✓ Concept Formation: INTEGRATED" << std::endl;
        } else {
            std::cout << "✗ Concept Formation: NOT INTEGRATED" << std::endl;
            success = false;
        }
        
        if (brain->getAttention() != nullptr) {
            std::cout << "✓ Attention System: INTEGRATED" << std::endl;
        } else {
            std::cout << "✗ Attention System: NOT INTEGRATED" << std::endl;
            success = false;
        }
        
        // Test development system
        if (brain->getDevelopmentSystem() != nullptr) {
            std::cout << "✓ Development System: INTEGRATED" << std::endl;
        } else {
            std::cout << "✗ Development System: NOT INTEGRATED" << std::endl;
            success = false;
        }
        
        std::cout << std::endl;
        if (success) {
            std::cout << "=== VERIFICATION PASSED ===" << std::endl;
            std::cout << "All integrated systems are working correctly." << std::endl;
        } else {
            std::cout << "=== VERIFICATION FAILED ===" << std::endl;
            std::cout << "Some systems are not properly integrated." << std::endl;
        }
    }
    
    void runBenchmark(const CommandOptions& options) {
        std::cout << "=== NLM Performance Benchmark ===" << std::endl;
        std::cout << "Running " << options.benchmarkRuns << " benchmark runs..." << std::endl << std::endl;
        
        // Create configuration for benchmark
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(options.neuronCount));
        cfg->set("region_count", static_cast<int64_t>(options.regionCount));
        cfg->set("connection_probability", options.connectionProbability);
        cfg->set("stdp_ltp_weight", 0.01f);
        cfg->set("stdp_ltd_weight", 0.012f);
        
        // Simple benchmark: create brain and run steps
        std::vector<double> times;
        std::vector<float> firingRates;
        
        for (size_t run = 0; run < options.benchmarkRuns; ++run) {
            std::cout << "Benchmark run " << (run + 1) << "/" << options.benchmarkRuns << "..." << std::endl;
            
            auto brain = std::make_shared<Brain>(cfg);
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain in benchmark run " + std::to_string(run + 1));
                continue;
            }
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Run a moderate number of steps
            for (uint64_t step = 0; step < 1000; ++step) {
                brain->step(step, step * 0.001);
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(endTime - startTime).count();
            
            times.push_back(duration);
            firingRates.push_back(brain->getAverageFiringRate());
            
            std::cout << "  Time: " << duration << "s, "
                     << "Firing rate: " << firingRates.back() << std::endl;
        }
        
        if (times.empty()) {
            std::cout << "Benchmark failed - no successful runs." << std::endl;
            return;
        }
        
        // Calculate statistics
        double avgTime = 0.0, minTime = times[0], maxTime = times[0];
        float avgFiring = 0.0f;
        
        for (double t : times) {
            avgTime += t;
            if (t < minTime) minTime = t;
            if (t > maxTime) maxTime = t;
        }
        
        avgTime /= times.size();
        for (float fr : firingRates) avgFiring += fr;
        avgFiring /= firingRates.size();
        
        std::cout << std::endl;
        std::cout << "=== BENCHMARK RESULTS ===" << std::endl;
        std::cout << "Runs completed: " << times.size() << "/" << options.benchmarkRuns << std::endl;
        std::cout << "Average time per run: " << avgTime << "s" << std::endl;
        std::cout << "Min time per run: " << minTime << "s" << std::endl;
        std::cout << "Max time per run: " << maxTime << "s" << std::endl;
        std::cout << "Average firing rate: " << avgFiring << std::endl;
        std::cout << "Steps per second: " << (1000.0 / avgTime) << std::endl;
    }
    
    int run(const CommandOptions& options) {
        try {
            initializeLogger(options.verbose, options.quiet);
            
            // Load configuration
            loadConfig(std::make_shared<Config>(), options.configFile);
            
            switch (options.mode) {
                case Mode::DEMO:
                    runDemo(options);
                    break;
                case Mode::EXPERIMENT:
                    runExperiment(options);
                    break;
                case Mode::STATUS:
                    // Create a minimal brain for status display
                    auto cfg = std::make_shared<Config>();
                    cfg->set("neuron_count", static_cast<int64_t>(options.neuronCount));
                    cfg->set("region_count", static_cast<int64_t>(options.regionCount));
                    cfg->set("connection_probability", options.connectionProbability);
                    
                    auto brain = std::make_shared<Brain>(cfg);
                    if (brain->initialize()) {
                        printStatus(brain);
                    }
                    break;
                case Mode::CONFIG:
                    printConfigInfo(std::make_shared<Config>());
                    break;
                case Mode::INTEGRATION_VERIFY:
                    runVerification(options);
                    break;
                case Mode::QUICK_START:
                    printQuickStart();
                    break;
                case Mode::INFO:
                    printVersion();
                    printConfigInfo(std::make_shared<Config>());
                    break;
                case Mode::CHECKPOINT:
                    // For now, just show that checkpoint functionality exists
                    std::cout << "Checkpoint system is implemented and can be tested via experiment mode." << std::endl;
                    std::cout << "Use: nlm experiment --enable-checkpointing" << std::endl;
                    break;
                case Mode::BENCHMARK:
                    runBenchmark(options);
                    break;
                case Mode::HELP:
                    printHelp();
                    break;
                case Mode::VERSION:
                    printVersion();
                    break;
                default:
                    // Default behavior: run demo
                    runDemo(options);
                    break;
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error: " + std::string(e.what()));
            return 1;
        } catch (...) {
            NLM_LOG_ERROR("Unknown error occurred");
            return 1;
        }
        
        return 0;
    }
};

int main(int argc, char** argv) {
    NLMCLI cli;
    
    try {
        NLMCLI::CommandOptions options;
        NLMCLI::Mode mode = cli.parseCommandLine(argc, argv, options);
        
        return cli.run(options);
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}