#include <iostream>
#include <sstream>
#include <ctime>

namespace nlm {

// Get version string
std::string getVersion() {
    std::ostringstream version;
    version << "NLM v0.1.0";
    return version.str();
}

} // namespace nlm

using namespace nlm;

namespace {
    // Configuration validation rules
    struct ValidationRule {
        std::string key;
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
    };
    
    // Get validation rules for all configuration keys
    std::vector<ValidationRule> getValidationRules() {
        return {
            // Neuron count validation
            {"neuron_count", 
             [](const ConfigValue& v) { 
                 return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0 && std::get<int64_t>(v) <= 1000000; 
             }, 
             "neuron_count must be a positive integer <= 1000000"},
            
            // Region count validation
            {"region_count",
             [](const ConfigValue& v) {
                 return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) >= 1 && std::get<int64_t>(v) <= 100;
             },
             "region_count must be an integer between 1 and 100"},
            
            // Connection probability validation
            {"connection_probability",
             [](const ConfigValue& v) {
                 return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0;
             },
             "connection_probability must be a number between 0.0 and 1.0"},
            
            // Simulation timestep validation
            {"simulation_timestep",
             [](const ConfigValue& v) {
                 return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0 && std::get<double>(v) <= 1.0;
             },
             "simulation_timestep must be a positive number <= 1.0"},
            
            // STDP parameters validation
            {"stdp_ltp_weight",
             [](const ConfigValue& v) {
                 return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0;
             },
             "stdp_ltp_weight must be non-negative"},
            
            {"stdp_ltd_weight",
             [](const ConfigValue& v) {
                 return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0;
             },
             "stdp_ltd_weight must be non-negative"},
            
            {"stdp_tau",
             [](const ConfigValue& v) {
                 return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0;
             },
             "stdp_tau must be positive"},
            
            // Plasticity parameters validation
            {"synaptogenesis_rate",
             [](const ConfigValue& v) {
                 return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0;
             },
             "synaptogenesis_rate must be between 0.0 and 1.0"},
            
            {"pruning_rate",
             [](const ConfigValue& v) {
                 return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0;
             },
             "pruning_rate must be between 0.0 and 1.0"},
            
            // Reward parameters validation
            {"reward_discount_factor",
             [](const ConfigValue& v) {
                 return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0;
             },
             "reward_discount_factor must be between 0.0 and 1.0"},
            
            // Development parameters validation
            {"development_synaptogenesis_rate",
             [](const ConfigValue& v) {
                 return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0;
             },
             "development_synaptogenesis_rate must be between 0.0 and 1.0"},
            
            {"development_pruning_rate",
             [](const ConfigValue& v) {
                 return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0;
             },
             "development_pruning_rate must be between 0.0 and 1.0"},
            
            // Neuromodulation parameters validation
            {"dopamine_baseline",
             [](const ConfigValue& v) {
                 return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0;
             },
             "dopamine_baseline must be non-negative"},
            
            // Environment parameters validation
            {"environment_width",
             [](const ConfigValue& v) {
                 return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) >= 1 && std::get<int64_t>(v) <= 10000;
             },
             "environment_width must be a positive integer <= 10000"},
            
            {"environment_height",
             [](const ConfigValue& v) {
                 return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) >= 1 && std::get<int64_t>(v) <= 10000;
             },
             "environment_height must be a positive integer <= 10000"},
            
            // Visualization parameters validation
            {"visualization_update_rate",
             [](const ConfigValue& v) {
                 return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0;
             },
             "visualization_update_rate must be positive"},
            
            // Log level validation
            {"log_level",
             [](const ConfigValue& v) {
                 return std::holds_alternative<std::string>(v);
                 // Note: We can't validate enum values without including the enum header
             },
             "log_level must be a string"},
        };
    }
    
    // Validate a specific configuration key
    bool validateKey(const std::string& key, const ConfigValue& value, std::string& error) {
        const auto& rules = getValidationRules();
        for (const auto& rule : rules) {
            if (rule.key == key) {
                if (!rule.validator(value)) {
                    error = rule.errorMessage;
                    return false;
                }
                return true;
            }
        }
        return true; // No validation rule for this key
    }
    
    // Validate all configuration entries
    bool validateConfiguration(const Config& config, std::vector<std::string>& errors) {
        errors.clear();
        
        for (const auto& entry : config.getKeys()) {
            auto valueOpt = config.get<std::string>(entry);
            if (!valueOpt) {
                continue; // Skip if can't get value
            }
            
            // Convert to appropriate type for validation
            ConfigValue value;
            const std::string& strValue = *valueOpt;
            
            // Try to parse as different types
            try {
                if (strValue.find('.') != std::string::npos) {
                    value = std::stod(strValue);
                } else {
                    value = std::stoll(strValue);
                }
            } catch (...) {
                // If parsing fails, try boolean
                if (strValue == "true" || strValue == "false") {
                    value = (strValue == "true");
                } else {
                    value = strValue; // Keep as string
                }
            }
            
            std::string error;
            if (!validateKey(entry, value, error)) {
                errors.push_back("Key '" + entry + "': " + error);
            }
        }
        
        return errors.empty();
    }
    
    // Apply default values for missing keys
    void applyDefaults(Config& config) {
        const auto& defaults = getDefaultValues();
        
        for (const auto& defaultPair : defaults) {
            if (!config.has(defaultPair.first)) {
    // Try different types
    try {
        if (defaultPair.second.find('.') != std::string::npos) {
            config.set(defaultPair.first, std::stod(defaultPair.second), ConfigSource::Default);
        } else {
            config.set(defaultPair.first, std::stoll(defaultPair.second), ConfigSource::Default);
        }
    } catch (...) {
        // Try integer
        try {
            config.set(defaultPair.first, std::stoll(defaultPair.second), ConfigSource::Default);
        } catch (...) {
            // Try boolean
            if (defaultPair.second == "true") {
                config.set(defaultPair.first, true, ConfigSource::Default);
            } else if (defaultPair.second == "false") {
                config.set(defaultPair.first, false, ConfigSource::Default);
            } else {
                // Keep as string
                config.set(defaultPair.first, defaultPair.second, ConfigSource::Default);
            }
        }
    }
            }
        }
    }
    
    // Get default configuration values
    std::map<std::string, std::string> getDefaultValues() {
        return {
            {"random_seed", "42"},
            {"simulation_timestep", "0.001"},
            {"neuron_count", "1000"},
            {"region_count", "2"},
            {"connection_probability", "0.1"},
            {"plasticity_learning_rate", "0.01"},
            {"stdp_ltp_weight", "0.01"},
            {"stdp_ltd_weight", "0.012"},
            {"stdp_time_constant", "20.0"},
            {"development_synaptogenesis_rate", "0.001"},
            {"development_pruning_rate", "0.0001"},
            {"dopamine_baseline", "0.1"},
            {"reward_discount_factor", "0.99"},
            {"environment_name", "GridWorld"},
            {"environment_width", "10"},
            {"environment_height", "10"},
            {"log_level", "INFO"},
            {"log_to_file", "false"},
            {"log_filename", "nlm.log"},
            {"max_simulation_steps", "10000"},
            {"simulation_time_limit", "0.0"},
            {"visualization_enabled", "false"},
            {"visualization_update_rate", "30.0"},
        };
    }
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Apply default values after initialization
    applyDefaults(*this);
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO: Implement proper JSON/YAML parser
    // For now, use the simple key=value format but with validation
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Configuration file not found: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Set the value with file source
            set(key, value, ConfigSource::File);
            
            // Validate the value
            std::vector<std::string> errors;
            bool isValid = validateConfiguration(*this, errors);
            if (!isValid) {
                NLM_LOG_ERROR("Configuration error in " + filepath + ":" + std::to_string(lineNumber) +
                             ": " + errors.back());
                hasErrors = true;
            }
        } else {
            NLM_LOG_WARNING("Invalid configuration line in " + filepath + ":" + std::to_string(lineNumber) +
                           ": " + line);
        }
    }
    
    if (hasErrors) {
        NLM_LOG_ERROR("Configuration file " + filepath + " contains validation errors");
        return false;
    }
    
    NLM_LOG_INFO("Loaded configuration from: " + filepath);
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool hasChanges = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
                hasChanges = true;
                
                // Validate
                std::vector<std::string> errors;
                if (!validateConfiguration(*this, errors)) {
                    NLM_LOG_ERROR("Invalid command line argument: --" + key + "=" + value + ": " + errors.back());
                    return false;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            hasChanges = true;
            
            // Validate
            std::vector<std::string> errors;
            if (!validateConfiguration(*this, errors)) {
                NLM_LOG_ERROR("Invalid command line argument: -" + key + " " + value + ": " + errors.back());
                return false;
            }
        }
    }
    
    if (hasChanges) {
        NLM_LOG_INFO("Configuration updated from command line arguments");
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM " << getVersion() << "\n";
    file << "# \n";
    file << "# This is a human-readable configuration format.\n";
    file << "# Format: key = value\n";
    file << "# \n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Format the value appropriately
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        file << "\n";
    }
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARNING("Type mismatch for key '" + key + "': expected " + typeid(T).name());
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    } else {
        NLM_LOG_DEBUG("Using default value for key '" + key + "': " + std::to_string(defaultValue));
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        
        // Validate when setting
        std::vector<std::string> errors;
        if (!validateConfiguration(*this, errors)) {
            NLM_LOG_WARNING("Configuration key '" + key + "' validation warning: " + errors.back());
        }
    } else {
        pImpl->entries.emplace_back(key, value, source);
        
        // Validate new entry
        std::vector<std::string> errors;
        if (!validateConfiguration(*this, errors)) {
            NLM_LOG_WARNING("New configuration key '" + key + "' validation warning: " + errors.back());
        }
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(),
                      pImpl->entries.end(),
                      [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    // Add validation status
    std::vector<std::string> errors;
    if (!validateConfiguration(*this, errors)) {
        oss << "\nValidation errors (" << errors.size() << "):\n";
        for (const auto& error : errors) {
            oss << "  - " << error << "\n";
        }
    } else {
        oss << "\n✓ Configuration is valid" << std::endl;
    }
    
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Explicit template instantiations
namespace {
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    
    template int Config::getOr<int>(const std::string&, const int&) const;
    template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
}

} // namespace nlm