// Config validation and helper functions
namespace nlm {

namespace ConfigValidation {
    // Common config key validators
    bool validateKey(const std::string& key) {
        // Key should not be empty
        if (key.empty()) return false;
        
        // Key should contain only alphanumeric characters, underscores, and hyphens
        for (char c : key) {
            if (!std::isalnum(c) && c != '_' && c != '-') {
                return false;
            }
        }
        
        return true;
    }
    
    // Type conversion functions for validation
    bool toBool(const std::string& value) {
        std::string lower = toLower(value);
        return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
    }
    
    int toInt(const std::string& value) {
        try {
            return std::stoi(value);
        } catch (...) {
            return 0;
        }
    }
    
    double toDouble(const std::string& value) {
        try {
            return std::stod(value);
        } catch (...) {
            return 0.0;
        }
    }
}

// Include ConfigParser header for format detection
#include "ConfigFormatDetector.hpp"

// Forward declare for loading
bool Config::loadFromFile(const std::string& filepath);

// Update Config.cpp with improved implementation
Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Use ConfigFormatDetector to detect format and parse
    nlm::ConfigParser::ConfigMap configMap;
    
    if (!nlm::ConfigParser::parse(filepath, configMap)) {
        return false;
    }
    
    // Clear existing entries and add new ones
    clear();
    
    for (const auto& pair : configMap) {
        // Convert string value to appropriate type based on key name
        std::string key = pair.first;
        const auto& value = pair.second;
        
        // Try to determine type from value (for string values)
        if (std::holds_alternative<std::string>(value)) {
            const std::string& strValue = std::get<std::string>(value);
            
            // Try to convert to bool
            std::string lowerValue = toLower(strValue);
            if (lowerValue == "true" || lowerValue == "false" || 
                lowerValue == "1" || lowerValue == "0" ||
                lowerValue == "yes" || lowerValue == "no" ||
                lowerValue == "on" || lowerValue == "off") {
                set(key, ConfigValidation::toBool(strValue), ConfigSource::File);
            } else if (lowerValue.find('.') != std::string::npos) {
                // Try to convert to double
                double doubleVal = ConfigValidation::toDouble(strValue);
                if (!std::isnan(doubleVal)) {
                    set(key, doubleVal, ConfigSource::File);
                } else {
                    set(key, strValue, ConfigSource::File);
                }
            } else {
                // Try to convert to int
                try {
                    int intVal = std::stoi(strValue);
                    set(key, intVal, ConfigSource::File);
                } catch (...) {
                    set(key, strValue, ConfigSource::File);
                }
            }
        } else {
            // Already a typed value, copy it
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Handle --help flag
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --help flag
        if (arg == "--help" || arg == "-help") {
            printHelp();
            return true;
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

void Config::printHelp() {
    std::cout << "NLM Configuration Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --config=<file>           Load configuration from file" << std::endl;
    std::cout << "  --<key>=<value>           Set configuration value" << std::endl;
    std::cout << "  -<key> <value>            Set configuration value (alternative format)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Common configuration keys:" << std::endl;
    std::cout << "  random_seed               Random seed for reproducible experiments" << std::endl;
    std::cout << "  simulation_timestep       Simulation timestep in seconds" << std::endl;
    std::cout << "  neuron_count              Number of neurons in the brain" << std::endl;
    std::cout << "  region_count              Number of brain regions" << std::endl;
    std::cout << "  connection_probability    Connection probability for random connectivity" << std::endl;
    std::cout << "  stdp_ltp_weight           STDP LTP weight" << std::endl;
    std::cout << "  stdp_ltd_weight           STDP LTD weight" << std::endl;
    std::cout << "  synaptogenesis_rate       Synaptogenesis rate" << std::endl;
    std::cout << "  pruning_rate              Pruning rate" << std::endl;
    std::cout << "  dopamine_baseline         Dopamine baseline value" << std::endl;
    std::cout << "  reward_discount_factor    Reward discount factor" << std::endl;
    std::cout << "  log_level                 Log level (DEBUG, INFO, WARNING, ERROR)" << std::endl;
    std::cout << "  log_to_file               Log to file (true/false)" << std::endl;
    std::cout << "  max_simulation_steps     Maximum simulation steps" << std::endl;
    std::cout << "  visualization_enabled     Enable visualization (true/false)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "File formats supported:" << std::endl;
    std::cout << "  .cfg, .txt               Key=value format" << std::endl;
    std::cout << "  .json                    JSON format" << std::endl;
    std::cout << "  .yaml, .yml              YAML format" << std::endl;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Detect format from extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        // Use ConfigParser to serialize to JSON
        nlm::ConfigParser::ConfigMap configMap;
        for (const auto& entry : pImpl->entries) {
            std::visit([&configMap, &entry](auto&& arg) {
                configMap[entry.key] = arg;
            }, entry.value);
        }
        file << nlm::ConfigParser::serializeToJSON(configMap);
    } else if (ext == ".yaml" || ext == ".yml") {
        // Use ConfigParser to serialize to YAML
        nlm::ConfigParser::ConfigMap configMap;
        for (const auto& entry : pImpl->entries) {
            std::visit([&configMap, &entry](auto&& arg) {
                configMap[entry.key] = arg;
            }, entry.value);
        }
        file << nlm::ConfigParser::serializeToYAML(configMap);
    } else {
        // Default to key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            // Serialize value based on its type
            std::visit([&file, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << entry.key << " = " << arg << "\n";
                } else if constexpr (std::is_same_v<T, double>) {
                    file << entry.key << " = " << arg << "\n";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                }
            }, entry.value);
        }
    }
    
    return true;
}

void Config::validateAndSet(const std::string& key, const std::string& value, ConfigSource source) {
    // Validate key
    if (!ConfigValidation::validateKey(key)) {
        throw std::invalid_argument("Invalid configuration key: " + key);
    }
    
    // Convert string value to appropriate type based on key name
    std::string lowerKey = toLower(key);
    
    // Special validation for known keys
    if (lowerKey == "random_seed") {
        try {
            int64_t seed = std::stoll(value);
            if (seed < 0) {
                throw std::invalid_argument("Random seed must be non-negative");
            }
            set(key, seed, source);
        } catch (...) {
            throw std::invalid_argument("Invalid value for random_seed: must be an integer");
        }
    } else if (lowerKey == "simulation_timestep") {
        try {
            double timestep = std::stod(value);
            if (timestep <= 0.0) {
                throw std::invalid_argument("Simulation timestep must be positive");
            }
            set(key, timestep, source);
        } catch (...) {
            throw std::invalid_argument("Invalid value for simulation_timestep: must be a positive number");
        }
    } else if (lowerKey == "neuron_count" || lowerKey == "region_count" || lowerKey == "max_simulation_steps") {
        try {
            int64_t count = std::stoll(value);
            if (count <= 0) {
                throw std::invalid_argument("Count must be positive");
            }
            set(key, count, source);
        } catch (...) {
            throw std::invalid_argument("Invalid value for " + key + ": must be a positive integer");
        }
    } else if (lowerKey == "connection_probability" || lowerKey == "stdp_ltp_weight" || 
               lowerKey == "stdp_ltd_weight" || lowerKey == "stdp_time_constant" ||
               lowerKey == "synaptogenesis_rate" || lowerKey == "pruning_rate" ||
               lowerKey == "dopamine_baseline" || lowerKey == "reward_discount_factor" ||
               lowerKey == "visualization_update_rate") {
        try {
            double val = std::stod(value);
            set(key, val, source);
        } catch (...) {
            throw std::invalid_argument("Invalid value for " + key + ": must be a number");
        }
    } else if (lowerKey == "log_level") {
        std::string level = toLower(value);
        if (level != "debug" && level != "info" && level != "warning" && level != "error") {
            throw std::invalid_argument("Invalid log level: must be DEBUG, INFO, WARNING, or ERROR");
        }
        set(key, value, source);
    } else if (lowerKey == "log_to_file" || lowerKey == "visualization_enabled") {
        std::string boolVal = toLower(value);
        if (boolVal != "true" && boolVal != "false" && boolVal != "1" && boolVal != "0") {
            throw std::invalid_argument("Invalid boolean value for " + key + ": must be true/false or 0/1");
        }
        set(key, ConfigValidation::toBool(value), source);
    } else {
        // Generic key - try to convert to appropriate type
        std::string lowerValue = toLower(value);
        
        if (lowerValue == "true" || lowerValue == "false" || 
            lowerValue == "1" || lowerValue == "0" ||
            lowerValue == "yes" || lowerValue == "no" ||
            lowerValue == "on" || lowerValue == "off") {
            set(key, ConfigValidation::toBool(value), source);
        } else if (lowerValue.find('.') != std::string::npos) {
            try {
                double doubleVal = std::stod(value);
                set(key, doubleVal, source);
            } catch (...) {
                set(key, value, source);
            }
        } else {
            try {
                int intVal = std::stoi(value);
                set(key, intVal, source);
            } catch (...) {
                set(key, value, source);
            }
        }
    }
}

// Update set methods to use validation
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    validateAndSet(key, value, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    validateAndSet(key, std::to_string(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    validateAndSet(key, std::to_string(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    validateAndSet(key, value ? "true" : "false", source);
}

} // namespace nlm
