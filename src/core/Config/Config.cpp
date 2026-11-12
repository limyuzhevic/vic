#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

// Include for logging
#include "../Logger/Logger.hpp"

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, size_t> keyToIndex;  // O(1) lookup
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to convert YAML node to ConfigValue
ConfigValue yamlNodeToConfigValue(const YAML::Node& node) {
    if (node.IsScalar()) {
        const std::string& value = node.as<std::string>();
        // Try to infer type
        if (value == "true" || value == "false") {
            return value == "true";
        }
        try {
            return std::stoll(value);
        } catch (...) {
            try {
                return std::stod(value);
            } catch (...) {
                return value;
            }
        }
    } else if (node.IsSequence()) {
        std::vector<double> doubleVec;
        for (const auto& item : node) {
            if (item.IsScalar()) {
                try {
                    doubleVec.push_back(item.as<double>());
                } catch (...) {
                    doubleVec.push_back(0.0);
                }
            }
        }
        return doubleVec;
    } else if (node.IsMap()) {
        // For complex objects, we'll just stringify them
        std::string jsonStr;
        YAML::Emitter emitter;
        emitter << node;
        jsonStr = emitter.c_str();
        return jsonStr;
    }
    return 0.0;
}

// Helper function to convert ConfigValue to JSON node
void configValueToJsonNode(const ConfigValue& value, nlohmann::json& node) {
    std::visit([&node](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>) {
            node = val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            node = val;
        } else if constexpr (std::is_same_v<T, double>) {
            node = val;
        } else if constexpr (std::is_same_v<T, bool>) {
            node = val;
        } else if constexpr (std::is_same_v<T, std::string>) {
            node = val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            node = val;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            node = val;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            node = val;
        }
    }, value);
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }

    // Determine file type by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    bool success = false;
    
    if (ext == ".json" || ext == ".cfg") {
        try {
            // Try JSON first
            auto jsonData = nlohmann::json::parse(file);
            success = loadFromJson(jsonData);
        } catch (const nlohmann::json::parse_error& e) {
            NLM_LOG_ERROR("JSON parse error in " + filepath + ": " + std::string(e.what()));
            // Try as YAML if JSON fails
            file.clear();
            file.seekg(0);
            try {
                auto yamlData = YAML::Load(file);
                success = loadFromYaml(yamlData);
            } catch (const YAML::ParserException& e) {
                NLM_LOG_ERROR("YAML parse error in " + filepath + ": " + std::string(e.what()));
                success = false;
            }
        }
    } else if (ext == ".yaml" || ext == ".yml") {
        try {
            auto yamlData = YAML::Load(file);
            success = loadFromYaml(yamlData);
        } catch (const YAML::ParserException& e) {
            NLM_LOG_ERROR("YAML parse error in " + filepath + ": " + std::string(e.what()));
            success = false;
        }
    } else {
        // Try generic format with hints
        std::string line;
        bool isJsonLike = false;
        bool isYamlLike = false;
        
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') continue;
            
            if (line.find('{') != std::string::npos || line.find('}') != std::string::npos) {
                isJsonLike = true;
            }
            if (line.find(':') != std::string::npos && (line.find(' ') == std::string::npos || line.find('=') != std::string::npos)) {
                isYamlLike = true;
            }
        }
        
        file.clear();
        file.seekg(0);
        
        if (isJsonLike) {
            try {
                auto jsonData = nlohmann::json::parse(file);
                success = loadFromJson(jsonData);
            } catch (const nlohmann::json::parse_error& e) {
                NLM_LOG_ERROR("Parse error in " + filepath + ": " + std::string(e.what()));
                success = false;
            }
        } else {
            success = loadFromSimple(file);
        }
    }

    if (success) {
        NLM_LOG_INFO("Successfully loaded configuration from: " + filepath);
    }
    return success;
}

bool Config::loadFromJson(const nlohmann::json& data) {
    pImpl->entries.clear();
    pImpl->keyToIndex.clear();
    
    if (!data.is_object()) {
        NLM_LOG_ERROR("JSON root must be an object");
        return false;
    }
    
    for (auto it = data.begin(); it != data.end(); ++it) {
        std::string key = it.key();
        ConfigValue value = jsonToConfigValue(*it);
        pImpl->entries.emplace_back(key, value, ConfigSource::File);
        pImpl->keyToIndex[key] = pImpl->entries.size() - 1;
    }
    
    return true;
}

bool Config::loadFromYaml(const YAML::Node& data) {
    pImpl->entries.clear();
    pImpl->keyToIndex.clear();
    
    if (!data.IsMap()) {
        NLM_LOG_ERROR("YAML root must be a map");
        return false;
    }
    
    for (const auto& it : data) {
        std::string key = it.first.as<std::string>();
        ConfigValue value = yamlNodeToConfigValue(it.second);
        pImpl->entries.emplace_back(key, value, ConfigSource::File);
        pImpl->keyToIndex[key] = pImpl->entries.size() - 1;
    }
    
    return true;
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& data) {
    if (data.is_boolean()) {
        return data.get<bool>();
    } else if (data.is_number_integer()) {
        return data.get<int64_t>();
    } else if (data.is_number_float()) {
        return data.get<double>();
    } else if (data.is_string()) {
        return data.get<std::string>();
    } else if (data.is_array()) {
        std::vector<double> doubleVec;
        for (const auto& item : data) {
            if (item.is_number()) {
                doubleVec.push_back(item.get<double>());
            } else if (item.is_string()) {
                // Try to convert string to number
                try {
                    doubleVec.push_back(std::stod(item.get<std::string>()));
                } catch (...) {
                    doubleVec.push_back(0.0);
                }
            }
        }
        return doubleVec;
    }
    
    // Default to string representation
    return data.dump();
}

bool Config::loadFromSimple(std::ifstream& file) {
    pImpl->entries.clear();
    pImpl->keyToIndex.clear();
    
    std::string line;
    while (std::getline(file, line)) {
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
        } else if (line.find(':') != std::string::npos) {
            // YAML-like format
            size_t pos = line.find(':');
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    // Use JSON format for better compatibility
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        nlohmann::json value;
        configValueToJsonNode(entry.value, value);
        jsonData[entry.key] = value;
        
        // Add comment if description exists
        if (!entry.description.empty()) {
            // JSON doesn't have comment support, so we'll write it as a field
            std::string commentKey = "#" + entry.key;
            jsonData[commentKey] = entry.description;
        }
    }
    
    file << jsonData.dump(2);  // Pretty print with 2-space indentation
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        pImpl->entries[it->second].value = value;
        pImpl->entries[it->second].source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
        pImpl->keyToIndex[key] = pImpl->entries.size() - 1;
    }
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

bool Config::has(const std::string& key) const {
    return pImpl->keyToIndex.find(key) != pImpl->keyToIndex.end();
}

void Config::remove(const std::string& key) {
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        size_t index = it->second;
        pImpl->entries.erase(pImpl->entries.begin() + index);
        pImpl->keyToIndex.erase(it);
        
        // Update indices for all remaining entries
        for (auto& kv : pImpl->keyToIndex) {
            if (kv.second > index) {
                kv.second--;
            }
        }
    }
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
    pImpl->keyToIndex.clear();
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

} // namespace nlm