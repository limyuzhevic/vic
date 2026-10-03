#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <limits>

namespace nlm {

struct ConfigSchema::Impl {
    std::map<std::string, ConfigConstraint> constraints;
};

ConfigSchema::ConfigSchema() : pImpl(std::make_unique<Impl>()) {}

ConfigSchema::~ConfigSchema() = default;

void ConfigSchema::addConstraint(const ConfigConstraint& constraint) {
    pImpl->constraints[constraint.key] = constraint;
}

ValidationResult ConfigSchema::validate(const std::string& key, const ConfigValue& value) const {
    auto it = pImpl->constraints.find(key);
    if (it == pImpl->constraints.end()) {
        return ValidationResult::success();  // No constraint means no validation error
    }
    
    const ConfigConstraint& constraint = it->second;
    
    // Validate type
    if (!constraint.typeName.empty()) {
        ValidationResult typeResult = validateType(key, value, constraint.typeName);
        if (!typeResult.isValid) {
            return typeResult;
        }
    }
    
    // Validate range
    if (!constraint.minValue.empty() || !constraint.maxValue.empty()) {
        ValidationResult rangeResult = validateRange(key, value, constraint.minValue, constraint.maxValue);
        if (!rangeResult.isValid) {
            return rangeResult;
        }
    }
    
    // Validate enum values
    if (!constraint.allowedValues.empty()) {
        ValidationResult enumResult = validateEnum(key, value, constraint.allowedValues);
        if (!enumResult.isValid) {
            return enumResult;
        }
    }
    
    return ValidationResult::success();
}

bool ConfigSchema::hasConstraint(const std::string& key) const {
    return pImpl->constraints.find(key) != pImpl->constraints.end();
}

std::optional<ConfigConstraint> ConfigSchema::getConstraint(const std::string& key) const {
    auto it = pImpl->constraints.find(key);
    if (it != pImpl->constraints.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<ConfigConstraint> ConfigSchema::getAllConstraints() const {
    std::vector<ConfigConstraint> constraints;
    constraints.reserve(pImpl->constraints.size());
    for (const auto& pair : pImpl->constraints) {
        constraints.push_back(pair.second);
    }
    return constraints;
}

std::string ConfigSchema::getTypeName(const std::string& key) const {
    auto it = pImpl->constraints.find(key);
    if (it != pImpl->constraints.end()) {
        return it->second.typeName;
    }
    return "";
}

std::string ConfigSchema::getDescription(const std::string& key) const {
    auto it = pImpl->constraints.find(key);
    if (it != pImpl->constraints.end()) {
        return it->second.description;
    }
    return "";
}

std::vector<std::string> ConfigSchema::getAllowedKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->constraints.size());
    for (const auto& pair : pImpl->constraints) {
        keys.push_back(pair.first);
    }
    return keys;
}

// Config implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> stringCache;
};

Config::Config() : pImpl(std::make_unique<Impl>()), schema_() {
    initializeDefaults();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
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
        
        // Parse simple key=value pairs with support for various types
        size_t pos = line.find('=');
        if (pos != std::string&npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to parse based on schema
            if (schema_.hasConstraint(key)) {
                auto constraint = schema_.getConstraint(key);
                if (constraint) {
                    ConfigValue parsedValue;
                    bool parsed = false;
                    
                    if (constraint->typeName == "int") {
                        try {
                            parsedValue = std::stoi(value);
                            parsed = true;
                        } catch (...) {}
                    } else if (constraint->typeName == "int64_t") {
                        try {
                            parsedValue = std::stoll(value);
                            parsed = true;
                        } catch (...) {}
                    } else if (constraint->typeName == "float") {
                        try {
                            parsedValue = std::stof(value);
                            parsed = true;
                        } catch (...) {}
                    } else if (constraint->typeName == "double") {
                        try {
                            parsedValue = std::stod(value);
                            parsed = true;
                        } catch (...) {}
                    } else if (constraint->typeName == "bool") {
                        if (value == "true" || value == "1" || value == "yes") {
                            parsedValue = true;
                            parsed = true;
                        } else if (value == "false" || value == "0" || value == "no") {
                            parsedValue = false;
                            parsed = true;
                        }
                    } else if (constraint->typeName == "string") {
                        parsedValue = value;
                        parsed = true;
                    }
                    
                    if (parsed) {
                        // Validate before setting
                        ValidationResult validation = schema_.validate(key, parsedValue);
                        if (validation.isValid) {
                            set(key, parsedValue, ConfigSource::File);
                        } else {
                            // Log validation error but continue
                        }
                        continue;
                    }
                }
            }
            
            // Fallback: try to infer type
            ConfigValue parsedValue;
            bool parsed = false;
            
            if (isFloatString(value)) {
                float floatVal = stringToFloat(value);
                // Check if it's an integer
                if (std::abs(floatVal - static_cast<int>(floatVal)) < 1e-6f) {
                    parsedValue = static_cast<int>(floatVal);
                } else {
                    parsedValue = floatVal;
                }
                parsed = true;
            } else if (value == "true" || value == "false") {
                parsedValue = (value == "true");
                parsed = true;
            } else if (value.find(',') != std::string::npos) {
                // Vector parsing
                std::vector<float> floatVector;
                std::stringstream ss(value);
                std::string item;
                while (std::getline(ss, item, ',')) {
                    item = trim(item);
                    if (isFloatString(item)) {
                        floatVector.push_back(stringToFloat(item));
                    }
                }
                if (!floatVector.empty()) {
                    // Convert to appropriate vector type based on schema
                    parsedValue = floatVector;
                    parsed = true;
                }
            } else {
                // String value
                parsedValue = value;
                parsed = true;
            }
            
            if (parsed) {
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
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<float>>) {
                // Convert vector to comma-separated list
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ",";
                    file << arg[i];
                }
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
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

template<typename T>
T Config::getFloat(const std::string& key, float defaultValue) const {
    static_assert(std::is_same_v<T, float>, "getFloat requires float type");
    auto val = get<float>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
T Config::getSizeT(const std::string& key, size_t defaultValue) const {
    static_assert(std::is_same_v<T, size_t>, "getSizeT requires size_t type");
    auto val = get<size_t>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
T Config::getInt64(const std::string& key, int64_t defaultValue) const {
    static_assert(std::is_same_v<T, int64_t>, "getInt64 requires int64_t type");
    auto val = get<int64_t>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate using schema if available
    ValidationResult validation = schema_.validate(key, value);
    
    if (!validation.isValid) {
        // For now, we'll log the validation error but continue
        // In a production system, this might throw or use a proper logging system
        // std::cerr << "Config validation error for key " << key << ": " << validation.errorMessage << std::endl;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    // Clear string cache
    pImpl->stringCache.erase(key);
}

void Config::set(const std::string& key, float value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, size_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::setValidated(const std::string& key, const ConfigValue& value, ConfigSource source) {
    ValidationResult validation = schema_.validate(key, value);
    
    if (!validation.isValid) {
        return false;
    }
    
    set(key, value, source);
    return true;
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
    pImpl->stringCache.erase(key);
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
    pImpl->stringCache.clear();
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
            } else if constexpr (std::is_same_v<T, std::vector<float>>) {
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ",";
                    oss << arg[i];
                }
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

void Config::initializeDefaults() {
    // Initialize configuration schema with validation constraints
    ConfigSchema& schema = getSchema();
    
    // Add configuration constraints for better validation
    schema.addConstraint(ConfigConstraint("random_seed", "int64_t", "Random number generator seed"));
    schema.addConstraint(ConfigConstraint("simulation_timestep", "float", "Simulation time step in seconds", "0.001", "0.0001", "1.0"));
    schema.addConstraint(ConfigConstraint("neuron_count", "size_t", "Number of neurons in the network", "500", "1", "10000"));
    schema.addConstraint(ConfigConstraint("region_count", "size_t", "Number of neural regions", "1", "1", "10"));
    schema.addConstraint(ConfigConstraint("connection_probability", "float", "Probability of synaptic connections", "0.15", "0.0", "1.0"));
    
    // STDP parameters
    schema.addConstraint(ConfigConstraint("stdp_ltp_weight", "float", "STDP long-term potentiation weight", "0.02", "0.0", "1.0"));
    schema.addConstraint(ConfigConstraint("stdp_ltd_weight", "float", "STDP long-term depression weight", "0.015", "0.0", "1.0"));
    schema.addConstraint(ConfigConstraint("stdp_tau", "float", "STDP time constant in ms", "20.0", "1.0", "100.0"));
    
    // Structural plasticity parameters
    schema.addConstraint(ConfigConstraint("synaptogenesis_rate", "float", "Rate of new synapse formation", "0.0001", "0.0", "0.1"));
    schema.addConstraint(ConfigConstraint("pruning_rate", "float", "Rate of synapse pruning", "0.00001", "0.0", "0.1"));
    
    // Neural population parameters
    schema.addConstraint(ConfigConstraint("max_neurons_per_region", "size_t", "Maximum neurons per brain region", "1000", "1", "10000"));
    schema.addConstraint(ConfigConstraint("max_synapses_per_neuron", "size_t", "Maximum synapses per neuron", "1000", "0", "10000"));
    
    // Learning parameters
    schema.addConstraint(ConfigConstraint("learning_rate", "float", "General learning rate", "0.01", "0.0", "1.0"));
    schema.addConstraint(ConfigConstraint("forgetting_rate", "float", "Memory forgetting rate", "0.001", "0.0", "1.0"));
    
    // Advanced neuromodulation parameters
    schema.addConstraint(ConfigConstraint("dopamine_strength", "float", "Dopamine neuromodulation strength", "1.0", "0.0", "5.0"));
    schema.addConstraint(ConfigConstraint("curiosity_gain", "float", "Curiosity-driven exploration gain", "0.5", "0.0", "2.0"));
    
    // Set default values for Phase 2
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001f, ConfigSource::Default);
    set("neuron_count", static_cast<size_t>(500), ConfigSource::Default);
    set("region_count", static_cast<size_t>(1), ConfigSource::Default);
    set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
}

ValidationResult Config::validateType(const std::string& key, const ConfigValue& value, const std::string& expectedType) const {
    // Check value type against expected type
    ValidationResult typeResult;
    
    if (expectedType == "int") {
        typeResult = std::holds_alternative<int>(value) ? ValidationResult::success() 
                                                      : ValidationResult::error("Expected int type, got " + getTypeName(value));
    } else if (expectedType == "int64_t") {
        typeResult = std::holds_alternative<int64_t>(value) ? ValidationResult::success()
                                                           : ValidationResult::error("Expected int64_t type, got " + getTypeName(value));
    } else if (expectedType == "float") {
        typeResult = std::holds_alternative<float>(value) ? ValidationResult::success()
                                                        : ValidationResult::error("Expected float type, got " + getTypeName(value));
    } else if (expectedType == "double") {
        typeResult = std::holds_alternative<double>(value) ? ValidationResult::success()
                                                         : ValidationResult::error("Expected double type, got " + getTypeName(value));
    } else if (expectedType == "bool") {
        typeResult = std::holds_alternative<bool>(value) ? ValidationResult::success()
                                                       : ValidationResult::error("Expected bool type, got " + getTypeName(value));
    } else if (expectedType == "string") {
        typeResult = std::holds_alternative<std::string>(value) ? ValidationResult::success()
                                                              : ValidationResult::error("Expected string type, got " + getTypeName(value));
    } else if (expectedType == "size_t") {
        typeResult = std::holds_alternative<size_t>(value) ? ValidationResult::success()
                                                         : ValidationResult::error("Expected size_t type, got " + getTypeName(value));
    } else if (expectedType == "vector<float>") {
        typeResult = std::holds_alternative<std::vector<float>>(value) ? ValidationResult::success()
                                                                    : ValidationResult::error("Expected vector<float> type, got " + getTypeName(value));
    }
    
    return typeResult;
}

ValidationResult Config::validateRange(const std::string& key, const ConfigValue& value, const std::string& minStr, const std::string& maxStr) const {
    // Validate value is within specified range
    if (!minStr.empty() && std::holds_alternative<float>(value)) {
        float val = std::get<float>(value);
        float minVal = std::stof(minStr);
        if (val < minVal) {
            return ValidationResult::error("Value " + std::to_string(val) + " is less than minimum " + minStr);
        }
    } else if (!minStr.empty() && std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        double minVal = std::stod(minStr);
        if (val < minVal) {
            return ValidationResult::error("Value " + std::to_string(val) + " is less than minimum " + minStr);
        }
    }
    
    if (!maxStr.empty() && std::holds_alternative<float>(value)) {
        float val = std::get<float>(value);
        float maxVal = std::stof(maxStr);
        if (val > maxVal) {
            return ValidationResult::error("Value " + std::to_string(val) + " is greater than maximum " + maxStr);
        }
    } else if (!maxStr.empty() && std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        double maxVal = std::stod(maxStr);
        if (val > maxVal) {
            return ValidationResult::error("Value " + std::to_string(val) + " is greater than maximum " + maxStr);
        }
    }
    
    return ValidationResult::success();
}

ValidationResult Config::validateEnum(const std::string& key, const ConfigValue& value, const std::vector<std::string>& allowedValues) const {
    // Validate value against allowed enum values
    if (std::holds_alternative<std::string>(value)) {
        std::string val = std::get<std::string>(value);
        for (const auto& allowed : allowedValues) {
            if (val == allowed) {
                return ValidationResult::success();
            }
        }
        return ValidationResult::error("Value '" + val + "' is not in allowed values list");
    }
    
    // For numeric values, check if they correspond to allowed values
    for (const auto& allowed : allowedValues) {
        try {
            if (std::holds_alternative<float>(value) && std::abs(std::get<float>(value) - std::stof(allowed)) < 1e-6f) {
                return ValidationResult::success();
            } else if (std::holds_alternative<double>(value) && std::abs(std::get<double>(value) - std::stod(allowed)) < 1e-6f) {
                return ValidationResult::success();
            } else if (std::holds_alternative<int>(value) && std::get<int>(value) == std::stoi(allowed)) {
                return ValidationResult::success();
            }
        } catch (...) {}
    }
    
    return ValidationResult::error("Value is not in allowed values list");
}

std::string Config::getTypeName(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<float>(value)) return "float";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<float>>(value)) return "vector<float>";
    if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
    return "unknown";
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

bool Config::isFloatString(const std::string& str) {
    try {
        size_t pos;
        float val = std::stof(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

float Config::stringToFloat(const std::string& str) {
    try {
        return std::stof(str);
    } catch (...) {
        return 0.0f;
    }
}

size_t Config::stringToSizeT(const std::string& str) {
    try {
        return std::stoull(str);
    } catch (...) {
        return 0;
    }
}

int64_t Config::stringToInt64(const std::string& str) {
    try {
        return std::stoll(str);
    } catch (...) {
        return 0;
    }
}

// Explicit template instantiations
// get

template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// getOr

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// getFloat

template float Config::getFloat<float>(const std::string&, float) const;

// getSizeT

template size_t Config::getSizeT<size_t>(const std::string&, size_t) const;

// getInt64

template int64_t Config::getInt64<int64_t>(const std::string&, int64_t) const;

} // namespace nlm