#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <algorithm>
#include <cctype>

// Include nlohmann JSON library (single header)
#include <nlohmann/json.hpp>
// Note: For YAML support, you would typically include a YAML library like nlohmann/yaml
// or a separate YAML parser. For now, we'll focus on JSON.

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigError> errors;
    std::string lastError;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool validate) {
    clearErrors();
    
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        setError(ConfigError::FileNotFound, "Configuration file not found: " + filepath);
        return false;
    }
    
    // Read file content
    std::ifstream file(filepath);
    if (!file.is_open()) {
        setError(ConfigError::InvalidFileFormat, "Cannot open configuration file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Determine format by file extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    if (ext == ".json" || ext == ".jsonl") {
        success = parseJSON(content, *this);
    } else if (ext == ".yaml" || ext == ".yml") {
        // TODO: Implement YAML parsing
        setError(ConfigError::InvalidFileFormat, "YAML parsing not yet implemented");
        return false;
    } else {
        // Default to JSON format
        success = parseJSON(content, *this);
    }
    
    if (!success) {
        setError(ConfigError::InvalidFileFormat, "Failed to parse configuration file: " + filepath);
        return false;
    }
    
    if (validate) {
        for (const auto& entry : pImpl->entries) {
            if (!validateConfigValue(entry.value, entry.key)) {
                return false;
            }
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    clearErrors();
    
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

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content;
    if (format == "yaml" || format == "yml") {
        content = serializeToYAML(*this);
    } else {
        // Default to JSON
        content = serializeToJSON(*this);
    }
    
    file << content;
    return true;
}

// Type conversion utilities
std::optional<ConfigValue> Config::stringToConfigValue(const std::string& str, const std::string& typeHint) {
    // Try to parse based on type hint or auto-detect
    std::string hint = typeHint;
    if (hint == "auto") {
        // Auto-detect type
        if (str.empty()) return ConfigValue(std::string(""));
        
        // Check if it's a boolean
        if (str == "true" || str == "false") {
            return ConfigValue(str == "true");
        }
        
        // Check if it's an integer
        try {
            size_t pos;
            int64_t intVal = std::stoll(str, &pos);
            if (pos == str.size()) {
                return ConfigValue(intVal);
            }
        } catch (...) {}
        
        // Check if it's a double
        try {
            size_t pos;
            double doubleVal = std::stod(str, &pos);
            if (pos == str.size()) {
                return ConfigValue(doubleVal);
            }
        } catch (...) {}
        
        // Return as string
        return ConfigValue(str);
    }
    
    // Specific type conversion
    if (hint == "int" || hint == "integer") {
        try {
            return ConfigValue(std::stoll(str));
        } catch (...) {
            return std::nullopt;
        }
    } else if (hint == "double" || hint == "float" || hint == "number") {
        try {
            return ConfigValue(std::stod(str));
        } catch (...) {
            return std::nullopt;
        }
    } else if (hint == "bool" || hint == "boolean") {
        if (str == "true") return ConfigValue(true);
        if (str == "false") return ConfigValue(false);
        return std::nullopt;
    } else if (hint == "string") {
        return ConfigValue(str);
    }
    
    return std::nullopt;
}

std::string Config::configValueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::fixed << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                            std::is_same_v<T, std::vector<double>> ||
                            std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            bool first = true;
            std::visit([&oss, &first](auto&& vec) {
                using VecType = std::decay_t<decltype(vec)>;
                if constexpr (std::is_same_v<VecType, std::vector<int>>) {
                    for (const auto& item : vec) {
                        if (!first) oss << ", ";
                        oss << item;
                        first = false;
                    }
                } else if constexpr (std::is_same_v<VecType, std::vector<double>>) {
                    for (const auto& item : vec) {
                        if (!first) oss << ", ";
                        oss << std::fixed << std::setprecision(6) << item;
                        first = false;
                    }
                } else if constexpr (std::is_same_v<VecType, std::vector<std::string>>) {
                    for (const auto& item : vec) {
                        if (!first) oss << ", ";
                        oss << "\"" << item << "\"";
                        first = false;
                    }
                }
            }, arg);
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::map<std::string, ConfigValue>>) {
            oss << "{";
            bool first = true;
            for (const auto& pair : arg) {
                if (!first) oss << ", ";
                oss << "\"" << pair.first << "\": " << configValueToString(pair.second);
                first = false;
            }
            oss << "}";
        } else if constexpr (std::is_same_v<T, std::vector<std::map<std::string, ConfigValue>>>) {
            oss << "[";
            bool first = true;
            for (const auto& map : arg) {
                if (!first) oss << ", ";
                oss << "{";
                bool innerFirst = true;
                for (const auto& pair : map) {
                    if (!innerFirst) oss << ", ";
                    oss << "\"" << pair.first << "\": " << configValueToString(pair.second);
                    innerFirst = false;
                }
                oss << "}";
                first = false;
            }
            oss << "]";
        }
    }, value);
    return oss.str();
}

bool Config::validateType(const ConfigValue& value, const std::string& typeName) const {
    if (typeName == "auto") {
        // Auto-validation based on actual type
        return true;
    }
    
    std::string type = toLower(typeName);
    
    if (type == "int" || type == "integer" || type == "int64") {
        return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    } else if (type == "double" || type == "float" || type == "number") {
        return std::holds_alternative<double>(value);
    } else if (type == "bool" || type == "boolean") {
        return std::holds_alternative<bool>(value);
    } else if (type == "string") {
        return std::holds_alternative<std::string>(value);
    } else if (type == "vector") {
        return std::holds_alternative<std::vector<int>>(value) ||
               std::holds_alternative<std::vector<double>>(value) ||
               std::holds_alternative<std::vector<std::string>>(value);
    } else if (type == "object" || type == "map") {
        return std::holds_alternative<std::map<std::string, ConfigValue>>(value);
    }
    
    return false;
}

bool Config::validateConfigFile(const std::string& filepath, const std::string& format) {
    // Simple validation - check if file exists and is readable
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse to validate
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // For now, just check if content is not empty
    return !content.empty();
}

bool Config::validateConfigValue(const ConfigValue& value, const std::string& key, const std::string& typeHint) const {
    // Basic validation - can be extended with more specific rules
    if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        if (std::isnan(val) || std::isinf(val)) {
            return false;
        }
    }
    
    // Type-specific validation
    if (typeHint != "auto") {
        if (!validateType(value, typeHint)) {
            return false;
        }
    }
    
    return true;
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    return ConfigSource::Default;
}

std::vector<ConfigEntry> Config::getEntriesFromSource(ConfigSource source) const {
    std::vector<ConfigEntry> entries;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            entries.push_back(entry);
        }
    }
    return entries;
}

bool Config::isDefault(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->source == ConfigSource::Default;
    }
    return true;
}

std::vector<ConfigError> Config::getValidationErrors() const {
    return pImpl->errors;
}

std::string Config::getLastError() const {
    return pImpl->lastError;
}

void Config::setError(ConfigError error, const std::string& message) {
    pImpl->errors.push_back(error);
    pImpl->lastError = message;
}

void Config::clearErrors() {
    pImpl->errors.clear();
    pImpl->lastError.clear();
}

// Template instantiations
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

// Static helper implementations
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

std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string Config::normalizeKey(const std::string& key) {
    std::string result = key;
    // Replace spaces and special characters with underscores
    std::replace_if(result.begin(), result.end(),
        [](char c) { return !std::isalnum(c) && c != '_'; }, '_');
    return result;
}

// Merge strategies implementation
bool Config::mergeFromFile(const std::string& filepath, bool overwriteFileSettings) {
    clearErrors();
    
    // Load from file
    Config fileConfig;
    if (!fileConfig.loadFromFile(filepath, false)) { // No validation on merge
        // If the file doesn't exist, it's not an error for merge
        if (getLastError() != "Configuration file not found: " + filepath) {
            return false;
        }
    }
    
    // Merge with current config
    for (const auto& entry : fileConfig.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != pImpl->entries.end()) {
            if (overwriteFileSettings) {
                it->value = entry.value;
                it->source = entry.source;
            } else if (it->source != ConfigSource::Default) {
                // Keep existing non-default values
                continue;
            } else {
                // Replace default values
                it->value = entry.value;
                it->source = entry.source;
            }
        } else {
            // Add new entry
            pImpl->entries.push_back(entry);
        }
    }
    
    return true;
}

bool Config::mergeFromArgs(int argc, char** argv, bool overwriteCommandLine) {
    clearErrors();
    
    // Load from command line arguments
    Config argsConfig;
    if (!argsConfig.loadFromArgs(argc, argv)) {
        return false;
    }
    
    // Merge with current config
    for (const auto& entry : argsConfig.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != pImpl->entries.end()) {
            if (overwriteCommandLine) {
                it->value = entry.value;
                it->source = entry.source;
            } else if (it->source != ConfigSource::Default) {
                // Keep existing non-default values
                continue;
            } else {
                // Replace default values
                it->value = entry.value;
                it->source = entry.source;
            }
        } else {
            // Add new entry
            pImpl->entries.push_back(entry);
        }
    }
    
    return true;
}

Config Config::mergeWith(const Config& other, ConfigSource source) const {
    Config merged;
    
    // Copy existing entries
    for (const auto& entry : pImpl->entries) {
        merged.set(entry.key, entry.value, entry.source);
    }
    
    // Merge with other config
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(merged.pImpl->entries.begin(), merged.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != merged.pImpl->entries.end()) {
            // Prefer the other config's value
            it->value = entry.value;
            it->source = source;
        } else {
            merged.set(entry.key, entry.value, source);
        }
    }
    
    return merged;
}

// JSON parsing implementation
bool Config::parseJSON(const std::string& content, Config& config) {
    try {
        auto jsonData = nlohmann::json::parse(content);
        
        // Clear existing config
        config.clear();
        
        // Convert JSON to Config
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            nlohmann::json value = *it;
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            bool success = false;
            
            if (value.is_string()) {
                configValue = value.get<std::string>();
                success = true;
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
                success = true;
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
                success = true;
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
                success = true;
            } else if (value.is_array()) {
                // Convert JSON array to appropriate vector type
                if (!value.empty()) {
                    const auto& firstElement = value[0];
                    if (firstElement.is_number_integer()) {
                        std::vector<int64_t> intVector = value.get<std::vector<int64_t>>();
                        configValue = std::vector<int>(intVector.begin(), intVector.end());
                        success = true;
                    } else if (firstElement.is_number_float()) {
                        configValue = value.get<std::vector<double>>();
                        success = true;
                    } else if (firstElement.is_string()) {
                        configValue = value.get<std::vector<std::string>>();
                        success = true;
                    } else {
                        // Try to convert mixed array
                        config.setError(ConfigError::InvalidType, "Cannot parse JSON array with mixed types");
                        return false;
                    }
                } else {
                    // Empty array
                    configValue = std::vector<int>();
                    success = true;
                }
            } else if (value.is_object()) {
                // Convert JSON object to map
                std::map<std::string, ConfigValue> objectMap;
                for (auto objIt = value.begin(); objIt != value.end(); ++objIt) {
                    std::string objKey = objIt.key();
                    ConfigValue objValue;
                    
                    // Recursively convert nested JSON values
                    if (objIt->is_string()) {
                        objValue = objIt->get<std::string>();
                    } else if (objIt->is_number_integer()) {
                        objValue = objIt->get<int64_t>();
                    } else if (objIt->is_number_float()) {
                        objValue = objIt->get<double>();
                    } else if (objIt->is_boolean()) {
                        objValue = objIt->get<bool>();
                    } else if (objIt->is_array()) {
                        // For now, treat arrays as generic ConfigValue
                        objValue = objIt->get<std::vector<nlohmann::json>>();
                    } else if (objIt->is_object()) {
                        // Nested object - treat as generic ConfigValue
                        objValue = objIt->get<std::map<std::string, nlohmann::json>>();
                    }
                    
                    objectMap[objKey] = objValue;
                }
                configValue = objectMap;
                success = true;
            }
            
            if (success) {
                config.set(key, configValue, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        config.setError(ConfigError::InvalidFileFormat, "JSON parse error: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        config.setError(ConfigError::InvalidFileFormat, "Error parsing JSON: " + std::string(e.what()));
        return false;
    }
}

// YAML parsing implementation (placeholder - would need YAML library)
bool Config::parseYAML(const std::string& content, Config& config) {
    // TODO: Implement YAML parsing
    // This is a placeholder that assumes YAML parsing will be added
    config.setError(ConfigError::InvalidFileFormat, "YAML parsing not yet implemented");
    return false;
}

// JSON serialization implementation
std::string Config::serializeToJSON(const Config& config) const {
    nlohmann::json jsonData;
    
    for (const auto& entry : config.pImpl->entries) {
        // Convert ConfigValue to JSON
        nlohmann::json jsonValue;
        
        std::visit([&jsonValue](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::map<std::string, ConfigValue>>) {
                // Convert map to JSON object
                nlohmann::json jsonMap;
                for (const auto& pair : arg) {
                    // For nested objects, we need to convert ConfigValue to JSON
                    // This is a simplified approach
                    jsonMap[pair.first] = pair.second;
                }
                jsonValue = jsonMap;
            }
        }, entry.value);
        
        jsonData[entry.key] = jsonValue;
    }
    
    return jsonData.dump(2); // Pretty print with 2-space indentation
}

// YAML serialization implementation (placeholder)
std::string Config::serializeToYAML(const Config& config) const {
    // TODO: Implement YAML serialization
    // This is a placeholder that assumes YAML serialization will be added
    std::ostringstream oss;
    oss << "# YAML serialization not yet implemented\n";
    for (const auto& entry : config.pImpl->entries) {
        oss << entry.key << ": " << configValueToString(entry.value) << "\n";
    }
    return oss.str();
}

} // namespace nlm