#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <set>
#include <sstream>
#include <iomanip>
#include <numeric>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigSchema> schemas;
    mutable std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 3: Implement proper JSON/YAML parser
    // Enhanced format with validation and type checking
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse enhanced key=value pairs
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
            
            // Try to parse value based on type
            ConfigValue parsedValue;
            bool parsed = false;
            
            // Try boolean
            if (!parsed) {
                if (value == "true" || value == "false") {
                    parsedValue = (value == "true");
                    parsed = true;
                }
            }
            
            // Try int
            if (!parsed) {
                try {
                    size_t posEnd;
                    int intVal = std::stoi(value, &posEnd);
                    if (posEnd == value.size()) {
                        parsedValue = intVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try int64_t
            if (!parsed) {
                try {
                    size_t posEnd;
                    long long intVal = std::stoll(value, &posEnd);
                    if (posEnd == value.size()) {
                        parsedValue = intVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try double
            if (!parsed) {
                try {
                    size_t posEnd;
                    double doubleVal = std::stod(value, &posEnd);
                    if (posEnd == value.size()) {
                        parsedValue = doubleVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try vector<int>
            if (!parsed && value.find('[') != std::string::npos && value.find(']') != std::string::npos) {
                std::vector<int> intVec;
                size_t start = value.find('[');
                size_t end = value.find(']');
                if (start < end) {
                    std::string content = value.substr(start + 1, end - start - 1);
                    std::stringstream ss(content);
                    std::string item;
                    while (std::getline(ss, item, ',')) {
                        item = trim(item);
                        try {
                            int val = std::stoi(item);
                            intVec.push_back(val);
                        } catch (...) {
                            break;
                        }
                    }
                    if (!intVec.empty()) {
                        parsedValue = intVec;
                        parsed = true;
                    }
                }
            }
            
            // Try vector<double>
            if (!parsed && value.find('[') != std::string::npos && value.find(']') != std::string::npos && 
                (value.find('.') != std::string::npos || value.find('e') != std::string::npos)) {
                std::vector<double> doubleVec;
                size_t start = value.find('[');
                size_t end = value.find(']');
                if (start < end) {
                    std::string content = value.substr(start + 1, end - start - 1);
                    std::stringstream ss(content);
                    std::string item;
                    while (std::getline(ss, item, ',')) {
                        item = trim(item);
                        try {
                            double val = std::stod(item);
                            doubleVec.push_back(val);
                        } catch (...) {
                            break;
                        }
                    }
                    if (!doubleVec.empty()) {
                        parsedValue = doubleVec;
                        parsed = true;
                    }
                }
            }
            
            // Fall back to string
            if (!parsed) {
                parsedValue = value;
            }
            
            // Apply validation before setting
            if (validateWithSchema(key, parsedValue, ConfigSource::File)) {
                set(key, parsedValue, ConfigSource::File);
            }
        }
    }
    
    return true;
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
                
                // Try to parse value based on type
                ConfigValue parsedValue;
                bool parsed = false;
                
                // Try boolean
                if (!parsed) {
                    if (value == "true" || value == "false") {
                        parsedValue = (value == "true");
                        parsed = true;
                    }
                }
                
                // Try int
                if (!parsed) {
                    try {
                        size_t posEnd;
                        int intVal = std::stoi(value, &posEnd);
                        if (posEnd == value.size()) {
                            parsedValue = intVal;
                            parsed = true;
                        }
                    } catch (...) {}
                }
                
                // Try int64_t
                if (!parsed) {
                    try {
                        size_t posEnd;
                        long long intVal = std::stoll(value, &posEnd);
                        if (posEnd == value.size()) {
                            parsedValue = intVal;
                            parsed = true;
                        }
                    } catch (...) {}
                }
                
                // Try double
                if (!parsed) {
                    try {
                        size_t posEnd;
                        double doubleVal = std::stod(value, &posEnd);
                        if (posEnd == value.size()) {
                            parsedValue = doubleVal;
                            parsed = true;
                        }
                    } catch (...) {}
                }
                
                // Fall back to string
                if (!parsed) {
                    parsedValue = value;
                }
                
                // Apply validation before setting
                if (validateWithSchema(key, parsedValue, ConfigSource::CommandLine)) {
                    set(key, parsedValue, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            ConfigValue parsedValue;
            bool parsed = false;
            
            // Try boolean
            if (!parsed) {
                if (value == "true" || value == "false") {
                    parsedValue = (value == "true");
                    parsed = true;
                }
            }
            
            // Try int
            if (!parsed) {
                try {
                    size_t posEnd;
                    int intVal = std::stoi(value, &posEnd);
                    if (posEnd == value.size()) {
                        parsedValue = intVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try int64_t
            if (!parsed) {
                try {
                    size_t posEnd;
                    long long intVal = std::stoll(value, &posEnd);
                    if (posEnd == value.size()) {
                        parsedValue = intVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try double
            if (!parsed) {
                try {
                    size_t posEnd;
                    double doubleVal = std::stod(value, &posEnd);
                    if (posEnd == value.size()) {
                        parsedValue = doubleVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Fall back to string
            if (!parsed) {
                parsedValue = value;
            }
            
            // Apply validation before setting
            if (validateWithSchema(key, parsedValue, ConfigSource::CommandLine)) {
                set(key, parsedValue, ConfigSource::CommandLine);
            }
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
        file << entry.key << " = " << toJsonString(entry.value) << "\n";
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
        if constexpr (std::is_same_v<T, float>) {
            if (std::holds_alternative<double>(it->value)) {
                return static_cast<float>(std::get<double>(it->value));
            }
        }
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
    // Apply validation before setting
    if (!validateWithSchema(key, value, source)) {
        throw ConfigValidationError(
            "Validation failed for key: " + key + ". Value is out of bounds or invalid.",
            key, value, source
        );
    }
    
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
    ConfigValue parsedValue;
    bool parsed = false;
    
    // Try boolean
    if (!parsed) {
        if (value == "true" || value == "false") {
            parsedValue = (value == "true");
            parsed = true;
        }
    }
    
    // Try int
    if (!parsed) {
        try {
            size_t posEnd;
            int intVal = std::stoi(value, &posEnd);
            if (posEnd == value.size()) {
                parsedValue = intVal;
                parsed = true;
            }
        } catch (...) {}
    }
    
    // Try int64_t
    if (!parsed) {
        try {
            size_t posEnd;
            long long intVal = std::stoll(value, &posEnd);
            if (posEnd == value.size()) {
                parsedValue = intVal;
                parsed = true;
            }
        } catch (...) {}
    }
    
    // Try double
    if (!parsed) {
        try {
            size_t posEnd;
            double doubleVal = std::stod(value, &posEnd);
            if (posEnd == value.size()) {
                parsedValue = doubleVal;
                parsed = true;
            }
        } catch (...) {}
    }
    
    // Fall back to string
    if (!parsed) {
        parsedValue = value;
    }
    
    set(key, parsedValue, source);
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

void Config::addSchema(const std::string& category, const ConfigSchema& schema) {
    pImpl->schemas[category] = schema;
}

bool Config::validateParameter(const std::string& key) const {
    pImpl->validationErrors.clear();
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return true;  // No validation needed for non-existent keys
    }
    
    bool isValid = validateWithSchema(key, it->value, it->source);
    
    if (!isValid) {
        pImpl->validationErrors.push_back("Validation failed for key: " + key);
    }
    
    return isValid;
}

bool Config::validateAll() const {
    pImpl->validationErrors.clear();
    bool allValid = true;
    
    for (const auto& entry : pImpl->entries) {
        if (!validateWithSchema(entry.key, entry.value, entry.source)) {
            pImpl->validationErrors.push_back("Validation failed for key: " + entry.key);
            allValid = false;
        }
    }
    
    return allValid;
}

std::vector<std::string> Config::getValidationErrors() const {
    if (pImpl->validationErrors.empty()) {
        validateAll();
    }
    return pImpl->validationErrors;
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

// Enhanced API implementations

void Config::setBatch(const std::unordered_map<std::string, ConfigValue>& values, ConfigSource source) {
    for (const auto& pair : values) {
        try {
            set(pair.first, pair.second, source);
        } catch (const ConfigValidationError& e) {
            // Log error but continue with other values
            std::cerr << "Warning: Failed to set key '" << pair.first << "': " << e.what() << std::endl;
        }
    }
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Check if key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            // Add new entry
            pImpl->entries.push_back(entry);
        } else {
            // Update existing entry, keeping the source from other if it's higher priority
            if (entry.source > it->source) {
                it->value = entry.value;
                it->source = entry.source;
            }
        }
    }
}

Config Config::copy() const {
    Config result;
    result.pImpl->entries = pImpl->entries;
    result.pImpl->schemas = pImpl->schemas;
    return result;
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == other.pImpl->entries.end() || it->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> Config::findDifferences(const Config& other) const {
    std::vector<std::string> differences;
    
    // Check for keys in this config but not in other
    for (const auto& entry : pImpl->entries) {
        bool found = false;
        for (const auto& otherEntry : other.pImpl->entries) {
            if (entry.key == otherEntry.key) {
                found = true;
                if (entry.value != otherEntry.value) {
                    differences.push_back("Key '" + entry.key + "' has different values");
                }
                break;
            }
        }
        if (!found) {
            differences.push_back("Key '" + entry.key + "' is missing in other config");
        }
    }
    
    // Check for keys in other but not in this
    for (const auto& entry : other.pImpl->entries) {
        bool found = false;
        for (const auto& thisEntry : pImpl->entries) {
            if (entry.key == thisEntry.key) {
                found = true;
                break;
            }
        }
        if (!found) {
            differences.push_back("Key '" + entry.key + "' is missing in this config");
        }
    }
    
    return differences;
}

Config Config::createMinimalBrainConfig() {
    Config config;
    config.set("neuron_count", 100, ConfigSource::Default);
    config.set("region_count", 1, ConfigSource::Default);
    config.set("connection_probability", 0.1, ConfigSource::Default);
    config.set("random_seed", 42, ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    return config;
}

Config Config::createStandardConfig() {
    Config config;
    config.set("neuron_count", 500, ConfigSource::Default);
    config.set("region_count", 1, ConfigSource::Default);
    config.set("connection_probability", 0.15, ConfigSource::Default);
    config.set("random_seed", 42, ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.02, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.015, ConfigSource::Default);
    config.set("stdp_tau", 20.0, ConfigSource::Default);
    config.set("synaptogenesis_rate", 0.0001, ConfigSource::Default);
    config.set("pruning_rate", 0.00001, ConfigSource::Default);
    return config;
}

Config Config::createHighPlasticityConfig() {
    Config config;
    config.set("neuron_count", 500, ConfigSource::Default);
    config.set("region_count", 1, ConfigSource::Default);
    config.set("connection_probability", 0.15, ConfigSource::Default);
    config.set("random_seed", 42, ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.05, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.04, ConfigSource::Default);
    config.set("stdp_tau", 50.0, ConfigSource::Default);
    config.set("learning_rate", 0.5, ConfigSource::Default);
    config.set("max_weight", 1.0, ConfigSource::Default);
    return config;
}

Config Config::createDevelopmentConfig() {
    Config config;
    config.set("neuron_count", 100, ConfigSource::Default);
    config.set("region_count", 1, ConfigSource::Default);
    config.set("connection_probability", 0.1, ConfigSource::Default);
    config.set("random_seed", 42, ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    config.set("maturation_rate", 0.1, ConfigSource::Default);
    config.set("pruning_threshold", 0.01, ConfigSource::Default);
    config.set("refinement_rate", 0.05, ConfigSource::Default);
    return config;
}

Config Config::createNeuromodulationConfig() {
    Config config;
    config.set("dopamine_level", 0.5, ConfigSource::Default);
    config.set("curiosity_level", 0.5, ConfigSource::Default);
    config.set("novelty_threshold", 0.3, ConfigSource::Default);
    config.set("decay_rate", 0.1, ConfigSource::Default);
    return config;
}

std::string Config::exportToString() const {
    std::ostringstream oss;
    oss << "NLM Config Export\n";
    oss << "=================\n\n";
    
    oss << "Entries (" << pImpl->entries.size() << "):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = " << toJsonString(entry.value) << "\n";
    }
    
    oss << "\nSchemas (" << pImpl->schemas.size() << "):\n";
    for (const auto& schemaPair : pImpl->schemas) {
        oss << "  " << schemaPair.first << ": " << schemaPair.second.name << "\n";
    }
    
    return oss.str();
}

bool Config::importFromString(const std::string& data) {
    std::istringstream iss(data);
    std::string line;
    
    // Parse simple key=value pairs
    while (std::getline(iss, line)) {
        line = trim(line);
        if (line.empty() || line.find("=") == std::string::npos) {
            continue;
        }
        
        size_t pos = line.find('=');
        std::string key = trim(line.substr(0, pos));
        std::string valueStr = trim(line.substr(pos + 1));
        
        // Try to parse value type
        ConfigValue value;
        bool parsed = false;
        
        // Try boolean
        if (!parsed) {
            if (valueStr == "true" || valueStr == "false") {
                value = (valueStr == "true");
                parsed = true;
            }
        }
        
        // Try int
        if (!parsed) {
            try {
                size_t posEnd;
                int val = std::stoi(valueStr, &posEnd);
                if (posEnd == valueStr.size()) {
                    value = val;
                    parsed = true;
                }
            } catch (...) {}
        }
        
        // Try double
        if (!parsed) {
            try {
                size_t posEnd;
                double val = std::stod(valueStr, &posEnd);
                if (posEnd == valueStr.size()) {
                    value = val;
                    parsed = true;
                }
            } catch (...) {}
        }
        
        // Fall back to string
        if (!parsed) {
            value = valueStr;
        }
        
        set(key, value, ConfigSource::File);
    }
    
    return true;
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

std::string Config::toJsonString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

ConfigValue Config::fromJsonString(const std::string& str) {
    // Simple parsing - for production would use a proper JSON parser
    if (str == "true" || str == "false") {
        return str == "true";
    }
    
    // Try int
    try {
        size_t posEnd;
        int val = std::stoi(str, &posEnd);
        if (posEnd == str.size()) {
            return val;
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t posEnd;
        double val = std::stod(str, &posEnd);
        if (posEnd == str.size()) {
            return val;
        }
    } catch (...) {}
    
    // Try vector
    if (str.find('[') != std::string::npos && str.find(']') != std::string::npos) {
        std::vector<double> vec;
        size_t start = str.find('[');
        size_t end = str.find(']');
        if (start < end) {
            std::string content = str.substr(start + 1, end - start - 1);
            std::stringstream ss(content);
            std::string item;
            while (std::getline(ss, item, ',')) {
                item = trim(item);
                try {
                    double val = std::stod(item);
                    vec.push_back(val);
                } catch (...) {
                    break;
                }
            }
            if (!vec.empty()) {
                return vec;
            }
        }
    }
    
    // Fall back to string
    return str;
}

bool Config::validateWithSchema(const std::string& key, const ConfigValue& value, ConfigSource source) const {
    // Check if any schema applies to this key
    for (const auto& schemaPair : pImpl->schemas) {
        const auto& schema = schemaPair.second;
        
        // Check if schema contains this key
        auto boundsIt = schema.parameterBounds.find(key);
        if (boundsIt == schema.parameterBounds.end()) {
            continue;
        }
        
        // Validate against schema bounds
        const auto& bounds = boundsIt->second;
        
        try {
            if (std::holds_alternative<int>(value)) {
                double val = static_cast<double>(std::get<int>(value));
                if (!bounds.isValid(val)) {
                    return false;
                }
            } else if (std::holds_alternative<int64_t>(value)) {
                double val = static_cast<double>(std::get<int64_t>(value));
                if (!bounds.isValid(val)) {
                    return false;
                }
            } else if (std::holds_alternative<double>(value)) {
                double val = std::get<double>(value);
                if (!bounds.isValid(val)) {
                    return false;
                }
            } else if (std::holds_alternative<bool>(value)) {
                return true; // Boolean values always valid
            } else if (std::holds_alternative<std::string>(value)) {
                return true; // String values always valid
            } else if (std::holds_alternative<std::vector<int>>(value)) {
                const auto& vec = std::get<std::vector<int>>(value);
                if (!std::all_of(vec.begin(), vec.end(),
                    [&bounds](int v) { return bounds.isValid(static_cast<double>(v)); })) {
                    return false;
                }
            } else if (std::holds_alternative<std::vector<double>>(value)) {
                const auto& vec = std::get<std::vector<double>>(value);
                if (!std::all_of(vec.begin(), vec.end(),
                    [&bounds](double v) { return bounds.isValid(v); })) {
                    return false;
                }
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                return true; // String vector values always valid
            }
        } catch (...) {
            return false;
        }
    }
    
    return true;
}

// Explicit template instantiations
namespace nlm {

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

} // namespace nlm
