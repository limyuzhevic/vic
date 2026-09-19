#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::vector<std::string>> validationErrors;
    std::unordered_map<std::string, std::string> defaultValues;
    
    // Valid parameter definitions
    static const std::unordered_map<std::string, std::string> VALID_PARAMETERS;
    
    // Type validation functions
    static bool validateBool(const ConfigValue& value);
    static bool validateInt(const ConfigValue& value);
    static bool validateInt64(const ConfigValue& value);
    static bool validateFloat(const ConfigValue& value);
    static bool validateString(const ConfigValue& value);
    static bool validateStringNonEmpty(const ConfigValue& value);
    static std::string typeToString(const ConfigValue& value);
    
    // Parameter definitions for validation
    static bool validateNeuronCount(const ConfigValue& value);
    static bool validateRegionCount(const ConfigValue& value);
    static bool validateConnectionProbability(const ConfigValue& value);
    static bool validateTimestep(const ConfigValue& value);
    static bool validateSeed(const ConfigValue& value);
    static bool validatePlasticityRate(const ConfigValue& value);
    static bool validatePlasticityWeight(const ConfigValue& value);
    static bool validateDelay(const ConfigValue& value);
    static bool validateTemperature(const ConfigValue& value);
    static bool validateSensoryGain(const ConfigValue& value);
    static bool validateMotorStrength(const ConfigValue& value);
    static bool validateMemoryCapacity(const ConfigValue& value);
    static bool validateLearningRate(const ConfigValue& value);
    static bool validateExplorationRate(const ConfigValue& value);
    static bool validateCuriosityLevel(const ConfigValue& value);
    static bool validateNoveltyThreshold(const ConfigValue& value);
    static bool validatePredictionError(const ConfigValue& value);
    static bool validateRewardScale(const ConfigValue& value);
    static bool validateTemperature(const ConfigValue& value);
    static bool validatePressure(const ConfigValue& value);
    static bool validateLight(const ConfigValue& value);
    static bool validateSound(const ConfigValue& value);
    static bool validateVision(const ConfigValue& value);
    static bool validateTouch(const ConfigValue& value);
    static bool validateProprioception(const ConfigValue& value);
    static bool validateInternal(const ConfigValue& value);
    static bool validateMovement(const ConfigValue& value);
    static bool validateInteraction(const ConfigValue& value);
};

// Initialize static members
const std::unordered_map<std::string, std::string> Config::Impl::VALID_PARAMETERS = {
    {"neuron_count", "int"},
    {"region_count", "int"},
    {"connection_probability", "float"},
    {"simulation_timestep", "float"},
    {"random_seed", "int64_t"},
    {"stdp_ltp_weight", "float"},
    {"stdp_ltd_weight", "float"},
    {"stdp_tau", "float"},
    {"synaptogenesis_rate", "float"},
    {"pruning_rate", "float"},
    {"replay_interval", "size_t"},
    {"consolidation_interval", "size_t"},
    {"working_memory_capacity", "size_t"},
    {"episodic_memory_capacity", "size_t"},
    {"learning_rate", "float"},
    {"exploration_rate", "float"},
    {"curiosity_level", "float"},
    {"novelty_threshold", "float"},
    {"reward_scale", "float"},
    {"temperature", "float"},
    {"pressure", "float"},
    {"light", "float"},
    {"sound", "float"},
    {"vision", "float"},
    {"touch", "float"},
    {"proprioception", "float"},
    {"internal", "float"},
    {"movement", "float"},
    {"interaction", "float"}
};

// Validation function implementations
bool Config::Impl::validateBool(const ConfigValue& value) {
    return std::holds_alternative<bool>(value);
}

bool Config::Impl::validateInt(const ConfigValue& value) {
    return std::holds_alternative<int>(value);
}

bool Config::Impl::validateInt64(const ConfigValue& value) {
    return std::holds_alternative<int64_t>(value);
}

bool Config::Impl::validateFloat(const ConfigValue& value) {
    return std::holds_alternative<double>(value) || std::holds_alternative<float>(value);
}

bool Config::Impl::validateString(const ConfigValue& value) {
    return std::holds_alternative<std::string>(value);
}

bool Config::Impl::validateStringNonEmpty(const ConfigValue& value) {
    if (!validateString(value)) return false;
    return std::get<std::string>(value).length() > 0;
}

std::string Config::Impl::typeToString(const ConfigValue& value) {
    if (validateBool(value)) return "bool";
    if (validateInt(value)) return "int";
    if (validateInt64(value)) return "int64_t";
    if (validateFloat(value)) return "float/double";
    if (validateString(value)) return "string";
    return "unknown";
}

// Parameter-specific validators
bool Config::Impl::validateNeuronCount(const ConfigValue& value) {
    if (!validateInt(value)) return false;
    int count = std::get<int>(value);
    return count > 0 && count <= 100000;
}

bool Config::Impl::validateRegionCount(const ConfigValue& value) {
    if (!validateInt(value)) return false;
    int count = std::get<int>(value);
    return count > 0 && count <= 100;
}

bool Config::Impl::validateConnectionProbability(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float prob = std::get<float>(value);
    return prob >= 0.0f && prob <= 1.0f;
}

bool Config::Impl::validateTimestep(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float timestep = std::get<float>(value);
    return timestep > 0.0f && timestep <= 0.1f;
}

bool Config::Impl::validateSeed(const ConfigValue& value) {
    if (!validateInt64(value)) return false;
    int64_t seed = std::get<int64_t>(value);
    return seed >= 0;
}

bool Config::Impl::validatePlasticityRate(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float rate = std::get<float>(value);
    return rate >= 0.0f && rate <= 0.1f;
}

bool Config::Impl::validatePlasticityWeight(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float weight = std::get<float>(value);
    return weight >= -10.0f && weight <= 10.0f;
}

bool Config::Impl::validateDelay(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float delay = std::get<float>(value);
    return delay >= 0.0f && delay <= 100.0f;
}

bool Config::Impl::validateTemperature(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float temp = std::get<float>(value);
    return temp >= -50.0f && temp <= 150.0f;
}

bool Config::Impl::validateSensoryGain(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float gain = std::get<float>(value);
    return gain >= 0.0f && gain <= 10.0f;
}

bool Config::Impl::validateMotorStrength(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float strength = std::get<float>(value);
    return strength >= 0.0f && strength <= 10.0f;
}

bool Config::Impl::validateMemoryCapacity(const ConfigValue& value) {
    if (!validateInt(value)) return false;
    int capacity = std::get<int>(value);
    return capacity > 0 && capacity <= 10000;
}

bool Config::Impl::validateLearningRate(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float rate = std::get<float>(value);
    return rate >= 0.0f && rate <= 1.0f;
}

bool Config::Impl::validateExplorationRate(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float rate = std::get<float>(value);
    return rate >= 0.0f && rate <= 1.0f;
}

bool Config::Impl::validateCuriosityLevel(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float level = std::get<float>(value);
    return level >= 0.0f && level <= 10.0f;
}

bool Config::Impl::validateNoveltyThreshold(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float threshold = std::get<float>(value);
    return threshold >= 0.0f && threshold <= 1.0f;
}

bool Config::Impl::validatePredictionError(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float error = std::get<float>(value);
    return error >= 0.0f && error <= 10.0f;
}

bool Config::Impl::validateRewardScale(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float scale = std::get<float>(value);
    return scale >= 0.0f && scale <= 100.0f;
}

// Additional validators for sensory and environmental parameters
bool Config::Impl::validatePressure(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float pressure = std::get<float>(value);
    return pressure >= 0.0f && pressure <= 1000.0f;
}

bool Config::Impl::validateLight(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float light = std::get<float>(value);
    return light >= 0.0f && light <= 100.0f;
}

bool Config::Impl::validateSound(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float sound = std::get<float>(value);
    return sound >= 0.0f && sound <= 100.0f;
}

bool Config::Impl::validateVision(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float vision = std::get<float>(value);
    return vision >= 0.0f && vision <= 1.0f;
}

bool Config::Impl::validateTouch(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float touch = std::get<float>(value);
    return touch >= 0.0f && touch <= 1.0f;
}

bool Config::Impl::validateProprioception(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float proprio = std::get<float>(value);
    return proprio >= 0.0f && proprio <= 1.0f;
}

bool Config::Impl::validateInternal(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float internal = std::get<float>(value);
    return internal >= 0.0f && internal <= 1.0f;
}

bool Config::Impl::validateMovement(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float movement = std::get<float>(value);
    return movement >= 0.0f && movement <= 10.0f;
}

bool Config::Impl::validateInteraction(const ConfigValue& value) {
    if (!validateFloat(value)) return false;
    float interaction = std::get<float>(value);
    return interaction >= 0.0f && interaction <= 1.0f;
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Config::Impl member definitions already added to header
// Now we need to update the Config constructor and add validation methods

// Update Config constructor
Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize default values for common parameters
    pImpl->defaultValues["neuron_count"] = "1000";
    pImpl->defaultValues["region_count"] = "1";
    pImpl->defaultValues["connection_probability"] = "0.1";
    pImpl->defaultValues["simulation_timestep"] = "0.001";
    pImpl->defaultValues["random_seed"] = "42";
    pImpl->defaultValues["stdp_ltp_weight"] = "0.01";
    pImpl->defaultValues["stdp_ltd_weight"] = "0.012";
    pImpl->defaultValues["stdp_tau"] = "20.0";
    pImpl->defaultValues["synaptogenesis_rate"] = "0.0001";
    pImpl->defaultValues["pruning_rate"] = "0.00001";
    pImpl->defaultValues["replay_interval"] = "100";
    pImpl->defaultValues["consolidation_interval"] = "1000";
    pImpl->defaultValues["working_memory_capacity"] = "100";
    pImpl->defaultValues["episodic_memory_capacity"] = "1000";
    pImpl->defaultValues["learning_rate"] = "0.1";
    pImpl->defaultValues["exploration_rate"] = "0.1";
    pImpl->defaultValues["curiosity_level"] = "0.5";
    pImpl->defaultValues["novelty_threshold"] = "0.5";
    pImpl->defaultValues["reward_scale"] = "1.0";
}

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
            
            // Validate before setting
            if (validateAndSet(key, value, ConfigSource::File)) {
                set(key, value, ConfigSource::File);
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
                // Validate before setting
                if (validateAndSet(key, value, ConfigSource::CommandLine)) {
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            // Validate before setting
            if (validateAndSet(key, value, ConfigSource::CommandLine)) {
                set(key, value, ConfigSource::CommandLine);
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
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

bool Config::validateAndSet(const std::string& key, const std::string& value, ConfigSource source) {
    // Check if parameter is valid
    if (pImpl->VALID_PARAMETERS.find(key) == pImpl->VALID_PARAMETERS.end()) {
        // Unknown parameter, but we can still set it (for forward compatibility)
        return true;
    }
    
    // Type validation
    std::string expectedType = pImpl->VALID_PARAMETERS.at(key);
    bool typeValid = false;
    
    if (expectedType == "int") {
        typeValid = validateInt(value);
    } else if (expectedType == "int64_t") {
        typeValid = validateInt64(value);
    } else if (expectedType == "float") {
        typeValid = validateFloat(value);
    } else if (expectedType == "bool") {
        typeValid = validateBool(value);
    } else if (expectedType == "string") {
        typeValid = validateString(value);
    }
    
    if (!typeValid) {
        pImpl->validationErrors[key].push_back("Type mismatch: expected " + expectedType + " for key " + key);
        return false;
    }
    
    // Range validation for known parameters
    bool rangeValid = true;
    if (expectedType == "int") {
        int val = std::stoi(value);
        rangeValid = (val > 0 && val <= 100000);
    } else if (expectedType == "float") {
        float val = std::stof(value);
        if (key == "neuron_count") {
            rangeValid = (val > 0 && val <= 100000);
        } else if (key == "region_count") {
            rangeValid = (val > 0 && val <= 100);
        } else if (key == "connection_probability") {
            rangeValid = (val >= 0.0f && val <= 1.0f);
        } else if (key == "simulation_timestep") {
            rangeValid = (val > 0.0f && val <= 0.1f);
        }
    }
    
    if (!rangeValid) {
        pImpl->validationErrors[key].push_back("Value out of range for key " + key);
        return false;
    }
    
    return true;
}

// Helper validation functions for string values
template<typename T>
static T parseValue(const std::string& str) {
    std::istringstream iss(str);
    T value;
    iss >> value;
    return value;
}

bool Config::Impl::validateInt(const std::string& str) {
    try {
        parseValue<int>(str);
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::Impl::validateInt64(const std::string& str) {
    try {
        parseValue<int64_t>(str);
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::Impl::validateFloat(const std::string& str) {
    try {
        parseValue<float>(str);
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::Impl::validateBool(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true" || lower == "false" || lower == "1" || lower == "0";
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
