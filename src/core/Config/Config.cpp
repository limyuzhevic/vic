// Config.cpp - Fixed configuration system addressing Phase 2 TODOs
#include "Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    Impl() = default;
};

// Simple JSON/YAML utilities
namespace json_utils {
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    bool parseJson(const std::string& jsonString, std::unordered_map<std::string, ConfigValue>& entries) {
        try {
            // Very basic JSON parser for simple key-value pairs
            if (jsonString.empty() || jsonString[0] != '{') {
                return false;
            }
            
            std::string::size_type pos = 1;
            while (pos < jsonString.size()) {
                // Skip whitespace
                while (pos < jsonString.size() && std::isspace(jsonString[pos])) pos++;
                
                if (pos >= jsonString.size()) break;
                
                // Check for closing brace
                if (jsonString[pos] == '}') {
                    break;
                }
                
                // Parse key (simple JSON)
                if (jsonString[pos] != '"') {
                    return false;
                }
                
                std::string::size_type keyStart = ++pos;
                std::string::size_type keyEnd = jsonString.find('"', keyStart);
                if (keyEnd == std::string::npos) return false;
                std::string key = jsonString.substr(keyStart, keyEnd - keyStart);
                pos = keyEnd + 1;
                
                // Skip whitespace
                while (pos < jsonString.size() && std::isspace(jsonString[pos])) pos++;
                
                // Expect colon
                if (pos >= jsonString.size() || jsonString[pos] != ':') {
                    return false;
                }
                pos++;
                
                // Skip whitespace
                while (pos < jsonString.size() && std::isspace(jsonString[pos])) pos++;
                
                // Parse value
                if (pos >= jsonString.size()) return false;
                
                if (jsonString[pos] == '"') {
                    // String value
                    std::string::size_type valStart = ++pos;
                    std::string::size_type valEnd = jsonString.find('"', valStart);
                    if (valEnd == std::string::npos) return false;
                    std::string value = jsonString.substr(valStart, valEnd - valStart);
                    entries[key] = value;
                    pos = valEnd + 1;
                } else if (jsonString[pos] == 't' || jsonString[pos] == 'f') {
                    // Boolean value
                    if (jsonString.substr(pos, 5) == "true") {
                        entries[key] = true;
                        pos += 4;
                    } else if (jsonString.substr(pos, 4) == "false") {
                        entries[key] = false;
                        pos += 3;
                    } else {
                        return false;
                    }
                } else if (jsonString[pos] == '-' || std::isdigit(jsonString[pos])) {
                    // Number value
                    std::string::size_type valStart = pos;
                    while (pos < jsonString.size() && (std::isdigit(jsonString[pos]) || jsonString[pos] == '.' || jsonString[pos] == '-')) {
                        pos++;
                    }
                    std::string valueStr = jsonString.substr(valStart, pos - valStart);
                    try {
                        size_t endpos;
                        int64_t intVal = std::stoll(valueStr, &endpos);
                        if (endpos == valueStr.size()) {
                            entries[key] = intVal;
                        } else {
                            double doubleVal = std::stod(valueStr, &endpos);
                            entries[key] = doubleVal;
                        }
                    } catch (const std::exception&) {
                        entries[key] = valueStr;
                    }
                }
                
                // Skip whitespace
                while (pos < jsonString.size() && std::isspace(jsonString[pos])) pos++;
                
                // Expect comma or closing brace
                if (pos < jsonString.size() && jsonString[pos] == ',') {
                    pos++;
                }
            }
            
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    bool parseYaml(const std::string& yamlString, std::unordered_map<std::string, ConfigValue>& entries) {
        try {
            std::istringstream stream(yamlString);
            std::string line;
            
            while (std::getline(stream, line)) {
                // Skip empty lines and comments
                line = trim(line);
                if (line.empty() || line[0] == '#') {
                    continue;
                }
                
                // Parse key=value pairs (simple YAML)
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
                    
                    entries[key] = value;
                }
            }
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    std::string toJson(const std::unordered_map<std::string, ConfigValue>& entries) {
        std::ostringstream oss;
        oss << "{\n";
        bool first = true;
        
        for (const auto& [key, value] : entries) {
            if (!first) oss << ",\n";
            first = false;
            
            oss << "  \"" << key << "\": ";
            std::visit([&oss](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << val << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (val ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> || 
                                   std::is_same_v<T, int64_t>) {
                    oss << val;
                } else if constexpr (std::is_same_v<T, double>) {
                    oss << std::fixed << std::setprecision(6) << val;
                }
            }, value);
        }
        
        oss << "\n}";
        return oss.str();
    }
    
    std::string toYaml(const std::unordered_map<std::string, ConfigValue>& entries) {
        std::ostringstream oss;
        bool first = true;
        
        for (const auto& [key, value] : entries) {
            if (!first) oss << "\n";
            first = false;
            
            oss << key << " = ";
            std::visit([&oss](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << val << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (val ? "true" : "false");
                } else {
                    oss << val;
                }
            }, value);
        }
        
        return oss.str();
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string extension = "." + path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Cannot open config file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    bool success = false;
    
    if (extension == ".json") {
        std::unordered_map<std::string, ConfigValue> entries;
        success = json_utils::parseJson(content, entries);
        if (success) {
            for (const auto& [key, value] : entries) {
                set(key, value, ConfigSource::File);
            }
        } else {
            NLM_LOG_ERROR("Failed to parse JSON config file: " + filepath);
        }
    } else if (extension == ".yaml" || extension == ".yml") {
        std::unordered_map<std::string, ConfigValue> entries;
        success = json_utils::parseYaml(content, entries);
        if (success) {
            for (const auto& [key, value] : entries) {
                set(key, value, ConfigSource::File);
            }
        } else {
            NLM_LOG_ERROR("Failed to parse YAML config file: " + filepath);
        }
    } else {
        file.open(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            ++lineNumber;
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Malformed config line " + std::to_string(lineNumber) + ": " + line);
            }
        }
        
        success = true;
    }
    
    if (success) {
        NLM_LOG_INFO("Loaded configuration from: " + filepath);
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    std::vector<std::pair<std::string, std::string>> args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                args.emplace_back(key, value);
            } else {
                NLM_LOG_WARNING("Invalid command line argument: " + arg);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            args.emplace_back(key, value);
        } else if (arg[0] == '-') {
            NLM_LOG_WARNING("Missing value for command line argument: " + arg);
        }
    }
    
    for (const auto& [key, valueStr] : args) {
        bool parsed = false;
        
        if (!parsed) {
            if (valueStr == "true" || valueStr == "false") {
                bool val = (valueStr == "true");
                set(key, val, ConfigSource::CommandLine);
                parsed = true;
                NLM_LOG_INFO("Config from command line: " + key + " = " + valueStr);
            }
        }
        
        if (!parsed) {
            try {
                size_t pos;
                int64_t val = std::stoll(valueStr, &pos);
                if (pos == valueStr.size()) {
                    set(key, val, ConfigSource::CommandLine);
                    parsed = true;
                    NLM_LOG_INFO("Config from command line: " + key + " = " + valueStr);
                }
            } catch (const std::exception&) {}
        }
        
        if (!parsed) {
            try {
                size_t pos;
                double val = std::stod(valueStr, &pos);
                if (pos == valueStr.size()) {
                    set(key, val, ConfigSource::CommandLine);
                    parsed = true;
                    NLM_LOG_INFO("Config from command line: " + key + " = " + valueStr);
                }
            } catch (const std::exception&) {}
        }
        
        if (!parsed) {
            set(key, valueStr, ConfigSource::CommandLine);
            NLM_LOG_INFO("Config from command line: " + key + " = \"" + valueStr + "\"");
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::filesystem::path path(filepath);
    std::string extension = "." + path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return saveAsJson(filepath);
    } else if (extension == ".yaml" || extension == ".yml") {
        return saveAsYaml(filepath);
    } else {
        return saveAsText(filepath);
    }
}

bool Config::saveAsJson(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot write config file: " + filepath);
        return false;
    }
    
    std::unordered_map<std::string, ConfigValue> entries;
    for (const auto& entry : pImpl->entries) {
        entries[entry.key] = entry.value;
    }
    
    std::string json = json_utils::toJson(entries);
    file << json;
    
    if (file.good()) {
        NLM_LOG_INFO("Saved configuration as JSON to: " + filepath);
        return true;
    } else {
        NLM_LOG_ERROR("Error writing config file: " + filepath);
        return false;
    }
}

bool Config::saveAsYaml(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot write config file: " + filepath);
        return false;
    }
    
    std::unordered_map<std::string, ConfigValue> entries;
    for (const auto& entry : pImpl->entries) {
        entries[entry.key] = entry.value;
    }
    
    std::string yaml = json_utils::toYaml(entries);
    file << yaml;
    
    if (file.good()) {
        NLM_LOG_INFO("Saved configuration as YAML to: " + filepath);
        return true;
    } else {
        NLM_LOG_ERROR("Error writing config file: " + filepath);
        return false;
    }
}

bool Config::saveAsText(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot write config file: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << getCurrentTimestamp() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << val << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (val ? "true" : "false") << "\n";
            } else {
                file << entry.key << " = " << val << "\n";
            }
        }, entry.value);
        file << "\n";
    }
    
    if (file.good()) {
        NLM_LOG_INFO("Saved configuration as text to: " + filepath);
        return true;
    } else {
        NLM_LOG_ERROR("Error writing config file: " + filepath);
        return false;
    }
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
        NLM_LOG_ERROR("Type mismatch for config key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        NLM_LOG_DEBUG("Using default for config key: " + key);
    }
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

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it);
        NLM_LOG_INFO("Removed config key: " + key);
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
    if (!pImpl->entries.empty()) {
        NLM_LOG_INFO("Cleared all configuration entries");
        pImpl->entries.clear();
    }
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    oss << "Generated: " << getCurrentTimestamp() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ") - " << entry.description << "\n";
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

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
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