#include "ConfigValidator.hpp"
#include <filesystem>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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
        
        // Parse simple key=value pairs
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
            
            set(key, value, ConfigSource::File);
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
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

//
// ConfigValidator implementation
//

struct ConfigValidator::Impl {
    std::unordered_map<std::string, std::pair<Validator, std::string>> validators;
};

ConfigValidator::ConfigValidator() : pImpl(std::make_unique<Impl>()) {}

void ConfigValidator::registerValidator(const std::string& key, 
                                        Validator validator,
                                        const std::string& description) {
    pImpl->validators[key] = std::make_pair(validator, description);
}

bool ConfigValidator::validate(const std::string& key, const ConfigValue& value, 
                             std::string& errorMessage) const {
    auto it = pImpl->validators.find(key);
    if (it == pImpl->validators.end()) {
        errorMessage = "Key '" + key + "' has no validator registered";
        return false;
    }
    
    bool isValid = it->second.first(value, errorMessage);
    return isValid;
}

bool ConfigValidator::validate(const Config& config, std::string& errorMessage) const {
    bool allValid = true;
    for (const auto& entry : config.pImpl->entries) {
        std::string entryError;
        if (!validate(entry.key, entry.value, entryError)) {
            allValid = false;
            if (!errorMessage.empty()) errorMessage += "\n";
            errorMessage += "Key '" + entry.key + "': " + entryError;
        }
    }
    return allValid;
}

bool ConfigValidator::validateFile(const std::string& filepath, std::string& errorMessage) const {
    Config config;
    if (!config.loadFromFile(filepath)) {
        errorMessage = "Failed to load configuration file: " + filepath;
        return false;
    }
    return validate(config, errorMessage);
}

std::vector<std::pair<std::string, std::string>> ConfigValidator::getRegisteredValidators() const {
    std::vector<std::pair<std::string, std::string>> result;
    result.reserve(pImpl->validators.size());
    for (const auto& pair : pImpl->validators) {
        result.push_back(pair);
    }
    return result;
}

void ConfigValidator::removeValidator(const std::string& key) {
    pImpl->validators.erase(key);
}

void ConfigValidator::clearValidators() {
    pImpl->validators.clear();
}

void ConfigValidator::setupDefaultValidators() {
    // Clear existing validators
    clearValidators();
    
    // Register default validators for common NLM configuration keys
    registerValidator("neuron_count", makeNeuronCountValidator(), "Valid range for neuron count");
    registerValidator("region_count", makeRegionCountValidator(), "Valid range for region count");
    registerValidator("connection_probability", makeConnectivityValidator(), 
                     "Valid connectivity probability (0.0 to 1.0)");
    registerValidator("simulation_timestep", makeTimestepValidator(), "Valid timestep (positive)");
    registerValidator("plasticity_learning_rate", makePlasticityValidator(), 
                     "Valid learning rate (0.0 to 1.0)");
    registerValidator("stdp_ltp_weight", makePlasticityValidator(), 
                     "Valid STDP LTP weight (0.0 to 1.0)");
    registerValidator("stdp_ltd_weight", makePlasticityValidator(), 
                     "Valid STDP LTD weight (0.0 to 1.0)");
    registerValidator("dopamine_baseline", makeNeuromodulationValidator(), 
                     "Valid dopamine baseline (0.0 to 1.0)");
    registerValidator("random_seed", makeRandomSeedValidator(), "Valid random seed (0 to 2^64-1)");
    registerValidator("max_simulation_steps", makeSimulationStepsValidator(), 
                     "Valid maximum simulation steps (reasonable range)");
    registerValidator("log_level", makeLogLevelValidator(), "Valid logging level");
    registerValidator("checkpoint_dir", makeCheckpointDirValidator(), "Valid checkpoint directory");
}

ConfigValidator::Validator ConfigValidator::makeRangeValidator(double min, double max, bool inclusive) {
    return [min, max, inclusive](const ConfigValue& value, std::string& errorMessage) {
        if constexpr (std::is_same_v<ConfigValue, double> || 
                      std::is_same_v<ConfigValue, int> || 
                      std::is_same_v<ConfigValue, int64_t> || 
                      std::is_same_v<ConfigValue, float>) {
            double val = std::visit([](auto&& v) { return static_cast<double>(v); }, value);
            
            if (inclusive) {
                if (val < min || val > max) {
                    errorMessage = "Value " + std::to_string(val) + " is outside range [" + 
                                 std::to_string(min) + ", " + std::to_string(max) + "]";
                    return false;
                }
            } else {
                if (val <= min || val >= max) {
                    errorMessage = "Value " + std::to_string(val) + " is outside range (" + 
                                 std::to_string(min) + ", " + std::to_string(max) + ")";
                    return false;
                }
            }
        } else {
            errorMessage = "Value is not numeric";
            return false;
        }
        return true;
    };
}

ConfigValidator::Validator ConfigValidator::makePositiveValidator() {
    return [](const ConfigValue& value, std::string& errorMessage) {
        if constexpr (std::is_same_v<ConfigValue, double> || 
                      std::is_same_v<ConfigValue, int> || 
                      std::is_same_v<ConfigValue, int64_t> || 
                      std::is_same_v<ConfigValue, float>) {
            double val = std::visit([](auto&& v) { return static_cast<double>(v); }, value);
            
            if (val < 0.0) {
                errorMessage = "Value " + std::to_string(val) + " must be non-negative";
                return false;
            }
        } else {
            errorMessage = "Value is not numeric";
            return false;
        }
        return true;
    };
}

ConfigValidator::Validator ConfigValidator::makeStringLengthValidator(size_t minLength, size_t maxLength) {
    return [minLength, maxLength](const ConfigValue& value, std::string& errorMessage) {
        if constexpr (std::is_same_v<ConfigValue, std::string>) {
            size_t length = value.get<std::string>().size();
            
            if (length < minLength) {
                errorMessage = "String length " + std::to_string(length) + " is less than minimum " + 
                             std::to_string(minLength);
                return false;
            }
            if (length > maxLength) {
                errorMessage = "String length " + std::to_string(length) + " exceeds maximum " + 
                             std::to_string(maxLength);
                return false;
            }
        } else {
            errorMessage = "Value is not a string";
            return false;
        }
        return true;
    };
}

ConfigValidator::Validator ConfigValidator::makeEnumValidator(const std::vector<std::string>& validValues) {
    std::unordered_set<std::string> validSet;
    for (const auto& val : validValues) {
        validSet.insert(val);
    }
    
    return [validSet](const ConfigValue& value, std::string& errorMessage) {
        if constexpr (std::is_same_v<ConfigValue, std::string>) {
            std::string strVal = value.get<std::string>();
            if (validSet.find(strVal) == validSet.end()) {
                errorMessage = "Value '" + strVal + "' is not a valid enum value";
                return false;
            }
        } else {
            errorMessage = "Value is not a string";
            return false;
        }
        return true;
    };
}

ConfigValidator::Validator ConfigValidator::makeConnectivityValidator() {
    return makeRangeValidator(0.0, 1.0, true);
}

ConfigValidator::Validator ConfigValidator::makeTimestepValidator() {
    return makeRangeValidator(0.000001, 1.0, true); // Very small timestep to large timestep
}

ConfigValidator::Validator ConfigValidator::makeNeuronCountValidator() {
    return makeRangeValidator(1, 1000000, true); // 1 to 1 million neurons
}

ConfigValidator::Validator ConfigValidator::makeRegionCountValidator() {
    return makeRangeValidator(1, 100, true); // 1 to 100 regions
}

ConfigValidator::Validator ConfigValidator::makePlasticityValidator() {
    return makeRangeValidator(0.0, 1.0, true);
}

ConfigValidator::Validator ConfigValidator::makeNeuromodulationValidator() {
    return makeRangeValidator(0.0, 1.0, true);
}

ConfigValidator::Validator ConfigValidator::makeRandomSeedValidator() {
    return makeRangeValidator(0, std::numeric_limits<uint64_t>::max(), true);
}

ConfigValidator::Validator ConfigValidator::makeSimulationStepsValidator() {
    return makeRangeValidator(1, 1000000000, true); // 1 to 1 billion steps
}

ConfigValidator::Validator ConfigValidator::makeCheckpointDirValidator() {
    return [](const ConfigValue& value, std::string& errorMessage) {
        if constexpr (std::is_same_v<ConfigValue, std::string>) {
            std::string dir = value.get<std::string>();
            if (dir.empty()) {
                errorMessage = "Checkpoint directory cannot be empty";
                return false;
            }
            if (dir.find('/') == std::string::npos && dir.find('\\') == std::string::npos) {
                errorMessage = "Checkpoint directory should be an absolute path";
                return false;
            }
        } else {
            errorMessage = "Value is not a string";
            return false;
        }
        return true;
    };
}

ConfigValidator::Validator ConfigValidator::makeLogLevelValidator() {
    const std::vector<std::string> validLevels = {"DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL", "OFF"};
    return makeEnumValidator(validLevels);
}

ConfigValidator::Validator ConfigValidator::makeVisualizationValidator() {
    return [](const ConfigValue& value, std::string& errorMessage) {
        if constexpr (std::is_same_v<ConfigValue, bool>) {
            return true; // Boolean value is always valid
        } else {
            errorMessage = "Value is not a boolean";
            return false;
        }
    };
}

// ConfigValidator destructor
ConfigValidator::~ConfigValidator() = default;
