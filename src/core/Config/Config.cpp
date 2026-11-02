#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

// Include JSON library for modern config file support
// Simple JSON parsing utilities (since we can't rely on external dependencies)
namespace nlm {

// Simple JSON parsing utilities (since we can't rely on external dependencies)
namespace JsonUtils {
    
    // Parse a simple key=value format with support for JSON-like syntax
    bool parseKeyValue(const std::string& line, std::string& key, std::string& value) {
        size_t pos = line.find('=');
        if (pos == std::string::npos) return false;
        
        key = line.substr(0, pos);
        value = line.substr(pos + 1);
        
        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        
        // Remove quotes from value if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        return true;
    }
    
    // Convert string to appropriate type
    template<typename T>
    T convertString(const std::string& str);
    
    template<>
    std::string convertString<std::string>(const std::string& str) {
        return str;
    }
    
    template<>
    int convertString<int>(const std::string& str) {
        try {
            return std::stoi(str);
        } catch (const std::exception&) {
            throw std::runtime_error("Failed to convert string to int: " + str);
        }
    }
    
    template<>
    int64_t convertString<int64_t>(const std::string& str) {
        try {
            return std::stoll(str);
        } catch (const std::exception&) {
            throw std::runtime_error("Failed to convert string to int64_t: " + str);
        }
    }
    
    template<>
    double convertString<double>(const std::string& str) {
        try {
            return std::stod(str);
        } catch (const std::exception&) {
            throw std::runtime_error("Failed to convert string to double: " + str);
        }
    }
    
    template<>
    bool convertString<bool>(const std::string& str) {
        std::string lowerStr = str;
        std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
        
        if (lowerStr == "true" || lowerStr == "1" || lowerStr == "yes") return true;
        if (lowerStr == "false" || lowerStr == "0" || lowerStr == "no") return false;
        
        throw std::runtime_error("Failed to convert string to bool: " + str);
    }
    
    // Parse vector values (simple [] format)
    template<typename T>
    std::vector<T> parseVector(const std::string& str) {
        std::vector<T> result;
        if (str.empty() || (str.size() > 1 && str.front() == '[' && str.back() == ']')) {
            std::string content = str;
            if (content.size() > 2) content = content.substr(1, content.size() - 2);
            
            if (content.empty()) return result;
            
            size_t pos = 0;
            while (pos < content.size()) {
                size_t commaPos = content.find(',', pos);
                std::string element;
                if (commaPos == std::string::npos) {
                    element = content.substr(pos);
                    pos = content.size();
                } else {
                    element = content.substr(pos, commaPos - pos);
                    pos = commaPos + 1;
                }
                
                // Trim whitespace
                element.erase(0, element.find_first_not_of(" \t\r\n"));
                element.erase(element.find_last_not_of(" \t\r\n") + 1);
                
                if (!element.empty()) {
                    result.push_back(convertString<T>(element));
                }
            }
        }
        return result;
    }
} // namespace JsonUtils

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
    
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Remove inline comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
            line = trim(line);
            if (line.empty()) continue;
        }
        
        try {
            // Parse key=value pair
            std::string key, valueStr;
            if (!JsonUtils::parseKeyValue(line, key, valueStr)) {
                NLM_LOG_WARNING("Config: Invalid line " + std::to_string(lineNumber) + ": " + line);
                continue;
            }
            
            // Determine the appropriate type for the value
            ConfigValue configValue;
            
            // Try parsing as boolean first (true/false, 1/0, yes/no)
            try {
                bool boolVal = JsonUtils::convertString<bool>(valueStr);
                configValue = boolVal;
            } catch (const std::exception&) {
                // Not a boolean, try other types
                try {
                    int intVal = JsonUtils::convertString<int>(valueStr);
                    configValue = intVal;
                } catch (const std::exception&) {
                    try {
                        int64_t int64Val = JsonUtils::convertString<int64_t>(valueStr);
                        configValue = int64Val;
                    } catch (const std::exception&) {
                        try {
                            double doubleVal = JsonUtils::convertString<double>(valueStr);
                            configValue = doubleVal;
                        } catch (const std::exception&) {
                            // Try parsing as vector
                            try {
                                std::vector<int> intVec = JsonUtils::parseVector<int>(valueStr);
                                if (!intVec.empty()) {
                                    configValue = intVec;
                                } else {
                                    configValue = valueStr;  // Fall back to string
                                }
                            } catch (const std::exception&) {
                                try {
                                    std::vector<double> doubleVec = JsonUtils::parseVector<double>(valueStr);
                                    if (!doubleVec.empty()) {
                                        configValue = doubleVec;
                                    } else {
                                        configValue = valueStr;  // Fall back to string
                                    }
                                } catch (const std::exception&) {
                                    configValue = valueStr;  // Final fallback to string
                                }
                            }
                        }
                    }
                }
            }
            
            set(key, configValue, ConfigSource::File);
            
        } catch (const std::exception& e) {
            NLM_LOG_WARNING(std::string("Config: Error parsing line ") + std::to_string(lineNumber) + ": " + e.what());
            continue;
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
    
    // Write configuration in key=value format with descriptions
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Visit the value and write it in an appropriate format
        std::visit([&file](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                // Quote strings to preserve formatting
                file << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (value ? "true" : "false");
            } else if constexpr (std::is_arithmetic_v<T>) {
                file << value;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << value[i] << "\"";
                }
                file << "]";
            } else {
                // Default fallback for unknown types
                file << "UNKNOWN_TYPE";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
