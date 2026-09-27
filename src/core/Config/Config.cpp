#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <regex>
#include <iostream>
#include <functional>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<std::function<ValidationResult(const std::string&, const ConfigValue&)>> validators;
    std::mutex mutex;
    std::unordered_map<std::string, std::vector<std::string>> sections;
    std::unordered_map<std::string, bool> lockedKeys;
    bool autoSave;
    std::string saveFormat;
    
    Impl() : autoSave(false), saveFormat("json") {}
};

// Validation result structure
struct ValidationResult {
    bool isValid;
    std::string message;
    
    ValidationResult(bool valid = true, const std::string& msg = "") 
        : isValid(valid), message(msg) {}
};

// Environment variable substitution
std::string Config::substituteVariables(const std::string& str) {
    std::string result = str;
    // Pattern for ${VAR} or ${VAR:-default}
    std::regex pattern(R"(\$\{([^}]+)\:-?[^}]+\})");
    std::smatch matches;
    
    auto start = result.find("${");
    while (start != std::string::npos) {
        auto end = result.find("}", start);
        if (end == std::string::npos) break;
        
        std::string varExpr = result.substr(start + 2, end - start - 3);
        size_t colonPos = varExpr.find(":-");
        std::string varName;
        std::string defaultValue = "";
        
        if (colonPos != std::string::npos) {
            varName = varExpr.substr(0, colonPos);
            defaultValue = varExpr.substr(colonPos + 2);
        } else {
            varName = varExpr;
        }
        
        const char* envValue = std::getenv(varName.c_str());
        std::string replacement = envValue ? envValue : defaultValue;
        
        result.replace(start, end - start + 1, replacement);
        
        start = result.find("${", start + replacement.size());
    }
    
    return result;
}

bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        auto jsonData = nlohmann::json::parse(jsonStr);
        clear();
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            std::string valueStr = it.dump();
            
            // Try to determine type and set appropriately
            try {
                // Try bool
                bool boolVal = std::stoi(valueStr);
                set(key, boolVal, ConfigSource::File);
            } catch (...) {
                try {
                    // Try int
                    int intVal = std::stoi(valueStr);
                    set(key, intVal, ConfigSource::File);
                } catch (...) {
                    try {
                        // Try double
                        double doubleVal = std::stod(valueStr);
                        set(key, doubleVal, ConfigSource::File);
                    } catch (...) {
                        // Use string
                        set(key, valueStr.substr(1, valueStr.size() - 2), ConfigSource::File);
                    }
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& yamlStr) {
    try {
        auto yamlData = YAML::Load(yamlStr);
        clear();
        
        for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
            std::string key = it->first.as<std::string>();
            
            // Try to determine type
            if (it->IsScalar()) {
                std::string value = it->as<std::string>();
                
                // Try to parse as different types
                try {
                    bool boolVal = (value == "true" || value == "1");
                    set(key, boolVal, ConfigSource::File);
                } catch (...) {
                    try {
                        int intVal = std::stoi(value);
                        set(key, intVal, ConfigSource::File);
                    } catch (...) {
                        try {
                            double doubleVal = std::stod(value);
                            set(key, doubleVal, ConfigSource::File);
                        } catch (...) {
                            set(key, value, ConfigSource::File);
                        }
                    }
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromFile(const std::string& filepath, ConfigFileFormat format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        switch (format) {
            case ConfigFileFormat::JSON:
                return loadFromJSONStream(file);
            case ConfigFileFormat::YAML:
                return loadFromYAMLStream(file);
            default:
                return loadFromKeyValueStream(file);
        }
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath, ConfigFileFormat format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    switch (format) {
        case ConfigFileFormat::JSON:
            saveToJSONFile(file);
            break;
        case ConfigFileFormat::YAML:
            saveToYAMLFile(file);
            break;
        default:
            saveToKeyValueFile(file);
            break;
    }
    
    return true;
}

std::string Config::saveToString(ConfigFileFormat format) const {
    std::ostringstream oss;
    
    switch (format) {
        case ConfigFileFormat::JSON:
            saveToJSONStream(oss);
            break;
        case ConfigFileFormat::YAML:
            saveToYAMLStream(oss);
            break;
        default:
            saveToKeyValueStream(oss);
            break;
    }
    
    return oss.str();
}

std::vector<std::string> Config::getSections() const {
    std::vector<std::string> sections;
    for (const auto& entry : pImpl->entries) {
        size_t colonPos = entry.key.find("::");
        if (colonPos != std::string::npos) {
            std::string section = entry.key.substr(0, colonPos);
            if (std::find(sections.begin(), sections.end(), section) == sections.end()) {
                sections.push_back(section);
            }
        }
    }
    return sections;
}

std::vector<std::string> Config::getKeys(const std::string& section) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(section + "::") == 0) {
            size_t colonPos = entry.key.find("::", section.size() + 2);
            if (colonPos != std::string::npos) {
                keys.push_back(entry.key.substr(colonPos + 2));
            } else {
                keys.push_back(entry.key.substr(section.size() + 2));
            }
        }
    }
    return keys;
}

bool Config::setValidator(const std::function<ValidationResult(const std::string&, const ConfigValue&)>& validator) {
    pImpl->validators.push_back(validator);
    return true;
}

bool Config::setValidated(const std::string& key, const ConfigValue& value, ConfigSource source, 
                         const std::function<ValidationResult(const std::string&, const ConfigValue&)>& validator) {
    ValidationResult result = validator(key, value);
    if (!result.isValid) {
        return false;
    }
    set(key, value, source);
    return true;
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, entry.source);
    }
}

void Config::lock(const std::string& key, bool locked) {
    pImpl->lockedKeys[key] = locked;
}

bool Config::isLocked(const std::string& key) const {
    auto it = pImpl->lockedKeys.find(key);
    return it != pImpl->lockedKeys.end() && it->second;
}

void Config::setAutoSave(bool enable, const std::string& format) {
    pImpl->autoSave = enable;
    pImpl->saveFormat = format;
}

bool Config::isAutoSaveEnabled() const {
    return pImpl->autoSave;
}

std::string Config::getSaveFormat() const {
    return pImpl->saveFormat;
}

bool Config::hasSection(const std::string& section) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&section](const ConfigEntry& e) {
            return e.key.find(section + "::") == 0;
        });
}

ConfigValue Config::getSectionValue(const std::string& section, const std::string& key) const {
    std::string fullKey = section + "::" + key;
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&fullKey](const ConfigEntry& e) { return e.key == fullKey; });
    
    return (it != pImpl->entries.end()) ? it->value : ConfigValue();
}

void Config::removeSection(const std::string& section) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&section](const ConfigEntry& e) { return e.key.find(section + "::") == 0; }),
        pImpl->entries.end()
    );
    pImpl->sections.erase(section);
}

std::vector<ConfigEntry> Config::getSectionEntries(const std::string& section) const {
    std::vector<ConfigEntry> entries;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(section + "::") == 0) {
            entries.push_back(entry);
        }
    }
    return entries;
}

std::string Config::getSectionDescription(const std::string& section) const {
    auto sectionKeys = getKeys(section);
    if (sectionKeys.empty()) {
        return "";
    }
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(section + "::") == 0) {
            return entry.description;
        }
    }
    
    return "";
}

void Config::setSectionDescription(const std::string& section, const std::string& description) {
    auto sectionKeys = getKeys(section);
    if (!sectionKeys.empty()) {
        for (auto& entry : pImpl->entries) {
            if (entry.key.find(section + "::") == 0) {
                entry.description = description;
                break;
            }
        }
    }
}

void Config::saveToJSONFile(std::ofstream& file) const {
    json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&jsonData, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonData[entry.key] = arg;
            }
        }, entry.value);
    }
    
    file << jsonData.dump(2);
}

void Config::saveToJSONStream(std::ostream& stream) const {
    json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&jsonData, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonData[entry.key] = arg;
            }
        }, entry.value);
    }
    
    stream << jsonData.dump(2);
}

void Config::saveToYAMLFile(std::ofstream& file) const {
    YAML::Emitter emitter;
    emitter.SetIndent(2);
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        
        std::visit([&emitter](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, int>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                emitter << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                emitter << YAML::Flow << arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                emitter << YAML::Flow << arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                emitter << YAML::Flow << arg;
            }
        }, entry.value);
    }
    
    file << emitter.c_str();
}

void Config::saveToYAMLStream(std::ostream& stream) const {
    YAML::Emitter emitter;
    emitter.SetIndent(2);
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        
        std::visit([&emitter](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, int>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                emitter << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                emitter << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                emitter << YAML::Flow << arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                emitter << YAML::Flow << arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                emitter << YAML::Flow << arg;
            }
        }, entry.value);
    }
    
    stream << emitter.c_str();
}

void Config::saveToKeyValueFile(std::ofstream& file) const {
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
}

void Config::saveToKeyValueStream(std::ostream& stream) const {
    for (const auto& entry : pImpl->entries) {
        stream << "# " << entry.description << "\n";
        stream << entry.key << " = ";
        
        std::visit([&stream](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                stream << "\"" << arg << "\"";
            } else {
                stream << arg;
            }
        }, entry.value);
        
        stream << "\n\n";
    }
}

bool Config::loadFromJSONStream(std::istream& stream) {
    std::stringstream buffer;
    buffer << stream.rdbuf();
    std::string jsonStr = buffer.str();
    return loadFromJSON(jsonStr);
}

bool Config::loadFromYAMLStream(std::istream& stream) {
    std::stringstream buffer;
    buffer << stream.rdbuf();
    std::string yamlStr = buffer.str();
    return loadFromYAML(yamlStr);
}

bool Config::loadFromKeyValueStream(std::istream& stream) {
    std::string line;
    while (std::getline(stream, line)) {
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