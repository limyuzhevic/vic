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
    // Try to detect file format and parse accordingly
    // Supports both simple key=value format and JSON format
    
    // First, try to parse as JSON (simple implementation)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read the entire file to check if it's JSON
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    if (!content.empty() && content.front() == '{') {
        // Try JSON parsing
        return loadFromJSON(content);
    } else {
        // Fall back to simple key=value format
        return loadFromSimpleFormat(content);
    }
}

bool Config::loadFromJSON(const std::string& json) {
    // Simple JSON parser for basic structures
    // This is a minimal implementation that handles basic key-value pairs
    // In a production system, you'd use a proper JSON library like nlohmann/json
    
    size_t pos = 0;
    size_t braceCount = 0;
    bool inString = false;
    char quoteChar = '\0';
    std::string currentKey;
    std::string currentValue;
    
    while (pos < json.size()) {
        char c = json[pos];
        
        if (!inString) {
            if (c == '{') braceCount++;
            else if (c == '}') braceCount--;
            else if (c == '"') {
                inString = true;
                quoteChar = c;
            } else if (c == '}' && braceCount == 0) break;
        } else {
            if (c == quoteChar && (pos == 0 || json[pos - 1] != '\\')) {
                inString = false;
                quoteChar = '\0';
                
                if (!currentKey.empty()) {
                    // We've finished parsing a key-value pair
                    set(currentKey, currentValue, ConfigSource::File);
                    currentKey.clear();
                    currentValue.clear();
                }
            } else {
                // Collect key or value characters
                if (currentKey.empty() && c != ' ' && c != '\t' && c != '\n' && c != '\r') {
                    // We're reading a key
                    currentKey += c;
                } else if (!currentKey.empty() && (c == ':' || c == ' ' || c == '\t')) {
                    // Skip to value
                    continue;
                } else if (!currentKey.empty() && c == '"') {
                    // Start of quoted value
                    continue;
                } else if (!currentKey.empty() && c != ' ' && c != '\t' && c != '\n' && c != '\r' && 
                           c != ',' && c != '}') {
                    // We're reading value
                    currentValue += c;
                } else if (!currentKey.empty() && c == ',') {
                    // End of current pair, continue to next
                    set(currentKey, currentValue, ConfigSource::File);
                    currentKey.clear();
                    currentValue.clear();
                }
            }
        }
        
        pos++;
    }
    
    // Handle any remaining key-value pair
    if (!currentKey.empty()) {
        set(currentKey, currentValue, ConfigSource::File);
    }
    
    return !pImpl->entries.empty();
}

bool Config::loadFromSimpleFormat(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
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
            
            // Convert string to appropriate type
            setConfigValue(key, value, ConfigSource::File);
        }
    }
    
    return !pImpl->entries.empty();
}

void Config::setConfigValue(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Enhanced type detection and conversion
    // Try different types in order of specificity
    
    // Check if it's a boolean
    if (valueStr == "true" || valueStr == "false") {
        set(key, valueStr == "true", source);
        return;
    }
    
    // Check if it's a number (integer or float)
    try {
        if (valueStr.find('.') != std::string::npos) {
            // Float
            double floatVal = std::stod(valueStr);
            set(key, floatVal, source);
            return;
        } else {
            // Integer (try int64 first to handle larger numbers)
            int64_t intVal = std::stoll(valueStr);
            set(key, intVal, source);
            return;
        }
    } catch (const std::exception&) {
        // Not a number, fall through
    }
    
    // Try vector parsing (comma-separated)
    size_t commaPos = valueStr.find(',');
    if (commaPos != std::string::npos) {
        // Could be a vector, try to parse each element
        std::vector<std::string> elements;
        size_t start = 0;
        while (start < valueStr.size()) {
            size_t end = valueStr.find(',', start);
            if (end == std::string::npos) {
                end = valueStr.size();
            }
            std::string element = trim(valueStr.substr(start, end - start));
            if (!element.empty()) {
                elements.push_back(element);
            }
            start = end + 1;
        }
        
        if (!elements.empty()) {
            // Try to determine if it's vector<int>, vector<double>, or vector<string>
            bool allInts = true;
            bool allFloats = true;
            for (const auto& elem : elements) {
                try {
                    std::stoll(elem);
                } catch (...) {
                    allInts = false;
                }
                try {
                    std::stod(elem);
                } catch (...) {
                    allFloats = false;
                }
            }
            
            if (allInts) {
                std::vector<int> intVec;
                for (const auto& elem : elements) {
                    intVec.push_back(std::stoi(elem));
                }
                set(key, intVec, source);
                return;
            } else if (allFloats) {
                std::vector<double> floatVec;
                for (const auto& elem : elements) {
                    floatVec.push_back(std::stod(elem));
                }
                set(key, floatVec, source);
                return;
            } else {
                // All strings
                set(key, elements, source);
                return;
            }
        }
    }
    
    // Default to string
    set(key, valueStr, source);
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM " << "system\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << " (Source: " << static_cast<int>(entry.source) << ")\n";
        
        // Write the value in a format that preserves type information
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                  std::is_same_v<T, std::vector<double>> ||
                                  std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        file << "\"" << arg[i] << "\"";
                    } else {
                        file << arg[i];
                    }
                }
                file << "]\n\n";
            } else {
                file << entry.key << " = " << arg << "\n\n";
            }
        }, entry.value);
    }
    
    return true;
}

// Enhanced validation and utility methods
bool Config::validate(std::vector<std::string>& errors) const {
    errors.clear();
    
    // Check for duplicate keys
    std::vector<std::string> seenKeys;
    for (const auto& entry : pImpl->entries) {
        if (std::find(seenKeys.begin(), seenKeys.end(), entry.key) != seenKeys.end()) {
            errors.push_back("Duplicate configuration key: " + entry.key);
        }
        seenKeys.push_back(entry.key);
    }
    
    // Validate required keys
    const std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count", 
        "connection_probability",
        "simulation_timestep"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            errors.push_back("Missing required configuration key: " + key);
        }
    }
    
    // Validate value types
    for (const auto& entry : pImpl->entries) {
        std::string expectedType;
        if (std::holds_alternative<int>(entry.value)) {
            expectedType = "integer";
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            expectedType = "64-bit integer";
        } else if (std::holds_alternative<double>(entry.value)) {
            expectedType = "floating-point number";
        } else if (std::holds_alternative<bool>(entry.value)) {
            expectedType = "boolean";
        } else if (std::holds_alternative<std::string>(entry.value)) {
            expectedType = "string";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            expectedType = "integer array";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            expectedType = "float array";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            expectedType = "string array";
        }
        
        if (!expectedType.empty()) {
            // All types are valid, just record the type for debugging
            // In a more complex validation, you might check specific ranges or formats
        }
    }
    
    return errors.empty();
}

std::string Config::getFileFormatInfo() const {
    return "NLM Configuration Format\n" +
           "===========================\n\n" +
           "This file uses the following format:\n" +
           "  key = value\n\n" +
           "Where value can be:\n" +
           "  - String (quoted): \"value with spaces\"\n" +
           "  - Integer: 42\n" +
           "  - Floating-point: 3.14\n" +
           "  - Boolean: true or false\n" +
           "  - Array: [item1, item2, item3]\n\n" +
           "Arrays preserve their element types and can contain:\n" +
           "  - Integers: [1, 2, 3]\n" +
           "  - Floats: [1.1, 2.2, 3.3]\n" +
           "  - Strings: [\"hello\", \"world\"]\n\n" +
           "Configuration entries are ordered by their source priority:\n" +
           "  2. Command Line\n" +
           "  1. File\n" +
           "  0. Default (none)\n";
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Override current entries with values from other config
        // unless the current entry has higher priority
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != pImpl->entries.end()) {
            // Only override if the other source has higher priority
            if (static_cast<int>(entry.source) > static_cast<int>(it->source)) {
                *it = entry;
            }
        } else {
            pImpl->entries.push_back(entry);
        }
    }
}

Config Config::getSubset(const std::vector<std::string>& keys) const {
    Config subset;
    for (const auto& key : keys) {
        auto val = get<std::string>(key);
        if (val.has_value()) {
            subset.set(key, val.value(), ConfigSource::Runtime);
        }
    }
    return subset;
}

std::string Config::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        oss << "  \"" << entry.key << "\": ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_arithmetic_v<T>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j) oss << ", ";
                    oss << arg[j];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j) oss << ", ";
                    oss << arg[j];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j) oss << ", ";
                    oss << "\"" << arg[j] << "\"";
                }
                oss << "]";
            } else {
                oss << "null";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    
    oss << "}\n";
    return oss.str();
}

std::vector<Config> Config::partitionBySource() const {
    std::vector<Config> partitions(static_cast<int>(ConfigSource::Runtime) + 1);
    
    for (const auto& entry : pImpl->entries) {
        int sourceIndex = static_cast<int>(entry.source);
        if (sourceIndex >= 0 && sourceIndex <= static_cast<int>(ConfigSource::Runtime)) {
            partitions[sourceIndex].set(entry.key, entry.value, entry.source);
        }
    }
    
    return partitions;
}

void Config::setConfigValue(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Enhanced type detection and conversion
    // Try different types in order of specificity
    
    // Check if it's a boolean
    if (valueStr == "true" || valueStr == "false") {
        set(key, valueStr == "true", source);
        return;
    }
    
    // Check if it's a number (integer or float)
    try {
        if (valueStr.find('.') != std::string::npos) {
            // Float
            double floatVal = std::stod(valueStr);
            set(key, floatVal, source);
            return;
        } else {
            // Integer (try int64 first to handle larger numbers)
            int64_t intVal = std::stoll(valueStr);
            set(key, intVal, source);
            return;
        }
    } catch (const std::exception&) {
        // Not a number, fall through
    }
    
    // Try vector parsing (comma-separated)
    size_t commaPos = valueStr.find(',');
    if (commaPos != std::string::npos) {
        // Could be a vector, try to parse each element
        std::vector<std::string> elements;
        size_t start = 0;
        while (start < valueStr.size()) {
            size_t end = valueStr.find(',', start);
            if (end == std::string::npos) {
                end = valueStr.size();
            }
            std::string element = trim(valueStr.substr(start, end - start));
            if (!element.empty()) {
                elements.push_back(element);
            }
            start = end + 1;
        }
        
        if (!elements.empty()) {
            // Try to determine if it's vector<int>, vector<double>, or vector<string>
            bool allInts = true;
            bool allFloats = true;
            for (const auto& elem : elements) {
                try {
                    std::stoll(elem);
                } catch (...) {
                    allInts = false;
                }
                try {
                    std::stod(elem);
                } catch (...) {
                    allFloats = false;
                }
            }
            
            if (allInts) {
                std::vector<int> intVec;
                for (const auto& elem : elements) {
                    intVec.push_back(std::stoi(elem));
                }
                set(key, intVec, source);
                return;
            } else if (allFloats) {
                std::vector<double> floatVec;
                for (const auto& elem : elements) {
                    floatVec.push_back(std::stod(elem));
                }
                set(key, floatVec, source);
                return;
            } else {
                // All strings
                set(key, elements, source);
                return;
            }
        }
    }
    
    // Default to string
    set(key, valueStr, source);
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
