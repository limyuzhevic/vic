#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <regex>
#include <iostream>

namespace nlm {

// Simple JSON/YAML parser stub for now (would use proper libraries in production)
namespace {
    std::string detectFileFormat(const std::string& filepath) {
        if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
            return "json";
        } else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml") {
            return "yaml";
        } else if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yml") {
            return "yaml";
        }
        // Try to detect by content
        std::ifstream file(filepath);
        if (!file.is_open()) return "unknown";
        
        char firstChar;
        if (file.get(firstChar)) {
            if (firstChar == '{' || firstChar == '[') {
                return "json";
            }
            file.clear();
            file.seekg(0);
            std::string firstLine;
            if (std::getline(file, firstLine)) {
                std::string trimmed = trim(firstLine);
                if (trimmed.find("#") != std::string::npos) {
                    return "yaml";
                }
            }
        }
        return "unknown";
    }
    
    bool parseJsonFile(const std::string& filepath, json& configData) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            file >> configData;
            return true;
        } catch (const json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool parseSimpleFile(const std::string& filepath, json& configData) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        configData = json::object();
        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
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
                if (isNumber(value)) {
                    if (value.find('.') != std::string::npos) {
                        configData[key] = std::stod(value);
                    } else {
                        configData[key] = std::stoll(value);
                    }
                } else if (value == "true" || value == "false") {
                    configData[key] = (value == "true");
                } else {
                    configData[key] = value;
                }
            }
        }
        return true;
    }
    
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    bool isNumber(const std::string& str) {
        return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit) ||
               (str.size() > 1 && str.front() == '-' && std::all_of(str.begin() + 1, str.end(), ::isdigit)) ||
               (str.find('.') != std::string::npos && std::all_of(str.begin(), str.end(), 
                   [](char c) { return std::isdigit(c) || c == '.'; }));
    }
    
    // Convert ConfigValue to JSON
    json configValueToJson(const ConfigValue& value) {
        return std::visit([](auto&& v) -> json {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return v;
            } else if constexpr (std::is_same_v<T, int> ||
                                 std::is_same_v<T, int64_t> ||
                                 std::is_same_v<T, float> ||
                                 std::is_same_v<T, double>) {
                return static_cast<double>(v);
            } else if constexpr (std::is_same_v<T, bool>) {
                return v;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                return json(v);
            }
            return json();
        }, value);
    }
    
    // Convert JSON to ConfigValue
    ConfigValue jsonToConfigValue(const json& j) {
        if (j.is_string()) {
            return j.get<std::string>();
        } else if (j.is_number_integer()) {
            return j.get<int64_t>();
        } else if (j.is_number_unsigned()) {
            return j.get<uint64_t>();
        } else if (j.is_number_float()) {
            return j.get<double>();
        } else if (j.is_boolean()) {
            return j.get<bool>();
        } else if (j.is_array()) {
            // Check array element type
            if (!j.empty()) {
                if (j[0].is_number_integer()) {
                    std::vector<int> vec;
                    vec.reserve(j.size());
                    for (const auto& item : j) {
                        vec.push_back(item.get<int>());
                    }
                    return vec;
                } else if (j[0].is_number_float()) {
                    std::vector<double> vec;
                    vec.reserve(j.size());
                    for (const auto& item : j) {
                        vec.push_back(item.get<double>());
                    }
                    return vec;
                } else if (j[0].is_string()) {
                    std::vector<std::string> vec;
                    vec.reserve(j.size());
                    for (const auto& item : j) {
                        vec.push_back(item.get<std::string>());
                    }
                    return vec;
                }
            }
            // Default to empty int vector
            return std::vector<int>();
        }
        return std::string();
    }
    
    // Validate configuration against schema
    bool validateAgainstSchema(const json& configData, const ConfigSchema& schema) {
        // Simple validation - in production, this would be more comprehensive
        for (const auto& [key, pattern] : schema.typePatterns) {
            if (configData.contains(key)) {
                const auto& value = configData[key];
                // Basic type validation based on pattern
                if (pattern == "int" && !value.is_number_integer()) {
                    return false;
                } else if (pattern == "float" && !value.is_number()) {
                    return false;
                } else if (pattern == "bool" && !value.is_boolean()) {
                    return false;
                } else if (pattern == "string" && !value.is_string()) {
                    return false;
                }
            } else if (schema.requiredKeys.count(key) && schema.requiredKeys.at(key)) {
                // Required key missing
                return false;
            }
        }
        return true;
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool validate) {
    std::string format = detectFileFormat(filepath);
    json configData;
    
    bool success = false;
    if (format == "json") {
        success = parseJsonFile(filepath, configData);
    } else {
        // Try simple format (key=value)
        success = parseSimpleFile(filepath, configData);
    }
    
    if (!success) {
        return false;
    }
    
    clear();
    
    // Convert JSON to Config entries
    for (const auto& [key, value] : configData.items()) {
        ConfigValue configValue = jsonToConfigValue(value);
        // Default to File source for file-loaded entries
        set(key, configValue, ConfigSource::File);
        
        // Try to extract description from comments or metadata
        // For now, we'll use a default description
        if (pImpl->entries.size() > 0) {
            pImpl->entries.back().description = "Loaded from " + filepath;
        }
    }
    
    if (validate) {
        validate();
    }
    
    return true;
}

bool Config::loadFromYamlFile(const std::string& filepath, bool validate) {
    // For now, fall back to simple file parsing since we don't have a full YAML parser
    // In production, this would use a proper YAML library like yaml-cpp
    std::cerr << "YAML support requires yaml-cpp library. Falling back to simple parsing." << std::endl;
    return loadFromFile(filepath, validate);
}

bool Config::loadFromJsonFile(const std::string& filepath, bool validate) {
    json configData;
    if (!parseJsonFile(filepath, configData)) {
        return false;
    }
    
    clear();
    
    for (const auto& [key, value] : configData.items()) {
        ConfigValue configValue = jsonToConfigValue(value);
        set(key, configValue, ConfigSource::File);
    }
    
    if (validate) {
        validate();
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv, bool validate) {
    // Handle --key=value format
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                setFromString(key + "=" + value);
            }
        }
        // Handle --key value format
        else if (arg.substr(0, 2) == "--") {
            // Could be just --help or --version, handle separately
            if (arg == "--help" || arg == "--version") {
                // Let the main program handle these
                continue;
            }
            // Look ahead for value
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string key = arg.substr(2);
                std::string value = argv[++i];
                setFromString(key + "=" + value);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            std::string key = arg.substr(1);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string value = argv[++i];
                setFromString(key + "=" + value);
            }
        }
    }
    
    if (validate) {
        validate();
    }
    
    return true;
}

bool Config::loadFromEnvironment(const std::string& prefix, bool validate) {
    clear();
    
    // Simple environment variable parsing
    // In production, this could be more sophisticated
    for (const auto& env : std::unordered_map<std::string, std::string>()) {
        // This would normally iterate over all environment variables
        // For now, it's a placeholder
    }
    
    if (validate) {
        validate();
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool pretty) const {
    json configData;
    
    for (const auto& entry : pImpl->entries) {
        configData[entry.key] = configValueToJson(entry.value);
        
        // Add description as a comment (JSON doesn't support comments)
        // In a real implementation, we might use a YAML format for comments
        if (!entry.description.empty()) {
            configData["__descriptions"][entry.key] = entry.description;
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (pretty) {
        file << configData.dump(2);
    } else {
        file << configData.dump();
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    set(key, value, "", "", source);
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

void Config::set(const std::string& key, const std::string& value, const std::string& description,
                 const std::string& validation, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = ConfigValue(value);
        it->source = source;
        it->description = description;
        it->validation = validation;
    } else {
        ConfigEntry entry;
        entry.key = key;
        entry.value = ConfigValue(value);
        entry.source = source;
        entry.description = description;
        entry.validation = validation;
        entry.required = false;
        pImpl->entries.push_back(entry);
    }
}

Config Config::createDefaultConfig() {
    Config config;
    
    // Set default values based on the configuration schema
    config.set("random_seed", 42, "Random seed for reproducible experiments", "int >= 0");
    config.set("simulation_timestep", 0.001, "Simulation timestep in seconds", "float > 0");
    config.set("neuron_count", 1000, "Number of neurons in the brain", "int > 0");
    config.set("region_count", 2, "Number of brain regions", "int >= 1");
    config.set("connection_probability", 0.1, "Connection probability for random connectivity", "float [0,1]");
    
    // Plasticity settings
    config.set("plasticity_learning_rate", 0.01, "Learning rate for plasticity", "float [0,1]");
    config.set("stdp_ltp_weight", 0.01, "STDP LTP weight", "float [0,1]");
    config.set("stdp_ltd_weight", 0.012, "STDP LTD weight", "float [0,1]");
    config.set("stdp_tau", 20.0, "STDP time constant", "float > 0");
    
    // Development settings
    config.set("synaptogenesis_rate", 0.001, "Synaptogenesis rate", "float >= 0");
    config.set("pruning_rate", 0.0001, "Pruning rate", "float >= 0");
    
    // Neuromodulation settings
    config.set("dopamine_baseline", 0.1, "Dopamine baseline level", "float >= 0");
    
    // Reward settings
    config.set("reward_discount_factor", 0.99, "Reward discount factor", "float [0,1]");
    
    // Environment settings
    config.set("environment_name", "GridWorld", "Environment name", "string");
    config.set("environment_width", 10, "Environment width", "int > 0");
    config.set("environment_height", 10, "Environment height", "int > 0");
    
    // Logging settings
    config.set("log_level", "INFO", "Log level", "enum:DEBUG,INFO,WARN,ERROR");
    config.set("log_to_file", false, "Log to file", "bool");
    config.set("log_filename", "nlm.log", "Log filename", "string");
    
    // Simulation settings
    config.set("max_simulation_steps", 10000, "Maximum simulation steps", "int > 0");
    config.set("simulation_time_limit", 0.0, "Simulation time limit", "float >= 0");
    
    // Visualization settings
    config.set("visualization_enabled", false, "Enable visualization", "bool");
    config.set("visualization_update_rate", 30.0, "Visualization update rate", "float > 0");
    
    return config;
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

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->validationErrors.clear();
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

json Config::toJson() const {
    json configData;
    for (const auto& entry : pImpl->entries) {
        configData[entry.key] = configValueToJson(entry.value);
    }
    return configData;
}

bool Config::validate() const {
    pImpl->validationErrors.clear();
    
    // Validate against type patterns if schema available
    // For now, we'll do basic validation
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry.key, entry.value)) {
            pImpl->validationErrors.push_back("Invalid value for key: " + entry.key);
        }
    }
    
    return pImpl->validationErrors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

ConfigSchema Config::getDefaultSchema() {
    ConfigSchema schema;
    
    // Define the default configuration schema
    schema.typePatterns = {
        {"random_seed", "int"},
        {"simulation_timestep", "float"},
        {"neuron_count", "int"},
        {"region_count", "int"},
        {"connection_probability", "float"},
        {"plasticity_learning_rate", "float"},
        {"stdp_ltp_weight", "float"},
        {"stdp_ltd_weight", "float"},
        {"stdp_tau", "float"},
        {"synaptogenesis_rate", "float"},
        {"pruning_rate", "float"},
        {"dopamine_baseline", "float"},
        {"reward_discount_factor", "float"},
        {"environment_width", "int"},
        {"environment_height", "int"},
        {"log_level", "enum"},
        {"log_to_file", "bool"},
        {"max_simulation_steps", "int"},
        {"simulation_time_limit", "float"},
        {"visualization_enabled", "bool"},
        {"visualization_update_rate", "float"}
    };
    
    schema.validationRules = {
        {"random_seed", "int >= 0"},
        {"simulation_timestep", "float > 0"},
        {"neuron_count", "int > 0"},
        {"region_count", "int >= 1"},
        {"connection_probability", "float [0,1]"},
        {"plasticity_learning_rate", "float [0,1]"},
        {"stdp_ltp_weight", "float [0,1]"},
        {"stdp_ltd_weight", "float [0,1]"},
        {"stdp_tau", "float > 0"},
        {"synaptogenesis_rate", "float >= 0"},
        {"pruning_rate", "float >= 0"},
        {"dopamine_baseline", "float >= 0"},
        {"reward_discount_factor", "float [0,1]"},
        {"environment_width", "int > 0"},
        {"environment_height", "int > 0"},
        {"max_simulation_steps", "int > 0"},
        {"simulation_time_limit", "float >= 0"},
        {"visualization_update_rate", "float > 0"}
    };
    
    schema.descriptions = {
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Simulation timestep in seconds"},
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Connection probability for random connectivity"},
        {"plasticity_learning_rate", "Learning rate for plasticity"},
        {"stdp_ltp_weight", "STDP LTP weight"},
        {"stdp_ltd_weight", "STDP LTD weight"},
        {"stdp_tau", "STDP time constant"},
        {"synaptogenesis_rate", "Synaptogenesis rate"},
        {"pruning_rate", "Pruning rate"},
        {"dopamine_baseline", "Dopamine baseline level"},
        {"reward_discount_factor", "Reward discount factor"},
        {"environment_name", "Environment name"},
        {"environment_width", "Environment width"},
        {"environment_height", "Environment height"},
        {"log_level", "Log level"},
        {"log_to_file", "Log to file"},
        {"log_filename", "Log filename"},
        {"max_simulation_steps", "Maximum simulation steps"},
        {"simulation_time_limit", "Simulation time limit"},
        {"visualization_enabled", "Enable visualization"},
        {"visualization_update_rate", "Visualization update rate"}
    };
    
    schema.defaultValues = {
        {"random_seed", "42"},
        {"simulation_timestep", "0.001"},
        {"neuron_count", "1000"},
        {"region_count", "2"},
        {"connection_probability", "0.1"},
        {"plasticity_learning_rate", "0.01"},
        {"stdp_ltp_weight", "0.01"},
        {"stdp_ltd_weight", "0.012"},
        {"stdp_tau", "20.0"},
        {"synaptogenesis_rate", "0.001"},
        {"pruning_rate", "0.0001"},
        {"dopamine_baseline", "0.1"},
        {"reward_discount_factor", "0.99"},
        {"environment_name", "GridWorld"},
        {"environment_width", "10"},
        {"environment_height", "10"},
        {"log_level", "INFO"},
        {"log_to_file", "false"},
        {"log_filename", "nlm.log"},
        {"max_simulation_steps", "10000"},
        {"simulation_time_limit", "0.0"},
        {"visualization_enabled", "false"},
        {"visualization_update_rate", "30.0"}
    };
    
    schema.requiredKeys = {
        {"random_seed", false},
        {"simulation_timestep", false},
        {"neuron_count", false},
        {"region_count", false},
        {"connection_probability", false},
        {"plasticity_learning_rate", false},
        {"stdp_ltp_weight", false},
        {"stdp_ltd_weight", false},
        {"stdp_tau", false},
        {"synaptogenesis_rate", false},
        {"pruning_rate", false},
        {"dopamine_baseline", false},
        {"reward_discount_factor", false},
        {"environment_name", false},
        {"environment_width", false},
        {"environment_height", false},
        {"log_level", false},
        {"log_to_file", false},
        {"log_filename", false},
        {"max_simulation_steps", false},
        {"simulation_time_limit", false},
        {"visualization_enabled", false},
        {"visualization_update_rate", false}
    };
    
    return schema;
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != pImpl->entries.end()) {
            // Merge existing entry - keep the other if it's a higher priority source
            if (static_cast<int>(entry.source) > static_cast<int>(it->source)) {
                *it = entry;
            }
        } else {
            pImpl->entries.push_back(entry);
        }
    }
}

void Config::setFromString(const std::string& keyValuePair) {
    size_t pos = keyValuePair.find('=');
    if (pos != std::string::npos) {
        std::string key = trim(keyValuePair.substr(0, pos));
        std::string value = trim(keyValuePair.substr(pos + 1));
        set(key, value, ConfigSource::CommandLine);
    }
}

std::string Config::getAsString(const std::string& key) const {
    auto val = get<std::string>(key);
    if (val.has_value()) {
        return val.value();
    }
    
    // Try to convert to string from other types
    auto intVal = get<int64_t>(key);
    if (intVal.has_value()) {
        return std::to_string(intVal.value());
    }
    
    auto doubleVal = get<double>(key);
    if (doubleVal.has_value()) {
        return std::to_string(doubleVal.value());
    }
    
    auto boolVal = get<bool>(key);
    if (boolVal.has_value()) {
        return boolVal.value() ? "true" : "false";
    }
    
    return "";
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Basic validation - check if value is valid for its type
    return std::visit([](auto&& v) -> bool {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return !v.empty();
        } else if constexpr (std::is_arithmetic_v<T>) {
            return true;  // Any arithmetic value is valid
        } else if constexpr (std::is_same_v<T, bool>) {
            return true;  // Always valid
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                             std::is_same_v<T, std::vector<double>> ||
                             std::is_same_v<T, std::vector<std::string>>) {
            return true;  // Any vector is valid
        }
        return false;
    }, value);
}

bool Config::validateType(const std::string& key, const ConfigValue& value) const {
    // This would be used with a schema to validate types
    // For now, we'll just return true
    return true;
}

// Template instantiations
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
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

// Explicit template instantiations
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

} // namespace nlm
