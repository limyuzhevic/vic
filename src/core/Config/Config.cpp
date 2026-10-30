// Enhanced Config.cpp with JSON/YAML support and validation

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cassert>
#include <unordered_map>
#include <regex>

namespace nlm {

// Internal implementation details
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unique_ptr<ConfigSchema> schema;
    bool strictValidation = false;
    std::vector<ValidationError> validationErrors;
    std::vector<ValidationError> validationWarnings;
};

struct ConfigSchema::Impl {
    std::map<std::string, TypeConstraint> parameters;
    std::map<std::string, std::string> descriptions;
    std::map<std::string, bool> requiredFlags;
    std::map<std::string, std::string> defaultValues;
};

// JSON parsing helper functions
namespace {
    bool parseJsonValue(const std::string& str, ConfigValue& value);
    std::string serializeJsonValue(const ConfigValue& value);
}

// Simple JSON parser implementation (without external dependencies)
namespace {
    bool parseJsonValue(const std::string& str, ConfigValue& value) {
        // Remove whitespace
        std::string trimmed = str;
        trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
        trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
        
        // Check for different types
        if (trimmed.empty()) return false;
        
        if (trimmed == "true") {
            value = true;
            return true;
        } else if (trimmed == "false") {
            value = false;
            return true;
        } else if (trimmed.front() == '\"' && trimmed.back() == '\"') {
            // String
            std::string strValue = trimmed.substr(1, trimmed.size() - 2);
            // Unescape quotes
            size_t pos = 0;
            while ((pos = strValue.find("\\\"", pos)) != std::string::npos) {
                strValue.replace(pos, 2, "\"");
                pos += 1;
            }
            value = strValue;
            return true;
        } else if (trimmed.front() == '\'' && trimmed.back() == '\'') {
            // String with single quotes
            std::string strValue = trimmed.substr(1, trimmed.size() - 2);
            value = strValue;
            return true;
        } else if (std::isdigit(trimmed.front()) || (trimmed.front() == '-' && std::isdigit(trimmed[1]))) {
            // Try to parse as number
            try {
                if (trimmed.find('.') != std::string::npos) {
                    // Double
                    double doubleVal = std::stod(trimmed);
                    value = doubleVal;
                    return true;
                } else {
                    // Integer
                    int intVal = std::stoi(trimmed);
                    value = intVal;
                    return true;
                }
            } catch (...) {
                return false;
            }
        } else if (trimmed.front() == '[' && trimmed.back() == ']') {
            // Array
            std::string arrayContent = trimmed.substr(1, trimmed.size() - 2);
            std::vector<double> doubleArray;
            std::vector<std::string> stringArray;
            std::vector<int> intArray;
            
            // Parse array elements
            size_t start = 0;
            size_t end = 0;
            bool hasStrings = false;
            bool hasNumbers = false;
            
            while ((end = arrayContent.find(',', start)) != std::string::npos) {
                std::string element = arrayContent.substr(start, end - start);
                element.erase(0, element.find_first_not_of(" \t\n\r"));
                element.erase(element.find_last_not_of(" \t\n\r") + 1);
                
                if (element.empty()) continue;
                
                if (element.front() == '\"' || element.front() == '\'') {
                    hasStrings = true;
                    stringArray.push_back(element.substr(1, element.size() - 2));
                } else {
                    hasNumbers = true;
                    try {
                        if (element.find('.') != std::string::npos) {
                            doubleArray.push_back(std::stod(element));
                        } else {
                            intArray.push_back(std::stoi(element));
                        }
                    } catch (...) {
                        hasStrings = false;
                        hasNumbers = false;
                        break;
                    }
                }
                
                start = end + 1;
            }
            
            if (start < arrayContent.size()) {
                std::string element = arrayContent.substr(start);
                element.erase(0, element.find_first_not_of(" \t\n\r"));
                element.erase(element.find_last_not_of(" \t\n\r") + 1);
                
                if (element.empty()) continue;
                
                if (element.front() == '\"' || element.front() == '\'') {
                    hasStrings = true;
                    stringArray.push_back(element.substr(1, element.size() - 2));
                } else {
                    hasNumbers = true;
                    try {
                        if (element.find('.') != std::string::npos) {
                            doubleArray.push_back(std::stod(element));
                        } else {
                            intArray.push_back(std::stoi(element));
                        }
                    } catch (...) {
                        hasStrings = false;
                        hasNumbers = false;
                    }
                }
            }
            
            if (hasStrings && !hasNumbers) {
                value = stringArray;
                return true;
            } else if (hasNumbers && !hasStrings) {
                if (!intArray.empty()) {
                    value = intArray;
                    return true;
                } else if (!doubleArray.empty()) {
                    value = doubleArray;
                    return true;
                }
            }
            
            return false;
        }
        
        return false;
    }
    
    std::string serializeJsonValue(const ConfigValue& value) {
        std::ostringstream oss;
        
        if (std::holds_alternative<int>(value)) {
            oss << std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            oss << std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            oss << std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            oss << (std::get<bool>(value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(value)) {
            oss << "\"" << std::get<std::string>(value) << "\"";
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            const auto& vec = std::get<std::vector<int>>(value);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            const auto& vec = std::get<std::vector<double>>(value);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            const auto& vec = std::get<std::vector<std::string>>(value);
            oss << "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << vec[i] << "\"";
            }
            oss << "]";
        }
        
        return oss.str();
    }
}

// Config implementation
Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file type by extension
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    
    // Convert to lowercase for comparison
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".ini" || ext == ".cfg") {
        return loadFromIniFile(filepath);
    } else if (ext == ".json") {
        return loadFromJsonFile(filepath);
    } else {
        // Try INI first, then JSON
        if (loadFromIniFile(filepath)) return true;
        return loadFromJsonFile(filepath);
    }
}

bool Config::loadFromJson(const std::string& jsonStr) {
    try {
        // Simple JSON parsing for common formats
        if (jsonStr.empty() || jsonStr.front() != '{' || jsonStr.back() != '}') {
            return false;
        }
        
        std::string content = jsonStr.substr(1, jsonStr.size() - 2);
        std::vector<std::string> pairs;
        size_t start = 0;
        size_t end = 0;
        
        while ((end = content.find("\",", start)) != std::string::npos) {
            pairs.push_back(content.substr(start, end - start));
            start = end + 2;
        }
        if (start < content.size()) {
            pairs.push_back(content.substr(start));
        }
        
        pImpl->entries.clear();
        pImpl->validationErrors.clear();
        pImpl->validationWarnings.clear();
        
        for (const auto& pair : pairs) {
            size_t colonPos = pair.find(':');
            if (colonPos == std::string::npos) continue;
            
            std::string key = pair.substr(0, colonPos);
            key.erase(0, key.find_first_not_of(" \t\n\r\"'"));
            key.erase(key.find_last_not_of(" \t\n\r"));
            
            std::string valueStr = pair.substr(colonPos + 1);
            valueStr.erase(0, valueStr.find_first_not_of(" \t\n\r"));
            valueStr.erase(valueStr.find_last_not_of(" \t\n\r\"'"));
            
            ConfigValue value;
            if (parseJsonValue(valueStr, value)) {
                set(key, value, ConfigSource::File);
            }
        }
        
        return validate();
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    pImpl->validationErrors.clear();
    pImpl->validationWarnings.clear();
    
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
    
    return validate();
}

bool Config::saveToFile(const std::string& filepath) const {
    // Determine file type by extension
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    
    // Convert to lowercase for comparison
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save as JSON
    file << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) file << ",\n";
        file << "  \"" << entry.key << "\": " << serializeJsonValue(entry.value);
        first = false;
    }
    file << "\n}\n";
    
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
    
    // Clear validation caches
    pImpl->validationErrors.clear();
    pImpl->validationWarnings.clear();
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
