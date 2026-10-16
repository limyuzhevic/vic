// NLM Configuration System - Enhanced Error Handling and Validation
// Phase 6: Final Integration - Robust Configuration Management

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <regex>
#include <unordered_map>

namespace nlm {

namespace {

// Validation rules for configuration values
struct ValidationRules {
    static bool validateNeuronCount(int64_t value) {
        return value > 0 && value <= 100000;  // Reasonable upper limit
    }
    
    static bool validateRegionCount(int64_t value) {
        return value > 0 && value <= 1000;
    }
    
    static bool validateConnectionProbability(float value) {
        return value >= 0.0f && value <= 1.0f;
    }
    
    static bool validateSimulationTimestep(double value) {
        return value > 0.0 && value <= 1.0;  // Maximum 1 second timestep
    }
    
    static bool validateSTPWeight(float value) {
        return value >= -1.0f && value <= 1.0f;
    }
    
    static bool validateString(const std::string& value, size_t max_length = 1000) {
        return !value.empty() && value.length() <= max_length;
    }
    
    static bool isValidKey(const std::string& key) {
        // Allow alphanumeric, underscore, and hyphen
        static const std::regex key_pattern("^[a-zA-Z0-9_-]+$");
        return std::regex_match(key, key_pattern);
    }
};

// Type conversion utilities
class TypeConverter {
public:
    static int64_t toInt64(const std::string& str, int64_t default_value = 0) {
        try {
            size_t pos;
            int64_t value = std::stoll(str, &pos);
            if (pos != str.length()) {
                throw std::invalid_argument("Extra characters after number");
            }
            return value;
        } catch (const std::exception&) {
            return default_value;
        }
    }
    
    static float toFloat(const std::string& str, float default_value = 0.0f) {
        try {
            size_t pos;
            float value = std::stof(str, &pos);
            if (pos != str.length()) {
                throw std::invalid_argument("Extra characters after number");
            }
            return value;
        } catch (const std::exception&) {
            return default_value;
        }
    }
    
    static double toDouble(const std::string& str, double default_value = 0.0) {
        try {
            size_t pos;
            double value = std::stod(str, &pos);
            if (pos != str.length()) {
                throw std::invalid_argument("Extra characters after number");
            }
            return value;
        } catch (const std::exception&) {
            return default_value;
        }
    }
    
    static bool toBool(const std::string& str, bool default_value = false) {
        std::string lower = toLower(str);
        if (lower == "true" || lower == "yes" || lower == "1") {
            return true;
        } else if (lower == "false" || lower == "no" || lower == "0") {
            return false;
        }
        return default_value;
    }
    
    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
};

} // anonymous namespace

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::vector<std::string>> validation_errors;
    bool validation_enabled = true;
    
    // Statistics
    size_t total_reads = 0;
    size_t total_writes = 0;
    size_t validation_errors_count = 0;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Enable validation by default
    pImpl->validation_enabled = true;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_ERROR("Configuration file does not exist: " + filepath);
        return false;
    }
    
    // Check if file is readable
    if (!std::filesystem::is_regular_file(filepath) || !std::filesystem::exists(filepath + ".readable")) {
        NLM_LOG_ERROR("Configuration file is not readable: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    try {
        std::string line;
        size_t line_number = 0;
        
        while (std::getline(file, line)) {
            line_number++;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs with enhanced error handling
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                NLM_LOG_WARNING("Invalid configuration line " + std::to_string(line_number) + 
                               ": missing '=' in line: " + line);
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key format
            if (!ValidationRules::isValidKey(key)) {
                NLM_LOG_ERROR("Invalid configuration key at line " + std::to_string(line_number) + ": " + key);
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Parse value with type inference
            ConfigValue config_value = parseConfigValue(value);
            
            // Apply validation rules based on key
            if (!validateConfigValue(key, config_value)) {
                continue;  // Skip invalid values
            }
            
            // Set configuration value
            set(key, config_value, ConfigSource::File);
            
            pImpl->total_reads++;
        }
        
        NLM_LOG_INFO("Successfully loaded configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading configuration file ") + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc < 2) {
        return true;  // No arguments to parse
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key format
                if (!ValidationRules::isValidKey(key)) {
                    NLM_LOG_WARNING("Invalid command line key: " + key);
                    continue;
                }
                
                // Parse value
                ConfigValue config_value = parseConfigValue(value);
                set(key, config_value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            std::string key = arg.substr(1);
            
            // Validate key format
            if (!ValidationRules::isValidKey(key)) {
                NLM_LOG_WARNING("Invalid command line key: " + key);
                continue;
            }
            
            if (i + 1 < argc) {
                std::string value = argv[++i];
                
                // Parse value
                ConfigValue config_value = parseConfigValue(value);
                set(key, config_value, ConfigSource::CommandLine);
            }
        }
        // Handle positional arguments (key-value pairs)
        else {
            // Check if it's a key=value pair
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(arg.substr(0, pos));
                std::string value = trim(arg.substr(pos + 1));
                
                // Validate key format
                if (!ValidationRules::isValidKey(key)) {
                    NLM_LOG_WARNING("Invalid configuration key: " + key);
                    continue;
                }
                
                // Parse value
                ConfigValue config_value = parseConfigValue(value);
                set(key, config_value, ConfigSource::CommandLine);
            }
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    try {
        // Write header
        file << "# NLM Configuration File" << std::endl;
        file << "# Generated on " << __DATE__ << " " << __TIME__ << std::endl;
        file << std::endl;
        
        // Write configuration entries
        for (const auto& entry : pImpl->entries) {
            // Add comment with source
            file << "# Source: " << static_cast<int>(entry.source) << std::endl;
            file << "# Description: " << entry.description << std::endl;
            
            // Write key-value pair
            file << entry.key << " = ";
            
            // Format value based on type
            std::visit([&file](const auto& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << '"' << arg << '"';
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << std::endl << std::endl;
        }
        
        // Flush and verify write
        file.flush();
        if (!file.good()) {
            NLM_LOG_ERROR("Failed to write configuration file: " + filepath);
            return false;
        }
        
        NLM_LOG_INFO("Successfully saved configuration to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving configuration file ") + filepath + ": " + e.what());
        return false;
    }
}

ConfigValue Config::parseConfigValue(const std::string& value_str) {
    // Try to parse as different types based on content
    if (value_str.empty()) {
        return std::string();
    }
    
    // Check if it's a boolean
    std::string lower = toLower(value_str);
    if (lower == "true" || lower == "false") {
        return lower == "true";
    }
    
    // Check if it's an integer
    try {
        size_t pos;
        int64_t int_value = std::stoll(value_str, &pos);
        if (pos == value_str.length()) {
            return int_value;
        }
    } catch (const std::exception&) {}
    
    // Check if it's a float
    try {
        size_t pos;
        float float_value = std::stof(value_str, &pos);
        if (pos == value_str.length()) {
            return float_value;
        }
    } catch (const std::exception&) {}
    
    // Check if it's a double
    try {
        size_t pos;
        double double_value = std::stod(value_str, &pos);
        if (pos == value_str.length()) {
            return double_value;
        }
    } catch (const std::exception&) {}
    
    // Default to string
    return value_str;
}

bool Config::validateConfigValue(const std::string& key, const ConfigValue& value) {
    if (!pImpl->validation_enabled) {
        return true;
    }
    
    std::string error_message;
    
    // Validate based on key
    if (key == "neuron_count") {
        if (std::holds_alternative<int64_t>(value)) {
            if (!ValidationRules::validateNeuronCount(std::get<int64_t>(value))) {
                error_message = "Invalid neuron count: " + std::to_string(std::get<int64_t>(value));
            }
        } else {
            error_message = "Neuron count must be an integer";
        }
    } else if (key == "region_count") {
        if (std::holds_alternative<int64_t>(value)) {
            if (!ValidationRules::validateRegionCount(std::get<int64_t>(value))) {
                error_message = "Invalid region count: " + std::to_string(std::get<int64_t>(value));
            }
        } else {
            error_message = "Region count must be an integer";
        }
    } else if (key == "connection_probability") {
        if (std::holds_alternative<float>(value)) {
            if (!ValidationRules::validateConnectionProbability(std::get<float>(value))) {
                error_message = "Invalid connection probability: " + std::to_string(std::get<float>(value));
            }
        } else {
            error_message = "Connection probability must be a float";
        }
    } else if (key == "simulation_timestep") {
        if (std::holds_alternative<double>(value)) {
            if (!ValidationRules::validateSimulationTimestep(std::get<double>(value))) {
                error_message = "Invalid simulation timestep: " + std::to_string(std::get<double>(value));
            }
        } else {
            error_message = "Simulation timestep must be a double";
        }
    } else if (key == "stdp_ltp_weight" || key == "stdp_ltd_weight") {
        if (std::holds_alternative<float>(value)) {
            if (!ValidationRules::validateSTPWeight(std::get<float>(value))) {
                error_message = "Invalid STP weight: " + std::to_string(std::get<float>(value));
            }
        } else {
            error_message = "STP weight must be a float";
        }
    } else if (key == "random_seed") {
        if (std::holds_alternative<int64_t>(value)) {
            if (std::get<int64_t>(value) < 0) {
                error_message = "Random seed must be non-negative: " + std::to_string(std::get<int64_t>(value));
            }
        } else {
            error_message = "Random seed must be an integer";
        }
    } else {
        // Validate string keys
        if (std::holds_alternative<std::string>(value)) {
            if (!ValidationRules::validateString(std::get<std::string>(value))) {
                error_message = "Invalid string value: " + std::get<std::string>(value);
            }
        }
    }
    
    if (!error_message.empty()) {
        NLM_LOG_ERROR(error_message);
        pImpl->validation_errors[key].push_back(error_message);
        pImpl->validation_errors_count++;
        return false;
    }
    
    return true;
}

// Template function implementations with error handling
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    pImpl->total_reads++;
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARNING("Type mismatch when retrieving configuration key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key format
    if (!ValidationRules::isValidKey(key)) {
        NLM_LOG_ERROR("Invalid configuration key: " + key);
        return;
    }
    
    // Validate value based on key
    if (!validateConfigValue(key, value)) {
        NLM_LOG_ERROR("Invalid configuration value for key: " + key);
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        pImpl->total_writes++;
    } else {
        pImpl->entries.emplace_back(key, value, source);
        pImpl->total_writes++;
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
    pImpl->validation_errors.clear();
    pImpl->validation_errors_count = 0;
    pImpl->total_reads = 0;
    pImpl->total_writes = 0;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary:" << std::endl;
    oss << "=====================" << std::endl;
    oss << "Total entries: " << pImpl->entries.size() << std::endl;
    oss << "Total reads: " << pImpl->total_reads << std::endl;
    oss << "Total writes: " << pImpl->total_writes << std::endl;
    oss << "Validation errors: " << pImpl->validation_errors_count << std::endl;
    oss << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")" << std::endl;
    }
    
    return oss.str();
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
