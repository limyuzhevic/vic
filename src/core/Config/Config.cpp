#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>  // For debugging

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schemas;
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
    
    // Try to detect file format
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    if (content.empty()) {
        return false;
    }
    
    // Detect format and parse accordingly
    if (content.find('{') != std::string::npos || content.find("[") != std::string::npos) {
        // Try JSON first
        if (loadFromJSON(content)) {
            return true;
        }
    }
    
    // Fallback to simple key=value format (original implementation)
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

bool Config::loadFromJSON(const std::string& jsonContent) {
    // Simple JSON parser - handle basic objects and key-value pairs
    // This is a simplified implementation for NLM
    
    // Remove all whitespace for parsing
    std::string content = jsonContent;
    content.erase(std::remove_if(content.begin(), content.end(), ::isspace), content.end());
    
    if (content.empty() || content[0] != '{') {
        return false; // Not a JSON object
    }
    
    // Simple parsing: {key:value,key2:value2}
    std::string::size_type pos = content.find('{') + 1;
    std::string::size_type endPos = content.rfind('}');
    
    if (pos == std::string::npos || endPos == std::string::npos || endPos <= pos) {
        return false;
    }
    
    std::string jsonData = content.substr(pos, endPos - pos);
    
    // Parse key-value pairs
    size_t start = 0;
    while (start < jsonData.length()) {
        size_t colonPos = jsonData.find(':', start);
        if (colonPos == std::string::npos) {
            return false; // Malformed JSON
        }
        
        std::string key = jsonData.substr(start, colonPos - start);
        key = trim(key);
        
        // Find value (simple string or number parsing)
        size_t commaOrEnd = jsonData.find(',', colonPos + 1);
        if (commaOrEnd == std::string::npos) {
            commaOrEnd = jsonData.length();
        }
        
        std::string valueStr = jsonData.substr(colonPos + 1, commaOrEnd - (colonPos + 1));
        valueStr = trim(valueStr);
        
        // Parse value based on content
        if (valueStr.empty()) {
            return false;
        }
        
        // Handle different value types
        if (valueStr.front() == '"' && valueStr.back() == '"') {
            // String value
            std::string value = valueStr.substr(1, valueStr.length() - 2);
            set(key, value, ConfigSource::File);
        } else if (valueStr == "true" || valueStr == "false") {
            // Boolean value
            set(key, valueStr == "true", ConfigSource::File);
        } else if (valueStr.find('.') != std::string::npos || 
                   valueStr.find('e') != std::string::npos ||
                   valueStr.find('E') != std::string::npos) {
            // Float value
            float floatValue = std::stof(valueStr);
            set(key, floatValue, ConfigSource::File);
        } else {
            // Integer value
            bool isNegative = false;
            if (valueStr[0] == '-') {
                isNegative = true;
                valueStr = valueStr.substr(1);
            }
            
            long long intValue = std::stoll(valueStr);
            if (isNegative) {
                intValue = -intValue;
            }
            
            set(key, intValue, ConfigSource::File);
        }
        
        start = commaOrEnd + 1; // Move to next key
        // Skip comma separator
        while (start < jsonData.length() && jsonData[start] == ',') {
            ++start;
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
        // Properly format the value based on its type
        std::visit([&file, &entry](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << value << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (value ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << value << "\n";
            } else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
                file << entry.key << " = " << value << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        file << "\"" << value[i] << "\"";
                    } else {
                        file << value[i];
                    }
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
            }
        }, entry.value);
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
        std, remove_if(pImpl->entries.begin(), pImpl->entries.end(),
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

// Validation methods
std::vector<ValidationError> Config::validate() const {
    std::vector<ValidationError> errors;
    
    for (const auto& entry : pImpl->entries) {
        bool isValid = true;
        
        // Type check
        if (!isTypeCompatible(entry.value, "generic")) {
            errors.push_back(ValidationError(
                entry.key,
                "Invalid type detected",
                "Value should be valid",
                getValueTypeName(entry.value)
            ));
        }
        
        // Check value ranges based on key
        if (entry.key == "neuron_count") {
            if constexpr (std::is_same<int, int64_t>::value) {
                if (auto val = std::get_optional<int64_t>(entry.value)) {
                    if (val > 1000) {
                        isValid = false;
                        errors.push_back(ValidationError(
                            entry.key,
                            "Neuron count too high (recommended max: 1000)",
                            "int64",
                            getValueTypeName(entry.value)
                        ));
                    }
                }
            }
        } else if (entry.key == "connection_probability") {
            if constexpr (std::is_same<float, double>::value) {
                if (auto val = std::get_optional<double>(entry.value)) {
                    if (val > 1.0f) {
                        isValid = false;
                        errors.push_back(ValidationError(
                            entry.key,
                            "Connection probability must be between 0 and 1",
                            "float [0,1]",
                            getValueTypeName(entry.value)
                        ));
                    } else if (val < 0.0f) {
                        isValid = false;
                        errors.push_back(ValidationError(
                            entry.key,
                            "Connection probability must be between 0 and 1",
                            "float [0,1]",
                            getValueTypeName(entry.value)
                        ));
                    }
                }
            }
        } else if (entry.key == "random_seed") {
            if constexpr (std::is_same<int64_t, int64_t>::value) {
                if (auto val = std::get_optional<int64_t>(entry.value)) {
                    if (val < 0) {
                        isValid = false;
                        errors.push_back(ValidationError(
                            entry.key,
                            "Random seed must be non-negative",
                            "int64 >= 0",
                            getValueTypeName(entry.value)
                        ));
                    }
                }
            }
        }
        
        if (!isValid) {
            errors.back().message += " - needs correction";
        }
    }
    
    return errors;
}

bool Config::validate(const std::vector<ConfigSchema>& schema) const {
    std::vector<ValidationError> errors = validate();
    
    // Additional schema validation
    for (const auto& schemaEntry : schema) {
        if (!has(schemaEntry.key)) {
            if (schemaEntry.required) {
                errors.push_back(ValidationError(
                    schemaEntry.key,
                    "Required configuration key is missing",
                    "present",
                    "missing"
                ));
            }
            continue;
        }
        
        // Check type compatibility
        auto value = getOptionalFromKey(schemaEntry.key);
        if (value.has_value()) {
            if (!isTypeCompatible(value.value(), schemaEntry.type)) {
                errors.push_back(ValidationError(
                    schemaEntry.key,
                    "Value type does not match schema",
                    schemaEntry.type,
                    getValueTypeName(value.value())
                ));
            }
            
            // Check range constraints
            if (schemaEntry.type == "int" || schemaEntry.type == "float" || 
                schemaEntry.type == "double" || schemaEntry.type == "int64") {
                double val = 0.0;
                if constexpr (std::is_same<int, int>::value) {
                    val = std::get<int>(value.value());
                } else if constexpr (std::is_same<double, double>::value) {
                    val = std::get<double>(value.value());
                }
                
                if (schemaEntry.minValue.has_value() && val < schemaEntry.minValue.value()) {
                    errors.push_back(ValidationError(
                        schemaEntry.key,
                        "Value below minimum allowed",
                        std::to_string(schemaEntry.minValue.value()) + "-inf",
                        std::to_string(val)
                    ));
                }
                
                if (schemaEntry.maxValue.has_value() && val > schemaEntry.maxValue.value()) {
                    errors.push_back(ValidationError(
                        schemaEntry.key,
                        "Value above maximum allowed",
                        "-inf-" + std::to_string(schemaEntry.maxValue.value()),
                        std::to_string(val)
                    ));
                }
            } else if (schemaEntry.type == "vector") {
                size_t size = 0;
                if constexpr (std::is_same<int, int>::value) {
                    size = std::get<std::vector<int>>(value.value()).size();
                } else if constexpr (std::is_same<double, double>::value) {
                    size = std::get<std::vector<double>>(value.value()).size();
                } else if constexpr (std::is_same<std::string, std::string>::value) {
                    size = std::get<std::vector<std::string>>(value.value()).size();
                }
                
                if (schemaEntry.minSize.has_value() && size < schemaEntry.minSize.value()) {
                    errors.push_back(ValidationError(
                        schemaEntry.key,
                        "Vector size below minimum allowed",
                        std::to_string(schemaEntry.minSize.value()) + "+",
                        std::to_string(size)
                    ));
                }
                
                if (schemaEntry.maxSize.has_value() && size > schemaEntry.maxSize.value()) {
                    errors.push_back(ValidationError(
                        schemaEntry.key,
                        "Vector size above maximum allowed",
                        "0-" + std::to_string(schemaEntry.maxSize.value()),
                        std::to_string(size)
                    ));
                }
            }
        }
    }
    
    return errors.empty();
}

bool Config::addSchema(const ConfigSchema& schema) {
    // Check if schema with this key already exists
    auto it = std::find_if(pImpl->schemas.begin(), pImpl->schemas.end(),
        [&schema](const ConfigSchema& s) { return s.key == schema.key; });
    
    if (it != pImpl->schemas.end()) {
        return false; // Schema already exists
    }
    
    pImpl->schemas.push_back(schema);
    return true;
}

std::vector<ConfigSchema> Config::getSchemas() const {
    return pImpl->schemas;
}

void Config::clearSchemas() {
    pImpl->schemas.clear();
}

// Private helper method
std::optional<ConfigValue> Config::getOptionalFromKey(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->value;
    }
    
    return std::nullopt;
}

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm