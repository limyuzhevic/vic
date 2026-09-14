#include "Config.hpp"
#include <cassert>
#include <cstring>
#include <typeinfo>
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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    clear();
    
    // Simple JSON parser for configuration files
    // Handles: { "key": "value", "num": 42, "bool": true, "array": [1, 2, 3] }
    size_t pos = 0;
    while (pos < content.length()) {
        // Skip whitespace
        while (pos < content.length() && std::isspace(content[pos])) {
            pos++;
        }
        
        if (pos >= content.length()) break;
        
        if (content[pos] == '}') {
            break; // End of object
        }
        
        if (content[pos] != '"') {
            // Skip malformed entry
            pos++;
            continue;
        }
        
        // Parse key
        size_t keyStart = ++pos; // Skip opening quote
        while (pos < content.length() && content[pos] != '"') {
            if (content[pos] == '\\') pos++; // Skip escaped characters
            pos++;
        }
        
        if (pos >= content.length()) break;
        
        std::string key = content.substr(keyStart, pos - keyStart);
        pos++; // Skip closing quote
        
        // Skip whitespace
        while (pos < content.length() && std::isspace(content[pos])) {
            pos++;
        }
        
        if (pos >= content.length() || content[pos] != ':') {
            continue; // Malformed JSON
        }
        pos++; // Skip colon
        
        // Skip whitespace
        while (pos < content.length() && std::isspace(content[pos])) {
            pos++;
        }
        
        if (pos >= content.length()) break;
        
        // Parse value based on type
        ConfigValue value;
        
        if (content[pos] == '"') {
            // String value
            size_t valueStart = ++pos;
            while (pos < content.length() && content[pos] != '"') {
                if (content[pos] == '\\') pos++; // Skip escaped characters
                pos++;
            }
            
            if (pos < content.length()) {
                value = content.substr(valueStart, pos - valueStart);
                pos++; // Skip closing quote
            }
        } else if (content[pos] == 't' || content[pos] == 'f') {
            // Boolean value
            if (content.substr(pos, 4) == "true") {
                value = true;
                pos += 4;
            } else if (content.substr(pos, 5) == "false") {
                value = false;
                pos += 5;
            }
        } else if (content[pos] == '-' || std::isdigit(content[pos])) {
            // Number value
            size_t numStart = pos;
            while (pos < content.length() && (std::isdigit(content[pos]) || content[pos] == '.' || content[pos] == '-')) {
                pos++;
            }
            
            std::string numStr = content.substr(numStart, pos - numStart);
            
            // Try to parse as integer first
            try {
                size_t endpos;
                int64_t intVal = std::stoll(numStr, &endpos);
                if (endpos == numStr.length()) {
                    value = intVal;
                } else {
                    // Try as double
                    double doubleVal = std::stod(numStr);
                    value = doubleVal;
                }
            } catch (...) {
                // If parsing fails, treat as string
                value = numStr;
            }
        } else if (content[pos] == '[') {
            // Array value
            std::vector<ConfigValue> array;
            pos++; // Skip opening bracket
            
            while (pos < content.length() && content[pos] != ']') {
                // Skip whitespace
                while (pos < content.length() && std::isspace(content[pos])) {
                    pos++;
                }
                
                if (pos >= content.length()) break;
                
                ConfigValue arrayItem;
                
                if (content[pos] == '"') {
                    size_t valueStart = ++pos;
                    while (pos < content.length() && content[pos] != '"') {
                        if (content[pos] == '\\') pos++;
                        pos++;
                    }
                    if (pos < content.length()) {
                        arrayItem = content.substr(valueStart, pos - valueStart);
                        pos++; // Skip closing quote
                    }
                } else if (content[pos] == 't' || content[pos] == 'f') {
                    if (content.substr(pos, 4) == "true") {
                        arrayItem = true;
                        pos += 4;
                    } else if (content.substr(pos, 5) == "false") {
                        arrayItem = false;
                        pos += 5;
                    }
                } else if (content[pos] == '-' || std::isdigit(content[pos])) {
                    size_t numStart = pos;
                    while (pos < content.length() && (std::isdigit(content[pos]) || content[pos] == '.' || content[pos] == '-')) {
                        pos++;
                    }
                    
                    std::string numStr = content.substr(numStart, pos - numStart);
                    try {
                        size_t endpos;
                        int64_t intVal = std::stoll(numStr, &endpos);
                        if (endpos == numStr.length()) {
                            arrayItem = intVal;
                        } else {
                            double doubleVal = std::stod(numStr);
                            arrayItem = doubleVal;
                        }
                    } catch (...) {
                        arrayItem = numStr;
                    }
                }
                
                // Add to array
                if (!std::holds_alternative<std::monostate>(arrayItem)) {
                    array.push_back(arrayItem);
                }
                
                // Skip whitespace and comma
                while (pos < content.length() && std::isspace(content[pos])) {
                    pos++;
                }
                
                if (pos < content.length() && content[pos] == ',') {
                    pos++;
                }
            }
            
            if (pos < content.length() && content[pos] == ']') {
                pos++; // Skip closing bracket
            }
            
            value = array;
        } else {
            // Skip unknown value type
            while (pos < content.length() && content[pos] != ',' && content[pos] != '}') {
                pos++;
            }
            continue;
        }
        
        // Skip whitespace
        while (pos < content.length() && std::isspace(content[pos])) {
            pos++;
        }
        
        // Add entry
        if (!std::holds_alternative<std::monostate>(value)) {
            set(key, value, ConfigSource::File);
        }
        
        // Skip comma or closing brace
        if (pos < content.length() && content[pos] == ',') {
            pos++; // Skip comma
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
        file << entry.key << " = ";
        
        // Store actual value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
                file << arg;
            } else if constexpr (std::is_floating_point_v<T>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << "UNKNOWN_TYPE";
            }
        }, entry.value);
        file << "\n\n";
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
