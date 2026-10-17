#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6Demo.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace nlm {

// Command line argument parser
class CommandLineParser {
public:
    struct Options {
        std::string configFile = "configs/default.cfg";
        std::string outputFile = "";
        std::string logFile = "";
        std::string experimentType = "demo";
        int verbosity = 1;
        bool help = false;
        bool version = false;
        bool listConfigs = false;
        bool validateConfig = false;
        std::map<std::string, std::string> customConfigs;
        bool runDemo = false;
        bool runTests = false;
    };
    
    static Options parse(int argc, char** argv) {
        Options options;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                options.help = true;
            } else if (arg == "--version") {
                options.version = true;
            } else if (arg == "--list-configs") {
                options.listConfigs = true;
            } else if (arg == "--validate-config") {
                options.validateConfig = true;
            } else if (arg == "--demo") {
                options.runDemo = true;
                options.experimentType = "demo";
            } else if (arg == "--tests") {
                options.runTests = true;
                options.experimentType = "tests";
            } else if (arg == "--config" && i + 1 < argc) {
                options.configFile = argv[++i];
            } else if (arg.substr(0, 7) == "--config") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    options.customConfigs[arg.substr(7, pos - 7)] = arg.substr(pos + 1);
                }
            } else if (arg == "--output" && i + 1 < argc) {
                options.outputFile = argv[++i];
            } else if (arg == "--log-file" && i + 1 < argc) {
                options.logFile = argv[++i];
            } else if (arg == "--verbosity" && i + 1 < argc) {
                options.verbosity = std::stoi(argv[++i]);
                if (options.verbosity < 0 || options.verbosity > 3) {
                    std::cerr << "Error: Verbosity must be between 0 and 3" << std::endl;
                    options.verbosity = 1;
                }
            } else if (arg.substr(0, 2) == "--" && i + 1 < argc && arg.find('=') == std::string::npos) {
                std::string key = arg.substr(2);
                std::string value = argv[++i];
                options.customConfigs[key] = value;
            } else if (arg[0] == '-' && i + 1 < argc && arg.find('=') == std::string::npos) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                options.customConfigs[key] = value;
            } else if (arg == "--experiment" && i + 1 < argc) {
                options.experimentType = argv[++i];
            } else {
                std::cerr << "Unknown option: " << arg << std::endl;
                std::cerr << "Use --help for usage information." << std::endl;
            }
        }
        
        return options;
    }
    
    static void printHelp() {
        std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
        std::cout << "An experimental artificial developmental brain" << std::endl << std::endl;
        
        std::cout << "Usage: nlm [options]" << std::endl << std::endl;
        
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h              Show this help message" << std::endl;
        std::cout << "  --version               Show version information" << std::endl;
        std::cout << "  --list-configs          List all available configuration options" << std::endl;
        std::cout << "  --validate-config       Validate current configuration" << std::endl;
        std::cout << "  --demo                  Run integration demo" << std::endl;
        std::cout << "  --tests                 Run unit tests" << std::endl;
        std::cout << "  --config FILE          Load configuration from FILE (default: configs/default.cfg)" << std::endl;
        std::cout << "  --output FILE           Save output to FILE" << std::endl;
        std::cout << "  --log-file FILE         Log output to FILE" << std::endl;
        std::cout << "  --verbosity LEVEL       Set verbosity level (0-3, default: 1)" << std::endl;
        std::cout << "  --experiment TYPE       Specify experiment type (demo, integration, learning, etc.)" << std::endl;
        std::cout << "  --key=value             Set configuration value" << std::endl;
        std::cout << "  -key value              Short form of --key=value" << std::endl << std::endl;
        
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm --demo --config myconfig.json" << std::endl;
        std::cout << "  nlm --config experiments.cfg neuron_count=2000" << std::endl;
        std::cout << "  nlm --list-configs | head -20" << std::endl;
    }
};

// Configuration manager
class ConfigurationManager {
public:
    static std::unique_ptr<Config> loadConfig(const std::string& configFile,
                                             const std::map<std::string, std::string>& customConfigs) {
        auto config = std::make_unique<Config>();
        
        // Try to load config file (supports JSON, YAML, and simple formats)
        if (configFile.substr(configFile.find_last_of(".") + 1) == "json") {
            config->loadFromJSONFile(configFile, true);
        } else if (configFile.substr(configFile.find_last_of(".") + 1) == "yaml" ||
                   configFile.substr(configFile.find_last_of(".") + 1) == "yml") {
            config->loadFromYAMLFile(configFile, true);
        } else {
            config->loadFromFile(configFile, true);
        }
        
        // Apply custom configurations
        for (const auto& pair : customConfigs) {
            config->set(pair.first, pair.second, ConfigSource::CommandLine, true);
        }
        
        // Validate configuration
        auto validationResult = config->validate();
        if (!validationResult.valid) {
            std::cerr << "Configuration validation failed:" << std::endl;
            std::cerr << validationResult.errorMessage << std::endl;
            return nullptr;
        }
        
        return config;
    }
    
    static void setupLogging(const std::string& logFile, int verbosity) {
        auto logger = std::make_shared<Logger>();
        
        LogLevel level;
        switch (verbosity) {
            case 0: level = LogLevel::Error; break;
            case 1: level = LogLevel::Info; break;
            case 2: level = LogLevel::Warning; break;
            case 3: level = LogLevel::Debug; break;
            default: level = LogLevel::Info; break;
        }
        
        auto consoleLogger = std::make_shared<ConsoleLogger>(level);
        consoleLogger->setUseColors(verbosity >= 2);
        
        if (!logFile.empty()) {
            consoleLogger->setOutputFile(logFile);
        }
        
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
    }
    
    static void printConfigSummary(const Config& config) {
        std::cout << "=== Configuration Summary ===" << std::endl;
        std::cout << config.summary() << std::endl;
        
        // Print key configuration values
        std::cout << "=== Key Configuration Values ===" << std::endl;
        
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            if (config.has("neuron_count") && key == "neuron_count") {
                auto neurons = config.getOr<size_t>("neuron_count", 0);
                std::cout << "  Neurons: " << neurons << std::endl;
            } else if (config.has("simulation_timestep") && key == "simulation_timestep") {
                auto timestep = config.getOr<double>("simulation_timestep", 0.0);
                std::cout << "  Time step: " << timestep << "s" << std::endl;
            } else if (config.has("region_count") && key == "region_count") {
                auto regions = config.getOr<size_t>("region_count", 0);
                std::cout << "  Regions: " << regions << std::endl;
            }
        }
        
        std::cout << "=========================" << std::endl;
    }
};

// Main execution class
class NLMApplication {
public:
    static int run(const CommandLineParser::Options& options) {
        // Print help or version
        if (options.help) {
            CommandLineParser::printHelp();
            return 0;
        }
        
        if (options.version) {
            std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
            std::cout << "Version: 0.1.0 (Phase 6 - Final Integration)" << std::endl;
            std::cout << "Built on " << __DATE__ << " at " << __TIME__ << std::endl;
            return 0;
        }
        
        if (options.listConfigs) {
            listAvailableConfigurations();
            return 0;
        }
        
        // Load configuration
        std::cout << "Loading configuration..." << std::endl;
        auto config = ConfigurationManager::loadConfig(options.configFile, options.customConfigs);
        if (!config) {
            return 1;
        }
        
        // Setup logging
        ConfigurationManager::setupLogging(options.logFile, options.verbosity);
        
        // Print configuration summary
        if (options.verbosity >= 1) {
            ConfigurationManager::printConfigSummary(*config);
        }
        
        // Run the specified experiment
        if (options.experimentType == "demo" || options.runDemo) {
            std::cout << "Running Phase 6 integration demo..." << std::endl;
            return runDemo(*config);
        } else if (options.experimentType == "tests" || options.runTests) {
            std::cout << "Running unit tests..." << std::endl;
            return runTests();
        } else {
            std::cerr << "Error: Unknown experiment type: " << options.experimentType << std::endl;
            return 1;
        }
    }
    
private:
    static int runDemo(Config& config) {
        // Set demo-specific configuration
        config.set("neuron_count", 500, ConfigSource::Runtime);
        config.set("max_steps", 2000, ConfigSource::Runtime);
        config.set("enable_checkpointing", true, ConfigSource::Runtime);
        config.set("enable_replay", true, ConfigSource::Runtime);
        
        // Create and run Phase 6 demo
        Phase6IntegratedExperiment experiment;
        Phase6Config demoConfig;
        demoConfig.neuronCount = 500;
        demoConfig.maxSteps = 2000;
        demoConfig.enableCheckpointing = true;
        demoConfig.enableReplay = true;
        demoConfig.enableDevelopment = true;
        
        auto result = experiment.run(demoConfig);
        
        // Print results
        std::cout << std::endl;
        std::cout << "=== DEMO RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        
        return 0;
    }
    
    static int runTests() {
        // Run config tests
        std::cout << "Running configuration tests..." << std::endl;
        test_config::runAll();
        
        std::cout << std::endl;
        std::cout << "Configuration tests completed successfully!" << std::endl;
        return 0;
    }
    
    static void listAvailableConfigurations() {
        std::cout << "Available Configuration Options:" << std::endl;
        std::cout << "===============================" << std::endl;
        std::cout << "  neuron_count (int, default: 1000) - Number of neurons in the brain" << std::endl;
        std::cout << "  region_count (int, default: 1) - Number of brain regions" << std::endl;
        std::cout << "  simulation_timestep (double, default: 0.001) - Simulation time step" << std::endl;
        std::cout << "  connection_probability (double, default: 0.1) - Neural connectivity probability" << std::endl;
        std::cout << "  stdp_ltp_weight (double, default: 0.01) - STDP Long-Term Potentiation weight" << std::endl;
        std::cout << "  stdp_ltd_weight (double, default: 0.012) - STDP Long-Term Depression weight" << std::endl;
        std::cout << "  synaptogenesis_rate (double, default: 0.0001) - New synapse formation rate" << std::endl;
        std::cout << "  pruning_rate (double, default: 0.00001) - Synapse elimination rate" << std::endl;
        std::cout << "  dopamine_baseline (double, default: 0.0) - Baseline dopamine level" << std::endl;
        std::cout << "  reward_discount_factor (double, default: 0.99) - Reward discount factor" << std::endl;
        std::cout << "  log_level (string, default: INFO) - Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)" << std::endl;
        std::cout << "  max_simulation_steps (int, default: 10000) - Maximum simulation steps" << std::endl;
        std::cout << "  enable_checkpointing (bool, default: false) - Enable checkpoint saving" << std::endl;
        std::cout << "  checkpoint_dir (string, default: ./checkpoints) - Checkpoint directory" << std::endl;
        std::cout << "  visualization_enabled (bool, default: false) - Enable visualization" << std::endl;
        std::cout << "  working_memory_capacity (int, default: 1000) - Working memory capacity" << std::endl;
        std::cout << "  episodic_memory_max_episodes (int, default: 1000) - Maximum episodic memory episodes" << std::endl;
        std::cout << "  curiosity_decay_rate (double, default: 0.95) - Curiosity decay rate" << std::endl;
        std::cout << "  novelty_threshold (double, default: 0.1) - Novelty detection threshold" << std::endl;
        std::cout << "  prediction_error_decay (double, default: 0.9) - Prediction error decay rate" << std::endl;
        std::cout << "  developmental_stage_duration (int, default: 1000) - Developmental stage duration" << std::endl;
        std::cout << "  plasticity_rate_multiplier (double, default: 1.0) - Plasticity rate multiplier" << std::endl;
        std::cout << "  structural_plasticity_enabled (bool, default: true) - Enable structural plasticity" << std::endl;
        std::cout << std::endl;
        std::cout << "Configuration files can be in JSON, YAML, or simple key=value format." << std::endl;
        std::cout << "For examples, see configs/ directory." << std::endl;
    }
};

// Main entry point
int main(int argc, char** argv) {
    auto options = CommandLineParser::parse(argc, argv);
    return NLMApplication::run(options);
}

} // namespace nlm