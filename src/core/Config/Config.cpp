#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
}};

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file type based on extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    try {
        if (ext == ".json") {
            return loadFromJSON(filepath);
        } else if (ext == ".yaml" || ext == ".yml") {
            return loadFromYAML(filepath);
        } else {
            // Try JSON first, then YAML, then fall back to simple format for backward compatibility
            try {
                return loadFromJSON(filepath);
            } catch (...) {
                try {
                    return loadFromYAML(filepath);
                } catch (...) {
                    // Fall back to simple key=value format
                    return loadFromSimpleFormat(filepath);
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Helper method for loading JSON files
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Process JSON object
        if (j.is_object()) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                ConfigSource source = ConfigSource::File;
                
                // Handle different JSON value types
                if (it.value().is_string()) {
                    set(key, it.value().get<std::string>(), source);
                } else if (it.value().is_number_integer()) {
                    set(key, it.value().get<int>(), source);
                } else if (it.value().is_number_unsigned()) {
                    set(key, it.value().get<uint64_t>(), source);
                } else if (it.value().is_number_float()) {
                    set(key, it.value().get<double>(), source);
                } else if (it.value().is_boolean()) {
                    set(key, it.value().get<bool>(), source);
                } else if (it.value().is_array()) {
                    // For arrays, convert to vector based on element type
                    if (!it.value().empty()) {
                        if (it.value()[0].is_string()) {
                            std::vector<std::string> vec = it.value().get<std::vector<std::string>>();
                            set(key, vec, source);
                        } else if (it.value()[0].is_number_integer()) {
                            std::vector<int> vec = it.value().get<std::vector<int>>();
                            set(key, vec, source);
                        } else if (it.value()[0].is_number_float()) {
                            std::vector<double> vec = it.value().get<std::vector<double>>();
                            set(key, vec, source);
                        } else if (it.value()[0].is_boolean()) {
                            std::vector<bool> vec = it.value().get<std::vector<bool>>();
                            set(key, vec, source);
                        } else {
                            NLM_LOG_WARNING("Unsupported array element type in JSON config for key: " + key);
                        }
                    }
                } else {
                    NLM_LOG_WARNING("Unsupported JSON type for key " + key);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error in config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Helper method for loading YAML files
bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Process YAML map
        if (doc.IsMap()) {
            for (const auto& pair : doc) {
                std::string key = pair.first.as<std::string>();
                ConfigSource source = ConfigSource::File;
                
                // Handle different YAML value types
                if (pair.second.IsScalar()) {
                    // Try to determine type
                    std::string valueStr = pair.second.as<std::string>();
                    // Try boolean
                    if (valueStr == "true" || valueStr == "false") {
                        set(key, valueStr == "true", source);
                    } else if (valueStr.find('.') != std::string::npos || valueStr.find('e') != std::string::npos || valueStr.find('E') != std::string::npos) {
                        // Try double
                        try {
                            double val = std::stod(valueStr);
                            set(key, val, source);
                        } catch (...) {
                            // Fall back to string
                            set(key, valueStr, source);
                        }
                    } else {
                        // Try integer
                        try {
                            int val = std::stoi(valueStr);
                            set(key, val, source);
                        } catch (...) {
                            set(key, valueStr, source);
                        }
                    }
                } else if (pair.second.IsSequence()) {
                    // Handle sequences (arrays)
                    std::vector<std::string> vec;
                    for (const auto& item : pair.second) {
                        vec.push_back(item.as<std::string>());
                    }
                    set(key, vec, source);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("YAML parsing error in config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Helper method for simple key=value format (backward compatibility)
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

// Save configuration to JSON file
bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json j;
        
        // Process entries into JSON object
        for (const auto& entry : pImpl->entries) {
            if (std::holds_alternative<std::string>(entry.value)) {
                j[entry.key] = std::get<std::string>(entry.value);
            } else if (std::holds_alternative<int>(entry.value)) {
                j[entry.key] = std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                j[entry.key] = std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                j[entry.key] = std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                j[entry.key] = std::get<bool>(entry.value);
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                j[entry.key] = std::get<std::vector<std::string>>(entry.value);
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                j[entry.key] = std::get<std::vector<int>>(entry.value);
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                j[entry.key] = std::get<std::vector<double>>(entry.value);
            }
        }
        
        file << j.dump(2); // Pretty print with 2-space indentation
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON save error in config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Save configuration to YAML file
bool Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            
            if (std::holds_alternative<std::string>(entry.value)) {
                emitter << std::get<std::string>(entry.value);
            } else if (std::holds_alternative<int>(entry.value)) {
                emitter << std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                emitter << static_cast<int64_t>(std::get<int64_t>(entry.value));
            } else if (std::holds_alternative<double>(entry.value)) {
                emitter << std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                emitter << (std::get<bool>(entry.value) ? "true" : "false");
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                emitter << YAML::Flow << std::get<std::vector<std::string>>(entry.value);
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                emitter << YAML::Flow << std::get<std::vector<int>>(entry.value);
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                emitter << YAML::Flow << std::get<std::vector<double>>(entry.value);
            }
        }
        
        emitter << YAML::EndMap;
        file << emitter.c_str();
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("YAML save error in config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Override saveToFile to use JSON format
bool Config::saveToFile(const std::string& filepath) const {
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return saveToJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return saveToYAML(filepath);
    } else {
        // Default to JSON format
        return saveToJSON(filepath);
    }
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