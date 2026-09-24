#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cmath>
#include <cctype>

namespace nlm {

// Configuration validation constants and functions
namespace ConfigValidation {
    const std::unordered_map<std::string, std::vector<std::function<bool(const ConfigValue&)>>> VALIDATORS = {
        {"neuron_count", { [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; } }},
        {"region_count", { [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; } }},
        {"simulation_timestep", { [](const ConfigValue& v) { 
            return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0 && std::get<double>(v) <= 1.0; } }},
        {"connection_probability", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; } }},
        {"stdp_ltp_weight", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f; } }},
        {"stdp_ltd_weight", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f; } }},
        {"stdp_tau", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) > 0.0f; } }},
        {"synaptogenesis_rate", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f; } }},
        {"pruning_rate", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f; } }},
        {"learning_rate", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; } }},
        {"memory_capacity", { [](const ConfigValue& v) { 
            return std::holds_alternative<int>(v) && std::get<int>(v) > 0; } }},
        {"curiosity_weight_novelty", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f; } }},
        {"curiosity_weight_prediction_error", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f; } }},
        {"exploration_threshold", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; } }},
        {"max_exploration_chance", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; } }},
        {"social_tie_strength", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; } }},
        {"cooperation_probability", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; } }},
        {"group_size", { [](const ConfigValue& v) { 
            return std::holds_alternative<int>(v) && std::get<int>(v) > 0; } }},
        {"empathy_level", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; } }},
        {"adaptation_threshold", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; } }},
        {"energy_decay_rate", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; } }},
        {"sensory_vision_scale", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f; } }},
        {"sensory_touch_scale", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f; } }},
        {"sensory_internal_scale", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f; } }},
        {"sensory_proprioception_scale", { [](const ConfigValue& v) { 
            return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f; } }},
    };
    
    const std::unordered_map<std::string, std::pair<double, double>> RANGES = {
        {"neuron_count", {1.0, 100000.0}},
        {"region_count", {1.0, 100.0}},
        {"simulation_timestep", {0.0, 1.0}},
        {"connection_probability", {0.0, 1.0}},
        {"stdp_ltp_weight", {0.0, 1.0}},
        {"stdp_ltd_weight", {0.0, 1.0}},
        {"stdp_tau", {0.0, 100.0}},
        {"synaptogenesis_rate", {0.0, 1.0}},
        {"pruning_rate", {0.0, 1.0}},
        {"learning_rate", {0.0, 1.0}},
        {"memory_capacity", {1.0, 1000000.0}},
        {"curiosity_weight_novelty", {0.0, 10.0}},
        {"curiosity_weight_prediction_error", {0.0, 10.0}},
        {"exploration_threshold", {0.0, 1.0}},
        {"max_exploration_chance", {0.0, 1.0}},
        {"social_tie_strength", {0.0, 1.0}},
        {"cooperation_probability", {0.0, 1.0}},
        {"group_size", {1.0, 1000.0}},
        {"empathy_level", {0.0, 1.0}},
        {"adaptation_threshold", {0.0, 1.0}},
        {"energy_decay_rate", {0.0, 1.0}},
    };
    
    const std::unordered_map<std::string, std::string> DESCRIPTIONS = {
        {"random_seed", "Random seed for reproducible simulations"},
        {"simulation_timestep", "Simulation timestep in seconds"},
        {"neuron_count", "Number of neurons in the network"},
        {"region_count", "Number of neural regions"},
        {"connection_probability", "Probability of synaptic connections between neurons"},
        {"stdp_ltp_weight", "STDP long-term potentiation weight"},
        {"stdp_ltd_weight", "STDP long-term depression weight"},
        {"stdp_tau", "STDP time constant"},
        {"synaptogenesis_rate", "Rate of new synapse formation"},
        {"pruning_rate", "Rate of synapse pruning"},
        {"learning_rate", "Learning rate for plasticity"},
        {"memory_capacity", "Capacity of memory systems"},
        {"curiosity_weight_novelty", "Weight of novelty in curiosity calculation"},
        {"curiosity_weight_prediction_error", "Weight of prediction error in curiosity calculation"},
        {"exploration_threshold", "Threshold for curiosity-based exploration"},
        {"max_exploration_chance", "Maximum probability of exploration"},
        {"social_tie_strength", "Strength of social connections"},
        {"cooperation_probability", "Probability of cooperative behavior"},
        {"group_size", "Size of social groups"},
        {"empathy_level", "Level of empathetic processing"},
        {"adaptation_threshold", "Threshold for behavioral adaptation"},
        {"energy_decay_rate", "Rate of energy decay over time"},
        {"sensory_vision_scale", "Scaling factor for vision sensory input"},
        {"sensory_touch_scale", "Scaling factor for touch sensory input"},
        {"sensory_internal_scale", "Scaling factor for internal sensory input"},
        {"sensory_proprioception_scale", "Scaling factor for proprioception input"},
    };
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    mutable bool validationEnabled;
    std::string validationErrors;
    
    Impl() : validationEnabled(true), validationErrors() {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Check if file exists
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
            
            // Parse key=value pairs with support for various value formats
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                continue; // Skip malformed lines
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Convert string value to appropriate type based on key
            ConfigValue configValue = stringToConfigValue(value, key);
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                ConfigValue configValue = stringToConfigValue(value, key);
                set(key, configValue, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            ConfigValue configValue = stringToConfigValue(value, key);
            set(key, configValue, ConfigSource::CommandLine);
        }
    }
    return true;
}

ConfigValue Config::stringToConfigValue(const std::string& str, const std::string& key) {
    // Try different types in order of likelihood
    try {
        // Check if it's a boolean
        if (str == "true" || str == "false") {
            return str == "true";
        }
        
        // Check if it's an integer (with optional suffix)
        size_t pos;
        int64_t intValue;
        if (tryParseInt(str, intValue)) {
            return intValue;
        }
        
        // Check if it's a float
        float floatValue;
        if (tryParseFloat(str, floatValue)) {
            return floatValue;
        }
        
        // Default to string
        return str;
    } catch (...) {
        return str;
    }
}

bool Config::tryParseInt(const std::string& str, int64_t& result) {
    size_t pos;
    try {
        size_t endPos;
        if (str.find(' ') != std::string::npos) {
            return false;
        }
        
        // Handle hexadecimal and octal
        if (str.substr(0, 2) == "0x" || str.substr(0, 2) == "0X") {
            result = std::stoll(str, &pos, 16);
        } else if (str.substr(0, 1) == "0") {
            result = std::stoll(str, &pos, 8);
        } else {
            result = std::stoll(str, &pos);
        }
        
        // Ensure we parsed the entire string
        return pos == str.length();
    } catch (...) {
        return false;
    }
}

bool Config::tryParseFloat(const std::string& str, float& result) {
    size_t pos;
    try {
        if (str.find(' ') != std::string::npos) {
            return false;
        }
        
        double doubleValue;
        doubleValue = std::stod(str, &pos);
        
        // Ensure we parsed the entire string
        if (pos != str.length()) {
            return false;
        }
        
        // Check for NaN or infinity
        if (std::isnan(doubleValue) || std::isinf(doubleValue)) {
            return false;
        }
        
        result = static_cast<float>(doubleValue);
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# NLM Configuration File\n";
        file << "# Generated by NLM on " << getCurrentTimestamp() << "\n\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << getDescription(entry.key) << "\n";
            file << entry.key << " = ";
            
            // Convert ConfigValue to string
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, float>) {
                    file << arg;
                } else {
                    file << "unknown";
                }
            }, entry.value);
            
            file << "\n\n";
        }
        
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        if constexpr (std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<int64_t>(it->value)) {
                return std::get<int64_t>(it->value);
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(it->value)) {
                return std::get<double>(it->value);
            }
            // Also accept float as double
            if (std::holds_alternative<float>(it->value)) {
                return static_cast<double>(std::get<float>(it->value));
            }
        } else if constexpr (std::is_same_v<T, float>) {
            if (std::holds_alternative<float>(it->value)) {
                return std::get<float>(it->value);
            }
            // Also accept double as float
            if (std::holds_alternative<double>(it->value)) {
                return static_cast<float>(std::get<double>(it->value));
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(it->value)) {
                return std::get<bool>(it->value);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(it->value)) {
                return std::get<std::string>(it->value);
            }
        } else if constexpr (std::is_same_v<T, int>) {
            if (std::holds_alternative<int64_t>(it->value)) {
                return static_cast<int>(std::get<int64_t>(it->value));
            }
        }
        
        return std::nullopt;
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
    try {
        // Get description if available
        std::string description = getDescription(key);
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source, description);
        }
        
        // Validate if validation is enabled
        if (pImpl->validationEnabled) {
            validateSingleKey(key, value);
        }
        
    } catch (...) {
        // Silently fail for set operations to maintain robustness
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue configValue = stringToConfigValue(value, key);
    set(key, configValue, source);
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
        oss << "] (" << static_cast<int>(entry.source) << ") - " << entry.description << "\n";
    }
    return oss.str();
}

bool Config::validate() {
    if (!pImpl->validationEnabled) {
        return true;
    }
    
    pImpl->validationErrors.clear();
    
    for (const auto& entry : pImpl->entries) {
        validateSingleKey(entry.key, entry.value);
    }
    
    return pImpl->validationErrors.empty();
}

void Config::validateSingleKey(const std::string& key, const ConfigValue& value) {
    std::string normalizedKey = toLower(key);
    
    // Check if there's a validator for this key
    auto validatorIt = ConfigValidation::VALIDATORS.find(normalizedKey);
    if (validatorIt != ConfigValidation::VALIDATORS.end()) {
        for (const auto& validator : validatorIt->second) {
            if (!validator(value)) {
                pImpl->validationErrors += "Invalid value for key \"" + key + "\": ";
                
                // Add specific error message
                if (normalizedKey == "neuron_count") {
                    pImpl->validationErrors += "must be a positive integer\n";
                } else if (normalizedKey == "simulation_timestep") {
                    pImpl->validationErrors += "must be between 0.0 and 1.0\n";
                } else if (normalizedKey == "connection_probability") {
                    pImpl->validationErrors += "must be between 0.0 and 1.0\n";
                } else {
                    pImpl->validationErrors += "does not meet validation criteria\n";
                }
            }
        }
    }
    
    // Check range if defined
    auto rangeIt = ConfigValidation::RANGES.find(normalizedKey);
    if (rangeIt != ConfigValidation::RANGES.end()) {
        double min_val, max_val;
        std::tie(min_val, max_val) = rangeIt->second;
        
        // Extract numeric value for range checking
        double numericValue = extractNumericValue(value);
        if (numericValue < 0.0 || numericValue > max_val) {
            pImpl->validationErrors += "Key \"" + key + "\" must be between " + std::to_string(min_val) + 
                                      " and " + std::to_string(max_val) + "\n";
        }
    }
}

double Config::extractNumericValue(const ConfigValue& value) {
    try {
        if (std::holds_alternative<int64_t>(value)) {
            return static_cast<double>(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        } else if (std::holds_alternative<float>(value)) {
            return static_cast<double>(std::get<float>(value));
        }
    } catch (...) {
        // Return 0.0 if we can't extract a value
    }
    return 0.0;
}

std::string Config::getDescription(const std::string& key) {
    std::string normalizedKey = toLower(key);
    auto it = ConfigValidation::DESCRIPTIONS.find(normalizedKey);
    if (it != ConfigValidation::DESCRIPTIONS.end()) {
        return it->second;
    }
    return "";
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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
