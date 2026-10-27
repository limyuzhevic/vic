#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>
#include <random>
#include <chrono>
#include <iomanip>
#include <cmath>

namespace nlm {

// Helper function to parse YAML value
static bool parseYAMLValue(const YAML::Node& node, ConfigValue& result) {
    if (node.IsScalar()) {
        std::string value = node.Scalar();
        // Try to determine type
        if (value == "true" || value == "false") {
            result = (value == "true");
            return true;
        }
        try {
            if (value.find('.') != std::string::npos) {
                result = std::stod(value);
            } else {
                result = std::stoll(value);
            }
            return true;
        } catch (...) {
            result = value;
            return true;
        }
    } else if (node.IsSequence()) {
        // Try to infer element type from first element
        if (node.size() > 0) {
            ConfigValue sample;
            parseYAMLValue(node[0], sample);
            
            if (std::holds_alternative<int>(sample)) {
                std::vector<int> vec;
                for (size_t i = 0; i < node.size(); ++i) {
                    try {
                        vec.push_back(std::stoll(node[i].Scalar()));
                    } catch (...) {
                        return false;
                    }
                }
                if (!vec.empty()) {
                    result = vec;
                    return true;
                }
            } else if (std::holds_alternative<double>(sample)) {
                std::vector<double> vec;
                for (size_t i = 0; i < node.size(); ++i) {
                    try {
                        vec.push_back(std::stod(node[i].Scalar()));
                    } catch (...) {
                        return false;
                    }
                }
                if (!vec.empty()) {
                    result = vec;
                    return true;
                }
            } else if (std::holds_alternative<std::string>(sample)) {
                std::vector<std::string> vec;
                for (size_t i = 0; i < node.size(); ++i) {
                    vec.push_back(node[i].Scalar());
                }
                if (!vec.empty()) {
                    result = vec;
                    return true;
                }
            }
        }
    }
    return false;
}

// Helper function to serialize to YAML
static std::string serializeToYAML(const ConfigValue& value) {
    std::ostringstream oss;
    
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::setprecision(6) << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        }
    }, value);
    
    return oss.str();
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::function<ValidationResult(const ConfigValue&)>> constraints;
    std::unordered_map<std::string, std::string> schemas;
    std::unordered_map<std::string, std::string> evolutionMappings;
    std::unordered_map<std::string, std::function<ConfigValue(const ConfigValue&)>> converters;
    std::unordered_map<std::string, std::string> descriptions;
    
    // Default values for known keys
    static std::unordered_map<std::string, ConfigValue> defaultValues;
    
    Impl() {
        // Initialize default values
        defaultValues["random_seed"] = int64_t(42);
        defaultValues["simulation_timestep"] = 0.001;
        defaultValues["neuron_count"] = int64_t(500);
        defaultValues["region_count"] = int64_t(1);
        defaultValues["connection_probability"] = 0.15f;
        defaultValues["stdp_ltp_weight"] = 0.02f;
        defaultValues["stdp_ltd_weight"] = 0.015f;
        defaultValues["stdp_tau"] = 20.0f;
        defaultValues["synaptogenesis_rate"] = 0.0001f;
        defaultValues["pruning_rate"] = 0.00001f;
        defaultValues["replay_interval"] = size_t(100);
        defaultValues["consolidation_interval"] = size_t(1000);
        defaultValues["checkpoint_dir"] = std::string("./checkpoints");
        
        // Initialize descriptions
        descriptions["random_seed"] = "Random seed for reproducible experiments";
        descriptions["simulation_timestep"] = "Simulation time step in seconds";
        descriptions["neuron_count"] = "Total number of neurons in the brain";
        descriptions["region_count"] = "Number of brain regions";
        descriptions["connection_probability"] = "Probability of connections between neurons";
        descriptions["stdp_ltp_weight"] = "STDP long-term potentiation weight";
        descriptions["stdp_ltd_weight"] = "STDP long-term depression weight";
        descriptions["stdp_tau"] = "STDP time constant in milliseconds";
        descriptions["synaptogenesis_rate"] = "Rate of new synapse formation";
        descriptions["pruning_rate"] = "Rate of synapse elimination";
        descriptions["replay_interval"] = "Interval for memory replay (simulation steps)";
        descriptions["consolidation_interval"] = "Interval for memory consolidation (simulation steps)";
        descriptions["checkpoint_dir"] = "Directory for checkpoint files";
        
        // Initialize constraints
        constraints["random_seed"] = validatePositive;
        constraints["simulation_timestep"] = validatePositive;
        constraints["neuron_count"] = validateNeuronCount;
        constraints["region_count"] = validateNonNegative;
        constraints["connection_probability"] = validateConnectionProbability;
        constraints["stdp_ltp_weight"] = validateRange(0.0f, 1.0f);
        constraints["stdp_ltd_weight"] = validateRange(0.0f, 1.0f);
        constraints["stdp_tau"] = validatePositive;
        constraints["synaptogenesis_rate"] = validateRange(0.0f, 1.0f);
        constraints["pruning_rate"] = validateRange(0.0f, 1.0f);
        constraints["replay_interval"] = validateNonNegative;
        constraints["consolidation_interval"] = validateNonNegative;
    }
};

// Static initialization
std::unordered_map<std::string, ConfigValue> Config::Impl::defaultValues;

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first, then YAML, then simple format
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return loadFromJSON(filepath);
    } else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml") {
        return loadFromYAML(filepath);
    } else {
        // Try to detect based on content
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string firstLine;
        std::getline(file, firstLine);
        file.close();
        
        if (firstLine.find('{') != std::string::npos || firstLine.find('[') != std::string::npos) {
            // Try JSON
            return loadFromJSON(filepath);
        } else {
            // Use simple format
            return loadFromFile(filepath);
        }
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node config = YAML::LoadFile(filepath);
        
        for (const auto& pair : config) {
            std::string key = pair.first.as<std::string>();
            ConfigValue value;
            
            if (parseYAMLValue(pair.second, value)) {
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonConfig = nlohmann::json::parse(file);
        
        for (const auto& [key, value] : jsonConfig.items()) {
            ConfigValue configValue;
            
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_unsigned()) {
                configValue = value.get<uint64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Infer array type
                if (!value.empty() && value[0].is_number_integer()) {
                    std::vector<int> intArray;
                    for (const auto& item : value) {
                        intArray.push_back(item.get<int>());
                    }
                    configValue = intArray;
                } else if (!value.empty() && value[0].is_number_float()) {
                    std::vector<double> doubleArray;
                    for (const auto& item : value) {
                        doubleArray.push_back(item.get<double>());
                    }
                    configValue = doubleArray;
                } else if (!value.empty() && value[0].is_string()) {
                    std::vector<std::string> stringArray;
                    for (const auto& item : value) {
                        stringArray.push_back(item.get<std::string>());
                    }
                    configValue = stringArray;
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            emitter << YAML::Value << serializeToYAML(entry.value);
        }
        
        emitter << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }

        file << emitter.c_str();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
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
    // Apply evolution mappings
    std::string actualKey = key;
    if (pImpl->evolutionMappings.find(key) != pImpl->evolutionMappings.end()) {
        actualKey = pImpl->evolutionMappings[key];
        if (pImpl->converters.find(key) != pImpl->converters.end()) {
            // Apply converter
            ConfigValue converted = pImpl->converters[key](value);
            set(actualKey, converted, source);
            return;
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&actualKey](const ConfigEntry& e) { return e.key == actualKey; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->timestamp = std::chrono::system_clock::now();
    } else {
        std::string description = "";
        if (pImpl->descriptions.find(actualKey) != pImpl->descriptions.end()) {
            description = pImpl->descriptions[actualKey];
        } else if (pImpl->defaultValues.find(actualKey) != pImpl->defaultValues.end()) {
            description = "Default value for " + actualKey;
        }
        
        pImpl->entries.emplace_back(actualKey, value, source, description);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
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

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

bool Config::isSet(const std::string& key, ConfigSource minSource) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return it->source >= minSource;
}

std::string Config::getDescription(const std::string& key) const {
    auto it = pImpl->descriptions.find(key);
    if (it != pImpl->descriptions.end()) {
        return it->second;
    }
    return "";
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    
    return ConfigSource::Default;
}

std::chrono::system_clock::time_point Config::getTimestamp(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->timestamp;
    }
    
    return std::chrono::system_clock::time_point();
}

ValidationResult Config::validate() const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult keyResult = validateKey(entry.key);
        if (!keyResult.isValid) {
            result.errors.insert(result.errors.end(), keyResult.errors.begin(), keyResult.errors.end());
            result.isValid = false;
        }
        result.warnings.insert(result.warnings.end(), keyResult.warnings.begin(), keyResult.warnings.end());
    }
    
    return result;
}

ValidationResult Config::validateKey(const std::string& key) const {
    ValidationResult result;
    
    // Find the entry
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        // Key not set, check if it's a default
        if (pImpl->defaultValues.find(key) != pImpl->defaultValues.end()) {
            return result;  // Default is valid
        }
        return result.addError("Key not set: " + key);
    }
    
    // Apply type validation
    ValidationResult typeResult = validateType(it->key, it->value);
    if (!typeResult.isValid) {
        result.errors.insert(result.errors.end(), typeResult.errors.begin(), typeResult.errors.end());
        result.isValid = false;
    }
    result.warnings.insert(result.warnings.end(), typeResult.warnings.begin(), typeResult.warnings.end());
    
    // Apply constraints
    auto constraintIt = pImpl->constraints.find(key);
    if (constraintIt != pImpl->constraints.end()) {
        ValidationResult constraintResult = constraintIt->second(it->value);
        if (!constraintResult.isValid) {
            result.errors.insert(result.errors.end(), constraintResult.errors.begin(), constraintResult.errors.end());
            result.isValid = false;
        }
        result.warnings.insert(result.warnings.end(), constraintResult.warnings.begin(), constraintResult.warnings.end());
    }
    
    return result;
}

void Config::addConstraint(const std::string& key,
                           const std::function<ValidationResult(const ConfigValue&)>& validator,
                           const std::string& description) {
    pImpl->constraints[key] = validator;
    
    auto it = pImpl->descriptions.find(key);
    if (it == pImpl->descriptions.end()) {
        it = pImpl->descriptions.insert({key, description}).first;
    } else {
        it->second += ". Constraint: " + description;
    }
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

std::shared_ptr<Config> Config::createDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Set all default values
    for (const auto& pair : Impl::defaultValues) {
        config->set(pair.first, pair.second, ConfigSource::Default);
    }
    
    return config;
}

void Config::loadFromEnvironment(const std::string& prefix) {
    for (const auto& entry : pImpl->defaultValues) {
        std::string key = entry.first;
        std::string envKey = prefix + key;
        
        char* envValue = std::getenv(envKey.c_str());
        if (envValue != nullptr) {
            std::string valueStr(envValue);
            set(key, valueStr, ConfigSource::Runtime);
        }
    }
}

void Config::setSchema(const std::string& key, const std::string& schema) {
    pImpl->schemas[key] = schema;
}

std::string Config::getSchema(const std::string& key) const {
    auto it = pImpl->schemas.find(key);
    if (it != pImpl->schemas.end()) {
        return it->second;
    }
    return "";
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, entry.source);
    }
}

void Config::applyEvolution(const std::string& oldKey, const std::string& newKey,
                            const std::function<ConfigValue(const ConfigValue&)>& converter) {
    pImpl->evolutionMappings[oldKey] = newKey;
    pImpl->converters[oldKey] = converter;
    
    // Convert existing values
    for (auto& entry : pImpl->entries) {
        if (entry.key == oldKey) {
            entry.key = newKey;
            entry.value = converter(entry.value);
        }
    }
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

ValidationResult Config::validatePositive(const ConfigValue& value) {
    ValidationResult result;
    
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            if (arg <= 0) {
                result.addError("Value must be positive");
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (arg <= 0.0) {
                result.addError("Value must be positive");
            }
        } else if constexpr (std::is_same_v<T, float>) {
            if (arg <= 0.0f) {
                result.addError("Value must be positive");
            }
        }
    }, value);
    
    return result;
}

ValidationResult Config::validateNonNegative(const ConfigValue& value) {
    ValidationResult result;
    
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            if (arg < 0) {
                result.addError("Value must be non-negative");
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (arg < 0.0) {
                result.addError("Value must be non-negative");
            }
        } else if constexpr (std::is_same_v<T, float>) {
            if (arg < 0.0f) {
                result.addError("Value must be non-negative");
            }
        }
    }, value);
    
    return result;
}

ValidationResult Config::validateRange(const ConfigValue& value, double min, double max) {
    ValidationResult result;
    
    std::visit([&result, min, max](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            if (arg < min || arg > max) {
                result.addError("Value must be in range [" + std::to_string(min) + ", " + std::to_string(max) + "]");
            }
        }
    }, value);
    
    return result;
}

ValidationResult Config::validateNeuronCount(const ConfigValue& value) {
    ValidationResult result;
    
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            if (arg < 1) {
                result.addError("Neuron count must be at least 1");
            } else if (arg > 1000000) {
                result.addWarning("Very large neuron count may impact performance");
            }
        }
    }, value);
    
    return result;
}

ValidationResult Config::validateConnectionProbability(const ConfigValue& value) {
    ValidationResult result;
    
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            if (arg < 0.0f || arg > 1.0f) {
                result.addError("Connection probability must be in range [0.0, 1.0]");
            }
        }
    }, value);
    
    return result;
}

ConfigStats Config::getStats() const {
    ConfigStats stats;
    
    for (const auto& entry : pImpl->entries) {
        stats.totalEntries++;
        
        switch (entry.source) {
            case ConfigSource::Default:
                stats.fromDefault++;
                break;
            case ConfigSource::File:
                stats.fromFile++;
                break;
            case ConfigSource::CommandLine:
                stats.fromCommandLine++;
                break;
            case ConfigSource::Runtime:
                stats.fromRuntime++;
                break;
        }
    }
    
    // Estimate memory usage (approximate)
    stats.memoryUsage = sizeof(Config::Impl) + sizeof(ConfigEntry) * stats.totalEntries;
    
    return stats;
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

// Helper function to generate random string
static std::string generateRandomString(size_t length) {
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, chars.size() - 1);
    
    std::string result;
    for (size_t i = 0; i < length; ++i) {
        result += chars[dis(gen)];
    }
    return result;
}

// Helper function to validate type
static ValidationResult validateType(const std::string& key, const ConfigValue& value) {
    ValidationResult result;
    
    // Basic type validation
    if (std::holds_alternative<int>(value)) {
        // Valid int type
    } else if (std::holds_alternative<int64_t>(value)) {
        // Valid int64_t type
    } else if (std::holds_alternative<double>(value)) {
        // Valid double type
    } else if (std::holds_alternative<bool>(value)) {
        // Valid bool type
    } else if (std::holds_alternative<std::string>(value)) {
        // Valid string type
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        // Valid int array type
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        // Valid double array type
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        // Valid string array type
    }
    
    return result;
}

// Generate random configuration for testing
static void generateRandomConfig(std::shared_ptr<Config> config, uint64_t seed) {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    std::uniform_int_distribution<int> intDist(1, 1000);
    
    config->set("random_seed", static_cast<int64_t>(seed));
    config->set("simulation_timestep", 0.001 * (0.5 + dist(gen)));
    config->set("neuron_count", static_cast<int64_t>(100 + intDist(gen) * 10));
    config->set("region_count", static_cast<int64_t>(1 + dist(gen) * 5));
    config->set("connection_probability", 0.1 + dist(gen) * 0.8);
    config->set("stdp_ltp_weight", 0.01 * dist(gen));
    config->set("stdp_ltd_weight", 0.01 * dist(gen));
    config->set("stdp_tau", 10.0 * dist(gen));
    config->set("synaptogenesis_rate", 0.00001 * dist(gen));
    config->set("pruning_rate", 0.000001 * dist(gen));
    config->set("replay_interval", static_cast<size_t>(50 + dist(gen) * 150));
    config->set("consolidation_interval", static_cast<size_t>(500 + dist(gen) * 1500));
}

// Initialize default values
std::unordered_map<std::string, ConfigValue> Config::Impl::defaultValues;

// Configure YAML and JSON libraries
static struct ConfigInitializer {
    ConfigInitializer() {
        YAML::SetErrorHandler(YAML::ErrorHandler::Safe);
    }
} configInitializer;

// Generate random configuration for testing
void Config::generateRandomConfig(std::shared_ptr<Config> config, uint64_t seed) {
    ::generateRandomConfig(config, seed);
}