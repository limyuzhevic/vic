#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cctype>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// Simple JSON parsing helpers
namespace json {

// Trim whitespace from both ends
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Convert string to lowercase
static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Check if string is quoted
static bool isQuoted(const std::string& str) {
    return (str.size() >= 2 && 
           ((str.front() == '"' && str.back() == '"') ||
            (str.front() == '\'' && str.back() == '\'')));
}

// Remove quotes from string
static std::string unquote(const std::string& str) {
    if (isQuoted(str)) {
        return str.substr(1, str.size() - 2);
    }
    return str;
}

// Parse JSON string value
static ConfigValue parseJsonString(const std::string& str) {
    std::string value = unquote(str);
    
    // Try to detect number
    try {
        size_t pos;
        int intVal = std::stoi(value, &pos);
        if (pos == value.size()) return intVal;
        
        int64_t int64Val = std::stoll(value, &pos);
        if (pos == value.size()) return int64Val;
        
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) return doubleVal;
    } catch (...) {
        // Not a number, keep as string
    }
    
    // Try to detect boolean
    std::string lower = toLower(value);
    if (lower == "true") return true;
    if (lower == "false") return false;
    
    return value;
}

// Parse JSON array
static ConfigValue parseJsonArray(const std::string& str) {
    std::vector<ConfigValue> array;
    std::string content = unquote(str);
    size_t pos = 0;
    
    while (pos < content.size()) {
        content = trim(content);
        if (content.empty()) break;
        
        size_t elementEnd = content.find(',');
        if (elementEnd == std::string::npos) {
            elementEnd = content.size();
        }
        
        std::string elementStr = content.substr(0, elementEnd);
        array.push_back(parseJsonValue(trim(elementStr)));
        
        pos += elementEnd;
        if (pos < content.size()) {
            // Skip comma and any whitespace
            while (pos < content.size() && (content[pos] == ',' || std::isspace(content[pos]))) {
                pos++;
            }
        }
    }
    
    // Try to infer array type
    if (!array.empty()) {
        bool allInt = true;
        bool allDouble = true;
        bool allBool = true;
        bool allString = true;
        
        for (const auto& val : array) {
            if (std::holds_alternative<int>(val)) {
                allString = allDouble = allBool = false;
            } else if (std::holds_alternative<int64_t>(val)) {
                allString = allDouble = allBool = false;
            } else if (std::holds_alternative<double>(val)) {
                allString = allBool = false;
                allInt = false; // Since we have floats
            } else if (std::holds_alternative<bool>(val)) {
                allString = allDouble = allInt = false;
            } else if (std::holds_alternative<std::string>(val)) {
                allInt = allDouble = allBool = false;
            } else {
                // Unsupported type
                return array;
            }
        }
        
        if (allInt) {
            std::vector<int> intArray;
            intArray.reserve(array.size());
            for (const auto& val : array) {
                intArray.push_back(std::get<int>(val));
            }
            return intArray;
        } else if (allDouble) {
            std::vector<double> doubleArray;
            doubleArray.reserve(array.size());
            for (const auto& val : array) {
                doubleArray.push_back(std::get<double>(val));
            }
            return doubleArray;
        } else if (allBool) {
            std::vector<bool> boolArray;
            boolArray.reserve(array.size());
            for (const auto& val : array) {
                boolArray.push_back(std::get<bool>(val));
            }
            return boolArray;
        } else if (allString) {
            std::vector<std::string> stringArray;
            stringArray.reserve(array.size());
            for (const auto& val : array) {
                stringArray.push_back(std::get<std::string>(val));
            }
            return stringArray;
        }
    }
    
    return array;
}

// Parse JSON object
static ConfigValue parseJsonObject(const std::string& str) {
    std::vector<ConfigEntry> entries;
    std::string content = unquote(str);
    size_t pos = 0;
    
    while (pos < content.size()) {
        content = trim(content);
        if (content.empty()) break;
        
        // Find next key (look for "key": value)
        size_t colonPos = content.find(':');
        if (colonPos == std::string::npos) break;
        
        std::string keyPart = trim(content.substr(0, colonPos));
        if (keyPart.empty() || (keyPart.front() != '"' && keyPart.front() != '\'')) {
            break;
        }
        
        std::string key = unquote(keyPart);
        
        // Find the start of the value (after colon)
        size_t valueStart = colonPos + 1;
        while (valueStart < content.size() && std::isspace(content[valueStart])) {
            valueStart++;
        }
        
        if (valueStart >= content.size()) break;
        
        // Find matching braces/brackets for the value
        char firstChar = content[valueStart];
        size_t valueEnd;
        
        if (firstChar == '"' || firstChar == '\'') {
            // String literal
            size_t quoteEnd = valueStart + 1;
            while (quoteEnd < content.size() && content[quoteEnd] != firstChar) {
                if (content[quoteEnd] == '\\' && quoteEnd + 1 < content.size()) {
                    quoteEnd += 2; // Skip escaped character
                } else {
                    quoteEnd++;
                }
            }
            if (quoteEnd >= content.size()) break;
            valueEnd = quoteEnd + 1;
        } else if (firstChar == '[') {
            // Array
            int braceCount = 1;
            valueEnd = valueStart + 1;
            while (valueEnd < content.size() && braceCount > 0) {
                if (content[valueEnd] == '[') braceCount++;
                else if (content[valueEnd] == ']') braceCount--;
                valueEnd++;
            }
            if (braceCount != 0) break;
        } else if (firstChar == '{') {
            // Nested object
            int braceCount = 1;
            valueEnd = valueStart + 1;
            while (valueEnd < content.size() && braceCount > 0) {
                if (content[valueEnd] == '{') braceCount++;
                else if (content[valueEnd] == '}') braceCount--;
                valueEnd++;
            }
            if (braceCount != 0) break;
        } else {
            // Simple value (until comma or closing brace)
            size_t nextComma = content.find(',', valueStart);
            size_t nextBrace = content.find('}', valueStart);
            valueEnd = std::min(
                (nextComma == std::string::npos) ? content.size() : nextComma,
                (nextBrace == std::string::npos) ? content.size() : nextBrace
            );
            if (valueEnd == std::string::npos) break;
        }
        
        std::string valueStr = trim(content.substr(valueStart, valueEnd - valueStart));
        ConfigValue value;
        
        if (valueStr.empty()) {
            value = std::string();
        } else if (valueStr.front() == '"' || valueStr.front() == '\'') {
            value = parseJsonString(valueStr);
        } else if (valueStr.front() == '[') {
            value = parseJsonArray(valueStr);
        } else if (valueStr.front() == '{') {
            value = parseJsonObject(valueStr);
        } else {
            value = parseJsonString(valueStr);
        }
        
        entries.emplace_back(key, value, ConfigSource::File);
        
        // Move to next entry
        pos = valueEnd;
        if (pos < content.size()) {
            while (pos < content.size() && (content[pos] == ',' || std::isspace(content[pos]))) {
                pos++;
            }
            content = content.substr(pos);
        }
    }
    
    // Convert vector of ConfigEntry to vector of ConfigValue for storage
    std::vector<ConfigValue> objectValues;
    objectValues.reserve(entries.size());
    for (const auto& entry : entries) {
        objectValues.push_back(entry.value);
    }
    
    return objectValues;
}

// Parse any JSON value
static ConfigValue parseJsonValue(const std::string& str) {
    std::string content = trim(str);
    if (content.empty()) {
        return std::string();
    }
    
    char firstChar = content.front();
    
    if (firstChar == '"' || firstChar == '\'') {
        return parseJsonString(content);
    } else if (firstChar == '[') {
        return parseJsonArray(content);
    } else if (firstChar == '{') {
        return parseJsonObject(content);
    } else {
        return parseJsonString(content);
    }
}
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format based on extension
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json" || extension == ".jsonc" || extension == ".jsonl") {
        // Load as JSON
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Read entire file
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Parse JSON object
        if (content.find('{') == std::string::npos) {
            return false; // Not valid JSON object
        }
        
        try {
            // Parse the JSON object
            std::vector<ConfigValue> jsonObject = json::parseJsonObject(content);
            
            // Extract key-value pairs
            for (size_t i = 0; i < jsonObject.size(); i += 2) {
                if (i + 1 < jsonObject.size()) {
                    // Key should be a string
                    if (std::holds_alternative<std::string>(jsonObject[i])) {
                        std::string key = std::get<std::string>(jsonObject[i]);
                        set(key, jsonObject[i + 1], ConfigSource::File);
                    }
                }
            }
            
            return true;
        } catch (const std::exception&) {
            // If JSON parsing fails, fall back to simple format
        }
    }
    
    // Legacy simple key=value format
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