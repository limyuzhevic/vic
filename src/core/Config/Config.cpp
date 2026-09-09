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
    // Detect file format based on extension and content
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    // Try JSON parser first for .json extension
    if (ext == "json") {
        return loadFromJSON(filepath);
    }
    
    // Try YAML parser for .yaml/.yml extension  
    if (ext == "yaml" || ext == "yml") {
        return loadFromYAML(filepath);
    }
    
    // Default to simple key=value format for backward compatibility
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Simple JSON parser for basic key-value pairs
        // This is a minimal implementation - in production would use a full JSON library
        std::string content((std::istreambuf_iterator<char>(file)), 
                           std::istreambuf_iterator<char>());
        
        // Parse basic JSON format: {"key": "value", "nested": {"inner": "value"}}
        // This is a simplified parser for demonstration
        parseSimpleJSON(content, "");
        
        return true;
    } catch (const std::exception& e) {
        // If JSON parsing fails, return false to fallback
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // YAML parsing - for now, treat as key=value format
        // Full YAML support would require a YAML parser library
        return loadFromSimpleFormat(filepath);
    } catch (const std::exception& e) {
        return false;
    }
}

void Config::parseSimpleJSON(const std::string& jsonStr, const std::string& prefix) {
    // Very basic JSON parser for simple cases
    // In production, use a proper JSON library like nlohmann/json
    
    size_t pos = 0;
    while (pos < jsonStr.size()) {
        char c = jsonStr[pos];
        
        if (c == '{') {
            // Object - find matching '}'
            int depth = 1;
            size_t bracePos = pos + 1;
            while (bracePos < jsonStr.size() && depth > 0) {
                if (jsonStr[bracePos] == '{') depth++;
                else if (jsonStr[bracePos] == '}') depth--;
                bracePos++;
            }
            if (depth == 0) {
                std::string objectStr = jsonStr.substr(pos + 1, bracePos - pos - 2);
                parseSimpleJSONObject(objectStr, prefix);
                pos = bracePos;
            } else {
                break;
            }
        } else if (c == '"') {
            // String key - find the key and value
            size_t keyEnd = jsonStr.find('"', pos + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = jsonStr.substr(pos + 1, keyEnd - pos - 1);
            pos = keyEnd + 1;
            
            // Skip whitespace and colon
            while (pos < jsonStr.size() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\t')) pos++;
            if (pos >= jsonStr.size() || jsonStr[pos] != ':') break;
            pos++;
            
            // Skip whitespace
            while (pos < jsonStr.size() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\t')) pos++;
            
            // Parse value
            if (jsonStr[pos] == '"') {
                // String value
                size_t valueEnd = jsonStr.find('"', pos + 1);
                if (valueEnd == std::string::npos) break;
                std::string value = jsonStr.substr(pos + 1, valueEnd - pos - 1);
                set(prefix + key, value, ConfigSource::File);
                pos = valueEnd + 1;
            } else if (jsonStr[pos] == 't' || jsonStr[pos] == 'f') {
                // Boolean value
                std::string value;
                if (jsonStr.substr(pos, 5) == "true") {
                    value = "true";
                    pos += 4;
                } else if (jsonStr.substr(pos, 4) == "false") {
                    value = "false";
                    pos += 3;
                } else {
                    break;
                }
                set(prefix + key, value, ConfigSource::File);
            } else if (jsonStr[pos] == '{' || jsonStr[pos] == '[') {
                // Object or array - skip for now
                break;
            } else {
                // Number - parse digits
                size_t numEnd = pos;
                while (numEnd < jsonStr.size() && (std::isdigit(jsonStr[numEnd]) || jsonStr[numEnd] == '.' || jsonStr[numEnd] == '-')) {
                    numEnd++;
                }
                std::string value = jsonStr.substr(pos, numEnd - pos);
                set(prefix + key, value, ConfigSource::File);
                pos = numEnd;
            }
            
            // Skip whitespace and comma
            while (pos < jsonStr.size() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\t' || jsonStr[pos] == ',')) {
                if (jsonStr[pos] == ',') {
                    pos++;
                    while (pos < jsonStr.size() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\t')) pos++;
                } else {
                    pos++;
                }
            }
        } else {
            pos++;
        }
    }
}

void Config::parseSimpleJSONObject(const std::string& objectStr, const std::string& prefix) {
    size_t pos = 0;
    while (pos < objectStr.size()) {
        // Skip whitespace
        while (pos < objectStr.size() && (objectStr[pos] == ' ' || objectStr[pos] == '\t' || objectStr[pos] == '\n')) {
            pos++;
        }
        
        if (pos >= objectStr.size()) break;
        
        // Find next key
        if (objectStr[pos] != '"') break;
        
        size_t keyEnd = objectStr.find('"', pos + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = objectStr.substr(pos + 1, keyEnd - pos - 1);
        pos = keyEnd + 1;
        
        // Skip whitespace and colon
        while (pos < objectStr.size() && (objectStr[pos] == ' ' || objectStr[pos] == '\t')) pos++;
        if (pos >= objectStr.size() || objectStr[pos] != ':') break;
        pos++;
        
        // Skip whitespace
        while (pos < objectStr.size() && (objectStr[pos] == ' ' || objectStr[pos] == '\t')) pos++;
        
        // Parse value
        if (objectStr[pos] == '"') {
            // String value
            size_t valueEnd = objectStr.find('"', pos + 1);
            if (valueEnd == std::string::npos) break;
            std::string value = objectStr.substr(pos + 1, valueEnd - pos - 1);
            set(prefix + key, value, ConfigSource::File);
            pos = valueEnd + 1;
        } else if (objectStr[pos] == 't' || objectStr[pos] == 'f') {
            // Boolean value
            std::string value;
            if (objectStr.substr(pos, 5) == "true") {
                value = "true";
                pos += 4;
            } else if (objectStr.substr(pos, 4) == "false") {
                value = "false";
                pos += 3;
            } else {
                break;
            }
            set(prefix + key, value, ConfigSource::File);
        } else if (objectStr[pos] == '{') {
            // Nested object - find matching }
            int depth = 1;
            size_t bracePos = pos + 1;
            while (bracePos < objectStr.size() && depth > 0) {
                if (objectStr[bracePos] == '{') depth++;
                else if (objectStr[bracePos] == '}') depth--;
                bracePos++;
            }
            if (depth == 0) {
                std::string nestedStr = objectStr.substr(pos + 1, bracePos - pos - 2);
                parseSimpleJSONObject(nestedStr, prefix + key + ".");
                pos = bracePos;
            } else {
                break;
            }
        } else if (objectStr[pos] == '[') {
            // Array - skip for now
            break;
        } else {
            // Number - parse digits
            size_t numEnd = pos;
            while (numEnd < objectStr.size() && (std::isdigit(objectStr[numEnd]) || objectStr[numEnd] == '.' || objectStr[numEnd] == '-')) {
                numEnd++;
            }
            std::string value = objectStr.substr(pos, numEnd - pos);
            set(prefix + key, value, ConfigSource::File);
            pos = numEnd;
        }
        
        // Skip whitespace and comma
        while (pos < objectStr.size() && (objectStr[pos] == ' ' || objectStr[pos] == '\t' || objectStr[pos] == ',' || objectStr[pos] == '\n')) {
            if (objectStr[pos] == ',') {
                pos++;
                while (pos < objectStr.size() && (objectStr[pos] == ' ' || objectStr[pos] == '\t')) pos++;
            } else {
                pos++;
            }
        }
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
