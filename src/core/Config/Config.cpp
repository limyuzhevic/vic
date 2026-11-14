// Config.cpp - Core implementation

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <cassert>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schemas;
    std::unordered_map<std::string, bool> flags;
    
    // Error tracking
    std::vector<std::string> errors;
    
    Impl() {
        // Initialize with common flags
        flags["validate_on_set"] = true;
        flags["save_on_change"] = false;
        flags["throw_on_error"] = false;
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first (simplified implementation)
    std::ifstream jsonFile(filepath);
    if (jsonFile.is_open()) {
        // Try to parse as JSON (simplified)
        std::string line;
        while (std::getline(jsonFile, line)) {
            line = trim(line);
            if (line.empty() || line.find("{*") == 0) {
                continue;
            }
            
            // Simple key=value parsing as fallback
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));\n                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "// NLM Configuration File" << std::endl;
    file << "// Generated at " << std::ctime(&std::time(nullptr));
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i) file << ", ";
                    file << "\"" << value[i] << "\"";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i) file << ", ";
                    file << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i) file << ", ";
                    file << value[i];
                }
                file << "]";
            } else {
                file << value;
            }
        }, entry.value);
        
        file << " (source: " << static_cast<int>(entry.source) << ")\n\n";
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (pImpl->flags["validate_on_set"]) {
        std::string error;
        if (!validate(key, error)) {
            if (pImpl->flags["throw_on_error"]) {
                throw std::runtime_error("Configuration validation failed: " + error);
            } else {
                pImpl->errors.push_back("Failed to set \"" + key + "\": " + error);
                return;
            }
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        std::string description = "Automatic entry";
        if (auto schema = getSchema(key)) {
            description = schema->description;
        }
        pImpl->entries.emplace_back(key, value, source, description);
    }
    
    if (pImpl->flags["save_on_change"]) {
        // Save to default location
        saveToFile("config_backup.cfg");
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

bool Config::validate(const std::string& key, std::string& errorMsg) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        errorMsg = "Key not found in configuration";
        return false;
    }
    
    const auto& schema = pImpl->schemas.find(key);
    if (schema != pImpl->schemas.end()) {
        const ConfigSchema& s = schema->second;
        std::string valueStr = toString(it->value);
        
        // Check required field
        if (s.required && (valueStr.empty() || valueStr == "0")) {
            errorMsg = "Required configuration key \"" + key + "\" cannot be empty";
            return false;
        }
        
        // Check value format (basic regex matching)
        if (!s.validationRegex.empty() && !matchesRegex(valueStr, s.validationRegex)) {
            errorMsg = "Configuration key \"" + key + "\" has invalid format";
            return false;
        }
    }
    
    return true;
}

bool Config::validateAll(std::vector<std::string>& errors) const {
    bool allValid = true;
    for (const auto& entry : pImpl->entries) {
        std::string error;
        if (!validate(entry.key, error)) {
            errors.push_back("Key \"" + entry.key + "\": " + error);
            allValid = false;
        }
    }
    return allValid;
}

std::string Config::getType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "unknown";
    }
    
    return getValueTypeName(it->value);
}

bool Config::isType(const std::string& key, const std::string& typeName) const {
    return getType(key) == typeName;
}

void Config::addSchema(const ConfigSchema& schema) {
    pImpl->schemas.push_back(schema);
}

const ConfigSchema* Config::getSchema(const std::string& key) const {
    for (const auto& schema : pImpl->schemas) {
        if (schema.name == key) {
            return &schema;
        }
    }
    return nullptr;
}

bool Config::hasSchema(const std::string& key) const {
    return getSchema(key) != nullptr;
}

void Config::printHelp() const {
    std::cout << "=== NLM Configuration Help ===" << std::endl;
    std::cout << "Available configuration keys:" << std::endl;
    
    const auto& impl = *pImpl;
    for (const auto& entry : impl.entries) {
        std::cout << "  " << entry.key << ": ";
        if (hasSchema(entry.key)) {
            std::cout << "[REQUIRED] ";
        }
        std::cout << " (type: " << getValueTypeName(entry.value) << ") ";
        std::cout << " (source: ";
        switch (entry.source) {
            case ConfigSource::Default:
                std::cout << "default";
                break;
            case ConfigSource::File:
                std::cout << "file";
                break;
            case ConfigSource::CommandLine:
                std::cout << "cli";
                break;
            case ConfigSource::Runtime:
                std::cout << "runtime";
                break;
        }
        std::cout << ") - " << entry.description << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  --key=value" << std::endl;
    std::cout << "  --config_file=/path/to/config.cfg" << std::endl;
    std::cout << "  --help (for this help)" << std::endl;
}

void Config::setFlag(const std::string& flag, bool value) {
    pImpl->flags[flag] = value;
}

bool Config::getFlag(const std::string& flag) const {
    auto it = pImpl->flags.find(flag);
    return it != pImpl->flags.end() ? it->second : false;
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
    // Also remove associated schema
    pImpl->schemas.erase(
        std::remove_if(pImpl->schemas.begin(), pImpl->schemas.end(),
            [&key](const ConfigSchema& s) { return s.name == key; }),
        pImpl->schemas.end()
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

std::vector<ConfigEntry> Config::getAllEntries() const {
    return pImpl->entries;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->schemas.clear();
    pImpl->flags.clear();
    pImpl->errors.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries, " << pImpl->errors.size() << " errors):\n";
    
    const auto& impl = *pImpl;
    for (const auto& entry : impl.entries) {
        oss << "  " << entry.key << " = " << toString(entry.value);
        
        if (entry.source == ConfigSource::Runtime) {
            oss << " [#runtime]";
        } else if (entry.source == ConfigSource::CommandLine) {
            oss << " [#cli]";
        } else {
            oss << " [#file]";
        }
        
        oss << " - " << entry.description << "\n";
        
        // Check if valid
        std::string error;
        if (!validate(entry.key, error)) {
            oss << "    [INVALID: " << error << "]\n";
        }
    }
    
    if (!impl.errors.empty()) {
        oss << "\nErrors:\n";
        for (const auto& error : impl.errors) {
            oss << "  " << error << "\n";
        }
    }
    
    return oss.str();
}

// Internal helper methods
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

bool Config::matchesRegex(const std::string& value, const std::string& regexPattern) {
    if (regexPattern.empty()) return true;
    try {
        std::regex pattern(regexPattern);
        return std::regex_match(value, pattern);
    } catch (const std::regex_error&) {
        return false;
    }
}

ConfigValue Config::convertToConfigValue(const std::string& strValue) {
    // Try different types in order
    if (strValue == "true" || strValue == "false") {
        return strValue == "true";
    }
    
    try {
        if (strValue.find('.') != std::string::npos) {
            return std::stod(strValue);
        }
        
        size_t pos;
        int64_t intVal = std::stoll(strValue, &pos);
        if (pos == strValue.length()) {
            return intVal;
        }
        
        double doubleVal = std::stod(strValue);
        return doubleVal;
    } catch (...) {
        // Not a number, return string
        return strValue;
    }
}

std::string Config::toString(const ConfigValue& value) {
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            os << val;
        } else if constexpr (std::is_same_v<T, int>) {
            os << val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            os << val;
        } else if constexpr (std::is_same_v<T, double>) {
            os << val;
        } else if constexpr (std::is_same_v<T, bool>) {
            os << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            os << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i) os << ", ";
                os << val[i];
            }
            os << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            os << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i) os << ", ";
                os << val[i];
            }
            os << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            os << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i) os << ", ";
                os << "\"" << val[i] << "\"";
            }
            os << "]";
        }
    }, value);
    
    return oss.str();
}

std::string Config::getValueTypeName(const ConfigValue& value) {
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            typeName = "string";
        } else if constexpr (std::is_same_v<T, int>) {
            typeName = "int";
        } else if constexpr (std::is_same_v<T, int64_t>) {
            typeName = "int64";
        } else if constexpr (std::is_same_v<T, double>) {
            typeName = "double";
        } else if constexpr (std::is_same_v<T, bool>) {
            typeName = "bool";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            typeName = "vector<int>";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            typeName = "vector<double>";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            typeName = "vector<string>";
        }
    }, value);
    
    return typeName;
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