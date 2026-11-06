#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), parameterSchemas() {
    // Register default parameter schemas
    registerParameterSchema(ParameterSchema(
        NEURON_COUNT, 
        "Total number of neurons in the brain",
        ConfigSource::Default,
        {}, // No specific allowed values
        [](const ConfigValue& v) {
            return validateIntRange(v, 10, 1000000);
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        SYNAPSE_DENSITY,
        "Synaptic connectivity density (0.0 to 1.0)",
        ConfigSource::Default,
        {},
        [](const ConfigValue& v) {
            return validateDoubleRange(v, 0.0, 1.0);
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        CONNECTION_PROBABILITY,
        "Probability of connection between neurons (0.0 to 1.0)",
        ConfigSource::Default,
        {},
        [](const ConfigValue& v) {
            return validateDoubleRange(v, 0.0, 1.0);
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        INITIAL_WEIGHT_MEAN,
        "Mean initial synaptic weight",
        ConfigSource::Default,
        {},
        [](const ConfigValue& v) {
            return validateDoubleRange(v, -1.0, 1.0);
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        INITIAL_WEIGHT_STD,
        "Standard deviation of initial synaptic weights",
        ConfigSource::Default,
        {},
        [](const ConfigValue& v) {
            return validateDoubleRange(v, 0.0, 1.0);
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        V_THRESH,
        "Neuron threshold potential (mV)",
        ConfigSource::Default,
        {},
        [](const ConfigValue& v) {
            return validateDoubleRange(v, -100.0, -10.0);
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        V_REST,
        "Resting potential (mV)",
        ConfigSource::Default,
        {},
        [](const ConfigValue& v) {
            return validateDoubleRange(v, -100.0, -40.0);
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        V_RESET,
        "Reset potential after spike (mV)",
        ConfigSource::Default,
        {},
        [](const ConfigValue& v) {
            return validateDoubleRange(v, -100.0, -40.0);
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        TAU_MEM,
        "Membrane time constant (ms)",
        ConfigSource::Default,
        {},
        [](const ConfigValue& v) {
            return validateDoubleRange(v, 1.0, 100.0);
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        TAU_REF,
        "Refractory period (ms)",
        ConfigSource::Default,
        {},
        [](const ConfigValue& v) {
            return validateDoubleRange(v, 0.1, 10.0);
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        STDP_ENABLE,
        "Enable spike-timing dependent plasticity",
        ConfigSource::Default,
        {"true", "false"},
        [](const ConfigValue& v) {
            if constexpr (std::is_same_v<bool, std::decay_t<decltype(v)>>) {
                return true;
            }
            return false;
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        STDP_LEARNING_RATE,
        "STDP learning rate",
        ConfigSource::Default,
        {},
        [](const ConfigValue& v) {
            return validateDoubleRange(v, 0.0, 1.0);
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        STDP_TAU_PLUS,
        "STDP time constant for potentiation (ms)",
        ConfigSource::Default,
        {},
        [](const ConfigValue& v) {
            return validateDoubleRange(v, 1.0, 100.0);
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        STDP_TAU_MINUS,
        "STDP time constant for depression (ms)",
        ConfigSource::Default,
        {},
        [](const ConfigValue& v) {
            return validateDoubleRange(v, 1.0, 100.0);
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        HEBBIAN_ENABLE,
        "Enable Hebbian learning",
        ConfigSource::Default,
        {"true", "false"},
        [](const ConfigValue& v) {
            if constexpr (std::is_same_v<bool, std::decay_t<decltype(v)>>) {
                return true;
            }
            return false;
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        STRUCTURAL_PLASTICITY_ENABLE,
        "Enable structural plasticity (synaptogenesis/pruning)",
        ConfigSource::Default,
        {"true", "false"},
        [](const ConfigValue& v) {
            if constexpr (std::is_same_v<bool, std::decay_t<decltype(v)>>) {
                return true;
            }
            return false;
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        SYNAPTOGENESIS_RATE,
        "Synaptogenesis rate",
        ConfigSource::Default,
        {},
        [](const ConfigValue& v) {
            return validateDoubleRange(v, 0.0, 1.0);
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        PRUNING_RATE,
        "Pruning rate",
        ConfigSource::Default,
        {},
        [](const ConfigValue& v) {
            return validateDoubleRange(v, 0.0, 1.0);
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        DOPAMINE_SCALE,
        "Dopamine modulation scale",
        ConfigSource::Default,
        {},
        [](const ConfigValue& v) {
            return validateDoubleRange(v, 0.0, 10.0);
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        CURIOUSITY_ENABLE,
        "Enable curiosity-driven exploration",
        ConfigSource::Default,
        {"true", "false"},
        [](const ConfigValue& v) {
            if constexpr (std::is_same_v<bool, std::decay_t<decltype(v)>>) {
                return true;
            }
            return false;
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        NOVELTY_ENABLE,
        "Enable novelty detection",
        ConfigSource::Default,
        {"true", "false"},
        [](const ConfigValue& v) {
            if constexpr (std::is_same_v<bool, std::decay_t<decltype(v)>>) {
                return true;
            }
            return false;
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        PREDICTION_ERROR_WEIGHT,
        "Prediction error weight",
        ConfigSource::Default,
        {},
        [](const ConfigValue& v) {
            return validateDoubleRange(v, 0.0, 1.0);
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        DEVELOPMENT_ENABLE,
        "Enable developmental system",
        ConfigSource::Default,
        {"true", "false"},
        [](const ConfigValue& v) {
            if constexpr (std::is_same_v<bool, std::decay_t<decltype(v)>>) {
                return true;
            }
            return false;
        }
    ));
    
    registerParameterSchema(ParameterSchema(
        DEVELOPMENT_STAGE,
        "Initial developmental stage",
        ConfigSource::Default,
        {"Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"},
        [](const ConfigValue& v) {
            if constexpr (std::is_same_v<std::string, std::decay_t<decltype(v)>>) {
                const std::string& str = v;
                return str == "Initial" || str == "CriticalPeriod" || str == "Maturation" || 
                       str == "Adult" || str == "Aging";
            }
            return false;
        }
    ));
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigLoadException(filepath, "Cannot open file");
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
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const ConfigValidationException& e) {
                throw ConfigLoadException(filepath, 
                    "Line " + std::to_string(lineNumber) + ": " + e.what());
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
        throw ConfigSaveException(filepath, "Cannot open file for writing");
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        // Use visit to get the string representation
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    return true;
}

void Config::validateAndStore(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate against parameter schema if available
    if (hasSchema(key)) {
        const ParameterSchema& schema = getSchema(key);
        if (!schema.validateValue(value)) {
            ValidationResult result = ValidationResult::InvalidType;
            std::string error = "Value does not conform to parameter schema";
            
            // Provide more specific error messages
            if (schema.allowedValues.empty()) {
                result = ValidationResult::InvalidType;
                error = "Invalid type or value for parameter \"" + key + "\"";
            } else {
                result = ValidationResult::InvalidFormat;
                error = "Value must be one of: " + join(schema.allowedValues, ", ");
            }
            
            throw ConfigValidationException(key, result, error);
        }
    }
    
    // Store the value (will be validated later on access)
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->validationResult = ValidationResult::Success;
        it->validationError = "";
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    validateAndStore(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue configValue = value;
    validateAndStore(key, configValue, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    ConfigValue configValue = value;
    validateAndStore(key, configValue, source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    ConfigValue configValue = value;
    validateAndStore(key, configValue, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    ConfigValue configValue = value;
    validateAndStore(key, configValue, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    ConfigValue configValue = value;
    validateAndStore(key, configValue, source);
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
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
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

ValidationResult Config::validate(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return ValidationResult::MissingRequired;
    }
    
    ValidationResult result = validateEntry(*it);
    it->validationResult = result;
    return result;
}

std::string Config::validationError(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "Key not found";
    }
    
    return it->validationError;
}

bool Config::isValid(const std::string& key) const {
    return validate(key) == ValidationResult::Success;
}

std::vector<std::string> Config::getInvalidKeys() const {
    std::vector<std::string> invalidKeys;
    for (const auto& entry : pImpl->entries) {
        if (validateEntry(entry) != ValidationResult::Success) {
            invalidKeys.push_back(entry.key);
        }
    }
    return invalidKeys;
}

void Config::registerParameterSchema(const ParameterSchema& schema) {
    parameterSchemas[schema.key] = schema;
}

bool Config::hasSchema(const std::string& key) const {
    return parameterSchemas.find(key) != parameterSchemas.end();
}

const ParameterSchema& Config::getSchema(const std::string& key) const {
    auto it = parameterSchemas.find(key);
    if (it == parameterSchemas.end()) {
        throw ConfigValidationException(key, ValidationResult::InvalidType, 
            "No parameter schema found for key\"" + key + "\"");
    }
    return it->second;
}

bool Config::validateIntRange(const ConfigValue& value, int min, int max) {
    if constexpr (std::is_same_v<int, std::decay_t<decltype(value)>>) {
        return value >= min && value <= max;
    }
    return false;
}

bool Config::validateDoubleRange(const ConfigValue& value, double min, double max) {
    if constexpr (std::is_same_v<double, std::decay_t<decltype(value)>>) {
        return value >= min && value <= max;
    }
    return false;
}

bool Config::validateStringChoices(const ConfigValue& value, const std::vector<std::string>& choices) {
    if constexpr (std::is_same_v<std::string, std::decay_t<decltype(value)>>) {
        return std::find(choices.begin(), choices.end(), value) != choices.end();
    }
    return false;
}

bool Config::validateVectorSize(const ConfigValue& value, size_t minSize, size_t maxSize) {
    if constexpr (std::is_same_v<std::vector<int>, std::decay_t<decltype(value)>>) {
        return value.size() >= minSize && value.size() <= maxSize;
    }
    return false;
}

ValidationResult Config::validateEntry(const ConfigEntry& entry) const {
    // Check if entry exists
    if (entry.key.empty()) {
        return ValidationResult::InvalidFormat;
    }
    
    // Check schema validation if available
    if (hasSchema(entry.key)) {
        const ParameterSchema& schema = getSchema(entry.key);
        if (!schema.validateValue(entry.value)) {
            return ValidationResult::SchemaViolation;
        }
    }
    
    // Basic type validation
    ValidationResult result = ValidationResult::Success;
    std::string error;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            if (arg < -2147483648 || arg > 2147483647) {
                result = ValidationResult::OutOfRange;
                error = "Integer value out of range";
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (arg < -9223372036854775808 || arg > 9223372036854775807) {
                result = ValidationResult::OutOfRange;
                error = "64-bit integer value out of range";
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::isnan(arg) || std::isinf(arg)) {
                result = ValidationResult::InvalidFormat;
                error = "Invalid double value (NaN or infinity)";
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            // Boolean values are always valid
        } else if constexpr (std::is_same_v<T, std::string>) {
            // String validation - check for empty string if required
            if (arg.empty()) {
                result = ValidationResult::InvalidFormat;
                error = "String value cannot be empty";
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            // Validate vector elements
            for (const auto& elem : arg) {
                if (elem < -2147483648 || elem > 2147483647) {
                    result = ValidationResult::OutOfRange;
                    error = "Vector element out of range";
                    break;
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& elem : arg) {
                if (std::isnan(elem) || std::isinf(elem)) {
                    result = ValidationResult::InvalidFormat;
                    error = "Vector contains invalid double values";
                    break;
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& elem : arg) {
                if (elem.empty()) {
                    result = ValidationResult::InvalidFormat;
                    error = "Vector contains empty string";
                    break;
                }
            }
        }
    }, entry.value);
    
    if (result != ValidationResult::Success) {
        // Update entry validation info
        const_cast<ConfigEntry&>(entry).validationResult = result;
        const_cast<ConfigEntry&>(entry).validationError = error;
        return result;
    }
    
    return ValidationResult::Success;
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

// Helper function for string joining
template<typename T>
std::string Config::join(const std::vector<T>& vec, const std::string& separator) {
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) oss << separator;
        oss << vec[i];
    }
    return oss.str();
}

} // namespace nlm

