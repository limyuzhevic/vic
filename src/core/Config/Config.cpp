#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ParameterInfo> parameterInfo;
    std::mutex mutex;
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate the key exists and value is valid
                auto paramInfo = getParameterInfo(key);
                if (paramInfo) {
                    // Try to convert based on type
                    if (paramInfo->type == "int") {
                        try {
                            int intVal = std::stoi(value);
                            if (validate(key, intVal)) {
                                set(key, intVal, *paramInfo, ConfigSource::CommandLine);
                            } else {
                                throw ValidationError("Invalid value for parameter: " + key, key);
                            }
                        } catch (const std::exception&) {
                            throw ValidationError("Invalid integer value for parameter: " + key, key);
                        }
                    } else if (paramInfo->type == "double") {
                        try {
                            double doubleVal = std::stod(value);
                            if (validate(key, doubleVal)) {
                                set(key, doubleVal, *paramInfo, ConfigSource::CommandLine);
                            } else {
                                throw ValidationError("Invalid value for parameter: " + key, key);
                            }
                        } catch (const std::exception&) {
                            throw ValidationError("Invalid double value for parameter: " + key, key);
                        }
                    } else if (paramInfo->type == "bool") {
                        if (value == "true" || value == "false") {
                            bool boolVal = (value == "true");
                            if (validate(key, boolVal)) {
                                set(key, boolVal, *paramInfo, ConfigSource::CommandLine);
                            } else {
                                throw ValidationError("Invalid value for parameter: " + key, key);
                            }
                        } else {
                            throw ValidationError("Boolean value must be true or false", key);
                        }
                    } else if (paramInfo->type == "string") {
                        if (validate(key, value)) {
                            set(key, value, *paramInfo, ConfigSource::CommandLine);
                        } else {
                            throw ValidationError("Invalid value for parameter: " + key, key);
                        }
                    } else {
                        // Generic string value
                        if (validate(key, value)) {
                            set(key, value, ConfigSource::CommandLine);
                        } else {
                            throw ValidationError("Invalid value for parameter: " + key, key);
                        }
                    }
                } else {
                    // If parameter not registered, use generic setting
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Look for registered parameter
            auto paramInfo = getParameterInfo(key);
            if (paramInfo) {
                if (paramInfo->type == "int") {
                    try {
                        int intVal = std::stoi(value);
                        if (validate(key, intVal)) {
                            set(key, intVal, *paramInfo, ConfigSource::CommandLine);
                        } else {
                            throw ValidationError("Invalid value for parameter: " + key, key);
                        }
                    } catch (const std::exception&) {
                        throw ValidationError("Invalid integer value for parameter: " + key, key);
                    }
                } else if (paramInfo->type == "double") {
                    try {
                        double doubleVal = std::stod(value);
                        if (validate(key, doubleVal)) {
                            set(key, doubleVal, *paramInfo, ConfigSource::CommandLine);
                        } else {
                            throw ValidationError("Invalid value for parameter: " + key, key);
                        }
                    } catch (const std::exception&) {
                        throw ValidationError("Invalid double value for parameter: " + key, key);
                    }
                } else if (paramInfo->type == "bool") {
                    if (value == "true" || value == "false") {
                        bool boolVal = (value == "true");
                        if (validate(key, boolVal)) {
                            set(key, boolVal, *paramInfo, ConfigSource::CommandLine);
                        } else {
                            throw ValidationError("Invalid value for parameter: " + key, key);
                        }
                    } else {
                        throw ValidationError("Boolean value must be true or false", key);
                    }
                } else if (paramInfo->type == "string") {
                    if (validate(key, value)) {
                        set(key, value, *paramInfo, ConfigSource::CommandLine);
                    } else {
                        throw ValidationError("Invalid value for parameter: " + key, key);
                    }
                } else {
                    if (validate(key, value)) {
                        set(key, value, ConfigSource::CommandLine);
                    } else {
                        throw ValidationError("Invalid value for parameter: " + key, key);
                    }
                }
            } else {
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Look for registered parameter info
    auto it = pImpl->parameterInfo.find(key);
    
    // Get description from registered parameter info if available
    std::string description = "";
    if (it != pImpl->parameterInfo.end()) {
        description = it->second.description;
    }
    
    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entryIt != pImpl->entries.end()) {
        entryIt->value = value;
        entryIt->source = source;
        entryIt->description = description; // Update description if present
    } else {
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

// Enhanced set with parameter metadata
void Config::set(const std::string& key, const ConfigValue& value, const ParameterInfo& paramInfo, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Validate the value against parameter constraints
    if (!validate(key, value)) {
        throw ValidationError("Invalid value for parameter: " + key, key);
    }
    
    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entryIt != pImpl->entries.end()) {
        entryIt->value = value;
        entryIt->source = source;
        entryIt->description = paramInfo.description;
    } else {
        pImpl->entries.emplace_back(key, value, source, paramInfo.description);
    }
}

// Parameter metadata management
void Config::registerParameter(const ParameterInfo& paramInfo) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->parameterInfo[paramInfo.key] = paramInfo;
}

std::optional<ParameterInfo> Config::getParameterInfo(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->parameterInfo.find(key);
    if (it != pImpl->parameterInfo.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<ParameterInfo> Config::getParametersByCategory(ParameterCategory category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<ParameterInfo> result;
    for (const auto& pair : pImpl->parameterInfo) {
        if (pair.second.category == category) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<ParameterCategory> Config::getAllCategories() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<ParameterCategory> result;
    std::unordered_set<ParameterCategory> seen;
    
    for (const auto& pair : pImpl->parameterInfo) {
        if (seen.find(pair.second.category) == seen.end()) {
            result.push_back(pair.second.category);
            seen.insert(pair.second.category);
        }
    }
    
    return result;
}

// Validation
namespace {
    template<typename T>
    bool isValueInRange(const T& value, const ParameterInfo& paramInfo) {
        // Simple range validation - could be enhanced with more sophisticated logic
        return true; // Placeholder - real implementation would check ranges
    }
    
    template<typename T>
    bool isValueInValidValues(const T& value, const ParameterInfo& paramInfo) {
        if (paramInfo.validValues.empty()) return true;
        
        std::string valStr;
        if constexpr (std::is_same_v<T, std::string>) {
            valStr = value;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            valStr = std::to_string(value);
        } else if constexpr (std::is_same_v<T, double>) {
            valStr = std::to_string(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            valStr = value ? "true" : "false";
        }
        
        return std::find(paramInfo.validValues.begin(), paramInfo.validValues.end(), valStr) != paramInfo.validValues.end();
    }
}

template<typename T>
bool Config::validate(const std::string& key, const T& value) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto paramInfoIt = pImpl->parameterInfo.find(key);
    if (paramInfoIt == pImpl->parameterInfo.end()) {
        return true; // No validation for unregistered parameters
    }
    
    const ParameterInfo& paramInfo = paramInfoIt->second;
    
    // Check valid values
    if (!isValueInValidValues(value, paramInfo)) {
        return false;
    }
    
    // Check type
    bool typeMatches = false;
    if constexpr (std::is_same_v<T, int>) {
        typeMatches = paramInfo.type == "int";
    } else if constexpr (std::is_same_v<T, int64_t>) {
        typeMatches = paramInfo.type == "int64_t";
    } else if constexpr (std::is_same_v<T, double>) {
        typeMatches = paramInfo.type == "double";
    } else if constexpr (std::is_same_v<T, bool>) {
        typeMatches = paramInfo.type == "bool";
    } else if constexpr (std::is_same_v<T, std::string>) {
        typeMatches = paramInfo.type == "string";
    }
    
    return typeMatches;
}

bool Config::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Also remove parameter info
    pImpl->parameterInfo.erase(key);
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.clear();
    pImpl->parameterInfo.clear();
}

std::string Config::summary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
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

// Preset configurations
Config Config::createFastPreset() {
    Config config;
    
    // Fast preset - optimized for speed
    config.registerParameter(ParameterInfo("simulation_timestep", "Simulation timestep in seconds", ParameterCategory::Simulation, 0.001, "double"));
    config.registerParameter(ParameterInfo("neuron_count", "Number of neurons in the brain", ParameterCategory::Neural, 100, "int"));
    config.registerParameter(ParameterInfo("region_count", "Number of brain regions", ParameterCategory::Neural, 1, "int"));
    config.registerParameter(ParameterInfo("connection_probability", "Connection probability for random connectivity", ParameterCategory::Neural, 0.1, "double"));
    config.registerParameter(ParameterInfo("stdp_ltp_weight", "STDP Long-Term Potentiation weight", ParameterCategory::Plasticity, 0.01, "double"));
    config.registerParameter(ParameterInfo("stdp_ltd_weight", "STDP Long-Term Depression weight", ParameterCategory::Plasticity, 0.012, "double"));
    config.registerParameter(ParameterInfo("stdp_time_constant", "STDP time constant", ParameterCategory::Plasticity, 20.0, "double"));
    config.registerParameter(ParameterInfo("plasticity_learning_rate", "Plasticity learning rate", ParameterCategory::Plasticity, 0.01, "double"));
    config.registerParameter(ParameterInfo("development_synaptogenesis_rate", "Synaptogenesis rate", ParameterCategory::Development, 0.001, "double"));
    config.registerParameter(ParameterInfo("development_pruning_rate", "Pruning rate", ParameterCategory::Development, 0.0001, "double"));
    config.registerParameter(ParameterInfo("max_simulation_steps", "Maximum simulation steps", ParameterCategory::Simulation, 1000, "int"));
    config.registerParameter(ParameterInfo("visualization_enabled", "Enable visualization", ParameterCategory::Debug, false, "bool"));
    
    // Set preset values
    config.set("simulation_timestep", 0.001);
    config.set("neuron_count", 100);
    config.set("region_count", 1);
    config.set("connection_probability", 0.1);
    config.set("stdp_ltp_weight", 0.01);
    config.set("stdp_ltd_weight", 0.012);
    config.set("stdp_time_constant", 20.0);
    config.set("plasticity_learning_rate", 0.01);
    config.set("development_synaptogenesis_rate", 0.001);
    config.set("development_pruning_rate", 0.0001);
    config.set("max_simulation_steps", 1000);
    config.set("visualization_enabled", false);
    
    return config;
}

Config Config::createAccuratePreset() {
    Config config;
    
    // Accurate preset - optimized for precision
    config.registerParameter(ParameterInfo("simulation_timestep", "Simulation timestep in seconds", ParameterCategory::Simulation, 0.0001, "double"));
    config.registerParameter(ParameterInfo("neuron_count", "Number of neurons in the brain", ParameterCategory::Neural, 2000, "int"));
    config.registerParameter(ParameterInfo("region_count", "Number of brain regions", ParameterCategory::Neural, 2, "int"));
    config.registerParameter(ParameterInfo("connection_probability", "Connection probability for random connectivity", ParameterCategory::Neural, 0.05, "double"));
    config.registerParameter(ParameterInfo("stdp_ltp_weight", "STDP Long-Term Potentiation weight", ParameterCategory::Plasticity, 0.002, "double"));
    config.registerParameter(ParameterInfo("stdp_ltd_weight", "STDP Long-Term Depression weight", ParameterCategory::Plasticity, 0.0018, "double"));
    config.registerParameter(ParameterInfo("stdp_time_constant", "STDP time constant", ParameterCategory::Plasticity, 10.0, "double"));
    config.registerParameter(ParameterInfo("plasticity_learning_rate", "Plasticity learning rate", ParameterCategory::Plasticity, 0.005, "double"));
    config.registerParameter(ParameterInfo("development_synaptogenesis_rate", "Synaptogenesis rate", ParameterCategory::Development, 0.01, "double"));
    config.registerParameter(ParameterInfo("development_pruning_rate", "Pruning rate", ParameterCategory::Development, 0.001, "double"));
    config.registerParameter(ParameterInfo("max_simulation_steps", "Maximum simulation steps", ParameterCategory::Simulation, 100000, "int"));
    config.registerParameter(ParameterInfo("visualization_enabled", "Enable visualization", ParameterCategory::Debug, true, "bool"));
    config.registerParameter(ParameterInfo("visualization_update_rate", "Visualization update rate (Hz)", ParameterCategory::Debug, 60.0, "double"));
    
    // Set preset values
    config.set("simulation_timestep", 0.0001);
    config.set("neuron_count", 2000);
    config.set("region_count", 2);
    config.set("connection_probability", 0.05);
    config.set("stdp_ltp_weight", 0.002);
    config.set("stdp_ltd_weight", 0.0018);
    config.set("stdp_time_constant", 10.0);
    config.set("plasticity_learning_rate", 0.005);
    config.set("development_synaptogenesis_rate", 0.01);
    config.set("development_pruning_rate", 0.001);
    config.set("max_simulation_steps", 100000);
    config.set("visualization_enabled", true);
    config.set("visualization_update_rate", 60.0);
    
    return config;
}

Config Config::createBalancedPreset() {
    Config config;
    
    // Balanced preset - general purpose
    config.registerParameter(ParameterInfo("simulation_timestep", "Simulation timestep in seconds", ParameterCategory::Simulation, 0.001, "double"));
    config.registerParameter(ParameterInfo("neuron_count", "Number of neurons in the brain", ParameterCategory::Neural, 500, "int"));
    config.registerParameter(ParameterInfo("region_count", "Number of brain regions", ParameterCategory::Neural, 1, "int"));
    config.registerParameter(ParameterInfo("connection_probability", "Connection probability for random connectivity", ParameterCategory::Neural, 0.15, "double"));
    config.registerParameter(ParameterInfo("stdp_ltp_weight", "STDP Long-Term Potentiation weight", ParameterCategory::Plasticity, 0.01, "double"));
    config.registerParameter(ParameterInfo("stdp_ltd_weight", "STDP Long-Term Depression weight", ParameterCategory::Plasticity, 0.01, "double"));
    config.registerParameter(ParameterInfo("stdp_time_constant", "STDP time constant", ParameterCategory::Plasticity, 20.0, "double"));
    config.registerParameter(ParameterInfo("plasticity_learning_rate", "Plasticity learning rate", ParameterCategory::Plasticity, 0.01, "double"));
    config.registerParameter(ParameterInfo("development_synaptogenesis_rate", "Synaptogenesis rate", ParameterCategory::Development, 0.005, "double"));
    config.registerParameter(ParameterInfo("development_pruning_rate", "Pruning rate", ParameterCategory::Development, 0.0005, "double"));
    config.registerParameter(ParameterInfo("max_simulation_steps", "Maximum simulation steps", ParameterCategory::Simulation, 50000, "int"));
    config.registerParameter(ParameterInfo("log_level", "Logging level", ParameterCategory::Performance, "INFO", "string", {"DEBUG", "INFO", "WARN", "ERROR"}));
    
    // Set preset values
    config.set("simulation_timestep", 0.001);
    config.set("neuron_count", 500);
    config.set("region_count", 1);
    config.set("connection_probability", 0.15);
    config.set("stdp_ltp_weight", 0.01);
    config.set("stdp_ltd_weight", 0.01);
    config.set("stdp_time_constant", 20.0);
    config.set("plasticity_learning_rate", 0.01);
    config.set("development_synaptogenesis_rate", 0.005);
    config.set("development_pruning_rate", 0.0005);
    config.set("max_simulation_steps", 50000);
    config.set("log_level", "INFO");
    
    return config;
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
