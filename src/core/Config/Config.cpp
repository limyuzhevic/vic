#pragma once

#include "../core/Config/Config.hpp"
#include <nlohmann/json.hpp>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>

namespace nlm {

// Enhanced Config implementation with JSON support and advanced features
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> envOverrides;  // Environment variable overrides
    std::shared_ptr<struct json> jsonCache;  // Cached JSON for performance
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Load environment variable overrides
    loadEnvironmentOverrides();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first, fallback to simple format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check if file is JSON format
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0);
    
    bool isJSON = false;
    if (firstLine.find('{') != std::string::npos || 
        firstLine.find('[') != std::string::npos) {
        isJSON = true;
    }
    
    if (isJSON) {
        try {
            return loadFromJSON(filepath);
        } catch (const std::exception& e) {
            // Fall back to simple format
        }
    }
    
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    auto jsonData = json::parse(file);
    pImpl->jsonCache = std::make_shared<json>(jsonData);
    
    // Convert JSON to ConfigEntry format
    if (jsonData.is_object()) {
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            // Convert JSON value to ConfigValue
            if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_unsigned()) {
                value = it->get<uint64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_array()) {
                // Convert array to vector
                std::vector<double> doubleArray;
                for (const auto& item : it->items()) {
                    if (item.value().is_number()) {
                        doubleArray.push_back(item.value().get<double>());
                    }
                }
                value = doubleArray;
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
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
            
            // Try to infer type
            try {
                // Try integer
                size_t posEnd;
                int intVal = std::stoi(value, &posEnd);
                if (posEnd == value.size()) {
                    set(key, intVal, ConfigSource::File);
                    continue;
                }
            } catch (...) {}
            
            try {
                // Try double
                size_t posEnd;
                double doubleVal = std::stod(value, &posEnd);
                if (posEnd == value.size()) {
                    set(key, doubleVal, ConfigSource::File);
                    continue;
                }
            } catch (...) {}
            
            try {
                // Try boolean
                if (value == "true") {
                    set(key, true, ConfigSource::File);
                    continue;
                } else if (value == "false") {
                    set(key, false, ConfigSource::File);
                    continue;
                }
            } catch (...) {}
            
            // Default to string
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Check if we have JSON cache
    if (pImpl->jsonCache) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        file << pImpl->jsonCache->dump(2);
        return true;
    }
    
    // Save in simple format
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << entry.key << " = ";
        
        // Convert value to string
        if (std::holds_alternative<int>(entry.value)) {
            file << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            file << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<uint64_t>(entry.value)) {
            file << std::get<uint64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            file << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            file << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            file << "\"" << std::get<std::string>(entry.value) << "\"";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            const auto& vec = std::get<std::vector<int>>(entry.value);
            file << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) file << ", ";
                file << vec[i];
            }
            file << "]";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            const auto& vec = std::get<std::vector<double>>(entry.value);
            file << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) file << ", ";
                file << vec[i];
            }
            file << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            const auto& vec = std::get<std::vector<std::string>>(entry.value);
            file << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) file << ", ";
                file << "\"" << vec[i] << "\"";
            }
            file << "]";
        }
        
        file << "\n";
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value or --key value format
        if (arg.find("--") == 0) {
            std::string key, value;
            size_t eqPos = arg.find('=');
            
            if (eqPos != std::string::npos) {
                key = arg.substr(2, eqPos - 2);
                value = arg.substr(eqPos + 1);
            } else {
                key = arg.substr(2);
                if (i + 1 < argc) {
                    value = argv[++i];
                }
            }
            
            if (!key.empty()) {
                // Try to convert value to appropriate type
                try {
                    size_t posEnd;
                    int intVal = std::stoi(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, intVal, ConfigSource::CommandLine);
                        success = true;
                        continue;
                    }
                } catch (...) {}
                
                try {
                    size_t posEnd;
                    double doubleVal = std::stod(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, doubleVal, ConfigSource::CommandLine);
                        success = true;
                        continue;
                    }
                } catch (...) {}
                
                try {
                    if (value == "true") {
                        set(key, true, ConfigSource::CommandLine);
                        success = true;
                        continue;
                    } else if (value == "false") {
                        set(key, false, ConfigSource::CommandLine);
                        success = true;
                        continue;
                    }
                } catch (...) {}
                
                // Default to string
                set(key, value, ConfigSource::CommandLine);
                success = true;
            }
        }
    }
    
    return success;
}

void Config::loadEnvironmentOverrides() {
    // Check for NLM_ prefix environment variables
    for (const auto& envPair : std::filesystem::environment()) {
        std::string envKey = envPair.first;
        std::string envValue = envPair.second;
        
        if (envKey.find("NLM_") == 0) {
            std::string configKey = envKey.substr(4); // Remove NLM_ prefix
            pImpl->envOverrides[configKey] = envValue;
        }
    }
}

void Config::applyEnvironmentOverrides() {
    // Apply environment variable overrides (higher priority than file)
    for (const auto& override : pImpl->envOverrides) {
        // Try to parse as different types
        bool applied = false;
        
        // Try integer
        try {
            size_t posEnd;
            int64_t intVal = std::stoll(override.second, &posEnd);
            if (posEnd == override.second.size()) {
                set(override.first, intVal, ConfigSource::Runtime);
                applied = true;
            }
        } catch (...) {}
        
        // Try double
        if (!applied) {
            try {
                size_t posEnd;
                double doubleVal = std::stod(override.second, &posEnd);
                if (posEnd == override.second.size()) {
                    set(override.first, doubleVal, ConfigSource::Runtime);
                    applied = true;
                }
            } catch (...) {}
        }
        
        // Try boolean
        if (!applied) {
            if (override.second == "true") {
                set(override.first, true, ConfigSource::Runtime);
                applied = true;
            } else if (override.second == "false") {
                set(override.first, false, ConfigSource::Runtime);
                applied = true;
            }
        }
        
        // Default to string
        if (!applied) {
            set(override.first, override.second, ConfigSource::Runtime);
        }
    }
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

Config& Config::getGlobal() {
    static Config globalConfig;
    return globalConfig;
}

void Config::setGlobal(std::shared_ptr<Config> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

} // namespace nlm