#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ParameterValidation> parameterDefinitions;
    std::vector<ConfigEntry> defaultEntries;
};

// Helper function to validate a configuration value against parameter validation
bool validateConfigValue(const std::string& key, const ConfigValue& value, const ParameterValidation& param) {
    if (!param.validator) {
        return true; // No validation required
    }
    
    try {
        return param.validator(value);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Validation error for parameter '" + key + "': " + std::string(e.what()));
        return false;
    }
}

// Helper function to normalize a configuration value
double normalizeConfigValue(const ConfigValue& value) {
    for (const auto& param : DefaultParameters::getAll()) {
        if (param.normalizer) {
            // Find if this parameter matches
            return param.normalizer(value);
        }
    }
    return 0.0; // Default normalization
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize parameter definitions
    for (const auto& param : DefaultParameters::getAll()) {
        pImpl->parameterDefinitions[param.name] = param;
    }
    
    // Set default values
    for (const auto& param : DefaultParameters::getAll()) {
        if (param.defaultSource == ConfigSource::Default) {
            ConfigValue defaultValue;
            if (param.name == "brain.neuron_count") {
                defaultValue = 1000;
            } else if (param.name == "brain.region_count") {
                defaultValue = 1;
            } else if (param.name == "brain.synapse_density") {
                defaultValue = 0.1;
            } else if (param.name.find("neuron.v_")) {
                // Set typical neuron parameters
                if (param.name == "neuron.v_thresh") defaultValue = -50.0;
                else if (param.name == "neuron.v_rest") defaultValue = -70.0;
                else if (param.name == "neuron.v_reset") defaultValue = -75.0;
            } else if (param.name.find("plasticity.stdp")) {
                if (param.name == "plasticity.stdp.learning_rate") defaultValue = 0.001;
                else if (param.name == "plasticity.stdp.tau") defaultValue = 20.0;
            } else if (param.name.find("plasticity.hebbian")) {
                if (param.name == "plasticity.hebbian.learning_rate") defaultValue = 0.01;
            } else if (param.name.find("neuromod.dopamine")) {
                if (param.name == "neuromod.dopamine.scale") defaultValue = 1.0;
            } else if (param.name.find("neuromod.curiosity")) {
                if (param.name == "neuromod.curiosity.scale") defaultValue = 0.5;
            } else if (param.name.find("development.critical_period")) {
                if (param.name == "development.critical_period.duration") defaultValue = 60.0;
            } else if (param.name.find("development.maturation")) {
                if (param.name == "development.maturation.rate") defaultValue = 0.3;
            } else if (param.name.find("memory.working")) {
                if (param.name == "memory.working.capacity") defaultValue = 100;
            } else if (param.name.find("memory.episodic")) {
                if (param.name == "memory.episodic.max_entries") defaultValue = 1000;
            } else if (param.name.find("simulation.timestep")) {
                if (param.name == "simulation.timestep") defaultValue = 0.001;
            } else if (param.name.find("simulation.max_steps")) {
                if (param.name == "simulation.max_steps") defaultValue = 10000;
            }
            
            if (!std::holds_alternative<std::monostate>(defaultValue)) {
                // Validate the default value
                if (validateConfigValue(param.name, defaultValue, param)) {
                    pImpl->defaultEntries.emplace_back(param.name, defaultValue, param.defaultSource, param.description);
                }
            }
        }
    }
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to detect file format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    // Simple key=value format for now
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
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
            
            // Parse value based on type
            ConfigValue configValue;
            bool parsed = false;
            
            // Try to parse as boolean
            if (value == "true" || value == "false") {
                configValue = (value == "true");
                parsed = true;
            }
            // Try to parse as integer
            else {
                try {
                    size_t pos2;
                    int intVal = std::stoi(value, &pos2);
                    if (pos2 == value.size()) {
                        configValue = intVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try to parse as double
            if (!parsed) {
                try {
                    size_t pos2;
                    double doubleVal = std::stod(value, &pos2);
                    if (pos2 == value.size()) {
                        configValue = doubleVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Treat as string if not parsed
            if (!parsed) {
                configValue = value;
            }
            
            // Validate against parameter definition if exists
            auto itDef = pImpl->parameterDefinitions.find(key);
            if (itDef != pImpl->parameterDefinitions.end()) {
                if (!validateConfigValue(key, configValue, itDef->second)) {
                    NLM_LOG_WARNING("Invalid value for parameter '" + key + "' in file " + filepath + ": " + value);
                    continue;
                }
            }
            
            set(key, configValue, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Invalid config line in " + filepath + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from file: " + filepath);
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });

    if (it == pImpl->entries.end()) {
        // Try to get from defaults
        auto itDefault = std::find_if(pImpl->defaultEntries.begin(), pImpl->defaultEntries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (itDefault != pImpl->defaultEntries.end()) {
            try {
                return std::get<T>(itDefault->value);
            } catch (const std::bad_variant_access&) {
                return std::nullopt;
            }
        }
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARNING("Type mismatch for configuration key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate against parameter definition if exists
    auto itDef = pImpl->parameterDefinitions.find(key);
    if (itDef != pImpl->parameterDefinitions.end()) {
        if (!validateConfigValue(key, value, itDef->second)) {
            throw std::runtime_error("Invalid configuration value for parameter: " + key);
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_INFO("Updated configuration parameter: " + key + " = " + (std::holds_alternative<std::string>(value) ? std::get<std::string>(value) : "[complex value]"));
    } else {
        // Find parameter description for new entries
        std::string description = "Custom parameter";
        if (itDef != pImpl->parameterDefinitions.end()) {
            description = itDef->second.description;
        }
        
        pImpl->entries.emplace_back(key, value, source, description);
        NLM_LOG_INFO("Added configuration parameter: " + key);
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
    oss << "Configuration Summary:\n";
    oss << "===================\n";
    oss << "Total entries: " << (pImpl->entries.size() + pImpl->defaultEntries.size()) << "\n\n";
    
    oss << "User-defined parameters:\n";
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
        oss << "] (" << static_cast<int>(entry.source) << ") " << entry.description << "\n";
    }
    
    if (!pImpl->defaultEntries.empty()) {
        oss << "\nDefault parameters:\n";
        for (const auto& entry : pImpl->defaultEntries) {
            auto itDef = pImpl->parameterDefinitions.find(entry.key);
            if (itDef != pImpl->parameterDefinitions.end()) {
                oss << "  " << entry.key << " = [";
                std::visit([&oss](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        oss << "\"" << arg << "\"";
                    } else {
                        oss << arg;
                    }
                }, entry.value);
                oss << "] (default) " << itDef->second.description << "\n";
            }
        }
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
