#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>
#include <string>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    if (parseJSONFile(file, filepath)) {
        return true;
    }
    
    // Reset stream and try legacy format
    file.clear();
    file.seekg(0);
    
    // Try legacy key=value format
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

bool Config::parseJSONFile(std::ifstream& file, const std::string& filepath) {
    // Simple JSON parsing without external dependencies
    // This handles basic JSON objects with string keys and various value types
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Skip whitespace and check for opening brace
    size_t pos = 0;
    while (pos < content.size() && std::isspace(content[pos])) pos++;
    
    if (pos >= content.size() || content[pos] != '{') {
        return false; // Not JSON object format
    }
    
    pos++; // Skip opening brace
    
    while (pos < content.size()) {
        // Skip whitespace
        while (pos < content.size() && std::isspace(content[pos])) pos++;
        
        // Check for closing brace
        if (pos < content.size() && content[pos] == '}') {
            break;
        }
        
        // Parse key
        size_t keyStart = pos;
        while (pos < content.size() && content[pos] != ':' && content[pos] != '\"') pos++;
        
        std::string key;
        if (pos < content.size() && content[pos] == '\"') {
            // String key with quotes
            pos++; // Skip opening quote
            keyStart = pos;
            while (pos < content.size() && content[pos] != '\"') pos++;
            key = content.substr(keyStart, pos - keyStart);
            pos++; // Skip closing quote
        } else {
            // Unquoted key (like legacy format)
            keyStart = pos;
            while (pos < content.size() && content[pos] != ':' && content[pos] != ' ' && content[pos] != '\n') pos++;
            key = content.substr(keyStart, pos - keyStart);
            key = trim(key);
        }
        
        // Skip whitespace and colon
        while (pos < content.size() && std::isspace(content[pos])) pos++;
        if (pos >= content.size() || content[pos] != ':') {
            break; // Malformed JSON
        }
        
        pos++; // Skip colon
        while (pos < content.size() && std::isspace(content[pos])) pos++;
        
        // Parse value based on first character
        if (pos < content.size()) {
            char firstChar = content[pos];
            
            if (firstChar == '\"') {
                // String value
                pos++; // Skip opening quote
                size_t valueStart = pos;
                while (pos < content.size() && content[pos] != '\"') pos++;
                std::string value = content.substr(valueStart, pos - valueStart);
                set(key, value, ConfigSource::File);
                pos++; // Skip closing quote
            } else if (firstChar == '[') {
                // Array value (JSON array)
                std::vector<double> array;
                pos++; // Skip opening bracket
                while (pos < content.size() && content[pos] != ']') {
                    while (pos < content.size() && std::isspace(content[pos])) pos++;
                    
                    if (pos >= content.size() || content[pos] == ']') break;
                    
                    size_t valueStart = pos;
                    while (pos < content.size() && content[pos] != ',' && content[pos] != ']') pos++;
                    
                    std::string numStr = content.substr(valueStart, pos - valueStart);
                    try {
                        double val = std::stod(numStr);
                        array.push_back(val);
                    } catch (...) {
                        array.clear();
                        break;
                    }
                    
                    if (pos < content.size() && content[pos] == ',') {
                        pos++; // Skip comma
                    }
                }
                
                if (pos < content.size() && content[pos] == ']') {
                    pos++; // Skip closing bracket
                    set(key, array, ConfigSource::File);
                }
            } else if (firstChar == '{') {
                // Nested object - for now, skip it
                size_t braceCount = 1;
                pos++; // Skip opening brace
                while (pos < content.size() && braceCount > 0) {
                    if (content[pos] == '{') braceCount++;
                    else if (content[pos] == '}') braceCount--;
                    pos++;
                }
            } else if (firstChar == 't' || firstChar == 'f') {
                // Boolean value
                std::string boolStr;
                size_t boolStart = pos;
                while (pos < content.size() && content[pos] != ',' && content[pos] != '}' && content[pos] != ']') pos++;
                boolStr = content.substr(boolStart, pos - boolStart);
                if (boolStr == "true") {
                    set(key, true, ConfigSource::File);
                } else if (boolStr == "false") {
                    set(key, false, ConfigSource::File);
                }
            } else {
                // Numeric value
                size_t valueStart = pos;
                while (pos < content.size() && content[pos] != ',' && content[pos] != '}' && content[pos] != ']') pos++;
                std::string numStr = content.substr(valueStart, pos - valueStart);
                try {
                    // Try double first
                    double doubleVal = std::stod(numStr);
                    set(key, doubleVal, ConfigSource::File);
                } catch (...) {
                    try {
                        // Try int
                        long long intVal = std::stoll(numStr);
                        set(key, intVal, ConfigSource::File);
                    } catch (...) {
                        // Ignore unparseable values
                    }
                }
            }
        }
        
        // Skip to next entry
        while (pos < content.size() && content[pos] != ',') pos++;
        if (pos < content.size() && content[pos] == ',') pos++;
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
    
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        // Write value based on type
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                file << val;
            } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, long long>) {
                file << val;
            } else if constexpr (std::is_same_v<T, int>) {
                file << static_cast<int64_t>(val);
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << val[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << val[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << val[j] << "\"";
                }
                file << "]";
            } else {
                file << "null"; // Fallback for unknown types
            }
        }, entry.value);
        
        file << ",\n";
    }
    
    // Remove trailing comma from last entry
    if (!pImpl->entries.empty()) {
        file.seekp(-2, std::ios_base::end); // Go back 2 characters to remove ",\n"
        file << "\n";
    }
    
    file << "}\n";
    
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
