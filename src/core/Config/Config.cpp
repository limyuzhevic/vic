#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <set>
#include <iomanip>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigParameter> parameters;
    std::map<std::string, std::vector<std::function<bool(const ConfigValue&)>>> validators;
    std::set<std::string> requiredKeys;
};

// Get type name from ConfigValue
template<typename T>
std::string getTypeName() {
    if constexpr (std::is_same_v<T, int>) return "int";
    else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
    else if constexpr (std::is_same_v<T, double>) return "double";
    else if constexpr (std::is_same_v<T, bool>) return "bool";
    else if constexpr (std::is_same_v<T, std::string>) return "string";
    else if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
    else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
    else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
    else return "unknown";
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeDefaultParameters();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultParameters() {
    // Define the complete configuration schema with validation rules
    
    // Random seed parameters
    pImpl->parameters["random_seed"] = ConfigParameter(
        "random_seed", "Random number generator seed", ConfigSource::Default, true, 0, 9223372036854775807
    );
    
    // Simulation parameters
    pImpl->parameters["simulation_timestep"] = ConfigParameter(
        "simulation_timestep", "Simulation time step in seconds", ConfigSource::Default, true, 0.0, 1.0
    );
    
    // Neural network parameters
    pImpl->parameters["neuron_count"] = ConfigParameter(
        "neuron_count", "Total number of neurons in the network", ConfigSource::Default, true, 0, 1000000
    );
    
    pImpl->parameters["region_count"] = ConfigParameter(
        "region_count", "Number of neural regions", ConfigSource::Default, true, 1, 100
    );
    
    pImpl->parameters["connection_probability"] = ConfigParameter(
        "connection_probability", "Probability of connection between neurons", ConfigSource::Default, true, 0.0, 1.0
    );
    
    // STDP parameters
    pImpl->parameters["stdp_ltp_weight"] = ConfigParameter(
        "stdp_ltp_weight", "STDP Long-Term Potentiation weight factor", ConfigSource::Default, true, 0.0, 10.0
    );
    
    pImpl->parameters["stdp_ltd_weight"] = ConfigParameter(
        "stdp_ltd_weight", "STDP Long-Term Depression weight factor", ConfigSource::Default, true, 0.0, 10.0
    );
    
    pImpl->parameters["stdp_tau"] = ConfigParameter(
        "stdp_tau", "STDP time constant (ms)", ConfigSource::Default, true, 0.0, 1000.0
    );
    
    // Structural plasticity parameters
    pImpl->parameters["synaptogenesis_rate"] = ConfigParameter(
        "synaptogenesis_rate", "Rate of new synapse formation", ConfigSource::Default, true, 0.0, 1.0
    );
    
    pImpl->parameters["pruning_rate"] = ConfigParameter(
        "pruning_rate", "Rate of synapse removal", ConfigSource::Default, true, 0.0, 1.0
    );
    
    // Advanced configuration options
    pImpl->parameters["enable_debug_logging"] = ConfigParameter(
        "enable_debug_logging", "Enable debug logging output", ConfigSource::Default, false, 0.0, 1.0
    );
    
    pImpl->parameters["max_neurons_per_region"] = ConfigParameter(
        "max_neurons_per_region", "Maximum neurons per region", ConfigSource::Default, true, 1, 10000
    );
    
    pImpl->parameters["learning_rate"] = ConfigParameter(
        "learning_rate", "Global learning rate factor", ConfigSource::Default, true, 0.0, 10.0
    );
    
    // Additional advanced parameters
    pImpl->parameters["max_synapses_per_neuron"] = ConfigParameter(
        "max_synapses_per_neuron", "Maximum synapses per neuron", ConfigSource::Default, true, 1, 10000
    );
    
    pImpl->parameters["min_weight"] = ConfigParameter(
        "min_weight", "Minimum synaptic weight value", ConfigSource::Default, true, -1.0, 1.0
    );
    
    pImpl->parameters["max_weight"] = ConfigParameter(
        "max_weight", "Maximum synaptic weight value", ConfigSource::Default, true, -1.0, 1.0
    );
    
    // Validate required parameters
    for (const auto& [key, param] : pImpl->parameters) {
        if (param.required) {
            pImpl->requiredKeys.insert(key);
        }
    }
}

bool Config::validateValue(const ConfigEntry& entry) const {
    const auto& paramIt = pImpl->parameters.find(entry.key);
    if (paramIt == pImpl->parameters.end()) {
        return true;
    }
    
    const auto& param = paramIt->second;
    
    if (!validateType(entry.key, entry.value, param)) {
        return false;
    }
    
    if (param.hasRange && !validateRange(entry.key, entry.value, param)) {
        return false;
    }
    
    auto validatorIt = pImpl->validators.find(entry.key);
    if (validatorIt != pImpl->validators.end()) {
        for (const auto& validator : validatorIt->second) {
            if (!validator(entry.value)) {
                return false;
            }
        }
    }
    
    return true;
}

bool Config::validateType(const std::string& key, const ConfigValue& value, const ConfigParameter& param) const {
    if (key == "simulation_timestep" || key == "stdp_ltp_weight" || 
        key == "stdp_ltd_weight" || key == "stdp_tau" ||
        key == "synaptogenesis_rate" || key == "pruning_rate" ||
        key == "learning_rate") {
        return std::holds_alternative<double>(value);
    } else if (key == "random_seed" || key == "neuron_count" || 
               key == "region_count" || key == "max_neurons_per_region" ||
               key == "max_synapses_per_neuron") {
        return std::holds_alternative<int64_t>(value) || std::holds_alternative<int>(value);
    } else if (key == "connection_probability" || key == "enable_debug_logging") {
        return std::holds_alternative<double>(value);
    } else if (key == "enable_debug_logging") {
        return std::holds_alternative<bool>(value);
    }
    
    return true;
}

bool Config::validateRange(const std::string& key, const ConfigValue& value, const ConfigParameter& param) const {
    double val;
    if (std::holds_alternative<int>(value)) {
        val = static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        val = static_cast<double>(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        val = std::get<double>(value);
    } else {
        return false;
    }
    
    if (val < param.minValue || val > param.maxValue) {
        throw ConfigRangeError(key, val, param.minValue, param.maxValue);
    }
    return true;
}

void Config::validateAndSet(const std::string& key, const ConfigValue& value, ConfigSource source) {
    ConfigEntry entry(key, value, source);
    
    if (!validateValue(entry)) {
        throw ConfigValidationError("Invalid configuration value for key: " + key);
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

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
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
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
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
        throw ConfigTypeError(key, getTypeName<T>());
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    } else {
        auto paramIt = pImpl->parameters.find(key);
        if (paramIt != pImpl->parameters.end() && paramIt->second.required) {
            throw ConfigValidationError("Required configuration key \"" + key + "\" is missing");
        }
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    validateAndSet(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    try {
        size_t pos;
        int intVal = std::stoi(value, &pos);
        if (pos == value.size()) {
            set(key, ConfigValue(intVal), source);
            return;
        }
        
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) {
            set(key, ConfigValue(doubleVal), source);
            return;
        }
        
        if (value == "true" || value == "1") {
            set(key, ConfigValue(true), source);
            return;
        } else if (value == "false" || value == "0") {
            set(key, ConfigValue(false), source);
            return;
        }
        
        set(key, ConfigValue(value), source);
    } catch (const std::exception&) {
        set(key, ConfigValue(value), source);
    }
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
    initializeDefaultParameters();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Schema (" << pImpl->parameters.size() << " parameters):" << std::endl;
    oss << "  Required keys: " << pImpl->requiredKeys.size() << std::endl;
    oss << "  Current entries: " << pImpl->entries.size() << std::endl;
    oss << std::endl;
    
    oss << "Available Parameters:" << std::endl;
    for (const auto& [key, param] : pImpl->parameters) {
        oss << "  " << key << ": " << param.description;
        if (param.required) oss << " [REQUIRED]";
        if (param.hasRange) oss << " [" << param.minValue << ", " << param.maxValue << "]";
        oss << std::endl;
    }
    
    oss << std::endl << "Current Configuration:" << std::endl;
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
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

template std::optional<int> nlm::Config::get<int>(const std::string&) const;
template std::optional<int64_t> nlm::Config::get<int64_t>(const std::string&) const;
template std::optional<double> nlm::Config::get<double>(const std::string&) const;
template std::optional<bool> nlm::Config::get<bool>(const std::string&) const;
template std::optional<std::string> nlm::Config::get<std::string>(const std::string&) const;

template int nlm::Config::getOr<int>(const std::string&, const int&) const;
template int64_t nlm::Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double nlm::Config::getOr<double>(const std::string&, const double&) const;
template bool nlm::Config::getOr<bool>(const std::string&, const bool&) const;
template std::string nlm::Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
