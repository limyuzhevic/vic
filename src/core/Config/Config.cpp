#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string filepath;  // For round-trip preservation
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    pImpl->filepath = filepath;
    
    // Determine file format from extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return loadFromYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        return loadFromSimple(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Recursive function to convert JSON to Config entries
        std::function<void(const nlohmann::json&, std::string)> convertJSON = 
            [&](const nlohmann::json& json, const std::string& prefix) {
                if (json.is_object()) {
                    for (auto it = json.begin(); it != json.end(); ++it) {
                        std::string key = prefix + "." + it.key();
                        convertJSON(it.value(), key);
                    }
                } else if (json.is_array()) {
                    for (size_t i = 0; i < json.size(); ++i) {
                        std::string key = prefix + "[" + std::to_string(i) + "]";
                        convertJSON(json[i], key);
                    }
                } else {
                    std::string valueStr;
                    ConfigSource source = ConfigSource::File;
                    
                    if (json.is_string()) {
                        valueStr = json.get<std::string>();
                    } else if (json.is_number_integer()) {
                        int64_t val = json.get<int64_t>();
                        set(prefix, val, source);
                        return;
                    } else if (json.is_number_unsigned()) {
                        uint64_t val = json.get<uint64_t>();
                        set(prefix, static_cast<int64_t>(val), source);
                        return;
                    } else if (json.is_number_float()) {
                        double val = json.get<double>();
                        set(prefix, val, source);
                        return;
                    } else if (json.is_boolean()) {
                        valueStr = json.get<bool>() ? "true" : "false";
                    } else if (json.is_null()) {
                        valueStr = "null";
                    }
                    
                    set(prefix, valueStr, source);
                }
            };
        
        convertJSON(jsonData, "");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Recursive function to convert YAML to Config entries
        std::function<void(const YAML::Node&, std::string)> convertYAML = 
            [&](const YAML::Node& node, const std::string& prefix) {
                if (node.IsMap()) {
                    for (auto it = node.begin(); it != node.end(); ++it) {
                        std::string key = prefix + "." + it->first.as<std::string>();
                        convertYAML(*it, key);
                    }
                } else if (node.IsSequence()) {
                    for (size_t i = 0; i < node.size(); ++i) {
                        std::string key = prefix + "[" + std::to_string(i) + "]";
                        convertYAML(node[i], key);
                    }
                } else {
                    ConfigSource source = ConfigSource::File;
                    
                    if (node.IsScalar()) {
                        std::string value = node.as<std::string>();
                        set(prefix, value, source);
                    }
                }
            };
        
        convertYAML(yamlData, "");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse YAML file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromSimple(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    pImpl->entries.clear();
    
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
    pImpl->filepath = filepath;
    
    // Determine file format from extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return saveToJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return saveToYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        return saveToSimple(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    try {
        nlohmann::json jsonData;
        
        // Convert Config entries to JSON
        for (const auto& entry : pImpl->entries) {
            setJSONEntry(jsonData, entry.key, entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Pretty print JSON
        file << std::setw(2) << jsonData;
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to write JSON file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath) const {
    try {
        YAML::Emitter emitter;
        
        // Convert Config entries to YAML
        bool first = true;
        for (const auto& entry : pImpl->entries) {
            if (!first) emitter << YAML::Key << entry.key;
            else first = false;
            
            // Convert ConfigValue to YAML scalar
            std::visit([&emitter](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    emitter << YAML::Value << static_cast<int64_t>(arg);
                } else if constexpr (std::is_same_v<T, double>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    emitter << YAML::Value << arg;
                }
            }, entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << emitter.c_str();
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to write YAML file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::saveToSimple(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert ConfigValue to string
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n";
    }
    
    return true;
}

void Config::setJSONEntry(nlohmann::json& json, const std::string& key, const ConfigValue& value) const {
    // Split key by dots for nested structure
    size_t dotPos = key.find('.');
    if (dotPos == std::string::npos) {
        // Leaf node
        setJSONLeaf(json, key, value);
    } else {
        std::string prefix = key.substr(0, dotPos);
        std::string suffix = key.substr(dotPos + 1);
        
        if (!json.contains(prefix)) {
            json[prefix] = nlohmann::json::object();
        }
        
        setJSONEntry(json[prefix], suffix, value);
    }
}

void Config::setJSONLeaf(nlohmann::json& json, const std::string& key, const ConfigValue& value) const {
    std::visit([&json, &key](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            json[key] = static_cast<int64_t>(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json[key] = arg;
        }
    }, value);
}

std::string Config::getFilepath() const {
    return pImpl->filepath;
}

void Config::setDescription(const std::string& key, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        it->description = description;
    }
}
