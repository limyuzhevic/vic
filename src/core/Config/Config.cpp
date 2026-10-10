#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Validation schema
    std::unordered_map<std::string, ConfigValue> defaultValues;
    std::unordered_map<std::string, std::string> descriptions;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators;
    
    Impl() {
        // Initialize with default configuration schema
        initializeDefaultSchema();
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

Config::loadFromFile(const std::string& filepath) {
    try {
        // First, try to parse as JSON
        if (parseJSONFile(filepath, pImpl->entries)) {
            return true;
        }
        
        // If JSON fails, fall back to simple key=value format with error handling
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open configuration file: " + filepath);
            return false;
        }
        
        std::string line;
        size_t lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Validate key
                if (!isValidKey(key)) {
                    NLM_LOG_WARNING("Unknown configuration key in " + filepath + " line " + 
                                  std::to_string(lineNumber) + ": " + key);
                    continue;
                }
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Validate value type before setting
                if (!isValidValueType(key, ConfigValue(value))) {
                    NLM_LOG_WARNING("Invalid value type for key " + key + " in " + filepath + 
                                  " line " + std::to_string(lineNumber));
                    continue;
                }
                
                try {
                    set(key, value, ConfigSource::File);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error parsing configuration in " + filepath + " line " + 
                                std::to_string(lineNumber) + ": " + e.what());
                }
            } else {
                NLM_LOG_WARNING("Malformed configuration line in " + filepath + 
                              " line " + std::to_string(lineNumber) + ": " + line);
            }
        }
        
        // Validate after loading
        ValidationResult validation = validate();
        if (!validation.valid) {
            NLM_LOG_ERROR("Configuration validation failed after loading from " + filepath + ": " + 
                        validation.errorMessage);
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading configuration file " + filepath + ": ") + e.what());
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

template<typename T>
void Config::setValidated(const std::string& key, const T& value, ConfigSource source,
                         const std::string& description) {
    if (!isValidKey(key)) {
        throw std::invalid_argument("Invalid configuration key: " + key);
    }
    
    ConfigValue configValue;
    try {
        configValue = std::get<ConfigValue>(value);
    } catch (...) {
        throw std::invalid_argument("Incompatible type for configuration key: " + key);
    }
    
    if (!isValidValueType(key, configValue)) {
        throw std::invalid_argument("Invalid value type for configuration key: " + key);
    }
    
    // Validate if validator exists
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end() && !validatorIt->second(configValue)) {
        throw std::invalid_argument("Invalid value for configuration key " + key + ": " + 
                                   std::to_string(configValue));
    }
    
    set(key, configValue, source);
    
    if (!description.empty()) {
        auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                   [&key](const ConfigEntry& e) { return e.key == key; });
        if (entryIt != pImpl->entries.end()) {
            entryIt->description = description;
        }
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end() && !validatorIt->second(value)) {
        throw std::invalid_argument("Invalid value for configuration key " + key + ": " + 
                                   std::to_string(value));
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        std::string description = pImpl->descriptions.count(key) ? pImpl->descriptions[key] : "";
        pImpl->entries.emplace_back(key, value, source, description);
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
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << "\n";
    }
    return oss.str();
}

ValidationResult Config::validate() const {
    std::vector<std::string> warnings;
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult result = validateKey(entry.key);
        if (!result.valid) {
            return ValidationResult(false, result.errorMessage, warnings);
        }
        
        // Collect warnings from validation
        for (const auto& warning : result.warnings) {
            warnings.push_back("Key \"" + entry.key + "\": " + warning);
        }
    }
    
    return ValidationResult(true, "", warnings);
}

ValidationResult Config::validateKey(const std::string& key) const {
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt == pImpl->validators.end()) {
        return ValidationResult(true, "", {"No validator for key: " + key});
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        // Key exists in schema but not in config - check if it has a default value
        if (pImpl->defaultValues.find(key) != pImpl->defaultValues.end()) {
            return ValidationResult(true, "", {"Using default value for key: " + key});
        } else {
            return ValidationResult(false, "Required configuration key is missing: " + key);
        }
    }
    
    // Validate the value
    try {
        if (!validatorIt->second(it->value)) {
            return ValidationResult(false, "Invalid value for configuration key: " + key);
        }
    } catch (const std::exception& e) {
        return ValidationResult(false, std::string("Error validating key " + key + ": ") + e.what());
    }
    
    return ValidationResult(true, "");
}

int Config::getInt(const std::string& key, int defaultValue) const {
    auto val = get<int64_t>(key);
    if (val.has_value()) {
        return static_cast<int>(val.value());
    }
    return defaultValue;
}

double Config::getDouble(const std::string& key, double defaultValue) const {
    auto val = get<double>(key);
    if (val.has_value()) {
        return val.value();
    }
    return defaultValue;
}

bool Config::getBool(const std::string& key, bool defaultValue) const {
    auto val = get<bool>(key);
    if (val.has_value()) {
        return val.value();
    }
    return defaultValue;
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    auto val = get<std::string>(key);
    if (val.has_value()) {
        return val.value();
    }
    return defaultValue;
}

int Config::getRandomSeed() const {
    return getOr<int64_t>("random_seed", 42);
}

double Config::getSimulationTimestep() const {
    return getOr<double>("simulation_timestep", 0.001);
}

size_t Config::getNeuronCount() const {
    return static_cast<size_t>(getOr<int64_t>("neuron_count", 1000));
}

size_t Config::getRegionCount() const {
    return static_cast<size_t>(getOr<int64_t>("region_count", 1));
}

float Config::getConnectionProbability() const {
    return getOr<float>("connection_probability", 0.1f);
}

float Config::getSTDP LTPWeight() const {
    return getOr<float>("stdp_ltp_weight", 0.01f);
}

float Config::getSTDP LTDWeight() const {
    return getOr<float>("stdp_ltd_weight", 0.012f);
}

float Config::getSTPDA tau() const {
    return getOr<float>("stdp_tau", 20.0f);
}

float Config::getSynaptogenesisRate() const {
    return getOr<float>("synaptogenesis_rate", 0.0001f);
}

float Config::getPruningRate() const {
    return getOr<float>("pruning_rate", 0.00001f);
}

std::string Config::getCheckpointDir() const {
    return getString("checkpoint_dir", "./checkpoints");
}

bool Config::isValidKey(const std::string& key) {
    // Check against known valid keys
    static const std::vector<std::string> validKeys = {
        "neuron_count", "region_count", "connection_probability", "simulation_timestep",
        "random_seed", "stdp_ltp_weight", "stdp_ltd_weight", "stdp_tau",
        "synaptogenesis_rate", "pruning_rate", "checkpoint_dir",
        "checkpoint_interval", "replay_interval", "consolidation_interval"
    };
    
    return std::find(validKeys.begin(), validKeys.end(), key) != validKeys.end();
}

bool Config::isValidValueType(const std::string& key, const ConfigValue& value) {
    // Determine expected type based on key
    if (key == "neuron_count" || key == "region_count" || key == "random_seed" ||
        key == "checkpoint_interval" || key == "replay_interval" || key == "consolidation_interval") {
        return std::holds_alternative<int64_t>(value);
    } else if (key == "connection_probability" || key == "stdp_ltp_weight" ||
               key == "stdp_ltd_weight" || key == "stdp_tau" ||
               key == "synaptogenesis_rate" || key == "pruning_rate") {
        return std::holds_alternative<float>(value);
    } else if (key == "simulation_timestep") {
        return std::holds_alternative<double>(value);
    } else if (key == "checkpoint_dir") {
        return std::holds_alternative<std::string>(value);
    }
    
    return true; // For unknown keys, assume valid
}

std::optional<ConfigValue> Config::getDefaultValue(const std::string& key) {
    if (pImpl && pImpl->defaultValues.find(key) != pImpl->defaultValues.end()) {
        return pImpl->defaultValues[key];
    }
    return std::nullopt;
}

std::string Config::getErrorMessage(int errorCode) {
    // Add error message lookup table
    static const std::unordered_map<int, std::string> errorMessages = {
        {1, "Configuration key validation failed"},
        {2, "Invalid configuration value type"},
        {3, "Configuration value out of range"},
        {4, "Required configuration key missing"},
        {5, "Configuration file I/O error"},
        {6, "JSON parsing error"},
        {7, "Invalid configuration source"}
    };
    
    auto it = errorMessages.find(errorCode);
    return it != errorMessages.end() ? it->second : "Unknown error";
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

// This part would continue with all the explicit instantiations
// For brevity, I'll show the most important ones

} // namespace nlm
