#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Implement proper JSON/YAML parser with automatic format detection
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Detect file format by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Try to detect format by content
        std::string firstLine;
        if (std::getline(file, firstLine)) {
            file.clear();
            file.seekg(0);
            
            // Check if it looks like JSON (starts with { or [)
            std::string trimmedFirst = trim(firstLine);
            if (trimmedFirst.find('{') == 0 || trimmedFirst.find('[') == 0) {
                return loadFromJSON(filepath);
            } else {
                // Fall back to simple key=value format for backward compatibility
                file.clear();
                file.seekg(0);
                return loadFromSimpleFormat(filepath);
            }
        } else {
            return loadFromSimpleFormat(filepath);
        }
    }
}

// Config.cpp implementation

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Implement proper JSON/YAML parser with automatic format detection
bool Config::loadFromFile(const std::string& filepath) {
    // Implement proper JSON/YAML parser with automatic format detection
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Detect file format by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Try to detect format by content
        std::string firstLine;
        if (std::getline(file, firstLine)) {
            file.clear();
            file.seekg(0);
            
            // Check if it looks like JSON (starts with { or [)
            std::string trimmedFirst = trim(firstLine);
            if (trimmedFirst.find('{') == 0 || trimmedFirst.find('[') == 0) {
                return loadFromJSON(filepath);
            } else {
                // Fall back to simple key=value format for backward compatibility
                file.clear();
                file.seekg(0);
                return loadFromSimpleFormat(filepath);
            }
        } else {
            return loadFromSimpleFormat(filepath);
        }
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    // Use nlohmann::json for JSON parsing
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json json;
        file >> json;
        
        clear();
        
        // Recursively parse JSON and convert to Config entries
        parseJson(json, "");
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    // For now, fall back to simple format or implement YAML parsing
    // TODO: Implement proper YAML parsing using a YAML library
    return loadFromSimpleFormat(filepath);
}

std::string Config::saveToJSON() const {
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        std::string fullKey = entry.key;
        // Parse nested key (e.g., "brain.neuron_count" -> "brain", "neuron_count")
        size_t dotPos = fullKey.find('.');
        if (dotPos != std::string::npos) {
            std::string parentKey = fullKey.substr(0, dotPos);
            std::string childKey = fullKey.substr(dotPos + 1);
            
            if (!json.contains(parentKey)) {
                json[parentKey] = nlohmann::json::object();
            }
            json[parentKey][childKey] = configValueToJson(entry.value);
        } else {
            json[entry.key] = configValueToJson(entry.value);
        }
    }
    
    std::stringstream ss;
    ss << std::setw(4) << json;
    return ss.str();
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
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

void Config::parseJson(const nlohmann::json& json, const std::string& prefix) {
    if (json.is_object()) {
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
            
            if (it->is_object() && !it->empty()) {
                // Nested object - could flatten or keep as is
                parseJson(*it, key);
            } else if (it->is_array()) {
                // Convert array to string representation
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < it->size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << (*it)[i].dump();
                }
                ss << "]";
                set(key, ss.str(), ConfigSource::File);
            } else {
                // Simple value
                set(key, it->dump(), ConfigSource::File);
            }
        }
    } else if (json.is_array()) {
        std::string key = prefix.empty() ? "array" : prefix + ".array";
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < json.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << json[i].dump();
        }
        ss << "]";
        set(key, ss.str(), ConfigSource::File);
    }
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) const {
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return nlohmann::json(arg);
        }
    }, value);
}

// Rest of the implementation...

// Stub implementations for missing functions
bool Config::loadFromYAML(const std::string& filepath) {
    return loadFromSimpleFormat(filepath);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
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
