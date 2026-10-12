#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <limits>
#include <typeinfo>
#include <unordered_map>

namespace nlm {

namespace ConfigValidation {
    bool validateType(const ConfigValue& value, const std::type_index& expectedType) {
        if (expectedType == typeid(int)) {
            return std::holds_alternative<int>(value);
        } else if (expectedType == typeid(int64_t)) {
            return std::holds_alternative<int64_t>(value);
        } else if (expectedType == typeid(double)) {
            return std::holds_alternative<double>(value);
        } else if (expectedType == typeid(bool)) {
            return std::holds_alternative<bool>(value);
        } else if (expectedType == typeid(std::string)) {
            return std::holds_alternative<std::string>(value);
        } else if (expectedType == typeid(std::vector<int>)) {
            return std::holds_alternative<std::vector<int>>(value);
        } else if (expectedType == typeid(std::vector<double>)) {
            return std::holds_alternative<std::vector<double>>(value);
        } else if (expectedType == typeid(std::vector<std::string>)) {
            return std::holds_alternative<std::vector<std::string>>(value);
        }
        return false;
    }

    bool validateString(const std::string& value, size_t minLength, size_t maxLength) {
        return value.length() >= minLength && value.length() <= maxLength;
    }

    bool validateNumber(const ConfigValue& value, double min, double max) {
        if (std::holds_alternative<int>(value)) {
            return static_cast<double>(std::get<int>(value)) >= min && 
                   static_cast<double>(std::get<int>(value)) <= max;
        } else if (std::holds_alternative<int64_t>(value)) {
            return static_cast<double>(std::get<int64_t>(value)) >= min && 
                   static_cast<double>(std::get<int64_t>(value)) <= max;
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) >= min && std::get<double>(value) <= max;
        }
        return false;
    }

    bool validateBoolean(const ConfigValue& value) {
        return std::holds_alternative<bool>(value);
    }

    bool validateArray(const ConfigValue& value, size_t minSize, size_t maxSize) {
        if (std::holds_alternative<std::vector<int>>(value)) {
            auto& arr = std::get<std::vector<int>>(value);
            return arr.size() >= minSize && arr.size() <= maxSize;
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            auto& arr = std::get<std::vector<double>>(value);
            return arr.size() >= minSize && arr.size() <= maxSize;
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            auto& arr = std::get<std::vector<std::string>>(value);
            return arr.size() >= minSize && arr.size() <= maxSize;
        }
        return false;
    }

    bool validatePath(const std::string& path) {
        return !path.empty() && path.find('\0') == std::string::npos &&
               (path.find("..") == std::string::npos || path.find("..") == path.length() - 3);
    }

    std::string sanitizeKey(const std::string& key) {
        std::string result;
        for (char c : key) {
            if (std::isalnum(c) || c == '_' || c == '-') {
                result += c;
            } else if (c == ' ') {
                result += '_';
            }
        }
        return result;
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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

// ConfigValidator implementation
ConfigValidationResult ConfigValidator::validateRequiredKeys(const Config& config,
    const std::vector<std::string>& requiredKeys) {
    ConfigValidationResult result;
    
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            result.addError("Missing required configuration key: '" + key + "'");
        }
    }
    
    return result;
}

ConfigValidationResult ConfigValidator::validateTypes(const Config& config,
    const std::unordered_map<std::string, std::type_index>& schema) {
    ConfigValidationResult result;
    
    for (const auto& entry : config.getEntries(ConfigSource::Default)) {
        auto it = schema.find(entry.key);
        if (it != schema.end()) {
            if (!ConfigValidation::validateType(entry.value, it->second)) {
                result.addError("Configuration key '" + entry.key + "' has invalid type. Expected: " +
                    (it->second == typeid(int) ? "int" :
                     it->second == typeid(int64_t) ? "int64_t" :
                     it->second == typeid(double) ? "double" :
                     it->second == typeid(bool) ? "bool" :
                     it->second == typeid(std::string) ? "string" :
                     it->second == typeid(std::vector<int>) ? "vector<int>" :
                     it->second == typeid(std::vector<double>) ? "vector<double>" :
                     it->second == typeid(std::vector<std::string>) ? "vector<string>" : "unknown"));
            }
        }
    }
    
    return result;
}

ConfigValidationResult ConfigValidator::validateConstraints(const Config& config,
    const std::unordered_map<std::string, std::function<bool(const ConfigValue&)>>& constraints) {
    ConfigValidationResult result;
    
    for (const auto& entry : config.getEntries(ConfigSource::Default)) {
        auto it = constraints.find(entry.key);
        if (it != constraints.end() && !it->second(entry.value)) {
            result.addError("Configuration key '" + entry.key + "' violates constraints");
        }
    }
    
    return result;
}

ConfigValidationResult ConfigValidator::validateRanges(const Config& config,
    const std::unordered_map<std::string, std::pair<double, double>>& ranges) {
    ConfigValidationResult result;
    
    for (const auto& entry : config.getEntries(ConfigSource::Default)) {
        auto it = ranges.find(entry.key);
        if (it != ranges.end()) {
            if (!ConfigValidation::validateNumber(entry.value, it->second.first, it->second.second)) {
                result.addError("Configuration key '" + entry.key + "' has value " + std::to_string(
                    std::holds_alternative<int>(entry.value) ? std::get<int>(entry.value) :
                    std::holds_alternative<int64_t>(entry.value) ? std::get<int64_t>(entry.value) :
                    std::holds_alternative<double>(entry.value) ? std::get<double>(entry.value) : 0) +
                    " outside valid range [" + std::to_string(it->second.first) + ", " +
                    std::to_string(it->second.second) + "]");
            }
        }
    }
    
    return result;
}

ConfigValidationResult ConfigValidator::validateDependencies(const Config& config,
    const std::unordered_map<std::string, std::vector<std::string>>& dependencies) {
    ConfigValidationResult result;
    
    for (const auto& entry : dependencies) {
        if (!config.has(entry.first)) {
            continue;
        }
        
        for (const auto& dep : entry.second) {
            if (!config.has(dep)) {
                result.addError("Configuration key '" + entry.first + "' depends on missing key: '" + dep + "'");
            }
        }
    }
    
    return result;
}

// Config::ConfigValidationResult implementation
ConfigValidationResult Config::validate(const std::vector<std::string>& requiredKeys) const {
    return ConfigValidator::validateRequiredKeys(*this, requiredKeys);
}

ConfigValidationResult Config::validateTypes(const std::unordered_map<std::string, std::type_index>& schema) const {
    return ConfigValidator::validateTypes(*this, schema);
}

ConfigValidationResult Config::validateConstraints(const std::unordered_map<std::string, std::function<bool(const ConfigValue&)>>& constraints) const {
    return ConfigValidator::validateConstraints(*this, constraints);
}

ConfigValidationResult Config::validate(const std::vector<std::string>& requiredKeys,
    const std::unordered_map<std::string, std::type_index>& schema,
    const std::unordered_map<std::string, std::function<bool(const ConfigValue&)>>& constraints) const {
    return ConfigValidator::validate(*this, requiredKeys, schema, constraints);
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.getEntries(ConfigSource::Default)) {
        set(entry.key, entry.value, ConfigSource::Runtime);
    }
}

std::vector<ConfigEntry> Config::getEntries(ConfigSource source) const {
    std::vector<ConfigEntry> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.push_back(entry);
        }
    }
    return result;
}

} // namespace nlm
