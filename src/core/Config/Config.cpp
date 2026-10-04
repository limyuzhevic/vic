#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iostream>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schema;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default schema
    initializeSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeSchema() {
    // Core configuration parameters with documentation and validation
    pImpl->schema = {
        {"random_seed", "int64", "Random seed for reproducible experiments", 42, false, "^[0-9]+$", ""},
        {"simulation_timestep", "double", "Simulation timestep in seconds", 0.001, false, "^[0-9]+(\\.[0-9]+)?$", "s"},
        {"neuron_count", "int64", "Number of neurons in the brain", 1000, false, "^[0-9]+$", "count"},
        {"region_count", "int64", "Number of brain regions", 2, false, "^[0-9]+$", "count"},
        {"connection_probability", "double", "Connection probability for random connectivity", 0.1, false, "^(0\\.[0-9]+|[1]\\.[0-9]+|[1])$", ""},
        {"plasticity_learning_rate", "double", "Plasticity learning rate", 0.01, false, "^[0-9]+(\\.[0-9]+)?$", ""},
        {"stdp_ltp_weight", "double", "STDP LTP weight", 0.01, false, "^[0-9]+(\\.[0-9]+)?$", ""},
        {"stdp_ltd_weight", "double", "STDP LTD weight", 0.012, false, "^[0-9]+(\\.[0-9]+)?$", ""},
        {"stdp_time_constant", "double", "STDP time constant in ms", 20.0, false, "^[0-9]+(\\.[0-9]+)?$", "ms"},
        {"development_synaptogenesis_rate", "double", "Development synaptogenesis rate", 0.001, false, "^[0-9]+(\\.[0-9]+)?$", ""},
        {"development_pruning_rate", "double", "Development pruning rate", 0.0001, false, "^[0-9]+(\\.[0-9]+)?$", ""},
        {"dopamine_baseline", "double", "Baseline dopamine level", 0.0, false, "^(-?[0-9]+(\\.[0-9]+)?)$", ""},
        {"reward_discount_factor", "double", "Reward discount factor", 0.99, false, "^[0-9]+(\\.[0-9]+)?$", ""},
        {"environment_name", "string", "Environment type name", "GridWorld", false, "^([A-Za-z0-9_]+)$", ""},
        {"environment_width", "int64", "Environment width in units", 10, false, "^[0-9]+$", "units"},
        {"environment_height", "int64", "Environment height in units", 10, false, "^[0-9]+$", "units"},
        {"log_level", "string", "Logging level (DEBUG, INFO, WARN, ERROR)", "INFO", false, "^(DEBUG|INFO|WARN|ERROR)$", ""},
        {"log_to_file", "bool", "Enable logging to file", false, false, "", ""},
        {"log_filename", "string", "Log file name", "nlm.log", false, "^([a-zA-Z0-9_\\-\\.]+)\\.log$", ""},
        {"max_simulation_steps", "int64", "Maximum simulation steps", 10000, false, "^[0-9]+$", "count"},
        {"simulation_time_limit", "double", "Simulation time limit in seconds", 0.0, false, "^[0-9]+(\\.[0-9]+)?$", "s"},
        {"visualization_enabled", "bool", "Enable visualization", false, false, "", ""},
        {"visualization_update_rate", "double", "Visualization update rate in Hz", 30.0, false, "^[0-9]+(\\.[0-9]+)?$", "Hz"},
    };
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
            return false;
        }
        
        // Try JSON first
        try {
            auto j = nlohmann::json::parse(file);
            return loadFromJSON(j);
        } catch (const nlohmann::json::parse_error&) {
            // Reset file stream for text parsing
            file.clear();
            file.seekg(0, std::ios::beg);
        }
        
        // Fall back to text format
        return loadFromTextFile(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromTextFile(const std::string& filepath) {
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
        
        // Parse key=value pairs with validation
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate key exists in schema
            if (!validateKey(key)) {
                NLM_LOG_WARN("Unknown configuration key in " + filepath + ": " + key + " (line " + std::to_string(lineNumber) + ")");
                // Continue anyway for backward compatibility
            }
            
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARN("Malformed configuration line in " + filepath + ": " + line + " (line " + std::to_string(lineNumber) + ")");
        }
    }
    
    return true;
}

bool Config::loadFromJSON(const nlohmann::json& j) {
    try {
        if (j.is_object()) {
            for (const auto& item : j.items()) {
                const std::string& key = item.key();
                const nlohmann::json& value = item.value();
                
                if (!validateKey(key)) {
                    NLM_LOG_WARN("Unknown configuration key in JSON: " + key);
                    continue;
                }
                
                // Convert JSON value to ConfigValue
                ConfigValue configValue;
                if (value.is_number_integer()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_array()) {
                    // Handle arrays
                    if (value.empty()) {
                        configValue = std::vector<int>();
                    } else if (value[0].is_number_integer()) {
                        configValue = value.get<std::vector<int64_t>>();
                    } else if (value[0].is_number_float()) {
                        configValue = value.get<std::vector<double>>();
                    } else {
                        configValue = value.get<std::vector<std::string>>();
                    }
                } else {
                    NLM_LOG_ERROR("Unsupported JSON value type for key: " + key);
                    continue;
                }
                
                set(key, configValue, ConfigSource::File);
            }
        } else {
            NLM_LOG_ERROR("Invalid JSON format: expected object at root level");
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON configuration: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath, Format format) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
            return false;
        }
        
        if (format == Format::JSON) {
            return saveToJSONFile(file);
        } else {
            return saveToTextFile(file);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToJSONFile(std::ostream& stream) const {
    nlohmann::json j;
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = convertToJSON(entry.value);
    }
    
    stream << j.dump(2); // Pretty print with 2-space indentation
    return true;
}

bool Config::saveToTextFile(std::ostream& stream) const {
    for (const auto& entry : pImpl->entries) {
        // Write description as comment
        if (!entry.description.empty()) {
            stream << "# " << entry.description << "\n";
        }
        if (!entry.units.empty()) {
            stream << "# Units: " << entry.units << "\n";
        }
        
        // Write the key-value pair
        stream << entry.key << " = ";
        
        // Convert value to string
        std::visit([&stream](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                stream << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                stream << (arg ? "true" : "false");
            } else {
                stream << arg;
            }
        }, entry.value);
        
        stream << "\n\n";
    }
    
    return true;
}

nlohmann::json Config::convertToJSON(const ConfigValue& value) const {
    nlohmann::json j;
    
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::same_as<T, int64_t> || std::same_as<T, int>) {
            j = arg;
        } else if constexpr (std::same_as<T, double>) {
            j = arg;
        } else if constexpr (std::same_as<T, bool>) {
            j = arg;
        } else if constexpr (std::same_as<T, std::string>) {
            j = arg;
        } else if constexpr (std::same_as<T, std::vector<int64_t>>) {
            j = arg;
        } else if constexpr (std::same_as<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::same_as<T, std::vector<std::string>>) {
            j = arg;
        } else if constexpr (std::same_as<T, std::vector<int>>) {
            j = arg;
        }
    }, value);
    
    return j;
}

bool Config::validateKey(const std::string& key) const {
    return std::any_of(pImpl->schema.begin(), pImpl->schema.end(),
        [&key](const ConfigSchema& s) { return s.key == key; });
}

bool Config::validateEntry(const ConfigEntry& entry) const {
    // Find schema entry
    auto schemaIt = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
        [&entry](const ConfigSchema& s) { return s.key == entry.key; });
    
    if (schemaIt == pImpl->schema.end()) {
        // No schema validation, but key exists in entries
        return true;
    }
    
    const auto& schema = *schemaIt;
    
    // Check required field
    if (schemaIt->required && (entry.description.empty())) {
        NLM_LOG_ERROR("Required configuration key missing: " + entry.key);
        return false;
    }
    
    // Validate value type
    bool typeValid = false;
    if (schema.type == "int64" || schema.type == "int") {
        typeValid = std::holds_alternative<int64_t>(entry.value) || 
                   std::holds_alternative<int>(entry.value);
    } else if (schema.type == "double") {
        typeValid = std::holds_alternative<double>(entry.value);
    } else if (schema.type == "bool") {
        typeValid = std::holds_alternative<bool>(entry.value);
    } else if (schema.type == "string") {
        typeValid = std::holds_alternative<std::string>(entry.value);
    } else if (schema.type == "array") {
        typeValid = std::holds_alternative<std::vector<int64_t>>(entry.value) ||
                   std::holds_alternative<std::vector<double>>(entry.value) ||
                   std::holds_alternative<std::vector<std::string>>(entry.value) ||
                   std::holds_alternative<std::vector<int>>(entry.value);
    }
    
    if (!typeValid) {
        NLM_LOG_ERROR("Invalid type for configuration key " + entry.key + ": expected " + schema.type);
        return false;
    }
    
    // Validate against regex pattern if specified
    if (!schema.validationRegex.empty() && !entry.key.empty()) {
        std::string valueStr;
        std::visit([&valueStr](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = arg;
            } else {
                std::ostringstream oss;
                oss << arg;
                valueStr = oss.str();
            }
        }, entry.value);
        
        std::regex regex(schema.validationRegex);
        if (!std::regex_match(valueStr, regex)) {
            NLM_LOG_ERROR("Configuration key " + entry.key + " value '" + valueStr + "' does not match pattern: " + schema.validationRegex);
            return false;
        }
    }
    
    // Validate against allowed values if specified
    if (!schema.allowedValues.empty()) {
        std::string valueStr;
        std::visit([&valueStr](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = arg;
            } else {
                std::ostringstream oss;
                oss << arg;
                valueStr = oss.str();
            }
        }, entry.value);
        
        if (std::find(schema.allowedValues.begin(), schema.allowedValues.end(), valueStr) == schema.allowedValues.end()) {
            NLM_LOG_ERROR("Configuration key " + entry.key + " value '" + valueStr + "' is not in allowed values: " + 
                         [](const std::vector<std::string>& v) {
                             std::string result;
                             for (size_t i = 0; i < v.size(); ++i) {
                                 if (i > 0) result += ", ";
                                 result += v[i];
                             }
                             return result;
                         }(schema.allowedValues));
            return false;
        }
    }
    
    return true;
}

ConfigValue Config::parseValueFromString(const std::string& valueStr, const std::string& key) const {
    // Find schema entry for type hint
    auto schemaIt = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
        [&key](const ConfigSchema& s) { return s.key == key; });
    
    if (schemaIt == pImpl->schema.end()) {
        // No schema, try to infer type
        return inferType(valueStr);
    }
    
    const auto& schema = *schemaIt;
    
    try {
        if (schema.type == "int64" || schema.type == "int") {
            return std::stoll(valueStr);
        } else if (schema.type == "double") {
            return std::stod(valueStr);
        } else if (schema.type == "bool") {
            // Handle various boolean representations
            std::string lower = toLower(valueStr);
            if (lower == "true" || lower == "yes" || lower == "1") {
                return true;
            } else if (lower == "false" || lower == "no" || lower == "0") {
                return false;
            } else {
                throw std::invalid_argument("Invalid boolean value: " + valueStr);
            }
        } else if (schema.type == "string") {
            return valueStr;
        } else if (schema.type == "array") {
            // Simple array parsing (comma-separated values)
            std::vector<int64_t> intArray;
            std::stringstream ss(valueStr);
            std::string token;
            while (std::getline(ss, token, ',')) {
                token = trim(token);
                if (!token.empty()) {
                    intArray.push_back(std::stoll(token));
                }
            }
            return intArray;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing value for key " + key + ": " + std::string(e.what()));
        throw;
    }
    
    return inferType(valueStr);
}

ConfigValue Config::inferType(const std::string& valueStr) const {
    // Try different types in order of likelihood
    
    // Boolean
    std::string lower = toLower(valueStr);
    if (lower == "true" || lower == "yes" || lower == "1") {
        return true;
    } else if (lower == "false" || lower == "no" || lower == "0") {
        return false;
    }
    
    // Integer
    try {
        size_t pos;
        int64_t intVal = std::stoll(valueStr, &pos);
        if (pos == valueStr.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Double
    try {
        size_t pos;
        double doubleVal = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // String (default)
    return valueStr;
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
    // Validate if we have a schema
    if (!pImpl->schema.empty()) {
        ConfigEntry tempEntry(key, value, source);
        if (!validateEntry(tempEntry)) {
            throw std::invalid_argument("Invalid configuration value for key: " + key);
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
    try {
        ConfigValue parsedValue = parseValueFromString(value, key);
        set(key, parsedValue, source);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error setting configuration key " + key + ": " + std::string(e.what()));
        throw;
    }
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