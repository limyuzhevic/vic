#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <tuple>
#include <cstring>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

namespace {

// Helper function to detect file format by extension
inline bool isJsonFile(const std::string& filepath) {
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == "json";
}

inline bool isYamlFile(const std::string& filepath) {
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == "yaml" || ext == "yml";
}

// Simple JSON parser for key-value pairs
inline bool parseJsonFile(const std::string& filepath, std::map<std::string, std::string>& parsed) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Find the first colon for key-value pairs
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Remove quotes from value if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            parsed[key] = value;
        }
    }
    
    return true;
}

// Simple key=value parser (existing format)
inline bool parseKeyValueFile(const std::string& filepath, std::map<std::string, std::string>& parsed) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        // Skip comments and empty lines
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
            
            parsed[key] = value;
        }
    }
    
    return true;
}

} // namespace

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::map<std::string, std::string> parsed;
    
    if (isJsonFile(filepath)) {
        if (!parseJsonFile(filepath, parsed)) {
            return false;
        }
    } else if (isYamlFile(filepath)) {
        // TODO: Implement YAML parser when needed
        return false;
    } else {
        if (!parseKeyValueFile(filepath, parsed)) {
            return false;
        }
    }
    
    // Convert parsed values to appropriate types
    for (const auto& pair : parsed) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;
        
        // Try to detect type
        if (value == "true" || value == "false") {
            set(key, value == "true", ConfigSource::File);
        } else if (value.find('.') != std::string::npos) {
            try {
                double doubleVal = std::stod(value);
                set(key, doubleVal, ConfigSource::File);
            } catch (...) {
                set(key, value, ConfigSource::File);
            }
        } else {
            try {
                int intVal = std::stoi(value);
                set(key, intVal, ConfigSource::File);
            } catch (...) {
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
    
    // Determine if we should save in JSON or key=value format
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        file << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "    \"" << entry.key << "\": ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else {
                    file << arg;
                }
            }, entry.value);
            
            if (i < pImpl->entries.size() - 1) {
                file << ",\n";
            } else {
                file << "\n";
            }
        }
        file << "}\n";
    } else if (ext == "yaml" || ext == "yml") {
        // TODO: Implement YAML output when needed
        return false;
    } else {
        // Default to key=value format for .cfg and other extensions
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            file << "\n\n";
        }
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

// Implementation of validation and utility methods

bool Config::validate(const std::vector<ConfigSchemaField>& schema) const {
    std::string errors = validateAndGetErrors(schema);
    return errors.empty();
}

std::string Config::validateAndGetErrors(const std::vector<ConfigSchemaField>& schema) const {
    std::string errors;
    
    for (const auto& field : schema) {
        if (field.required && !has(field.key)) {
            if (!errors.empty()) errors += "\n";
            errors += "Missing required field: " + field.key;
            continue;
        }
        
        if (has(field.key)) {
            auto value = getAny(field.key);
            if (value) {
                if (!validateType(value.value(), field.type)) {
                    if (!errors.empty()) errors += "\n";
                    errors += "Field '" + field.key + "' has incorrect type. Expected: " + field.type;
                }
            }
        }
    }
    
    return errors;
}

template<typename T>
void Config::setValidated(const std::string& key, const T& value, ConfigSource source,
                         const std::string& type) {
    // Determine the actual type
    std::string actualType = type;
    if (actualType == "auto") {
        if constexpr (std::is_same_v<T, std::string>) actualType = "string";
        else if constexpr (std::is_same_v<T, int>) actualType = "int";
        else if constexpr (std::is_same_v<T, int64_t>) actualType = "int64_t";
        else if constexpr (std::is_same_v<T, double>) actualType = "double";
        else if constexpr (std::is_same_v<T, bool>) actualType = "bool";
        else if constexpr (std::is_same_v<T, std::vector<int>>) actualType = "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) actualType = "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) actualType = "vector<string>";
        else actualType = "string";
    }
    
    if (!validateType(std::any(value), actualType)) {
        throw ConfigValidationError("Invalid type for field '" + key + "'. Expected: " + actualType);
    }
    
    set(key, value, source);
}

template<typename T>
std::optional<T> Config::convertTo(const std::string& valueStr) {
    try {
        if constexpr (std::is_same_v<T, int>) {
            return std::stoi(valueStr);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::stoll(valueStr);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(valueStr);
        } else if constexpr (std::is_same_v<T, bool>) {
            std::string lower = toLower(valueStr);
            if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
                return true;
            } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
                return false;
            } else {
                throw std::invalid_argument("Invalid boolean value: " + valueStr);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            return valueStr;
        } else {
            return std::nullopt;
        }
    } catch (const std::exception& e) {
        return std::nullopt;
    }
}

// Helper method to get ConfigValue as any
ConfigValue Config::getAny(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigValue();
    }
    
    return it->value;
}

// Helper function to validate type against string specification
bool Config::validateType(const std::any& value, const std::string& typeStr) {
    if (typeStr == "auto") return true;
    
    const std::string type = toLower(typeStr);
    
    if (type == "string") {
        return value.type() == typeid(std::string);
    } else if (type == "int") {
        return value.type() == typeid(int) || value.type() == typeid(int64_t);
    } else if (type == "double") {
        return value.type() == typeid(double) || value.type() == typeid(int) || value.type() == typeid(int64_t);
    } else if (type == "bool") {
        return value.type() == typeid(bool);
    } else if (type == "vector<int>") {
        return value.type() == typeid(std::vector<int>);
    } else if (type == "vector<double>") {
        return value.type() == typeid(std::vector<double>);
    } else if (type == "vector<string>") {
        return value.type() == typeid(std::vector<std::string>);
    }
    
    return false;
}

// Helper function to convert any to string
std::string Config::anyToString(const std::any& value) {
    if (value.type() == typeid(int)) {
        return std::to_string(std::any_cast<int>(value));
    } else if (value.type() == typeid(int64_t)) {
        return std::to_string(std::any_cast<int64_t>(value));
    } else if (value.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(value));
    } else if (value.type() == typeid(bool)) {
        return std::any_cast<bool>(value) ? "true" : "false";
    } else if (value.type() == typeid(std::string)) {
        return std::any_cast<std::string>(value);
    } else if (value.type() == typeid(std::vector<int>)) {
        const auto& vec = std::any_cast<std::vector<int>>(value);
        std::string result = "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) result += ", ";
            result += std::to_string(vec[i]);
        }
        result += "]";
        return result;
    } else if (value.type() == typeid(std::vector<double>)) {
        const auto& vec = std::any_cast<std::vector<double>>(value);
        std::string result = "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) result += ", ";
            result += std::to_string(vec[i]);
        }
        result += "]";
        return result;
    } else if (value.type() == typeid(std::vector<std::string>)) {
        const auto& vec = std::any_cast<std::vector<std::string>>(value);
        std::string result = "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) result += ", ";
            result += "\"" + vec[i] + "\"";
        }
        result += "]";
        return result;
    }
    
    return "";
}

} // namespace nlm
