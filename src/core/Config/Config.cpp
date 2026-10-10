#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <ctime>
#include <map>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// Simple YAML parser for basic structures
class SimpleYamlParser {
public:
    static bool parse(const std::string& yaml, std::map<std::string, std::string>& output) {
        std::istringstream stream(yaml);
        std::string line;
        std::string currentKey;
        
        while (std::getline(stream, line)) {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            if (line.empty() || line[0] == '#') {
                continue;  // Skip comments and empty lines
            }
            
            // Check if it's a key-value pair (simple format: key: value)
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos && colonPos > 0) {
                currentKey = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                // Trim the value
                value.erase(0, value.find_first_not_of(" \t"));
                output[currentKey] = value;
            }
        }
        return true;
    }
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Cache for fast lookups
    std::unordered_map<std::string, size_t> keyToIndex;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to parse JSON-like strings
static std::string parseJsonString(const std::string& str) {
    if (str.size() >= 2 && str.front() == '"' && str.back() == '"') {
        return str.substr(1, str.size() - 2);
    }
    return str;
}

// Helper to convert string to various types
static bool parseToInt(const std::string& str, int64_t& result) {
    try {
        size_t pos;
        result = std::stoll(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

static bool parseToDouble(const std::string& str, double& result) {
    try {
        size_t pos;
        result = std::stod(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

static bool parseToBool(const std::string& str, bool& result) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        result = true;
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        result = false;
        return true;
    }
    return false;
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Try to detect file format
    bool isJson = false;
    bool isYaml = false;
    
    // Simple detection
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        // Check if it's valid JSON
        try {
            json j = json::parse(content);
            isJson = true;
        } catch (...) {
            // Not JSON, try YAML
        }
    }
    
    if (!isJson) {
        // Try YAML parsing
        std::map<std::string, std::string> yamlMap;
        if (SimpleYamlParser::parse(content, yamlMap)) {
            isYaml = true;
            for (const auto& pair : yamlMap) {
                set(pair.first, pair.second, ConfigSource::File);
            }
            return true;
        }
    }
    
    if (isJson) {
        // Parse JSON
        json j;
        try {
            j = json::parse(content);
        } catch (const json::parse_error& e) {
            // Fallback to simple key=value format
            return loadFromSimpleFile(filepath);
        }
        
        // Recursively parse JSON to Config entries
        parseJsonToConfig(j, "");
        return true;
    }
    
    // Fallback to simple key=value format
    return loadFromSimpleFile(filepath);
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
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

void Config::parseJsonToConfig(const json& j, const std::string& prefix) {
    if (j.is_object()) {
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
            parseJsonToConfig(it.value(), key);
        }
    } else if (j.is_array()) {
        for (size_t i = 0; i < j.size(); ++i) {
            std::string key = prefix + "[" + std::to_string(i) + "]";
            parseJsonToConfig(j[i], key);
        }
    } else if (j.is_string()) {
        std::string value = j.get<std::string>();
        set(prefix, value, ConfigSource::File);
    } else if (j.is_number_integer()) {
        int64_t value = j.get<int64_t>();
        set(prefix, value, ConfigSource::File);
    } else if (j.is_number_float()) {
        double value = j.get<double>();
        set(prefix, value, ConfigSource::File);
    } else if (j.is_boolean()) {
        bool value = j.get<bool>();
        set(prefix, value, ConfigSource::File);
    } else if (j.is_null()) {
        set(prefix, "", ConfigSource::File);
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
    
    // Save in JSON format for better compatibility
    json j;
    
    for (const auto& entry : pImpl->entries) {
        // Convert key to JSON path format
        std::string jsonKey = entry.key;
        // Replace dots with nested structure if needed
        size_t dotPos = jsonKey.find('.');
        if (dotPos != std::string::npos) {
            // For now, just save as flat structure with dots
            j[jsonKey] = toJsonString(entry.value);
        } else {
            j[jsonKey] = toJsonString(entry.value);
        }
    }
    
    try {
        file << j.dump(2);  // Pretty print with 2-space indentation
    } catch (const json::exception& e) {
        // Fallback to simple format if JSON serialization fails
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << entryToString(entry.value) << "\n";
        }
    }
    
    return true;
}

std::string Config::toJsonString(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            // Convert to JSON array
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            // Convert to JSON array
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            // Convert to JSON array
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + arg[i] + "\"";
            }
            result += "]";
            return result;
        } else {
            return "null";  // Unknown type
        }
    }, value);
}

std::string Config::entryToString(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else {
            return std::to_string(arg);
        }
    }, value);
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        // Try to convert from string
        std::string keyStr = key;
        std::string valueStr;
        
        if (const auto* strValue = std::get_if<std::string>(&it->value)) {
            valueStr = *strValue;
            
            T result;
            if constexpr (std::is_same_v<T, int64_t>) {
                if (parseToInt(valueStr, result)) {
                    return result;
                }
            } else if constexpr (std::is_same_v<T, double>) {
                if (parseToDouble(valueStr, result)) {
                    return result;
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                if (parseToBool(valueStr, result)) {
                    return result;
                }
            } else if constexpr (std::is_same_v<T, std::string>) {
                return valueStr;
            }
        }
        
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Update cache
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        // Update existing entry
        pImpl->entries[it->second].value = value;
        pImpl->entries[it->second].source = source;
    } else {
        // Add new entry
        pImpl->entries.emplace_back(key, value, source);
        pImpl->keyToIndex[key] = pImpl->entries.size() - 1;
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
    return pImpl->keyToIndex.find(key) != pImpl->keyToIndex.end();
}

void Config::remove(const std::string& key) {
    auto it = pImpl->keyToIndex.find(key);
    if (it != pImpl->keyToIndex.end()) {
        size_t index = it->second;
        pImpl->entries.erase(pImpl->entries.begin() + index);
        pImpl->keyToIndex.erase(it);
        
        // Update indices in cache
        for (auto& pair : pImpl->keyToIndex) {
            if (pair.second > index) {
                pair.second--;
            }
        }
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
    pImpl->entries.clear();
    pImpl->keyToIndex.clear();
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
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
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