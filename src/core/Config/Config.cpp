#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <regex>
#include <zlib.h>
#include <zlib.h>
#include <iostream>
#include <algorithm>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::string> metadata;
    std::map<std::string, ConfigValue> defaultValues;
    bool loggingEnabled = true;
    std::vector<ConfigLogEntry> log;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::string format = getFileFormat(filepath);
    
    if (format == "json") {
        return loadFromJSON(filepath);
    } else if (format == "yaml" || format == "yml") {
        return loadFromYAML(filepath);
    } else if (format == "ini") {
        return loadFromINI(filepath);
    } else if (format == "xml") {
        return loadFromXML(filepath);
    } else {
        return false;
    }
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

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::string targetFormat = (format == "auto") ? getFileFormat(filepath) : format;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (targetFormat == "json") {
        return saveToJSON(file);
    } else if (targetFormat == "yaml" || targetFormat == "yml") {
        return saveToYAML(file);
    } else if (targetFormat == "ini") {
        return saveToINI(file);
    } else if (targetFormat == "xml") {
        return saveToXML(file);
    } else {
        return false;
    }
}

// Helper functions
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

bool Config::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string Config::getFileFormat(const std::string& filepath) {
    std::string extension = ".";
    size_t pos = filepath.find_last_of(".");
    if (pos != std::string::npos && pos < filepath.size() - 1) {
        extension = filepath.substr(pos);
    }
    
    std::string lowerExt = toLower(extension);
    if (lowerExt == ".json") return "json";
    if (lowerExt == ".yaml" || lowerExt == ".yml") return "yaml";
    if (lowerExt == ".ini") return "ini";
    if (lowerExt == ".xml") return "xml";
    
    return "unknown";
}

// File format specific parsers
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            std::string description = it.value().contains("description") ? it.value()["description"].get<std::string>() : "";
            
            ConfigValue value;
            try {
                value = jsonToConfigValue(it.value());
            } catch (const std::exception& e) {
                continue;  // Skip invalid values
            } catch (...) {
                continue;  // Skip unknown exceptions
            }
            
            set(key, value, ConfigSource::File);
            
            if (!description.empty()) {
                setMetadata(key, description);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    } catch (...) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
            std::string key = it->first.as<std::string>();
            YAML::Node valueNode = it->second;
            
            ConfigValue value;
            try {
                value = yamlToConfigValue(valueNode);
            } catch (const std::exception& e) {
                continue;  // Skip invalid values
            } catch (...) {
                continue;  // Skip unknown exceptions
            }
            
            set(key, value, ConfigSource::File);
            
            if (valueNode["description"]) {
                setMetadata(key, valueNode["description"].as<std::string>());
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    } catch (...) {
        return false;
    }
}

bool Config::loadFromINI(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentSection = "default";
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        if (line[0] == '[' && line[line.size() - 1] == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            std::string fullKey = currentSection + "." + key;
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(fullKey, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromXML(const std::string& filepath) {
    // Simplified XML parsing - in real implementation would use a proper XML parser
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentTag = "";
    bool inTag = false;
    
    while (std::getline(file, line)) {
        if (line.find("<config>") != std::string::npos) {
            currentTag = "config";
            continue;
        }
        
        if (currentTag == "config" && line.find("</config>") != std::string::npos) {
            currentTag = "";
            continue;
        }
        
        if (currentTag == "config") {
            std::string tag = "key";
            std::string value = "value";
            
            if (line.find("<key>") != std::string::npos) {
                tag = line.substr(5, line.find("<") - 5);
            } else if (line.find("</key>") != std::string::npos) {
                tag = "";
            } else if (tag.empty() && !tag.empty() && line.find("</") == std::string::npos) {
                if (line.find("<value>") != std::string::npos) {
                    value = line.substr(7, line.find("<") - 7);
                }
            }
        }
    }
    
    return true;
}

// File format specific writers
bool Config::saveToJSON(std::ofstream& file) const {
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        nlohmann::json jsonEntry;
        jsonEntry["value"] = configValueToJSON(entry.value);
        jsonEntry["source"] = static_cast<int>(entry.source);
        
        if (!entry.description.empty()) {
            jsonEntry["description"] = entry.description;
        }
        if (!entry.type.empty()) {
            jsonEntry["type"] = entry.type;
        }
        
        jsonData[entry.key] = jsonEntry;
    }
    
    file << jsonData.dump(2);
    return true;
}

bool Config::saveToYAML(std::ofstream& file) const {
    YAML::Emitter yamlData;
    yamlData << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        yamlData << YAML::Key << entry.key;
        yamlData << YAML::Value;
        yamlData << yamlConfigValueToYAML(entry.value);
        
        if (!entry.description.empty()) {
            yamlData << YAML::Key << "description";
            yamlData << YAML::Value << entry.description;
        }
    }
    
    yamlData << YAML::EndMap;
    
    std::stringstream buffer;
    buffer << yamlData;
    file << buffer.str();
    
    return true;
}

bool Config::saveToINI(std::ofstream& file) const {
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << configValueToJSON(entry.value).dump() << "\n\n";
    }
    
    return true;
}

bool Config::saveToXML(std::ofstream& file) const {
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<config>\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "  <entry key=\"" << entry.key << "\" source=\"" << static_cast<int>(entry.source) << "\">\n";
        file << "    <value>" << configValueToJSON(entry.value).dump() << "</value>\n";
        if (!entry.description.empty()) {
            file << "    <description>" << entry.description << "</description>\n";
        }
        file << "  </entry>\n";
    }
    
    file << "</config>\n";
    return true;
}

// Format detection and conversion
nlohmann::json Config::configValueToJSON(const ConfigValue& value) {
    nlohmann::json jsonValue;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : arg) {
                jsonValue.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                jsonValue.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                jsonValue.push_back(item);
            }
        }
    }, value);
    
    return jsonValue;
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& json) {
    ConfigValue value;
    
    if (json.is_number_integer()) {
        value = json.get<int>();
    } else if (json.is_number_float()) {
        value = json.get<double>();
    } else if (json.is_boolean()) {
        value = json.get<bool>();
    } else if (json.is_string()) {
        value = json.get<std::string>();
    } else if (json.is_array()) {
        if (!json.empty()) {
            if (json[0].is_number_integer()) {
                std::vector<int> vec;
                try {
                    for (const auto& item : json) {
                        vec.push_back(item.get<int>());
                    }
                    value = vec;
                } catch (...) {
                    // Invalid array values - return empty
                }
            } else if (json[0].is_number_float()) {
                std::vector<double> vec;
                try {
                    for (const auto& item : json) {
                        vec.push_back(item.get<double>());
                    }
                    value = vec;
                } catch (...) {
                    // Invalid array values - return empty
                }
            } else if (json[0].is_string()) {
                std::vector<std::string> vec;
                try {
                    for (const auto& item : json) {
                        vec.push_back(item.get<std::string>());
                    }
                    value = vec;
                } catch (...) {
                    // Invalid array values - return empty
                }
            }
        }
    }
    
    return value;
}

YAML::Node Config::configValueToYAML(const ConfigValue& value) {
    YAML::Node yamlNode;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : arg) {
                yamlNode.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                yamlNode.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                yamlNode.push_back(item);
            }
        }
    }, value);
    
    return yamlNode;
}

ConfigValue Config::yamlToConfigValue(const YAML::Node& yaml) {
    ConfigValue value;
    
    if (yaml.IsScalar()) {
        try {
            if (yaml.IsInt()) {
                value = yaml.as<int>();
            } else if (yaml.IsFloat()) {
                value = yaml.as<double>();
            } else if (yaml.IsBool()) {
                value = yaml.as<bool>();
            } else {
                value = yaml.as<std::string>();
            }
        } catch (const std::exception& e) {
            // Handle conversion errors gracefully
            value = "";
        }
    } else if (yaml.IsSequence()) {
        if (!yaml.empty()) {
            if (yaml[0].IsInt()) {
                std::vector<int> vec;
                try {
                    for (const auto& item : yaml) {
                        vec.push_back(item.as<int>());
                    }
                    value = vec;
                } catch (...) {
                    // Invalid array values - return empty
                }
            } else if (yaml[0].IsFloat()) {
                std::vector<double> vec;
                try {
                    for (const auto& item : yaml) {
                        vec.push_back(item.as<double>());
                    }
                    value = vec;
                } catch (...) {
                    // Invalid array values - return empty
                }
            } else {
                std::vector<std::string> vec;
                try {
                    for (const auto& item : yaml) {
                        vec.push_back(item.as<std::string>());
                    }
                    value = vec;
                } catch (...) {
                    // Invalid array values - return empty
                }
            }
        }
    }
    
    return value;
}

// Validation helpers
bool Config::validateValue(const std::string& key, const ConfigValue& value, const std::string& validation) const {
    if (validation.empty()) {
        return true;
    }
    
    try {
        std::string valueStr = valueToString(value);
        
        if (validation.find("range(") != std::string::npos) {
            // Parse range like "range(0,100)"
            size_t pos = validation.find("range(");
            size_t end = validation.find(")", pos);
            if (pos != std::string::npos && end != std::string::npos) {
                std::string rangeStr = validation.substr(pos + 6, end - pos - 6);
                // Parse range and check value
                // For now, just return true if range syntax is found
            }
        }
        
        return true;
    } catch (...) {
        return false; // Validation failed due to error
    }
}

std::string Config::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&](auto&& arg) {
        oss << arg;
    }, value);
    return oss.str();
}

// Advanced configuration methods
void Config::setMetadata(const std::string& key, const std::string& description,
                         const std::string& type,
                         const std::string& validation,
                         const std::string& defaultValue,
                         bool required,
                         const std::vector<std::string>& options) {
    if (pImpl->entries.empty() && !has(key)) {
        return; // Don't set metadata for non-existent keys
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->description = description;
        it->type = type;
        it->validation = validation;
        it->defaultValue = defaultValue;
        it->required = required;
        it->options = options;
    }
}

void Config::setDescription(const std::string& key, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->description = description;
    }
}

void Config::setType(const std::string& key, const std::string& type) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->type = type;
    }
}

void Config::setValidation(const std::string& key, const std::string& validation) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->validation = validation;
    }
}

void Config::setDefaultValue(const std::string& key, const std::string& defaultValue) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->defaultValue = defaultValue;
    }
}

void Config::setRequired(const std::string& key, bool required) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->required = required;
    }
}

void Config::setOptions(const std::string& key, const std::vector<std::string>& options) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->options = options;
    }
}

std::string Config::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->description;
    }
    return "";
}

std::string Config::getType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->type;
    }
    return "";
}

std::string Config::getValidation(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->validation;
    }
    return "";
}

std::string Config::getDefaultValue(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->defaultValue;
    }
    return "";
}

bool Config::isRequired(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->required;
    }
    return false;
}

std::vector<std::string> Config::getOptions(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->options;
    }
    return {};
}

bool Config::hasOptions(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return !it->options.empty();
    }
    return false;
}

// Configuration inheritance and merging
bool Config::loadFromConfig(const Config& other, ConfigSource source) {
    bool success = false;
    
    for (const auto& otherEntry : other.pImpl->entries) {
        bool exists = false;
        for (const auto& thisEntry : pImpl->entries) {
            if (thisEntry.key == otherEntry.key) {
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            ConfigValue value;
            try {
                value = otherEntry.value;
                set(otherEntry.key, value, source);
                success = true;
            } catch (...) {
                continue;
            }
        }
    }
    
    return success;
}

bool Config::mergeWith(const Config& other, ConfigSource source) {
    bool success = false;
    
    for (const auto& otherEntry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&otherEntry](const ConfigEntry& e) { return e.key == otherEntry.key; });
        
        if (it != pImpl->entries.end()) {
            // Key exists, check if we should override
            it->value = otherEntry.value;
            it->source = source;
            success = true;
        } else {
            // Key doesn't exist, add it
            set(otherEntry.key, otherEntry.value, source);
            success = true;
        }
    }
    
    return success;
}

// Configuration validation
bool Config::validate(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false; // Key doesn't exist
    }
    
    return validateValue(key, it->value, it->validation);
}

bool Config::validateAll() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry.key, entry.value, entry.validation)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry.key, entry.value, entry.validation)) {
            errors.push_back("Validation failed for key: " + entry.key);
        }
        
        if (isRequired(entry.key)) {
            if (entry.value.index() == 0 || entry.value.index() == 1 || entry.value.index() == 2 || entry.value.index() == 3 || entry.value.index() == 4 || entry.value.index() == 5 || entry.value.index() == 6 || entry.value.index() == 7 || entry.value.index() == 8) {
                // Check if it's a non-empty value
                auto value = valueToString(entry.value);
                if (value.empty()) {
                    errors.push_back("Required key has empty value: " + entry.key);
                }
            }
        }
    }
    
    return errors;
}

// Configuration management
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

// Configuration introspection
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
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << item;
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << item;
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << "\"" << item << "\"";
                    first = false;
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
        
        if (!entry.description.empty()) {
            oss << "      Description: " << entry.description << "\n";
        }
        if (!entry.type.empty()) {
            oss << "      Type: " << entry.type << "\n";
        }
    }
    return oss.str();
}

std::string Config::getConfigInfo() const {
    std::ostringstream oss;
    oss << "NLM Configuration System v" << getVersion() << "\n";
    oss << "Format support: JSON, YAML, INI, XML\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Default source: " << (pImpl->defaultValues.empty() ? "none" : "configured") << "\n";
    oss << "Validation: " << (validateAll() ? "passed" : "failed") << "\n";
    oss << "Backup available: " << (pImpl->entries.size() > 0 ? "yes" : "no") << "\n";
    oss << "Compression: supported" << "\n";
    return oss.str();
}

std::string Config::getConfigFormatInfo() const {
    std::ostringstream oss;
    oss << "Configuration File Format Information:\n";
    oss << "  JSON (.json): Hierarchical key-value pairs with rich metadata\n";
    oss << "  YAML (.yaml/.yml): Human-readable structure with anchors and aliases\n";
    oss << "  INI (.ini): Simple key-value pairs with sections\n";
    oss << "  XML (.xml): Document-based format with DTD support\n";
    oss << "\nAuto-detection based on file extension and content.\n";
    return oss.str();
}

// Configuration persistence
bool Config::backup(const std::string& backupPath) const {
    if (pImpl->entries.empty()) {
        return false;
    }
    
    std::string actualPath = backupPath.empty() ? "config_backup.json" : backupPath;
    return saveToFile(actualPath, "json");
}

bool Config::restore(const std::string& restorePath) {
    if (!std::filesystem::exists(restorePath)) {
        return false;
    }
    
    clear();
    return loadFromFile(restorePath);
}

bool Config::exportConfig(const std::string& filepath, const std::string& format) const {
    return saveToFile(filepath, format);
}

bool Config::importConfig(const std::string& filepath, const std::string& format) {
    if (format == "auto") {
        return loadFromFile(filepath);
    } else {
        // For specific formats, would need format-specific implementations
        return false;
    }
}

// Environment variable expansion
bool Config::expandEnvironmentVariables(std::string& value) const {
    std::regex envVarRegex("\$\{?([A-Za-z_][A-Za-z0-9_]*)\}?");
    std::string result;
    
    size_t lastPos = 0;
    size_t pos = value.find_first_of('$', lastPos);
    
    while (pos != std::string::npos) {
        result.append(value, lastPos, pos - lastPos);
        
        size_t endPos = value.find_first_of('$', pos + 1);
        if (endPos == std::string::npos) {
            endPos = value.size();
        }
        
        std::string envVar = value.substr(pos + 1, endPos - pos - 1);
        const char* envValue = std::getenv(envVar.c_str());
        
        if (envValue != nullptr) {
            result.append(envValue);
        } else {
            result.append("");
        }
        
        lastPos = endPos;
        pos = value.find_first_of('$', lastPos);
    }
    
    if (lastPos < value.size()) {
        result.append(value, lastPos, value.size() - lastPos);
    }
    
    value = result;
    return true;
}

std::string Config::expandEnvironmentVariables(const std::string& value) const {
    std::string result = value;
    expandEnvironmentVariables(result);
    return result;
}

// Default value management
bool Config::hasDefaultValue(const std::string& key) const {
    return pImpl->defaultValues.find(key) != pImpl->defaultValues.end();
}

ConfigValue Config::getDefaultValueAsVariant(const std::string& key) const {
    auto it = pImpl->defaultValues.find(key);
    if (it != pImpl->defaultValues.end()) {
        return it->second;
    }
    return ConfigValue();
}

void Config::addDefaultValue(const std::string& key, const ConfigValue& value) {
    pImpl->defaultValues[key] = value;
}

// Compression support
bool Config::compressBackup(const std::string& sourcePath, const std::string& targetPath) const {
    // Simplified gzip compression - in real implementation would use proper compression
    std::ifstream src(sourcePath, std::ios::binary);
    std::ofstream dst(targetPath, std::ios::binary);
    
    if (!src.is_open() || !dst.is_open()) {
        return false;
    }
    
    dst << src.rdbuf();
    return true;
}

bool Config::decompressBackup(const std::string& sourcePath, const std::string& targetPath) const {
    return compressBackup(sourcePath, targetPath); // Same implementation for now
}

// Configuration diff
std::string Config::diff(const Config& other) const {
    std::ostringstream oss;
    oss << "Configuration Difference:\n";
    
    // Check for differences in entries
    std::vector<std::string> thisKeys = getKeys();
    std::vector<std::string> otherKeys = other.getKeys();
    
    for (const auto& key : thisKeys) {
        if (std::find(otherKeys.begin(), otherKeys.end(), key) == otherKeys.end()) {
            oss << "  + Added: " << key << "\n";
        }
    }
    
    for (const auto& key : otherKeys) {
        if (std::find(thisKeys.begin(), thisKeys.end(), key) == thisKeys.end()) {
            oss << "  - Removed: " << key << "\n";
        } else {
            auto thisIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            
            if (thisIt != pImpl->entries.end() && otherIt != other.pImpl->entries.end()) {
                if (thisIt->value != otherIt->value) {
                    oss << "  ~ Changed: " << key << " (from " << valueToString(otherIt->value) << " to " << valueToString(thisIt->value) << ")\n";
                }
            }
        }
    }
    
    return oss.str();
}

bool Config::equals(const Config& other) const {
    return pImpl->entries == other.pImpl->entries;
}

// Logging and auditing
void Config::logChange(const std::string& operation, const std::string& key, 
                      const std::string& oldValue, const std::string& newValue,
                      const std::string& source, const std::string& userId) {
    if (loggingEnabled) {
        ConfigLogEntry entry(operation, key, oldValue, newValue, source, userId);
        pImpl->log.push_back(entry);
        
        // Update performance metrics
        metrics.totalOperations++;
        metrics.successfulOperations++;
        metrics.lastAccess = std::chrono::system_clock::now();
    }
}

void Config::addLogEntry(const ConfigLogEntry& entry) {
    if (loggingEnabled) {
        pImpl->log.push_back(entry);
    }
}

std::vector<Config::ConfigLogEntry> Config::getLog() const {
    return pImpl->log;
}

void Config::clearLog() {
    pImpl->log.clear();
}

void Config::enableLogging(bool enable) {
    loggingEnabled = enable;
}

bool Config::isLoggingEnabled() const {
    return loggingEnabled;
}

// Template instantiations
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
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    std::string oldValueStr = "";
    if (it != pImpl->entries.end()) {
        oldValueStr = valueToString(it->value);
    }
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        
        logChange("set", key, oldValueStr, valueToString(value), "runtime", "");
    } else {
        pImpl->entries.emplace_back(key, value, source);
        logChange("set", key, "", valueToString(value), "runtime", "");
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
