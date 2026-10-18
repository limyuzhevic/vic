#include "Config.hpp"
#include "StringUtils.cpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <iomanip>
#include <chrono>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigSchemaEntry> schema;
    std::map<std::string, std::string> environmentVariables;
    std::shared_ptr<ConfigSchema> configSchema;
    
    // Validation and error handling
    std::vector<std::string> validationErrors;
    bool validationEnabled;
    
    Impl() : validationEnabled(true) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default schema
    initializeDefaultSchema();
    
    // Create default schema
    pImpl->configSchema = std::make_shared<ConfigSchema>();
    initializeDefaultSchema(pImpl->configSchema.get());
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultSchema() {
    // Standard NLM configuration parameters with validation rules
    pImpl->schema = {
        {"random_seed", "Random seed for reproducible simulation", false, ConfigSource::Default},
        {"simulation_timestep", "Simulation timestep in seconds", true, ConfigSource::Default},
        {"neuron_count", "Total number of neurons in the simulation", true, ConfigSource::Default},
        {"region_count", "Number of neural regions", true, ConfigSource::Default},
        {"connection_probability", "Probability of synapse formation", true, ConfigSource::Default},
        {"stdp_ltp_weight", "STDP LTP weight factor", true, ConfigSource::Default},
        {"stdp_ltd_weight", "STDP LTD weight factor", true, ConfigSource::Default},
        {"stdp_tau", "STDP time constant", true, ConfigSource::Default},
        {"synaptogenesis_rate", "Structural plasticity synaptogenesis rate", true, ConfigSource::Default},
        {"pruning_rate", "Structural plasticity pruning rate", true, ConfigSource::Default},
        {"max_spikes_per_step", "Maximum spikes allowed per simulation step", false, ConfigSource::Default},
        {"enable_visualization", "Enable visualization output", false, ConfigSource::Default},
        {"checkpoint_path", "Path for checkpoint save/load", false, ConfigSource::Default},
        {"enable_reward_modulation", "Enable reward-based neuromodulation", false, ConfigSource::Default},
        {"curiosity_threshold", "Novelty threshold for curiosity-driven exploration", false, ConfigSource::Default},
        {"novelty_threshold", "Novelty threshold for learning", false, ConfigSource::Default},
        {"temperature", "Temperature parameter for stochastic processes", false, ConfigSource::Default},
        {"learning_rate", "Base learning rate for plasticity rules", false, ConfigSource::Default},
    };
}

void Config::initializeDefaultSchema(ConfigSchema* schema) {
    // Add validation rules for important parameters
    schema->addEntry(ConfigSchemaEntry("simulation_timestep", 
                                     "Simulation timestep in seconds (must be positive)",
                                     true, ConfigSource::Default, "float",
                                     "range=0.000001,1.0"));
    
    schema->addEntry(ConfigSchemaEntry("neuron_count",
                                     "Total number of neurons (must be positive and reasonable)",
                                     true, ConfigSource::Default, "int",
                                     "range=1,100000"));
    
    schema->addEntry(ConfigSchemaEntry("region_count",
                                     "Number of neural regions (must be 1 or more)",
                                     true, ConfigSource::Default, "int",
                                     "min=1"));
    
    schema->addEntry(ConfigSchemaEntry("connection_probability",
                                     "Probability of synapse formation (0.0 to 1.0)",
                                     true, ConfigSource::Default, "float",
                                     "range=0.0,1.0"));
    
    schema->addEntry(ConfigSchemaEntry("random_seed",
                                     "Random seed for reproducible simulation",
                                     false, ConfigSource::Default, "int",
                                     "min=0"));
    
    schema->addEntry(ConfigSchemaEntry("stdp_ltp_weight",
                                     "STDP LTP weight factor",
                                     true, ConfigSource::Default, "float",
                                     "range=-1.0,1.0"));
    
    schema->addEntry(ConfigSchemaEntry("stdp_ltd_weight",
                                     "STDP LTD weight factor",
                                     true, ConfigSource::Default, "float",
                                     "range=-1.0,1.0"));
    
    schema->addEntry(ConfigSchemaEntry("stdp_tau",
                                     "STDP time constant (positive)",
                                     true, ConfigSource::Default, "float",
                                     "min=0.1"));
    
    schema->addEntry(ConfigSchemaEntry("synaptogenesis_rate",
                                     "Structural plasticity synaptogenesis rate (positive)",
                                     true, ConfigSource::Default, "float",
                                     "min=0.0"));
    
    schema->addEntry(ConfigSchemaEntry("pruning_rate",
                                     "Structural plasticity pruning rate (positive)",
                                     true, ConfigSource::Default, "float",
                                     "min=0.0"));
}

// String parsing helpers
namespace {
    std::string configValueToString(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }
    
    // Simple JSON-like string parsing for file format
    bool parseSimpleConfigLine(const std::string& line, std::string& key, std::string& value) {
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            return false;
        }
        
        key = StringUtils::trim(line.substr(0, pos));
        value = StringUtils::trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        return !key.empty();
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARN("Configuration file not found: " + filepath);
            return false;
        }
        
        std::string line;
        int lineNum = 0;
        bool hasValidConfig = false;
        
        while (std::getline(file, line)) {
            lineNum++;
            
            // Skip empty lines and comments
            line = StringUtils::trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            std::string key, value;
            if (parseSimpleConfigLine(line, key, value)) {
                try {
                    if (!setFromString(key, value, ConfigSource::File)) {
                        NLM_LOG_WARN("Skipping invalid configuration line " + std::to_string(lineNum) + ": " + line);
                    } else {
                        hasValidConfig = true;
                    }
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error parsing configuration line " + std::to_string(lineNum) + ": " + e.what() + " - Line: " + line);
                }
            } else {
                NLM_LOG_WARN("Skipping malformed configuration line " + std::to_string(lineNum) + ": " + line);
            }
        }
        
        if (hasValidConfig) {
            NLM_LOG_INFO("Loaded configuration from file: " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
            return true;
        } else {
            NLM_LOG_WARN("No valid configuration found in file: " + filepath);
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration file '" + filepath + "': " + e.what());
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (!setFromString(key, value, ConfigSource::CommandLine)) {
                    NLM_LOG_ERROR("Invalid command line argument: " + arg);
                    success = false;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (!setFromString(key, value, ConfigSource::CommandLine)) {
                NLM_LOG_ERROR("Invalid command line argument: " + arg + " " + value);
                success = false;
            }
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open configuration file for writing: " + filepath);
            return false;
        }
        
        // Add metadata
        file << "# NLM Configuration File\n";
        file << "# Generated by NLM Configuration System v2.0\n";
        file << "# Format: key = value\n\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << configValueToString(entry.value) << "\n\n";
        }
        
        NLM_LOG_INFO("Saved configuration to file: " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration file '" + filepath + "': " + e.what());
        return false;
    }
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
        NLM_LOG_WARN("Type mismatch for configuration key '" + key + "' - expected " + typeid(T).name());
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate configuration key
    if (!isValidKey(key)) {
        throw ConfigException("Invalid configuration key: '" + key + "'");
    }
    
    // Validate value type for key
    if (!validateValue(key, value)) {
        throw ConfigValidationError(key, "Valid value type", typeid(ConfigValue).name());
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
    
    // Group by source for better readability
    std::map<ConfigSource, std::vector<const ConfigEntry*>> grouped;
    for (const auto& entry : pImpl->entries) {
        grouped[entry.source].push_back(&entry);
    }
    
    for (const auto& [source, entries] : grouped) {
        oss << "  From " << static_cast<int>(source) << ":\n";
        for (const auto* entry : entries) {
            oss << "    " << entry->key << " = [" << configValueToString(entry->value) << "]\n";
        }
    }
    
    return oss.str();
}

bool Config::isValidKey(const std::string& key) const {
    return pImpl->schema.find(key) != pImpl->schema.end() ||
           key.find('/') == std::string::npos; // Allow non-schema keys (extensions)
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    if (pImpl->schema.find(key) == pImpl->schema.end()) {
        return true; // Unknown key, allow any value
    }
    
    const auto& schemaEntry = pImpl->schema.at(key);
    
    // Check type compatibility
    try {
        if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
            return true; // numeric
        } else if (std::holds_alternative<double>(value)) {
            return true; // floating point
        } else if (std::holds_alternative<bool>(value)) {
            return true; // boolean
        } else if (std::holds_alternative<std::string>(value)) {
            return true; // string
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            return true; // int array
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            return true; // double array
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            return true; // string array
        }
    } catch (...) {
        return false;
    }
    
    return false;
}

bool Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    try {
        if (valueStr.empty()) {
            set(key, std::string(""), source);
            return true;
        }
        
        // Try to parse boolean
        if (valueStr == "true" || valueStr == "false") {
            bool val = (valueStr == "true");
            set(key, val, source);
            return true;
        }
        
        // Try to parse integer
        try {
            size_t pos;
            int64_t intVal = std::stoll(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, intVal, source);
                return true;
            }
        } catch (...) {}
        
        // Try to parse floating point
        try {
            size_t pos;
            double doubleVal = std::stod(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, doubleVal, source);
                return true;
            }
        } catch (...) {}
        
        // Fall back to string
        set(key, valueStr, source);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse configuration value '" + valueStr + "' for key '" + key + "': " + e.what());
        return false;
    }
}

void Config::loadFromEnvironment() {
    for (const auto& [key, schemaEntry] : pImpl->schema) {
        const char* envValue = std::getenv(key.c_str());
        if (envValue != nullptr) {
            std::string envKey = "env_" + key; // Prefix to distinguish from command line
            if (setFromString(envKey, envValue, ConfigSource::Environment)) {
                NLM_LOG_INFO("Loaded environment variable: " + key + " = " + envValue);
            }
        }
    }
}

std::string Config::generateConfigSchema() const {
    // Simple schema representation as JSON-like string
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"type\": \"object\",\n";
    oss << "  \"properties\": {\n";
    
    bool firstProp = true;
    for (const auto& [key, schemaEntry] : pImpl->schema) {
        if (!firstProp) oss << ",\n";
        firstProp = false;
        
        oss << "    \"" << key << "\": {\n";
        oss << "      \"type\": \"string\",\n";
        oss << "      \"description\": \"" << schemaEntry.description << "\",\n";
        oss << "      \"required\": " << (schemaEntry.required ? "true" : "false") << ",\n";
        oss << "      \"default_source\": " << static_cast<int>(schemaEntry.defaultSource);
        oss << "\n    }";
    }
    
    oss << "\n  },\n";
    oss << "  \"required\": [\n";
    
    firstProp = true;
    for (const auto& [key, schemaEntry] : pImpl->schema) {
        if (schemaEntry.required) {
            if (!firstProp) oss << ",\n";
            firstProp = false;
            oss << "    \"" << key << "\"";
        }
    }
    
    oss << "\n  ]\n";
    oss << "}\n";
    
    return oss.str();
}

bool Config::validate() const {
    if (!pImpl->configSchema) {
        return true; // No schema, validation passes
    }
    
    return pImpl->configSchema->validate(*this);
}

std::vector<std::string> Config::validateWithErrors() const {
    if (!pImpl->configSchema) {
        return {}; // No schema, no validation errors
    }
    
    return pImpl->configSchema->validateWithErrors(*this);
}

void Config::setSchema(std::shared_ptr<ConfigSchema> schema) {
    pImpl->configSchema = schema;
}

bool Config::hasSchema() const {
    return pImpl->configSchema != nullptr;
}

void Config::enableValidation(bool enable) {
    pImpl->validationEnabled = enable;
}

bool Config::isValidationEnabled() const {
    return pImpl->validationEnabled;
}

void Config::clearValidationErrors() {
    pImpl->validationErrors.clear();
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

bool Config::hasValidationErrors() const {
    return !pImpl->validationErrors.empty();
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

// ConfigValue getter implementation
ConfigValue Config::getValue(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        throw ConfigException("Configuration key not found: '" + key + "'");
    }
    
    return it->value;
}

} // namespace nlm