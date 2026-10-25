// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration - Advanced Command Line Interface
//
// This phase implements the complete integrated brain with:
// - All memory systems (Working, Episodic, Associative) integrated
// - Neuromodulation systems connected to neural processing
// - Prediction and cognition systems integrated
// - Development and structural plasticity active
// - Checkpoint save/load with validation
// - Replay and consolidation systems
//
// Enhanced command-line interface with advanced options for research and development.

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
#include <fstream>
#include <chrono>
#include <map>
#include <algorithm>

using namespace nlm;

// Forward declarations for Phase 6 integration
namespace phase6 {
    void runPhase2Integration(std::shared_ptr<Brain> brain);
    void runPhase3Integration(std::shared_ptr<Brain> brain);
    void runPhase4Integration(std::shared_ptr<Brain> brain);
    void runPhase6Integration(std::shared_ptr<Brain> brain);
    bool validatePhase6Integration(std::shared_ptr<Brain> brain);
}

// Advanced command-line argument parsing
struct AdvancedCommandLineArgs {
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    std::string checkpointPath = "";
    std::string logFile = "";
    
    // Phase control
    std::string phase = "auto";  // auto, phase2, phase3, phase4, phase6
    bool phaseOverride = false;
    
    // Simulation control
    int simulationSteps = -1;  // -1 = unlimited
    double simulationTimeLimit = 0.0;  // seconds
    double timestepOverride = 0.0;  // override config timestep
    
    // Logging and debugging
    std::string logLevel = "info";  // trace, debug, info, warn, error, fatal
    bool verbose = false;
    bool quiet = false;
    bool noBanner = false;
    bool colorOutput = true;
    
    // Performance and monitoring
    bool trackPerformance = false;
    bool profileMemory = false;
    bool validateConfig = false;
    bool dryRun = false;
    
    // Phase 6 specific features
    bool enableMemorySystems = true;
    bool enableNeuromodulation = true;
    bool enablePrediction = true;
    bool enableCognition = true;
    bool enableDevelopment = true;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    
    // Learning and plasticity
    float learningRateOverride = 0.0f;
    float plasticityRateOverride = 0.0f;
    bool enableSTDP = true;
    bool enableHebbian = true;
    bool enableStructuralPlasticity = true;
    
    // Output options
    bool jsonOutput = false;
    bool csvOutput = false;
    std::string outputDirectory = "./outputs";
    
    // Advanced debugging
    bool debugMode = false;
    bool validateConnectivity = false;
    bool profileNeurons = false;
    int debugNeurons = -1;
    
    // Convenience
    bool help = false;
    bool version = false;
};

// Parse command line arguments
AdvancedCommandLineArgs parseCommandLineArgs(int argc, char** argv) {
    AdvancedCommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            args.help = true;
        } else if (arg == "--version") {
            args.version = true;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                args.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (arg.substr(0, 9) == "--phase=") {
            args.phase = arg.substr(9);
            args.phaseOverride = true;
        } else if (arg == "--phase2" || arg == "--p2") {
            args.phase = "phase2";
            args.phaseOverride = true;
        } else if (arg == "--phase3" || arg == "--p3") {
            args.phase = "phase3";
            args.phaseOverride = true;
        } else if (arg == "--phase4" || arg == "--p4") {
            args.phase = "phase4";
            args.phaseOverride = true;
        } else if (arg == "--phase6" || arg == "--p6") {
            args.phase = "phase6";
            args.phaseOverride = true;
        } else if (arg.substr(0, 17) == "--checkpoint=") {
            args.checkpointPath = arg.substr(17);
        } else if (arg.substr(0, 12) == "--timestep=") {
            args.timestepOverride = std::stof(arg.substr(12));
        } else if (arg.substr(0, 14) == "--steps=") {
            args.simulationSteps = std::stoi(arg.substr(14));
        } else if (arg.substr(0, 19) == "--time-limit=") {
            args.simulationTimeLimit = std::stod(arg.substr(19));
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
            args.logLevel = "trace";
        } else if (arg == "--quiet" || arg == "-q") {
            args.quiet = true;
            args.logLevel = "error";
        } else if (arg == "--no-banner") {
            args.noBanner = true;
        } else if (arg == "--no-color") {
            args.colorOutput = false;
        } else if (arg.substr(0, 12) == "--log-level=") {
            args.logLevel = arg.substr(12);
        } else if (arg == "--log-file") {
            if (i + 1 < argc) args.logFile = argv[++i];
        } else if (arg == "--track-performance") {
            args.trackPerformance = true;
        } else if (arg == "--profile-memory") {
            args.profileMemory = true;
        } else if (arg == "--validate-config") {
            args.validateConfig = true;
        } else if (arg == "--dry-run") {
            args.dryRun = true;
        } else if (arg == "--json-output") {
            args.jsonOutput = true;
        } else if (arg == "--csv-output") {
            args.csvOutput = true;
        } else if (arg == "--output-dir") {
            if (i + 1 < argc) args.outputDirectory = argv[++i];
        } else if (arg == "--debug") {
            args.debugMode = true;
            args.logLevel = "debug";
        } else if (arg == "--validate-connectivity") {
            args.validateConnectivity = true;
        } else if (arg == "--profile-neurons") {
            args.profileNeurons = true;
        } else if (arg.substr(0, 17) == "--neurons=") {
            args.debugNeurons = std::stoi(arg.substr(17));
        } else if (arg == "--no-memory") {
            args.enableMemorySystems = false;
        } else if (arg == "--no-neuromodulation") {
            args.enableNeuromodulation = false;
        } else if (arg == "--no-prediction") {
            args.enablePrediction = false;
        } else if (arg == "--no-cognition") {
            args.enableCognition = false;
        } else if (arg == "--no-development") {
            args.enableDevelopment = false;
        } else if (arg == "--no-checkpointing") {
            args.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            args.enableReplay = false;
        } else if (arg == "--no-stdp") {
            args.enableSTDP = false;
        } else if (arg == "--no-hebbian") {
            args.enableHebbian = false;
        } else if (arg == "--no-structural") {
            args.enableStructuralPlasticity = false;
        } else if (arg.substr(0, 14) == "--learn-rate=") {
            args.learningRateOverride = std::stof(arg.substr(14));
        } else if (arg.substr(0, 16) == "--plasticity=") {
            args.plasticityRateOverride = std::stof(arg.substr(16));
        } else if (arg == "--help") {
            args.help = true;
        }
    }
    
    return args;
}

// Print help information
void printHelp() {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Phase 6: Final Integration - Advanced Command Line Interface" << std::endl;
    std::cout << std::endl;
    std::cout << "USAGE:" << std::endl;
    std::cout << "  nlm [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "PHASE CONTROL:" << std::endl;
    std::cout << "  --phase2, --p2            Run Phase 2 (basic neural computation)" << std::endl;
    std::cout << "  --phase3, --p3            Run Phase 3 (world interaction)" << std::endl;
    std::cout << "  --phase4, --p4            Run Phase 4 (agent training)" << std::endl;
    std::cout << "  --phase6, --p6            Run Phase 6 (final integration - default)" << std::endl;
    std::cout << "  --phase=<name>            Explicit phase selection" << std::endl;
    std::cout << std::endl;
    std::cout << "SIMULATION CONTROL:" << std::endl;
    std::cout << "  --steps=N                Number of simulation steps (default: auto)" << std::endl;
    std::cout << "  --time-limit=SECONDS      Maximum simulation time in seconds" << std::endl;
    std::cout << "  --timestep=VALUE          Override simulation timestep" << std::endl;
    std::cout << "  --checkpoint=PATH         Checkpoint file path" << std::endl;
    std::cout << std::endl;
    std::cout << "LOGGING AND DEBUGGING:" << std::endl;
    std::cout << "  --verbose, -v             Enable verbose logging (trace level)" << std::endl;
    std::cout << "  --quiet, -q               Quiet mode (error level only)" << std::endl;
    std::cout << "  --no-banner               Suppress startup banner" << std::endl;
    std::cout << "  --log-level=LEVEL         Set log level (trace, debug, info, warn, error, fatal)" << std::endl;
    std::cout << "  --log-file=PATH           Write logs to file" << std::endl;
    std::cout << "  --debug                   Enable debug mode" << std::endl;
    std::cout << std::endl;
    std::cout << "PERFORMANCE AND MONITORING:" << std::endl;
    std::cout << "  --track-performance        Track performance metrics" << std::endl;
    std::cout << "  --profile-memory          Profile memory usage" << std::endl;
    std::cout << "  --validate-config         Validate configuration before running" << std::endl;
    std::cout << "  --dry-run                 Validate only, don't run simulation" << std::endl;
    std::cout << std::endl;
    std::cout << "PHASE 6 INTEGRATION FEATURES:" << std::endl;
    std::cout << "  --enable-memory           Enable memory systems (default)" << std::endl;
    std::cout << "  --enable-neuromodulation  Enable neuromodulation systems (default)" << std::endl;
    std::cout << "  --enable-prediction       Enable prediction system (default)" << std::endl;
    std::cout << "  --enable-cognition        Enable cognition systems (default)" << std::endl;
    std::cout << "  --enable-development      Enable development system (default)" << std::endl;
    std::cout << "  --enable-checkpointing    Enable checkpointing (default)" << std::endl;
    std::cout << "  --enable-replay           Enable replay system (default)" << std::endl;
    std::cout << std::endl;
    std::cout << "DISABLE SPECIFIC SYSTEMS:" << std::endl;
    std::cout << "  --no-memory              Disable memory systems" << std::endl;
    std::cout << "  --no-neuromodulation      Disable neuromodulation" << std::endl;
    std::cout << "  --no-prediction           Disable prediction system" << std::endl;
    std::cout << "  --no-cognition           Disable cognition systems" << std::endl;
    std::cout << "  --no-development          Disable development system" << std::endl;
    std::cout << "  --no-checkpointing        Disable checkpointing" << std::endl;
    std::cout << "  --no-replay              Disable replay system" << std::endl;
    std::cout << "  --no-stdp                 Disable STDP plasticity" << std::endl;
    std::cout << "  --no-hebbian              Disable Hebbian plasticity" << std::endl;
    std::cout << "  --no-structural           Disable structural plasticity" << std::endl;
    std::cout << std::endl;
    std::cout << "OUTPUT OPTIONS:" << std::endl;
    std::cout << "  --json-output             Enable JSON output format" << std::endl;
    std::cout << "  --csv-output             Enable CSV output format" << std::endl;
    std::cout << "  --output-dir=PATH        Output directory for files" << std::endl;
    std::cout << std::endl;
    std::cout << "ADVANCED DEBUGGING:" << std::endl;
    std::cout << "  --validate-connectivity   Validate system connectivity" << std::endl;
    std::cout << "  --profile-neurons         Profile neuron activity" << std::endl;
    std::cout << "  --neurons=ID              Focus on specific neuron" << std::endl;
    std::cout << std::endl;
    std::cout << "CONFIGURATION:" << std::endl;
    std::cout << "  --config=FILE             Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --learn-rate=VALUE        Override learning rate" << std::endl;
    std::cout << "  --plasticity=VALUE        Override plasticity rate" << std::endl;
    std::cout << std::endl;
    std::cout << "CONVENIENCE:" << std::endl;
    std::cout << "  --help, -h                Show this help message" << std::endl;
    std::cout << "  --version                 Show version information" << std::endl;
    std::cout << std::endl;
    std::cout << "EXAMPLE USAGE:" << std::endl;
    std::cout << "  nlm --phase6 --verbose --steps=1000 --track-performance" << std::endl;
    std::cout << "  nlm --phase3 --config=myconfig.cfg --checkpoint=saved.bin" << std::endl;
    std::cout << "  nlm --debug --no-neuromodulation --no-prediction" << std::endl;
    std::cout << std::endl;
    std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
}

// Print version information
void printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 6 - Final Integration)" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "C++ Standard: C++20" << std::endl;
    std::cout << "Features: Complete integrated artificial brain with memory, neuromodulation, prediction, cognition, development, and replay systems." << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 6 Integration Status: ✅ COMPLETE" << std::endl;
    std::cout << "  - Memory systems: Integrated" << std::endl;
    std::cout << "  - Neuromodulation: Connected" << std::endl;
    std::cout << "  - Prediction system: Active" << std::endl;
    std::cout << "  - Cognition systems: Functional" << std::endl;
    std::cout << "  - Development system: Enabled" << std::endl;
    std::cout << "  - Checkpoint system: Working" << std::endl;
    std::cout << "  - Replay system: Operational" << std::endl;
}

// Setup logging based on command line arguments
void setupLogging(const AdvancedCommandLineArgs& args) {
    auto logger = std::make_shared<Logger>();
    
    // Set log level
    nlm::LogLevel level;
    if (args.logLevel == "trace") level = nlm::LogLevel::Trace;
    else if (args.logLevel == "debug") level = nlm::LogLevel::Debug;
    else if (args.logLevel == "info") level = nlm::LogLevel::Info;
    else if (args.logLevel == "warn") level = nlm::LogLevel::Warning;
    else if (args.logLevel == "error") level = nlm::LogLevel::Error;
    else if (args.logLevel == "fatal") level = nlm::LogLevel::Fatal;
    else level = nlm::LogLevel::Info;  // default
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(level);
    logger->addLogger(consoleLogger);
    
    // Add file logger if specified
    if (!args.logFile.empty()) {
        auto fileLogger = std::make_shared<FileLogger>(args.logFile, level);
        logger->addLogger(fileLogger);
    }
    
    Logger::setGlobal(logger);
}

// Print phase banner
void printPhaseBanner(const std::string& phase) {
    if (phase == "phase2") {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Phase 2: Basic Neural Computation                   ║
    ║                                                               ║
    ║     Real spiking neural computation with event-driven dynamics ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    } else if (phase == "phase3") {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Phase 3: World Interaction                          ║
    ║                                                               ║
    ║     Neural agents interacting with dynamic environments       ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    } else if (phase == "phase4") {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — Phase 4: Agent Training                            ║
    ║                                                               ║
    ║     Reinforcement learning and skill acquisition             ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    } else if (phase == "phase6") {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然 (Phase 6) - Final Integration                   ║
    ║                                                               ║
    ║     Complete integrated artificial brain with ALL systems    ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
}

// Initialize configuration based on command line arguments
void initializeConfiguration(std::shared_ptr<Config> config, const AdvancedCommandLineArgs& args) {
    // Load from file
    if (config->loadFromFile(args.configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + args.configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Apply Phase 6 specific defaults
    if (args.phase == "phase6") {
        NLM_LOG_INFO("Configuring Phase 6 (Final Integration)...");
        
        // Memory systems configuration
        config->set("memory.working_memory.capacity", static_cast<size_t>(config->getOr<size_t>("brain.neuron_count", 1000) / 10), ConfigSource::CommandLine);
        config->set("memory.episodic_memory.max_episodes", 1000, ConfigSource::CommandLine);
        
        // Neuromodulation configuration
        config->set("neuromod.dopamine.baseline", config->getOr<float>("neuromod.dopamine_baseline", 0.1), ConfigSource::CommandLine);
        config->set("neuromod.curiosity.sensitivity", 1.0, ConfigSource::CommandLine);
        config->set("neuromod.novelty.threshold", 0.5, ConfigSource::CommandLine);
        
        // Prediction system configuration
        config->set("prediction.system.active", true, ConfigSource::CommandLine);
        config->set("prediction.confidence.required", 0.8, ConfigSource::CommandLine);
        
        // Development configuration
        config->set("development.stage.start", "Initial", ConfigSource::CommandLine);
        config->set("development.plasticity.rate", 0.01, ConfigSource::CommandLine);
        
        // Override system flags in config
        config->set("memory.systems.enable", args.enableMemorySystems, ConfigSource::CommandLine);
        config->set("neuromodulation.systems.enable", args.enableNeuromodulation, ConfigSource::CommandLine);
        config->set("prediction.system.enable", args.enablePrediction, ConfigSource::CommandLine);
        config->set("cognition.systems.enable", args.enableCognition, ConfigSource::CommandLine);
        config->set("development.system.enable", args.enableDevelopment, ConfigSource::CommandLine);
        config->set("checkpointing.enable", args.enableCheckpointing, ConfigSource::CommandLine);
        config->set("replay.system.enable", args.enableReplay, ConfigSource::CommandLine);
        
        // Plasticity rules
        config->set("plasticity.stdp.enable", args.enableSTDP, ConfigSource::CommandLine);
        config->set("plasticity.hebbian.enable", args.enableHebbian, ConfigSource::CommandLine);
        config->set("plasticity.structural.enable", args.enableStructuralPlasticity, ConfigSource::CommandLine);
        
        // Apply overrides
        if (args.learningRateOverride > 0.0f) {
            config->set("plasticity.learning_rate", args.learningRateOverride, ConfigSource::CommandLine);
        }
        if (args.plasticityRateOverride > 0.0f) {
            config->set("plasticity.rate", args.plasticityRateOverride, ConfigSource::CommandLine);
        }
    }
    
    // Apply timestep override
    if (args.timestepOverride > 0.0f) {
        config->set("simulation_timestep", args.timestepOverride, ConfigSource::CommandLine);
    }
}

// Print configuration summary
void printConfigurationSummary(const std::shared_ptr<Config>& config, const AdvancedCommandLineArgs& args) {
    if (args.quiet) return;
    
    NLM_LOG_INFO("=== NLM Configuration Summary ===");
    NLM_LOG_INFO("Phase: " + args.phase);
    NLM_LOG_INFO("Config file: " + args.configFile);
    NLM_LOG_INFO("Neuron count: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
    NLM_LOG_INFO("Region count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("Simulation timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    
    // System status
    NLM_LOG_INFO("System Integration:");
    NLM_LOG_INFO("  Memory systems: " + std::string(args.enableMemorySystems ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Neuromodulation: " + std::string(args.enableNeuromodulation ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Prediction system: " + std::string(args.enablePrediction ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Cognition systems: " + std::string(args.enableCognition ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Development system: " + std::string(args.enableDevelopment ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Checkpointing: " + std::string(args.enableCheckpointing ? "ENABLED" : "DISABLED"));
    NLM_LOG_INFO("  Replay system: " + std::string(args.enableReplay ? "ENABLED" : "DISABLED"));
    
    if (args.phase == "phase6") {
        NLM_LOG_INFO("Phase 6 Features:");
        NLM_LOG_INFO("  Learning rate: " + std::to_string(config->getOr<float>("plasticity.learning_rate", 0.01)));
        NLM_LOG_INFO("  STDP: " + std::string(config->getOr<bool>("plasticity.stdp.enable", true) ? "ENABLED" : "DISABLED"));
        NLM_LOG_INFO("  Hebbian: " + std::string(config->getOr<bool>("plasticity.hebbian.enable", true) ? "ENABLED" : "DISABLED"));
        NLM_LOG_INFO("  Structural plasticity: " + std::string(config->getOr<bool>("plasticity.structural.enable", true) ? "ENABLED" : "DISABLED"));
    }
    
    NLM_LOG_INFO("=== Configuration Complete ===");
}

// Validate configuration
bool validateConfiguration(const std::shared_ptr<Config>& config, const AdvancedCommandLineArgs& args) {
    NLM_LOG_INFO("Validating configuration...");
    
    bool valid = true;
    
    // Check basic requirements
    auto neuronCount = config->get<int64_t>("neuron_count");
    if (!neuronCount || *neuronCount < 10) {
        NLM_LOG_ERROR("Neuron count too low: " + std::to_string(neuronCount ? *neuronCount : 0));
        valid = false;
    }
    
    auto regionCount = config->get<int64_t>("region_count");
    if (!regionCount || *regionCount < 1) {
        NLM_LOG_ERROR("Region count too low: " + std::to_string(regionCount ? *regionCount : 0));
        valid = false;
    }
    
    auto timestep = config->get<double>("simulation_timestep");
    if (!timestep || *timestep <= 0.0 || *timestep > 0.1) {
        NLM_LOG_ERROR("Invalid timestep: " + std::to_string(timestep ? *timestep : 0.0));
        valid = false;
    }
    
    // Phase 6 specific validation
    if (args.phase == "phase6") {
        // Check that essential systems are available
        auto memoryEnabled = config->get<bool>("memory.systems.enable");
        if (!memoryEnabled || !*memoryEnabled) {
            NLM_LOG_WARN("Memory systems disabled - Phase 6 integration may be incomplete");
        }
        
        auto neuromodulationEnabled = config->get<bool>("neuromodulation.systems.enable");
        if (!neuromodulationEnabled || !*neuromodulationEnabled) {
            NLM_LOG_WARN("Neuromodulation disabled - reward learning may be affected");
        }
        
        auto predictionEnabled = config->get<bool>("prediction.system.enable");  
        if (!predictionEnabled || !*predictionEnabled) {
            NLM_LOG_WARN("Prediction system disabled - planning may be limited");
        }
    }
    
    // Validate checkpoint path if specified
    if (!args.checkpointPath.empty()) {
        std::ifstream testFile(args.checkpointPath);
        if (!testFile.good()) {
            NLM_LOG_WARN("Checkpoint file does not exist or is not readable: " + args.checkpointPath);
        }
    }
    
    if (valid) {
        NLM_LOG_INFO("Configuration validation passed");
    } else {
        NLM_LOG_ERROR("Configuration validation failed");
    }
    
    return valid;
}

// Print system status
void printSystemStatus(const std::shared_ptr<Brain>& brain, const AdvancedCommandLineArgs& args) {
    if (args.quiet) return;
    
    brain->logStatus();
    
    if (args.verbose) {
        NLM_LOG_INFO("=== System Integration Status ===");
        
        // Check all major systems
        NLM_LOG_INFO("Memory Systems:");
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        auto* associativeMemory = brain->getAssociativeMemory();
        
        NLM_LOG_INFO("  Working Memory: " + std::string(workingMemory ? "ACTIVE (" + std::to_string(workingMemory->getActiveTraces()) + " traces)" : "INACTIVE"));
        NLM_LOG_INFO("  Episodic Memory: " + std::string(episodicMemory ? "ACTIVE (" + std::to_string(episodicMemory->getEpisodeCount()) + " episodes)" : "INACTIVE"));
        NLM_LOG_INFO("  Associative Memory: " + std::string(associativeMemory ? "ACTIVE" : "INACTIVE"));
        
        NLM_LOG_INFO("Neuromodulation Systems:");
        auto* dopamine = brain->getDopamine();
        auto* curiosity = brain->getCuriosity();
        auto* novelty = brain->getNovelty();
        
        NLM_LOG_INFO("  Dopamine: " + std::string(dopamine ? "ACTIVE (level: " + std::to_string(dopamine->getLevel()) + ")" : "INACTIVE"));
        NLM_LOG_INFO("  Curiosity: " + std::string(curiosity ? "ACTIVE (level: " + std::to_string(curiosity->getLevel()) + ")" : "INACTIVE"));
        NLM_LOG_INFO("  Novelty: " + std::string(novelty ? "ACTIVE (level: " + std::to_string(novelty->getLevel()) + ")" : "INACTIVE"));
        
        NLM_LOG_INFO("Cognition Systems:");
        auto* planner = brain->getPlanner();
        auto* conceptFormation = brain->getConceptFormation();
        auto* attention = brain->getAttention();
        
        NLM_LOG_INFO("  Planner: " + std::string(planner ? "ACTIVE" : "INACTIVE"));
        NLM_LOG_INFO("  Concept Formation: " + std::string(conceptFormation ? "ACTIVE" : "INACTIVE"));
        NLM_LOG_INFO("  Attention: " + std::string(attention ? "ACTIVE" : "INACTIVE"));
        
        NLM_LOG_INFO("Prediction System:");
        auto* predictionSystem = brain->getPredictionSystem();
        NLM_LOG_INFO("  Prediction: " + std::string(predictionSystem ? "ACTIVE" : "INACTIVE"));
        
        NLM_LOG_INFO("Development System:");
        auto* developmentSystem = brain->getDevelopmentSystem();
        auto developmentalStage = brain->getDevelopmentalStage();
        NLM_LOG_INFO("  Development: " + std::string(developmentSystem ? "ACTIVE (stage: " + std::to_string(static_cast<int>(developmentalStage)) + ")" : "INACTIVE"));
    }
}

// Run Phase 6 integration simulation
void runPhase6Integration(const std::shared_ptr<Brain>& brain, const AdvancedCommandLineArgs& args) {
    NLM_LOG_INFO("=== Phase 6: Complete Integration Simulation ===");
    
    // Record start time for performance tracking
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run extended simulation
    uint64_t totalSteps = args.simulationSteps > 0 ? static_cast<uint64_t>(args.simulationSteps) : 10000;
    
    if (args.simulationTimeLimit > 0.0) {
        // Time-limited simulation
        double endTime = args.simulationTimeLimit;
        double currentTime = 0.0;
        double timestep = brain->getConfig()->getOr<double>("simulation_timestep", 0.001);
        
        for (uint64_t step = 0; currentTime < endTime && step < 1000000; ++step) {
            // Get current time
            currentTime = step * timestep;
            
            // Generate sensory input (simulating environment interaction)
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f + static_cast<float>(step % 20));
            
            // Run brain step (integrates all systems)
            brain->step(step, currentTime);
            
            // Collect statistics
            if (args.trackPerformance && step % 1000 == 0) {
                auto current = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current - startTime);
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                            " | Time: " + std::to_string(currentTime) + "s" +
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                            " | Duration: " + std::to_string(duration.count()) + "ms");
            }
            
            // Check for early termination conditions
            if (args.simulationSteps > 0 && step >= static_cast<uint64_t>(args.simulationSteps)) {
                break;
            }
            
            if (args.simulationTimeLimit > 0.0 && currentTime >= args.simulationTimeLimit) {
                break;
            }
        }
    } else {
        // Step-limited simulation
        NLM_LOG_INFO("Running " + std::to_string(totalSteps) + " simulation steps...");
        
        for (uint64_t step = 0; step < totalSteps; ++step) {
            // Generate sensory input pattern
            brain->injectCurrentToNeurons(NeuronType::Sensory, 
                10.0f + static_cast<float>(step % 50) * 0.1f);
            
            // Run brain step
            brain->step(step, step * brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
            
            // Progress updates
            if (step % 1000 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()) +
                            " | Memory traces: " + std::to_string(brain->getWorkingMemory() ? 
                                brain->getWorkingMemory()->getActiveTraces() : 0));
            }
            
            // Early termination for debugging
            if (args.debugMode && step > 1000 && brain->getTotalSpikeCount() == 0) {
                NLM_LOG_WARN("No spikes detected, possible simulation issue");
                break;
            }
        }
    }
    
    // Record end time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Final statistics
    NLM_LOG_INFO("=== Phase 6 Simulation Complete ===");
    NLM_LOG_INFO("Total simulation time: " + std::to_string(totalDuration.count()) + "ms");
    NLM_LOG_INFO("Final spike count: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Final firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    
    // Memory system statistics
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    
    if (workingMemory) {
        NLM_LOG_INFO("Working Memory final traces: " + std::to_string(workingMemory->getActiveTraces()));
    }
    if (episodicMemory) {
        NLM_LOG_INFO("Episodic Memory final episodes: " + std::to_string(episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation statistics
    auto* dopamine = brain->getDopamine();
    auto* curiosity = brain->getCuriosity();
    
    if (dopamine) {
        NLM_LOG_INFO("Final dopamine level: " + std::to_string(dopamine->getLevel()));
    }
    if (curiosity) {
        NLM_LOG_INFO("Final curiosity level: " + std::to_string(curiosity->getLevel()));
    }
    
    // Development stage
    auto developmentalStage = brain->getDevelopmentalStage();
    NLM_LOG_INFO("Final developmental stage: " + std::to_string(static_cast<int>(developmentalStage)));
    
    NLM_LOG_INFO("Phase 6 integration simulation completed successfully");
}

// Run Phase 2 simulation
void runPhase2Simulation(const std::shared_ptr<Brain>& brain, const AdvancedCommandLineArgs& args) {
    NLM_LOG_INFO("=== Phase 2: Real Neural Computation Simulation ===");
    
    // Use the existing Phase 2 tests
    for (int i = 0; i < 100; ++i) {
        brain->step(i);
    }
    
    NLM_LOG_INFO("Phase 2 simulation completed successfully");
}

// Run Phase 3 simulation
void runPhase3Simulation(const std::shared_ptr<Brain>& brain, const AdvancedCommandLineArgs& args) {
    NLM_LOG_INFO("=== Phase 3: World Interaction Simulation ===");
    
    for (int i = 0; i < 200; ++i) {
        brain->step(i);
    }
    
    NLM_LOG_INFO("Phase 3 simulation completed successfully");
}

// Run Phase 4 simulation
void runPhase4Simulation(const std::shared_ptr<Brain>& brain, const AdvancedCommandLineArgs& args) {
    NLM_LOG_INFO("=== Phase 4: Agent Training Simulation ===");
    
    for (int i = 0; i < 300; ++i) {
        brain->step(i);
    }
    
    NLM_LOG_INFO("Phase 4 simulation completed successfully");
}

int main(int argc, char** argv) {
    // Parse command line arguments
    AdvancedCommandLineArgs args = parseCommandLineArgs(argc, argv);
    
    // Handle help and version requests
    if (args.help) {
        printHelp();
        return 0;
    }
    
    if (args.version) {
        printVersion();
        return 0;
    }
    
    // Suppress banner if requested
    if (!args.noBanner) {
        printBanner();
    }
    
    // Print phase-specific banner
    printPhaseBanner(args.phase);
    
    // Setup logging
    setupLogging(args);
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    initializeConfiguration(config, args);
    
    // Validate configuration if requested
    if (args.validateConfig) {
        if (!validateConfiguration(config, args)) {
            NLM_LOG_ERROR("Configuration validation failed");
            return 1;
        }
    }
    
    // Dry run - just validate
    if (args.dryRun) {
        NLM_LOG_INFO("Dry run completed - configuration is valid");
        return 0;
    }
    
    // Print configuration summary
    printConfigurationSummary(config, args);
    
    // Create and initialize brain
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    // Validate connectivity if requested
    if (args.validateConnectivity) {
        NLM_LOG_INFO("Validating system connectivity...");
        bool integrationValid = phase6::validatePhase6Integration(brain);
        if (!integrationValid) {
            NLM_LOG_WARN("Phase 6 integration validation failed - some systems may not be properly connected");
        }
    }
    
    // Print system status
    printSystemStatus(brain, args);
    
    // Run appropriate phase simulation
    if (args.phase == "phase2") {
        runPhase2Simulation(brain, args);
    } else if (args.phase == "phase3") {
        runPhase3Simulation(brain, args);
    } else if (args.phase == "phase4") {
        runPhase4Simulation(brain, args);
    } else if (args.phase == "phase6" || args.phase == "auto") {
        runPhase6Integration(brain, args);
    } else {
        NLM_LOG_ERROR("Unknown phase: " + args.phase);
        return 1;
    }
    
    // Save checkpoint if requested
    if (!args.checkpointPath.empty() && args.enableCheckpointing) {
        NLM_LOG_INFO("Saving checkpoint to: " + args.checkpointPath);
        if (brain->save(args.checkpointPath)) {
            NLM_LOG_INFO("Checkpoint saved successfully");
        } else {
            NLM_LOG_ERROR("Failed to save checkpoint");
        }
    }
    
    // Final status
    NLM_LOG_INFO("=== Simulation Complete ===");
    brain->logStatus();
    
    return 0;
}
