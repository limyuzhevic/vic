#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <cmath>
#include <iostream>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Implement proper JSON parser
    // Parse JSON files with key-value pairs
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Simple JSON parsing: parse key-value pairs in {"key": "value", "num": 123, "bool": true}
    size_t pos = content.find('{');
    if (pos == std::string::npos) {
        return false; // Not a valid JSON object
    }
    
    pos++; // Skip '{'
    size_t end = content.find('}', pos);
    if (end == std::string::npos) {
        return false; // No closing '}'
    }
    
    std::string jsonContent = content.substr(pos, end - pos);
    
    // Parse key-value pairs
    size_t keyStart = 0;
    while (keyStart < jsonContent.length()) {
        // Skip whitespace
        while (keyStart < jsonContent.length() && isWhitespace(jsonContent[keyStart])) {
            keyStart++;
        }
        
        if (keyStart >= jsonContent.length()) break;
        
        // Parse key (string)
        if (jsonContent[keyStart] != '"') {
            break; // Malformed JSON
        }
        
        size_t keyEnd = jsonContent.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) {
            break;
        }
        
        std::string key = jsonContent.substr(keyStart + 1, keyEnd - keyStart - 1);
        keyStart = keyEnd + 1; // Skip closing "
        
        // Skip whitespace
        while (keyStart < jsonContent.length() && isWhitespace(jsonContent[keyStart])) {
            keyStart++;
        }
        
        if (keyStart >= jsonContent.length() || jsonContent[keyStart] != ':') {
            break;
        }
        
        keyStart++; // Skip ':'
        
        // Skip whitespace
        while (keyStart < jsonContent.length() && isWhitespace(jsonContent[keyStart])) {
            keyStart++;
        }
        
        if (keyStart >= jsonContent.length()) {
            break;
        }
        
        // Parse value
        ConfigValue value;
        size_t valueEnd;
        
        if (jsonContent[keyStart] == '"') {
            // String value
            size_t quotePos = jsonContent.find('"', keyStart + 1);
            if (quotePos == std::string::npos) {
                break;
            }
            value = jsonContent.substr(keyStart + 1, quotePos - keyStart - 1);
            valueEnd = quotePos + 1;
        } else if (jsonContent[keyStart] == '{' || jsonContent[keyStart] == '[') {
            // Object or array - skip for now (not needed for basic config)
            break;
        } else if (jsonContent[keyStart] >= '0' && jsonContent[keyStart] <= '9' || jsonContent[keyStart] == '-') {
            // Number value
            size_t numEnd = keyStart;
            while (numEnd < jsonContent.length() && (isdigit(jsonContent[numEnd]) || jsonContent[numEnd] == '.' || jsonContent[numEnd] == '-' || jsonContent[numEnd] == 'e' || jsonContent[numEnd] == 'E')) {
                numEnd++;
            }
            value = std::stod(jsonContent.substr(keyStart, numEnd - keyStart));
            valueEnd = numEnd;
        } else if (jsonContent[keyStart] == 't' || jsonContent[keyStart] == 'f') {
            // Boolean value
            if (jsonContent.substr(keyStart, 4) == "true") {
                value = true;
                valueEnd = keyStart + 4;
            } else if (jsonContent.substr(keyStart, 5) == "false") {
                value = false;
                valueEnd = keyStart + 5;
            } else {
                break;
            }
        } else {
            break; // Unknown type
        }
        
        // Set the value
        set(key, value, ConfigSource::File);
        
        // Skip to next key
        keyStart = valueEnd;
        
        // Skip comma and whitespace
        while (keyStart < jsonContent.length() && (isWhitespace(jsonContent[keyStart]) || jsonContent[keyStart] == ',')) {
            keyStart++;
        }
    }
    
    return true;
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

// JSON parsing helper functions
    bool isWhitespace(char c);
    void skipWhitespace(const std::string& json, size_t& pos);
    std::string parseString(const std::string& json, size_t& pos);
    double parseNumber(const std::string& json, size_t& pos);
    bool parseBool(const std::string& json, size_t& pos);
    std::vector<ConfigValue> parseArray(const std::string& json, size_t& pos);
    std::string parseIdentifier(const std::string& json, size_t& pos);

// Helper function implementations

bool Config::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

void Config::skipWhitespace(const std::string& json, size_t& pos) {
    while (pos < json.length() && isWhitespace(json[pos])) {
        pos++;
    }
}

std::string Config::parseString(const std::string& json, size_t& pos) {
    std::string result;
    pos++; // Skip opening quote
    while (pos < json.length()) {
        if (json[pos] == '\\' && pos + 1 < json.length()) {
            // Handle escape sequences
            pos += 2;
            result.push_back(json[pos - 1]);
        } else if (json[pos] == '"') {
            pos++; // Skip closing quote
            return result;
        } else {
            result.push_back(json[pos]);
            pos++;
        }
    }
    return result; // Error
}

// Add other helper functions as needed...

// Template instantiations

} // namespace nlm
