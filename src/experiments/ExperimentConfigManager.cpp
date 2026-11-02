#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>

namespace nlm {

class ExperimentConfigManager {
private:
    std::shared_ptr<Config> config;
    
    // Configuration schema for validation
    struct ConfigSchema {
        std::string key;
        std::string type;
        std::string description;
        std::string defaultValue;
        bool required;
        std::string validationPattern;
        
        ConfigSchema(const std::string& k, const std::string& t, const std::string& d, 
                     const std::string& dv, bool req = false, const std::string& pattern = "")
            : key(k), type(t), description(d), defaultValue(dv), required(req), validationPattern(pattern) {}
    };
    
    static const std::vector<ConfigSchema> getConfigSchema() {
        return {
            // Random seed
            ConfigSchema("random_seed", "int64", "Random seed for reproducible experiments", "42", true),
            
            // Simulation timestep
            ConfigSchema("simulation_timestep", "double", "Simulation timestep in seconds", "0.001", true),
            
            // Network parameters
            ConfigSchema("neuron_count", "int64", "Number of neurons in the brain", "1000", true),
            ConfigSchema("region_count", "int64", "Number of brain regions", "2", true),
            ConfigSchema("connection_probability", "double", "Connection probability for random connectivity", "0.1", true),
            
            // Plasticity settings
            ConfigSchema("plasticity_learning_rate", "double", "Plasticity learning rate", "0.01", true),
            ConfigSchema("stdp_ltp_weight", "double", "STDP LTP weight", "0.01", true),
            ConfigSchema("stdp_ltd_weight", "double", "STDP LTD weight", "0.012", true),
            ConfigSchema("stdp_time_constant", "double", "STDP time constant", "20.0", true),
            
            // Development settings
            ConfigSchema("development_synaptogenesis_rate", "double", "Development synaptogenesis rate", "0.001", true),
            ConfigSchema("development_pruning_rate", "double", "Development pruning rate", "0.0001", true),
            
            // Neuromodulation settings
            ConfigSchema("dopamine_baseline", "double", "Dopamine baseline level", "0.1", true),
            
            // Reward settings
            ConfigSchema("reward_discount_factor", "double", "Reward discount factor", "0.99", true),
            
            // Environment settings
            ConfigSchema("environment_name", "string", "Environment name", "GridWorld", true),
            ConfigSchema("environment_width", "int64", "Environment width", "10", true),
            ConfigSchema("environment_height", "int64", "Environment height", "10", true),
            
            // Logging settings
            ConfigSchema("log_level", "string", "Log level (TRACE, DEBUG, INFO, WARN, ERROR)", "INFO", true),
            ConfigSchema("log_to_file", "bool", "Log to file", "false", true),
            ConfigSchema("log_filename", "string", "Log filename", "nlm.log", true),
            
            // Simulation settings
            ConfigSchema("max_simulation_steps", "int64", "Maximum simulation steps", "10000", true),
            ConfigSchema("simulation_time_limit", "double", "Simulation time limit", "0.0", true),
            
            // Visualization settings
            ConfigSchema("visualization_enabled", "bool", "Visualization enabled", "false", true),
            ConfigSchema("visualization_update_rate", "double", "Visualization update rate", "30.0", true)
        };
    }
    
    bool validateConfigValue(const ConfigSchema& schema, const std::string& value) {
        if (schema.validationPattern.empty()) {
            return true; // No validation pattern specified
        }
        
        if (schema.type == "int64") {
            try {
                std::stoll(value);
                return true;
            } catch (...) {
                return false;
            }
        } else if (schema.type == "double") {
            try {
                std::stod(value);
                return true;
            } catch (...) {
                return false;
            }
        } else if (schema.type == "bool") {
            return value == "true" || value == "false" || value == "1" || value == "0";
        } else if (schema.type == "string") {
            return !value.empty();
        }
        
        return true;
    }
    
public:
    ExperimentConfigManager() {
        config = std::make_shared<Config>();n    }
    
    ~ExperimentConfigManager() {}
    
    void run(int argc, char* argv[]) {
        printBanner();
        
        // Parse command line arguments
        std::string command;
        std::string key;
        std::string value;
        std::string filename;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                showHelp();
                return;
            } else if (arg == "--list" || arg == "-l") {
                listConfigOptions();
                return;
            } else if (arg == "--save" || arg == "-s") {
                if (i + 1 < argc) {
                    filename = argv[++i];
                }
                saveConfigToFile(filename);
                return;
            } else if (arg == "--load" || arg == "-r") {
                if (i + 1 < argc) {
                    filename = argv[++i];
                }
                loadConfigFromFile(filename);
                return;
            } else if (arg == "--validate") {
                validateAllConfig();
                return;
            } else if (arg == "--set" || arg == "-x") {
                if (i + 2 < argc) {
                    key = argv[++i];
                    value = argv[++i];
                    setConfigValue(key, value);
                } else {
                    std::cout << "Error: Missing key and value for --set command" << std::endl;
                    showHelp();
                    return;
                }
            } else if (arg == "--get" || arg == "-g") {
                if (i + 1 < argc) {
                    key = argv[++i];
                    getConfigValue(key);
                } else {
                    std::cout << "Error: Missing key for --get command" << std::endl;
                    showHelp();
                    return;
                }
            } else if (arg == "--apply" || arg == "-a") {
                applyDefaultConfig();
                return;
            } else {
                std::cout << "Error: Unknown command '" << arg << "'" << std::endl;
                showHelp();
                return;
            }
        }
        
        if (argc <= 1) {
            showHelp();
        }
    }
    
    void printBanner() {
        std::cout << R"(
╔══════════════════════════════════════════════════════════════════╗
║                                                                  ║
║  NLM Experiment Configuration Manager                             ║
║                                                                  ║
║  Configure and manage NLM experiment parameters with ease.        ║
║                                                                  ║
╚══════════════════════════════════════════════════════════════════╝
" << std::endl;
    }
    
    void showHelp() {
        std::cout << "\n=== NLM Experiment Configuration Manager ===\n" << std::endl;
        std::cout << "Usage: experiment_config [OPTIONS]\n" << std::endl;
        std::cout << "\nCommands:\n" << std::endl;
        std::cout << "  --help, -h                    Show this help message\n" << std::endl;
        std::cout << "  --list, -l                    List all available configuration options\n" << std::endl;
        std::cout << "  --get, -g <key>              Get configuration value for key\n" << std::endl;
        std::cout << "  --set, -x <key> <value>      Set configuration value for key\n" << std::endl;
        std::cout << "  --load, -r <file>            Load configuration from file\n" << std::endl;
        std::cout << "  --save, -s <file>            Save configuration to file\n" << std::endl;
        std::cout << "  --apply, -a                  Apply default configuration\n" << std::endl;
        std::cout << "  --validate                    Validate current configuration\n" << std::endl;
        std::cout << "\nExamples:\n" << std::endl;
        std::cout << "  experiment_config --list\n" << std::endl;
        std::cout << "  experiment_config --set neuron_count 500\n" << std::endl;
        std::cout << "  experiment_config --get environment_width\n" << std::endl;
        std::cout << "  experiment_config --load my_config.cfg\n" << std::endl;
        std::cout << "  experiment_config --save production_config.cfg\n" << std::endl;
        std::cout << "\nConfiguration File Format:\n" << std::endl;
        std::cout << "  # Key = Value\n" << std::endl;
        std::cout << "  # Lines starting with # are comments\n" << std::endl;
        std::cout << "  neuron_count = 500\n" << std::endl;
        std::cout << "  environment_width = 20\n" << std::endl;
    }
    
    void listConfigOptions() {
        std::cout << "\n=== Available Configuration Options ===\n" << std::endl;
        
        auto schema = getConfigSchema();
        std::cout << "Key                           Type        Description                                    Default\n" << std::endl;
        std::cout << "---------------------------------------------------------------------------------------------\n" << std::endl;
        
        for (const auto& s : schema) {
            std::cout << s.key << " (" << s.type << ")"
                      << std::string(26 - s.key.length() - s.type.length(), ' ');
            std::cout << s.description;
            std::cout << std::string(42 - s.description.length(), ' ');
            std::cout << s.defaultValue << "\n";
        }
        
        std::cout << "\nTotal options: " << schema.size() << std::endl;
    }
    
    void setConfigValue(const std::string& key, const std::string& value) {
        // Find schema for validation
        auto schema = getConfigSchema();
        auto it = std::find_if(schema.begin(), schema.end(),
            [&key](const ConfigSchema& s) { return s.key == key; });n        
        if (it == schema.end()) {
            std::cout << "Error: Unknown configuration key: '" << key << "'" << std::endl;
            std::cout << "Use '--list' to see all available keys." << std::endl;
            return;
        }
        
        // Validate value
        if (!validateConfigValue(*it, value)) {
            std::cout << "Error: Invalid value '" << value << "' for key '" << key << "'.\n";
            std::cout << "Expected type: " << it->type << std::endl;
            if (!it->validationPattern.empty()) {
                std::cout << "Validation pattern: " << it->validationPattern << std::endl;
            }
            return;
        }
        
        // Set the value
        config->set(key, value, ConfigSource::Runtime);
        
        // Log the change
        std::cout << "Set '" << key << "' = '" << value << "'" << std::endl;
    }
    
    void getConfigValue(const std::string& key) {
        auto schema = getConfigSchema();
        auto it = std::find_if(schema.begin(), schema.end(),
            [&key](const ConfigSchema& s) { return s.key == key; });n        
        if (it == schema.end()) {
            std::cout << "Error: Unknown configuration key: '" << key << "'" << std::endl;
            std::cout << "Use '--list' to see all available keys." << std::endl;
            return;
        }
        
        if (config->has(key)) {
            if (it->type == "int64") {
                std::cout << key << " = " << config->getOr<int64_t>(key, std::stoll(it->defaultValue)) << std::endl;
            } else if (it->type == "double") {
                std::cout << key << " = " << config->getOr<double>(key, std::stod(it->defaultValue)) << std::endl;
            } else if (it->type == "bool") {
                std::cout << key << " = " << (config->getOr<bool>(key, it->defaultValue == "true") ? "true" : "false") << std::endl;
            } else if (it->type == "string") {
                std::cout << key << " = " << config->getOr<std::string>(key, it->defaultValue) << std::endl;
            }
        } else {
            std::cout << key << " = " << it->defaultValue << " (default, not set)\n" << std::endl;
        }
    }
    
    void loadConfigFromFile(const std::string& filename) {
        if (filename.empty()) {
            std::cout << "Error: No filename provided for --load command" << std::endl;
            return;
        }
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Could not open file '" << filename << "'" << std::endl;
            return;
        }
        
        if (config->loadFromFile(filename)) {
            std::cout << "Loaded configuration from: '" << filename << "'" << std::endl;
            
            // Show some key values
            std::cout << "\nCurrent configuration summary:" << std::endl;
            std::cout << config->summary() << std::endl;
        } else {
            std::cout << "Error: Failed to load configuration from '" << filename << "'" << std::endl;
        }
    }
    
    void saveConfigToFile(const std::string& filename) {
        if (filename.empty()) {
            std::cout << "Error: No filename provided for --save command" << std::endl;
            return;
        }
        
        if (config->saveToFile(filename)) {
            std::cout << "Saved configuration to: '" << filename << "'" << std::endl;
        } else {
            std::cout << "Error: Failed to save configuration to '" << filename << "'" << std::endl;
        }
    }
    
    void validateAllConfig() {
        std::cout << "=== Validating Configuration ===\n" << std::endl;
        
        auto schema = getConfigSchema();
        bool hasErrors = false;
        bool hasWarnings = false;
        
        for (const auto& s : schema) {
            if (config->has(s.key)) {
                if (!validateConfigValue(s, getConfigValueAsString(s.key))) {
                    std::cout << "ERROR: Invalid value for key '" << s.key << "'\n";
                    hasErrors = true;
                } else {
                    std::cout << "OK: '" << s.key << "' = " << getConfigValueAsString(s.key) << "\n";
                }
            } else if (s.required) {
                std::cout << "WARNING: Required key '" << s.key << "' is not set (using default)\n";
                hasWarnings = true;
            }
        }
        
        if (hasErrors) {
            std::cout << "\nValidation failed with errors!" << std::endl;
        } else if (hasWarnings) {
            std::cout << "\nValidation completed with warnings only." << std::endl;
        } else {
            std::cout << "\nValidation passed successfully!" << std::endl;
        }
    }
    
    std::string getConfigValueAsString(const std::string& key) {
        if (config->has(key)) {
            // Try different types
            if (config->has(key)) {
                auto strVal = config->get<std::string>(key);
                if (strVal) return *strVal;
            }
            if (config->has(key)) {
                auto intVal = config->get<int64_t>(key);
                if (intVal) return std::to_string(*intVal);
            }
            if (config->has(key)) {
                auto doubleVal = config->get<double>(key);
                if (doubleVal) return std::to_string(*doubleVal);
            }
            if (config->has(key)) {
                auto boolVal = config->get<bool>(key);
                if (boolVal) return *boolVal ? "true" : "false";
            }
        }
        return "";
    }
    
    void applyDefaultConfig() {
        std::cout << "Applying default configuration...\n" << std::endl;
        
        auto schema = getConfigSchema();
        for (const auto& s : schema) {
            // Convert default value to appropriate type
            if (s.type == "int64") {
                config->set(s.key, std::stoll(s.defaultValue), ConfigSource::Default);
            } else if (s.type == "double") {
                config->set(s.key, std::stod(s.defaultValue), ConfigSource::Default);
            } else if (s.type == "bool") {
                bool val = (s.defaultValue == "true" || s.defaultValue == "1");
                config->set(s.key, val, ConfigSource::Default);
            } else if (s.type == "string") {
                config->set(s.key, s.defaultValue, ConfigSource::Default);
            }
            
            std::cout << "  " << s.key << " = " << s.defaultValue << "\n";
        }
        
        std::cout << "\nDefault configuration applied successfully!\n" << std::endl;
    }
};

// Main entry point for experiment configuration manager
int main(int argc, char* argv[]) {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    nlm::ExperimentConfigManager configManager;
    configManager.run(argc, argv);
    
    return 0;
}
