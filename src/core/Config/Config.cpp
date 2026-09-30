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
    // Try JSON format first
    bool jsonSuccess = loadFromJSONFile(filepath);
    if (jsonSuccess) {
        return true;
    }
    
    // Fall back to key=value format
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

bool Config::loadFromJSONFile(const std::string& filepath) {
    // Simple JSON parser for basic key-value pairs
    // This is a minimal implementation - for production use, consider a proper JSON library
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Remove whitespace
    std::string json = trim(content);
    if (json.empty() || (json[0] != '{' && json[0] != '[')) {
        return false;  // Not JSON
    }
    
    try {
        // Simple JSON parsing - only supports { "key": "value", "number": 123 }
        if (json[0] == '{') {
            // Object parsing
            size_t pos = 1;
            while (pos < json.size() && json[pos] != '}') {
                // Skip whitespace
                while (pos < json.size() && std::isspace(json[pos])) {
                    pos++;
                }
                if (pos >= json.size() || json[pos] == '}') {
                    break;
                }
                
                // Parse key
                if (json[pos] != '"') {
                    return false;
                }
                size_t keyStart = pos + 1;
                pos = json.find('"', keyStart);
                if (pos == std::string::npos) {
                    return false;
                }
                std::string key = json.substr(keyStart, pos - keyStart);
                
                // Skip to colon
                pos++;
                while (pos < json.size() && std::isspace(json[pos])) {
                    pos++;
                }
                if (pos >= json.size() || json[pos] != ':') {
                    return false;
                }
                pos++;
                while (pos < json.size() && std::isspace(json[pos])) {
                    pos++;
                }
                
                // Parse value (simplified)
                std::string value;
                if (pos < json.size() && json[pos] == '"') {
                    // String value
                    size_t valStart = pos + 1;
                    pos = json.find('"', valStart);
                    if (pos == std::string::npos) {
                        return false;
                    }
                    value = json.substr(valStart, pos - valStart);
                    pos++;  // Skip closing quote
                } else if (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '-')) {
                    // Number value
                    size_t valStart = pos;
                    while (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '.' || json[pos] == '-' || json[pos] == 'e' || json[pos] == 'E' || json[pos] == '+')) {
                        pos++;
                    }
                    value = json.substr(valStart, pos - valStart);
                } else if (pos < json.size() && json[pos] == 't' || json[pos] == 'f') {
                    // Boolean value
                    if (json.substr(pos, 4) == "true") {
                        value = "true";
                        pos += 4;
                    } else if (json.substr(pos, 5) == "false") {
                        value = "false";
                        pos += 5;
                    } else {
                        return false;
                    }
                } else if (pos < json.size() && json[pos] == 'n') {
                    // Null value
                    if (json.substr(pos, 4) == "null") {
                        value = "null";
                        pos += 4;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
                
                // Set the key-value pair
                set(key, value, ConfigSource::File);
                
                // Skip comma or closing brace
                while (pos < json.size() && std::isspace(json[pos])) {
                    pos++;
                }
                if (pos < json.size() && json[pos] == ',') {
                    pos++;
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
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
    
    // Write in JSON format for better interoperability
    file << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Write value based on type
        if (std::holds_alternative<int>(entry.value)) {
            file << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            file << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            file << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            file << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            file << "\"" << escapeString(std::get<std::string>(entry.value)) << "\"";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            file << "[";  // Simplified array support
            const auto& arr = std::get<std::vector<int>>(entry.value);
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) file << ", ";
                file << arr[i];
            }
            file << "]";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            file << "[";  // Simplified array support
            const auto& arr = std::get<std::vector<double>>(entry.value);
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) file << ", ";
                file << arr[i];
            }
            file << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            file << "[";  // Simplified array support
            const auto& arr = std::get<std::vector<std::string>>(entry.value);
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) file << ", ";
                file << "\"" << escapeString(arr[i]) << "\"";
            }
            file << "]";
        } else {
            file << "null";  // Unknown type
        }
        
        if (!entry.description.empty()) {
            file << "  // " << entry.description;
        }
    }
    
    file << "\n}\n";
    
    return true;
}

// Helper method to escape JSON strings
std::string Config::escapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size() + 10);
    
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    // Control character - use hex escape
                    result += "\\u00";
                    result += "0123456789ABCDEF"[(c >> 4) & 0xF];
                    result += "0123456789ABCDEF"[ c       & 0xF];
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
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

std::string Config::escapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size() + 10);
    
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    // Control character - use hex escape
                    result += "\\u00";
                    result += "0123456789ABCDEF"[(c >> 4) & 0xF];
                    result += "0123456789ABCDEF"[ c       & 0xF];
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    // Simple JSON parser for basic key-value pairs
    // This is a minimal implementation - for production use, consider a proper JSON library
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Remove whitespace
    std::string json = trim(content);
    if (json.empty() || (json[0] != '{' && json[0] != '[')) {
        return false;  // Not JSON
    }
    
    try {
        // Simple JSON parsing - only supports { "key": "value", "number": 123 }
        if (json[0] == '{') {
            // Object parsing
            size_t pos = 1;
            while (pos < json.size() && json[pos] != '}') {
                // Skip whitespace
                while (pos < json.size() && std::isspace(json[pos])) {
                    pos++;
                }
                if (pos >= json.size() || json[pos] == '}') {
                    break;
                }
                
                // Parse key
                if (json[pos] != '"') {
                    return false;
                }
                size_t keyStart = pos + 1;
                pos = json.find('"', keyStart);
                if (pos == std::string::npos) {
                    return false;
                }
                std::string key = json.substr(keyStart, pos - keyStart);
                
                // Skip to colon
                pos++;
                while (pos < json.size() && std::isspace(json[pos])) {
                    pos++;
                }
                if (pos >= json.size() || json[pos] != ':') {
                    return false;
                }
                pos++;
                while (pos < json.size() && std::isspace(json[pos])) {
                    pos++;
                }
                
                // Parse value (simplified)
                std::string value;
                if (pos < json.size() && json[pos] == '"') {
                    // String value
                    size_t valStart = pos + 1;
                    pos = json.find('"', valStart);
                    if (pos == std::string::npos) {
                        return false;
                    }
                    value = json.substr(valStart, pos - valStart);
                    pos++;  // Skip closing quote
                } else if (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '-')) {
                    // Number value
                    size_t valStart = pos;
                    while (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '.' || json[pos] == '-' || json[pos] == 'e' || json[pos] == 'E' || json[pos] == '+')) {
                        pos++;
                    }
                    value = json.substr(valStart, pos - valStart);
                } else if (pos < json.size() && json[pos] == 't' || json[pos] == 'f') {
                    // Boolean value
                    if (json.substr(pos, 4) == "true") {
                        value = "true";
                        pos += 4;
                    } else if (json.substr(pos, 5) == "false") {
                        value = "false";
                        pos += 5;
                    } else {
                        return false;
                    }
                } else if (pos < json.size() && json[pos] == 'n') {
                    // Null value
                    if (json.substr(pos, 4) == "null") {
                        value = "null";
                        pos += 4;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
                
                // Set the key-value pair
                set(key, value, ConfigSource::File);
                
                // Skip comma or closing brace
                while (pos < json.size() && std::isspace(json[pos])) {
                    pos++;
                }
                if (pos < json.size() && json[pos] == ',') {
                    pos++;
                }
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
    }
    
    return true;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;

template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
