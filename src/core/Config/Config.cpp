#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>  // For std::setprecision

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // PHASE 2: Enhanced config system with JSON/YAML support
    // Try different file formats in order of preference
    
    // First, try JSON format (modern, widely supported)
    if (loadFromJSON(filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format (backward compatibility)
    if (loadFromSimpleFormat(filepath)) {
        return true;
    }
    
    return false;
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

// Helper method for simple key=value format (backward compatibility)
bool Config::loadFromSimpleFormat(const std::string& filepath) {
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

// Helper method for JSON format (modern config files)
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple JSON parser for NLM configuration
    // Supports: {"key": "value", "number": 42, "boolean": true, "array": [1,2,3]}
    // This is a simplified implementation suitable for NLM's specific needs
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Remove whitespace and newlines for easier parsing
    std::string cleaned;
    for (char c : content) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            cleaned += c;
        }
    }
    
    if (cleaned.empty() || cleaned[0] != '{') {
        return false; // Not a valid JSON object
    }
    
    // Parse key-value pairs in JSON format
    size_t pos = 1; // Start after opening brace
    while (pos < cleaned.size() && cleaned[pos] != '}') {
        // Find next key
        size_t keyStart = cleaned.find('"', pos);
        if (keyStart == std::string::npos) {
            break; // Malformed JSON
        }
        
        size_t keyEnd = cleaned.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) {
            break; // Malformed JSON
        }
        
        std::string key = cleaned.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Find colon separator
        size_t colonPos = cleaned.find(':', keyEnd);
        if (colonPos == std::string::npos) {
            break; // Malformed JSON
        }
        
        // Skip whitespace after colon
        size_t valueStart = colonPos + 1;
        while (valueStart < cleaned.size() && (cleaned[valueStart] == ' ' || cleaned[valueStart] == '\t' || cleaned[valueStart] tournament_)) {
            valueStart++;
        }
        
        // Parse value based on its type
        if (valueStart < cleaned.size() && cleaned[valueStart] == '"') {
            // String value
            size_t valueEnd = cleaned.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) {
                break; // Malformed JSON
            }
            std::string value = cleaned.substr(valueStart + 1, valueEnd - valueStart - 1);
            set(key, value, ConfigSource::File);
            pos = valueEnd + 1;
        } else if (valueStart < cleaned.size() && std::isdigit(static_cast<unsigned char>(cleaned[valueStart]))) {
            // Numeric value (integer or float)
            size_t valueEnd = valueStart;
            while (valueEnd < cleaned.size() && (std::isdigit(static_cast<unsigned char>(cleaned[valueEnd])) || cleaned[valueEnd] == '.' || cleaned[valueEnd] == '-')) {
                valueEnd++;
            }
            std::string valueStr = cleaned.substr(valueStart, valueEnd - valueStart);
            
            // Try to parse as double first, then check if it's an integer
            try {
                double doubleValue = std::stod(valueStr);
                // Check if it's close to an integer
                if (std::abs(doubleValue - std::round(doubleValue)) < 1e-6) {
                    set(key, static_cast<int64_t>(std::round(doubleValue)), ConfigSource::File);
                } else {
                    set(key, doubleValue, ConfigSource::File);
                }
            } catch (...) {
                set(key, valueStr, ConfigSource::File);
            }
            
            pos = valueEnd;
        } else if (valueStart < cleaned.size() && cleaned[valueStart] == 't' && cleaned.find("true", valueStart) == valueStart) {
            // Boolean value "true"
            set(key, true, ConfigSource::File);
            pos = valueStart + 4;
        } else if (valueStart < cleaned.size() && cleaned[valueStart] == 'f' && cleaned.find("false", valueStart) == valueStart) {
            // Boolean value "false"
            set(key, false, ConfigSource::File);
            pos = valueStart + 5;
        } else if (valueStart < cleaned.size() && cleaned[valueStart] == '[') {
            // Array value - parse as comma-separated values
            size_t arrayEnd = cleaned.find(']', valueStart);
            if (arrayEnd == std::string::npos) {
                break; // Malformed JSON
            }
            
            std::string arrayContent = cleaned.substr(valueStart + 1, arrayEnd - valueStart - 1);
            std::vector<double> arrayValues;
            
            size_t arrayPos = 0;
            while (arrayPos < arrayContent.size()) {
                size_t commaPos = arrayContent.find(',', arrayPos);
                size_t nextPos = commaPos == std::string::npos ? arrayContent.size() : commaPos;
                
                std::string valueStr = arrayContent.substr(arrayPos, nextPos - arrayPos);
                try {
                    double value = std::stod(valueStr);
                    arrayValues.push_back(value);
                } catch (...) {
                    // Skip malformed array elements
                }
                
                arrayPos = nextPos + 1;
                // Skip comma
                if (arrayPos < arrayContent.size() && arrayContent[arrayPos] == ',') {
                    arrayPos++;
                }
            }
            
            set(key, arrayValues, ConfigSource::File);
            pos = arrayEnd + 1;
        } else {
            // Unknown value type, skip
            break;
        }
        
        // Skip comma or whitespace
        while (pos < cleaned.size() && (cleaned[pos] == ',' || cleaned[pos] == ' ' || cleaned[pos] == '\t')) {
            pos++;
        }
        
        // Check for next key or end of object
        if (pos < cleaned.size() && cleaned[pos] == '"') {
            continue; // More key-value pairs
        } else {
            break; // End of object or malformed JSON
        }
    }
    
    return true;
}

// Save configuration to file (supports both formats)
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Use simple key=value format for compatibility
    // This is the most robust format for NLM
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Format value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::fixed << std::setprecision(6) << arg;
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
                    file << std::fixed << std::setprecision(6) << arg[i];
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
