#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// nlohmann/json for JSON support
#include <nlohmann/json.hpp>

// yaml-cpp for YAML support
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
    // Enhanced implementation with format detection
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file format based on extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJSONFile(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAMLFile(filepath);
    } else {
        // Try backward compatible format first
        return loadFromLegacyFile(filepath);
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

bool Config::saveToFile(const std::string& filepath) const {
    // For backward compatibility, save to legacy format
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << getStringValue(entry.value) << "\n";
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "json") {
        return saveToJSONFile(filepath);
    } else if (format == "yaml" || format == "yml") {
        return saveToYAMLFile(filepath);
    } else {
        return saveToLegacyFile(filepath);
    }
}

bool Config::saveToJSONFile(const std::string& filepath) const {
    try {
        nlohmann::json jsonObj;
        
        for (const auto& entry : pImpl->entries) {
            jsonObj[entry.key] = entry.value;
        }
        
        std::ofstream file(filepath);
        file << jsonObj.dump(2);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToYAMLFile(const std::string& filepath) const {
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            emitConfigValue(emitter, entry.value);
        }
        
        emitter << YAML::EndMap;
        
        std::ofstream file(filepath);
        emitter.cstr();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void Config::emitConfigValue(YAML::Emitter& emitter, const ConfigValue& value) const {
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                      std::is_same_v<T, uint64_t> || std::is_same_v<T, float> || 
                      std::is_same_v<T, double>) {
            emitter << val;
        } else if constexpr (std::is_same_v<T, bool>) {
            emitter << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            emitter << YAML::DoubleQuoted << val;
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                             std::is_same_v<T, std::vector<double>> ||
                             std::is_same_v<T, std::vector<std::string>>) {
            emitter << YAML::BeginSeq;
            for (const auto& item : val) {
                emitConfigValue(emitter, item);
            }
            emitter << YAML::EndSeq;
        }
    }, value);
}

std::string Config::getStringValue(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&](const auto& val) {
        oss << val;
    }, value);
    return oss.str();
}

} // namespace nlm