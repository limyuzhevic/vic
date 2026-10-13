#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <variant>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".jsonc") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        return loadFromSimple(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Cannot open JSON config file: " + filepath);
            return false;
        }
        
        json j;
        file >> j;
        
        // Convert JSON to our config entries
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Try to determine value type
            if (it->is_string()) {
                set(key, it->get<std::string>(), ConfigSource::File);
            } else if (it->is_number_integer()) {
                set(key, it->get<int64_t>(), ConfigSource::File);
            } else if (it->is_number_unsigned()) {
                set(key, static_cast<int64_t>(it->get<uint64_t>()), ConfigSource::File);
            } else if (it->is_number_float()) {
                set(key, it->get<double>(), ConfigSource::File);
            } else if (it->is_boolean()) {
                set(key, it->get<bool>(), ConfigSource::File);
            } else if (it->is_array()) {
                // For arrays, convert to string representation
                std::string value = "[";
                for (size_t i = 0; i < it->size(); ++i) {
                    if (i > 0) value += ", ";
                    value += std::to_string((*it)[i].get<double>());
                }
                value += "]";
                set(key, value, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Unsupported JSON type for key " + key + ", skipping");
            }
        }
        
        return true;
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error in " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const json::type_error& e) {
        NLM_LOG_ERROR("JSON type error in " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error reading JSON config " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        // For now, fallback to simple parsing since we don't have YAML library
        // TODO: Add proper YAML support with yaml-cpp or similar
        NLM_LOG_WARNING("YAML config loading not yet implemented, falling back to simple format");
        return loadFromSimple(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error reading YAML config " + filepath + ": " + std::string(e.what()));
        return false;
    }
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
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}
