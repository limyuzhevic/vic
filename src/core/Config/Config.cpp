#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Phase 2: Enhanced JSON configuration parser
    // Supports JSON configuration files with proper parsing
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    try {
        // Try to parse as JSON first
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Recursively process JSON object
        processJsonData(jsonData, ConfigSource::File);
        
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        // If JSON parsing fails, fall back to simple key=value format
        file.clear();
        file.seekg(0);
        
        NLM_LOG_INFO("JSON parsing failed, falling back to key=value format for: " + filepath);
        
        std::string line;
        int lineNum = 0;
        while (std::getline(file, line)) {
            lineNum++;
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
                NLM_LOG_INFO("  Parsed config line " + std::to_string(lineNum) + ": " + key + " = " + value);
            } else {
                NLM_LOG_WARNING("Skipping invalid config line " + std::to_string(lineNum) + ": " + line);
            }
        }
        
        if (lineNum > 0) {
            NLM_LOG_INFO("Loaded configuration from fallback format file: " + filepath + " (" + std::to_string(lineNum) + " lines)");
            return true;
        }
        
        return false;
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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    // Log the configuration change
    std::string valueStr;
    std::visit([&](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            valueStr = "\"" + val + "\"";
        } else {
            valueStr = std::to_string(val);
        }
    }, value);
    NLM_LOG_DEBUG("Config set: " + key + " = " + valueStr + " (source: " + std::to_string(static_cast<int>(source)) + ")");
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::processJsonData(const nlohmann::json& jsonData, ConfigSource source, const std::string& parentKey) {
    std::string fullKey = parentKey.empty() ? "" : parentKey + ".";
    
    if (jsonData.is_object()) {
        for (const auto& [key, value] : jsonData.items()) {
            processJsonData(value, source, fullKey + key);
        }
    } else if (jsonData.is_array()) {
        // For arrays, create a single entry with string representation
        std::string arrayStr = "["; 
        for (size_t i = 0; i < jsonData.size(); ++i) {
            if (i > 0) arrayStr += ", ";
            if (jsonData[i].is_string()) {
                arrayStr += "\"" + jsonData[i].get<std::string>() + "\"";
            } else if (jsonData[i].is_number()) {
                arrayStr += std::to_string(jsonData[i].get<double>());
            } else if (jsonData[i].is_boolean()) {
                arrayStr += jsonData[i].get<bool>() ? "true" : "false";
            } else {
                arrayStr += "null";
            }
        }
        arrayStr += "]";
        set(fullKey + "array", arrayStr, source);
    } else if (jsonData.is_string()) {
        set(fullKey, jsonData.get<std::string>(), source);
    } else if (jsonData.is_number()) {
        double value = jsonData.get<double>();
        if (value == static_cast<int>(value)) {
            set(fullKey, static_cast<int>(value), source);
        } else {
            set(fullKey, value, source);
        }
    } else if (jsonData.is_boolean()) {
        set(fullKey, jsonData.get<bool>(), source);
    } else {
        NLM_LOG_ERROR("Unsupported JSON type at key: " + fullKey);
    }
}

bool Config::validate(const std::string& key) const {
    if (!has(key)) {
        NLM_LOG_WARNING("Configuration key not found: " + key);
        return false;
    }
    return true;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
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
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
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