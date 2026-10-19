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
    // Simple key=value format parser with basic JSON support
    // TODO PHASE 2: Replace with full JSON/YAML parser (nlohmann::json)
    
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
        
        // Handle JSON-style object { "key": "value", "num": 123 }
        if (line.find('{') != std::string::npos && line.find('}') != std::string::npos) {
            parseJSONLine(line);
            continue;
        }
        
        // Handle JSON-style array [ "value1", "value2" ]
        if (line.find('[') != std::string::npos && line.find(']') != std::string::npos) {
            // Try to extract key=value pattern from around the array
            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = trim(line.substr(0, eqPos));
                std::string valueStr = trim(line.substr(eqPos + 1));
                set(key, parseJSONValue(valueStr), ConfigSource::File);
            }
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
            
            // Try to parse as different types
            set(key, parseJSONValue(value), ConfigSource::File);
        }
    }
    
    return true;
}

// Parse JSON value string into appropriate ConfigValue
    ConfigValue Config::parseJSONValue(const std::string& valueStr) {
        // Simple JSON value parser with type detection
        // Attempts to parse in order: string, boolean, double, int64, int, fallback to string
        
        std::string trimmed = trim(valueStr);
        
        if (trimmed.empty()) {
            return ConfigValue("");
        }
        
        // Handle JSON strings - quoted values
        if (trimmed[0] == '"' && trimmed.back() == '"') {
            return ConfigValue(trimmed.substr(1, trimmed.size() - 2));
        } 
        // Handle JSON booleans
        else if (trimmed == "true" || trimmed == "false") {
            return ConfigValue(trimmed == "true");
        } 
        // Handle JSON numbers - try double first for floating point
        else if (trimmed.find('.') != std::string::npos) {
            try {
                double val = std::stod(trimmed);
                return ConfigValue(val);
            } catch (...) {
                // Fall through to string if conversion fails
            }
        } 
        // Handle integers - try int64 first for larger range, then int
        else {
            try {
                // Try int64 first for larger range
                int64_t val = std::stoll(trimmed);
                return ConfigValue(val);
            } catch (...) {
                try {
                    int val = std::stoi(trimmed);
                    return ConfigValue(val);
                } catch (...) {
                    // Fall through to string if conversion fails
                }
            }
        }
        
        // Fallback to treating as string if no other type matches
        return ConfigValue(trimmed);
    }

    // Find matching closing bracket for array or object
    // Used for parsing nested JSON structures
    size_t Config::findMatchingBracket(const std::string& str, size_t start, char openBracket) {
        char closeBracket = (openBracket == '[') ? ']' : '}';
        size_t count = 1;
        size_t pos = start + 1;
        
        while (pos < str.size() && count > 0) {
            if (str[pos] == openBracket) count++;
            else if (str[pos] == closeBracket) count--;
            pos++;
        }
        
        return pos - 1;
    }

    // Parse JSON object string and extract key-value pairs
    // Handles nested structures and arrays within the object
    void Config::parseJSONLine(const std::string& line) {
        // Simple JSON object parser for key-value pairs
        // Format: { "key1": value1, "key2": [1, 2, 3], "key3": { "nested": "value" } }
        
        size_t braceStart = line.find('{');
        size_t braceEnd = line.find('}');
        
        if (braceStart == std::string::npos || braceEnd == std::string::npos) {
            return;
        }
        
        std::string content = line.substr(braceStart + 1, braceEnd - braceStart - 1);
        size_t pos = 0;
        
        while (pos < content.size()) {
            // Find key - must be quoted
            size_t keyStart = content.find('"');
            if (keyStart == std::string::npos) break;
            size_t keyEnd = content.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Find colon after key
            size_t colonPos = content.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            // Find value - skip whitespace and commas
            size_t valueStart = content.find_first_not_of(", \t", colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            // Handle arrays and objects (nested structures)
            if (content[valueStart] == '[' || content[valueStart] == '{') {
                char openBracket = content[valueStart];
                size_t valueEnd = findMatchingBracket(content, valueStart, openBracket);
                std::string valueStr = trim(content.substr(valueStart, valueEnd - valueStart + 1));
                set(key, parseJSONValue(valueStr), ConfigSource::File);
                pos = valueEnd + 1;
                continue;
            }
            
            // Handle simple values
            size_t valueEnd = content.find(',', valueStart);
            if (valueEnd == std::string::npos) {
                valueEnd = content.find('}', valueStart);
            }
            if (valueEnd == std::string::npos) {
                valueEnd = content.size();
            }
            
            std::string valueStr = trim(content.substr(valueStart, valueEnd - valueStart));
            set(key, parseJSONValue(valueStr), ConfigSource::File);
            
            pos = valueStart + 1;
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
        file << entry.key << " = ";
        
        // Convert ConfigValue to string
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
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
                file << arg;
            }
        }, entry.value);
        
        file << "\n";
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

// String value setter
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Integer value setter
void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Double value setter
void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Boolean value setter
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
