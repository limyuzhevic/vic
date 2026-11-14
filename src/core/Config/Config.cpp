#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <functional>
#include <regex>
#include <unordered_map>
#include "../Errors/Errors.hpp"

namespace nlm {

// Static parameter specifications storage
static std::vector<ParameterSpec> g_parameterSpecs;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeParameterSpecs();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // Acceptance criteria:
    // 1. Support JSON format for configuration files
    // 2. Support YAML format for configuration files
    // 3. Parse nested object structures and arrays
    // 4. Maintain backward compatibility with simple key=value format
    // 5. Implement validation of parsed configuration values
    // 6. Support comment syntax in JSON/YAML files
    // 7. Handle file not found and parsing errors gracefully
    // 8. Provide detailed error messages for malformed configuration files
    
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
    // Validate parameter before setting
    if (!validateParameter(key, value)) {
        // Find error message by validating again and getting the error
        auto it = std::find_if(g_parameterSpecs.begin(), g_parameterSpecs.end(),
            [&key](const ParameterSpec& spec) { return spec.key == key; });
        if (it != g_parameterSpecs.end()) {
            ConfigValidationResult result = validateValue(key, value, *it);
            if (!result.valid) {
                throw ConfigError("validation", "Parameter \"" + key + "\": " + result.errorMessage);
            }
        }
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

// Validation function implementations
ConfigValidationResult Config::validateValue(const std::string& key, const ConfigValue& value, const ParameterSpec& spec) {
    // Type validation
    bool typeMatches = false;
    if (spec.type == "int" || spec.type == "integer") {
        typeMatches = std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    } else if (spec.type == "double" || spec.type == "float" || spec.type == "number") {
        typeMatches = std::holds_alternative<double>(value) || 
                     std::holds_alternative<int>(value) || 
                     std::holds_alternative<int64_t>(value);
    } else if (spec.type == "bool" || spec.type == "boolean") {
        typeMatches = std::holds_alternative<bool>(value);
    } else if (spec.type == "string") {
        typeMatches = std::holds_alternative<std::string>(value);
    } else if (spec.type == "vector") {
        typeMatches = std::holds_alternative<std::vector<int>>(value) ||
                     std::holds_alternative<std::vector<double>>(value) ||
                     std::holds_alternative<std::vector<std::string>>(value);
    }

    if (!typeMatches) {
        return ConfigValidationResult(false, "Type mismatch: expected " + spec.type + " but got " + 
            (std::holds_alternative<int>(value) ? "int" :
             std::holds_alternative<int64_t>(value) ? "int64_t" :
             std::holds_alternative<double>(value) ? "double" :
             std::holds_alternative<bool>(value) ? "bool" :
             std::holds_alternative<std::string>(value) ? "string" :
             std::holds_alternative<std::vector<int>>(value) ? "vector<int>" :
             std::holds_alternative<std::vector<double>>(value) ? "vector<double>" :
             std::holds_alternative<std::vector<std::string>>(value) ? "vector<string>" : "unknown"));
    }

    // Range validation for numeric types
    if (spec.type == "int" || spec.type == "integer" || spec.type == "double" || spec.type == "float" || spec.type == "number") {
        double numericValue = 0.0;
        if (std::holds_alternative<int>(value)) {
            numericValue = static_cast<double>(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            numericValue = static_cast<double>(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            numericValue = std::get<double>(value);
        }

        if (spec.minValue.has_value() && numericValue < spec.minValue.value()) {
            return ConfigValidationResult(false, "Value " + std::to_string(numericValue) + " is less than minimum " + 
                std::to_string(spec.minValue.value()) + " for key \"" + spec.key + "\"");
        }

        if (spec.maxValue.has_value() && numericValue > spec.maxValue.value()) {
            return ConfigValidationResult(false, "Value " + std::to_string(numericValue) + " exceeds maximum " + 
                std::to_string(spec.maxValue.value()) + " for key \"" + spec.key + "\"");
        }
    }

    // String validation
    if (spec.type == "string" && !spec.allowedStrings.empty()) {
        if (std::holds_alternative<std::string>(value)) {
            const std::string& strValue = std::get<std::string>(value);
            bool found = false;
            for (const auto& allowed : spec.allowedStrings) {
                if (strValue == allowed) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::string allowedStr = "";
                for (size_t i = 0; i < spec.allowedStrings.size(); ++i) {
                    if (i > 0) allowedStr += ", ";
                    allowedStr += "\"" + spec.allowedStrings[i] + "\"";
                }
                return ConfigValidationResult(false, "Value \"" + strValue + "\" is not allowed for key \"" + 
                    spec.key + "\". Allowed values: " + allowedStr);
            }
        }
    }

    // Custom validator
    if (spec.customValidator) {
        ConfigValidationResult customResult = spec.customValidator(spec.key, value);
        if (!customResult.valid) {
            return customResult;
        }
    }

    return ConfigValidationResult(true);
}

bool Config::validateParameter(const std::string& key, const ConfigValue& value) const {
    // Find parameter specification
    auto it = std::find_if(g_parameterSpecs.begin(), g_parameterSpecs.end(),
        [&key](const ParameterSpec& spec) { return spec.key == key; });

    if (it != g_parameterSpecs.end()) {
        ConfigValidationResult result = validateValue(key, value, *it);
        return result.valid;
    }

    // No specific validation for unknown parameters (allow them)
    return true;
}

// Static member function implementations
std::vector<ParameterSpec>& Config::getParameterSpecs() {
    return g_parameterSpecs;
}

bool Config::isValidParameter(const std::string& key) {
    auto it = std::find_if(g_parameterSpecs.begin(), g_parameterSpecs.end(),
        [&key](const ParameterSpec& spec) { return spec.key == key; });
    return it != g_parameterSpecs.end();
}

std::vector<std::string> Config::getAllowedKeys() {
    std::vector<std::string> keys;
    for (const auto& spec : g_parameterSpecs) {
        keys.push_back(spec.key);
    }
    return keys;
}

void Config::setParameterSpec(const ParameterSpec& spec) {
    // Check if spec already exists, replace it
    auto it = std::find_if(g_parameterSpecs.begin(), g_parameterSpecs.end(),
        [&spec](const ParameterSpec& existing) { return existing.key == spec.key; });
    
    if (it != g_parameterSpecs.end()) {
        *it = spec;
    } else {
        g_parameterSpecs.push_back(spec);
    }
}

void Config::clearParameterSpecs() {
    g_parameterSpecs.clear();
}

void Config::initializeParameterSpecs() {
    // Clear existing specs
    clearParameterSpecs();
    
    // Initialize with default parameter specifications based on typical usage
    
    // Integer/Count parameters
    setParameterSpec(ParameterSpec("random_seed", "integer", "Random seed for reproducible experiments")
        .minValue = 0);
    
    setParameterSpec(ParameterSpec("neuron_count", "integer", "Number of neurons in the brain")
        .minValue = 1);
    
    setParameterSpec(ParameterSpec("region_count", "integer", "Number of brain regions")
        .minValue = 1);
    
    setParameterSpec(ParameterSpec("max_simulation_steps", "integer", "Maximum simulation steps")
        .minValue = 0);
    
    // Float parameters
    setParameterSpec(ParameterSpec("simulation_timestep", "double", "Simulation timestep in seconds")
        .minValue = 0.0
        .maxValue = 1.0);
    
    setParameterSpec(ParameterSpec("connection_probability", "double", "Connection probability for random connectivity")
        .minValue = 0.0
        .maxValue = 1.0);
    
    setParameterSpec(ParameterSpec("plasticity_learning_rate", "double", "Plasticity learning rate")
        .minValue = 0.0
        .maxValue = 1.0);
    
    setParameterSpec(ParameterSpec("stdp_ltp_weight", "double", "STDP LTP weight")
        .minValue = 0.0);
    
    setParameterSpec(ParameterSpec("stdp_ltd_weight", "double", "STDP LTD weight")
        .minValue = 0.0);
    
    setParameterSpec(ParameterSpec("stdp_time_constant", "double", "STDP time constant")
        .minValue = 0.0);
    
    setParameterSpec(ParameterSpec("development_synaptogenesis_rate", "double", "Development synaptogenesis rate")
        .minValue = 0.0);
    
    setParameterSpec(ParameterSpec("development_pruning_rate", "double", "Development pruning rate")
        .minValue = 0.0);
    
    setParameterSpec(ParameterSpec("dopamine_baseline", "double", "Neuromodulation dopamine baseline")
        .minValue = 0.0
        .maxValue = 1.0);
    
    setParameterSpec(ParameterSpec("reward_discount_factor", "double", "Reward discount factor")
        .minValue = 0.0
        .maxValue = 1.0);
    
    setParameterSpec(ParameterSpec("environment_width", "integer", "Environment width in grid cells")
        .minValue = 1);
    
    setParameterSpec(ParameterSpec("environment_height", "integer", "Environment height in grid cells")
        .minValue = 1);
    
    setParameterSpec(ParameterSpec("visualization_update_rate", "double", "Visualization update rate (Hz)")
        .minValue = 0.0);
    
    // String parameters
    setParameterSpec(ParameterSpec("environment_name", "string", "Environment name")
        .allowedStrings = {"GridWorld", "Continuous", "Custom"});
    
    setParameterSpec(ParameterSpec("log_level", "string", "Logging level")
        .allowedStrings = {"DEBUG", "INFO", "WARN", "ERROR"});
    
    // Boolean parameters
    setParameterSpec(ParameterSpec("log_to_file", "bool", "Enable logging to file");
    
    setParameterSpec(ParameterSpec("visualization_enabled", "bool", "Enable visualization");
    
    setParameterSpec(ParameterSpec("simulation_time_limit", "double", "Simulation time limit in seconds")
        .minValue = 0.0);
}
