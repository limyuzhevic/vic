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
    // Try JSON parser first
    try {
        // Check if file has .json extension or starts with {
        if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json" || 
            filepath.find(".json") != std::string::npos) {
            return loadFromJSONFile(filepath);
        } else {
            // Try to parse as JSON by checking first character
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            char firstChar;
            file >> firstChar;
            file.seekg(0, std::ios::beg);
            
            if (firstChar == '{') {
                return loadFromJSONFile(filepath);
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_WARNING(std::string("JSON parser failed: ") + e.what() + std::string(", falling back to simple parser"));
    }
    
    // Fall back to simple key=value parser for backward compatibility
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
    // For now, implement a simple JSON parser that handles key-value pairs
    // In a full implementation, this would use a proper JSON library like nlohmann/json
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple JSON parsing for key-value pairs
    // This handles basic JSON format like {"key": "value", "number": 123}
    std::string content((std::istreambuf_iterator<char>(file)), 
                       std::istreambuf_iterator<char>());
    
    size_t pos = 0;
    while (pos < content.size()) {
        // Find next key (" or after { or after ,)
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) {
            break; // No more keys
        }
        
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) {
            break;
        }
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        key = trim(key);
        
        // Find next : and then value
        size_t colonPos = content.find(':', keyEnd);
        if (colonPos == std::string::npos) {
            break;
        }
        
        size_t valueStart = content.find_first_not_of(' ', colonPos + 1);
        if (valueStart == std::string::npos) {
            break;
        }
        
        // Find end of value (either next " for string or next , or })
        size_t valueEnd;
        char nextChar = content[valueStart];
        
        if (nextChar == '"') {
            // String value
            valueEnd = content.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) {
                break;
            }
            std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            set(key, value, ConfigSource::File);
            pos = valueEnd + 1;
        } else if (nextChar == '{' || nextChar == '[') {
            // Object or array - skip for now
            size_t bracketPos = content.find(nextChar == '{' ? '}' : ']', valueStart);
            if (bracketPos == std::string::npos) {
                break;
            }
            pos = bracketPos + 1;
        } else {
            // Numeric value
            valueEnd = content.find_first_of(',}', valueStart);
            if (valueEnd == std::string::npos) {
                break;
            }
            std::string value = content.substr(valueStart, valueEnd - valueStart);
            value = trim(value);
            
            // Try to parse as different types
            try {
                // Try integer
                size_t posInt;
                int intValue = std::stoi(value, &posInt);
                if (posInt == value.size()) {
                    set(key, intValue, ConfigSource::File);
                } else {
                    // Try float
                    size_t posFloat;
                    float floatValue = std::stof(value, &posFloat);
                    if (posFloat == value.size()) {
                        set(key, floatValue, ConfigSource::File);
                    } else {
                        // Try bool
                        if (value == "true" || value == "false") {
                            bool boolValue = (value == "true");
                            set(key, boolValue, ConfigSource::File);
                        } else {
                            // String
                            set(key, value, ConfigSource::File);
                        }
                    }
                }
            } catch (...) {
                // If parsing fails, treat as string
                set(key, value, ConfigSource::File);
            }
            
            pos = valueEnd + 1;
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
