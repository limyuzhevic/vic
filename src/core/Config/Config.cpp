#include "Config.hpp"
#include "../ErrorHandling.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
// Include JSON and YAML libraries
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Use ErrorHandler for file operations
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR(ErrorCode::System_FileNotFound,
                         "Cannot open config file: " + filepath);
            return false;
        }
        
        // Detect file format by extension and content
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Clear existing entries
        clear();
        
        if (endsWith(filepath, ".json") || startsWith(content, "{")) {
            // Parse as JSON
            parseJSON(content, filepath);
        } else if (endsWith(filepath, ".yaml") || endsWith(filepath, ".yml") || 
                   startsWith(content, "---") || contains(content, ": ")) {
            // Parse as YAML
            parseYAML(content, filepath);
        } else {
            // Parse as simple key=value format (legacy)
            parseSimpleFormat(content, filepath);
        }
        
        return true;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::System_IOError,
                       "Failed to load config file: " + filepath + 
                       " - " + std::string(e.what()));
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    NLM_LOG_ERROR(ErrorCode::Component_FormatError,
                                 "Invalid command line argument: " + arg);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_ERROR(ErrorCode::Component_FormatError,
                             "Invalid command line argument: " + arg);
            }
        }
        return true;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_InitializationError,
                       "Failed to load config from command line arguments");
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Determine output format based on file extension
        std::string content;
        
        if (endsWith(filepath, ".json")) {
            // Save as JSON
            nlohmann::json jsonData;
            for (const auto& entry : pImpl->entries) {
                // Convert ConfigValue to nlohmann::json
                if (std::holds_alternative<std::string>(entry.value)) {
                    jsonData[entry.key] = std::get<std::string>(entry.value);
                } else if (std::holds_alternative<int64_t>(entry.value)) {
                    jsonData[entry.key] = std::get<int64_t>(entry.value);
                } else if (std::holds_alternative<double>(entry.value)) {
                    jsonData[entry.key] = std::get<double>(entry.value);
                } else if (std::holds_alternative<bool>(entry.value)) {
                    jsonData[entry.key] = std::get<bool>(entry.value);
                } else if (std::holds_alternative<std::vector<float>>(entry.value)) {
                    jsonData[entry.key] = std::get<std::vector<float>>(entry.value);
                }
            }
            content = jsonData.dump(4); // Pretty print with 4-space indentation
        } else if (endsWith(filepath, ".yaml") || endsWith(filepath, ".yml")) {
            // Save as YAML
            YAML::Emitter yamlData;
            yamlData << YAML::BeginMap;
            for (const auto& entry : pImpl->entries) {
                yamlData << YAML::Key << entry.key;
                
                // Convert ConfigValue to YAML::Node
                if (std::holds_alternative<std::string>(entry.value)) {
                    yamlData << std::get<std::string>(entry.value);
                } else if (std::holds_alternative<int64_t>(entry.value)) {
                    yamlData << std::get<int64_t>(entry.value);
                } else if (std::holds_alternative<double>(entry.value)) {
                    yamlData << std::get<double>(entry.value);
                } else if (std::holds_alternative<bool>(entry.value)) {
                    yamlData << std::get<bool>(entry.value);
                } else if (std::holds_alternative<std::vector<float>>(entry.value)) {
                    yamlData << std::get<std::vector<float>>(entry.value);
                }
            }
            yamlData << YAML::EndMap;
            content = std::string(yamlData.c_str());
        } else {
            // Save as legacy key=value format
            std::ostringstream oss;
            for (const auto& entry : pImpl->entries) {
                oss << entry.key << " = ";
                
                // Convert ConfigValue to string
                if (std::holds_alternative<std::string>(entry.value)) {
                    oss << "\"" << std::get<std::string>(entry.value) << "\"";
                } else if (std::holds_alternative<int64_t>(entry.value)) {
                    oss << std::get<int64_t>(entry.value);
                } else if (std::holds_alternative<double>(entry.value)) {
                    oss << std::get<double>(entry.value);
                } else if (std::holds_alternative<bool>(entry.value)) {
                    oss << (std::get<bool>(entry.value) ? "true" : "false");
                } else if (std::holds_alternative<std::vector<float>>(entry.value)) {
                    oss << "[";
                    const auto& vec = std::get<std::vector<float>>(entry.value);
                    for (size_t i = 0; i < vec.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << vec[i];
                    }
                    oss << "]";
                }
                oss << "\n";
            }
            content = oss.str();
        }
        
        // Use ErrorHandler for file operations
        ErrorHandler::writeFile(filepath, content);
        return true;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::System_IOError,
                       "Failed to save config to file: " + filepath + 
                       " - " + std::string(e.what()));
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    try {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            return std::nullopt;
        }
        
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            NLM_LOG_ERROR(ErrorCode::Component_FormatError,
                         "Type mismatch accessing config key: " + key);
            return std::nullopt;
        }
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::System_IOError,
                       "Error accessing config key: " + key);
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    try {
        auto val = get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::System_IOError,
                       "Error accessing config key: " + key);
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    try {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to set config key: " + key);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    try {
        set(key, ConfigValue(value), source);
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to set config key: " + key);
    }
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    try {
        set(key, ConfigValue(value), source);
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to set config key: " + key);
    }
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    try {
        set(key, ConfigValue(value), source);
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to set config key: " + key);
    }
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    try {
        set(key, ConfigValue(value), source);
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to set config key: " + key);
    }
}

bool Config::has(const std::string& key) const {
    try {
        return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::System_IOError,
                       "Error checking for config key: " + key);
    }
}

void Config::remove(const std::string& key) {
    try {
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            pImpl->entries.end()
        );
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to remove config key: " + key);
    }
}

std::vector<std::string> Config::getKeys() const {
    try {
        std::vector<std::string> keys;
        keys.reserve(pImpl->entries.size());
        for (const auto& entry : pImpl->entries) {
            keys.push_back(entry.key);
        }
        return keys;
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::System_IOError,
                       "Failed to get config keys");
    }
}

void Config::clear() {
    try {
        pImpl->entries.clear();
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to clear config");
    }
}

std::string Config::summary() const {
    try {
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
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_OperationFailed,
                       "Failed to generate config summary");
    }
}

// Helper function to check if string ends with suffix
bool Config::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// Helper function to check if string starts with prefix
bool Config::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && 
           str.compare(0, prefix.size(), prefix) == 0;
}

// Helper function to check if string contains substring
bool Config::contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

// Parse JSON content
void Config::parseJSON(const std::string& json, const std::string& filepath) {
    try {
        // Parse JSON using nlohmann/json library
        auto jsonData = nlohmann::json::parse(json);
        
        // Convert JSON to Config entries
        if (jsonData.is_object()) {
            for (const auto& [key, value] : jsonData.items()) {
                ConfigValue configValue;
                
                if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_number_integer()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_array()) {
                    // Convert JSON array to vector<float>
                    std::vector<float> vec;
                    for (const auto& item : value) {
                        vec.push_back(item.get<float>());
                    }
                    configValue = vec;
                } else if (value.is_null()) {
                    configValue = std::string(); // Empty string for null
                } else {
                    NLM_THROW_ERROR(ErrorCode::Component_FormatError,
                                   "Unsupported JSON value type for key: " + key);
                }
                
                set(key, configValue, ConfigSource::File);
            }
        }
        
    } catch (const nlohmann::json::parse_error& e) {
        NLM_THROW_ERROR(ErrorCode::Component_FormatError,
                       "Failed to parse JSON config file " + filepath + 
                       " - " + std::string(e.what()));
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_FormatError,
                       "Error parsing JSON config file " + filepath + 
                       " - " + std::string(e.what()));
    }
}

void Config::parseYAML(const std::string& yaml, const std::string& filepath) {
    try {
        // Parse YAML using yaml-cpp library
        auto yamlData = YAML::Load(yaml);
        
        // Convert YAML to Config entries
        if (yamlData.IsMap()) {
            for (const auto& it : yamlData) {
                std::string key = it.first.as<std::string>();
                const YAML::Node& value = it.second;
                
                ConfigValue configValue;
                
                if (value.IsScalar()) {
                    // Try to determine type
                    try {
                        bool boolVal = value.as<bool>();
                        configValue = boolVal;
                    } catch (...) {
                        try {
                            int64_t intVal = value.as<int64_t>();
                            configValue = intVal;
                        } catch (...) {
                            try {
                                double floatVal = value.as<double>();
                                configValue = floatVal;
                            } catch (...) {
                                // Treat as string
                                configValue = value.as<std::string>();
                            }
                        }
                    }
                } else if (value.IsSequence()) {
                    // Convert YAML sequence to vector<float>
                    std::vector<float> vec;
                    for (const auto& item : value) {
                        vec.push_back(item.as<float>());
                    }
                    configValue = vec;
                } else if (value.IsNull()) {
                    configValue = std::string(); // Empty string for null
                } else {
                    NLM_THROW_ERROR(ErrorCode::Component_FormatError,
                                   "Unsupported YAML value type for key: " + key);
                }
                
                set(key, configValue, ConfigSource::File);
            }
        }
        
    } catch (const YAML::Exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_FormatError,
                       "Failed to parse YAML config file " + filepath + 
                       " - " + std::string(e.what()));
    } catch (const NLMError&) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_ERROR(ErrorCode::Component_FormatError,
                       "Error parsing YAML config file " + filepath + 
                       " - " + std::string(e.what()));
    }
}

// Trim whitespace from string
std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}
    std::string line;
    size_t lineNumber = 0;
    
    // Use stringstream to process lines
    std::istringstream stream(content);
    while (std::getline(stream, line)) {
        lineNumber++;
        line = trim(line);
        
        // Skip empty lines and comments
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const NLMError&) {
                NLM_LOG_ERROR(ErrorCode::Component_FormatError,
                             "Error parsing config file " + filepath + 
                             " at line " + std::to_string(lineNumber));
            }
        }
    }
}

// Skip whitespace in string
void Config::skipWhitespace(const std::string& str, std::string::size_type& pos) {
    while (pos < str.size() && std::isspace(str[pos])) {
        pos++;
    }
}

// Trim whitespace from string
std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}
