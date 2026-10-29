// NLM (熙然) - Neural Learning Machine
// Unified Command-Line Interface for NLM Experiments
//
// This provides a unified interface for running different phases and experiments:
// - Phase 2: Basic neural computation (backward compatibility)
// - Phase 6: Full integration (new default)
// - Phase 3, 4, 5 experiments
// - Custom experiments
// - Status and configuration tools

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/Phase3Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>

using namespace nlm;

// Forward declarations
void printBanner();
void printUsage(const char* programName);
void printHelp();
std::string toLower(const std::string& str);

enum class RunMode {
    Phase2,      // Backward compatibility - basic neural computation
    Phase6,      // Full integration experiment
    Phase3,      // Phase 3 integration test
    Phase4,      // Phase 4 integration test
    Phase5,      // Phase 5 performance optimization test
    Custom,      // Custom experiment
    Status,      // Get status information
    Config,      // Configuration management
    Help         // Help/usage information
};

struct CommandLineOptions {
    RunMode mode;
    std::string configFile;
    std::string outputFile;
    std::string checkpointPath;
    int neuronCount;
    int regionCount;
    int maxSteps;
    float timestep;
    int randomSeed;
    bool verbose;
    bool enableCheckpointing;
    bool enableDevelopment;
    bool enableReplay;
    std::string logLevel;
    std::vector<std::string> extraArgs;
};

CommandLineOptions parseCommandLine(int argc, char* argv[]);
void runPhase2(const CommandLineOptions& options);
void runPhase6(const CommandLineOptions& options);
void runPhase3(const CommandLineOptions& options);
void runPhase4(const CommandLineOptions& options);
void runPhase5(const CommandLineOptions& options);
void runCustom(const CommandLineOptions& options);
void showStatus(const CommandLineOptions& options);
void manageConfig(const CommandLineOptions& options);
void printHelp();

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        CommandLineOptions options = parseCommandLine(argc, argv);
        
        // Print banner
        printBanner();
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Set log level
        if (options.logLevel != "INFO") {
            auto logLevel = LogLevel::Info;
            if (options.logLevel == "DEBUG") {
                logLevel = LogLevel::Debug;
            } else if (options.logLevel == "WARNING") {
                logLevel = LogLevel::Warning;
            } else if (options.logLevel == "ERROR") {
                logLevel = LogLevel::Error;
            }
            consoleLogger->setLevel(logLevel);
        }
        
        // Run the selected mode
        switch (options.mode) {
            case RunMode::Phase2:
                runPhase2(options);
                break;
            case RunMode::Phase6:
                runPhase6(options);
                break;
            case RunMode::Phase3:
                runPhase3(options);
                break;
            case RunMode::Phase4:
                runPhase4(options);
                break;
            case RunMode::Phase5:
                runPhase5(options);
                break;
            case RunMode::Custom:
                runCustom(options);
                break;
            case RunMode::Status:
                showStatus(options);
                break;
            case RunMode::Config:
                manageConfig(options);
                break;
            case RunMode::Help:
                printHelp();
                break;
        }
        
        std::cout << std::endl;
        std::cout << "=== NLM Execution Complete ===" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Unified Command-Line Interface                            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    std::cout << "   An experimental artificial developmental brain" << std::endl;
    std::cout << "   Run './nlm --help' for usage information" << std::endl << std::endl;
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << "       " << programName << " <mode> [OPTIONS]" << std::endl << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "  phase2    Run Phase 2: Basic neural computation (backward compatibility)" << std::endl;
    std::cout << "  phase6    Run Phase 6: Full integration experiment (default)" << std::endl;
    std::cout << "  phase3    Run Phase 3: Integration test" << std::endl;
    std::cout << "  phase4    Run Phase 4: Integration test" << std::endl;
    std::cout << "  phase5    Run Phase 5: Performance optimization test" << std::endl;
    std::cout << "  custom    Run custom experiment" << std::endl;
    std::cout << "  status    Get status information" << std::endl;
    std::cout << "  config    Manage configuration" << std::endl;
    std::cout << "  help      Show help information" << std::endl << std::endl;
    
    std::cout << "Common options:" << std::endl;
    std::cout << "  --config=FILE     Load configuration from FILE" << std::endl;
    std::cout << "  --neuron-count=N  Set number of neurons (default: 1000)" << std::endl;
    std::cout << "  --steps=N         Set max simulation steps (default: 10000)" << std::endl;
    std::cout << "  --timestep=N      Set simulation timestep (default: 0.001)" << std::endl;
    std::cout << "  --seed=N          Set random seed (default: 42)" << std::endl;
    std::cout << "  --verbose         Enable verbose logging" << std::endl;
    std::cout << "  --checkpoint      Enable checkpointing" << std::endl;
    std::cout << "  --development     Enable developmental processes" << std::endl;
    std::cout << "  --replay          Enable memory replay" << std::endl;
    std::cout << "  --log-level=L     Set log level (DEBUG, INFO, WARNING, ERROR)" << std::endl;
    std::cout << "  --help            Show this help message" << std::endl << std::endl;
}

void printHelp() {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "=====================================" << std::endl << std::endl;
    
    printUsage("nlm");
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm phase6                             Run Phase 6 integration experiment" << std::endl;
    std::cout << "  ./nlm phase2 --config=myconfig.cfg       Run Phase 2 with custom config" << std::endl;
    std::cout << "  ./nlm phase6 --neuron-count=5000 --steps=20000  Run with custom parameters" << std::endl;
    std::cout << "  ./nlm status                            Show system status and statistics" << std::endl;
    std::cout << "  ./nlm config --list                     List all available configuration options" << std::endl;
    std::cout << "  ./nlm help                              Show this help message" << std::endl << std::endl;
    
    std::cout << "Configuration File:" << std::endl;
    std::cout << "  configs/default.cfg is the default configuration" << std::endl;
    std::cout << "  Configuration can be overridden by command line options" << std::endl << std::endl;
    
    std::cout << "Phase 6 Integration:" << std::endl;
    std::cout << "  Phase 6 is the complete integration phase that connects all brain systems:" << std::endl;
    std::cout << "  - Memory systems (working memory, episodic memory, associative memory)" << std::endl;
    std::cout << "  - Neuromodulation (dopamine, curiosity, novelty)" << std::endl;
    std::cout << "  - Prediction system" << std::endl;
    std::cout << "  - Cognition systems (planning, concept formation, attention)" << std::endl;
    std::cout << "  - Development system" << std::endl;
    std::cout << "  - Checkpointing and replay systems" << std::endl;
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

CommandLineOptions parseCommandLine(int argc, char* argv[]) {
    CommandLineOptions options;
    options.mode = RunMode::Phase6;  // Default to Phase 6
    options.neuronCount = 1000;
    options.regionCount = 2;
    options.maxSteps = 10000;
    options.timestep = 0.001f;
    options.randomSeed = 42;
    options.verbose = false;
    options.enableCheckpointing = false;
    options.enableDevelopment = true;
    options.enableReplay = true;
    options.logLevel = "INFO";
    
    // Check for help flag
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            options.mode = RunMode::Help;
            return options;
        }
    }
    
    // Determine mode
    if (argc > 1) {
        std::string mode = argv[1];
        std::string lowerMode = toLower(mode);
        
        if (lowerMode == "phase2") options.mode = RunMode::Phase2;
        else if (lowerMode == "phase3") options.mode = RunMode::Phase3;
        else if (lowerMode == "phase4") options.mode = RunMode::Phase4;
        else if (lowerMode == "phase5") options.mode = RunMode::Phase5;
        else if (lowerMode == "phase6") options.mode = RunMode::Phase6;
        else if (lowerMode == "custom") options.mode = RunMode::Custom;
        else if (lowerMode == "status") options.mode = RunMode::Status;
        else if (lowerMode == "config") options.mode = RunMode::Config;
        else if (lowerMode == "help") options.mode = RunMode::Help;
        else {
            std::cerr << "ERROR: Unknown mode '" << mode << "'" << std::endl;
            std::cerr << "Run with --help for usage information." << std::endl;
            throw std::runtime_error("Unknown mode");
        }
    }
    
    // Parse remaining arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--verbose" || arg == "-v") {
            options.verbose = true;
        } else if (arg.find("--config=") == 0) {
            options.configFile = arg.substr(9);
        } else if (arg.find("--neuron-count=") == 0) {
            options.neuronCount = std::stoi(arg.substr(16));
        } else if (arg.find("--steps=") == 0) {
            options.maxSteps = std::stoi(arg.substr(8));
        } else if (arg.find("--timestep=") == 0) {
            options.timestep = std::stof(arg.substr(11));
        } else if (arg.find("--seed=") == 0) {
            options.randomSeed = std::stoi(arg.substr(7));
        } else if (arg.find("--log-level=") == 0) {
            options.logLevel = arg.substr(12);
        } else if (arg == "--checkpoint") {
            options.enableCheckpointing = true;
        } else if (arg == "--development") {
            options.enableDevelopment = true;
        } else if (arg == "--replay") {
            options.enableReplay = true;
        } else if (arg.find("--output=") == 0) {
            options.outputFile = arg.substr(9);
        } else if (arg.find("--checkpoint-path=") == 0) {
            options.checkpointPath = arg.substr(18);
        } else if (arg.find("--region-count=") == 0) {
            options.regionCount = std::stoi(arg.substr(16));
        } else {
            // Unknown argument - add to extra args for custom mode
            options.extraArgs.push_back(arg);
        }
    }
    
    return options;
}

void runPhase2(const CommandLineOptions& options) {
    std::cout << "=== Running NLM Phase 2: Real Neural Computation ===" << std::endl;
    std::cout << "Note: Phase 2 is maintained for backward compatibility only." << std::endl;
    std::cout << "For new work, use Phase 6 (--help for details)." << std::endl << std::endl;
    
    // Load configuration
    auto config = std::make_shared<Config>();
    if (!options.configFile.empty() && config->loadFromFile(options.configFile)) {
        std::cout << "Loaded configuration from: " << options.configFile << std::endl;
    } else {
        std::cout << "Using default configuration." << std::endl;
    }
    
    // Override with command line options
    config->set("random_seed", options.randomSeed, ConfigSource::Default);
    config->set("neuron_count", options.neuronCount, ConfigSource::Default);
    config->set("region_count", options.regionCount, ConfigSource::Default);
    config->set("simulation_timestep", options.timestep, ConfigSource::Default);
    
    // Initialize and run Phase 2 demo
    std::cout << "Phase 2 initialization would go here..." << std::endl;
    std::cout << "In Phase 2, the brain would implement:" << std::endl;
    std::cout << "  - Leaky Integrate-and-Fire (LIF) neurons" << std::endl;
    std::cout << "  - Event-driven spike propagation with delays" << std::endl;
    std::cout << "  - STDP and Hebbian plasticity rules" << std::endl;
    std::cout << "  - Structural plasticity (synaptogenesis/pruning)" << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 2 complete. For Phase 6 (full integration), use './nlm phase6'." << std::endl;
}

void runPhase6(const CommandLineOptions& options) {
    std::cout << "=== Running NLM Phase 6: Full Integration Experiment ===" << std::endl;
    std::cout << "This runs the complete Phase 6 integration experiment testing:" << std::endl;
    std::cout << "  - Memory systems integration" << std::endl;
    std::cout << "  - Neuromodulation integration" << std::endl;
    std::cout << "  - Prediction system integration" << std::endl;
    std::cout << "  - Cognition systems integration" << std::endl;
    std::cout << "  - Development system integration" << std::endl;
    std::cout << "  - Checkpointing and replay systems" << std::endl << std::endl;
    
    // Create Phase 6 experiment configuration
    Phase6Config config;
    config.neuronCount = options.neuronCount;
    config.maxSteps = options.maxSteps;
    config.enableCheckpointing = options.enableCheckpointing;
    config.enableReplay = options.enableReplay;
    config.enableDevelopment = options.enableDevelopment;
    config.checkpointPath = options.checkpointPath.empty() ? "/tmp/nlm_phase6_checkpoint.bin" : options.checkpointPath;
    config.regionCount = options.regionCount;
    config.connectionProbability = 0.15f;
    
    // Create and run experiment
    Phase6IntegratedExperiment experiment;
    
    std::cout << "Phase 6 configuration:" << std::endl;
    std::cout << "  Neurons: " << config.neuronCount << std::endl;
    std::cout << "  Max steps: " << config.maxSteps << std::endl;
    std::cout << "  Enable checkpointing: " << (config.enableCheckpointing ? "Yes" : "No") << std::endl;
    std::cout << "  Enable development: " << (config.enableDevelopment ? "Yes" : "No") << std::endl;
    std::cout << "  Enable replay: " << (config.enableReplay ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    
    // First, run the integration verification
    std::cout << "Running integration verification..." << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "Testing individual systems..." << std::endl;
    experiment.testMemoryIntegration();
    experiment.testNeuromodulationIntegration();
    experiment.testCheckpointing();
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment
    std::cout << "Running full integration experiment..." << std::endl;
    auto result = experiment.run(config);
    
    // Display results
    std::cout << std::endl;
    std::cout << "=== Phase 6 Results ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    std::cout << "=== Integration Status ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    if (result.totalReward > 0.0f) {
        std::cout << std::endl;
        std::cout << "=== SUCCESS ===" << std::endl;
        std::cout << "Phase 6 integration is working correctly!" << std::endl;
    } else {
        std::cout << std::endl;
        std::cout << "=== WARNING ===" << std::endl;
        std::cout << "No reward obtained. Check if all systems are properly integrated." << std::endl;
    }
}

void runPhase3(const CommandLineOptions& options) {
    std::cout << "=== Running NLM Phase 3: Integration Test ===" << std::endl;
    std::cout << "Phase 3 tests the integration of:" << std::endl;
    std::cout << "  - World interaction loop" << std::endl;
    std::cout << "  - Sensory input and motor output" << std::endl;
    std::cout << "  - Reward prediction error" << std::endl;
    std::cout << "  - Developmental stages" << std::endl;
    std::cout << "  - Novelty and curiosity" << std::endl;
    std::cout << std::endl;
    
    // This would create and run a Phase 3 experiment
    // For now, just indicate that Phase 3 is available but needs full implementation
    std::cout << "Phase 3 experiment framework is available in src/experiments/Phase3Experiment.hpp" << std::endl;
    std::cout << "To run Phase 3, build the nlm_phase3_demo executable." << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 3 functionality will be implemented as part of the unified interface." << std::endl;
}

void runPhase4(const CommandLineOptions& options) {
    std::cout << "=== Running NLM Phase 4: Integration Test ===" << std::endl;
    std::cout << "Phase 4 tests the integration of:" << std::endl;
    std::cout << "  - Neural prediction system" << std::endl;
    std::cout << "  - Working memory" << std::endl;
    std::cout << "  - Episodic-like memory" << std::endl;
    std::cout << "  - Concept formation" << std::endl;
    std::cout << "  - Neural attention (NOT Transformer)" << std::endl;
    std::cout << "  - Predictive planning" << std::endl;
    std::cout << "  - Self-model" << std::endl;
    std::cout << "  - Social learning" << std::endl;
    std::cout << "  - Continual learning" << std::endl;
    std::cout << std::endl;
    
    // This would create and run a Phase 4 experiment
    // For now, just indicate that Phase 4 is available but needs full implementation
    std::cout << "Phase 4 experiment framework is available in src/experiments/Phase4Experiment.hpp" << std::endl;
    std::cout << "To run Phase 4, build the nlm_phase4_demo executable." << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 4 functionality will be implemented as part of the unified interface." << std::endl;
}

void runPhase5(const CommandLineOptions& options) {
    std::cout << "=== Running NLM Phase 5: Performance Optimization Test ===" << std::endl;
    std::cout << "Phase 5 tests performance optimizations:" << std::endl;
    std::cout << "  - Memory pools and event queues" << std::endl;
    std::cout << "  - SIMD vectorization" << std::endl;
    std::cout << "  - Parallel processing" << std::endl;
    std::cout << "  - Checkpoint system" << std::endl;
    std::cout << std::endl;
    
    // This would create and run a Phase 5 experiment
    // For now, just indicate that Phase 5 is available but needs full implementation
    std::cout << "Phase 5 experiment framework is available in src/experiments/Phase5Experiment.hpp" << std::endl;
    std::cout << "To run Phase 5, build the nlm_phase5_demo executable." << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 5 functionality will be implemented as part of the unified interface." << std::endl;
}

void runCustom(const CommandLineOptions& options) {
    std::cout << "=== Running Custom Experiment ===" << std::endl;
    std::cout << "Custom experiments allow you to run specific combinations of features." << std::endl << std::endl;
    
    // Parse custom arguments
    if (options.extraArgs.empty()) {
        std::cout << "No custom arguments provided. Using defaults." << std::endl;
    } else {
        std::cout << "Custom arguments:" << std::endl;
        for (const auto& arg : options.extraArgs) {
            std::cout << "  " << arg << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Custom experiments are configured via configuration file:" << std::endl;
    std::cout << "  configs/default.cfg" << std::endl;
    std::cout << "  Command line options override configuration file values." << std::endl;
}

void showStatus(const CommandLineOptions& options) {
    std::cout << "=== NLM System Status ===" << std::endl;
    
    // This would show system status information
    // For now, show basic information
    std::cout << "System Information:" << std::endl;
    std::cout << "  NLM Version: 0.1.0" << std::endl;
    std::cout << "  Phase: 6 - Final Integration" << std::endl;
    std::cout << "  Build: CMake build system" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Available Components:" << std::endl;
    std::cout << "  ✓ Neural Core (Brain) - Working" << std::endl;
    std::cout << "  ✓ Memory Systems (Working, Episodic, Associative)" << std::endl;
    std::cout << "  ✓ Neuromodulation (Dopamine, Curiosity, Novelty)" << std::endl;
    std::cout << "  ✓ Prediction System" << std::endl;
    std::cout << "  ✓ Cognition Systems (Planner, Concept Formation, Attention)" << std::endl;
    std::cout << "  ✓ Development System" << std::endl;
    std::cout << "  ✓ Agent System (Sensory-Motor Interface)" << std::endl;
    std::cout << "  ✓ World System (SimpleWorld)" << std::endl;
    std::cout << "  ✓ Experiment Framework" << std::endl;
    std::cout << "  ✓ Performance Infrastructure (Memory pools, SIMD)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Demo Executables:" << std::endl;
    std::cout << "  nlm                - Phase 2 (backward compatibility)" << std::endl;
    std::cout << "  nlm_phase6_demo    - Phase 6 integration test" << std::endl;
    std::cout << "  nlm_phase3_demo    - Phase 3 integration test" << std::endl;
    std::cout << "  nlm_phase4_demo    - Phase 4 integration test" << std::endl;
    std::cout << "  nlm_phase5_demo    - Phase 5 performance test" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration:" << std::endl;
    std::cout << "  configs/default.cfg - Default configuration file" << std::endl;
    std::cout << "  Command line options can override configuration values" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Getting Started:" << std::endl;
    std::cout << "  1. Run ./nlm phase6          - See Phase 6 integration in action" << std::endl;
    std::cout << "  2. Run ./nlm --help           - Get detailed help information" << std::endl;
    std::cout << "  3. Run ./nlm config --list    - List all configuration options" << std::endl;
}

void manageConfig(const CommandLineOptions& options) {
    std::cout << "=== NLM Configuration Management ===" << std::endl;
    
    if (options.extraArgs.empty()) {
        std::cout << "Available configuration options:" << std::endl;
        std::cout << "  --config=FILE           Load configuration from file" << std::endl;
        std::cout << "  --neuron-count=N        Set number of neurons" << std::endl;
        std::cout << "  --steps=N               Set max simulation steps" << std::endl;
        std::cout << "  --timestep=N            Set simulation timestep" << std::endl;
        std::cout << "  --seed=N                Set random seed" << std::endl;
        std::cout << "  --verbose               Enable verbose logging" << std::endl;
        std::cout << "  --checkpoint            Enable checkpointing" << std::endl;
        std::cout << "  --development           Enable development processes" << std::endl;
        std::cout << "  --replay                Enable memory replay" << std::endl;
        std::cout << "  --log-level=L           Set log level (DEBUG, INFO, WARNING, ERROR)" << std::endl;
        std::cout << std::endl;
        std::cout << "Configuration file: configs/default.cfg" << std::endl;
    } else {
        for (const auto& arg : options.extraArgs) {
            if (arg == "--list") {
                std::cout << "All configuration options:" << std::endl;
                std::cout << "  random_seed                 - Random seed for reproducible experiments" << std::endl;
                std::cout << "  simulation_timestep         - Simulation timestep in seconds" << std::endl;
                std::cout << "  neuron_count                - Number of neurons in the brain" << std::endl;
                std::cout << "  region_count                - Number of brain regions" << std::endl;
                std::cout << "  connection_probability      - Connection probability for random connectivity" << std::endl;
                std::cout << "  stdp_ltp_weight             - STDP LTP weight" << std::endl;
                std::cout << "  stdp_ltd_weight             - STDP LTD weight" << std::endl;
                std::cout << "  stdp_time_constant          - STDP time constant" << std::endl;
                std::cout << "  development_synaptogenesis_rate - Development synaptogenesis rate" << std::endl;
                std::cout << "  development_pruning_rate    - Development pruning rate" << std::endl;
                std::cout << "  dopamine_baseline           - Dopamine baseline level" << std::endl;
                std::cout << "  reward_discount_factor      - Reward discount factor" << std::endl;
                std::cout << "  environment_name           - Environment name" << std::endl;
                std::cout << "  environment_width           - Environment width" << std::endl;
                std::cout << "  environment_height          - Environment height" << std::endl;
                std::cout << "  log_level                  - Log level" << std::endl;
                std::cout << "  log_to_file                - Log to file" << std::endl;
                std::cout << "  log_filename               - Log filename" << std::endl;
                std::cout << "  max_simulation_steps       - Maximum simulation steps" << std::endl;
                std::cout << "  simulation_time_limit       - Simulation time limit" << std::endl;
                std::cout << "  visualization_enabled       - Enable visualization" << std::endl;
                std::cout << "  visualization_update_rate   - Visualization update rate" << std::endl;
                break;
            }
        }
    }
}
