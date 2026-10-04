#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cctype>
#include <map>
#include "ErrorHandling/Error.hpp"
#include "ErrorHandlingSystem.hpp"

// nlohmann/json support for JSON parsing
#include <nlohmann/json.hpp>

// yaml-cpp support for YAML parsing
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
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW_IO("Cannot open configuration file: " + filepath);
            return false;
        }
    
        // Read the entire file content
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
    
        // Determine file format based on extension and content
        std::string ext = std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
        bool success = false;
    
        // Try to parse as JSON first (if extension is .json or .cfg)
        if (ext == ".json" || ext == ".cfg") {
            try {
                auto jsonData = nlohmann::json::parse(content);
                success = parseJSON(jsonData);
            } catch (const nlohmann::json::parse_error& e) {
                // If JSON parsing fails, try YAML
                if (ext == ".cfg") {
                    try {
                        auto yamlData = YAML::Load(content);
                        success = parseYAML(yamlData);
                    } catch (const YAML::ParserException& e) {
                        // If YAML parsing also fails, fall back to key=value
                        success = parseKeyValue(content);
                    }
                }
            }
        } 
        // Try to parse as YAML (if extension is .yaml or .yml)
        else if (ext == ".yaml" || ext == ".yml") {
            try {
                auto yamlData = YAML::Load(content);
                success = parseYAML(yamlData);
            } catch (const YAML::ParserException& e) {
                // If YAML parsing fails, try key=value
                success = parseKeyValue(content);
            }
        }
    
        // Fall back to key=value format for unknown extensions or parsing failures
        if (!success) {
            success = parseKeyValue(content);
        }
    
        return success;
    } catch (const NLMError& e) {
        // Re-throw NLM errors as-is
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG("Configuration loading failed: " + std::string(e.what()) + " in file: " + filepath);
        return false;
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
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG("Configuration from command line failed: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Determine file format based on extension
        std::string ext = std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == ".json") {
            return saveAsJSON(filepath);
        } else if (ext == ".yaml" || ext == ".yml") {
            return saveAsYAML(filepath);
        } else {
            // Default to key-value format for backward compatibility
            return saveAsKeyValue(filepath);
        }
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG("Configuration saving failed: " + std::string(e.what()) + " to file: " + filepath);
        return false;
    }
}

bool Config::saveAsJSON(const std::string& filepath) const {
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            // Convert config value to JSON
            nlohmann::json jsonValue;
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    jsonValue = val;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonValue = val;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonValue = val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonValue = val;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    jsonValue = val;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonValue = val;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonValue = val;
                } else {
                    jsonValue = configValueToString(val);
                }
            }, entry.value);
            
            jsonData[entry.key] = jsonValue;
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW_IO("Cannot write to configuration file: " + filepath);
            return false;
        }
        
        file << std::setw(4) << jsonData << std::endl;
        return true;
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG("JSON saving failed: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveAsYAML(const std::string& filepath) const {
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            
            // Convert config value to YAML
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    emitter << val;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    emitter << val;
                } else if constexpr (std::is_same_v<T, double>) {
                    emitter << val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    emitter << val;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : val) {
                        emitter << item;
                    }
                    emitter << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : val) {
                        emitter << item;
                    }
                    emitter << YAML::EndSeq;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    emitter << YAML::BeginSeq;
                    for (const auto& item : val) {
                        emitter << item;
                    }
                    emitter << YAML::EndSeq;
                } else {
                    emitter << configValueToString(val);
                }
            }, entry.value);
        }
        
        emitter << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW_IO("Cannot write to YAML configuration file: " + filepath);
            return false;
        }
        
        file << emitter.c_str();
        return true;
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG("YAML saving failed: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveAsKeyValue(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW_IO("Cannot write to key-value configuration file: " + filepath);
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << configValueToString(entry.value) << "\n\n";
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG("Key-value saving failed: " + std::string(e.what()));
        return false;
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
            NLM_THROW_VALIDATION("Type mismatch for key: " + key + ". Expected " + typeid(T).name() + ".");
            return std::nullopt;
        }
    } catch (const NLMError& e) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG("Error getting configuration value for key: " + key + ". " + std::string(e.what()));
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    try {
        auto val = get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    } catch (const NLMError& e) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG("Error getting default configuration value for key: " + key + ". " + std::string(e.what()));
        return defaultValue;
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
    } catch (const NLMError& e) {
        throw;
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG("Error setting configuration value for key: " + key + ". " + std::string(e.what()));
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
    try {
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            pImpl->entries.end()
        );
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG("Error removing configuration key: " + key + ". " + std::string(e.what()));
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
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG("Error getting configuration keys. " + std::string(e.what()));
        return std::vector<std::string>{};
    }
}

void Config::clear() {
    pImpl->entries.clear();
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
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG("Error generating configuration summary. " + std::string(e.what()));
        return "Error: Could not generate configuration summary.";
    }
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

// Helper function to parse JSON data
bool Config::parseJSON(const nlohmann::json& jsonData) {
    try {
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            std::string valueStr = it.dump();
            
            // Parse value based on type
            ConfigValue value;
            if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_array()) {
                // For arrays, convert to string representation for simplicity
                value = valueStr;
            } else {
                // For other types, convert to string
                value = valueStr;
            }
            
            set(key, value, ConfigSource::File);
        }
        return true;
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG("JSON parsing failed: " + std::string(e.what()));
        return false;
    }
}

// Helper function to parse YAML data
bool Config::parseYAML(const YAML::Node& yamlData) {
    try {
        for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
            std::string key = it->first.as<std::string>();
            
            // Parse value based on type
            ConfigValue value;
            if (it->second.IsScalar()) {
                std::string valueStr = it->second.as<std::string>();
                
                // Try to convert to appropriate type
                if (valueStr == "true" || valueStr == "false") {
                    value = valueStr == "true";
                } else {
                    try {
                        size_t pos;
                        int intVal = std::stoi(valueStr, &pos);
                        if (pos == valueStr.size()) {
                            value = intVal;
                        } else {
                            double doubleVal = std::stod(valueStr, &pos);
                            if (pos == valueStr.size()) {
                                value = doubleVal;
                            } else {
                                value = valueStr;
                            }
                        }
                    } catch (...) {
                        value = valueStr;
                    }
                }
            } else if (it->second.IsSequence()) {
                // For sequences, convert to string representation
                std::vector<std::string> vec;
                for (const auto& item : it->second) {
                    vec.push_back(item.as<std::string>());
                }
                value = vec;
            } else {
                // For other types, convert to string
                value = it->second.as<std::string>();
            }
            
            set(key, value, ConfigSource::File);
        }
        return true;
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG("YAML parsing failed: " + std::string(e.what()));
        return false;
    }
}

// Helper function to parse key=value format
bool Config::parseKeyValue(const std::string& content) {
    try {
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line)) {
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
                
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    } catch (const std::exception& e) {
        NLM_THROW_CONFIG("Key-value parsing failed: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm
