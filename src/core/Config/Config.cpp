#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <iomanip>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string filepath;
    bool isDirty;
    
    Impl() : isDirty(false) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()), schema() {
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)), schema(std::move(other.schema)) {
}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        schema = std::move(other.schema);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Update file path
        pImpl->filepath = filepath;
        
        // Try different file formats
        if (parseJsonFile(filepath, pImpl->entries) ||
            parseYamlFile(filepath, pImpl->entries) ||
            parseSimpleFile(filepath, pImpl->entries)) {
            pImpl->isDirty = false;
            return true;
        }
        
        NLM_LOG_ERROR("Failed to parse configuration file: " + filepath);
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        auto parsedArgs = parseCommandLineArgs(argc, argv);
        for (const auto& [key, value] : parsedArgs) {
            set(key, value, ConfigSource::CommandLine, true);
        }
        pImpl->isDirty = true;
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing command line arguments: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open configuration file for writing: " + filepath);
            return false;
        }
        
        file << "# NLM Configuration File\n";
        file << "# Generated on " << getCurrentTimeString() << "\n";
        file << "# DO NOT EDIT MANUALLY - Use command-line arguments or API instead\n\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << "#   Type: " << entry.getTypeString() << "\n";
            file << "#   Source: " << entry.getSourceString() << "\n";
            file << "#   Last validated: " << (entry.isValidated ? "Yes" : "No") << "\n";
            file << entry.key << " = " << configValueToString(entry.value) << "\n\n";
        }
        
        pImpl->isDirty = false;
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration file " + filepath + ": " + std::string(e.what()));
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
        // Validate type against schema if available
        if (!schema.typeDefinitions.empty()) {
            auto validation = schema.validate(key, it->value);
            if (!validation.isValid) {
                for (const auto& error : validation.errors) {
                    NLM_LOG_WARNING("Configuration type warning for " + key + ": " + error);
                }
            }
        }
        
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARNING("Type mismatch for configuration key " + key + 
                       ": expected different type");
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, bool validate) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigValue oldValue;
    ConfigSource oldSource;
    bool wasValidated = false;
    
    if (it != pImpl->entries.end()) {
        oldValue = it->value;
        oldSource = it->source;
        wasValidated = it->isValidated;
        
        // Update existing entry
        it->value = value;
        it->source = source;
        it->timestamp = std::chrono::system_clock::now();
        it->isValidated = false;
    } else {
        // Add new entry
        pImpl->entries.emplace_back(key, value, source);
        ConfigEntry& newEntry = pImpl->entries.back();
        newEntry.timestamp = std::chrono::system_clock::now();
        newEntry.isValidated = false;
    }
    
    pImpl->isDirty = true;
    
    // Validate if requested and schema available
    if (validate && !schema.typeDefinitions.empty()) {
        auto validation = schema.validate(key, value);
        if (validation.isValid) {
            ConfigEntry& entry = pImpl->entries.back();
            entry.isValidated = true;
        } else {
            for (const auto& error : validation.errors) {
                NLM_LOG_ERROR("Configuration validation error for " + key + ": " + error);
            }
            for (const auto& warning : validation.warnings) {
                NLM_LOG_WARNING("Configuration validation warning for " + key + ": " + warning);
            }
        }
    }
    
    // Log significant changes
    if (oldValue != value || oldSource != source) {
        NLM_LOG_INFO("Configuration updated: " + key + " changed from " + 
                    configValueToString(oldValue) + " to " + configValueToString(value) +
                    " (source: " + getSourceString(source) + ")");
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, int value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, double value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, bool value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

bool Config::isValidated(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return (it != pImpl->entries.end()) && it->isValidated;
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    pImpl->isDirty = true;
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
    pImpl->isDirty = true;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "  Schema: " << (schema.typeDefinitions.empty() ? "None" : std::to_string(schema.typeDefinitions.size()) + " parameters defined") << "\n";
    oss << "  Dirty: " << (pImpl->isDirty ? "Yes" : "No") << "\n";
    oss << "  File: " << (pImpl->filepath.empty() ? "None" : pImpl->filepath) << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        
        oss << "  Source: " << entry.getSourceString() << "  ";
        oss << "Type: " << entry.getTypeString() << "  ";
        oss << "Validated: " << (entry.isValidated ? "Yes" : "No") << "\n";
    }
    
    return oss.str();
}

void Config::setSchema(const ConfigSchema& newSchema) {
    schema = newSchema;
    
    // Apply default values from schema for missing entries
    for (const auto& [key, defaultValue] : schema.defaultValues) {
        if (!has(key)) {
            set(key, defaultValue, ConfigSource::Default, false);
        }
    }
}

void Config::setBatch(const std::map<std::string, ConfigValue>& values) {
    for (const auto& [key, value] : values) {
        set(key, value, ConfigSource::Runtime, true);
    }
}

void Config::loadFromEnvironment(const std::map<std::string, std::string>& envMapping) {
    for (const auto& [configKey, envVar] : envMapping) {
        const char* value = std::getenv(envVar.c_str());
        if (value) {
            set(configKey, std::string(value), ConfigSource::Runtime, true);
        }
    }
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        if (has(entry.key)) {
            // Keep existing value unless other has higher priority source
            if (entry.source > source) {
                set(entry.key, entry.value, source, true);
            }
        } else {
            set(entry.key, entry.value, source, true);
        }
    }
}

ValidationResult Config::validateAll() const {
    ValidationResult result;
    
    if (schema.typeDefinitions.empty()) {
        result.addWarning("No schema defined - skipping validation");
        return result;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto validation = schema.validate(entry.key, entry.value);
        if (!validation.isValid) {
            result.errors.insert(result.errors.end(), validation.errors.begin(), validation.errors.end());
        }
        result.warnings.insert(result.warnings.end(), validation.warnings.begin(), validation.warnings.end());
    }
    
    return result;
}

std::vector<std::pair<std::string, ConfigSource>> Config::getBySource(ConfigSource source) const {
    std::vector<std::pair<std::string, ConfigSource>> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.emplace_back(entry.key, entry.source);
        }
    }
    return result;
}

Config Config::createPhase2Config() {
    Config config;
    
    // Core simulation parameters
    config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    config.set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config.set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config.set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config.set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config.set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config.set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Development parameters
    config.set("development_enabled", true, ConfigSource::Default);
    config.set("critical_period_start", 1000.0, ConfigSource::Default);
    config.set("critical_period_duration", 5000.0, ConfigSource::Default);
    
    // Memory parameters
    config.set("working_memory_capacity", static_cast<size_t>(100), ConfigSource::Default);
    config.set("episodic_memory_max_episodes", static_cast<size_t>(1000), ConfigSource::Default);
    
    // Neuromodulation parameters
    config.set("dopamine_baseline", 0.1f, ConfigSource::Default);
    config.set("curiosity_weight", 0.5f, ConfigSource::Default);
    config.set("novelty_threshold", 0.3f, ConfigSource::Default);
    
    return config;
}

Config Config::createPhase6Config() {
    Config config;
    
    // Phase 6 specific parameters
    config.set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
    config.set("connection_probability", 0.08f, ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Enhanced memory systems
    config.set("working_memory_capacity", static_cast<size_t>(500), ConfigSource::Default);
    config.set("episodic_memory_max_episodes", static_cast<size_t>(10000), ConfigSource::Default);
    config.set("associative_memory_pattern_count", static_cast<size_t>(5000), ConfigSource::Default);
    
    // Advanced neuromodulation
    config.set("dopamine_baseline", 0.15f, ConfigSource::Default);
    config.set("dopamine_sensitivity", 1.2f, ConfigSource::Default);
    config.set("curiosity_weight", 0.8f, ConfigSource::Default);
    config.set("novelty_threshold", 0.4f, ConfigSource::Default);
    
    // Development and plasticity
    config.set("development_enabled", true, ConfigSource::Default);
    config.set("critical_period_start", 500.0, ConfigSource::Default);
    config.set("critical_period_duration", 8000.0, ConfigSource::Default);
    config.set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
    config.set("pruning_rate", 0.00002f, ConfigSource::Default);
    
    // Prediction and cognition
    config.set("prediction_confidence_threshold", 0.7f, ConfigSource::Default);
    config.set("planning_depth", 8, ConfigSource::Default);
    config.set("attention_excitation_strength", 2.0f, ConfigSource::Default);
    config.set("attention_inhibition_strength", 0.8f, ConfigSource::Default);
    
    return config;
}

Config Config::createMinimalConfig() {
    Config config;
    
    config.set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config.set("simulation_timestep", 0.01, ConfigSource::Default);
    
    return config;
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

bool Config::parseJsonFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            // Convert JSON value to ConfigValue
            ConfigValue value;
            const auto& jsonValue = *it;
            
            if (jsonValue.is_string()) {
                value = jsonValue.get<std::string>();
            } else if (jsonValue.is_number_integer()) {
                value = jsonValue.get<int64_t>();
            } else if (jsonValue.is_number_unsigned()) {
                value = jsonValue.get<uint64_t>();
            } else if (jsonValue.is_number_float()) {
                value = jsonValue.get<double>();
            } else if (jsonValue.is_boolean()) {
                value = jsonValue.get<bool>();
            } else if (jsonValue.is_array()) {
                // Determine array type
                if (!jsonValue.empty()) {
                    if (jsonValue[0].is_string()) {
                        std::vector<std::string> strList = jsonValue.get<std::vector<std::string>>();
                        value = strList;
                    } else if (jsonValue[0].is_number_integer()) {
                        std::vector<int64_t> intList = jsonValue.get<std::vector<int64_t>>();
                        value = intList;
                    } else if (jsonValue[0].is_number_float()) {
                        std::vector<double> doubleList = jsonValue.get<std::vector<double>>();
                        value = doubleList;
                    } else {
                        continue; // Skip unsupported array types
                    }
                }
            }
            
            entries.emplace_back(key, value, ConfigSource::File, "Loaded from JSON file");
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

bool Config::parseYamlFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    // TODO: Implement YAML parsing when YAML library is available
    NLM_LOG_WARNING("YAML parsing not yet implemented - falling back to simple parser");
    return parseSimpleFile(filepath, entries);
}

bool Config::parseSimpleFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
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
            
            set(key, value, ConfigSource::File, false);
        } else {
            NLM_LOG_WARNING(std::string("Invalid line in config file ") + filepath + ": " + 
                           std::to_string(lineNumber) + ": " + line);
        }
    }
    
    return true;
}

ConfigValue Config::stringToConfigValue(const std::string& str) {
    // Try to parse as boolean
    if (str == "true" || str == "false") {
        return str == "true";
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int64_t intVal = std::stoll(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Return as string
    return str;
}

std::string Config::configValueToString(const ConfigValue& value) {
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

std::map<std::string, std::string> Config::parseCommandLineArgs(int argc, char** argv) {
    std::map<std::string, std::string> result;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                result[key] = value;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            result[key] = value;
        }
        // Handle standalone flags (boolean values)
        else if (arg[0] == '-') {
            std::string key = arg.substr(1);
            result[key] = "true";
        }
    }
    
    return result;
}

std::map<std::string, ConfigValue> Config::parseConfigFile(const std::string& filepath) {
    std::map<std::string, ConfigValue> result;
    parseSimpleFile(filepath, result); // This will need to be adapted
    return result;
}

std::string Config::getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
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
