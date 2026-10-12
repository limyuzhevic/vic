#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default configuration parameters
    initializeDefaultParameters();
}

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO: Implement proper JSON/YAML parser
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
        // Properly serialize the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
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

// Get a ConfigValue from the parameter (not typed)
ConfigValue Config::getValue(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigValue{};  // Return empty variant
    }
    
    return it->value;
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

// Initialize default configuration parameters
void Config::initializeDefaultParameters() {
    // Neural computation parameters
    registerParameter(ConfigParameter{
        "random_seed", 42,
        "Random seed for reproducible experiments",
        true, "^[0-9]+$", -1000000, 1000000
    });
    
    registerParameter(ConfigParameter{
        "simulation_timestep", 0.001,
        "Simulation timestep in seconds",
        true, "^[0-9]+(\.[0-9]+)?$", 1e-6, 1.0
    });
    
    registerParameter(ConfigParameter{
        "neuron_count", 1000,
        "Number of neurons in the brain",
        true, "^[0-9]+$", 1, 1000000
    });
    
    registerParameter(ConfigParameter{
        "region_count", 2,
        "Number of brain regions",
        true, "^[0-9]+$", 1, 1000
    });
    
    registerParameter(ConfigParameter{
        "connection_probability", 0.1f,
        "Connection probability for random connectivity",
        true, "^[0-1](\.[0-9]+)?$", 0.0, 1.0
    });
    
    // Plasticity settings
    registerParameter(ConfigParameter{
        "plasticity_learning_rate", 0.01f,
        "Base learning rate for plasticity",
        true, "^[0-9]+(\.[0-9]+)?$", 0.0, 1.0
    });
    
    registerParameter(ConfigParameter{
        "stdp_ltp_weight", 0.01f,
        "STDP Long-Term Potentiation weight",
        true, "^[0-9]+(\.[0-9]+)?$", 0.0, 0.1
    });
    
    registerParameter(ConfigParameter{
        "stdp_ltd_weight", 0.012f,
        "STDP Long-Term Depression weight",
        true, "^[0-9]+(\.[0-9]+)?$", 0.0, 0.1
    });
    
    registerParameter(ConfigParameter{
        "stdp_time_constant", 20.0f,
        "STDP time constant in simulation steps",
        true, "^[0-9]+(\.[0-9]+)?$", 1.0, 1000.0
    });
    
    // Development settings
    registerParameter(ConfigParameter{
        "development_synaptogenesis_rate", 0.001f,
        "Rate of new synapse formation during development",
        true, "^[0-9]+(\.[0-9]+)?$", 0.0, 0.1
    });
    
    registerParameter(ConfigParameter{
        "development_pruning_rate", 0.0001f,
        "Rate of synapse removal during development",
        true, "^[0-9]+(\.[0-9]+)?$", 0.0, 0.01
    });
    
    // Neuromodulation settings
    registerParameter(ConfigParameter{
        "dopamine_baseline", 0.1f,
        "Baseline dopamine level",
        true, "^[0-9]+(\.[0-9]+)?$", 0.0, 1.0
    });
    
    // Reward settings
    registerParameter(ConfigParameter{
        "reward_discount_factor", 0.99f,
        "Factor for discounting future rewards",
        true, "^[0-1](\.[0-9]+)?$", 0.0, 1.0
    });
    
    // Environment settings
    registerParameter(ConfigParameter{
        "environment_name", std::string("GridWorld"),
        "Name of the environment simulation",
        true
    });
    
    registerParameter(ConfigParameter{
        "environment_width", 10,
        "Width of the environment grid",
        true, "^[0-9]+$", 1, 1000
    });
    
    registerParameter(ConfigParameter{
        "environment_height", 10,
        "Height of the environment grid",
        true, "^[0-9]+$", 1, 1000
    });
    
    // Logging settings
    registerParameter(ConfigParameter{
        "log_level", std::string("INFO"),
        "Logging level (DEBUG, INFO, WARNING, ERROR)",
        true
    });
    
    registerParameter(ConfigParameter{
        "log_to_file", false,
        "Whether to log to file",
        true, "^(true|false)$"
    });
    
    registerParameter(ConfigParameter{
        "log_filename", std::string("nlm.log"),
        "Log file name",
        true
    });
    
    // Simulation settings
    registerParameter(ConfigParameter{
        "max_simulation_steps", 10000,
        "Maximum number of simulation steps",
        true, "^[0-9]+$", 1, 1000000
    });
    
    registerParameter(ConfigParameter{
        "simulation_time_limit", 0.0f,
        "Maximum simulation time in seconds (0 = unlimited)",
        true, "^[0-9]+(\.[0-9]+)?$", 0.0, 1000.0
    });
    
    // Visualization settings
    registerParameter(ConfigParameter{
        "visualization_enabled", false,
        "Whether visualization is enabled",
        true, "^(true|false)$"
    });
    
    registerParameter(ConfigParameter{
        "visualization_update_rate", 30.0f,
        "Visualization update rate in Hz",
        true, "^[0-9]+(\.[0-9]+)?$", 1.0, 1000.0
    });
}

// Register a configuration parameter
void Config::registerParameter(const ConfigParameter& param) {
    auto it = std::find_if(registeredParameters.begin(), registeredParameters.end(),
        [&param](const ConfigParameter& p) { return p.key == param.key; });
    
    if (it == registeredParameters.end()) {
        registeredParameters.push_back(param);
    }
}

// Validate configuration values
bool Config::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    return errors.empty();
}

// Get validation errors
std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& param : registeredParameters) {
        if (param.required && !has(param.key)) {
            errors.push_back("Required parameter \"" + param.key + "\" is missing");
            continue;
        }
        
        if (has(param.key)) {
            auto valueOpt = getValue(param.key);  // We'll need to add this method
            if (valueOpt.has_value()) {
                if (!isValidValue(param.key, valueOpt.value())) {
                    errors.push_back("Invalid value for parameter \"" + param.key + "\": " + 
                                   "value " + valueToString(valueOpt.value()) + " violates validation");
                }
            }
        }
    }
    
    return errors;
}

// Get all registered parameters
std::vector<ConfigParameter> Config::getRegisteredParameters() const {
    return registeredParameters;
}

// Check if a value is valid for a parameter
bool Config::isValidValue(const std::string& key, const ConfigValue& value) const {
    auto paramIt = std::find_if(registeredParameters.begin(), registeredParameters.end(),
        [&key](const ConfigParameter& p) { return p.key == key; });
    
    if (paramIt == registeredParameters.end()) {
        return true;  // Unknown parameter, accept any value
    }
    
    return validateValue(*paramIt, value);
}

// Convert ConfigValue to string for display
std::string Config::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
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
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

// Validate a single parameter value
bool Config::validateValue(const ConfigParameter& param, const ConfigValue& value) {
    // Type validation - ensure the value type matches expected type
    if (!std::holds_alternative<int>(value) && 
        !std::holds_alternative<int64_t>(value) &&
        !std::holds_alternative<double>(value) &&
        !std::holds_alternative<bool>(value) &&
        !std::holds_alternative<std::string>(value) &&
        !std::holds_alternative<std::vector<int>>(value) &&
        !std::holds_alternative<std::vector<double>>(value) &&
        !std::holds_alternative<std::vector<std::string>>(value)) {
        return false;
    }
    
    // Range validation for numeric values
    if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        if (val < param.minValue || val > param.maxValue) {
            return false;
        }
    }
    
    // Allowed values validation
    if (!param.allowedValues.empty()) {
        std::string valStr;
        if (std::holds_alternative<std::string>(value)) {
            valStr = std::get<std::string>(value);
        } else if (std::holds_alternative<int>(value)) {
            valStr = std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            valStr = std::to_string(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            valStr = std::to_string(std::get<double>(value));
        } else {
            return false;  // Non-string types can't be in allowedValues
        }
        
        if (std::find(param.allowedValues.begin(), param.allowedValues.end(), valStr) == param.allowedValues.end()) {
            return false;
        }
    }
    
    return true;
}
