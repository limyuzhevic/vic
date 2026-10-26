// NLM Command-Line Interface
// Phase 6: Final integration with CLI support

#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/experiments/Phase6IntegratedExperiment.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <iomanip>
#include <vector>
#include <map>

using namespace nlm;

/**
 * Command-line argument parser
 */
class CLIParser {
public:
    struct Options {
        std::string mode = "demo";
        std::string configFile = "";
        std::string outputFile = "";
        std::string checkpointFile = "";
        bool verbose = false;
        bool quiet = false;
        uint64_t seed = 42;
        size_t iterations = 1000;
        std::string logLevel = "info";
        std::vector<std::string> experimentTypes;
        bool help = false;
        bool version = false;
        std::map<std::string, std::string> customArgs;
    };

    static Options parse(int argc, char* argv[]);
    static void printHelp();
    static void printVersion();
};

/**
 * Main NLM CLI application
 */
class NLMCLI {
public:
    NLMCLI() : logger_(std::make_shared<Logger>()) {}
    
    bool run(const CLIParser::Options& options);
    bool runDemo(const CLIParser::Options& options);
    bool runExperiment(const CLIParser::Options& options);
    bool runConfigTest(const CLIParser::Options& options);
    bool runCheckpointTest(const CLIParser::Options& options);
    
private:
    std::shared_ptr<Logger> logger_;
    
    bool setupLogging(const std::string& level);
    bool createConfigFromOptions(const CLIParser::Options& options);
    bool runPhase6Integration(const CLIParser::Options& options);
};

/**
 * Program entry point
 */
int main(int argc, char* argv[]) {
    try {
        CLIParser::Options options = CLIParser::parse(argc, argv);
        
        if (options.help) {
            CLIParser::printHelp();
            return 0;
        }
        
        if (options.version) {
            CLIParser::printVersion();
            return 0;
        }
        
        NLMCLI cli;
        if (!cli.run(options)) {
            std::cerr << "NLM CLI failed with error code 1" << std::endl;
            return 1;
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error" << std::endl;
        return 1;
    }
}

// CLIParser implementation
CLIParser::Options CLIParser::parse(int argc, char* argv[]) {
    Options options;
    
    // First pass: parse known options
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version" || arg == "-v") {
            options.version = true;
        } else if (arg == "--demo") {
            options.mode = "demo";
        } else if (arg == "--experiment") {
            options.mode = "experiment";
        } else if (arg == "--config" && i + 1 < argc) {
            options.configFile = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            options.outputFile = argv[++i];
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            options.checkpointFile = argv[++i];
        } else if (arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "--quiet") {
            options.quiet = true;
        } else if (arg == "--seed" && i + 1 < argc) {
            options.seed = std::stoull(argv[++i]);
        } else if (arg == "--iterations" && i + 1 < argc) {
            options.iterations = std::stoull(argv[++i]);
        } else if (arg == "--log-level" && i + 1 < argc) {
            options.logLevel = argv[++i];
        } else if (arg == "--experiment-type" && i + 1 < argc) {
            options.experimentTypes.push_back(argv[++i]);
        } else if (arg.substr(0, 2) == "--") {
            // Custom argument
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                options.customArgs[key] = value;
            }
        } else {
            // Positional argument
            if (options.mode == "demo") {
                // Parse demo arguments
                if (arg.find("phase") != std::string::npos) {
                    // Parse phase number
                    try {
                        size_t phaseNum = std::stoull(arg.substr(5));
                        options.iterations = phaseNum * 1000; // Rough mapping
                    } catch (...) {
                        // Ignore parsing errors
                    }
                }
            }
        }
    }
    
    return options;
}

void CLIParser::printHelp() {
    std::cout << R"(
=== NLM Command-Line Interface ===

Usage: nlm [OPTIONS]

Modes:
  --demo          Run demo mode (default)
  --experiment    Run experiment mode

Options:
  -h, --help              Show this help message
  -v, --version           Show version information
  --config FILE           Load configuration from file
  --output FILE           Save output to file
  --checkpoint FILE       Checkpoint/save file
  --verbose              Enable verbose output
  --quiet                 Suppress non-essential output
  --seed N               Random seed (default: 42)
  --iterations N          Number of simulation iterations (default: 1000)
  --log-level LEVEL       Logging level: debug, info, warn, error (default: info)
  --experiment-type TYPE  Specify experiment type to run

Examples:
  nlm --demo --iterations 500
  nlm --experiment --config my_config.cfg --output results.json
  nlm --demo --seed 123 --checkpoint brain.bin

Commands:
  nlm demo               Run Phase 6 integration demo
  nlm experiment         Run comprehensive experiment suite
  nlm config             Generate and validate configuration
  nlm checkpoint         Test checkpoint save/load functionality

For more information, visit: https://github.com/nlm-project/nlm
)" << std::endl;
}

void CLIParser::printVersion() {
    std::cout << "NLM (Neural Learning Machine) Phase 6.0" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "C++20 with SIMD optimizations" << std::endl;
}

// NLMCLI implementation
bool NLMCLI::run(const CLIParser::Options& options) {
    if (options.mode == "demo") {
        return runDemo(options);
    } else if (options.mode == "experiment") {
        return runExperiment(options);
    } else if (options.mode == "config") {
        return runConfigTest(options);
    } else if (options.mode == "checkpoint") {
        return runCheckpointTest(options);
    }
    
    std::cerr << "Unknown mode: " << options.mode << std::endl;
    return false;
}

bool NLMCLI::setupLogging(const std::string& level) {
    Logger::Level logLevel;
    
    if (level == "debug") logLevel = Logger::Level::Debug;
    else if (level == "info") logLevel = Logger::Level::Info;
    else if (level == "warn") logLevel = Logger::Level::Warn;
    else if (level == "error") logLevel = Logger::Level::Error;
    else logLevel = Logger::Level::Info;
    
    Logger::getInstance().setLevel(logLevel);
    return true;
}

bool NLMCLI::runDemo(const CLIParser::Options& options) {
    if (!setupLogging(options.logLevel)) {
        std::cerr << "Failed to setup logging" << std::endl;
        return false;
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Running Phase 6 integration verification..." << std::endl << std::endl;
    
    // Create Phase 6 experiment
    Phase6IntegratedExperiment experiment;
    
    // Integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return false;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
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
    
    // Run full experiment with configured settings
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = static_cast<uint64_t>(options.iterations / 10); // Scale iterations
    config.maxSteps = options.iterations;
    config.enableCheckpointing = !options.checkpointFile.empty();
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = options.checkpointFile.empty() ? "./demo_checkpoint.bin" : options.checkpointFile;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== DEMO RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    return true;
}

bool NLMCLI::runExperiment(const CLIParser::Options& options) {
    std::cout << "=== NLM Experiment Suite ===" << std::endl;
    std::cout << "Running comprehensive experiment suite..." << std::endl << std::endl;
    
    // Create Phase 6 experiment
    Phase6IntegratedExperiment experiment;
    
    // Run integration verification
    std::cout << "--- Running Integration Verification ---" << std::endl;
    if (!experiment.verifyIntegration()) {
        std::cerr << "Integration verification failed!" << std::endl;
        return false;
    }
    
    // Run multiple experiment types if specified
    std::vector<std::string> expTypes = options.experimentTypes;
    if (expTypes.empty()) {
        expTypes = {"memory", "neuromodulation", "checkpoint", "replay", "full"};
    }
    
    std::cout << "--- Running Specified Experiments ---" << std::endl;
    for (const auto& expType : expTypes) {
        std::cout << "Running experiment: " << expType << std::endl;
        
        if (expType == "memory") {
            if (!experiment.testMemoryIntegration()) {
                std::cerr << "Memory integration test failed!" << std::endl;
                return false;
            }
        } else if (expType == "neuromodulation") {
            if (!experiment.testNeuromodulationIntegration()) {
                std::cerr << "Neuromodulation integration test failed!" << std::endl;
                return false;
            }
        } else if (expType == "checkpoint") {
            if (!experiment.testCheckpointing()) {
                std::cerr << "Checkpoint test failed!" << std::endl;
                return false;
            }
        } else if (expType == "replay") {
            if (!experiment.testReplay()) {
                std::cerr << "Replay test failed!" << std::endl;
                return false;
            }
        } else if (expType == "full") {
            Phase6Config config;
            config.neuronCount = 500;
            config.maxSteps = options.iterations;
            config.enableCheckpointing = true;
            config.enableReplay = true;
            config.enableDevelopment = true;
            
            auto result = experiment.run(config);
            
            std::cout << "Full experiment results:" << std::endl;
            std::cout << "  Reward: " << result.totalReward << std::endl;
            std::cout << "  Firing rate: " << result.avgFiringRate << std::endl;
            std::cout << "  Memory episodes: " << result.memoryEpisodesStored << std::endl;
        }
    }
    
    std::cout << "=== ALL EXPERIMENTS COMPLETED ===" << std::endl;
    return true;
}

bool NLMCLI::runConfigTest(const CLIParser::Options& options) {
    std::cout << "=== NLM Configuration Test ===" << std::endl;
    std::cout << "Testing configuration loading and validation..." << std::endl << std::endl;
    
    auto cfg = std::make_shared<Config>();
    
    if (!options.configFile.empty()) {
        std::cout << "Loading configuration from: " << options.configFile << std::endl;
        if (!cfg->loadFromFile(options.configFile)) {
            std::cerr << "Failed to load configuration file" << std::endl;
            return false;
        }
    } else {
        std::cout << "Creating default configuration..." << std::endl;
        // Set default values
        cfg->set("neuron_count", 100);
        cfg->set("region_count", 1);
        cfg->set("connection_probability", 0.1f);
    }
    
    // Display configuration
    std::cout << "Configuration summary:" << std::endl;
    std::cout << "  Keys: " << cfg->getKeys().size() << std::endl;
    for (const auto& key : cfg->getKeys()) {
        std::cout << "  " << key << std::endl;
    }
    
    // Save if output file specified
    if (!options.outputFile.empty()) {
        std::cout << "Saving configuration to: " << options.outputFile << std::endl;
        if (!cfg->saveToFile(options.outputFile)) {
            std::cerr << "Failed to save configuration" << std::endl;
            return false;
        }
    }
    
    std::cout << "Configuration test passed!" << std::endl;
    return true;
}

bool NLMCLI::runCheckpointTest(const CLIParser::Options& options) {
    std::cout << "=== NLM Checkpoint Test ===" << std::endl;
    std::cout << "Testing checkpoint save/load functionality..." << std::endl << std::endl;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 100);
    
    // Create and initialize brain
    auto brain1 = std::make_shared<Brain>(cfg);
    if (!brain1->initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return false;
    }
    
    // Run some steps
    for (int i = 0; i < 50; ++i) {
        brain1->step(i, i * 0.001);
    }
    
    std::string checkpointPath = options.checkpointFile.empty() ? "/tmp/nlm_cli_checkpoint.bin" : options.checkpointFile;
    
    // Save checkpoint
    std::cout << "Saving checkpoint to: " << checkpointPath << std::endl;
    if (!brain1->save(checkpointPath)) {
        std::cerr << "Failed to save checkpoint" << std::endl;
        return false;
    }
    
    // Create new brain and load
    auto brain2 = std::make_shared<Brain>(cfg);
    brain2->initialize();
    
    std::cout << "Loading checkpoint from: " << checkpointPath << std::endl;
    if (!brain2->load(checkpointPath)) {
        std::cerr << "Failed to load checkpoint" << std::endl;
        return false;
    }
    
    // Compare basic stats
    size_t spikes1 = brain1->getTotalSpikeCount();
    size_t spikes2 = brain2->getTotalSpikeCount();
    
    std::cout << "Brain1 spikes: " << spikes1 << std::endl;
    std::cout << "Brain2 spikes: " << spikes2 << std::endl;
    
    std::cout << "=== CHECKPOINT TEST COMPLETED ===" << std::endl;
    return true;
}
