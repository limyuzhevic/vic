#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read the entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Skip empty files
    if (content.empty()) {
        return false;
    }
    
    // Determine file extension
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    Config::toLower(extension);
    
    bool parsedSuccessfully = false;
    
    // Try to parse as JSON
    if (extension == ".json") {
        if (parseJSON(content, pImpl->entries)) {
            parsedSuccessfully = true;
        }
    }
    
    // Try to parse as YAML
    if (!parsedSuccessfully && (extension == ".yaml" || extension == ".yml")) {
        if (parseYAML(content, pImpl->entries)) {
            parsedSuccessfully = true;
        }
    }
    
    // If JSON/YAML parsing failed, fall back to simple key=value format
    if (!parsedSuccessfully) {
        // Reopen the file for fallback parsing
        std::ifstream file2(filepath);
        if (!file2.is_open()) {
            return false;
        }
        
        // Reset entries
        pImpl->entries.clear();
        
        std::string line;
        while (std::getline(file2, line)) {
            // Skip empty lines and comments
            line = Config::trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = Config::trim(line.substr(0, pos));
                std::string value = Config::trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
    }
    
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
    
    // Determine file extension to decide format
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    Config::toLower(extension);
    
    // Use JSON format for .json files, otherwise use the serialized JSON format
    std::string content;
    if (extension == ".json") {
        content = serializeToJSON(pImpl->entries);
    } else {
        // For other files, still use JSON format for better interoperability
        content = serializeToJSON(pImpl->entries);
    }
    
    file << content;
    
    return true;
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
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
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

// Helper method to convert string to ConfigValue
static ConfigValue stringToConfigValue(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (...) {}
    
    try {
        return static_cast<int64_t>(std::stoll(str));
    } catch (...) {}
    
    try {
        return std::stod(str);
    } catch (...) {}
    
    std::string lower = Config::toLower(str);
    if (lower == "true" || lower == "yes" || lower == "1" || lower == "on") {
        return true;
    } else if (lower == "false" || lower == "no" || lower == "0" || lower == "off") {
        return false;
    }
    
    return str;
}

static nlohmann::json configValueToJSON(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            nlohmann::json jsonArray = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonArray.push_back(item);
            }
            return jsonArray;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json jsonArray = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonArray.push_back(item);
            }
            return jsonArray;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json jsonArray = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonArray.push_back(item);
            }
            return jsonArray;
        }
    }, value);
}

static ConfigValue jsonToConfigValue(const nlohmann::json& jsonValue) {
    if (jsonValue.is_boolean()) {
        return jsonValue.get<bool>();
    } else if (jsonValue.is_number_integer()) {
        return jsonValue.get<int64_t>();
    } else if (jsonValue.is_number()) {
        return jsonValue.get<double>();
    } else if (jsonValue.is_string()) {
        return jsonValue.get<std::string>();
    } else if (jsonValue.is_array()) {
        if (!jsonValue.empty()) {
            const nlohmann::json& firstElement = jsonValue[0];
            if (firstElement.is_number_integer()) {
                std::vector<int> intArray;
                for (const auto& item : jsonValue) {
                    intArray.push_back(item.get<int>());
                }
                return intArray;
            } else if (firstElement.is_number()) {
                std::vector<double> doubleArray;
                for (const auto& item : jsonValue) {
                    doubleArray.push_back(item.get<double>());
                }
                return doubleArray;
            } else {
                std::vector<std::string> stringArray;
                for (const auto& item : jsonValue) {
                    stringArray.push_back(item.get<std::string>());
                }
                return stringArray;
            }
        }
        return std::vector<int>{};
    }
    return std::string();
}

static bool jsonObjectToConfigEntry(const nlohmann::json& jsonObj, ConfigEntry& entry) {
    if (!jsonObj.contains("key") || !jsonObj.contains("value")) {
        return false;
    }
    
    entry.key = jsonObj["key"].get<std::string>();
    
    if (jsonObj.contains("value")) {
        entry.value = jsonToConfigValue(jsonObj["value"]);
    }
    
    if (jsonObj.contains("source")) {
        std::string sourceStr = jsonObj["source"].get<std::string>();
        if (sourceStr == "Default") {
            entry.source = ConfigSource::Default;
        } else if (sourceStr == "File") {
            entry.source = ConfigSource::File;
        } else if (sourceStr == "CommandLine") {
            entry.source = ConfigSource::CommandLine;
        } else if (sourceStr == "Runtime") {
            entry.source = ConfigSource::Runtime;
        }
    }
    
    if (jsonObj.contains("description")) {
        entry.description = jsonObj["description"].get<std::string>();
    }
    
    return true;
}

static bool parseJSON(const std::string& content, std::vector<ConfigEntry>& entries) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(content);
        
        if (jsonData.is_array()) {
            for (const auto& item : jsonData) {
                ConfigEntry entry;
                if (jsonObjectToConfigEntry(item, entry)) {
                    entries.push_back(entry);
                } else if (item.is_object() && item.contains("key") && item.contains("value")) {
                    ConfigEntry simpleEntry;
                    simpleEntry.key = item["key"].get<std::string>();
                    simpleEntry.value = jsonToConfigValue(item["value"]);
                    simpleEntry.source = ConfigSource::File;
                    entries.push_back(simpleEntry);
                }
            }
        } else if (jsonData.is_object()) {
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                ConfigEntry entry;
                entry.key = it.key();
                entry.value = jsonToConfigValue(it.value());
                entry.source = ConfigSource::File;
                entries.push_back(entry);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    }
}

static bool parseYAML(const std::string& content, std::vector<ConfigEntry>& entries) {
    try {
        YAML::Node yamlData = YAML::Load(content);
        
        if (yamlData.IsSequence()) {
            for (const auto& item : yamlData) {
                ConfigEntry entry;
                if (!jsonObjectToConfigEntry(nlohmann::json::parse(item.as<std::string>()), entry)) {
                    if (item.IsMap() && item["key"] && item["value"]) {
                        entry.key = item["key"].as<std::string>();
                        
                        if (item["value"].IsScalar()) {
                            entry.value = stringToConfigValue(item["value"].as<std::string>());
                        } else {
                            nlohmann::json jsonValue = nlohmann::json::parse(item["value"].as<std::string>());
                            entry.value = jsonToConfigValue(jsonValue);
                        }
                        
                        entry.source = ConfigSource::File;
                        entries.push_back(entry);
                    }
                }
            }
        } else if (yamlData.IsMap()) {
            for (const auto& item : yamlData) {
                ConfigEntry entry;
                entry.key = item.first.as<std::string>();
                
                if (item.second.IsScalar()) {
                    entry.value = stringToConfigValue(item.second.as<std::string>());
                } else {
                    nlohmann::json jsonValue = nlohmann::json::parse(item.second.as<std::string>());
                    entry.value = jsonToConfigValue(jsonValue);
                }
                
                entry.source = ConfigSource::File;
                entries.push_back(entry);
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

static std::string serializeToJSON(const std::vector<ConfigEntry>& entries) {
    nlohmann::json jsonData = nlohmann::json::array();
    
    for (const auto& entry : entries) {
        nlohmann::json jsonEntry;
        jsonEntry["key"] = entry.key;
        jsonEntry["value"] = configValueToJSON(entry.value);
        
        std::string sourceStr;
        switch (entry.source) {
            case ConfigSource::Default:
                sourceStr = "Default";
                break;
            case ConfigSource::File:
                sourceStr = "File";
                break;
            case ConfigSource::CommandLine:
                sourceStr = "CommandLine";
                break;
            case ConfigSource::Runtime:
                sourceStr = "Runtime";
                break;
        }
        jsonEntry["source"] = sourceStr;
        
        if (!entry.description.empty()) {
            jsonEntry["description"] = entry.description;
        }
        
        jsonData.push_back(jsonEntry);
    }
    
    return jsonData.dump(2);
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
