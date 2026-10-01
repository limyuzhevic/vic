// NLM Configuration System
// Phase 2: Enhanced Configuration with JSON/YAML Support
// 
// This implementation provides:
// 1. Proper configuration file parsing (JSON/YAML support)
// 2. Type-safe configuration values
// 3. Error handling and validation
// 4. Configuration serialization
// 5. Command-line argument support
// 6. Configuration summary and debugging

// NLM Configuration System
// Phase 2: Enhanced Configuration with JSON/YAML Support
// 
// This implementation provides:
// 1. Proper configuration file parsing (JSON/YAML support)
// 2. Type-safe configuration values
// 3. Error handling and validation
// 4. Configuration serialization
// 5. Command-line argument support
// 6. Configuration summary and debugging

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <chrono>
#include <json/json.h>
#include <yaml-cpp/yaml.h>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// JSON configuration parser
class JSONConfigParser {
public:
    static bool parse(const std::string& content, Config& config) {
        try {
            Json::Value root;
            Json::CharReaderBuilder readerBuilder;
            std::string errors;
            
            std::istringstream s(content);
            if (!Json::parseFromStream(readerBuilder, s, root, &errors)) {
                NLM_LOG_ERROR("JSON parsing error: " + errors);
                return false;
            }
            
            parseJSONValue(root, "", config);
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("JSON parser exception: " + std::string(e.what()));
            return false;
        }
    }
    
private:
    static void parseJSONValue(const Json::Value& value, const std::string& prefix, Config& config) {
        if (value.isObject()) {
            for (const auto& member : value.getMemberNames()) {
                std::string key = prefix.empty() ? member : prefix + "." + member;
                parseJSONValue(value[member], key, config);
            }
        } else if (value.isArray()) {
            for (size_t i = 0; i < value.size(); ++i) {
                std::string key = prefix + "[]";
                parseJSONValue(value[i], key, config);
            }
        } else {
            // Convert JSON type to Config type
            ConfigValue configValue;
            if (value.isBool()) {
                configValue = value.asBool();
            } else if (value.isInt()) {
                configValue = value.asInt();
            } else if (value.isUInt()) {
                configValue = static_cast<int64_t>(value.asUInt64());
            } else if (value.isDouble()) {
                configValue = value.asDouble();
            } else if (value.isString()) {
                configValue = value.asString();
            } else {
                configValue = "null";
            }
            
            std::string key = prefix.empty() ? "unknown" : prefix;
            config.set(key, configValue, ConfigSource::File);
        }
    }
};

// YAML configuration parser
class YAMLConfigParser {
public:
    static bool parse(const std::string& content, Config& config) {
        try {
            YAML::Node root = YAML::Load(content);
            parseYAMLValue(root, "", config);
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("YAML parser exception: " + std::string(e.what()));
            return false;
        }
    }
    
private:
    static void parseYAMLValue(const YAML::Node& node, const std::string& prefix, Config& config) {
        if (node.IsMap()) {
            for (const auto& yamlPair : node) {
                std::string key = prefix.empty() ? yamlPair.first.as<std::string>() : prefix + "." + yamlPair.first.as<std::string>();
                parseYAMLValue(yamlPair.second, key, config);
            }
        } else if (node.IsSequence()) {
            for (size_t i = 0; i < node.size(); ++i) {
                std::string key = prefix + "[]";
                parseYAMLValue(node[i], key, config);
            }
        } else {
            // Convert YAML type to Config type
            ConfigValue configValue;
            if (node.IsScalar()) {
                std::string scalar = node.as<std::string>();
                // Try to detect type
                if (scalar == "true" || scalar == "false") {
                    configValue = (scalar == "true");
                } else {
                    // Try numeric conversion
                    try {
                        if (scalar.find('.') != std::string::npos) {
                            configValue = std::stod(scalar);
                        } else {
                            configValue = std::stoi(scalar);
                        }
                    } catch (...) {
                        configValue = scalar;
                    }
                }
            } else {
                configValue = "null";
            }
            
            std::string key = prefix.empty() ? "unknown" : prefix;
            config.set(key, configValue, ConfigSource::File);
        }
    }
};

// Helper function to detect file type
FileType detectFileType(const std::string& content) {
    if (content.empty()) return FileType::Unknown;
    
    // Check for JSON
    if (content.find('{') != std::string::npos || 
        content.find("[") != std::string::npos ||
        content.find("\"\\\"") != std::string::npos) {
        return FileType::JSON;
    }
    
    // Check for YAML
    if (content.find(": ") != std::string::npos ||
        content.find("-") != std::string::npos) {
        return FileType::YAML;
    }
    
    return FileType::Unknown;
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_WARNING("Config file does not exist: " + filepath);
        return false;
    }
    
    // Read file content
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    if (content.empty()) {
        NLM_LOG_WARNING("Config file is empty: " + filepath);
        return true; // Empty file is valid
    }
    
    // Detect file type
    FileType fileType = detectFileType(content);
    
    // Parse based on file type
    bool success = false;
    if (fileType == FileType::JSON) {
        success = JSONConfigParser::parse(content, *this);
    } else if (fileType == FileType::YAML) {
        success = YAMLConfigParser::parse(content, *this);
    } else {
        // Try legacy key=value format
        success = loadFromLegacyFormat(content);
    }
    
    if (!success) {
        NLM_LOG_ERROR("Failed to parse config file: " + filepath);
    }
    
    return success;
}

bool Config::loadFromLegacyFormat(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
        line = trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line (no '=') at line " + std::to_string(lineNumber) + ": " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Try to parse as different types
        bool parsed = false;
        
        // Try int
        try {
            size_t posEnd;
            int intVal = std::stoi(value, &posEnd);
            if (posEnd == value.size()) {
                set(key, intVal, ConfigSource::File);
                parsed = true;
            }
        } catch (...) {}
        
        // Try double
        if (!parsed) {
            try {
                size_t posEnd;
                double doubleVal = std::stod(value, &posEnd);
                if (posEnd == value.size()) {
                    set(key, doubleVal, ConfigSource::File);
                    parsed = true;
                }
            } catch (...) {}
        }
        
        // Try bool
        if (!parsed) {
            if (value == "true" || value == "1") {
                set(key, true, ConfigSource::File);
                parsed = true;
            } else if (value == "false" || value == "0") {
                set(key, false, ConfigSource::File);
                parsed = true;
            }
        }
        
        // Store as string if not parsed
        if (!parsed) {
            set(key, value, ConfigSource::File);
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
    
    // Write header comment
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM on " << getCurrentTimestamp() << "\n";
    file << "# Source: " << (pImpl->entries.empty() ? "none" : std::to_string(pImpl->entries.size()) + " entries") << "\n";
    file << "\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Write the actual value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::fixed << std::setprecision(6) << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << escapeString(arg[i]) << "\"";
                }
                file << "]";
            } else {
                file << "<unknown_type>";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    file.flush();
    if (!file.good()) {
        NLM_LOG_ERROR("Error writing to config file: " + filepath);
        file.close();
        return false;
    }
    
    NLM_LOG_INFO("Config saved successfully to: " + filepath);
    return true;
}

// Implementation of helper functions
std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Config::escapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\0': result += "\\\\0"; break;
            default:
                if (c < 32 || c == 127) {
                    result += "\\\\x";
                    result += "0123456789ABCDEF"[(c >> 4) & 0xF];
                    result += "0123456789ABCDEF"[ c & 0xF];
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

// Additional helper functions
void Config::validateConfig() const {
    // Validate configuration consistency
    for (const auto& entry : pImpl->entries) {
        validateConfigEntry(entry);
    }
}

void Config::validateConfigEntry(const ConfigEntry& entry) const {
    // Validate key format
    if (entry.key.empty()) {
        NLM_LOG_WARNING("Empty config key found");
    }
    
    // Validate value type consistency
    if (entry.description.empty()) {
        NLM_LOG_WARNING("Config entry has no description: " + entry.key);
    }
}

std::string Config::generateDefaultConfig() const {
    std::ostringstream oss;
    oss << "# Default NLM Configuration\n";
    oss << "# This file contains configuration options for the Neural Learning Machine\n";
    oss << "# Use key=value format for simple configurations\n";
    oss << "\n";
    oss << "# Core simulation parameters\n";
    oss << "neuron_count = 1000\n";
    oss << "region_count = 1\n";
    oss << "simulation_timestep = 0.001\n";
    oss << "max_steps = 10000\n";
    oss << "\n";
    oss << "# Network connectivity\n";
    oss << "connection_probability = 0.1\n";
    oss << "\n";
    oss << "# Plasticity parameters\n";
    oss << "stdp_ltp_weight = 0.01\n";
    oss << "stdp_ltd_weight = 0.012\n";
    oss << "stdp_tau = 20.0\n";
    oss << "hebbian_learning_rate = 0.001\n";
    oss << "\n";
    oss << "# Structural plasticity\n";
    oss << "synaptogenesis_rate = 0.0001\n";
    oss << "pruning_rate = 0.00001\n";
    oss << "\n";
    oss << "# Development parameters\n";
    oss << "development_enabled = true\n";
    oss << "maturation_age = 60.0\n";
    oss << "plastic_receptor_decay = 0.95\n";
    
    return oss.str();
}

// Template instantiations for common types
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