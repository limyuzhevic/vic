#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 3: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    // Phase 2 (current): Enhanced simple parser with JSON support
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check file extension for JSON format
    std::string extension = ".";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos);
    }
    
    if (extension == ".json") {
        // Simple JSON parser for basic types
        return parseJsonFile(filepath);
    } else {
        // Original simple key=value parser for .cfg files
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
        
        // Serialize the config value based on its type
        std::visit([&file](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            
            if constexpr (std::is_same_v<T, int>) {
                file << value;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << value;
            } else if constexpr (std::is_same_v<T, double>) {
                // Use fixed precision for floating point values
                file << std::fixed << std::setprecision(6) << value;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (value ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << value << "\"";
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
                    file << std::fixed << std::setprecision(6) << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << value[i] << "\"";
                }
                file << "]";
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

// Configuration validation implementations
bool Config::validateKey(const std::string& key) const {
    // Validate key format
    if (key.empty()) {
        return false;
    }
    
    // Check for valid characters (alphanumeric, underscore, dot, dash)
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '.' && c != '-') {
            return false;
        }
    }
    
    // Check for reserved key patterns
    if (key.find("nlm.") == 0) {
        // System keys starting with nlm. are reserved
        return false;
    }
    
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Basic validation based on key patterns
    std::string lowerKey = toLower(key);
    
    if (lowerKey.find("rate") != std::string::npos) {
        // Rate values should be positive numbers
        if (std::holds_alternative<double>(value)) {
            double rate = std::get<double>(value);
            if (rate < 0.0 || rate > 1000.0) {
                return false;
            }
        }
    } else if (lowerKey.find("count") != std::string::npos || 
               lowerKey.find("size") != std::string::npos ||
               lowerKey.find("neurons") != std::string::npos) {
        // Count/size values should be positive integers
        if (std::holds_alternative<int>(value)) {
            int count = std::get<int>(value);
            if (count <= 0 || count > 100000) {
                return false;
            }
        } else if (std::holds_alternative<int64_t>(value)) {
            int64_t count = std::get<int64_t>(value);
            if (count <= 0 || count > 100000) {
                return false;
            }
        }
    } else if (lowerKey.find("seed") != std::string::npos) {
        // Seed values should be non-negative
        if (std::holds_alternative<int>(value)) {
            int seed = std::get<int>(value);
            if (seed <= 0 || seed > 2147483647) {
                return false;
            }
        }
    } else if (lowerKey.find("temperature") != std::string::npos || 
               lowerKey.find("temp") != std::string::npos) {
        // Temperature values should be reasonable
        if (std::holds_alternative<double>(value)) {
            double temp = std::get<double>(value);
            if (temp < -273.15 || temp > 1000.0) {
                return false;
            }
        }
    }
    
    return true;
}

void Config::setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!validateKey(key)) {
        throw std::invalid_argument("Invalid configuration key: " + key);
    }
    
    if (!validateValue(key, value)) {
        throw std::invalid_argument("Invalid configuration value for key: " + key);
    }
    
    set(key, value, source);
}

// Simple JSON parser for .json files
bool Config::parseJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Simple JSON parsing for key-value pairs
    // This handles basic JSON objects with string keys and simple values
    size_t pos = 0;
    while (pos < content.length()) {
        // Skip whitespace
        while (pos < content.length() && std::isspace(content[pos])) {
            pos++;
        }
        
        if (pos >= content.length()) break;
        
        // Parse key (must be quoted string)
        if (content[pos] != '\"') {
            // Skip invalid JSON
            pos++;
            continue;
        }
        
        size_t keyStart = pos + 1;
        pos = content.find('\"', keyStart);
        if (pos == std::string::npos) break;
        
        std::string key = content.substr(keyStart, pos - keyStart);
        pos += 2; // Skip closing quote
        
        // Skip whitespace
        while (pos < content.length() && std::isspace(content[pos])) {
            pos++;
        }
        
        if (pos >= content.length() || content[pos] != ':') break;
        
        pos++; // Skip colon
        
        // Skip whitespace
        while (pos < content.length() && std::isspace(content[pos])) {
            pos++;
        }
        
        // Parse value
        ConfigValue value;
        if (pos < content.length() && content[pos] == '\"') {
            // String value
            size_t valueStart = pos + 1;
            pos = content.find('\"', valueStart);
            if (pos == std::string::npos) break;
            
            std::string strValue = content.substr(valueStart, pos - valueStart);
            value = strValue;
            pos += 2; // Skip closing quote
        } else if (pos < content.length() && content[pos] == 't') {
            // Boolean true
            if (content.substr(pos, 4) == "true") {
                value = true;
                pos += 4;
            } else {
                pos++;
                continue;
            }
        } else if (pos < content.length() && content[pos] == 'f') {
            // Boolean false
            if (content.substr(pos, 5) == "false") {
                value = false;
                pos += 5;
            } else {
                pos++;
                continue;
            }
            }
        } else if (pos < content.length() && (std::isdigit(content[pos]) || content[pos] == '-')) {
            // Number value
            size_t numEnd = pos;
            while (numEnd < content.length() && (std::isdigit(content[numEnd]) || content[numEnd] == '.' || content[numEnd] == 'e' || content[numEnd] == 'E' || content[numEnd] == '+')) {
                numEnd++;
            }
            
            std::string numStr = content.substr(pos, numEnd - pos);
            try {
                // Try to parse as integer first
                size_t endPos;
                int64_t intVal = std::stoll(numStr, &endPos);
                if (endPos == numStr.length()) {
                    value = intVal;
                } else {
                    // Parse as double
                    double doubleVal = std::stod(numStr);
                    value = doubleVal;
                }
            } catch (...) {
                pos++;
                continue;
            }
            
            pos = numEnd;
        } else if (pos < content.length() && content[pos] == '[') {
            // Array value - simple handling for now
            pos++; // Skip '['
            std::vector<double> arrayValue;
            while (pos < content.length() && content[pos] != ']') {
                // Skip whitespace
                while (pos < content.length() && std::isspace(content[pos])) {
                    pos++;
                }
                
                if (pos >= content.length() || content[pos] == ']') break;
                
                // Parse number
                size_t numEnd = pos;
                while (numEnd < content.length() && (std::isdigit(content[numEnd]) || content[numEnd] == '.' || content[numEnd] == 'e' || content[numEnd] == 'E' || content[numEnd] == '+')) {
                    numEnd++;
                }
                
                std::string numStr = content.substr(pos, numEnd - pos);
                try {
                    double arrayNum = std::stod(numStr);
                    arrayValue.push_back(arrayNum);
                } catch (...) {
                    // Skip invalid array element
                }
                
                pos = numEnd;
                
                // Skip whitespace and comma
                while (pos < content.length() && std::isspace(content[pos])) {
                    pos++;
                }
                if (pos < content.length() && content[pos] == ',') {
                    pos++;
                }
            }
            
            if (pos < content.length() && content[pos] == ']') {
                value = arrayValue;
                pos++;
            } else {
                pos++;
                continue;
            }
        } else {
            // Unknown value type, skip
            pos++;
            continue;
        }
        
        // Skip whitespace and comma
        while (pos < content.length() && std::isspace(content[pos])) {
            pos++;
        }
        if (pos < content.length() && content[pos] == ',') {
            pos++;
        }
        
        // Set the configuration value
        set(key, value, ConfigSource::File);
    }
    
    return true;
}
