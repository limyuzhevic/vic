#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <cmath>
#include <unordered_set>
#include <numeric>
#include <regex>
#include <map>

using namespace nlm;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigSchema> schemas;
    std::vector<ConfigChangeCallback> changeCallbacks;
    ConfigStats stats;
    std::unordered_map<std::string, std::string> environmentCache;
    bool prettyPrintJson = false;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeDefaultSchemas();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultSchemas() {
    // Add common configuration schemas for NLM
    
    // Core configuration
    addSchema(ConfigSchema("neuron_count", "Number of neurons in the brain", true, "1000", true, "int", "", {}, 1, 100000));
    
    addSchema(ConfigSchema("region_count", "Number of neural regions", true, "1", true, "int", "", {}, 1, 100));
    
    addSchema(ConfigSchema("connection_probability", "Probability of synaptic connections", true, "0.1", false, "float", "", {}, 0.0, 1.0));
    
    // Performance and behavior
    addSchema(ConfigSchema("random_seed", "Random seed for reproducibility", false, "42", false, "int64", "", {}, 0, std::numeric_limits<int64_t>::max()));
    
    addSchema(ConfigSchema("simulation_timestep", "Simulation time step in seconds", false, "0.001", false, "float", "", {}, 1e-6, 1.0));
    
    // Plasticity parameters
    addSchema(ConfigSchema("stdp_ltp_weight", "STDP long-term potentiation weight", false, "0.01", false, "float", "", {}, 0.0, 1.0));
    
    addSchema(ConfigSchema("stdp_ltd_weight", "STDP long-term depression weight", false, "0.012", false, "float", "", {}, 0.0, 1.0));
    
    // Structural plasticity
    addSchema(ConfigSchema("synaptogenesis_rate", "Rate of new synapse formation", false, "0.0001", false, "float", "", {}, 0.0, 0.01));
    
    addSchema(ConfigSchema("pruning_rate", "Rate of synapse elimination", false, "0.00001", false, "float", "", {}, 0.0, 0.01));
    
    // Development parameters
    addSchema(ConfigSchema("development_stage", "Current developmental stage", false, "1", false, "int", "", {"0", "1", "2", "3", "4"}, 0, 4));
    
    // Memory systems
    addSchema(ConfigSchema("working_memory_capacity", "Capacity of working memory", false, "100", false, "int", "", {}, 10, 10000));
    
    // Neuromodulation
    addSchema(ConfigSchema("dopamine_level", "Dopamine neuromodulator level", false, "0.5", false, "float", "", {}, 0.0, 1.0));
    
    // Environment parameters
    addSchema(ConfigSchema("world_width", "Width of the simulated world", false, "16", false, "int", "", {}, 10, 1000));
    
    addSchema(ConfigSchema("world_height", "Height of the simulated world", false, "16", false, "int", "", {}, 10, 1000));
    
    // Checkpointing
    addSchema(ConfigSchema("checkpoint_dir", "Directory for checkpoint files", false, "./checkpoints", false, "string", "", {}, 0, std::numeric_limits<double>::max()));
    
    // Experiment controls
    addSchema(ConfigSchema("enable_reward_modulation", "Enable reward-based learning", false, "true", false, "bool", "", {"true", "false"}, 0.0, 1.0));
    
    addSchema(ConfigSchema("enable_structural_plasticity", "Enable structural plasticity", false, "true", false, "bool", "", {"true", "false"}, 0.0, 1.0));
    
    addSchema(ConfigSchema("enable_development", "Enable developmental processes", false, "true", false, "bool", "", {"true", "false"}, 0.0, 1.0));
    
    addSchema(ConfigSchema("enable_curiosity", "Enable curiosity-driven exploration", false, "true", false, "bool", "", {"true", "false"}, 0.0, 1.0));
    
    // Advanced options
    addSchema(ConfigSchema("log_level", "Logging level", false, "INFO", false, "string", "", {"DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"}, 0.0, 1.0));
}

bool Config::loadFromFile(const std::string& filepath, bool validate) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as simple key=value format
    std::string line;
    int lineNumber = 0;
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
            
            // Try to convert to appropriate type
            ConfigValue configValue = stringToConfigValue(value);
            
            // Validate if requested
            if (validate) {
                ValidationResult validation = validateAgainstSchema(key, configValue);
                if (validation.hasErrors()) {
                    NLM_LOG_WARNING("Configuration validation failed for key '" + key + "' at line " + 
                                  std::to_string(lineNumber) + ": " + validation.messages[0].second);
                }
            }
            
            set(key, configValue, ConfigSource::File, validate);
        }
    }
    
    // Update statistics
    pImpl->stats.fileEntries = pImpl->entries.size();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    pImpl->stats.validationTime = std::chrono::duration<double>(endTime - startTime).count();
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv, bool validate) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                ConfigValue configValue = stringToConfigValue(value);
                
                if (validate) {
                    ValidationResult validation = validateAgainstSchema(key, configValue);
                    if (validation.hasErrors()) {
                        NLM_LOG_ERROR("Command line validation failed for key '" + key + "': " + 
                                    validation.messages[0].second);
                        return false;
                    }
                }
                
                set(key, configValue, ConfigSource::CommandLine, validate);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            ConfigValue configValue = stringToConfigValue(value);
            
            if (validate) {
                ValidationResult validation = validateAgainstSchema(key, configValue);
                if (validation.hasErrors()) {
                    NLM_LOG_ERROR("Command line validation failed for key '" + key + "': " + 
                                validation.messages[0].second);
                    return false;
                }
            }
            
            set(key, configValue, ConfigSource::CommandLine, validate);
        }
    }
    
    pImpl->stats.commandLineEntries = pImpl->entries.size();
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool prettyPrint) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (prettyPrint || pImpl->prettyPrintJson) {
        // Simple JSON-like output
        file << "{\n";
        bool first = true;
        for (const auto& entry : pImpl->entries) {
            if (!first) file << ",\n";
            file << "  \"" << entry.key << "\": " << configValueToJson(entry.value);
            first = false;
        }
        file << "\n}\n";
    } else {
        // Output as simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << configValueToString(entry.value) << "\n";
            file << "\n";
        }
    }
    
    return true;
}

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFile(filepath, true);
}

bool Config::loadFromArgs(int argc, char** argv) {
    return loadFromArgs(argc, argv, true);
}

bool Config::saveToFile(const std::string& filepath) const {
    return saveToFile(filepath, false);
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

void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source, bool validate) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Update existing entry
        ConfigSource oldSource = it->source;
        it->value = value;
        it->source = source;
        
        // Notify callbacks of change
        notifyChange(key, value, source);
        
    } else {
        // Create new entry
        ConfigEntry newEntry;
        newEntry.key = key;
        newEntry.value = value;
        newEntry.source = source;
        
        // Get description from schema if available
        auto schemaIt = pImpl->schemas.find(key);
        if (schemaIt != pImpl->schemas.end()) {
            newEntry.description = schemaIt->second.description;
        }
        
        pImpl->entries.push_back(newEntry);
        
        // Notify callbacks of change
        notifyChange(key, value, source);
        
        // Update statistics
        pImpl->stats.totalEntries++;
        if (source == ConfigSource::Default) pImpl->stats.defaultEntries++;
        else if (source == ConfigSource::File) pImpl->stats.fileEntries++;
        else if (source == ConfigSource::CommandLine) pImpl->stats.commandLineEntries++;
        else if (source == ConfigSource::Runtime) pImpl->stats.runtimeEntries++;
    }
}

void Config::set(const std::string& key, const std::string& value, 
                 ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, int value, 
                 ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, int64_t value, 
                 ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, double value, 
                 ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, bool value, 
                 ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

template<typename T>
void Config::setTyped(const std::string& key, const T& value, 
                      ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
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
    clear(ConfigSource::Runtime);
}

void Config::clear(ConfigSource source) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [source](const ConfigEntry& e) { return e.source == source; }),
        pImpl->entries.end()
    );
}

std::string Config::summary() const {
    return summary(true);
}

std::string Config::summary(bool includeValidation) const {
    std::ostringstream oss;
    oss << "Configuration Statistics:" << std::endl;
    oss << "  Total Entries: " << pImpl->entries.size() << std::endl;
    oss << "  Default Entries: " << pImpl->stats.defaultEntries << std::endl;
    oss << "  File Entries: " << pImpl->stats.fileEntries << std::endl;
    oss << "  Command Line Entries: " << pImpl->stats.commandLineEntries << std::endl;
    oss << "  Runtime Entries: " << pImpl->stats.runtimeEntries << std::endl;
    oss << "  Memory Usage: " << pImpl->stats.memoryUsage << " KB" << std::endl;
    oss << "  Validation Time: " << pImpl->stats.validationTime << " seconds" << std::endl;
    
    if (includeValidation && !pImpl->schemas.empty()) {
        oss << std::endl << "Schema Validation:" << std::endl;
        ValidationResult validation = validateSchema();
        if (validation.hasErrors()) {
            oss << "  Errors: " << validation.messages.size() << std::endl;
            for (const auto& msg : validation.messages) {
                if (msg.first >= ValidationSeverity::Error) {
                    oss << "    [ERROR] " << msg.second << std::endl;
                }
            }
        }
        if (validation.hasWarnings()) {
            oss << "  Warnings: " << validation.messages.size() << std::endl;
            for (const auto& msg : validation.messages) {
                if (msg.first == ValidationSeverity::Warning) {
                    oss << "    [WARNING] " << msg.second << std::endl;
                }
            }
        }
    }
    
    oss << std::endl << "Configuration Entries:" << std::endl;
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
        oss << "] (" << static_cast<int>(entry.source) << ")" << std::endl;
    }
    
    return oss.str();
}

void Config::addSchema(const ConfigSchema& schema) {
    pImpl->schemas[schema.key] = schema;
}

bool Config::validateSchema() const {
    ValidationResult result;
    
    for (const auto& schema : pImpl->schemas) {
        if (!has(schema.first)) {
            result.messages.push_back({ValidationSeverity::Error, "Required key missing: " + schema.first});
        }
    }
    
    return !result.hasErrors();
}

ValidationResult Config::validateConfiguration() const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult validation = validateAgainstSchema(entry.key, entry.value);
        if (validation.hasErrors()) {
            result.messages.insert(result.messages.end(), validation.messages.begin(), validation.messages.end());
        }
    }
    
    return result;
}

ValidationResult Config::validateAgainstSchema(const std::string& key, const ConfigValue& value) const {
    ValidationResult result;
    
    auto schemaIt = pImpl->schemas.find(key);
    if (schemaIt == pImpl->schemas.end()) {
        result.messages.push_back({ValidationSeverity::Info, "No schema validation for key: " + key});
        return result;
    }
    
    const ConfigSchema& schema = schemaIt->second;
    
    // Validate type if specified
    if (!schema.type.empty() && schema.type != "auto") {
        bool typeValid = validateType(key, value, schema.type);
        if (!typeValid) {
            result.messages.push_back({ValidationSeverity::Error, "Type validation failed for key '" + key + "': expected " + schema.type});
        }
    }
    
    // Validate range if specified
    if (!schema.allowedValues.empty()) {
        bool valuesValid = validateAllowedValues(key, value, schema.allowedValues);
        if (!valuesValid) {
            result.messages.push_back({ValidationSeverity::Error, "Value validation failed for key '" + key + "': not in allowed values"});
        }
    }
    
    return result;
}

void Config::addChangeCallback(const ConfigChangeCallback& callback) {
    pImpl->changeCallbacks.push_back(callback);
}

void Config::removeChangeCallback(const ConfigChangeCallback& callback) {
    pImpl->changeCallbacks.erase(
        std::remove(pImpl->changeCallbacks.begin(), pImpl->changeCallbacks.end(), callback),
        pImpl->changeCallbacks.end()
    );
}

void Config::notifyChange(const std::string& key, const ConfigValue& value, ConfigSource source) {
    for (const auto& callback : pImpl->changeCallbacks) {
        try {
            callback(key, value, source);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error in configuration change callback: " + std::string(e.what()));
        }
    }
}

ConfigValue Config::stringToConfigValue(const std::string& str) {
    if (str == "true" || str == "false") {
        return str == "true";
    }
    
    try {
        if (str.find('.') == std::string::npos) {
            size_t pos;
            int64_t val = std::stoll(str, &pos);
            if (pos == str.size()) {
                return val;
            }
        }
    } catch (...) {}
    
    try {
        size_t pos;
        double val = std::stod(str, &pos);
        if (pos == str.size()) {
            return val;
        }
    } catch (...) {}
    
    return str;
}

std::string Config::configValueToJson(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        std::ostringstream oss;
        oss << std::get<double>(value);
        return oss.str();
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return "\"" + std::get<std::string>(value) + "\"";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& arr = std::get<std::vector<int>>(value);
        for (size_t i = 0; i < arr.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << arr[i];
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& arr = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < arr.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << arr[i];
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& arr = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < arr.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << arr[i] << "\"";
        }
        oss << "]";
        return oss.str();
    }
    
    return "null";
}

std::string Config::configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        oss << arg;
    }, value);
    return oss.str();
}

bool Config::validateType(const std::string& key, const ConfigValue& value, const std::string& type) const {
    if (type == "int") {
        return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    } else if (type == "float" || type == "double") {
        return std::holds_alternative<double>(value);
    } else if (type == "bool") {
        return std::holds_alternative<bool>(value);
    } else if (type == "string") {
        return std::holds_alternative<std::string>(value);
    } else if (type == "int64") {
        return std::holds_alternative<int64_t>(value);
    } else if (type == "auto") {
        return true;
    }
    
    return false;
}

bool Config::validateAllowedValues(const std::string& key, const ConfigValue& value, 
                                  const std::vector<std::string>& allowed) const {
    if (allowed.empty()) return true;
    
    std::string strValue = configValueToString(value);
    return std::find(allowed.begin(), allowed.end(), strValue) != allowed.end();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Explicit template instantiations
// ... (keeping existing instantiations from original file)

} // namespace nlm

// JSON library include
#include <nlohmann/json.hpp>