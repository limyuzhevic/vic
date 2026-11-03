#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>

namespace nlm {

// Command-line tool for configuration validation and analysis
class ConfigTool {
public:
    static int run(int argc, char** argv) {
        try {
            std::cout << "=== NLM Configuration Tool ===" << std::endl;
            
            if (argc < 2) {
                printUsage();
                return 1;
            }
            
            std::string command = argv[1];
            
            if (command == "validate") {
                return validateConfig(argc, argv);
            } else if (command == "generate") {
                return generateConfig(argc, argv);
            } else if (command == "analyze") {
                return analyzeConfig(argc, argv);
            } else if (command == "benchmark") {
                return benchmarkConfig(argc, argv);
            } else {
                std::cerr << "Error: Unknown command: " << command << std::endl;
                printUsage();
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "Fatal error: " << e.what() << std::endl;
            return 1;
        }
    }

private:
    static void printUsage() {
        std::cout << "Usage:" << std::endl;
        std::cout << "  nlm-config-tool validate [options]     Validate configuration files" << std::endl;
        std::cout << "  nlm-config-tool generate [options]   Generate configuration templates" << std::endl;
        std::cout << "  nlm-config-tool analyze [options]    Analyze configuration statistics" << std::endl;
        std::cout << "  nlm-config-tool benchmark [options]  Benchmark configuration performance" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm-config-tool validate configs/default.cfg" << std::endl;
        std::cout << "  nlm-config-tool generate --output research_config.json" << std::endl;
        std::cout << "  nlm-config-tool analyze --input config1.json --input config2.json" << std::endl;
    }

    static int validateConfig(int argc, char** argv) {
        std::string configFile;
        
        // Parse command line arguments
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--input" && i + 1 < argc) {
                configFile = argv[++i];
            } else if (arg.find("--input=") == 0) {
                configFile = arg.substr(9);
            }
        }
        
        if (configFile.empty()) {
            std::cerr << "Error: --input parameter required for validate command" << std::endl;
            return 1;
        }
        
        std::cout << "Validating configuration: " << configFile << std::endl;
        
        // Initialize logger for validation
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Load and validate configuration
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            std::cerr << "Error: Failed to load configuration file" << std::endl;
            return 1;
        }
        
        // Validate configuration
        bool isValid = validateConfiguration(config);
        
        if (isValid) {
            std::cout << "Configuration validation PASSED" << std::endl;
            return 0;
        } else {
            std::cout << "Configuration validation FAILED" << std::endl;
            return 1;
        }
    }

    static bool validateConfiguration(const std::shared_ptr<Config>& config) {
        bool allValid = true;
        
        // Check for critical configuration keys
        const std::vector<std::string> requiredKeys = {
            "brain.neuron_count",
            "brain.synapse_density",
            "simulation_timestep"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config->has(key)) {
                NLM_LOG_WARNING("Missing required configuration key: " + key);
                allValid = false;
            }
        }
        
        // Validate specific parameter ranges
        try {
            auto neuronCount = config->get<int64_t>("brain.neuron_count");
            if (neuronCount && (*neuronCount <= 0 || *neuronCount > 1000000)) {
                NLM_LOG_WARNING("Unreasonable neuron count: " + std::to_string(*neuronCount));
                allValid = false;
            }
        } catch (...) {
            NLM_LOG_WARNING("Invalid neuron count configuration");
            allValid = false;
        }
        
        try {
            auto synapseDensity = config->get<double>("brain.synapse_density");
            if (synapseDensity && (*synapseDensity < 0.0 || *synapseDensity > 1.0)) {
                NLM_LOG_WARNING("Synapse density should be between 0.0 and 1.0: " + std::to_string(*synapseDensity));
                allValid = false;
            }
        } catch (...) {
            NLM_LOG_WARNING("Invalid synapse density configuration");
            allValid = false;
        }
        
        try {
            auto timestep = config->get<double>("simulation_timestep");
            if (timestep && (*timestep <= 0.0 || *timestep > 0.1)) {
                NLM_LOG_WARNING("Simulation timestep should be small (<= 0.1s): " + std::to_string(*timestep));
                allValid = false;
            }
        } catch (...) {
            NLM_LOG_WARNING("Invalid simulation timestep configuration");
            allValid = false;
        }
        
        return allValid;
    }

    static int generateConfig(int argc, char** argv) {
        std::string outputFile = "generated_config.json";
        bool researchMode = false;
        bool performanceMode = false;
        
        // Parse command line arguments
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--research") {
                researchMode = true;
            } else if (arg == "--performance") {
                performanceMode = true;
            } else if (arg == "--output" && i + 1 < argc) {
                outputFile = argv[++i];
            } else if (arg.find("--output=") == 0) {
                outputFile = arg.substr(9);
            }
        }
        
        std::cout << "Generating configuration template..." << std::endl;
        
        // Create default configuration
        auto config = std::make_shared<Config>();
        
        // Set base configuration
        config->set("brain.neuron_count", int64_t(1000), ConfigSource::Default);
        config->set("brain.synapse_density", 0.1, ConfigSource::Default);
        config->set("brain.connection_probability", 0.05, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("random_seed", int64_t(42), ConfigSource::Default);
        
        // Add research-specific settings
        if (researchMode) {
            config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
            config->set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
            config->set("plasticity.structural.enable", true, ConfigSource::Runtime);
            config->set("neuromod.dopamine.scale", 2.0, ConfigSource::Runtime);
            config->set("neuromod.curiosity.enable", true, ConfigSource::Runtime);
            config->set("neuromod.novelty.enable", true, ConfigSource::Runtime);
        }
        
        // Add performance-specific settings
        if (performanceMode) {
            config->set("brain.neuron_count", int64_t(5000), ConfigSource::Runtime);
            config->set("brain.synapse_density", 0.05, ConfigSource::Runtime);
            config->set("simulation_timestep", 0.0005, ConfigSource::Runtime);
        }
        
        // Save configuration
        if (config->saveToFile(outputFile)) {
            std::cout << "Configuration generated successfully: " << outputFile << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to generate configuration" << std::endl;
            return 1;
        }
    }

    static int analyzeConfig(int argc, char** argv) {
        std::vector<std::string> configFiles;
        
        // Parse command line arguments
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--input" && i + 1 < argc) {
                configFiles.push_back(argv[++i]);
            } else if (arg.find("--input=") == 0) {
                configFiles.push_back(arg.substr(9));
            }
        }
        
        if (configFiles.empty()) {
            std::cerr << "Error: At least one --input configuration file required for analyze command" << std::endl;
            return 1;
        }
        
        std::cout << "Analyzing configuration files..." << std::endl;
        
        // Analyze each configuration file
        std::map<std::string, std::shared_ptr<Config>> configs;
        
        for (const auto& file : configFiles) {
            auto config = std::make_shared<Config>();
            if (config->loadFromFile(file)) {
                configs[file] = config;
                std::cout << "Loaded: " << file << std::endl;
            } else {
                std::cerr << "Warning: Failed to load configuration: " << file << std::endl;
            }
        }
        
        // Compare configurations
        if (configs.size() > 1) {
            compareConfigurations(configs);
        }
        
        return 0;
    }

    static void compareConfigurations(const std::map<std::string, std::shared_ptr<Config>>& configs) {
        std::cout << "\n=== Configuration Comparison ===" << std::endl;
        
        if (configs.empty()) {
            return;
        }
        
        // Get all unique keys from all configurations
        std::set<std::string> allKeys;
        for (const auto& pair : configs) {
            for (const auto& key : pair.second->getKeys()) {
                allKeys.insert(key);
            }
        }
        
        // Create comparison table
        std::cout << std::left << std::setw(30) << "Parameter";
        for (const auto& pair : configs) {
            std::cout << std::left << std::setw(15) << (pair.first + ":");
        }
        std::cout << std::endl;
        
        for (const auto& key : allKeys) {
            std::cout << std::left << std::setw(30) << key;
            for (const auto& pair : configs) {
                auto value = pair.second->get<std::string>(key);
                if (value) {
                    std::cout << std::left << std::setw(15) << *value;
                } else {
                    std::cout << std::left << std::setw(15) << "N/A";
                }
            }
            std::cout << std::endl;
        }
    }

    static int benchmarkConfig(int argc, char** argv) {
        std::cout << "=== NLM Configuration Benchmarking ===" << std::endl;
        
        // Simple benchmark of configuration operations
        auto config = std::make_shared<Config>();
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Test configuration operations
        const int iterations = 1000;
        
        for (int i = 0; i < iterations; ++i) {
            config->set("test.param", i, ConfigSource::Runtime);
            auto value = config->get<int>("test.param");
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        std::cout << "Configuration operations benchmark (" << iterations << " iterations):" << std::endl;
        std::cout << "  Total time: " << duration.count() << " microseconds" << std::endl;
        std::cout << "  Average time per operation: " << 
            (duration.count() * 1000.0 / iterations) << " nanoseconds" << std::endl;
        
        return 0;
    }
};

// Command-line entry point for config tool
int nlm_config_tool_main(int argc, char** argv) {
    return ConfigTool::run(argc, argv);
}

} // namespace nlm
