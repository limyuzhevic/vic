#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Implement proper JSON/YAML parser based on file extension
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    
    // Convert to lowercase for extension check
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Fallback to simple key=value format for backward compatibility
        return loadFromSimpleFormat(filepath);
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

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR(std::string("Failed to open JSON config file: ") + filepath);
        return false;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        // Recursively flatten JSON to our key=value format
        flattenJSON("".c_str(), j);
        
        NLM_LOG_INFO(std::string("Successfully loaded JSON config: ") + filepath);
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR(std::string("JSON parse error in ") + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const nlohmann::json::type_error& e) {
        NLM_LOG_ERROR(std::string("JSON type error in ") + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const nlohmann::json::out_of_range& e) {
        NLM_LOG_ERROR(std::string("JSON out of range error in ") + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to parse JSON config ") + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        std::string ext = std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == ".yaml" || ext == ".yml") {
            // For YAML files, we'll also flatten to key=value format for compatibility
            // In a real implementation, we would use a YAML parser like YAML-cpp
            // For now, fallback to simple format
            return loadFromSimpleFormat(filepath);
        }
        
        return loadFromSimpleFormat(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to parse YAML config: ") + e.what());
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
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
        } else {
            NLM_LOG_WARNING(std::string("Skipping malformed line ") + 
                           std::to_string(lineNumber) + ": " + line);
        }
    }
    
    return true;
}

void Config::flattenJSON(const char* prefix, const nlohmann::json& j) {
    if (prefix == "") {
        for (auto& [key, value] : j.items()) {
            std::string fullKey = key;
            flattenJSON(fullKey.c_str(), value);
        }
    } else {
        std::string currentPrefix = std::string(prefix);
        for (auto& [key, value] : j.items()) {
            std::string fullKey = currentPrefix + "." + key;
            
            if (value.is_object()) {
                flattenJSON(fullKey.c_str(), value);
            } else if (value.is_array()) {
                // Convert array to JSON string representation
                set(fullKey, value.dump(), ConfigSource::File);
            } else {
                // Handle basic types
                if (value.is_string()) {
                    set(fullKey, value.get<std::string>(), ConfigSource::File);
                } else if (value.is_number_integer()) {
                    set(fullKey, value.get<int64_t>(), ConfigSource::File);
                } else if (value.is_number_float()) {
                    set(fullKey, value.get<double>(), ConfigSource::File);
                } else if (value.is_boolean()) {
                    set(fullKey, value.get<bool>(), ConfigSource::File);
                } else {
                    // Unknown type, convert to string
                    set(fullKey, value.dump(), ConfigSource::File);
                }
            }
        }
    }
}

} // namespace nlm
