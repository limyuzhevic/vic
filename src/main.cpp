// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration and System Validation
//
// This phase implements the complete integrated artificial brain system:
// - All neural systems connected and functional
// - Memory systems integrated with neural processing
// - Neuromodulation affecting plasticity and dynamics
// - Development affecting plasticity rates
// - Prediction system integrated as central organizing principle
// - Replay and consolidation functional
// - Checkpoint save/load working
//
// Usage:
//   ./nlm --help                    Show help
//   ./nlm --demo                   Run Phase 6 integration demo
//   ./nlm --verify                 Run integration verification only
//   ./nlm --config /path/to/config   Load custom configuration
//   ./nlm --neurons N              Set neuron count
//   ./nlm --steps S                Set max simulation steps
//   ./nlm --verbose                 Enable verbose logging
//   ./nlm --quiet                   Suppress informational messages

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <chrono>

using namespace nlm;
namespace fs = std::filesystem;

// Forward declarations
void printBanner();
void printUsage(const std::string& programName);
void printVersion();
bool fileExists(const std::string& path);
std::string getTimestamp();

// CLI argument structure
struct CLIArgs {
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    std::string logFile;
    uint64_t neuronCount = 0;      // 0 means use config/default
    uint64_t maxSteps = 0;         // 0 means use config/default
    bool verbose = false;
    bool quiet = false;
    bool help = false;
    bool version = false;
    bool demo = false;             // Run Phase 6 demo
    bool verify = false;           // Run integration verification only
    bool validate = false;         // Validate configuration only
    bool benchmark = false;        // Run performance benchmark
    std::string checkpointPath;    // Custom checkpoint path
    bool enableDevelopment = true;
    bool enableCheckpointing = true;
    bool enableReplay = true;
};

CLIArgs parseCLIArgs(int argc, char** argv);
void applyCLIOverrides(CLIArgs& args, std::shared_ptr<Config> config);
void displayExperimentResults(const Phase6IntegrationResult& result);
void runPhase6Demo(const CLIArgs& args);
void runVerificationMode(const CLIArgs& args);
void runValidationMode(const CLIArgs& args);
void runBenchmarkMode(const CLIArgs& args);

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                               ║
    ║                                                               ║
    ║     Complete artificial brain integration test                ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "NLM Phase 6 - Final Integration Demo" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                  Show this help message" << std::endl;
    std::cout << "  --version               Show version information" << std::endl;
    std::cout << "  --demo                  Run Phase 6 integration demo (default)" << std::endl;
    std::cout << "  --verify                Run integration verification only" << std::endl;
    std::cout << "  --validate              Validate configuration only" << std::endl;
    std::cout << "  --benchmark             Run performance benchmark" << std::endl;
    std::cout << "  --config FILE           Load configuration from FILE" << std::endl;
    std::cout << "  --neurons N             Set neuron count (0 = use config)" << std::endl;
    std::cout << "  --steps S               Set max simulation steps (0 = use config)" << std::endl;
    std::cout << "  --checkpoint PATH       Set custom checkpoint path" << std::endl;
    std::cout << "  --no-checkpoint         Disable checkpointing" << std::endl;
    std::cout << "  --no-replay             Disable replay system" << std::endl;
    std::cout << "  --no-development        Disable development system" << std::endl;
    std::cout << "  --verbose               Enable verbose logging" << std::endl;
    std::cout << "  --quiet                 Suppress informational messages" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration file search order:" << std::endl;
    std::cout << "  1. Command line --config option" << std::endl;
    std::cout << "  2. 'configs/default.cfg'" << std::endl;
    std::cout << "  3. Built-in defaults" << std::endl;
}

void printVersion() {
    std::cout << "NLM Phase 6 Integration Demo" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Built: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Architecture: Experimental Artificial Developmental Brain" << std::endl;
}

bool fileExists(const std::string& path) {
    return fs::exists(path) && fs::is_regular_file(path);
}

std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

CLIArgs parseCLIArgs(int argc, char** argv) {
    CLIArgs args;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);

        if (arg == "--help") {
            args.help = true;
        } else if (arg == "--version") {
            args.version = true;
        } else if (arg == "--demo") {
            args.demo = true;
        } else if (arg == "--verify") {
            args.verify = true;
        } else if (arg == "--validate") {
            args.validate = true;
        } else if (arg == "--benchmark") {
            args.benchmark = true;
        } else if (arg == "--verbose") {
            args.verbose = true;
        } else if (arg == "--quiet") {
            args.quiet = true;
        } else if (arg == "--no-checkpoint") {
            args.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            args.enableReplay = false;
        } else if (arg == "--no-development") {
            args.enableDevelopment = false;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            } else {
                std::cerr << "Error: --config requires a file path" << std::endl;
                args.help = true;
            }
        } else if (arg == "--neurons") {
            if (i + 1 < argc) {
                try {
                    args.neuronCount = std::stoul(argv[++i]);
                } catch (...) {
                    std::cerr << "Error: Invalid neuron count" << std::endl;
                    args.help = true;
                }
            } else {
                std::cerr << "Error: --neurons requires a number" << std::endl;
                args.help = true;
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                try {
                    args.maxSteps = std::stoul(argv[++i]);
                } catch (...) {
                    std::cerr << "Error: Invalid step count" << std::endl;
                    args.help = true;
                }
            } else {
                std::cerr << "Error: --steps requires a number" << std::endl;
                args.help = true;
            }
        } else if (arg == "--checkpoint") {
            if (i + 1 < argc) {
                args.checkpointPath = argv[++i];
            } else {
                std::cerr << "Error: --checkpoint requires a path" << std::endl;
                args.help = true;
            }
        } else if (arg == "--log-file") {
            if (i + 1 < argc) {
                args.logFile = argv[++i];
            } else {
                std::cerr << "Error: --log-file requires a file path" << std::endl;
                args.help = true;
            }
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                args.outputFile = argv[++i];
            } else {
                std::cerr << "Error: --output requires a file path" << std::endl;
                args.help = true;
            }
        } else {
            // Unknown argument
            std::cerr << "Error: Unknown option: " << arg << std::endl;
            args.help = true;
        }
    }

    // Ensure at least one mode is selected
    if (!args.help && !args.version && !(args.demo || args.verify || args.validate || args.benchmark)) {
        args.demo = true; // Default to demo mode
    }

    // Handle conflicting modes
    int modeCount = (args.demo ? 1 : 0) + (args.verify ? 1 : 0) + (args.validate ? 1 : 0) + (args.benchmark ? 1 : 0);
    if (modeCount > 1) {
        std::cerr << "Error: Conflicting modes selected. Use only one of --demo, --verify, --validate, or --benchmark" << std::endl;
        args.help = true;
    }

    return args;
}

void applyCLIOverrides(CLIArgs& args, std::shared_ptr<Config> config) {
    if (args.neuronCount > 0) {
        config->set("neuron_count", static_cast<int64_t>(args.neuronCount), ConfigSource::CommandLine);
        NLM_LOG_INFO("Overridden neuron_count: " + std::to_string(args.neuronCount));
    }

    if (args.maxSteps > 0) {
        config->set("max_simulation_steps", static_cast<int64_t>(args.maxSteps), ConfigSource::CommandLine);
        NLM_LOG_INFO("Overridden max_simulation_steps: " + std::to_string(args.maxSteps));
    }

    if (!args.checkpointPath.empty()) {
        config->set("phase6_checkpoint_path", args.checkpointPath, ConfigSource::CommandLine);
        NLM_LOG_INFO("Overridden checkpoint path: " + args.checkpointPath);
    }

    if (args.enableCheckpointing) {
        config->set("phase6_enable_checkpointing", true, ConfigSource::CommandLine);
        NLM_LOG_INFO("Checkpointing: ENABLED");
    }

    if (args.enableReplay) {
        config->set("phase6_enable_replay", true, ConfigSource::CommandLine);
        NLM_LOG_INFO("Replay system: ENABLED");
    }

    if (args.enableDevelopment) {
        config->set("phase6_enable_development", true, ConfigSource::CommandLine);
        NLM_LOG_INFO("Development system: ENABLED");
    }
}

void displayExperimentResults(const Phase6IntegrationResult& result) {
    std::cout << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "           PHASE 6 INTEGRATION RESULTS" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << std::endl;

    std::cout << "System Integration Status:" << std::endl;
    std::cout << "  Working Memory:        " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory:       " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation:       " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Prediction System:     " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Development System:    " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Checkpointing:          " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    std::cout << "  Replay System:         " << (result.replayWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    std::cout << std::endl;

    std::cout << "Performance Metrics:" << std::endl;
    std::cout << "  Total Reward:          " << std::fixed << std::setprecision(4) << result.totalReward << std::endl;
    std::cout << "  Average Firing Rate:   " << std::fixed << std::setprecision(2) << result.avgFiringRate << std::endl;
    std::cout << "  Episodes Stored:       " << std::fixed << std::setprecision(2) << result.memoryEpisodesStored << std::endl;
    std::cout << "  Novelty Level:         " << std::fixed << std::setprecision(2) << result.noveltyLevel << std::endl;
    std::cout << "  Curiosity Level:       " << std::fixed << std::setprecision(2) << result.curiosityLevel << std::endl;
    std::cout << "  Dopamine Level:        " << std::fixed << std::setprecision(2) << result.dopamineLevel << std::endl;
    std::cout << std::endl;

    std::cout << "Timeline:" << std::endl;
    std::cout << "  Start Time:            " << ctime(&result.startTime);
    std::cout << "  End Time:              " << ctime(&result.endTime);
    std::cout << "  Total Wall Time:       " << std::fixed << std::setprecision(2) << result.totalWallClockTime << " seconds" << std::endl;
    std::cout << std::endl;

    if (result.memoryWorkingMemoryIntegrated && result.memoryEpisodicMemoryIntegrated &&
        result.neuromodulationIntegrated && result.predictionIntegrated &&
        result.developmentIntegrated && result.checkpointingWorks) {
        std::cout << "🎉 SUCCESS: All systems integrated and functional!" << std::endl;
    } else {
        std::cout << "❌ FAILURE: Some systems are not properly integrated." << std::endl;
    }
    std::cout << "==================================================" << std::endl;
}

void runPhase6Demo(const CLIArgs& args) {
    std::cout << std::endl;
    std::cout << "Starting Phase 6 Integration Demo..." << std::endl;
    std::cout << "Timestamp: " << getTimestamp() << std::endl;
    std::cout << std::endl;

    Phase6Config config;
    config.neuronCount = static_cast<size_t>(args.neuronCount > 0 ? args.neuronCount : 500);
    config.maxSteps = static_cast<size_t>(args.maxSteps > 0 ? args.maxSteps : 2000);
    config.enableCheckpointing = args.enableCheckpointing;
    config.enableReplay = args.enableReplay;
    config.enableDevelopment = args.enableDevelopment;

    if (!args.checkpointPath.empty()) {
        config.checkpointPath = args.checkpointPath;
    }

    Phase6IntegratedExperiment experiment;

    // Show integration verification first
    if (!args.quiet) {
        std::cout << "--- Phase 6 Integration Verification ---" << std::endl;
    }

    bool integrationOK = experiment.verifyIntegration();

    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return;
    }

    if (!args.quiet) {
        std::cout << "Integration verification passed!" << std::endl << std::endl;
    }

    // Test individual systems if verbose
    if (args.verbose) {
        std::cout << "--- Memory Integration Test ---" << std::endl;
        experiment.testMemoryIntegration();

        std::cout << std::endl;
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        experiment.testNeuromodulationIntegration();

        std::cout << std::endl;
        std::cout << "--- Checkpoint Test ---" << std::endl;
        experiment.testCheckpointing();

        std::cout << std::endl;
        std::cout << "--- Replay Test ---" << std::endl;
        experiment.testReplay();

        std::cout << std::endl;
    }

    // Run full experiment
    if (!args.quiet) {
        std::cout << "--- Running Full Integration Experiment ---" << std::endl;
        std::cout << "Configuration: " << config.neuronCount << " neurons, " << config.maxSteps << " steps" << std::endl;
        std::cout << "Checkpointing: " << (config.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Replay: " << (config.enableReplay ? "Enabled" : "Disabled") << std::endl;
        std::cout << "Development: " << (config.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
        std::cout << std::endl;
    }

    auto result = experiment.run(config);

    // Display results
    displayExperimentResults(result);
}

void runVerificationMode(const CLIArgs& args) {
    std::cout << "=== Phase 6 Integration Verification ===" << std::endl;
    std::cout << "Running quick system integration verification..." << std::endl << std::endl;

    Phase6IntegratedExperiment experiment;
    bool success = experiment.verifyIntegration();

    if (success) {
        std::cout << "✓ Integration verification PASSED" << std::endl;
        std::cout << "All systems are properly connected and functional." << std::endl;
    } else {
        std::cout << "✗ Integration verification FAILED" << std::endl;
        std::cout << "Some systems are not properly integrated." << std::endl;
    }
}

void runValidationMode(const CLIArgs& args) {
    std::cout << "=== Configuration Validation ===" << std::endl;

    // Validate configuration file
    if (!fileExists(args.configFile)) {
        std::cerr << "ERROR: Configuration file not found: " << args.configFile << std::endl;
        return;
    }

    std::cout << "Configuration file: " << args.configFile << std::endl;
    std::cout << "✓ File exists and is readable" << std::endl;

    // Basic parameter validation
    auto config = std::make_shared<Config>();
    if (config->loadFromFile(args.configFile)) {
        std::cout << "✓ Configuration loaded successfully" << std::endl;

        // Check critical parameters
        int64_t neurons = config->getOr<int64_t>("neuron_count", -1);
        if (neurons > 0) {
            std::cout << "✓ Valid neuron count: " << neurons << std::endl;
        } else {
            std::cerr << "✗ Invalid neuron count in configuration" << std::endl;
        }

        double timestep = config->getOr<double>("simulation_timestep", -1.0);
        if (timestep > 0.0) {
            std::cout << "✓ Valid simulation timestep: " << timestep << std::endl;
        } else {
            std::cerr << "✗ Invalid simulation timestep in configuration" << std::endl;
        }
    } else {
        std::cerr << "✗ Failed to load configuration file" << std::endl;
    }
}

void runBenchmarkMode(const CLIArgs& args) {
    std::cout << "=== Performance Benchmark ===" << std::endl;
    std::cout << "Running Phase 6 integration performance benchmark..." << std::endl << std::endl;

    Phase6Config config;
    config.neuronCount = 100;
    config.maxSteps = 500;
    config.enableCheckpointing = false;
    config.enableReplay = false;
    config.enableDevelopment = false;

    Phase6IntegratedExperiment experiment;

    // Warm-up run
    std::cout << "Warm-up run..." << std::endl;
    auto warmupResult = experiment.run(config);

    // Actual benchmark
    std::cout << "Benchmark run..." << std::endl;
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 3; ++i) {
        std::cout << "  Run " << (i + 1) << "/3..." << std::endl;
        experiment.run(config);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime);

    std::cout << std::endl;
    std::cout << "Benchmark Results:" << std::endl;
    std::cout << "  Total benchmark time: " << std::fixed << std::setprecision(2) << duration.count() << " seconds" << std::endl;
    std::cout << "  Average run time: " << std::fixed << std::setprecision(2) << duration.count() / 3.0 << " seconds" << std::endl;
    std::cout << "  Runs completed: 3" << std::endl;
    std::cout << "  Configuration: " << config.neuronCount << " neurons, " << config.maxSteps << " steps per run" << std::endl;
}

int main(int argc, char** argv) {
    // Parse command line arguments
    CLIArgs args = parseCLIArgs(argc, argv);

    // Handle help/version requests
    if (args.help) {
        printUsage(argv[0]);
        return 0;
    }

    if (args.version) {
        printVersion();
        return 0;
    }

    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>();

    // Set log level based on verbosity
    if (args.verbose) {
        consoleLogger->setLevel(LogLevel::Debug);
    } else if (args.quiet) {
        consoleLogger->setLevel(LogLevel::Warning);
    } else {
        consoleLogger->setLevel(LogLevel::Info);
    }

    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);

    // Set log file if specified
    if (!args.logFile.empty()) {
        auto fileLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(fileLogger);
        NLM_LOG_INFO("Log file: " + args.logFile);
    }

    printBanner();
    NLM_LOG_INFO("=== NLM Phase 6: Final Integration ===");
    NLM_LOG_INFO("Starting Phase 6 integration test");
    NLM_LOG_INFO("Timestamp: " + getTimestamp());

    // Load configuration
    auto config = std::make_shared<Config>();

    if (fileExists(args.configFile)) {
        if (config->loadFromFile(args.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + args.configFile);
        } else {
            NLM_LOG_WARNING("Using default configuration (failed to load: " + args.configFile + ")");
        }
    } else {
        NLM_LOG_WARNING("Configuration file not found: " + args.configFile);
        NLM_LOG_WARNING("Using default configuration.");
        // Try to load default config
        if (fileExists("configs/default.cfg") && config->loadFromFile("configs/default.cfg")) {
            NLM_LOG_INFO("Loaded default configuration from: configs/default.cfg");
        }
    }

    // Apply CLI overrides
    applyCLIOverrides(args, config);

    // Validate configuration
    if (args.validate) {
        runValidationMode(args);
        return 0;
    }

    // Run the appropriate mode
    if (args.benchmark) {
        runBenchmarkMode(args);
    } else if (args.verify) {
        runVerificationMode(args);
    } else {
        // Default to demo mode
        runPhase6Demo(args);
    }

    NLM_LOG_INFO("=== Phase 6 Complete ===");
    NLM_LOG_INFO("Integration test finished successfully");

    return 0;
}
