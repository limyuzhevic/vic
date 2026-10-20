#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <optional>
#include <cctype>

// JSON parsing helpers
static std::optional<ConfigValue> parseJSONValue(const std::string& valueStr);
static std::string configValueToString(const ConfigValue& value);

// Helper to trim whitespace
static std::string trim(const std::string& str);

// Helper to convert string to lowercase
static std::string toLower(const std::string& str);

struct Config::Impl {
    std::vector<ConfigEntry> entries;

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to detect file format and parse accordingly
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file content for JSON detection
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    if (content.empty()) {
        return true; // Empty file is valid
    }
    
    // Try to parse as JSON first
    if (parseJSONFile(content)) {
        return true;
    }
    
    // Fall back to key=value format
    std::ifstream fallbackFile(filepath);
    if (!fallbackFile.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(fallbackFile, line)) {
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

// Helper method to parse JSON file content
bool Config::parseJSONFile(const std::string& content) {
    if (content.empty()) return true;
    
    // Simple JSON format detection
    std::string trimmed = trim(content);
    if (trimmed.front() != '{') {
        return false; // Not JSON
    }
    
    // Try to parse as simple JSON key-value pairs
    // This is a simplified JSON parser for config files
    size_t pos = 1; // Skip opening {
    while (pos < trimmed.length()) {
        // Skip whitespace
        while (pos < trimmed.length() && std::isspace(trimmed[pos])) pos++;
        if (pos >= trimmed.length()) break;
        
        // Parse key
        if (trimmed[pos] != '"') {
            return false; // Invalid JSON
        }
        
        size_t keyStart = ++pos;
        while (pos < trimmed.length() && trimmed[pos] != '"') pos++;
        if (pos >= trimmed.length()) return false;
        
        std::string key = trimmed.substr(keyStart, pos - keyStart);
        pos++; // Skip closing "
        
        // Skip whitespace
        while (pos < trimmed.length() && std::isspace(trimmed[pos])) pos++;
        if (pos >= trimmed.length() || trimmed[pos] != ':') {
            return false; // Invalid JSON
        }
        pos++; // Skip :
        
        // Skip whitespace
        while (pos < trimmed.length() && std::isspace(trimmed[pos])) pos++;
        if (pos >= trimmed.length()) return false;
        
        // Parse value based on first character
        std::string valueStr;
        if (trimmed[pos] == '"' || trimmed[pos] == '\'') {
            // String value
            char quote = trimmed[pos];
            pos++;
            size_t valueStart = pos;
            while (pos < trimmed.length() && trimmed[pos] != quote) {
                if (trimmed[pos] == '\\' && pos + 1 < trimmed.length()) {
                    pos += 2; // Skip escaped character
                }
                pos++;
            }
            if (pos >= trimmed.length()) return false;
            
            valueStr = trimmed.substr(valueStart, pos - valueStart);
            pos++; // Skip closing quote
        } else if (trimmed[pos] == 't' || trimmed[pos] == 'f') {
            // Boolean value
            size_t boolStart = pos;
            while (pos < trimmed.length() && std::isalpha(trimmed[pos])) pos++;
            valueStr = trimmed.substr(boolStart, pos - boolStart);
        } else if (trimmed[pos] == '-' || std::isdigit(trimmed[pos])) {
            // Number value
            size_t numStart = pos;
            while (pos < trimmed.length() && (std::isdigit(trimmed[pos]) || 
                                             trimmed[pos] == '.' || 
                                             trimmed[pos] == '-' || 
                                             trimmed[pos] == 'e' || 
                                             trimmed[pos] == 'E')) pos++;
            valueStr = trimmed.substr(numStart, pos - numStart);
        } else if (trimmed[pos] == '[') {
            // Array value - parse until matching ]
            size_t arrayStart = pos;
            int bracketCount = 1;
            pos++;
            while (pos < trimmed.length() && bracketCount > 0) {
                if (trimmed[pos] == '[') bracketCount++;
                else if (trimmed[pos] == ']') bracketCount--;
                pos++;
            }
            valueStr = trimmed.substr(arrayStart, pos - arrayStart);
        } else {
            return false; // Unsupported value type
        }
        
        // Parse the value using our helper
        auto valueOpt = parseJSONValue(valueStr);
        if (valueOpt.has_value()) {
            set(key, valueOpt.value(), ConfigSource::File);
        }
        
        // Skip whitespace and comma
        while (pos < trimmed.length() && std::isspace(trimmed[pos])) pos++;
        if (pos < trimmed.length() && trimmed[pos] == ',') pos++;
        while (pos < trimmed.length() && std::isspace(trimmed[pos])) pos++;
    }
    
    return true;
}
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

// Save to file in key=value format (backward compatible)
bool Config::saveToFile(const std::string& filepath) const {
    return saveToKeyValueFile(filepath);
}

// Save to JSON file
bool Config::saveToJSONFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{" << std::endl;
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        // Escape key
        std::string escapedKey = entry.key;
        // Replace " with \" in key
        size_t pos = 0;
        while ((pos = escapedKey.find('"', pos)) != std::string::npos) {
            escapedKey.insert(pos, "\\");
            pos += 2;
        }
        
        file << "  \"" << escapedKey << "\": " << configValueToString(entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",";
        }
        file << std::endl;
    }
    
    file << "}" << std::endl;
    return true;
}

// Save to key=value file (backward compatible)
bool Config::saveToKeyValueFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << configValueToString(entry.value) << "\n";
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

// JSON parsing helpers
static std::optional<ConfigValue> parseJSONValue(const std::string& valueStr);
static std::string configValueToString(const ConfigValue& value);

// Helper to trim whitespace
static std::string trim(const std::string& str);

// Helper to convert string to lowercase
static std::string toLower(const std::string& str);

// Parse JSON array
get<>> static std::optional<std::vector<int>> parseJSONArrayInt(const std::string& jsonArray);
get<>> static std::optional<std::vector<double>> parseJSONArrayDouble(const std::string& jsonArray);
get<>> static std::optional<std::vector<std::string>> parseJSONArrayString(const std::string& jsonArray);

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
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

// Helper function implementations

std::optional<ConfigValue> parseJSONValue(const std::string& valueStr) {
    if (valueStr.empty()) return std::nullopt;
    
    // Remove surrounding whitespace
    std::string trimmed = trim(valueStr);
    
    // Check if it's a JSON array
    if (trimmed.front() == '[' && trimmed.back() == ']') {
        // Parse array - try to detect element type
        std::string content = trimmed.substr(1, trimmed.length() - 2);
        if (content.empty()) {
            return ConfigValue(std::vector<int>{});
        }
        
        // Try to parse as integers first
        std::vector<std::string> elements;
        size_t pos = 0;
        while (pos < content.length()) {
            size_t end = content.find(',', pos);
            if (end == std::string::npos) end = content.length();
            
            std::string element = trim(content.substr(pos, end - pos));
            elements.push_back(element);
            pos = end + 1;
        }
        
        // Check if all elements are integers
        bool allInts = true;
        for (const auto& elem : elements) {
            if (elem.empty() || elem.find('.') != std::string::npos) {
                allInts = false;
                break;
            }
            try {
                size_t idx;
                std::stoi(elem, &idx);
                if (idx != elem.length()) allInts = false;
            } catch (...) {
                allInts = false;
            }
        }
        
        if (allInts) {
            std::vector<int> intArray;
            for (const auto& elem : elements) {
                intArray.push_back(std::stoi(elem));
            }
            return ConfigValue(intArray);
        }
        
        // Try to parse as doubles
        bool allDoubles = true;
        for (const auto& elem : elements) {
            try {
                size_t idx;
                std::stod(elem, &idx);
                if (idx != elem.length()) allDoubles = false;
            } catch (...) {
                allDoubles = false;
            }
        }
        
        if (allDoubles) {
            std::vector<double> doubleArray;
            for (const auto& elem : elements) {
                doubleArray.push_back(std::stod(elem));
            }
            return ConfigValue(doubleArray);
        }
        
        // Assume strings
        std::vector<std::string> stringArray;
        for (const auto& elem : elements) {
            // Remove quotes if present
            std::string str = trim(elem);
            if (str.size() >= 2 && 
                ((str.front() == '"' && str.back() == '"') ||
                 (str.front() == '\'' && str.back() == '\''))) {
                str = str.substr(1, str.length() - 2);
            }
            stringArray.push_back(str);
        }
        return ConfigValue(stringArray);
    }
    
    // Parse as boolean
    if (toLower(trimmed) == "true") {
        return ConfigValue(true);
    } else if (toLower(trimmed) == "false") {
        return ConfigValue(false);
    }
    
    // Parse as number (int or double)
    try {
        size_t idx;
        long long intVal = std::stoll(trimmed, &idx);
        if (idx == trimmed.length()) {
            return ConfigValue(intVal);
        }
    } catch (...) {}
    
    try {
        size_t idx;
        double doubleVal = std::stod(trimmed, &idx);
        if (idx == trimmed.length()) {
            return ConfigValue(doubleVal);
        }
    } catch (...) {}
    
    // Default to string
    if (trimmed.size() >= 2 && 
        ((trimmed.front() == '"' && trimmed.back() == '"') ||
         (trimmed.front() == '\'' && trimmed.back() == '\''))) {
        return ConfigValue(trimmed.substr(1, trimmed.length() - 2));
    }
    
    return ConfigValue(trimmed);
}

std::string configValueToString(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            // Escape quotes and backslashes
            std::string result = arg;
            size_t pos = 0;
            while ((pos = result.find('\\', pos)) != std::string::npos) {
                result.insert(pos, "\\\\");
                pos += 2;
            }
            pos = 0;
            while ((pos = result.find('\"', pos)) != std::string::npos) {
                result.insert(pos, "\\\"");
                pos += 2;
            }
            return "\"" + result + "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + arg[i] + "\"";
            }
            result += "]";
            return result;
        }
        return "";
    }, value);
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Parse JSON array for specific types
std::optional<std::vector<int>> parseJSONArrayInt(const std::string& jsonArray) {
    if (jsonArray.empty() || jsonArray.front() != '[' || jsonArray.back() != ']') {
        return std::nullopt;
    }
    
    std::vector<int> result;
    std::string content = jsonArray.substr(1, jsonArray.length() - 2);
    if (content.empty()) return result;
    
    size_t pos = 0;
    while (pos < content.length()) {
        size_t end = content.find(',', pos);
        if (end == std::string::npos) end = content.length();
        
        std::string elem = trim(content.substr(pos, end - pos));
        try {
            result.push_back(std::stoi(elem));
        } catch (...) {
            return std::nullopt;
        }
        
        pos = end + 1;
    }
    
    return result;
}

std::optional<std::vector<double>> parseJSONArrayDouble(const std::string& jsonArray) {
    if (jsonArray.empty() || jsonArray.front() != '[' || jsonArray.back() != ']') {
        return std::nullopt;
    }
    
    std::vector<double> result;
    std::string content = jsonArray.substr(1, jsonArray.length() - 2);
    if (content.empty()) return result;
    
    size_t pos = 0;
    while (pos < content.length()) {
        size_t end = content.find(',', pos);
        if (end == std::string::npos) end = content.length();
        
        std::string elem = trim(content.substr(pos, end - pos));
        try {
            result.push_back(std::stod(elem));
        } catch (...) {
            return std::nullopt;
        }
        
        pos = end + 1;
    }
    
    return result;
}

std::optional<std::vector<std::string>> parseJSONArrayString(const std::string& jsonArray) {
    if (jsonArray.empty() || jsonArray.front() != '[' || jsonArray.back() != ']') {
        return std::nullopt;
    }
    
    std::vector<std::string> result;
    std::string content = jsonArray.substr(1, jsonArray.length() - 2);
    if (content.empty()) return result;
    
    size_t pos = 0;
    while (pos < content.length()) {
        size_t end = content.find(',', pos);
        if (end == std::string::npos) end = content.length();
        
        std::string elem = trim(content.substr(pos, end - pos));
        
        // Remove quotes if present
        if (elem.size() >= 2 && 
            ((elem.front() == '"' && elem.back() == '"') ||
             (elem.front() == '\'' && elem.back() == '\''))) {
            elem = elem.substr(1, elem.length() - 2);
        }
        
        result.push_back(elem);
        
        pos = end + 1;
    }
    
    return result;
}

} // namespace nlm
