#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> aliases;
    bool validateOnSet;
    bool strictTypes;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->validateOnSet = true;
    pImpl->strictTypes = false;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to determine file format from extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
        // Convert to lowercase for case-insensitive comparison
        std::transform(ext.begin(), ext.end(), ext.begin(), 
                       [](unsigned char c) { return std::tolower(c); });
    }
    
    if (ext == "json") {
        return loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return loadFromYAML(filepath);
    } else {
        // Fallback to simple format
        return loadFromSimple(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    // TODO: Implement JSON parsing when nlohmann/json is available
    return loadFromSimple(filepath);
}

bool Config::loadFromYAML(const std::string& filepath) {
    // TODO: Implement YAML parsing when yaml-cpp is available
    return loadFromSimple(filepath);
}

bool Config::loadFromSimple(const std::string& filepath) {
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Determine output format from extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), 
                       [](unsigned char c) { return std::tolower(c); });
    }
    
    if (ext == "json") {
        return saveToJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return saveToYAML(filepath);
    } else {
        return saveToSimple(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    // TODO: Implement JSON saving when nlohmann/json is available
    return saveToSimple(filepath);
}

bool Config::saveToYAML(const std::string& filepath) const {
    // TODO: Implement YAML saving when yaml-cpp is available
    return saveToSimple(filepath);
}

bool Config::saveToSimple(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert value to string based on type
        std::string valueStr;
        std::visit([&valueStr](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = "\"" + arg + "\"";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStr = arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                valueStr = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) valueStr += ", ";
                    valueStr += std::to_string(arg[i]);
                }
                valueStr += "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                valueStr = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) valueStr += ", ";
                    valueStr += std::to_string(arg[i]);
                }
                valueStr += "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                valueStr = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) valueStr += ", ";
                    valueStr += "\"" + arg[i] + "\"";
                }
                valueStr += "]";
            }
        }, entry.value);
        
        file << entry.key << " = " << valueStr << "\n";
    }
    
    return true;
}