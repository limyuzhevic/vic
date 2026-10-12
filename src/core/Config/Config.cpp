#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

// Validate configuration key-value pair
bool Config::validate(const std::string& key, const std::string& value) const {
    // Basic validation rules
    if (key.empty()) {
        return false; // Key cannot be empty
    }
    
    // Check for valid key characters (alphanumeric and underscores)
    if (!std::all_of(key.begin(), key.end(), [](char c) {
        return std::isalnum(c) || c == '_'; })) {
        return false;
    }
    
    // Validate value based on key type (basic checks)
    if (key == "random_seed") {
        try {
            int seed = std::stoi(value);
            return seed >= 0 && seed <= INT_MAX;
        } catch (...) {
            return false;
        }
    } else if (key == "simulation_timestep") {
        try {
            double timestep = std::stod(value);
            return timestep > 0.0 && timestep <= 1.0;
        } catch (...) {
            return false;
        }
    } else if (key == "neuron_count") {
        try {
            int count = std::stoi(value);
            return count > 0 && count <= 1000000;
        } catch (...) {
            return false;
        }
    } else if (key == "connection_probability") {
        try {
            double prob = std::stod(value);
            return prob >= 0.0 && prob <= 1.0;
        } catch (...) {
            return false;
        }
    } else if (key == "plasticity_learning_rate") {
        try {
            double rate = std::stod(value);
            return rate >= 0.0 && rate <= 1.0;
        } catch (...) {
            return false;
        }
    } else if (key == "dopamine_baseline") {
        try {
            double baseline = std::stod(value);
            return baseline >= 0.0 && baseline <= 1.0;
        } catch (...) {
            return false;
        }
    } else if (key == "reward_discount_factor") {
        try {
            double factor = std::stod(value);
            return factor >= 0.0 && factor <= 1.0;
        } catch (...) {
            return false;
        }
    } else if (key == "environment_width" || key == "environment_height") {
        try {
            int size = std::stoi(value);
            return size > 0 && size <= 10000;
        } catch (...) {
            return false;
        }
    } else if (key == "max_simulation_steps") {
        try {
            int steps = std::stoi(value);
            return steps > 0 && steps <= 1000000;
        } catch (...) {
            return false;
        }
    }
    
    // For other keys, accept any value
    return !value.empty();
}

// Validate configuration against schema
bool Config::validateSchema() const {
    // Check required keys
    const std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count",
        "connection_probability",
        "plasticity_learning_rate",
        "environment_name",
        "max_simulation_steps"
    };
    
    for (const auto& requiredKey : requiredKeys) {
        if (!has(requiredKey)) {
            return false;
        }
    }
    
    // Validate all entries
    for (const auto& entry : pImpl->entries) {
        if (!validate(entry.key, entry.value)) {
            return false;
        }
    }
    
    return true;
}

// Set with validation
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    // Validate if new entry or value change
    if (it == pImpl->entries.end() || it->value != value) {
        // Convert value to string for validation
        std::string valueStr;
        std::visit([&valueStr](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStr = arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::string>) {
                valueStr = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                valueStr = "[int array]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                valueStr = "[double array]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                valueStr = "[string array]";
            }
        }, value);
        
        if (!validate(key, valueStr)) {
            // Log validation error
            // TODO: Add proper logging
            return;
        }
    }
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

// Set with validation for specific types
template<typename T>
bool Config::setValidated(const std::string& key, const T& value, ConfigSource source) {
    // Convert to ConfigValue
    ConfigValue configValue;
    if constexpr (std::is_same_v<T, int>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, int64_t>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, double>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, bool>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, std::string>) {
        configValue = value;
    } else {
        return false; // Unsupported type
    }
    
    // Convert to string for validation
    std::string valueStr;
    std::visit([&valueStr](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            valueStr = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            valueStr = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            valueStr = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            valueStr = arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            valueStr = arg;
        }
    }, configValue);
    
    if (!validate(key, valueStr)) {
        return false;
    }
    
    set(key, configValue, source);
    return true;
}

// Get configuration diagnostics
std::string Config::getDiagnostics() const {
    std::ostringstream oss;
    oss << "Configuration diagnostics:\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
    oss << "  Source distribution: " << "\n";
    
    std::map<ConfigSource, int> sourceCount;
    for (const auto& entry : pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    for (const auto& pair : sourceCount) {
        oss << "    " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
    }
    
    oss << "  Configuration keys: ";
    for (const auto& key : getKeys()) {
        oss << key << " ";
    }
    oss << "\n";
    
    return oss.str();
}

private:
    // Configuration schema definitions
    struct ConfigSchema {
        std::string key;
        std::string type;
        std::string minValue;
        std::string maxValue;
        std::string description;
        bool required;
    };
    
    static const std::vector<ConfigSchema> defaultSchema;

// Initialize default schema
const std::vector<Config::ConfigSchema> Config::defaultSchema = {
    {"random_seed", "int", "0", "2147483647", "Random seed for reproducible experiments", true},
    {"simulation_timestep", "double", "0.0001", "0.1", "Simulation timestep in seconds", true},
    {"neuron_count", "int", "1", "1000000", "Number of neurons in the brain", true},
    {"region_count", "int", "1", "100", "Number of brain regions", true},
    {"connection_probability", "double", "0.0", "1.0", "Connection probability for random connectivity", true},
    {"plasticity_learning_rate", "double", "0.0", "1.0", "Plasticity learning rate", true},
    {"stdp_ltp_weight", "double", "0.0", "0.1", "STDP LTP weight", false},
    {"stdp_ltd_weight", "double", "0.0", "0.1", "STDP LTD weight", false},
    {"stdp_time_constant", "double", "0.0", "1000.0", "STDP time constant", false},
    {"development_synaptogenesis_rate", "double", "0.0", "1.0", "Developmental synaptogenesis rate", false},
    {"development_pruning_rate", "double", "0.0", "0.1", "Developmental pruning rate", false},
    {"dopamine_baseline", "double", "0.0", "1.0", "Neuromodulation baseline", true},
    {"reward_discount_factor", "double", "0.0", "1.0", "Reward discount factor", true},
    {"environment_name", "string", "", "", "Environment name", true},
    {"environment_width", "int", "1", "10000", "Environment width", true},
    {"environment_height", "int", "1", "10000", "Environment height", true},
    {"log_level", "string", "", "", "Log level (DEBUG, INFO, WARNING, ERROR)", false},
    {"log_to_file", "bool", "false", "true", "Log to file", false},
    {"log_filename", "string", "", "", "Log filename", false},
    {"max_simulation_steps", "int", "1", "1000000", "Maximum simulation steps", true},
    {"simulation_time_limit", "double", "0.0", "1000.0", "Simulation time limit", false},
    {"visualization_enabled", "bool", "false", "true", "Enable visualization", false},
    {"visualization_update_rate", "double", "1.0", "1000.0", "Visualization update rate", false}
};

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    // Enhanced error handling
    if (filepath.empty()) {
        return false;
    }
    
    // Check if file exists before attempting to open
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with enhanced error checking
        size_t pos = line.find('=');
        if (pos != std::string::npos && pos > 0) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key
            if (key.empty()) {
                continue; // Skip malformed lines
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else if (!line.empty()) {
            // Log warning for malformed line
            // TODO: Add proper logging
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Enhanced error handling for command line arguments
    if (!argv || argc <= 1) {
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos && pos > 2) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key format
                if (key.empty()) {
                    continue; // Skip malformed arguments
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                // Log warning for malformed argument
                // TODO: Add proper logging
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key format
            if (key.empty()) {
                continue; // Skip malformed arguments
            }
            
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Save the actual value
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        
        file << "\n";
    }
    
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
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
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
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
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
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
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

} // namespace nlm
