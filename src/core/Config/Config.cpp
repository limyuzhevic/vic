#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <stdexcept>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
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
    // Try to load as JSON first
    if (loadFromJSON(filepath)) {
        return true;
    }
    
    // Fall back to YAML
    if (loadFromYAML(filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
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

// JSON parser implementation using RapidJSON
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    rapidjson::IStreamWrapper isw(file);
    rapidjson::Document doc;
    
    try {
        doc.ParseStream(isw);
    } catch (const std::exception& e) {
        return false;
    }
    
    if (!doc.IsObject()) {
        return false;
    }
    
    for (rapidjson::Value::MemberIterator it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
        std::string key = it->name.GetString();
        
        if (it->value.IsBool()) {
            set(key, it->value.GetBool(), ConfigSource::File);
        } else if (it->value.IsInt()) {
            set(key, it->value.GetInt(), ConfigSource::File);
        } else if (it->value.IsInt64()) {
            set(key, it->value.GetInt64(), ConfigSource::File);
        } else if (it->value.IsUint()) {
            set(key, static_cast<int64_t>(it->value.GetUint()), ConfigSource::File);
        } else if (it->value.IsUint64()) {
            set(key, static_cast<int64_t>(it->value.GetUint64()), ConfigSource::File);
        } else if (it->value.IsDouble()) {
            set(key, it->value.GetDouble(), ConfigSource::File);
        } else if (it->value.IsString()) {
            set(key, it->value.GetString(), ConfigSource::File);
        } else if (it->value.IsArray()) {
            // For arrays, convert to string representation for simplicity
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            it->value.Accept(writer);
            set(key, buffer.GetString(), ConfigSource::File);
        }
    }
    
    return true;
}

// YAML parser implementation using YAML-CPP
bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        if (!doc.IsMap()) {
            return false;
        }
        
        for (const auto& pair : doc) {
            std::string key = pair.first.as<std::string>();
            
            if (pair.second.IsScalar()) {
                std::string value = pair.second.as<std::string>();
                set(key, value, ConfigSource::File);
            } else if (pair.second.IsSequence()) {
                // For sequences, convert to string representation
                std::string value = "[";
                bool first = true;
                for (const auto& item : pair.second) {
                    if (!first) value += ", ";
                    value += item.as<std::string>();
                    first = false;
                }
                value += "]";
                set(key, value, ConfigSource::File);
            } else if (pair.second.IsBoolean()) {
                set(key, pair.second.as<bool>(), ConfigSource::File);
            } else if (pair.second.IsInteger()) {
                set(key, pair.second.as<int>(), ConfigSource::File);
            } else if (pair.second.IsFloat()) {
                set(key, pair.second.as<float>(), ConfigSource::File);
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

// Helper functions for JSON/YAML parsing
namespace {
    // Parse JSON value string into appropriate type
    ConfigValue parseJSONValue(const std::string& value) {
        // Try to parse as boolean
        if (value == "true") {
            return true;
        } else if (value == "false") {
            return false;
        }
        
        // Try to parse as integer
        try {
            size_t pos;
            long long_val = std::stoll(value, &pos);
            if (pos == value.size()) {
                return long_val;
            }
        } catch (...) {}
        
        // Try to parse as double
        try {
            size_t pos;
            double double_val = std::stod(value, &pos);
            if (pos == value.size()) {
                return double_val;
            }
        } catch (...) {}
        
        // As a string
        return value;
    }
    
    // Convert string to lower case for JSON parsing
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), 
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
}

// Save to file implementation
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check file extension for format
    std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json" || ext == "yml" || ext == "yaml") {
        // Save as JSON/YAML
        file << "{\n";
        bool first = true;
        for (const auto& entry : pImpl->entries) {
            if (!first) file << ",\n";
            first = false;
            
            file << "  \"" << entry.key << "\": ";
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else {
                    file << arg;
                }
            }, entry.value);
        }
        file << "\n}\n";
    } else {
        // Save as simple key=value format for backward compatibility
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            file << "\n\n";
        }
    }
    
    return true;
}

// Rest of the Config.cpp implementation...

} // namespace nlm
