#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// Helper method to parse JSON value string into ConfigValue
static ConfigValue parseJSONValue(const std::string& jsonStr);

// JSON parsing implementation
bool Config::loadFromJSON(const std::string& filepath);
bool Config::loadFromYAML(const std::string& filepath);
bool Config::loadFromSimpleFormat(const std::string& filepath);

// Helper function for JSON array parsing
static ConfigValue parseJSONValue(const std::string& jsonStr) {
    if (jsonStr.empty()) return ConfigValue();
    
    // Remove whitespace
    std::string str = trim(jsonStr);
    
    // Parse boolean
    if (str == "true") return true;
    if (str == "false") return false;
    
    // Parse null
    if (str == "null") return std::string();
    
    // Parse string
    if ((str.front() == '"' && str.back() == '"') || 
        (str.front() == '\'' && str.back() == '\'')) {
        return str.substr(1, str.size() - 2);
    }
    
    // Parse numbers
    try {
        // Check for float
        size_t pos;
        double d = std::stod(str, &pos);
        if (pos == str.size()) return d;
        
        // Check for int64_t
        int64_t i = std::stoll(str, &pos);
        if (pos == str.size()) return i;
        
        // Check for int
        int i32 = std::stoi(str, &pos);
        if (pos == str.size()) return i32;
    } catch (...) {
        // Not a number
    }
    
    // Parse array (simplified)
    if (str.front() == '[' && str.back() == ']') {
        std::string arrayStr = str.substr(1, str.size() - 2);
        std::vector<ConfigValue> result;
        
        size_t pos = 0;
        while (pos < arrayStr.size()) {
            // Find next comma or end
            size_t commaPos = arrayStr.find(',', pos);
            size_t endPos = commaPos == std::string::npos ? arrayStr.size() : commaPos;
            
            std::string itemStr = trim(arrayStr.substr(pos, endPos - pos));
            
            // Parse the item recursively
            if (!itemStr.empty()) {
                // For simplicity, only support int arrays for now
                try {
                    int item = std::stoi(itemStr);
                    result.push_back(item);
                } catch (...) {
                    // Skip unparseable items
                }
            }
            
            if (commaPos != std::string::npos) {
                pos = commaPos + 1;
            } else {
                break;
            }
        }
        
        // Return as vector<int> for now
        return result;
    }
    
    // If all else fails, return as string
    return str;
}

// JSON parsing implementation
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), 
                        std::istreambuf_iterator<char>());
    
    // Remove whitespace for parsing
    std::string::iterator end = std::remove_if(content.begin(), content.end(), 
                                               [](char c) { return std::isspace(c); });
    content.erase(end, content.end());
    
    // Simple JSON parser - assumes object format {"key": "value"}
    if (content.front() != '{' || content.back() != '}') {
        return false;
    }
    
    std::string inner = content.substr(1, content.size() - 2);
    size_t pos = 0;
    
    while (pos < inner.size()) {
        // Find key
        size_t keyEnd = inner.find('"', pos);
        if (keyEnd == std::string::npos) break;
        
        size_t keyStart = pos;
        size_t valueStart = inner.find(':', keyEnd);
        if (valueStart == std::string::npos) break;
        
        std::string key = inner.substr(keyStart + 1, keyEnd - keyStart - 1);
        key = trim(key);
        
        // Find value start (skip colon and whitespace)
        size_t valuePos = inner.find_first_not_of(":", valueStart + 1);
        if (valuePos == std::string::npos) break;
        
        // Find value end (next comma or closing brace)
        size_t valueEnd = inner.find(',', valuePos);
        if (valueEnd == std::string::npos) {
            valueEnd = inner.find('}', valuePos);
        }
        
        if (valueEnd == std::string::npos) break;
        
        std::string valueStr = inner.substr(valuePos, valueEnd - valuePos);
        valueStr = trim(valueStr);
        
        // Remove quotes around string values
        if ((valueStr.front() == '"' && valueStr.back() == '"') ||
            (valueStr.front() == '\'' && valueStr.back() == '\'')) {
            valueStr = valueStr.substr(1, valueStr.size() - 2);
        }
        
        set(key, valueStr, ConfigSource::File);
        
        pos = valueEnd + 1;
        // Skip comma and whitespace
        pos = inner.find_first_not_of(",", pos);
    }
    
    return true;
}

// YAML parsing implementation (simplified)
bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentKey;
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        // Skip comments and empty lines
        if (line.empty() || line.substr(0, 2) == "# ") {
            continue;
        }
        
        // Check for key-value pair (key: value format)
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            currentKey = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Remove quotes from string values
            if ((value.front() == '"' && value.back() == '"') ||
                (value.front() == '\'' && value.back() == '\'')) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Parse boolean
            if (value == "true") {
                set(currentKey, true, ConfigSource::File);
            } else if (value == "false") {
                set(currentKey, false, ConfigSource::File);
            } 
            // Parse number
            else {
                try {
                    size_t pos;
                    double d = std::stod(value, &pos);
                    if (pos == value.size()) {
                        // It's a number
                        if (std::floor(d) == d) {
                            // Integer
                            if (d >= std::numeric_limits<int>::min() && 
                                d <= std::numeric_limits<int>::max()) {
                                set(currentKey, static_cast<int>(d), ConfigSource::File);
                            } else if (d >= std::numeric_limits<int64_t>::min() && 
                                       d <= std::numeric_limits<int64_t>::max()) {
                                set(currentKey, static_cast<int64_t>(d), ConfigSource::File);
                            } else {
                                set(currentKey, d, ConfigSource::File);
                            }
                        } else {
                            set(currentKey, d, ConfigSource::File);
                        }
                        continue;
                    }
                } catch (...) {
                    // Not a number, treat as string
                }
                
                set(currentKey, value, ConfigSource::File);
            }
        }
    }
    
    return true;
}

// Simple format loader (fallback)
bool Config::loadFromSimpleFormat(const std::string& filepath) {
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
        
        // Write the actual value based on the variant type
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            
            if constexpr (std::is_same_v<T, int>) {
                file << entry.key << " = " << value << "\n";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << value << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << entry.key << " = " << value << "\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (value ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << value << "\"\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << value[i] << "\"";
                }
                file << "]\n";
            }
        }, entry.value);
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
