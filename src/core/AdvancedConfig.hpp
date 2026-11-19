#include "core/Config/Config.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

namespace nlm {

class AdvancedConfig {
public:
    AdvancedConfig() : config_(std::make_shared<Config>()) {}
    
    // Load configuration from file, environment variables, and command line
    bool loadConfiguration(const std::string& filename, int argc, char** argv) {
        bool success = true;
        
        // Load from file
        if (!filename.empty() && !config_->loadFromFile(filename)) {
            std::cerr << "Warning: Could not load config file: " << filename << std::endl;
            // Continue with defaults and environment variables
        }
        
        // Load from environment variables
        success = loadFromEnvironmentVariables() && success;
        
        // Load from command line
        success = config_->loadFromArgs(argc, argv) && success;
        
        return success;
    }
    
    // Load configuration from environment variables
    bool loadFromEnvironmentVariables() {
        bool success = true;
        
        // Common environment variables
        const char* env_vars[] = {
            "NLM_RANDOM_SEED",
            "NLM_SIMULATION_TIMESTEP",
            "NLM_NEURON_COUNT",
            "NLM_REGION_COUNT",
            "NLM_CONNECTION_PROBABILITY",
            "NLM_STDP_LTP_WEIGHT",
            "NLM_STDP_LTD_WEIGHT",
            "NLM_STDP_TIME_CONSTANT",
            "NLM_SYNAPTOGENESIS_RATE",
            "NLM_PRUNING_RATE",
            "NLM_DOPAMINE_BASELINE",
            "NLM_REWARD_DISCOUNT_FACTOR",
            "NLM_LOG_LEVEL",
            "NLM_CHECKPOINT_DIR"
        };
        
        for (const char* env_var : env_vars) {
            const char* value = std::getenv(env_var);
            if (value) {
                std::string key = env_var;
                // Convert NLM_ prefix to lowercase and remove prefix
                if (key.find("NLM_") == 0) {
                    key = key.substr(4);
                    // Convert to lowercase
                    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                }
                
                std::string str_value(value);
                setFromString(key, str_value);
            }
        }
        
        return success;
    }
    
    // Set configuration value from string (automatically detect type)
    void setFromString(const std::string& key, const std::string& value) {
        if (value.empty()) return;
        
        // Try to parse as different types
        try {
            // Check if it's a boolean
            if (value == "true" || value == "false") {
                bool bool_val = (value == "true");
                config_->set(key, bool_val, ConfigSource::Runtime);
                return;
            }
            
            // Try integer
            size_t pos;
            int int_val = std::stoi(value, &pos);
            if (pos == value.size()) {
                config_->set(key, int_val, ConfigSource::Runtime);
                return;
            }
            
            // Try double
            double double_val = std::stod(value, &pos);
            if (pos == value.size()) {
                config_->set(key, double_val, ConfigSource::Runtime);
                return;
            }
            
            // Fall back to string
            config_->set(key, value, ConfigSource::Runtime);
            
        } catch (const std::exception&) {
            // If parsing fails, treat as string
            config_->set(key, value, ConfigSource::Runtime);
        }
    }
    
    // Get configuration value as string
    std::string getAsString(const std::string& key) const {
        auto val = config_->get<std::string>(key);
        if (val.has_value()) {
            return val.value();
        }
        return "";
    }
    
    // Get configuration value as boolean
    bool getAsBool(const std::string& key, bool default_value = false) const {
        auto val = config_->get<bool>(key);
        if (val.has_value()) {
            return val.value();
        }
        return default_value;
    }
    
    // Get configuration value as integer
    int getAsInt(const std::string& key, int default_value = 0) const {
        auto val = config_->get<int>(key);
        if (val.has_value()) {
            return val.value();
        }
        return default_value;
    }
    
    // Get configuration value as double
    double getAsDouble(const std::string& key, double default_value = 0.0) const {
        auto val = config_->get<double>(key);
        if (val.has_value()) {
            return val.value();
        }
        return default_value;
    }
    
    // Get the underlying config object
    std::shared_ptr<Config> getConfig() const {
        return config_;
    }
    
    // Add configuration option with description
    void addOption(const std::string& key, const std::string& value, 
                   const std::string& description = "", ConfigSource source = ConfigSource::Default) {
        config_->set(key, value, source);
        // Note: ConfigEntry doesn't have description field, so we can't store descriptions
        // For a full implementation, we'd need to extend the ConfigEntry structure
    }
    
    // Print configuration summary
    void printConfiguration() const {
        std::cout << "=== NLM Configuration ===" << std::endl;
        std::cout << config_->summary() << std::endl;
        std::cout << "========================" << std::endl;
    }
    
    // Validate configuration
    bool validateConfiguration() const {
        // Check required fields
        if (!config_->has("neuron_count")) {
            std::cerr << "Error: 'neuron_count' is required" << std::endl;
            return false;
        }
        
        if (!config_->has("region_count")) {
            std::cerr << "Error: 'region_count' is required" << std::endl;
            return false;
        }
        
        // Validate ranges
        auto neuron_count = config_->get<int64_t>("neuron_count");
        if (neuron_count && neuron_count.value() < 10) {
            std::cerr << "Warning: 'neuron_count' should be at least 10" << std::endl;
        }
        
        auto region_count = config_->get<int64_t>("region_count");
        if (region_count && region_count.value() < 1) {
            std::cerr << "Error: 'region_count' should be at least 1" << std::endl;
            return false;
        }
        
        auto connection_prob = config_->get<double>("connection_probability");
        if (connection_prob && (connection_prob.value() < 0.0 || connection_prob.value() > 1.0)) {
            std::cerr << "Error: 'connection_probability' must be between 0.0 and 1.0" << std::endl;
            return false;
        }
        
        return true;
    }
    
    // Get configuration file path from command line or default
    std::string getConfigFilePath(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg.find("--config") != std::string::npos) {
                if (arg.find('=') != std::string::npos) {
                    return arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    return argv[++i];
                }
            }
        }
        return "configs/default.cfg";
    }
    
private:
    std::shared_ptr<Config> config_;
};

// Global configuration manager instance
AdvancedConfig& getGlobalConfig() {
    static AdvancedConfig config;
    return config;
}

// Convenience function to initialize configuration
bool initializeConfiguration(int argc, char** argv, const std::string& config_filename = "") {
    std::string filename = config_filename.empty() ? 
        getGlobalConfig().getConfigFilePath(argc, argv) : config_filename;
    
    AdvancedConfig& config = getGlobalConfig();
    bool success = config.loadConfiguration(filename, argc, argv);
    
    if (success) {
        if (!config.validateConfiguration()) {
            return false;
        }
        config.printConfiguration();
    }
    
    return success;
}

// Convenience function to get configuration value
std::string getConfigString(const std::string& key) {
    return getGlobalConfig().getAsString(key);
}

// Get configuration value with default
int getConfigInt(const std::string& key, int default_value = 0) {
    return getGlobalConfig().getAsInt(key, default_value);
}

// Get configuration value with default
bool getConfigBool(const std::string& key, bool default_value = false) {
    return getGlobalConfig().getAsBool(key, default_value);
}

} // namespace nlm