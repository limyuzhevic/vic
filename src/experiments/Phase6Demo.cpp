/**
 * Phase 6 Demo - Enhanced Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 * 
 * Enhanced CLI for testing different scenarios:
 *   --neurons N    : Number of neurons (default: 500)
 *   --steps S       : Number of simulation steps (default: 2000)
 *   --region R      : Number of brain regions (default: 1)
 *   --checkpoint    : Enable checkpointing (default: true)
 *   --replay        : Enable replay system (default: true)
 *   --development   : Enable development system (default: true)
 *   --config FILE   : Load configuration from file
 *   --list-scenarios : List available test scenarios
 *   --scenario SCEN : Run predefined test scenario (quick, thorough, stress)
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <string>
#include <map>

using namespace nlm;

// Predefined test scenarios
struct TestScenario {
    std::string name;
    size_t neuronCount;
    size_t maxSteps;
    size_t regionCount;
    bool enableCheckpointing;
    bool enableReplay;
    bool enableDevelopment;
    std::string description;
};

// Available scenarios
const std::vector<TestScenario> availableScenarios = {
    {
        "quick",
        100,
        500,
        1,
        false,
        false,
        false,
        "Quick integration test - minimal setup"
    },
    {
        "thorough",
        500,
        2000,
        2,
        true,
        true,
        true,
        "Thorough integration test - all systems enabled"
    },
    {
        "stress",
        2000,
        10000,
        4,
        true,
        true,
        true,
        "Stress test - maximum load and complexity"
    }
};

void printHelp() {
    std::cout << "=== NLM Phase 6 Integration Demo - Enhanced CLI ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nml_phase6_demo [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Core Options:" << std::endl;
    std::cout << "  --help                    : Show this help message" << std::endl;
    std::cout << "  --list-scenarios         : List available test scenarios" << std::endl;
    std::cout << "  --scenario SCEN          : Run predefined scenario (quick, thorough, stress)" << std::endl;
    std::cout << "  --neurons N              : Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps S                : Number of simulation steps (default: 2000)" << std::endl;
    std::cout << "  --region R               : Number of brain regions (default: 1)" << std::endl;
    std::cout << std::endl;
    std::cout << "Feature Options:" << std::endl;
    std::cout << "  --checkpoint             : Enable checkpointing (default: true)" << std::endl;
    std::cout << "  --replay                 : Enable replay system (default: true)" << std::endl;
    std::cout << "  --development            : Enable development system (default: true)" << std::endl;
    std::cout << "  --no-checkpoint          : Disable checkpointing" << std::endl;
    std::cout << "  --no-replay              : Disable replay system" << std::endl;
    std::cout << "  --no-development         : Disable development system" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration Options:" << std::endl;
    std::cout << "  --config FILE            : Load configuration from file" << std::endl;
    std::cout << "  --set KEY=VALUE          : Set configuration value (can be repeated)" << std::endl;
    std::cout << "  --log-level LEVEL        : Set log level (DEBUG, INFO, WARN, ERROR, CRIT)" << std::endl;
    std::cout << std::endl;
}

void listScenarios() {
    std::cout << "=== Available Test Scenarios ===" << std::endl;
    std::cout << std::endl;
    for (const auto& scenario : availableScenarios) {
        std::cout << scenario.name << ":" << std::endl;
        std::cout << "  Description: " << scenario.description << std::endl;
        std::cout << "  Neurons: " << scenario.neuronCount << " | Steps: " << scenario.maxSteps << std::endl;
        std::cout << "  Regions: " << scenario.regionCount << " | Checkpoint: " << (scenario.enableCheckpointing ? "YES" : "NO") << std::endl;
        std::cout << "  Replay: " << (scenario.enableReplay ? "YES" : "NO") << " | Development: " << (scenario.enableDevelopment ? "YES" : "NO") << std::endl;
        std::cout << std::endl;
    }
}

bool parseCommandLine(int argc, char* argv[], Phase6Config& config,
                     std::map<std::string, std::string>& extraSettings,
                     std::string& scenarioName,
                     bool& verifyOnly,
                     bool& quiet,
                     std::string& configFile) {
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            printHelp();
            return true;  // Signal to exit
        }
        else if (arg == "--list-scenarios") {
            listScenarios();
            return true;  // Signal to exit
        }
        else if (arg.substr(0, 11) == "--scenario=") {
            scenarioName = arg.substr(11);
        }
        else if (arg == "--verify-only") {
            verifyOnly = true;
        }
        else if (arg == "--quiet") {
            quiet = true;
        }
        else if (arg.substr(0, 9) == "--set=") {
            std::string setting = arg.substr(9);
            size_t pos = setting.find('=');
            if (pos != std::string::npos) {
                std::string key = setting.substr(0, pos);
                std::string value = setting.substr(pos + 1);
                extraSettings[key] = value;
            }
        }
        else if (arg.substr(0, 8) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
        else if (arg == "--checkpoint") {
            config.enableCheckpointing = true;
        }
        else if (arg == "--no-checkpoint") {
            config.enableCheckpointing = false;
        }
        else if (arg == "--replay") {
            config.enableReplay = true;
        }
        else if (arg == "--no-replay") {
            config.enableReplay = false;
        }
        else if (arg == "--development") {
            config.enableDevelopment = true;
        }
        else if (arg == "--no-development") {
            config.enableDevelopment = false;
        }
        else if (arg.substr(0, 10) == "--neurons=") {
            config.neuronCount = std::stoull(arg.substr(10));
        }
        else if (arg.substr(0, 9) == "--steps=") {
            config.maxSteps = std::stoull(arg.substr(9));
        }
        else if (arg.substr(0, 9) == "--region=") {
            config.regionCount = std::stoull(arg.substr(9));
        }
        else if (arg.substr(0, 12) == "--log-level=") {
            // Would set log level here
        }
        else if (arg.substr(0, 2) == "--") {
            // Unknown option
            std::cerr << "Warning: Unknown option " << arg << std::endl;
        }
    }
    
    return false;
}

int main(int argc, char* argv[]) {
    Phase6Config config;
    std::map<std::string, std::string> extraSettings;
    std::string scenarioName;
    bool verifyOnly = false;
    bool quiet = false;
    std::string configFile = "configs/default.cfg";
    
    // Parse command line
    if (parseCommandLine(argc, argv, config, extraSettings, scenarioName, verifyOnly, quiet, configFile)) {
        return 0;  // Help or list scenarios was requested
    }
    
    // Apply scenario settings if specified
    if (!scenarioName.empty()) {
        auto scenarioIt = std::find_if(availableScenarios.begin(), availableScenarios.end(),
                                      [&](const TestScenario& s) { return s.name == scenarioName; });
        if (scenarioIt != availableScenarios.end()) {
            config = *scenarioIt;  // Copy scenario configuration
            if (!quiet) {
                std::cout << "=== Using Scenario: " << scenarioIt->name << " ===" << std::endl;
                std::cout << scenarioIt->description << std::endl << std::endl;
            }
        } else {
            std::cerr << "Error: Unknown scenario '" << scenarioName << "'" << std::endl;
            listScenarios();
            return 1;
        }
    }
    
    // Load configuration file
    auto cfg = std::make_shared<Config>();
    if (!configFile.empty() && cfg->loadFromFile(configFile)) {
        if (!quiet) {
            std::cout << "Loaded configuration from: " << configFile << std::endl;
        }
    } else if (!quiet) {
        std::cout << "Using default configuration (no file: " << configFile << ")" << std::endl;
    }
    
    // Apply extra settings from command line
    for (const auto& setting : extraSettings) {
        cfg->set(setting.first, setting.second, ConfigSource::CommandLine);
    }
    
    // Override with Phase 6 specific settings
    cfg->set("neuron_count", config.neuronCount, ConfigSource::Runtime);
    cfg->set("region_count", config.regionCount, ConfigSource::Runtime);
    cfg->set("connection_probability", config.connectionProbability, ConfigSource::Runtime);
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    if (!quiet) {
        std::cout << "=== NLM Phase 6 Integration Demo - Enhanced CLI ===" << std::endl;
        std::cout << "Starting integration test with configuration:" << std::endl;
        std::cout << "  Neurons: " << config.neuronCount << std::endl;
        std::cout << "  Steps: " << config.maxSteps << std::endl;
        std::cout << "  Regions: " << config.regionCount << std::endl;
        std::cout << "  Checkpointing: " << (config.enableCheckpointing ? "YES" : "NO") << std::endl;
        std::cout << "  Replay: " << (config.enableReplay ? "YES" : "NO") << std::endl;
        std::cout << "  Development: " << (config.enableDevelopment ? "YES" : "NO") << std::endl;
        std::cout << std::endl;
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    if (verifyOnly) {
        if (!quiet) {
            std::cout << "=== Verification Mode ===" << std::endl;
            std::cout << "Running integration verification only..." << std::endl;
        }
        
        bool integrationOK = experiment.verifyIntegration();
        
        if (!quiet) {
            std::cout << std::endl;
            std::cout << "=== Integration Verification Result ===" << std::endl;
            std::cout << "Integration status: " << (integrationOK ? "PASSED" : "FAILED") << std::endl;
        }
        
        return integrationOK ? 0 : 1;
    }
    
    // First, run the quick integration verification
    if (!quiet) {
        std::cout << "--- Integration Verification ---" << std::endl;
    }
    
    bool integrationOK = experiment.verifyIntegration();
    
    if (!quiet) {
        std::cout << std::endl;
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return 1;
        }
        
        std::cout << "Integration verification passed!" << std::endl << std::endl;
    }
    
    // Test individual systems
    if (!quiet) {
        std::cout << "--- Memory Integration Test ---" << std::endl;
    }
    experiment.testMemoryIntegration();
    
    if (!quiet) {
        std::cout << std::endl;
        std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    }
    experiment.testNeuromodulationIntegration();
    
    if (config.enableCheckpointing && !quiet) {
        std::cout << std::endl;
        std::cout << "--- Checkpoint Test ---" << std::endl;
    }
    experiment.testCheckpointing();
    
    if (config.enableReplay && !quiet) {
        std::cout << std::endl;
        std::cout << "--- Replay Test ---" << std::endl;
    }
    experiment.testReplay();
    
    // Run full experiment
    if (!quiet) {
        std::cout << std::endl;
        std::cout << "--- Full Integration Experiment ---" << std::endl;
    }
    
    auto result = experiment.run(config);
    
    // Final results
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
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
    
    return 0;
}