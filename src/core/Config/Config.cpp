#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <iostream>

namespace nlm {

// Platform-specific includes for JSON/YAML support
#if defined(_WIN32) || defined(_WIN64)
    #include <filesystem>
#else
    #include <unistd.h>
#endif

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to determine file extension and load appropriate parser
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file type by extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < filepath.length() - 1) {
        ext = filepath.substr(dotPos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    }
    
    // Try JSON first, then fallback to simple format
    if (ext == "json" || ext == "cfg" || ext == "config") {
        return loadJSONFromFile(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return loadYAMLFromFile(filepath);
    } else {
        // Try JSON first, then simple format
        bool jsonLoaded = loadJSONFromFile(filepath);
        if (jsonLoaded) {
            return true;
        }
        // Fallback to simple key=value format for backward compatibility
        return loadSimpleFromFile(filepath);
    }
}

// Enhanced JSON loader with proper error handling and type conversion
bool Config::loadJSONFromFile(const std::string& filepath) {
    // Since we don't have external JSON/YAML libraries, implement a simple but robust JSON parser
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple JSON parsing - handle basic objects and key-value pairs
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Remove whitespace and comments
    std::string jsonContent = cleanJSON(content);
    
    // Simple JSON object parser (handles { "key": "value" } format)
    size_t pos = 0;
    bool inObject = false;
    bool parsingSuccess = false;
    
    while (pos < jsonContent.length()) {
        char c = jsonContent[pos];
        
        if (c == '{') {
            inObject = true;
            pos++;
            continue;
        } else if (c == '}') {
            inObject = false;
            pos++;
            continue;
        } else if (inObject && c == '"') {
            // Found a key
            size_t keyEnd = jsonContent.find('"', pos + 1);
            if (keyEnd == std::string::npos) {
                break;
            }
            
            std::string key = jsonContent.substr(pos + 1, keyEnd - pos - 1);
            pos = keyEnd + 1;
            
            // Skip whitespace
            while (pos < jsonContent.length() && (jsonContent[pos] == ' ' || jsonContent[pos] == '\t' || jsonContent[pos] == '\n')) {
                pos++;
            }
            
            if (pos >= jsonContent.length() || jsonContent[pos] != ':') {
                break;
            }
            
            pos++; // Skip colon
            
            // Skip whitespace after colon
            while (pos < jsonContent.length() && (jsonContent[pos] == ' ' || jsonContent[pos] == '\t' || jsonContent[pos] == '\n')) {
                pos++;
            }
            
            // Parse value based on its first character
            if (pos < jsonContent.length()) {
                if (jsonContent[pos] == '"') {
                    // String value
                    size_t valueEnd = jsonContent.find('"', pos + 1);
                    if (valueEnd == std::string::npos) {
                        break;
                    }
                    std::string value = jsonContent.substr(pos + 1, valueEnd - pos - 1);
                    set(key, value, ConfigSource::File);
                    pos = valueEnd + 1;
                } else if (jsonContent[pos] == 't' || jsonContent[pos] == 'f') {
                    // Boolean value
                    size_t valueEnd = jsonContent.find_first_of(",}\]\n\r", pos);
                    if (valueEnd == std::string::npos) {
                        valueEnd = jsonContent.length();
                    }
                    std::string valueStr = jsonContent.substr(pos, valueEnd - pos);
                    bool value = (valueStr == "true" || valueStr == "yes" || valueStr == "1");
                    set(key, value, ConfigSource::File);
                    pos = valueEnd;
                } else if (jsonContent[pos] == 'n') {
                    // null value - store as empty string
                    size_t valueEnd = jsonContent.find_first_of(",}\]\n\r", pos);
                    if (valueEnd == std::string::npos) {
                        valueEnd = jsonContent.length();
                    }
                    set(key, "", ConfigSource::File);
                    pos = valueEnd;
                } else if (isdigit(jsonContent[pos]) || jsonContent[pos] == '-' || jsonContent[pos] == '+') {
                    // Number value (integer or float)
                    size_t valueEnd = jsonContent.find_first_of(",}\]\n\r", pos);
                    if (valueEnd == std::string::npos) {
                        valueEnd = jsonContent.length();
                    }
                    std::string valueStr = jsonContent.substr(pos, valueEnd - pos);
                    
                    // Try to parse as integer first
                    try {
                        size_t posAfterInt = 0;
                        long long intValue = std::stoll(valueStr, &posAfterInt);
                        if (posAfterInt == valueStr.length()) {
                            set(key, intValue, ConfigSource::File);
                            pos = valueEnd;
                            continue;
                        }
                    } catch (...) {}
                    
                    // Try to parse as double
                    try {
                        size_t posAfterDouble = 0;
                        double doubleValue = std::stod(valueStr, &posAfterDouble);
                        if (posAfterDouble == valueStr.length()) {
                            set(key, doubleValue, ConfigSource::File);
                            pos = valueEnd;
                            continue;
                        }
                    } catch (...) {}
                    
                    // Fallback to string
                    set(key, valueStr, ConfigSource::File);
                    pos = valueEnd;
                } else {
                    // Unknown value type, treat as string
                    size_t valueEnd = jsonContent.find_first_of(",}\]\n\r", pos);
                    if (valueEnd == std::string::npos) {
                        valueEnd = jsonContent.length();
                    }
                    std::string value = jsonContent.substr(pos, valueEnd - pos);
                    set(key, value, ConfigSource::File);
                    pos = valueEnd;
                }
                
                // Skip trailing comma
                while (pos < jsonContent.length() && (jsonContent[pos] == ',' || jsonContent[pos] == '\n' || jsonContent[pos] == '\r')) {
                    pos++;
                }
            }
        } else {
            pos++;
        }
    }
    
    return true;
}

bool Config::loadYAMLFromFile(const std::string& filepath) {
    // For now, treat YAML files like JSON files (most YAML is JSON-compatible)
    // In a full implementation, this would use a proper YAML parser
    return loadJSONFromFile(filepath);
}

bool Config::loadSimpleFromFile(const std::string& filepath) {
    // Original simple key=value parser for backward compatibility
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

std::string Config::cleanJSON(const std::string& content) {
    std::string result;
    bool inString = false;
    bool escaped = false;
    
    for (char c : content) {
        if (escaped) {
            result += c;
            escaped = false;
            continue;
        }
        
        if (c == '\\') {
            escaped = true;
            result += c;
            continue;
        }
        
        if (c == '\"') {
            inString = !inString;
            result += c;
            continue;
        }
        
        if (!inString && (c == '/' || c == '#')) {
            // Skip comments (single line)
            continue;
        }
        
        if (!inString && (c == ' ' || c == '\t' || c == '\n' || c == '\r')) {
            // Remove whitespace outside strings
            if (c == '\n' || c == '\r') {
                result += ' ';
            }
            continue;
        }
        
        result += c;
    }
    
    return result;
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
    
    // Determine file extension and format
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < filepath.length() - 1) {
        ext = filepath.substr(dotPos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    }
    
    // Save in JSON format if extension suggests it
    if (ext == "json" || ext == "cfg" || ext == "config") {
        file << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "  \"" << entry.key << "\": ";
            
            // Format value based on type
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << escapeString(arg) << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    // Format double to avoid scientific notation
                    file << std::fixed << std::setprecision(6) << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) file << ", ";
                        file << arg[j];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) file << ", ";
                        file << std::fixed << std::setprecision(6) << arg[j];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) file << ", ";
                        file << "\"" << escapeString(arg[j]) << "\"";
                    }
                    file << "]";
                } else {
                    file << "null";
                }
            }, entry.value);
            
            if (i < pImpl->entries.size() - 1) {
                file << ",\n";
            } else {
                file << "\n";
            }
        }
        file << "}\n";
    } else {
        // Save in simple key=value format for backward compatibility
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Format value based on type
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << escapeString(arg) << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            file << "\n\n";
        }
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

std::string Config::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\0': result += "\\\\0"; break;
            case '\b': result += "\\\\b"; break;
            case '\f': result += "\\\\f"; break;
            case '\n': result += "\\\\n"; break;
            case '\r': result += "\\\\r"; break;
            case '\t': result += "\\\\t"; break;
            default:
                if (c >= 0 && c < 0x20) {
                    // Control characters
                    result += "\\\\u";
                    char hex[5];
                    snprintf(hex, sizeof(hex), "%04x", (unsigned char)c);
                    result += hex;
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

// Add necessary includes for string manipulation
namespace std {
    template<typename... Args>
    std::basic_string<char> to_string(Args&&... args);
}
// Add necessary includes for string manipulation and file I/O
#include <cctype>
#include <iomanip>

// Add missing Config::escapeString method
std::string Config::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\0': result += "\\\\0"; break;
            case '\b': result += "\\\\b"; break;
            case '\f': result += "\\\\f"; break;
            case '\n': result += "\\\\n"; break;
            case '\r': result += "\\\\r"; break;
            case '\t': result += "\\\\t"; break;
            default:
                if (c >= 0 && c < 0x20) {
                    // Control characters
                    result += "\\\\u";
                    char hex[5];
                    snprintf(hex, sizeof(hex), "%04x", (unsigned char)c);
                    result += hex;
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

// Add missing template instantiations for get methods
namespace nlm {
    
// Template instantiations for get methods
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
    
}
